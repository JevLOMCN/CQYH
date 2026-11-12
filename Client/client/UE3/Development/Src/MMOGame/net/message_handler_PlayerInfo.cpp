#include "stdafx.h"
#include "message_handler.h"
#include "UITextParser.h"
#include "MMOFacade.h"
#include "CommonGameEvents.h"

#include "MMOPlayerInfoManager.h"
#include "MMOChatManager.h"

using namespace ExternalMessage;

void CMessageHandler::Send_QueryRoleDetailInfo( INT roleId , INT roleCRC , INT equipCRC , INT spellCRC , INT spiritCRC )
{
	C2G_QUERY_ROLE_DETAIL_DATA msg;
	

	msg.wMsg = c2g_query_role_detail_data;
	msg.dwAttrCRC = roleCRC;
	msg.dwEquipCRC = equipCRC;
	msg.dwSpellCRC = spellCRC;
	msg.dwSpiritCRC = spiritCRC;
	msg.dwRoleID = roleId;
		
	_Send(&msg, sizeof(msg));

	Send_QueryPower( roleId );
}

void CMessageHandler::Send_QueryPower( INT roleId )
{
	C2G_QUERY_ROLE_POW msg;

	msg.wMsg = c2g_query_role_pow;
	msg.dwRoleID = roleId;

	_Send( &msg , sizeof(msg) );

}

////========================================================================================
void CMessageHandler::Parse_g2c_sync_role_attr_data(const char* buf, UINT32 size)
{
	G2C_SYNC_ROLE_ATTR_DATA* msg = (G2C_SYNC_ROLE_ATTR_DATA*)buf;
	if(!msg)
	{
		return;
	}

	INT byAttrCount = ( size - sizeof(G2C_SYNC_ROLE_ATTR_DATA) ) / sizeof( msg->astAttrData[0] );

	// modified by zhangrui05
	//INT crc = appMemCrc( msg , size );
	INT crc = crc32(0, (const Bytef*)msg, sizeof(G2C_SYNC_ROLE_ATTR_DATA));

	MMOPlayerInfoManager::getSingleton().showInfoByRankList(msg->dwRoleID);
	MMOPlayerInfoManager::getSingleton().UpdatePlayerAttrInfo( msg->dwRoleID , byAttrCount , msg->astAttrData , crc ,msg->nFatigue);
}

////========================================================================================
void CMessageHandler::Parse_g2c_sync_role_equip_data(const char* buf, UINT32 size)
{
	G2C_SYNC_ROLE_EQUIP_DATA* msg = (G2C_SYNC_ROLE_EQUIP_DATA*)buf;
	if(!msg)
	{
		return;
	}

	// modified by zhangrui05
	//INT crc = appMemCrc( msg , size );
	INT crc = crc32(0, (const Bytef*)msg, sizeof(G2C_SYNC_ROLE_EQUIP_DATA));

	MMOPlayerInfoManager::getSingleton().showInfoByRankList(msg->dwRoleID);
	//Body strengthening info first, because the UI is refreshed by message only after equipment info
	MMOPlayerInfoManager::getSingleton().UpdatePlayerRefineInfo( msg->dwRoleID , msg->awRefineLevels );
	MMOPlayerInfoManager::getSingleton().UpdatePlayerEquipInfo( msg->dwRoleID , msg->byItemCount , msg->acItems , crc );
	
}

void CMessageHandler::Parse_g2c_sync_role_pow(const char* buf, UINT32 size)
{
	G2C_SYNC_ROLE_POW* msg = (G2C_SYNC_ROLE_POW*)buf;
	if(!msg)
	{
		return;
	}
	MMOPlayerInfoManager::getSingleton().showInfoByRankList(msg->dwRoleID);
	MMOPlayerInfoManager::getSingleton().UpdatePlayerPower( msg->dwRoleID , msg->nPow );
	MMOGameEvent_UpdateRolePow evt(msg->dwRoleID, msg->nPow);
	MMOUI::PostGameEvent(&evt);
	
}

