#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOGuildManager.h"


IMPLEMENT_CLASS(UMMOGFXPlayer_GuildBase)

void UMMOGFXPlayer_GuildBase::PostAdvance(FLOAT dt)
{
	if (_GuildCreatePanel->IsVisible)
	{
		AMMOActor* selectActor = Cast<AMMOActor>(MMOFacade::getSingleton().GetMainController()->GetSelectTarget());
		if (!selectActor)
		{
			_GuildCreatePanel->Show(FALSE);
		}
	}
	
	if (_GuildViewPanel->IsVisible && _GuildViewPanel->_IsNpcCalled)
	{
		AMMOActor* selectActor = Cast<AMMOActor>(MMOFacade::getSingleton().GetMainController()->GetSelectTarget());
		if (!selectActor)
		{
			_GuildViewPanel->Show(FALSE);
		}
	}
	
	if (_GuildWarehouse->IsVisible)
	{
		AMMOActor* selectActor = Cast<AMMOActor>(MMOFacade::getSingleton().GetMainController()->GetSelectTarget());
		if (!selectActor)
		{
			_GuildWarehouse->Show(FALSE);
		}
	}

	if (_GuildInstructionPanel->IsVisible)
	{
		AMMOActor* selectActor = Cast<AMMOActor>(MMOFacade::getSingleton().GetMainController()->GetSelectTarget());
		if (!selectActor)
		{
			_GuildInstructionPanel->Show(FALSE);
		}
	}
}

void UMMOGFXPlayer_GuildBase::SetItemSlotData(class UGFxObject* mc,class UItem* tItem,BYTE SlotType,BYTE ContainerID/* =13 */,INT SlotID/* =-1 */,BYTE inISS/* =0 */)
{
	if (!mc)
	{
		return;
	}

	mc->SetBool("isSlotLocked", FALSE);
	mc->SetBool("isSlotUnusable", FALSE);

	//placed above the Loader
	if (ISS_ItemLocked == inISS || ISS_SlotLocked == inISS)
	{
		mc->SetBool("isSlotLocked", TRUE);
	}
	else if (ISS_Unusable == inISS)
	{
		mc->SetBool("isSlotUnusable", TRUE);
	}

	//directly replace the Loader
	if (ISS_SlotLocked == inISS)
	{
		eventCallSetItemData(mc, 0, LockedSlotIconPath, -1, SlotType, TEXT(""), ITEMCOLOR_INVALID, ContainerID, SlotID);
		return;
	}
	else if (NULL == tItem)
	{
		eventCallSetItemData(mc, 0, TEXT(""), -1, SlotType, TEXT(""), ITEMCOLOR_INVALID, ContainerID, SlotID);
	}
	else
	{
		//having iteminfo means a local product
		FClientItemInfo* itemInfo = MMOGuildData::getSingleton().GetWarehouseItem(_GuildWarehouse->_CurPage, SlotID);
		switch (tItem->ItemDuration)
		{
		case E_DURTYPE_STACK:
			{
				if (itemInfo)
				{
					//when setting up a stall, if the current item is already listed
					if (itemInfo->IsOnSale && ESPType_StallSetting == SlotType)
					{
						eventCallSetItemData(mc, itemInfo->OnSaleCount, MMOFacade::getSingleton().GetItemIconNamePersonalized(tItem), tItem->ItemId, SlotType, TEXT(""), tItem->ItemColor + 1, ContainerID, SlotID);
					}
					//soul dragged into the soul fusion screen
					else if (itemInfo->IsDragIntoOtherUI >= 0 && (ESPType_SoulEmbed == SlotType || ESPType_SoulLevelUp == SlotType || ESPType_SoulUnEmbed == SlotType))
					{
						eventCallSetItemData(mc, 1, MMOFacade::getSingleton().GetItemIconNamePersonalized(tItem), tItem->ItemId, SlotType, TEXT(""), tItem->ItemColor, ContainerID, SlotID);
					}
					//during soul fusion, other screens are not shown
					else if (itemInfo->IsDragIntoOtherUI >= 0 && itemInfo->m_nCurDur <= 0) 
					{
						if (itemInfo->IsDragIntoOtherUI == ESPType_StallSetting)
						{// when setting up a stall, icons in the bag are not hidden
							eventCallSetItemData(mc, itemInfo->m_nCurDur, MMOFacade::getSingleton().GetItemIconNamePersonalized(tItem), tItem->ItemId, SlotType, TEXT(""), tItem->ItemColor + 1, ContainerID, SlotID);
						}
						else
						{
							//show empty
							eventCallSetItemData(mc, 0, TEXT(""), -1, SlotType, TEXT(""), ITEMCOLOR_INVALID, ContainerID, SlotID);
						}
					}
					else
					{
						eventCallSetItemData(mc, itemInfo->m_nCurDur, MMOFacade::getSingleton().GetItemIconNamePersonalized(tItem), tItem->ItemId, SlotType, TEXT(""), tItem->ItemColor, ContainerID, SlotID);
					}
				}
				else
				{
					eventCallSetItemData(mc, tItem->DefaultDuration, MMOFacade::getSingleton().GetItemIconNamePersonalized(tItem), tItem->ItemId, SlotType, TEXT(""), tItem->ItemColor, ContainerID, SlotID);
				}
			}
			break;
		case E_DURTYPE_EQUIP:
		case E_DURTYPE_USE:
		case E_DURTYPE_NONE:
		case E_DURTYPE_DEPOSIT:
			eventCallSetItemData(mc, 1, MMOFacade::getSingleton().GetItemIconNamePersonalized(tItem), tItem->ItemId, SlotType, TEXT(""), tItem->ItemColor + 1, ContainerID, SlotID);
			break;
		}
	}
}

