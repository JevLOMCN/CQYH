#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOPowerManager.h"
#include "BattleTeamManager.h"
#include "MMOMsgBoxManager.h"
#include "MallManager.h"
#include "UITextParser.h"

IMPLEMENT_CLASS(UMMOUIMoneyExchange)

#define ExchangeRate 10000
#define ExchangeMoneyGoodsId MONEYTYPE_02
#define LinePerPage 7

void UMMOUIMoneyExchange::Init()
{
	Super::Init();

	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_UpdateMoneyExchangeLog);
	EventAdapter = new TGameEventAdapter<UMMOUIMoneyExchange>(this, evts);
	
	
	if( GFXPlayer )
	{
		UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
		if(data)
		{
			data->SetPureString( TEXT("titleName") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MoneyExchange.MoneyExchange") ) );
			data->SetPureString( TEXT("exchangeButtonText") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MoneyExchange.ExchangeButton") ) );
			data->SetPureString( TEXT("cancelButtonText") , MMOUI::ParseLocalizedPropertyPath("MMOGame.MoneyExchange.CancelButton") );
			data->SetPureString( TEXT("exchangeDescText") , MMOUI::ParseLocalizedPropertyPath("MMOGame.MoneyExchange.Desc") );
			data->SetPureString( TEXT("moneyNameText0") , TEXT("") );
			data->SetPureString( TEXT("moneyNameText1") , TEXT("") );

			eventCallSetPanelText( data );
		}

		

		eventCallUpdateShowLog( TEXT("") );
	}

	
}
void UMMOUIMoneyExchange::UnInit()
{

}

void UMMOUIMoneyExchange::InputChange()
{
	UGFxObject* panel = GetObject(TEXT("exchangePanel_mc"));
	if( panel )
	{
		UGFxObject* wgt = panel->GetObject( TEXT("inputMoney") );
		if( wgt )
		{
			INT money = appAtoi( *wgt->GetString(TEXT("text")) ) ;
			eventCallUpdateExchangeGold( UITextParser::HTMLMoneyIconString( MONEYTYPE_02 , money * ExchangeRate ) );
		}
	}
}

void UMMOUIMoneyExchange::ExchangeOk()
{
	UGFxObject* panel = GetObject(TEXT("exchangePanel_mc"));
	if( panel )
	{
		UGFxObject* wgt = panel->GetObject( TEXT("inputMoney") );
		if( wgt )
		{
			INT money = appAtoi( *wgt->GetString(TEXT("text")) ) ;
			CMessageHandler::GetInstance().Send_MallBuyGoods( ExchangeMoneyGoodsId , money );
		}
	}
	
}

void UMMOUIMoneyExchange::PostOnShow(UBOOL v)
{
	if(v)
	{
		if( GFXPlayer )
		{
			UGFxObject* dataCurMoney = GFXPlayer->CreateObject( TEXT("Object") );
			if(dataCurMoney)
			{
				INT64 Money02 = MMOFacade::getSingleton().GetPlayerMoney64( E_MONEYTYPE_02 );
				dataCurMoney->SetString( TEXT("curMoneyText0") , UITextParser::HTMLMoneyIconString( E_MONEYTYPE_02 , Money02 ) );
				INT64 Money04 = MMOFacade::getSingleton().GetPlayerMoney64( E_MONEYTYPE_04 );
				dataCurMoney->SetString( TEXT("curMoneyText1") , UITextParser::HTMLMoneyIconString( E_MONEYTYPE_04 , Money04 ) );

				eventCallUpdateCurMoney(dataCurMoney);
			}
		}

		CMessageHandler::GetInstance().Send_RequestExchangeLog();
	}


}

void UMMOUIMoneyExchange::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch(evt->GetEventType())
	{
	case GameEvent_UpdateMoneyExchangeLog:
		{
			RefreshLog();
		}
		break;
	}
}

void UMMOUIMoneyExchange::RefreshLog()
{
	TArray<SHOP_LOG>& logArray = MallManager::getSingleton().GetShopLog();
	FString logText;
	
	
	for ( INT i = 0 ; i < logArray.Num() ; i++ )
	{
		INT year , month , dayOfWeek , day , hour , min , sec;
		year = month = dayOfWeek = day = hour = min = sec = 0;
		appSecondsToLocalTime( (time_t)logArray(i).dwShopTime , year , month , dayOfWeek , day , hour , min , sec );


		FString::Printf( TEXT("%s exchanged %s for %s <br>") , 
			*FString::Printf( TEXT("%d-%d-%d %d:%02d:%02d ") , year , month , day , hour , min , sec ) , 
			*UITextParser::HTMLMoneyIconString( MONEYTYPE_04 , logArray(i).nCost )  , 
			*UITextParser::HTMLMoneyIconString( MONEYTYPE_02 , logArray(i).nItemCount ) );
	}
}

void UMMOUIMoneyExchange::ASMoneyExchangeOk()
{
	ExchangeOk();
}

void UMMOUIMoneyExchange::ASMoneyExchangeInputChange()
{
	InputChange();
}
