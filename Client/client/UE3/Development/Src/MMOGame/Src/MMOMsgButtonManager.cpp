#include "stdafx.h"
#include "MMOMsgButtonManager.h"
#include "MMOFacade.h"
#include "MMOMsgBoxManager.h"
//#include "MMOHintButtonCommand.h"

//static MMOHintButtonCommand_ArenaQueue Command_ArenaQueue;

MMOMsgButtonManager::MMOMsgButtonManager()
{
	//const FString IconPath = GConfig->GetStr( TEXT("MMOGame.HintButtonIcon") , TEXT("ArenaQueueHintIcon") , GGameIni );
	//_HintIconMap.insert( std::make_pair( EHintButtonType_ArenaQueueHint , IconPath )  );

	//_HintCommandMap.insert( std::make_pair( EHintButtonType_ArenaQueueHint , &Command_ArenaQueue ) );
}

MMOMsgButtonManager::~MMOMsgButtonManager()
{

}

void MMOMsgButtonManager::ClearAll()
{
	//_MsgButtonInfoList.clear();
	//UpdateButtonBar();
	for ( INT i = 0 ; i < E_MsgBtnShow_Max ; i++ )
	{
		_MsgButtonInfoList[i].clear();
		UpdateButton(i);
	}
}

void MMOMsgButtonManager::ClickButton( INT type )
{
	if (type < 0 || type >= E_MsgBtnShow_Max)
		return;

	MsgButtonInfoVec& list = _MsgButtonInfoList[type];
	INT size = list.size();

	if( size == 0 )
	{
		//shouldn't normally reach here
		return;
	}

	if ( list[0]._eventHandleType == EVENTHANDLE_GAMEEVENT )
	{
		if( size == 1 )
		{
			//只有一个的话就直接弹出菜单，消掉此按钮
			MsgButtonInfo selected = list[0];
			list.erase(list.begin());
			UGFxObject* mc = MMOMsgBoxManager::getSingleton().AddMessageBoxEvent(selected.msgText, selected.infoId, selected.EventId, selected.type);
			if (mc)
			{
				mc->GetObject("yesButton_mc")->SetString(TEXT("label"),selected.leftButtonName);
				mc->GetObject("noButton_mc")->SetString(TEXT("label"),selected.rightButtonName);
			}
			UpdateButton(type);
			//refresh button
		}
		else
		{
			//if there are multiple, show a list
			ToggleMsgLine(type);
			UpdateMsgLine(type);
			//
		}
	}
	else if ( list[0]._eventHandleType == EVENTHANDLE_NOMSGBOX)
	{
		_EmitDelegate( type );
		list.clear();
		//refresh button
		UpdateButton(type);
	}
	
	
}

void MMOMsgButtonManager::ClickMsgLine( INT pos , INT type )
{
	if (type < 0 || type >= E_MsgBtnShow_Max)
		return;
	
	MsgButtonInfoVec& list = _MsgButtonInfoList[type];
	if (pos >= 0 && pos < (INT)list.size())
	{
		MsgButtonInfo selected = list[pos];
		list.erase(list.begin() + pos);
		UGFxObject* mc = MMOMsgBoxManager::getSingleton().AddMessageBoxEvent(selected.msgText, selected.infoId, selected.EventId, selected.type);
		if (mc)
		{
			mc->GetObject("yesButton_mc")->SetString(TEXT("label"),selected.leftButtonName);
			mc->GetObject("noButton_mc")->SetString(TEXT("label"),selected.rightButtonName);
		}
	}
	//refresh button
	UpdateButton(type);
	//refresh list
	UpdateMsgLine(type);
	

}

const MsgButtonInfoVec& MMOMsgButtonManager::GetMsgButtonInfo( INT type ) const
{
	static const MsgButtonInfoVec empty;
	if (type < 0 || type >= E_MsgBtnShow_Max)
		return empty;
	return _MsgButtonInfoList[type];
}

void MMOMsgButtonManager::_EmitDelegate(INT type)
{
	if (type < 0 || type >= E_MsgBtnShow_Max)
		return;
	if( _MsgButtonInfoList[type].size() == 0 )
	{
		return;
	}
	UObject* contextObject = _MsgButtonInfoList[type][0].OnEventHandle.Object;
	FName funName = _MsgButtonInfoList[type][0].OnEventHandle.FunctionName;
	//debugf(TEXT(" MMOMsgButtonManager::_EmitDelegate-%s-%s"), DebugFName(contextObject), DebugFName(funName));

	if( contextObject && funName.IsValid() && funName != NAME_None )
	{
		MMOMsgButtonManager_Parms parameters = _MsgButtonInfoList[type][0]._Parms;
		contextObject->ProcessEvent( contextObject->FindFunctionChecked(funName), &parameters, NULL );
	}
}

void MMOMsgButtonManager::UpdateButton( INT type )
{
	if (type < 0 || type >= E_MsgBtnShow_Max)
		return;
	MsgButtonInfoVec& list = _MsgButtonInfoList[type];
	INT size = list.size();

	if( size != 0 )
	{
		MMOGameEvent_AddMsgBall evt( GameEvent_AddMsgBall , type , size );
		MMOUI::PostGameEvent(&evt);
	}
	else
	{
		MMOGameEvent_OneInt evt( GameEvent_RemoveMsgBall , type );
		MMOUI::PostGameEvent(&evt);
	}
	
}