void UMMOGFXPlayer_GuildBase::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_GuildBase::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_GuildBase);

	MMOUI::GetUIManager()->SetShowGuildCreatePanelDelegate(FScriptDelegate(this, TEXT("ShowGuildCreatePanel")));
	MMOUI::GetUIManager()->SetShowGuildViewPanelDelegate(FScriptDelegate(this, TEXT("ShowGuildViewPanel")));

}

void UMMOGFXPlayer_GuildBase::UnInit()
{
	Super::UnInit();

	FScriptDelegate nullDelgate(EC_EventParm);

	MMOUI::GetUIManager()->SetShowGuildCreatePanelDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetShowGuildViewPanelDelegate(nullDelgate);

	MMOUI::GetUIManager()->SetFreshWarehouseDelegate( nullDelgate );
	MMOUI::GetUIManager()->SetUpdateAccessInfoDelegate( nullDelgate );
}

void UMMOGFXPlayer_GuildBase::ShowGuildCreatePanel()
{
	_GuildCreatePanel->ShowGuildCreatePanel();
}

void UMMOGFXPlayer_GuildBase::ShowGuildViewPanel()
{
	_GuildViewPanel->ShowGuildViewPanel();
}

//void UMMOGFXPlayer_GuildBase::ShowGuildMainPanel()
//{
//	_GuildMainPanel->ShowGuildMainPanel();
//}

// Create guild
void UMMOGFXPlayer_GuildBase::ASCreateGuild(const FString& guildName, const FString& guildAnnounce)
{
	_GuildCreatePanel->CreateGuild(guildName, guildAnnounce);
}

// View guild
void UMMOGFXPlayer_GuildBase::ASViewGuild()
{
	_GuildViewPanel->SetNpcCalled(FALSE);
	_GuildViewPanel->ShowGuildViewPanel();
	_GuildMainPanel->Show(FALSE);
}

void UMMOGFXPlayer_GuildBase::ASUpdateGuildViewInfo(INT id)
{
	_GuildViewPanel->UpdateGuildViewInfo(id);
	_GuildViewPanel->UpdateGuildBuildingList(id);
}

// Find guild
void UMMOGFXPlayer_GuildBase::ASFindGuild(UBOOL isId, const FString& content)
{
	_GuildViewPanel->FindGuild(isId, content);
}

// Turn page
void UMMOGFXPlayer_GuildBase::ASPageUp()
{
	_GuildViewPanel->PageUp();
}

void UMMOGFXPlayer_GuildBase::ASPageDown()
{
	_GuildViewPanel->PageDown();
}

// Warehouse
void UMMOGFXPlayer_GuildBase::ASGuildWarehouseMore()
{
	_GuildWarehouse->MoreInfo();
}

void UMMOGFXPlayer_GuildBase::ASGuildWarehouseUpdate()
{
	_GuildWarehouse->SendFreshWarehouse();
}

void UMMOGFXPlayer_GuildBase::ASGuildWarehouseTab(INT tabID)
{
	_GuildWarehouse->TabChange( tabID );
}

// Apply to join guild
void UMMOGFXPlayer_GuildBase::ASApplyForJoint(INT id)
{
	_GuildViewPanel->ApplyForJoint(id);
}

// Update application list
void UMMOGFXPlayer_GuildBase::ASUpdateGuildPendingMemberList()
{
	_GuildMainPanel->UpdateGuildPendingMemberList();
}

// Accept joining guild
void UMMOGFXPlayer_GuildBase::ASAgreeToJoin(INT id)
{
	_GuildMainPanel->AgreeToJoin(id);
}

// Reject joining guild
void UMMOGFXPlayer_GuildBase::ASRefuseToJoin(INT id)
{
	_GuildMainPanel->RefuseToJoin(id);
}

// fund donation
void UMMOGFXPlayer_GuildBase::ASContribute(INT type, INT count)
{
	if (count == 0)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Donation amount cannot be 0"),MsgInfoType_Table,TEXT("Donation amount cannot be 0"));
		return;
	}

	// determine whether funds have reached the limit
	UINT32 resMax = MMOGuildData::getSingleton().GetGuildTerritoryResMax(GUILD_ATTR_MONEY);
	UINT32 resCur = MMOGuildData::getSingleton().GetCurGuildTerritoryRes(GUILD_ATTR_MONEY);
	if (resCur >= resMax)
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MoneyStoreLimit"));		
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.MoneyStoreLimit"));
		return;
	}

	// #15422
	if (resCur + count > resMax)
	{
		FString text = TEXT("Donation amount too large, cannot donate funds");		
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.MoneyStoreLimit"));
		return;
	}

	CMessageHandler::GetInstance().Send_GuildContribute(GUILD_ATTR_MONEY, count);
}

