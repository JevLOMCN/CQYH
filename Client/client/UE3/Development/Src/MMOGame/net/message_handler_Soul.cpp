#include "stdafx.h"
#include "message_handler.h"
#include "UITextParser.h"
#include "MMOFacade.h"
#include "CommonGameEvents.h"

using namespace ExternalMessage;

/*
    struct G2C_UPGRADE_SOUL : MESSAGE_HEADER
    {
        UINT8       byResult;//0 failure, 1 success
    };

    struct G2C_ADD_SOUL : MESSAGE_HEADER
    {
        UINT8       byResult;//0 failure, 1 success
    };
*/

/*
struct C2G_UPGRADE_SOUL : MESSAGE_HEADER
    {
        UINT16 wTargetConSlot;
        UINT16 wTargetItemSlot;
        UINT8 bySrcCount;
        UINT16 wSrcConSlot[5];
        UINT16 wSrcItemSlot[5];
    };

struct C2G_ADD_SOUL: MESSAGE_HEADER
{
        UINT16 wEquipConSlot;
        UINT16 wEquipItemSlot;
        UINT8 bySocketIndex;
        UINT16 wSoulConSlot;
        UINT16 wSoulItemSlot;
 };
*/
//========================================================================================
void CMessageHandler::Parse_g2c_upgrade_soul(const char* buf, UINT32 size)
{
	G2C_UPGRADE_SOUL* msg = (G2C_UPGRADE_SOUL*)buf;
	Parse_g2c_upgrade_soul(msg->byResult);
}
//
void CMessageHandler::Parse_g2c_upgrade_soul(UINT8 byResult)
{
	MMOGameEvent_OneInt evt( GameEvent_SoulLevelUp, byResult );
	MMOUI::PostGameEvent(&evt);
}
//========================================================================================
void CMessageHandler::Parse_g2c_add_soul(const char* buf, UINT32 size)
{
	G2C_ADD_SOUL* msg = (G2C_ADD_SOUL*)buf;
	Parse_g2c_add_soul(msg->byResult);
}
//
void CMessageHandler::Parse_g2c_add_soul(UINT8 byResult)
{
	MMOGameEvent_OneInt evt( GameEvent_SoulEmbed, byResult );
	MMOUI::PostGameEvent(&evt);
}
//========================================================================================
void CMessageHandler::Parse_g2c_remove_soul(const char* buf, UINT32 size)
{
	G2C_REMOVE_SOUL* msg = (G2C_REMOVE_SOUL*)buf;
	Parse_g2c_remove_soul(msg->byResult);
}
//
void CMessageHandler::Parse_g2c_remove_soul(UINT8 byResult)
{
	MMOGameEvent_OneInt evt( GameEvent_SoulUnEmbed, byResult );
	MMOUI::PostGameEvent(&evt);
}
//========================================================================================
void CMessageHandler::Send_RemoveSoul( UINT wEquipConSlot, UINT wEquipItemSlot, UINT bySocketIndex)
{
	/*
	 struct C2G_REMOVE_SOUL : MESSAGE_HEADER
    {
        UINT16 wEquipConSlot;
        UINT16 wEquipItemSlot;
        UINT8 bySocketIndex;
    };
	*/
	C2G_REMOVE_SOUL msg;
	ZeroMemory(&msg, sizeof(msg));

	msg.wMsg = c2g_remove_soul;
	msg.byConSlot = wEquipConSlot;
	msg.byItemSlot = wEquipItemSlot;
	msg.bySocketIndex = bySocketIndex;

	_Send(&msg, sizeof(msg));

}
void CMessageHandler::Send_UpgradeSoul( UINT wTargetConSlot,UINT wTargetItemSlot, UINT bySrcCount, const STLIntVector &wSrcConSlot, const STLIntVector& wSrcItemSlot )
{
	C2G_UPGRADE_SOUL msg;
	ZeroMemory(&msg, sizeof(msg));

	if(bySrcCount < 0 || bySrcCount > 5)
	{
		return;
	}

	msg.wMsg = c2g_upgrade_soul;
	msg.byTargetConSlot = wTargetConSlot;
	msg.byTargetItemSlot = wTargetItemSlot;
	msg.bySrcCount = bySrcCount;

	for(UINT i = 0; i < bySrcCount; i++)
	{
		msg.bySrcConSlot[i] = wSrcConSlot[i];
		msg.bySrcItemSlot[i] = wSrcItemSlot[i];
	}

	_Send(&msg, sizeof(msg));
}
void CMessageHandler::Send_AddSoul( UINT wEquipConSlot, UINT wEquipItemSlot, UINT bySocketIndex, UINT wSoulConSlot, UINT wSoulItemSlot)
{
	C2G_ADD_SOUL msg;
	ZeroMemory(&msg, sizeof(msg));

	msg.wMsg = c2g_add_soul;
	msg.byEquipConSlot = wEquipConSlot;
	msg.byEquipItemSlot = wEquipItemSlot;
	msg.bySocketIndex = bySocketIndex;
	msg.bySoulConSlot = wSoulConSlot;
	msg.bySoulItemSlot = wSoulItemSlot;

	_Send(&msg, sizeof(msg));
}