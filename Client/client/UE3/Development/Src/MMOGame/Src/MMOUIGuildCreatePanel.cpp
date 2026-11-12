#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOGuildManager.h"
#include <time.h>
#include "UITextParser.h"

IMPLEMENT_CLASS(UMMOUIGuildCreatePanel)

void UMMOUIGuildCreatePanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Guild);
	evts.AddItem(GameEvent_UpdateMoney);	
	evts.AddItem(GameEvent_Item_Gain);
	evts.AddItem(GameEvent_Item_Delete);
	EventAdapter = new TGameEventAdapter<UMMOUIGuildCreatePanel>(this, evts);
}

void UMMOUIGuildCreatePanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if (v)
	{
		eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.CreatePanel")));
		UpdateGuildCreatePanel();
		eventCallSetFocusMc(TEXT("guild_create_name_mc"),TEXT("textField"),TEXT(""),TEXT(""));
	}
}

void UMMOUIGuildCreatePanel::OnNativeGameEvent(struct MMOGameEvent* gameEvent)
{
	if (!IsVisible)
	{
		return;
	}

	if (!gameEvent)
	{
		return;
	}

	switch (gameEvent->GetEventType())
	{
	case GameEvent_UpdateMoney:
	case GameEvent_Item_Gain:
	case GameEvent_Item_Delete:
		UpdateGuildCreatePanel();
		break;
	}
}

void UMMOUIGuildCreatePanel::ShowGuildCreatePanel()
{
	ChangeVisible();
	if (IsVisible)
	{
		eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.CreatePanel")));
		UpdateGuildCreatePanel();
	}
}

void UMMOUIGuildCreatePanel::UpdateGuildCreatePanel()
{
	if (IsVisible==FALSE)
	{
		return;
	}

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	// Gold required to create
	FString money = TEXT("");
	if (mainPlayer->GetMoney(MONEYTYPE_02) < CreateMoney)
	{
		money = FString::Printf(TEXT("<font color='#FF0000'>%s</font>"), *UITextParser::HTMLMoneyIconString(MONEYTYPE_02, CreateMoney));
	} 
	else
	{
		money = FString::Printf(TEXT("<font color='#FFFFFF'>%s</font>"), *UITextParser::HTMLMoneyIconString(MONEYTYPE_02, CreateMoney));
	}
	eventCallSetMoneyRequest(money);

	// Items required to create
	FString itemName = TEXT("");
	UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(CreateItemId);
	if (itemTemp)
	{
		itemName = FString::Printf(TEXT("<font color='#FF0000'>%s</font>"), *(itemTemp->GetDisplayName()));
		if (MMOFacade::getSingleton().GetItemCountInBag(CreateItemId, E_CONSLOT_DEFAULT) >= 1)
		{
			itemName = FString::Printf(TEXT("<font color='#FFFFFF'>%s</font>"), *(itemTemp->GetDisplayName()));
		}
		//itemName += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.CreateItem"));
		eventCallSetItemRequest(itemName, MMOFacade::getSingleton().GetItemIconNamePersonalized( itemTemp ), itemTemp->GetTemplateID());
	}
}

void UMMOUIGuildCreatePanel::CreateGuild(const FString& guildName,const FString& guildAnnounce)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	if (baseInfo.Id != 0)
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errGuildAlreadyJoin"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errGuildAlreadyJoin"));
		return;
	}

	UActorAttribute* attri = mainPlayer->ActorAttributes;
	if (attri && attri->LevelAttributes.Level < CreateLevel)
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errGuildLowerLevel"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errGuildLowerLevel"));
		return;
	}

	if (!mainPlayer->ItemManager)
	{
		return;
	}

	UItem* item = mainPlayer->ItemManager->GetItemById(CreateItemId, E_CONSLOT_DEFAULT);
	if (!item)
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errGuildNoItem"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errGuildNoItem"));
		return;
	}

	if(item->DefaultDuration < 1)
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errGuildNoItem"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errGuildNoItem"));
		return;
	}

	if (guildName.IsEmpty())
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.EnterName"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.EnterName"));
		return;
	}

	FTCHARToUTF8 szGuildName(*guildName);
	// Up to 8 Chinese characters (Length = 3 * 8 chars)
	if (szGuildName.Length() > 24)
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.LongName"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.LongName"));
		return;
	}
	
	if (MMOUI::HasSpecialSymbol(guildName))
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.SpecialSymbol"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.SpecialSymbol"));
		return;
	}

	if (MMOUI::HasCensoredWord(guildName))
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.SpecialName"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.SpecialName"));
		return;
	}

	if (guildAnnounce.IsEmpty())
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.EnterAnnounce"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.EnterAnnounce"));
		return;
	}

	FTCHARToUTF8 szGuildAnnounce(*guildAnnounce);
	// Up to 30 Chinese characters (Length = 3 * 30 chars)
	if (szGuildAnnounce.Length() > 90)
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.LongAnnounce"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.LongAnnounce"));
		return;
	}


	if (MMOUI::HasCensoredWord(guildAnnounce))
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.SpecialAnnounce"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.SpecialAnnounce"));
		return;
	}

	CMessageHandler::GetInstance().Send_CreateGuild(guildName, guildAnnounce); 
}

//////////////////////////////////////////////////////////////////////////

void UMMOUIGuildCreatePanel::ASCreateGuild(const FString& guildName, const FString& guildAnnounce)
{
	CreateGuild(guildName, guildAnnounce);
}

void UMMOUIGuildCreatePanel::ASInputError()
{		
	MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("May only contain English letters, Chinese characters and digits"), MsgInfoType_Table, TEXT(""));
}