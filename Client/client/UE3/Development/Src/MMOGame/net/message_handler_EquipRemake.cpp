#include "stdafx.h"
#include "message_handler.h"
#include "MMOFacade.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOEquipRemakeMgr.h"


void CMessageHandler::Send_Remake(UINT uGambleType,UINT uGambleLevel)
{
	C2G_GAMBLE_ADVANCE msg;
	msg.wMsg = c2g_gamble_advance;

	msg.nGambleType = uGambleType;
	msg.nGambleLevel = uGambleLevel;

	_Send(&msg,sizeof(msg));
}
void CMessageHandler::Parse_g2c_gamble(const char* buf, UINT32 size)
{
	if (!buf || size < sizeof(G2C_GAMBLE))
		return;
	G2C_GAMBLE* msg = (G2C_GAMBLE*)buf;
	if (msg->nResult == 1)
	{
		MMOEquipRemakeManager::getSingleton().OnRemakeResultSuccess(msg->nItemID);
	}
	else
	{
		MMOEquipRemakeManager::getSingleton().OnRemakeResultFailed();
	}
}

//==========================================================
void CMessageHandler::Send_RemakeBook()
{
	C2G_SPELL_BOOK_GAMBLE msg;
	msg.wMsg = c2g_spell_book_gamble;

	_Send(&msg,sizeof(msg));
}

void CMessageHandler::Parse_g2c_spell_book_gamble(const char* buf, UINT32 size)
{
	if (!buf || size < sizeof(G2C_SPELL_BOOK_GAMBLE))
		return;
	G2C_SPELL_BOOK_GAMBLE* msg = (G2C_SPELL_BOOK_GAMBLE*)buf;

	MMOGameEvent_RemakeResult remakeBookRst( GameEvent_BookRemakeResult, msg->nResult, msg->nItemID );
	MMOUI::PostGameEvent( &remakeBookRst );
}
//==========================================================
void CMessageHandler::Send_RemakeMedicine()
{
	C2G_POTION_GAMBLE msg;
	msg.wMsg = c2g_potion_gamble;
	msg.nGambleLevel = 0;

	_Send(&msg,sizeof(msg));
}

void CMessageHandler::Parse_g2c_potion_gamble(const char* buf, UINT32 size)
{
	if (!buf || size < sizeof(G2C_POTION_GAMBLE))
		return;
	G2C_POTION_GAMBLE* msg = (G2C_POTION_GAMBLE*)buf;

	MMOGameEvent_RemakeResult remakeMedicineRst( GameEvent_MedicineRemakeResult, msg->nResult, msg->nItemID );
	MMOUI::PostGameEvent( &remakeMedicineRst );
}