#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMsgButtonManager.h"
#include "MailManager.h"
#include "CommonGameEvents.h"
#include "MasterStudentManager.h"
#include "MMOGuildManager.h"
#include "WebHtmlManager.h"
#include "MMODailySignInManager.h"
#include "MMOJigsawManager.h"

IMPLEMENT_CLASS(UMMOUIMsgButtonBar)

void UMMOUIMsgButtonBar::NativeInitOnce()
{
	Super::NativeInitOnce();
	MailManager::getSingleton().SetOpenMailPanelDelegate(FScriptDelegate(this, TEXT("OpenMailPanel")));
	MasterStudentManager::getSingleton().SetOpenMasterStudentPanelDelegate(FScriptDelegate(this, TEXT("OpenMasterStudentPanel")));
	MasterStudentManager::getSingleton().SetOpenCongUpLevelPanelDelegate(FScriptDelegate(this, TEXT("OpenCongUpLevelPanel")));
	MMOGuildManager::getSingleton().SetOpenGuildWarStartPanelAttackDelegate(FScriptDelegate(this, TEXT("OpenGuildWarStartPanelAttack")));
	MMOGuildManager::getSingleton().SetOpenGuildWarStartPanelDefendDelegate(FScriptDelegate(this, TEXT("OpenGuildWarStartPanelDefend")));
	WebHtmlManager::getSingleton().SetOpenGmBallDelegate(FScriptDelegate(this,TEXT("OpenNewSurvey")));
	MMOGuildManager::getSingleton().SetOpenGuildMainPanelApplyAllyDelegate(FScriptDelegate(this, TEXT("OpenGuildMainPanelApplyAlly")));
	MMODailySignInManager::getSingleton().SetOpenDailySignInPanelDelegate(FScriptDelegate(this, TEXT("OpenDailySignInPanel")));
	MMOJigsawManager::getSingleton().SetOpenJigsawPanelDelegate(FScriptDelegate(this, TEXT("OpenJigsawPanel")));
}

void UMMOUIMsgButtonBar::OpenMailPanel()
{
	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_Mail , TRUE, "" );
}

void UMMOUIMsgButtonBar::OpenMasterStudentPanel()
{
	const MsgButtonInfoVec& list = MMOMsgButtonManager::getSingleton().GetMsgButtonInfo(E_MsgBtnShow_Master);
	for (INT i = 0; i < (INT)list.size(); ++ i)
	{
		if(list[i]._Parms.Parm1 == -1)
		{
			UMMOUIMasterStudentPanel*pkPanel = (UMMOUIMasterStudentPanel*)MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_MasterStudent , TRUE, "" );
			if (pkPanel)
			{
				pkPanel->eventCallUCClickMasterTap(list[i]._Parms.Parm2);

				if (list[i]._Parms.Parm3 > 0)
				{
					MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_OnClickMasterStudent);
					evt.ID = list[i]._Parms.Parm3;
					MMOUI::PostGameEvent(&evt);
				}
				break;
			}
		}
		else if(list[i]._Parms.Parm1 == 0)
		{
			MasterStudentManager::getSingleton().OpenHelloStudentPanel();
			break;
		}
		else if(list[i]._Parms.Parm1 == 1)
		{
			MasterStudentManager::getSingleton().OpenHelloTeacherPanel();
			break;
		}
	}
	/*MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_TurnStudentList, 0); //xzh
	MMOUI::PostGameEvent(&evt);*/

}


void UMMOUIMsgButtonBar::OpenCongUpLevelPanel()
{
	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_MasterStudent , TRUE, "" );
	/*MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_TurnStudentDetailList, 0);//xzh
	MMOUI::PostGameEvent(&evt);*/
}

void UMMOUIMsgButtonBar::OpenGuildWarStartPanelAttack()
{
	const MsgButtonInfoVec& list = MMOMsgButtonManager::getSingleton().GetMsgButtonInfo(E_MsgBtnShow_Guild);
	for (INT i = 0; i < (INT)list.size(); ++ i)
	{
		if (list[i]._Parms.Parm1 == GMBT_ATTACK)
		{
			UMMOUIGuildWarStartPanel* panel = (UMMOUIGuildWarStartPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_WarStart);
			if (panel)
			{
				panel->ShowGuildWarStartPanel(list[i]._Parms.Parm2);
			}
			break;
		}
	}
}

void UMMOUIMsgButtonBar::OpenGuildWarStartPanelDefend()
{
	const MsgButtonInfoVec& list = MMOMsgButtonManager::getSingleton().GetMsgButtonInfo(E_MsgBtnShow_Guild);
	for (INT i = 0; i < (INT)list.size(); ++ i)
	{
		if (list[i]._Parms.Parm1 == GMBT_DEFEND)
		{
			UMMOUIGuildWarStartPanel* panel = (UMMOUIGuildWarStartPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_WarStart);
			if (panel)
			{
				panel->ShowGuildWarStartPanel(list[i]._Parms.Parm2);
			}
			break;
		}
	}
}

