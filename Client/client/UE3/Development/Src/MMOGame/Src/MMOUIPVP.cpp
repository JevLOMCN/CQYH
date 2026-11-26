#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOBattleManager.h"
#include "MMOArenaManager.h"
#include "MMOChatManager.h"
#include "TeamManager.h"

IMPLEMENT_CLASS(UMMOUIPVP)

//
void UMMOUIPVP::NativeInitOnce()
{
	//if(ArenaPanel)
	//{
	//	UGFxObject* confirmButton = ArenaPanel->GetObject( TEXT("ConfirmButton") );
	//	if(confirmButton)
	//		confirmButton->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.ArenaEnter") ) );

	//	UGFxObject* cancelButton = ArenaPanel->GetObject( TEXT("CancelButton") );
	//	if(cancelButton)
	//		cancelButton->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleTeam.Cancel") ) );
	//}

}

void UMMOUIPVP::Init()
{
	if( GFXPlayer ){
		GFXPlayer->AddPostAdvanceDelegate( FScriptDelegate(this, TEXT("PostAdvance")) );
	}

	ArenaMode.Empty();
	for( INT i = 0 ; i < ArenaModeSelCount ; i++ )
	{
		ArenaMode.AddItem(GetObject( FString( TEXT("ArenaMode") ) + appItoa(i) ));
	}


	ArenaSize.Empty();
	for( INT i = 0 ; i < ArenaSizeSelCount ; i++ )
	{
		ArenaSize.AddItem(GetObject( FString( TEXT("ArenaSize") ) + appItoa(i) ));
	}

	//这个版本禁用掉5V5
	if (ArenaSize.IsValidIndex(1) && ArenaSize(1))
		ArenaSize(1)->SetBool( TEXT("enable") , FALSE );


	UGFxObject* data = GFXPlayer ? GFXPlayer->CreateObject( TEXT("Object") ) : NULL;
	if(data)
	{
		data->SetString( TEXT("titleName") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.Arena") ));
		data->SetString( TEXT("arenaModeText") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.ArenaModeText") ));
		data->SetString( TEXT("arenaSizeText") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.ArenaSize") ));
		data->SetString( TEXT("openTimeText") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.OpenTimeText") ));
		data->SetString( TEXT("confirmButton") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.ArenaEnter") ) );
		eventCallSetPanelText(data);
	}
	
}

void UMMOUIPVP::UnInit()
{
	Super::UnInit();
	if( GFXPlayer ){
		GFXPlayer->RemovePostAdvanceDelegate( FScriptDelegate(EC_EventParm) );
	}
}

void UMMOUIPVP::ArenaEnter()
{
	INT arenaModeId = 0;
	INT arenaSizeId = 0;
	for ( INT i = 0 ; i < ArenaMode.Num() ; i++ )
	{
		if( ArenaMode(i) && ArenaMode(i)->GetBool( TEXT("selected") ) )
		{
			arenaModeId = i;
			break;
		}
	}

	for ( INT i = 0 ; i < ArenaSize.Num() ; i++ )
	{
		if( ArenaSize(i) && ArenaSize(i)->GetBool( TEXT("selected") ) )
		{
			arenaSizeId = i;
			break;
		}
	}

	Show(FALSE);
	//MMOArenaManager::getSingleton().PrepareArenaEnter( arenaModeId , arenaSizeId );

}

void UMMOUIPVP::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	
	if( v && GFXPlayer )
	{
		if( ArenaMode.Num() >= 1 )
			GFXPlayer->eventCallWidgetDispatchEvent( ArenaMode(0) , TEXT("click") );
		if( ArenaSize.Num() >= 1 )
			GFXPlayer->eventCallWidgetDispatchEvent( ArenaSize(0) , TEXT("click") );
	}
}

//===============================================================================================================
//===============================================================================================================
void UMMOUIPVP::PostAdvance(FLOAT DeltaTime)
{
	TickForTestInteractNPCDistance(DeltaTime);
}
//===============================================================================================================
//===============================================================================================================