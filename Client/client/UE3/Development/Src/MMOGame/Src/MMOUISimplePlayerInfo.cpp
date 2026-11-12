#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "net\message_handler.h"
#include "CommonGameEvents.h"
#include "MMOPlayerInfoManager.h"
#include "MMOGameEvent.h"
#include "MMOFacade.h"

IMPLEMENT_CLASS(UMMOUISimplePlayerInfo)

void UMMOUISimplePlayerInfo::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_PlayerBaseInfo);

	EventAdapter = new TGameEventAdapter<UMMOUISimplePlayerInfo>(this, evts);
}

void UMMOUISimplePlayerInfo::UnInit()
{
	Super::UnInit();

}

void UMMOUISimplePlayerInfo::InitPlayerRoleId( INT roleId )
{
	_RoleId = roleId;

	PlayerSimpleInfo& simpleInfo = MMOPlayerInfoManager::getSingleton().GetSimpleInfo( );

	PlayerBaseInfo* baseinfo = MMOFacade::getSingleton().RequestPlayerBaseInfo( _RoleId , TRUE );
	if( baseinfo )
	{
		simpleInfo.Level = baseinfo->_Level;
		simpleInfo.ClassId = baseinfo->_RoleClass;
		simpleInfo.Sex = baseinfo->_Sex;
		simpleInfo.RoleName = baseinfo->_RoleName;
		simpleInfo.Vip = baseinfo->_VIPType > 0;
		simpleInfo.GuildName = baseinfo->_GuildName;
	}

	UpdatePlayerInfo();

	//There is also a request packet to send, not sent for now
}

void UMMOUISimplePlayerInfo::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch( evt->GetEventType() )
	{
	case GameEvent_PlayerBaseInfo:
		{
			MMOGameEvent_PlayerBaseInfo* msg = (MMOGameEvent_PlayerBaseInfo*)evt;
			if( msg && msg->_RoldID == _RoleId )
			{
				if( msg->_PlayerBaseInfo )
				{
					PlayerSimpleInfo& simpleInfo = MMOPlayerInfoManager::getSingleton().GetSimpleInfo();
					
					simpleInfo.Level = msg->_PlayerBaseInfo->_Level;
					simpleInfo.ClassId = msg->_PlayerBaseInfo->_RoleClass;
					simpleInfo.Sex = msg->_PlayerBaseInfo->_Sex;
					simpleInfo.RoleName = msg->_PlayerBaseInfo->_RoleName;
					simpleInfo.Vip = msg->_PlayerBaseInfo->_VIPType;
					simpleInfo.GuildName = msg->_PlayerBaseInfo->_GuildName;
				}
				UpdatePlayerInfo();
			}
		}
		break;
	
	}
}

void UMMOUISimplePlayerInfo::UpdatePlayerInfo()
{
	PlayerSimpleInfo& simpleInfo = MMOPlayerInfoManager::getSingleton().GetSimpleInfo();

	if( GFXPlayer )
	{
		UGFxObject* data = GFXPlayer->CreateObject(TEXT("Object"));
		if(data)
		{
			data->SetString( TEXT("ClassName") , MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.School.SchoolName") ) + appItoa( simpleInfo.ClassId ) ) );
			data->SetString( TEXT("Level") , appItoa( simpleInfo.Level ) ); 
			data->SetString( TEXT("RoleName") , simpleInfo.RoleName );
			data->SetBool( TEXT("IsVip") , simpleInfo.Vip );
			data->SetString( TEXT("HeadIcon") , MMOFacade::getSingleton().GetPlayerIcon( simpleInfo.ClassId , simpleInfo.Sex ) );
			data->SetString( TEXT("MapName") , TEXT("") );
			data->SetString( TEXT("Team") , TEXT("") );
			data->SetPureString( TEXT("Guild") , (simpleInfo.GuildName.Len() > 0 ? simpleInfo.GuildName : TEXT("[No Guild]")));
			//simpleInfo.Sex;
			eventCallUpdatePlayerInfo(data);
		}
	}
}