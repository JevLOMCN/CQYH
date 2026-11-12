#include "stdafx.h"
#include "message_handler.h"
#include "MMOFacade.h"
#include "MMOChatManager.h"

using namespace ExternalMessage;

extern bool bUseGMCmd;
extern UBOOL g_bGMBuff;
//----------------------------------------------------------
void CMessageHandler::Parse_m2c_chat_message(const char* buf, UINT32 size)
{
	M2C_CHAT_MESSAGE* msg =  (M2C_CHAT_MESSAGE*)buf;
	int ChannelId = GetTypeFromID ( msg->dwTargetID );
	int IndexId = msg->dwTargetID;
	int off = appStrlen( msg->szMessage )+1;
	//if( off > msg->wSize - sizeof( M2C_CHAT_MESSAGE ) )
	//	return;

	//if (msg->dwTargetID == MMOFacade::getSingleton().GetMainPlayer()->ServerID)
	//{
		//debugf(UTF8_TO_TCHAR(msg->szMessage));
	//}

	FString SenderName( UTF8_TO_TCHAR( msg->szMessage + off ));
	MMOChatManager::getSingleton().ReceiveMessage( msg->uLocation , ChannelId , IndexId , FString( UTF8_TO_TCHAR(msg->szMessage)) , SenderName, msg->dwSrcID , msg->dwTargetID ,msg->bGM);
}

//----------------------------------------------------------
void CMessageHandler::Parse_g2c_chat_message(const char* buf, UINT32 size)
{
	G2C_CHAT_MESSAGE* msg =  (G2C_CHAT_MESSAGE*)buf;
	int ChannelId = GetTypeFromID ( msg->dwTargetID );
	int IndexId = msg->dwTargetID ;
	int off = appStrlen( msg->szMessage )+1;


	FString SenderName( UTF8_TO_TCHAR( msg->szMessage + off ));
	MMOChatManager::getSingleton().ReceiveMessage( msg->uLocation , ChannelId , IndexId , FString( UTF8_TO_TCHAR(msg->szMessage)) , SenderName, msg->dwSrcID );
}

FString g_lastSendContent = TEXT("");

//------------------------------------------------------------
void CMessageHandler::Send_Chat( INT ChannelId ,  UINT32 TypeId  ,const FString& inContent   )
{
	C2M_CHAT_MESSAGE* msg = (C2M_CHAT_MESSAGE*)ms_MsgBuffer;
	
	if( inContent.IsEmpty() )
	{
		return;
	}

	FString Content = inContent;

	MMOUI::ReplaceCensoredWord(Content,CensoredWord_Chat);

	msg->wMsg = c2m_chat_message;
	msg->dwTargetID = TypeId;

	if(ChannelId == otRole) // Private chat, special
	{
		int beginPos = Content.InStr( TEXT("/"));

		if( beginPos >= 0 /*Content.Left(1) == TEXT("/")*/)
		{
			FString contentTemp =  Content.RightChop(beginPos);

			int Pos = contentTemp.InStr( TEXT(" "));
			if( Pos < 0)
			{
				MMOChatManager::getSingleton().AddScreenMessage();
				return;//Format error
			}
			FString leftContentString = Content.Left(beginPos);
			FString contentString = leftContentString + contentTemp.RightChop(Pos+1);
			FString targetName = contentTemp.Left(Pos).RightChop(1);
			if( /*contentString.IsEmpty() || */targetName.IsEmpty())
			{
				MMOChatManager::getSingleton().AddScreenMessage();
				return;//Format error
			}

			//Get id
			msg->dwTargetID = MMOFacade::getSingleton().GetPlayerBaseInfoByName(targetName);

			if (msg->dwTargetID == MMOFacade::getSingleton().GetMainPlayer()->ServerID)
				return;

			//MMOUI::ReplaceCensoredWord(contentString);
			Content = contentString;

			/*FTCHARToUTF8 szContent( *contentString);
			INT contentLen = appStrlen( szContent );
			if( contentLen > MAX_CHAT_MESSAGE_LEN )
			{
			contentLen = MAX_CHAT_MESSAGE_LEN;
			}

			FTCHARToUTF8 szName( *targetName);

			strxcpy( msg->szMessage , szContent , szContent.Length()+1 );
			strxcpy( msg->szMessage + szContent.Length() + 1 , szName , szName.Length() + 1 );
			msg->wSize = sizeof(C2G_CHAT_MESSAGE) + contentLen + szName.Length() + 2;*/
			//MMOChatManager::getSingleton().AddChatMessage( ECC_Private , contentString , targetName , -1 , TRUE );

		}
		else //Format error
		{
			MMOChatManager::getSingleton().AddScreenMessage();
			return;
		}
	}
	//else
	//{
	if(CheckIsDuplicateChatContent(Content))
	{
		return;
	}

	//MMOUI::ReplaceCensoredWord(Content,CensoredWord_Chat);
	INT contentLen = CopyUTF8(msg->szMessage,*Content,MAX_CHAT_MESSAGE_LEN);
	if(contentLen <= 0)
		return;
	msg->wSize = sizeof(C2M_CHAT_MESSAGE) + contentLen;
	//}


	_Send(msg, msg->wSize);
}

//------------------------------------------------------------
void CMessageHandler::Send_ChatChannel( INT ChannelId ,  UINT32 TypeId  , const FString& inContent ,UINT8 location)
{
	C2G_CHAT_MESSAGE* msg = (C2G_CHAT_MESSAGE*)ms_MsgBuffer;
	
	if( inContent.IsEmpty() )
	{
		return;
	}

	FString Content = inContent;

	MMOUI::ReplaceCensoredWord(Content,CensoredWord_Chat);


	msg->wMsg = c2g_chat_message;
	msg->dwTargetID = TypeId;//MakeObjectID(ChannelId  , TypeId);
	msg->byLocation = location;
/*
	FTCHARToUTF8 szContent( *Content);
	
	INT contentLen = appStrlen( szContent );
	if( contentLen > MAX_CHAT_MESSAGE_LEN )
	{
		contentLen = MAX_CHAT_MESSAGE_LEN;
	}
	msg->wSize = sizeof(C2G_CHAT_MESSAGE) + contentLen +1 ;
	strxcpy(msg->szMessage, szContent, MAX_CHAT_MESSAGE_LEN );
	*/
	if(CheckIsDuplicateChatContent(Content))
	{
		return;
	}

	INT contentLen = CopyUTF8(msg->szMessage,*Content,MAX_CHAT_MESSAGE_LEN);
	if(contentLen <= 0) 
		return;
	msg->wSize = sizeof(C2G_CHAT_MESSAGE) + contentLen ;

	_Send(msg, msg->wSize);
}

bool CMessageHandler::CheckIsDuplicateChatContent( FString &Content )
{
	if (bUseGMCmd || g_bGMBuff )
	{
		return false;
	}

	FString ContentTest = Content.Replace(TEXT(" "), TEXT(""));
	if(ContentTest == g_lastSendContent)
	{
		FString strScreen = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Chat.CanNotSendDuplicate"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(strScreen ,MsgInfoType_Table,TEXT("MMOGame.Chat.CanNotSendDuplicate"));
		return true;
	}

	g_lastSendContent = ContentTest;
	return false;
}