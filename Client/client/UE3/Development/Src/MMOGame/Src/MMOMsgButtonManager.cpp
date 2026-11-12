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
			//if there is only one, pop up the menu directly and remove this button
			UGFxObject* mc = MMOMsgBoxManager::getSingleton().AddMessageBoxEvent( list[0].msgText , list[0].infoId , list[0].EventId ,list[0].type);
			if (mc)
			{
				mc->GetObject("yesButton_mc")->SetString(TEXT("label"),list[0].leftButtonName);
				mc->GetObject("noButton_mc")->SetString(TEXT("label"),list[0].rightButtonName);
			}
			list.erase( list.begin() );
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
	
	MsgButtonInfoVec& list = _MsgButtonInfoList[type];
	if( (INT)list.size() > pos )
	{
		UGFxObject* mc = MMOMsgBoxManager::getSingleton().AddMessageBoxEvent( list[pos].msgText , list[pos].infoId , list[pos].EventId ,list[0].type);
		if (mc)
		{
			mc->GetObject("yesButton_mc")->SetString(TEXT("label"),list[pos].leftButtonName);
			mc->GetObject("noButton_mc")->SetString(TEXT("label"),list[pos].rightButtonName);
		}
		list.erase( list.begin() + pos );
	}
	//refresh button
	UpdateButton(type);
	//refresh list
	UpdateMsgLine(type);
	

}

MsgButtonInfoVec& MMOMsgButtonManager::GetMsgButtonInfo( eMsgBtnShowType type )
{	
	return _MsgButtonInfoList[type];
}

void MMOMsgButtonManager::_EmitDelegate(INT type)
{
	if( _MsgButtonInfoList[type].size() == 0 )
	{
		return;
	}
	UObject* contextObject = _MsgButtonInfoList[type][0].OnEventHandle.Object;
	FName funName = _MsgButtonInfoList[type][0].OnEventHandle.FunctionName;
	//debugf(TEXT(" MMOMsgButtonManager::_EmitDelegate-%s-%s"), DebugFName(contextObject), DebugFName(funName));

	if( contextObject && funName.IsValid() && funName != NAME_None )
	{
		contextObject->ProcessEvent( contextObject->FindFunctionChecked(funName),  &(_MsgButtonInfoList[type][0]._Parms) , NULL );
	}
}

void MMOMsgButtonManager::UpdateButton( INT type )
{
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
	MMOGameEvent_OneInt evt( GameEvent_ToggleMsgLine , type );
	MMOUI::PostGameEvent(&evt);	
}

void MMOMsgButtonManager::UpdateMsgLine( INT type )
{
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
	MsgButtonInfo msgButtonInfo;
	
	ZeroMemory(&msgButtonInfo, sizeof(msgButtonInfo));
	msgButtonInfo._eventHandleType = EVENTHANDLE_NOMSGBOX;	
	msgButtonInfo.OnEventHandle = OnEventHandle;
	msgButtonInfo._Parms = Parms;
	msgButtonInfo.leftButtonName = MMOUI::ParseLocalizedPropertyPath( leftButtonMsg );
	msgButtonInfo.rightButtonName = MMOUI::ParseLocalizedPropertyPath( rightButtonMsg );

	MsgButtonInfoVec& msgButtonList = _MsgButtonInfoList[type];
	msgButtonList.push_back( msgButtonInfo );
	msgButtonInfo.type = type;

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
	MsgButtonInfoVec& list = _MsgButtonInfoList[type];
	if(list.size() == 0)
		return;
	list.clear();
	UpdateButton(type);
}