// Change announcement
void UMMOGFXPlayer_GuildBase::ASChangeNotice(const FString& notice)
{
	_GuildMainPanel->ChangeGuildNotice(notice);
}

// Change declaration
void UMMOGFXPlayer_GuildBase::ASChangeAnnounce(const FString& announce)
{
}

// View more guild events
void UMMOGFXPlayer_GuildBase::ASViewMoreEvents()
{
	CMessageHandler::GetInstance().Send_ViewMoreEvents();
}

// Recruit members
void UMMOGFXPlayer_GuildBase::ASInviteMember(const FString& roleName)
{
	_GuildMainPanel->InviteMember(roleName);
}

void UMMOGFXPlayer_GuildBase::ASSortMember(INT type, UBOOL isUp)
{
	_GuildMainPanel->SortMember(type, isUp);
}

// Expel member
void UMMOGFXPlayer_GuildBase::ASExpelMember(INT id)
{
	_GuildMainPanel->ExpelMember(id);
}

// Change position
void UMMOGFXPlayer_GuildBase::ASChangePost(INT roleId, INT post)
{
	_GuildMainPanel->ChangePost(roleId, post);
}

// Whether to show online members
void UMMOGFXPlayer_GuildBase::ASShowOnlineMember(UBOOL isShow)
{
	_GuildMainPanel->ShowOnlineMember(isShow);
}

// Whether to show online members
void UMMOGFXPlayer_GuildBase::ASOnlineMemberNotify(UBOOL notify)
{
	MMOGuildManager::getSingleton().SetOnlineMemberNotify(notify);
}

// Guild leader abdication
void UMMOGFXPlayer_GuildBase::ASLeaderDemise(const FString& roleName)
{
	_GuildMainPanel->LeaderDemise(roleName);
}

// Guild disband
void UMMOGFXPlayer_GuildBase::ASDisbandGuild(const FString& text)
{
	_GuildMainPanel->DisbandGuild(text);
}

// Leave guild
void UMMOGFXPlayer_GuildBase::ASQuitGuild()
{
	_GuildMainPanel->QuitConfirm();
}

// Update guild permission display
void UMMOGFXPlayer_GuildBase::ASUpdateGuildRights(INT post)
{
	_GuildMainPanel->UpdateGuildRights(post + 1);
}

// Update warehouse permission display
void UMMOGFXPlayer_GuildBase::ASModifyGuildStorageRights(INT post, UGFxObject* rights)
{
	_GuildMainPanel->ModifyGuildStorageRights(post + 1, rights);
}

void UMMOGFXPlayer_GuildBase::ASUpdateGuildStorageRights(INT post)
{
	_GuildMainPanel->UpdateGuildStorageRights(post + 1);
}

// Salary distribution
void UMMOGFXPlayer_GuildBase::ASDistributeSalary(UGFxObject* percent)
{
	_GuildMainPanel->DistributeSalary(percent);
}

void UMMOGFXPlayer_GuildBase::ASApplyAlly(const FString& guildName, INT timeType)
{
	_GuildMainPanel->ApplyAlly(guildName, timeType);
}

void UMMOGFXPlayer_GuildBase::ASApplyEnemy(const FString& guildName, INT timeType)
{
	_GuildMainPanel->ApplyEnemy(guildName, timeType);
}

void UMMOGFXPlayer_GuildBase::ASAgreeAlly(INT id)
{
	_GuildMainPanel->AgreeAlly(id);
}

void UMMOGFXPlayer_GuildBase::ASRefuseAlly(INT id)
{
	_GuildMainPanel->RefuseAlly(id);
}

void UMMOGFXPlayer_GuildBase::ASFullMemberRightClick(INT posX, INT posY, INT roleId)
{
	_GuildMainPanel->FullMemberRightClick(posX, posY, roleId);
}

void UMMOGFXPlayer_GuildBase::ASForeignApplyDayType(int foreignType, int dayType)
{
	_GuildMainPanel->SetForeignApplyDayType(foreignType, dayType);
}

void UMMOGFXPlayer_GuildBase::ASChallenge(const FString& guildName)
{
	_GuildMainPanel->ChallengeConfirm(guildName);
}

void UMMOGFXPlayer_GuildBase::ASMatchChallenge(INT id)
{
	CMessageHandler::GetInstance().Send_MatchChallenge(id);
}

void UMMOGFXPlayer_GuildBase::ASChallengeConfirm(INT guildId)
{
}

// family matchmaking battle
void UMMOGFXPlayer_GuildBase::ASSearchGuild()
{
	CMessageHandler::GetInstance().Send_Search();
}

void UMMOGFXPlayer_GuildBase::ASInputError()
{		
	MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("May only contain English letters, Chinese characters and digits"), MsgInfoType_Table, TEXT(""));
}