void CMessageHandler::Parse_g2c_sync_role_spirit_data( const char* buf, UINT32 size )
{
	G2C_SYNC_ROLE_SPIRIT_DATA* msg = (G2C_SYNC_ROLE_SPIRIT_DATA*)buf;
	if(!msg)
	{
		return;
	}

	// modified by zhangrui05
	//INT crc = appMemCrc( msg , size );
	INT crc = crc32(0, (const Bytef*)msg, sizeof(G2C_SYNC_ROLE_SPIRIT_DATA));
	MMOPlayerInfoManager::getSingleton().UpdatePlayerSpiritInfo( msg->dwRoleID , (ROLE_DB_SPIRIT_DATA*)msg->szData , crc );
	
}
	
void CMessageHandler::Parse_g2c_sync_role_spell_data( const char* buf, UINT32 size )
{
	G2C_SYNC_ROLE_SPELL_DATA* msg = (G2C_SYNC_ROLE_SPELL_DATA*)buf;
	if(!msg)
	{
		return;
	}
		
	INT spellCount = ( size - sizeof(G2C_SYNC_ROLE_SPELL_DATA) ) / sizeof( SPELL_DATA );

	// modified by zhangrui05
	//INT crc = appMemCrc( msg , size );
	INT crc = crc32(0, (const Bytef*)msg, sizeof(G2C_SYNC_ROLE_SPELL_DATA));

	MMOPlayerInfoManager::getSingleton().showInfoByRankList(msg->dwRoleID);
	MMOPlayerInfoManager::getSingleton().UpdatePlayerSpellInfo( msg->dwRoleID , spellCount , msg->SpellData , crc );

}

void CMessageHandler::Parse_m2c_time_info( const char* buf, UINT32 size )
{
	M2C_TIME_INFO* msg = (M2C_TIME_INFO*)buf;
	if(!msg)
	{
		return;
	}

	MMOGameEvent_OneInt syncGameLeftTimeEvt( GameEvent_SyncGameLeftTime, (INT32)msg->nPlayTimeLeft );
	MMOUI::PostGameEvent( &syncGameLeftTimeEvt );
}

void CMessageHandler::Parse_m2c_buy_time( const char* buf, UINT32 size )
{
	M2C_BUY_TIME* msg = (M2C_BUY_TIME*)buf;
	if(!msg)
	{
		return;
	}

	if( !msg->bAuto ){
		UMMOUIRecharge* rechargePanel = (UMMOUIRecharge*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Recharge );
		if( rechargePanel != NULL ){
			rechargePanel->NotifyRechargeResult( msg->byResult );
		}
	}
	else{
		if( msg->byResult ){
			FString hintMsgFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Recharge.AutoRechargeHint") );
			FString timeStr = appItoa( 100 );
			timeStr += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Time.CountDown5") );
			FString hintMsg = FString::Printf( *hintMsgFormat, *timeStr );
			MMOUI::GetUIManager()->delegateAddScreenMessage( hintMsg, MsgInfoType_Table, TEXT("MMOGame.Recharge.AutoRechargeHint") );
			MMOChatManager::getSingleton().AddSystemMessage( hintMsg );
		}
	}
	

	//MMOGameEvent_OneInt rechargeRstEvt( GameEvent_RechargeGameTimeResult, (INT32)msg->byResult );
	//MMOUI::PostGameEvent( &rechargeRstEvt );
}

//===================================================================================
void CMessageHandler::Send_SignInToday()
{
	C2G_CHECKIN msg;
	msg.wMsg = c2g_checkin;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Parse_g2c_checkin( const char* buf, UINT32 size )
{
	G2C_CHECKIN* msg = (G2C_CHECKIN*)buf;
	if( !msg ){
		return;
	}

	MMOGameEvent_OneInt signedInTodayEvt( GameEvent_SignedInToday, msg->byIndex );
	MMOUI::PostGameEvent( &signedInTodayEvt );
}

void CMessageHandler::Parse_g2c_sync_checkin_info( const char* buf, UINT32 size )
{
	G2C_SYNC_CHECKIN_INFO* msg = (G2C_SYNC_CHECKIN_INFO*)buf;
	if( !msg ){
		return;
	}

	MMOGameEvent_DailySignIn syncDailySignInEvt( msg->byLastCheckInIndex, msg->byCanCheckInToday, msg->byCheckInValid );
	MMOUI::PostGameEvent( &syncDailySignInEvt );
	//msg->wLastCheckInYear;
	//msg->byLastCheckInMonth;
}
//===================================================================================