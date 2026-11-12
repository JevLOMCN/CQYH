#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOChatManager.h"

IMPLEMENT_CLASS(UMMOUIChatOption)


void UMMOUIChatOption::PostOnShow( UBOOL v )
{
	Super::PostOnShow(v);
	//copy a set of channel info for modification and restoration
	if(v)
	{
		MMOChatManager::getSingleton().ReCopyChatFilter();

		UpdateChannelButton();

		if( ChannelList.Num() > 0 && ChannelList(0))
		{
			GFXPlayer->eventCallWidgetDispatchEvent( ChannelList(0) , "click" );
		}
	}


}

void UMMOUIChatOption::UpdateChannelButton()
{
	TArray<ChatFilterChannelMessage>& FilterChannelList = MMOChatManager::getSingleton().GetCopyChatFilterChannelList();

	for ( INT i = 0 ; i < ChannelList.Num() ; i++ )
	{
		if(ChannelList(i))
		{
			if( FilterChannelList.Num() > i )
			{
				ChannelList(i)->SetString( TEXT("label") , FilterChannelList(i).ChannelName );
				ChannelList(i)->SetVisible(TRUE);
			}
			else
			{
				ChannelList(i)->SetVisible(FALSE);
			}
		}
	}
}

void UMMOUIChatOption::ChannelNameChanged(const FString& newName)
{
	FString name = newName;
	if(name.IsEmpty())
	{
		//empty not allowed
		return;
	}
	MMOUI::ReplaceCensoredWord(name);
	TArray<ChatFilterChannelMessage>& filterChannelList = MMOChatManager::getSingleton().GetCopyChatFilterChannelList();
	filterChannelList(CurrentChannel).ChannelName = name;
	UpdateChannelButton();
}

void UMMOUIChatOption::ClickFilter(INT id,UBOOL sel)
{
	TArray<ChatFilterChannelMessage>& filterChannelList = MMOChatManager::getSingleton().GetCopyChatFilterChannelList();

	if( sel )
	{
		filterChannelList(CurrentChannel).ChannelFilterType |= ( 0x1<<id );
	}
	else
	{
		filterChannelList(CurrentChannel).ChannelFilterType &= ~( 0x1<<id );
	}
	
}

void UMMOUIChatOption::ClickChannel( int index )
{
	TArray<ChatFilterChannelMessage>& filterChannelList = MMOChatManager::getSingleton().GetCopyChatFilterChannelList();
	
	if( filterChannelList.Num() <= index )
	{
		return;
	}

	ChatFilterChannelMessage& ChatFilterChannel = filterChannelList(index);

	if(ChannelNameInput)
	{
		ChannelNameInput->SetText( ChatFilterChannel.ChannelName );
	}
	for ( INT i = 0 ; i < FilterList.Num() ; i++ )
	{
		if( ( ChatFilterChannel.ChannelFilterType | (0x1<<i) ) == ChatFilterChannel.ChannelFilterType )
		{
			FilterList(i)->SetBool( "selected" , TRUE );
		}
		else
		{
			FilterList(i)->SetBool( "selected" , FALSE );
		}

		if( ChatFilterChannel.IsDefaultChannel )
		{
			FilterList(i)->SetBool( "enabled" , FALSE );
		}
		else
		{
			FilterList(i)->SetBool( "enabled" , TRUE );
		}
	}

	CurrentChannel = index;
}

void UMMOUIChatOption::Init()
{
	//this set of constants has two groups... not great
	ChatChannelStr.AddZeroed( ECC_Num );	
	ChatChannelStr(ECC_World) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.World") );
	ChatChannelStr(ECC_Team) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Team") );
	ChatChannelStr(ECC_Guild) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Guild") );
	ChatChannelStr(ECC_Nearby) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Nearby") );
	ChatChannelStr(ECC_Private) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Private") );
	ChatChannelStr(ECC_System) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.System") );
	ChatChannelStr(ECC_Hint) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Hint") );
	ChatChannelStr(ECC_Trumpet) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Trumpet") );

	ChannelList.AddZeroed( UCONST_MaxChatChannelNum );

	for( INT i = 0 ; i < UCONST_MaxChatChannelNum ; i++ )
	{
		FString ctrlName = TEXT("btnChannelText");
		ctrlName += appItoa(i);
		ChannelList(i) = GetObject(ctrlName);
	}

	FilterList.AddZeroed(UCONST_MaxChatFilterNum);

	for( INT i = 0 ; i < UCONST_MaxChatFilterNum ; i++ )
	{
		FString ctrlName = TEXT("ChatFilter");
		ctrlName += appItoa(i);
		FilterList(i) = GetObject(ctrlName);
	}

	ChannelNameInput = GetObject("channelNameInput");
	CurrentChannel = 0;

	for ( INT i = 0 ; i < FilterList.Num() ; i++ )
	{
		if(	FilterList(i) )
		{
			if( i < ECC_Num )
			{	
				if (i == ECC_Trumpet)
				{
					FilterList(i)->SetVisible(FALSE);
				}
				else
				{
					FilterList(i)->SetVisible(TRUE);
					FilterList(i)->SetString( TEXT("label") , ChatChannelStr(i) );
				}
				
			}
			else
			{
				FilterList(i)->SetVisible(FALSE);
			}

		}
	}

	if( GFXPlayer )
	{
		UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );

		if(data)
		{
			data->SetString( TEXT("panelName") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.ChatOption.ChatOption") ) );
			data->SetString( TEXT("button0") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.ChatOption.AddChannel") ) );
			data->SetString( TEXT("button1") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.ChatOption.DelChannel") ) );
			data->SetString( TEXT("button2") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.ChatOption.Reset") ) );
			data->SetString( TEXT("button3") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.ChatOption.Apply") ) );
			
			eventCallSetPanelText(data);
		}
		
	}

}

void UMMOUIChatOption::ClickReset()
{
	//MMOChatManager::getSingleton().ReCopyChatFilter();
	MMOChatManager::getSingleton().ReCopyDefaultChatFilter();
	UpdateChannelButton();
	if( ChannelList.Num() > 0 && ChannelList(0))
	{
		GFXPlayer->eventCallWidgetDispatchEvent( ChannelList(0) , "click" );
	}
}

void UMMOUIChatOption::ClickApply()
{
	MMOChatManager::getSingleton().ApplyCopyChatFilter();
	//save
	MMOChatManager::getSingleton().SaveRecord();

	MMOChatManager::getSingleton().NotifyUpdateChannelTabClicked();
	
	//UpdateChannelButton();
	//if( ChannelList.Num() > 0 && ChannelList(0))
	//{
	//	GFXPlayer->eventCallWidgetDispatchEvent( ChannelList(0) , "click" );
	//}
}

void UMMOUIChatOption::ClickAddChannel()
{
	MMOChatManager::getSingleton().AddNewFilterChannelCopy();

	UpdateChannelButton();

	MMOChatManager::getSingleton().SetLastChannelTab(TRUE);
}

void UMMOUIChatOption::ClickDelChannel()
{
	MMOChatManager::getSingleton().DelFilterChannelCopy(CurrentChannel);
	if( ChannelList.Num() > 0 && ChannelList(0))
	{
		GFXPlayer->eventCallWidgetDispatchEvent( ChannelList(0) , "click" );
	}

	UpdateChannelButton();
}

void UMMOUIChatOption::UnitWidget()
{
	Show(DefaultVisible);
}

void UMMOUIChatOption:: ASChatClickChatOption()
{

	ToogleVisible();

}