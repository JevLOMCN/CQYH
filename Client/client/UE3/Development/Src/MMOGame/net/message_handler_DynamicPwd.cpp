#include "stdafx.h"
#include "string_def.h"
#include "message_handler.h"


void CMessageHandler::Send_DynamicPassword(const FString& pwd)
{
	C2M_DYNAMIC_AUTH msg = {};
	msg.wMsg =  c2m_dynamic_auth;

	strxcpy(msg.szDynamicPassword, *pwd, OA_DYNAMIC_PASSWORD_LEN);

	_Send(&msg, sizeof(msg));
}