void MMOMsgButtonManager::ToggleMsgLine( INT type )
{
	if (type < 0 || type >= E_MsgBtnShow_Max)
		return;
	MMOGameEvent_OneInt evt( GameEvent_ToggleMsgLine , type );
	MMOUI::PostGameEvent(&evt);	
}

void MMOMsgButtonManager::UpdateMsgLine( INT type )
{
	if (type < 0 || type >= E_MsgBtnShow_Max)
		return;
	MMOGameEvent_OneInt evt( GameEvent_UpdateMsgLine , type );
	MMOUI::PostGameEvent(&evt);	
}

//INT MMOMsgButtonManager::AddNewButton( const FString& Text , INT InfoId , FScriptDelegate OnClickOK , FScriptDelegate OnClickNO, eMsgBtnShowType type )
//{
//	MsgButtonInfo msgButtonInfo;
//	msgButtonInfo._eventHandleType = EVENTHANDLE_DELEGATE;
//	msgButtonInfo.msgText = Text;
//	msgButtonInfo.infoId = InfoId;
//	msgButtonInfo.OnClickOK = OnClickOK;
//	msgButtonInfo.OnClickNO = OnClickNO;
//	msgButtonInfo._Type = type;
//
//	_MsgButtonInfoList.push_back( msgButtonInfo );
//	INT cnt = _MsgButtonInfoList.size();
//	if( cnt <= UCONST_msgButtonCount )
//	{
//		UpdateButtonBar();
//	}
//
//	return cnt - 1;
//}

INT MMOMsgButtonManager::AddNewButton( const FString& Text , INT InfoId , FString sendName , EGameEvent eventId , eMsgBtnShowType type ,FString leftButtonMsg, FString rightButtonMsg)
{
	if (type < 0 || type >= E_MsgBtnShow_Max)
		return -1;
	MsgButtonInfo msgButtonInfo;
	msgButtonInfo._eventHandleType = EVENTHANDLE_GAMEEVENT;
	msgButtonInfo.msgText = Text;
	msgButtonInfo.infoId = InfoId;		
	msgButtonInfo.EventId = eventId;
	msgButtonInfo.senderName = sendName;
	msgButtonInfo.leftButtonName = MMOUI::ParseLocalizedPropertyPath( leftButtonMsg );
	msgButtonInfo.rightButtonName = MMOUI::ParseLocalizedPropertyPath( rightButtonMsg );
	msgButtonInfo.type = type;

	MsgButtonInfoVec& msgButtonList = _MsgButtonInfoList[type];
	INT buttonNum = msgButtonList.size();
	for ( INT i = 0 ; i < buttonNum ; i++ )
	{
		//remove if the same message already exists
		if( msgButtonList[i].infoId == InfoId && msgButtonList[i].EventId == eventId )
		{
			msgButtonList.erase( msgButtonList.begin()+i );
			break;
		}
	}
	
	msgButtonList.push_back( msgButtonInfo );
	
	INT cnt = msgButtonList.size();	
	
	//update button
	MMOGameEvent_AddMsgBall evt( GameEvent_AddMsgBall , type , cnt );
	MMOUI::PostGameEvent(&evt);
	//update info bar
	UpdateMsgLine(type);

	return cnt - 1;
}

INT MMOMsgButtonManager::AddNewButton( FScriptDelegate OnEventHandle, MMOMsgButtonManager_Parms Parms, eMsgBtnShowType type ,FString leftButtonMsg, FString rightButtonMsg)
{
	if (type < 0 || type >= E_MsgBtnShow_Max)
		return -1;
	MsgButtonInfo msgButtonInfo;
	
	msgButtonInfo._eventHandleType = EVENTHANDLE_NOMSGBOX;	
	msgButtonInfo.OnEventHandle = OnEventHandle;
	msgButtonInfo._Parms = Parms;
	msgButtonInfo.type = type;
	msgButtonInfo.leftButtonName = MMOUI::ParseLocalizedPropertyPath( leftButtonMsg );
	msgButtonInfo.rightButtonName = MMOUI::ParseLocalizedPropertyPath( rightButtonMsg );

	MsgButtonInfoVec& msgButtonList = _MsgButtonInfoList[type];
	msgButtonList.push_back( msgButtonInfo );

	INT cnt = msgButtonList.size();
	//update button
	MMOGameEvent_AddMsgBall evt( GameEvent_AddMsgBall , type , cnt );
	MMOUI::PostGameEvent(&evt);

	return cnt - 1;
}


void MMOMsgButtonManager::RemoveButton( INT index )
{

	//if( (INT)_MsgButtonInfoList.size() <= index )
	//{
	//	return;
	//}

	//_MsgButtonInfoList.erase( _MsgButtonInfoList.begin() + index );

	//UpdateButtonBar();
}

void MMOMsgButtonManager::TickTime( float deltaTime )
{

}
void MMOMsgButtonManager::RemoveOneTypeButtons(INT type)
{
	if (type < 0 || type >= E_MsgBtnShow_Max)
		return;
	MsgButtonInfoVec& list = _MsgButtonInfoList[type];
	if(list.size() == 0)
		return;
	list.clear();
	UpdateButton(type);
}