#include "stdafx.h"
#include "message_handler.h"
#include "WebHtmlManager.h"
#include "MMOFacade.h"
#include "MMOChatManager.h"

void CMessageHandler::Send_WebSurveyPageAck()
{
	C2G_QUERY_SURVEY_PATH  msg;
	msg.wMsg = c2g_query_survey_path;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Parse_g2c_query_survey_path(const char* buf, UINT32 size)
{
	G2C_QUERY_SURVEY_PATH* msg = (G2C_QUERY_SURVEY_PATH*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	if(msg)
	{
		TArray<FString>  tArrUrlStr;
		for (INT n = 0; n < MAX_SURVEY_COUNT; ++n)
		{
			FString str = ReadFixedUTF8(msg->szQuerySurveyPath[n]);
			if(str.Len() > 0)
			    tArrUrlStr.AddItem(str);
			else
				break;
		}
		if(tArrUrlStr.Num() == 0)
		{
			FString strHint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.NowNoSurvey") );
			MMOChatManager::getSingleton().AddSystemMessage(strHint);
			MMOUI::GetUIManager()->delegateAddScreenMessage( strHint);
		}
		else
			WebHtmlManager::getSingleton().NotifyOpenSurveyPage(tArrUrlStr);
	}
}