void UMMOUIMsgButtonBar::OpenGuildMainPanelApplyAlly()
{
	const MsgButtonInfoVec& list = MMOMsgButtonManager::getSingleton().GetMsgButtonInfo(E_MsgBtnShow_GuildAlly);
	for (INT i = 0; i < (INT)list.size(); ++ i)
	{
		if (list[i]._Parms.Parm1 == GMBT_APPLY_ALLY)
		{
			UMMOUIGuildMainPanel* panel = (UMMOUIGuildMainPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_Main);
			if (panel)
			{
				panel->SetShowPage(4, 2);
				panel->Show(TRUE);
			}
			break;
		}
	}
}

void UMMOUIMsgButtonBar::OpenNewSurvey()
{
	WebHtmlManager::getSingleton().OpenNewSurvey();
}

void UMMOUIMsgButtonBar::OpenDailySignInPanel()
{
	UMMOUIDailySignIn *dailySignInPanel = (UMMOUIDailySignIn*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_DailySignIn );
	if( dailySignInPanel != NULL ){
		dailySignInPanel->Show( TRUE );
	}
}

void UMMOUIMsgButtonBar::OpenJigsawPanel()
{
	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_Jigsaw , TRUE, "" );
}

void UMMOUIMsgButtonBar::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_AddMsgBall);
	evts.AddItem(GameEvent_RemoveMsgBall);
	evts.AddItem(GameEvent_UpdateMsgLine);
	evts.AddItem(GameEvent_ToggleMsgLine);
	EventAdapter = new TGameEventAdapter<UMMOUIMsgButtonBar>(this, evts);
}

void UMMOUIMsgButtonBar::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch( evt->GetEventType() )
	{
		case GameEvent_AddMsgBall:
		{
			MMOGameEvent_AddMsgBall* ambEvt = (MMOGameEvent_AddMsgBall*)evt;
			if(ambEvt)
			{
				UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
				if( data )
				{
					data->SetInt( TEXT("type") , ambEvt->MsgBallType );
					data->SetInt( TEXT("num") , ambEvt->MsgNum );
					data->SetString( TEXT("tips") , MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.MsgbuttonBar.hint") )+appItoa(ambEvt->MsgBallType) ) );
					eventCallAddNewButton( data );
				}				
			}
		}
		break;
		case GameEvent_RemoveMsgBall:
		{
			MMOGameEvent_OneInt* rmbEvt = (MMOGameEvent_OneInt*)evt;
			if(rmbEvt)
			{
				eventCallRemoveButton( rmbEvt->IntData );
			}
		}
		break;
		case GameEvent_ToggleMsgLine:
		{
			MMOGameEvent_OneInt* tmlEvt = (MMOGameEvent_OneInt*)evt;
			if(tmlEvt)
			{
				eventCallToggleMsgScrolingList( tmlEvt->IntData );
			}
		}
		break;
		case GameEvent_UpdateMsgLine:
		{
			MMOGameEvent_OneInt* umlevt = (MMOGameEvent_OneInt*)evt;
			if( umlevt )
			{
				UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );

				if(data)
				{
					data->SetInt( TEXT("type") , umlevt->IntData );
					UGFxObject* nameList = GFXPlayer->CreateArray();
					if( nameList )
					{
						const MsgButtonInfoVec& list = MMOMsgButtonManager::getSingleton().GetMsgButtonInfo( umlevt->IntData );

						for ( INT idx = 0 ; idx < (INT)list.size() ; idx++ )
						{
							nameList->SetElementString( idx , list[idx].senderName );
						}
						data->SetObject( TEXT("nameList") , nameList );
						eventCallUpdateMsgScrolingList(data);
					}
				}
								
			}
		}
		break;
	}

}

void UMMOUIMsgButtonBar::SetButtonNum(INT num)
{
	//MMOMsgButtonManager *pBtnManager = &MMOMsgButtonManager::getSingleton();
	//for ( int i = 0 ; i < UCONST_msgButtonCount ; i++ )
	//{
	//	if(!_MsgButton[i])
	//		continue;

	//	if( i < num )
	//	{
	//		MsgButtonInfo *pBtnInfo = pBtnManager->GetNewButtonData( i );
	//		eMsgBtnShowType type = ( pBtnInfo ? pBtnInfo->_Type : E_MsgBtnShow_Normal );
	//		_MsgButton[i]->SetInt( TEXT("CurSeriesIdx"), type );

	//		_MsgButton[i]->SetVisible(TRUE);
	//	}
	//	else
	//	{
	//		_MsgButton[i]->SetVisible(FALSE);
	//	}
	//}

	return ;
}

void UMMOUIMsgButtonBar::ClearUI()
{
	
}

void UMMOUIMsgButtonBar::SetButtonIcon(INT index,INT frame)
{
	//TArray<struct FASValue> Arguments;
	//FASValue Asvalue( EC_EventParm );
	//Asvalue.I = pos;
	//Asvalue.Type = AS_Int;
	//Arguments.AddItem( Asvalue );

	//this->Invoke( TEXT("removeButton") , Arguments );

}


void UMMOUIMsgButtonBar::ASMsgButtonClick( INT type )
{
	MMOMsgButtonManager::getSingleton().ClickButton(type);
}

void UMMOUIMsgButtonBar::ASMsgListClick( INT pos , INT type )
{
	MMOMsgButtonManager::getSingleton().ClickMsgLine( pos , type );
}

//void UMMOUIMsgButtonBar::ASOnPanelClose(const FString& PanelName)
//{
//}

