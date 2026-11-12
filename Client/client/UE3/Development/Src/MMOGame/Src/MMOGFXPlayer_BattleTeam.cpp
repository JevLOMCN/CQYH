#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "UITextParser.h"
#include "BattleTeamManager.h"
#include "MMOMsgBoxManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_BattleTeam)

#define CreateTeamCost 1000

void UMMOGFXPlayer_BattleTeam::ASBattleTeamCheckName(const FString& teamName)
{

}

void UMMOGFXPlayer_BattleTeam::ASBattleTeamLeave()
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(player)
	{
		CMessageHandler::GetInstance().Send_LeaveBattleTeam( player->ServerID );
	}
}

void UMMOGFXPlayer_BattleTeam::ASBattleTeamAddMember()
{
	if( _BattleTeam )
	{
		_BattleTeam->ClickAddMember();
	}


}

//void UMMOGFXPlayer_BattleTeam::ASBattleTeamRightClick(INT x,INT y,class UGFxObject* mc,INT index)
//{
//	
//	
//}


void UMMOGFXPlayer_BattleTeam::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_BattleTeam::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_BattleTeam);
}

void UMMOGFXPlayer_BattleTeam::UnInit()
{
	Super::UnInit();

}

void UMMOGFXPlayer_BattleTeam::ASBattleTeamRightClickMember( INT roleId , INT x , INT y )
{
	if( _BattleTeam && _BattleTeam->PageType == BattleTeamPage_5v5 )
	{
		MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_BattleMember , x , y , roleId , TEXT("") , 0 );
	}
	
}

void UMMOGFXPlayer_BattleTeam::ASBattleTeamTurnOneToOne()
{
	if( _BattleTeam )
	{
		_BattleTeam->BattleTeamOneToOne();
	}
}

void UMMOGFXPlayer_BattleTeam::ASBattleTeamTurnFiveToFive()
{
	//determine whether there is a team
	if( false/*BattleTeamManager::getSingleton().HasTeam()*/ )
	{
		if( _BattleTeam )
		{
			_BattleTeam->BattleTeamFiveToFive();
		}
	}
	else
	{
		if( _BattleTeam )
		{
			_BattleTeam->ClickOwnPanel( BattleTeamPage_1v1 );
		}
		
		FString content = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleTeam.Cost") ) + UITextParser::HTMLMoneyIconString( MONEYTYPE_02 , CreateTeamCost );
		MMOMsgBoxManager::getSingleton().AddModalInputMsgboxEvent( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleTeam.CreateTeamName") ) , 0 , content , GameEvent_BattleTeamCreate );
	}

}