#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "ScaleformEngine.h"
#include "MMOGuildManager.h"
#include "MMOChatManager.h"
#include "LinkManager.h"
#include "TeamManager.h"
#include "MMOQuestManager.h"
#include "MMOGuideManager.h"
#include "UITextParser.h"
#include "MMOChatManager.h"
#include "MMOClientVariableManager.h"
#include "MMOVipManager.h"
#include "MMOAuctionManager.h"
#include "FSMIO.h"

IMPLEMENT_CLASS(UMMOUIChat)
extern UMMOGFXPlayer_RootBase* g_rootGFXPlayer;

#define WorldChannelConst 1000

INT Anz_Unicode2_Utf8( const FString& InputName, INT LimitSize)
{
	INT Count = 0;
	INT Size  = 0;
	//size_t Length = InputName.Len();
	//const TCHAR*  chatMsgBuffer= *InputName;

	//written by linjian, has problems, york modified it with a ue3 function
	FTCHARToUTF8 conv(*InputName);
	INT lenth = conv.Length() + 1;
	if( lenth > LimitSize)
		return lenth;
	return lenth;
	//for(size_t i=0;i<Length;i++)
	//{
	//	TCHAR c = *(chatMsgBuffer+i);
	//	if(c<=0x007F)
	//	{
	//		Size++;
	//	}
	//	else if(c<=0x07FF)
	//	{
	//		Size+=2;
	//	}
	//	else if(c<=0xFFFF)
	//	{
	//		Size+=3;
	//	}
	//	else
	//	{
	//		Size+=4;
	//	}

	//	if(Size <= LimitSize)
	//	{
	//		Count++;
	//	}
	//	else
	//	{
	//		return Count;
	//	}
	//}
	//return Count;
}

//INT CheckMsgInput( const FString& InputName, INT &Count ,INT &Size)
//{
//	Count = 0;
//	Size  = 0;
//	static TCHAR   chatMsgBuffer[2048];
//	wcscpy_s(chatMsgBuffer, _countof(chatMsgBuffer), *InputName) ;
//	size_t Length = InputName.Len();
//
//	INT res = 1;
//	for(size_t i=0;i<Length;i++)
//	{
//		TCHAR c = chatMsgBuffer[i];
//		if(c>=TCHAR('0')&&c<=TCHAR('9'))
//		{
//			Count++;
//			Size++;
//		}
//		else if(c>=TCHAR('a')&&c<=TCHAR('z'))
//		{
//			Count++;
//			Size++;
//		}
//		else if(c>=TCHAR('A')&&c<=TCHAR('Z'))
//		{
//			Count++;
//			Size++;
//		}
//		else if(c==TCHAR('<')||c==TCHAR('>'))
//		{
//			Count++;
//			Size++;
//		}
//		else if(c==TCHAR('_')||c==TCHAR('.'))
//		{
//			Count++;
//			Size++;
//		}
//		else if(c==TCHAR(',')||c==TCHAR('&'))
//		{
//			Count++;
//			Size++;
//		}
//		else if(c==TCHAR(';')||c==TCHAR('='))
//		{
//			Count++;
//			Size++;
//		}
//		else if(c==TCHAR('$')||c==TCHAR('/'))
//		{
//			Count++;
//			Size++;
//		}
//		else if(c==TCHAR('"')||c==TCHAR('\''))
//		{
//			Count++;
//			Size++;
//		}
//		else{
//			Count++;
//			Size+=3;
//		}
//	}
//
//	return res;
//}

#if (SHIPPING_PC_GAME)  //GM command mode enabled
bool bUseGMCmd = true;
#else
bool bUseGMCmd = true;
#endif

//---------------------------------------------------------------------
void UMMOUIChat::SendMessage(INT TypeId,const FString& msg,UBOOL showMsg)
{
	int ChannelId = 0;
	UINT32 TargetId = 0;
	int SendServer = 0;
	UINT8 localtion = cltNormal;

	if (msg.Len() <= 0)
	{
		ShowInputArea(FALSE);
		//MMOUI::RemoveTop(this); 

		if(g_rootGFXPlayer)
		{
			g_rootGFXPlayer->eventUCClearFocus();
		}
		return;
	}

	//UBOOL bUseGMCmd = ParseParam(appCmdLine(), TEXT("GMCmd"));
	if( bUseGMCmd && msg.Left(1) == TEXT(".") )
	{
		FString gmCmd = msg.RightChop(1);
		
		/*INT idx = gmCmd.Len() - FString(TEXT("<BR>")).Len();
		if(idx > 0)
		{
			gmCmd = gmCmd.Left(idx);
		}*/

		CMessageHandler::GetInstance().Send_GMCommand( *gmCmd );
		return;
	}

	if(msg.Left(1) == TEXT("`") )
	{
		FString gmCmd = msg.RightChop(1);
		AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
		if(pc)
		{
			pc->ConsoleCommand(gmCmd);
		}
		return;
	}

	// Task #18213 add by zhangrui05
	if (MMOChatManager::getSingleton().ParseSpecialCommand(msg))
	{
		return;
	}

	FString newMsg = msg;
	//simple limit on character count
	INT msgCount = Anz_Unicode2_Utf8( newMsg, MAX_CHAT_MESSAGE_LEN - 2);
	if(msgCount<newMsg.Len())
	{
		newMsg = newMsg.Left(msgCount);
	}

	//CheckMsgInput(newMsg,msgCount,msgSize);
	//if(msgSize > 510)
	//{
	//	if(newMsg.Len()>170)
	//	{
	//		newMsg = newMsg.Left(170);
	//	}
	//}

	/*if( msg.Left(1) == TEXT("/") )
	{
		FString gmCmd = msg.RightChop(1);
		
		FString key = gmCmd.Left(3);
		
			
		if( key == MMOUI::ParseLocalizedPropertyPath("MMOGame.Chat.Team")+TEXT(" ") )
		{
			newMsg = gmCmd.RightChop(3);
			CurrentChannel = ECC_Team;
			if(ChatDropDown)
			{
				ChatDropDown->SetInt( TEXT("selectedIndex") , ECC_Team );
			}
		}
		else if( key == MMOUI::ParseLocalizedPropertyPath("MMOGame.Chat.Nearby")+TEXT(" ") )
		{
			newMsg = gmCmd.RightChop(3);
			CurrentChannel = ECC_Nearby;
			if(ChatDropDown)
			{
				ChatDropDown->SetInt( TEXT("selectedIndex") , ECC_Nearby );
			}
		}
		else if( key == MMOUI::ParseLocalizedPropertyPath("MMOGame.Chat.World")+TEXT(" ") )
		{
			newMsg = gmCmd.RightChop(3);
			CurrentChannel = ECC_World;
			if(ChatDropDown)
			{
				ChatDropDown->SetInt( TEXT("selectedIndex") , ECC_World );
			}
		}
		else if( key == MMOUI::ParseLocalizedPropertyPath("MMOGame.Chat.Guild")+TEXT(" ") )
		{
			newMsg = gmCmd.RightChop(3);
			CurrentChannel = ECC_Guild;
			if(ChatDropDown)
			{
				ChatDropDown->SetInt( TEXT("selectedIndex") , ECC_Guild );
			}
		}
		else if(key == MMOUI::ParseLocalizedPropertyPath("MMOGame.Chat.Trumpet")+TEXT(" "))
		{
			newMsg = gmCmd.RightChop(3);
			CurrentChannel = ECC_Trumpet;
			if(ChatDropDown)
			{
				ChatDropDown->SetInt( TEXT("selectedIndex") , ECC_Trumpet );
			}
		}
		

	}

	if(newMsg.Right(4) == TEXT("<br>"))
		newMsg = newMsg.LeftChop(4);*/

	if(GetChannelType(newMsg) < ECC_System)
	{
		CurrentChannel = GetChannelType(newMsg);
		newMsg = ParseSendMessage((EChat_Channel)CurrentChannel , newMsg);
		
	}
	
	//this thing has no regularity at all; some data comes from server macros, some from the player's current data. The table can't even be configured
	switch ( CurrentChannel )
	{
		case  ECC_World:
			{
				ChannelId = otChannel;
				TargetId = cctGroup;
				/*if (showMsg &&  MMOFacade::getSingleton().GetPlayerMoney64(E_MONEYTYPE_02) < WorldChannelConst)
				{
					FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errChatMoneyNotEnough"));
					MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errChatMoneyNotEnough"));
					return;
				}*/
			}
			break;
		case ECC_Guild:
			ChannelId = otGuild; //none for now, to be added later
			TargetId = MMOGuildData::getSingleton().GetGuildBaseInfo().Id;
			if( MMOGuildData::getSingleton().GetGuildBaseInfo().Id == 0 )
			{
				if (showMsg)
				{
					FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Chat.NoGuildChat"));
					MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOGame.Chat.NoGuildChat"));
				}
				MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath( "MMOGame.Chat.NoGuildChat" ) );
				return;
			}
			break;
		case ECC_Team:
			ChannelId = otTeam;
			TargetId = TeamManager::getSingleton().TeamID;
			if( TargetId  <= 0 )//not in a team
			{
				if (showMsg)
				{
					FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Chat.NoTeamChat"));
					MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOGame.Chat.NoTeamChat"));
				}
				MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath( "MMOGame.Chat.NoTeamChat" ) );
				return;
			}
			break;
		case ECC_Nearby:
			{
				ChannelId = otChannel;
				TargetId =cctScreen;
			}
			break;
		case ECC_Private:
			ChannelId = otRole;
			TargetId = 0;//cctInvalid;
			//MakeObjectID( ChannelId  , TargetId );
			break;
		case ECC_Trumpet:

			if (MMOChatManager::getSingleton().CheckTrumpetItem() == FALSE)
			{
				//Add a scrolling text prompt
				FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Chat.NoTrumpetItem"));
				MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.Chat.NoTrumpetItem"));
				return;
			}
			else if (MMOChatManager::getSingleton().GetIsInTrumpetCDTimes())
			{
				//Add a scrolling text prompt
				FString tips = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Chat.TrumpetItemCD"));
				FString error = FString::Printf(*tips,MMOChatManager::getSingleton().TrumpetCDTimes);
		
				MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,error);
				return;
			}
			ChannelId = otTrumpet;
			TargetId = cctGroup;
			localtion = cltTrumpet;
			break;
		default:
			return;
	}

	ActiveCurChatMessageFilterChannel( EChat_Channel(CurrentChannel) );


	// Guide quest
	MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_Chat);

	MMOGameEvent_Guide evtGuide(EGUET_SendChat);
	MMOUI::PostGameEvent(&evtGuide);

	if ( MMOChatManager::getSingleton().IsImgChat(newMsg) ) 
	{
		if( MMOChatManager::getSingleton().ParsePrivateChatMessage(newMsg))
		{
			SendPrivateChatMessage( newMsg );
			return;
		}
	}

	if (ChannelId == otTrumpet)
	{
		MMOChatManager::getSingleton().SetIsInTrumpetCDTimes(TRUE);
	}

	if (ChannelId == otChannel || ChannelId == otTrumpet)
	{
		/*if ( TargetId == cctScreen )
		{
			if( bChatNear == FALSE )
			{
				FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errChatTooFrequent"));
				MMOUI::GetUIManager()->delegateAddScreenMessage(text ,MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errChatTooFrequent"));
				return;
			}
			else
				bChatNear = FALSE;
		}*/

		CMessageHandler::GetInstance().Send_ChatChannel( ChannelId ,  TargetId , newMsg, localtion );

		
	}
	else
	{
		CMessageHandler::GetInstance().Send_Chat( ChannelId ,  TargetId , newMsg );
	}

}

void UMMOUIChat::SendPrivateChatMessage(FString inContent)
{
	CMessageHandler::GetInstance().Send_Chat( otRole ,  0 , inContent );
	//eventCallSetDropDownListIndex(ECC_Private);
}


void UMMOUIChat::ActiveCurChatMessageFilterChannel( EChat_Channel channel )
{
	//requirement is to first check non-default filter channels
	TArray<ChatFilterChannelMessage>& ChatFilterChannelList = MMOChatManager::getSingleton().GetChatFilterChannelList();

	if( ChatFilterChannelList( CurrentFilterChannel ).ChannelFilterType == (ChatFilterChannelList(CurrentFilterChannel).ChannelFilterType| (0x1<<channel)) )
	{
		//the current channel can display the sent content, no need to switch
		return;
	}
	
	for ( INT i = 0 ; i < ChatFilterChannelList.Num() ; i++ )
	{
		if( ChatFilterChannelList(i).IsDefaultChannel )
		{
			continue;
		}
		if( ChatFilterChannelList(i).ChannelFilterType ==  (ChatFilterChannelList(i).ChannelFilterType| (0x1<<channel))  )
		{
			TurnFilterChannel( i );
			return;
		}
	}
	//not found among non-default, then look for the default filter channel
	for ( INT i = 0 ; i < ChatFilterChannelList.Num() ; i++ )
	{
		if( !ChatFilterChannelList(i).IsDefaultChannel )
		{
			continue;
		}
		if( ChatFilterChannelList(i).ChannelFilterType ==  (ChatFilterChannelList(i).ChannelFilterType| (0x1<<channel))  )
		{
			TurnFilterChannel( i );
			return;
		}
	}
}

void UMMOUIChat::TurnFilterChannel( INT idx )
{
	UGFxObject* widget = GetObject( TEXT("TopLable_mc") );
	if( widget )
	{
		widget = widget->GetObject( FString(TEXT("channel")) + appItoa(idx) );
	}

	if(widget )
	{
		GFXPlayer->eventCallWidgetDispatchEvent( widget , TEXT("click") );
	}
}


//-----------------------------------------------------------------------------------------------------------------------------
//void UMMOUIChat::AddChatMessage(INT TypeId,const FString& MessageContent,const FString& SenderName)
//{
	//ChatMessage CM;
	//CM.ChatTypeId = TypeId;
	//CM.ChatFilterChannelMsg = MessageContent;
	//CM.ChatPlayer = SenderName;
	//ChatBuffList.AddItem(CM);
//}

//-----------------------------------------------------------------------------------------------------------------------------
//void UMMOUIChat::AddSystemMessage(const FString& MessageContent)
//{
	//AddChatMessage( ECC_System , MessageContent , TEXT("") );
//}

void UMMOUIChat::NativeInitOnce()
{
	//local array<ASValue> val;

	UMMOUIBase::NativeInitOnce();
	//ChatChannelStr.AddZeroed( ECC_Num );

	//ChatChannelStr(ECC_World) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.World") );	
	//ChatChannelStr(ECC_Team) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Team") );
	//ChatChannelStr(ECC_Guild) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Guild") );
	//ChatChannelStr(ECC_Nearby) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Nearby") );
	//ChatChannelStr(ECC_Private) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Private") );
	//ChatChannelStr(ECC_System) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.System") );
	//ChatChannelStr(ECC_Hint) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Hint") );
	//ChatChannelStr(ECC_Trumpet) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Trumpet") );

	ChatChannelColorStr.AddZeroed( ECC_Num );

	ChatChannelColorStr(ECC_World) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.WorldColor") );	
	ChatChannelColorStr(ECC_Team) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.TeamColor") );
	ChatChannelColorStr(ECC_Guild) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.GuildColor") );
	ChatChannelColorStr(ECC_Nearby) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.NearbyColor") );
	ChatChannelColorStr(ECC_Private) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.PrivateColor") );
	ChatChannelColorStr(ECC_System) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.SystemColor") );
	ChatChannelColorStr(ECC_Hint) = TEXT("%s");
	ChatChannelColorStr(ECC_Trumpet) = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.TrumpetColor") );
	

	InputArea = GetObject( TEXT("InputBlock_mc") );
	if( InputArea )
		ChatDropDown = InputArea->GetObject( TEXT("chat_dm") );

	FString datastring;

	////speaking channel, system channel is enough
	//for( INT i = 0 ; i< ECC_System ; i++ ) 
	//{
	//	if( i == 0 )
	//	{
	//		datastring = ChatChannelStr(i);
	//	}
	//	else 
	//	{
	//		datastring = datastring + TEXT("=") + ChatChannelStr(i);
	//	}
	//}

	CreateDropDownList(ChatDropDown);

	//default initial page
	//TurnFilterChannel(1);
}

void UMMOUIChat::Init()
{
	//if( GFXPlayer )
	//{
	//	UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
	//	if( data )
	//	{
	//		eventCallSetPanelText(  );
	//	}
	//}
	
	UGFxObject* popFace = GetObject( TEXT("PopFace") );
	SetPopFaceTips(popFace);
	/*if( popFace )
	{
		for ( INT i = 0 ; i < faceNum ; i++ )
		{
			UGFxObject* face = popFace->GetObject( FString( TEXT("FaceButton") ) + appItoa(i) );
			if(face)
			{
				face->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.Chat.face") ) + appItoa(i) ) );
			}
		}
		
	}*/
	ChatNew = GetObject( TEXT("newChat") );
	if (ChatNew)
	{
		UGFxObject* newChat_PopFace = ChatNew->GetObject( TEXT("PopFacePanel") );
		SetPopFaceTips(newChat_PopFace);
	}
	

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_UpdateChannelTab);
	evts.AddItem(GameEvent_SetChannelByTab);
	evts.AddItem(GameEvent_ShowPrivateTabEffect);
	evts.AddItem(GameEvent_Chat_AppendInput);
	evts.AddItem(GameEvent_Chat_UpdateDropDownList);
	evts.AddItem(GameEvent_Chat_ErrorMsg);
	EventAdapter = new TGameEventAdapter<UMMOUIChat>(this, evts);
	TrumpetCDTime = 0;
	_ProcessTime = 0;
	_ProcessTrumpetCDTime = 0;
	bLock = FALSE;
}

void UMMOUIChat::SetPopFaceTips(UGFxObject* popFace)
{
	INT faceNum = 0;
	GConfig->GetInt( TEXT("Chat"), TEXT("FaceNum") , faceNum , GGameIni );
	if( popFace )
	{
		for ( INT i = 0 ; i < faceNum ; i++ )
		{
			UGFxObject* face = popFace->GetObject( FString( TEXT("FaceButton") ) + appItoa(i) );
			if(face)
			{
				face->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.Chat.face") ) + appItoa(i) ) );
			}
		}

	}
}
//-----------------------------------------------------------------------------------------------------------------------------
void UMMOUIChat::EnterPrivateChat(const FString& playerName)
{
	//if(ChatDropDown)
	//{
		//ChatDropDown->SetInt( TEXT("selectedIndex") , ECC_Private );
		
	//}

	
	TArray<struct FASValue> Arguments;
	FASValue ASValue(EC_EventParm);
	ASValue.S = FString::Printf( TEXT("/%s ") , *playerName );
	ASValue.Type = AS_String;
	Arguments.AddItem(ASValue);

	/*if(InputArea)
	{
		InputArea->GetObject("NewInput_mc")->GetObject("textField")->SetString( "htmlText" , ASValue.S );
	}*/

	//eventCallAppendInput(ASValue.S);
	
	//eventCallSetInputTextFocus();

	//AMMOPlayerController* Ctrl = MMOFacade::getSingleton().GetMainController();

	ShowInputArea(TRUE);

	//TArray<struct FASValue> Args;
	//Invoke(TEXT("SetTextInputFocus"),Args);
	//FGFxEngine::GetEngine()->SetFocusMovie(GFXPlayer->pMovie);
	//MMOUI::MoveToTop(this);

// 	extern bool g_isInputChat;
// 	g_isInputChat = TRUE;
	//Ctrl->SetInTextInput(TRUE);
	//MMOUI::MoveToTop(this);
	eventCallSetPrivateChatName(ASValue.S);
	
	
}

UBOOL UMMOUIChat::OnEsc()
{

	ShowInputArea(FALSE);
	//MMOUI::RemoveTop(this); 

	return FALSE;

}

//-----------------------------------------------------------------------------------------------------------------------------
void UMMOUIChat::EnterScene()
{
	//clean up once on entering
	//ChatBuffList.Empty();

	//for ( INT i = 0 ; i < ChatFilterChannelList.Num() ; i++ )
	//{
	//	ChatFilterChannelList(i).ChatMessageList.Empty();
	//}
	//UpdateShowTextArea();

}

//
void UMMOUIChat::ClearUI()
{
	MMOChatManager::getSingleton().ClearAll();
	eventCallClearChatMsg();
	//UpdateShowTextArea();
	TurnFilterChannel(0);
}

//-----------------------------------------------------------------------------------------------------------------------------
extern UBOOL g_bSyncPlayerMoney;
void UMMOUIChat::PlayerMoneyChange(BYTE MoneyType,INT DeltaMoney)
{
	if (g_bSyncPlayerMoney)
	{
		return;
	}

	if(MMO::GetCurrentGameState() != EGameState_InGame)
	{
		//do not update display when not in game
		return;
	}

	FString sMsg;
	FString sMoneyType;

	switch(MoneyType)
	{
	case MONEYTYPE_01:
	case MONEYTYPE_02:
		sMoneyType = TEXT("");
		break;
	default:
		sMoneyType = UITextParser::HTMLGetCurrencyName( (MONEY_TYPE)MoneyType );
		break;
	}

	if( MoneyType <= MONEYTYPE_02 )
	{
		if( DeltaMoney>0 )
		{
			sMsg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.GetMoney") ) + UITextParser::HTMLMoneyIconString( MoneyType , DeltaMoney );

			MMOChatManager::getSingleton().AddSystemMessage( sMsg );

			MMOFacade::getSingleton().PlayUISound(TEXT("Coin_get"));
		}
		else if( DeltaMoney<0 )
		{
			sMsg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.LostMoney") ) + UITextParser::HTMLMoneyIconString( MoneyType , -DeltaMoney );

			MMOChatManager::getSingleton().AddSystemMessage( sMsg );

			MMOFacade::getSingleton().PlayUISound(TEXT("Coin_pay"));
		}
	}
	else
	{
		INT money = Abs(DeltaMoney);

		if (MoneyType == MONEYTYPE_MORAL || MoneyType == MONEYTYPE_MASTER_EXP)
		{
			if (money > 0)
				money = money / 1000;

			if (money <= 0)
				return;

		}

		if( DeltaMoney>0 )
		{
			
			sMsg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.GetSomething") );

			MMOChatManager::getSingleton().AddSystemMessage( FString::Printf( *sMsg, money , *sMoneyType ));

			MMOFacade::getSingleton().PlayUISound(TEXT("Coin_get"));
		}
		else if( DeltaMoney<0 )
		{
			sMsg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.LostSomething") );

			MMOChatManager::getSingleton().AddSystemMessage( FString::Printf( *sMsg, money , *sMoneyType ));

			MMOFacade::getSingleton().PlayUISound(TEXT("Coin_pay"));
		}
	}

}

//-----------------------------------------------------------------------------------------------------------------------------
void UMMOUIChat::TickTime(FLOAT tickTime)
{
	_DeltaTime += tickTime;
	

	if( _DeltaTime > 0.5 )
	{
		_DeltaTime = 0;
		ProcessChatMsgBuff();
	}

	/*if (bChatNear == FALSE)
	{
		_ChatNearProcessTime += tickTime;
		if (_ChatNearProcessTime > MMOChatManager::getSingleton().GetChatNearCD())
		{
			_ChatNearProcessTime = 0;
			bChatNear = TRUE;
		}
	}*/
	

	ProcessTrumpet(tickTime);
	processTrumpetCDTimes(tickTime);

}


//-----------------------------------------------------------------------------------------------------------------------------
void UMMOUIChat::ProcessChatMsgBuff()
{
	INT ObjectIndex = 0;
	UGFxObject* DataProvider = NULL;
	

	TArray<ChatMessage>& ChatBuffList = MMOChatManager::getSingleton().GetChatBuffList();
	TArray<ChatFilterChannelMessage>& ChatFilterChannelList = MMOChatManager::getSingleton().GetChatFilterChannelList();

	UMMOUISceneHint *pSceneHint = (UMMOUISceneHint*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_SceneHint );
	MMOFacade *pFacade = &MMOFacade::getSingleton();
	if( ChatBuffList.Num() > 0 )
	{
		for ( INT k = 0 ; k < ChatBuffList.Num() ; k++ )
		{
			INT TypeId = ChatBuffList(k).ChatTypeId;
			INT chatActorId = ChatBuffList(k).ChatActorID;

			for ( INT i = 0 ; i < ChatFilterChannelList.Num() ; i++ )
			{
				if( ChatFilterChannelList(i).ChannelFilterType ==  (ChatFilterChannelList(i).ChannelFilterType| (0x1<<TypeId))  
					|| TypeId == ECC_Trumpet_World)
				{
					ChatMessage CM;
					CM.ChatFilterChannelMsg = ChatBuffList(k).ChatFilterChannelMsg;
					CM.ChatPlayer = ChatBuffList(k).ChatPlayer;
					CM.ChatTypeId = TypeId;
					CM.ChatActorID = ChatBuffList(k).ChatActorID;
					CM.VipType = ChatBuffList(k).VipType;
					CM.bGM = ChatBuffList(k).bGM;
					CM.isMe = ChatBuffList(k).isMe;
					ChatFilterChannelList(i).ChatMessageList.AddItem( CM );
					if( ChatFilterChannelList(i).ChatMessageList.Num() > MsgLimit)
					{
						ChatFilterChannelList(i).ChatMessageList.Remove( 0 );
					}
					//messages in General must be added to the Hint channel
					//if (TypeId == ECC_System)
					//{
						//MMOChatManager::getSingleton().AddHintItem(CM);
					//}

					if(CurrentFilterChannel == i)//messages the current channel can display add by JimZhang@snda.com
					{
						if(DataProvider==NULL)
						{
							DataProvider=GFXPlayer->CreateArray();
						}

						UGFxObject* ChatObj = MMOChatManager::getSingleton().GreateChatObject(CM ,GFXPlayer);
						if(ChatObj && DataProvider)
						{
							DataProvider->SetElementObject(ObjectIndex++, ChatObj);
						}
					}
				}

			}

			//needs bubble display
			AMMOActor* actor = pFacade->GetCharacter( chatActorId );
			if( actor != NULL && pSceneHint != NULL ){
				EActorClassType actorType = (EActorClassType)actor->GetActorType();
				if( TypeId == ECC_Nearby && ( actorType == EACT_OtherPlayer || actorType == EACT_PlayerSelf ) ){
					pSceneHint->AddActorTalkBubble( chatActorId, ChatBuffList(k).ChatFilterChannelMsg );
				}
				else if( actorType == EACT_NPC || actorType == EACT_Monster || actorType == EACT_Boss ){
					pSceneHint->AddActorTalkBubble( chatActorId, ChatBuffList(k).ChatFilterChannelMsg );
				}
			}
		}

		if(ObjectIndex>0)
		{
			eventCallAddChatMsg(DataProvider);
		}

		ChatBuffList.Empty();

		//UpdateShowTextArea();

	}
	
}

void UMMOUIChat::ProcessChatTrumpet()
{

	eventCallUpdateTrumpetMsg(GetTrumpet(),TRUE,GetTrumpetByGm());
	MMOChatManager::getSingleton().RemoveTrumpet();
}
//-----------------------------------------------------------------------------------------------------------------------------
//void UMMOUIChat::NativeReceiveMessage(int ChannelId,INT TargetId,const FString& MessageContent,const FString& SenderName )
//{
//	int TypeId = 0;
//	if( ChannelId == otChannel  )
//	{
//		if( TargetId ==  cctServer )
//		{
//			TypeId = ECC_World;
//		}
//		else if(  TargetId ==  cctScreen )
//		{
//			TypeId = ECC_Nearby;
//		}
//	}
//	else if( ChannelId == otTeam )
//		TypeId = ECC_Team;
//	else if( ChannelId == otGuild )
//		TypeId = ECC_Guild;
//	else if( ChannelId == otRole )
//		TypeId = ECC_Private;
//
//	AddChatMessage( TypeId , MessageContent , SenderName );
//
//
//}

//------------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIChat::AddDefaultFilterChannel()
{
	//MMOChatManager::getSingleton().AddDefaultFilterChannel();
	//MMOChatManager::getSingleton().LoadRecord();
}

//----------------------------------------------------------------------------------------------------------------------------
void UMMOUIChat::SetFilterChannel( INT pos , const FString& ChannelName )
{
	//FChatFilterChannelMessage FilterChannelMsg(EC_EventParm);
	//FilterChannelMsg.ChannelFilterType = FilterChannelType;
	//FilterChannelMsg.IsDefaultChannel = defaultChannel;
	//FilterChannelMsg.ChannelName = ChannelName;
	//ChatFilterChannelList.AddItem( FilterChannelMsg );

	TArray<struct FASValue> Arguments;
	FASValue ASValue(EC_EventParm);

	ASValue.I= pos;
	ASValue.Type = AS_Int;
	Arguments.AddItem(ASValue);

	ASValue.S= ChannelName;
	ASValue.Type = AS_String;
	Arguments.AddItem(ASValue);

	Invoke( "SetFilterChannel" , Arguments );

}

void UMMOUIChat::SetFilterChannelNum(INT num)
{
	TArray<struct FASValue> Arguments;
	FASValue ASValue(EC_EventParm);

	ASValue.I= num;
	ASValue.Type = AS_Int;
	Arguments.AddItem(ASValue);
	
	Invoke( "SetFilterChannelNum" , Arguments );
}

//----------------------------------------------------------------------------------------------------------------------------
void UMMOUIChat::ShowInputArea(UBOOL inShow)
{
	if (inShow)
	{
		MMOUI::MoveToTop(this);
	}
	else
	{
		if (ChatNew && !ChatNew->GetBool("visible"))
		{
			MMOUI::RemoveTop(this); 
		}
		
	}
	if(InputArea)
	{
		if(inShow)
		{
			if (bLock)
			{
				MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Lock") )  );		
				return;
			}
			else
			{
				FSMIO::getSingleton().Ctrl.SetInputText();
				FSMIO::getSingleton().Target.SetInputText();
				//Show(TRUE);

			}
		}
		
		//InputArea->SetVisible(inShow);
		this->eventCallShowChatBackBg(inShow);

		MMOFacade::getSingleton().GetMainController()->SetInTextInput(inShow);

		//MMOChatManager::getSingleton().SetFocus(inShow);
	}
}

void UMMOUIChat::ChannelTab( INT id )
{
	CurrentFilterChannel = id;
	UpdateShowTextArea();
	RefreshChatChannel();

	if (id == 0)
	{
		MMOGameEvent_Guide evtGuide(EGUET_ChooseChatChanel);
		MMOUI::PostGameEvent(&evtGuide);
	}
}
//{
//	CurrentFilterChannel = id;
//	UpdateShowTextArea();
//}


//---------------------------------------------------------------------------------------------------------------------------
void UMMOUIChat::RefreshChatChannel()
{
	TArray<ChatFilterChannelMessage>& ChatFilterChannelList = MMOChatManager::getSingleton().GetChatFilterChannelList();
	if( ChatFilterChannelList.Num() <= CurrentFilterChannel  )
		return;

	UGFxObject* DataProvider =GFXPlayer->CreateArray();
	INT ObjectIndex = 0;

	for (int i = 0 ;i < ChatFilterChannelList( CurrentFilterChannel ).ChatMessageList.Num() ; i++)
	{
		ChatMessage& CM = ChatFilterChannelList( CurrentFilterChannel ).ChatMessageList(i);

		UGFxObject* ChatObj = MMOChatManager::getSingleton().GreateChatObject(CM ,GFXPlayer);
		if(ChatObj && DataProvider)
		{
			DataProvider->SetElementObject(ObjectIndex++, ChatObj);
		}
	}

	//refresh data
	if(DataProvider){
		eventCallClearChatMsg();
		eventCallAddChatMsg(DataProvider);
	}
}


//----------------------------------------------------------------------------------------------------------------------------
void UMMOUIChat::UpdateShowTextArea()
{
	TArray<ChatFilterChannelMessage>& ChatFilterChannelList = MMOChatManager::getSingleton().GetChatFilterChannelList();

	if( ChatFilterChannelList.Num() <= CurrentFilterChannel  )
		return;
	FString MsgList;
	MsgList.Empty();
	

	for (int i = 0 ;i < ChatFilterChannelList( CurrentFilterChannel ).ChatMessageList.Num() ; i++)
	{
		ChatMessage& CM = ChatFilterChannelList( CurrentFilterChannel ).ChatMessageList(i);
		FString ColorString;
		FString ChannelIconPath;
		switch( CM.ChatTypeId )
		{
		case ECC_World:
			ColorString = MMOChatManager::getSingleton().TotalChannelFontColorString;
			ChannelIconPath = MMOChatManager::getSingleton().WorldChannelIconPath;
			break;
		case ECC_Nearby:
			ColorString = MMOChatManager::getSingleton().NearChannelFontColorString;
			ChannelIconPath = MMOChatManager::getSingleton().NearChannelIconPath;
			break;
		case ECC_Private:
			ColorString = MMOChatManager::getSingleton().PrivateChannelFontColorString;
			ChannelIconPath = MMOChatManager::getSingleton().PrivateChannelIconPath;
			break;
		case ECC_Team:
			ColorString = MMOChatManager::getSingleton().TeamChannelFontColorString;
			ChannelIconPath = MMOChatManager::getSingleton().TeamChannelIconPath;
			break;
		case ECC_System:
		case ECC_Hint:
			ColorString = MMOChatManager::getSingleton().SystemChannelFontColorString;
			ChannelIconPath = MMOChatManager::getSingleton().SystemChannelIconPath;
			break;
		case ECC_Guild:
			ColorString = MMOChatManager::getSingleton().GuildChannelFontColorString;
			ChannelIconPath = MMOChatManager::getSingleton().GuildChannelIconPath;
			break;
		case ECC_Trumpet_World:
			ColorString = MMOChatManager::getSingleton().TrumpetChannelFontColorString;
			ChannelIconPath = MMOChatManager::getSingleton().LabaIconPath;
			break;
		case ECC_Trumpet:
			ColorString = MMOChatManager::getSingleton().TrumpetChannelFontColorString;
			ChannelIconPath = MMOChatManager::getSingleton().TrumpetChannelIconPath;
			break;
		default:
			ColorString = MMOChatManager::getSingleton().TotalChannelFontColorString;
			ChannelIconPath = MMOChatManager::getSingleton().WorldChannelIconPath;
			break;
		}
		FString vipIconPath = MMOChatManager::getSingleton().getVipIconPath(CM.VipType);
		FString MsgList = TEXT("<img src='img://");
		MsgList += ChannelIconPath + TEXT("'/>"); 

		if (!vipIconPath.IsEmpty())
		{
			MsgList += vipIconPath;
		}
		if( CM.ChatPlayer.IsEmpty() )
		{
			MsgList = MsgList + FString::Printf( TEXT("<FONT COLOR='%s'> ") ,*ColorString ) + 
				CM.ChatFilterChannelMsg  + TEXT("</FONT>");
		}
		else
		{
			MsgList = MsgList + FString::Printf( TEXT("<FONT COLOR='%s'> ") ,*ColorString ) + 
				FString::Printf( TEXT("%s : %s ") , *CM.ChatPlayer  , *CM.ChatFilterChannelMsg ) + TEXT("</FONT>");
		}
		
	}

#if 1
	//MsgList += TEXT("<img src='img://UI_Quest.QuestIcon_Completed'>");
	eventCallAppendChatMsg(MsgList);
#else
	TArray<struct FASValue> Arguments;
	FASValue ASValue(EC_EventParm);
	ASValue.S = MsgList;
	ASValue.Type = AS_String;
	Arguments.AddItem(ASValue);

	GetObject(TEXT("TextShowArea_mc"))->Invoke( "NewAppendHtml" , Arguments );
#endif

}


//========================================================================================================================================
void UMMOUIChat::OnFocusGained()
{
	UMMOUIBase::OnFocusGained();
	TArray<struct FASValue> Args;
	Invoke(TEXT("SetTextInputFocus"),Args);	
}

//
void UMMOUIChat::OnFocusLost()
{


	UMMOUIBase::OnFocusLost();

	AMMOPlayerController* Ctrl = MMOFacade::getSingleton().GetMainController();
	if(Ctrl){
		Ctrl->FocusOnInput(this);
	}

}

UBOOL UMMOUIChat::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	if (key == TEXT("LeftMouseButton") || key == TEXT("RightMouseButton"))
	{
		//`log("___________CaptureInputKey:" @ strHoverControllerName );
 		if (strHoverControllerName == "listItem_tf" && g_rootGFXPlayer && g_rootGFXPlayer->GetlockMouseCursorForUI()==false)
 		{
 			return TRUE;
 		}
		if (strHoverControllerName == "ChatBg_mc" )
		{
			return TRUE;
		}
		if (strHoverControllerName == "AlphaBk_mc" )
		{
			return TRUE;
		}
	}

	//if ( Chat != none)
	//{
	//	Chat.SkipCaptureInputKey( key, eIE, strHoverControllerName, ClickPosInMovieSpace);
	//}

	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}

INT UMMOUIChat::GetSlotType()
{
	return ESPType_ChatInput;
}

UBOOL UMMOUIChat::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(SlotType == ESPType_Bag /*|| SlotType == ESPType_AuctionItem*/)
	{
		if(MMOFacade::getSingleton().GetMainController()->PlayerInput->IsCtrlPressed())
		{
			//const UItem* item = MMOFacade::getSingleton().FindItemTemplate(ContainerID, SlotID);
			AppendInpt(SlotType,ContainerID,SlotID,bDefault);
			return TRUE;
		}
	}
	return FALSE;
}

void UMMOUIChat::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	//check(NULL != evt);
	if (NULL != evt)
	{
		switch(evt->GetEventType())
		{
		case  GameEvent_UpdateChannelTab:
			{
				this->eventCallUpdateChannelTab(MMOChatManager::getSingleton().GetChatFilterChannelListNum());
			}
			break;
		case GameEvent_SetChannelByTab:
			{
				eventCallChangeChannelByTab();
			}
			break;
		case GameEvent_ShowPrivateTabEffect:
			{
				eventCallShowPrivateTabEffect(ECC_Private);
			}
			break;

		case GameEvent_Chat_AppendInput:
			{
				MMOGameEvent_Chat_AppendInput* e = (MMOGameEvent_Chat_AppendInput*)evt;
				if (e->SlotType == ESPType_Bag || e->SlotType == ESPType_Charactor || e->SlotType == ESPType_AuctionItem)
				{
					AppendInpt(e->SlotType,e->ContainerID,e->SlotID,e->bDefault);
				}
				
			}
			break;
		case GameEvent_Chat_UpdateDropDownList:
				UpdateChatDropDownList();
			break;
		case GameEvent_Chat_ErrorMsg:
			{
				MMOGameEvent_Chat_ShowChatErrorMsg*e = (MMOGameEvent_Chat_ShowChatErrorMsg*)evt;
				if (e)
				{
					ShowChatErrorMsg(e->errMsg);
				}
			}
			break;
		}
	}
}

void UMMOUIChat::ChatChannelChange( int idx )
{
	CurrentChannel = idx;

	ShowTrumpet();

	MMOUI::MoveToTop(this);
}

void UMMOUIChat::ShowTrumpet()
{
	if(CurrentChannel == ECC_Trumpet)
	{
		//check whether there is a megaphone item
		BOOL isShow = MMOChatManager::getSingleton().CheckTrumpetItem();
		eventCallShowTrumpet(isShow);

	}
}

BOOL UMMOUIChat::GetTrumpetByGm()
{
	BOOL bGm = FALSE;

	TArray<ChatMessage>& ChatTrumpetList = MMOChatManager::getSingleton().GetChatTrumpetList();
	if (ChatTrumpetList.Num() > 0)
	{
		ChatMessage& CM = ChatTrumpetList( 0 );
		if (CM.bGM > 0)
		{
			bGm = TRUE;
		}
	}

	return bGm;
}

FString UMMOUIChat::GetTrumpet()
{
	FString MsgList;
	MsgList.Empty();
	TArray<ChatMessage>& ChatTrumpetList = MMOChatManager::getSingleton().GetChatTrumpetList();
	if (ChatTrumpetList.Num() > 0)
	{
		ChatMessage& CM = ChatTrumpetList( 0 );
		FString ColorString;
		FString ChannelIconPath;
		switch( CM.ChatTypeId )
		{
		case ECC_Trumpet:
			ColorString = MMOChatManager::getSingleton().TrumpetChannelFontColorString;
			ChannelIconPath = MMOChatManager::getSingleton().TrumpetChannelIconPath;
			break;
		default:
			ColorString = MMOChatManager::getSingleton().TrumpetChannelFontColorString;
			ChannelIconPath = MMOChatManager::getSingleton().TrumpetChannelIconPath;
			break;
		}
		
		/*MsgList = TEXT("<img src='img://");
		MsgList += ChannelIconPath + TEXT("'/>");*/ 

		if( CM.ChatPlayer.IsEmpty() )
		{
			MsgList = MsgList + FString::Printf( TEXT("<FONT COLOR='%s'> ") ,*ColorString ) + 
				CM.ChatFilterChannelMsg  + TEXT("</FONT>");
		}
		else
		{
			if (CM.bGM > 0)
			{
				MsgList = MsgList + FString::Printf( TEXT("<FONT COLOR='%s'> ") ,GM_CHAT_COLOR ) + 
					FString::Printf( TEXT("%s ")  , *CM.ChatFilterChannelMsg ) + TEXT("</FONT>");
			}
			else
			{
				MsgList = MsgList + FString::Printf( TEXT("<FONT COLOR='%s'> ") ,*ColorString ) + 
					FString::Printf( TEXT("%s : %s ") , *CM.ChatPlayer  , *CM.ChatFilterChannelMsg ) + TEXT("</FONT>");
			}
			
		}

	}
	return MsgList;
}

void UMMOUIChat::ProcessTrumpet(FLOAT tickTime)
{
	if (MMOChatManager::getSingleton().GetShowTrumpet())
	{
		TrumpetCDTime = MMOChatManager::getSingleton().GetChatTrumpetList().Num() > 1?MMOChatManager::getSingleton().GetTrumpetTimes():MMOChatManager::getSingleton().GetTrumpetTime();

		if (LastTrumpet != MMOChatManager::getSingleton().GetCurrentTrumpet())
		{
			//FString currentTrumoet = MMOChatManager::getSingleton().GetCurrentTrumpet();
			//eventCallUpdateTrumpetMsg(CreateTrumpet(currentTrumoet),TRUE);
			eventCallUpdateTrumpetMsg(GetTrumpet(),TRUE,GetTrumpetByGm());
			LastTrumpet = MMOChatManager::getSingleton().GetCurrentTrumpet();
			MMOChatManager::getSingleton().AddWorldTrumpetMessage();
		}
		if (_ProcessTime >= TrumpetCDTime)
		{
			eventCallUpdateTrumpetMsg("",FALSE,GetTrumpetByGm());
			ResetTrumpet();
		}

		_ProcessTime += tickTime;
	}
}


void UMMOUIChat::processTrumpetCDTimes(FLOAT tickTime)
{
	if (MMOChatManager::getSingleton().GetIsInTrumpetCDTimes())
	{
		_ProcessTrumpetCDTime += tickTime;
		if (_ProcessTrumpetCDTime >= MMOChatManager::getSingleton().GetInTrumpetCDTimes())
		{
			_ProcessTrumpetCDTime = 0;
			MMOChatManager::getSingleton().SetIsInTrumpetCDTimes(FALSE);
		}
	}
	
}
void UMMOUIChat::ResetTrumpet()
{
	TrumpetCDTime = 0;
	_ProcessTime = 0;
	LastTrumpet = TEXT("");
	if(MMOChatManager::getSingleton().GetChatTrumpetList().Num() <= 0)
		MMOChatManager::getSingleton().SetShowTrumpet(FALSE);
	MMOChatManager::getSingleton().RemoveTrumpet();
}

FString UMMOUIChat::CreateTrumpet(FString msg)
{
	FString ColorString = MMOChatManager::getSingleton().TrumpetChannelFontColorString;
	FString ChannelIconPath = MMOChatManager::getSingleton().TrumpetChannelIconPath;

	FString MsgList = TEXT("<img src='img://");
	MsgList += ChannelIconPath + TEXT("'/>"); 

	MsgList = MsgList + FString::Printf( TEXT("<FONT COLOR='%s'> ") ,*ColorString ) + 
			msg  + TEXT("</FONT><br>");
	return MsgList;

}

void UMMOUIChat::CreateDropDownList(class UGFxObject* ChatDropDown)
{
	if( ChatDropDown )
	{
		TArray<FASValue> val;

		UGFxObject* dataProvider = GFXPlayer->CreateArray();
		UGFxObject* data = GFXPlayer->CreateArray();
		if(dataProvider){
			int nIndex = 0;
			for( INT i = 0 ; i< ECC_Num ; i++ )
			{
				FString ChannelStr = *ChatChannelStr(i);
				FString ChannelColor = *ChatChannelColorStr(i);

				if (i == ECC_System || i == ECC_Hint)
				{
					continue;
				}

				dataProvider->SetElementString(nIndex,  FString::Printf( *ChatChannelColorStr(i),  *ChatChannelStr(i))	);
				data->SetElementInt(nIndex,i);
				nIndex++;
			}
			//ChatDropDown->SetObject( TEXT("dataList") , dataProvider );
		}
		//ChatDropDown->Invoke( TEXT("SplitData") , val );
		//ChatDropDown->SetInt( TEXT("selectedIndex") , 0 );

		eventcallSetDropDownList(data);
		eventcallSetDropDownListData(dataProvider);
	}
}

EChat_Channel UMMOUIChat::GetChannelType(FString context)
{
	if( context.Left(1) == TEXT("/") && MMOChatManager::getSingleton().IsImgChat(context))
	{
		return ECC_Private;
	}
	else if(context.Left(2) == TEXT("!!"))
	{
		return ECC_Team;
	}
	else if(context.Left(2) == TEXT("!~"))
	{
		return ECC_Guild;;
	}
	else if (context.Left(1) == TEXT("!"))
	{
		return ECC_Nearby;
	}
	return ECC_MAX;;
	
}
FString UMMOUIChat::ParseSendMessage(EChat_Channel type , FString content)
{
	FString contentString = content;
	switch(type)
	{
	case ECC_Nearby:
		contentString = content.RightChop(1);
		break;
	case ECC_Team:
		contentString = content.RightChop(2);
		break;
	case ECC_Guild:
		contentString = content.RightChop(2);
		break;
	}
	return contentString;
}

void UMMOUIChat::AppendInpt(BYTE type, BYTE ContainerID, INT SlotID, UBOOL def)
{
	//ShowInputArea(TRUE);

	FClientItemInfo* info = NULL;

	if (type == ESPType_AuctionItem)
	{
		info = (FClientItemInfo*)MMOAuctionManager::getSingleton().GetAuctionItemInfo(SlotID);
		info->AuctionId = SlotID;
	}
	else
	{
		info = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
	}
		
	if(info)
	{
		UItem* item = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);
		if(item)
		{
			MMOUI::MoveToTop(this);
			MMOFacade::getSingleton().GetMainController()->SetInTextInput(TRUE);

			FString txt = "";
			if (type == ESPType_Bag)
			{
				txt = LinkManager::HtmlLink_ColorItem(item, info,LIT_CHATITEM);
			}
			else if(type == ESPType_AuctionItem)
			{
				txt = LinkManager::HtmlLink_ColorItem(item, info,LIT_AHITEM);
			}
			
			eventCallAppendInput(txt);
		}
	}
}

void UMMOUIChat::CleanChnnelChat()
{
	MMOChatManager::getSingleton().CleanChannelChat(CurrentFilterChannel);

	//RefreshChatChannel();
}
//---------moved--------------
void UMMOUIChat::ASOnPanelClose(const FString& PanelName)
{
	// on ESC remove own focus, basically yielding to the Skillbar
	if(PanelName == TEXT("Chat"))
	{
		UMMOUIBase* PanelSkilbar = MMOUI::FindUIPanel(TEXT("Skillbar"));

		if(PanelSkilbar)
		{
			UMMOUIChat* chatPanel = (UMMOUIChat*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Chat);
			if (chatPanel)
			{
				MMOUI::RemoveTop(chatPanel);
			}
			
		}

		//ShowInputArea(FALSE);

	}
	else if( PanelName == MMOUI::GetUIManager()->Panel_Chat_ChatOption.ToString() )
	{
		ASOnPanelClose(PanelName);
	}
}

//------------------------------------------------------------------------------------------------------------------------
void UMMOUIChat::ASFocusInTextInput(UBOOL In)
{
	AMMOPlayerController* Ctrl = MMOFacade::getSingleton().GetMainController();

	//Super::ASFocusInTextInput(In);
	if (Ctrl)
	{
		if( In)
		{
			UMMOUIChat* chatPanel = (UMMOUIChat*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Chat);
			if (chatPanel)
			{
				Ctrl->FocusOnInput(chatPanel);
			}
		}
		else
		{
			Ctrl->FocusOutInput();
		}
	}
	
}

//------------------------------------------------------------------------------------------------------------------------
void UMMOUIChat::PostAdvance(FLOAT inDeltaTime )
{
	UMMOUIChat* chatPanel = (UMMOUIChat*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Chat);

	if( inDeltaTime >= 0.0 && chatPanel && chatPanel->IsVisible )
	{
		TickTime(inDeltaTime);
	}
}

void UMMOUIChat::ASOnChatEnter()
{

	//Chat->ShowInputArea(TRUE);

	//TArray<struct FASValue> Args;

	//Chat->Invoke(TEXT("SetTextInputFocus"),Args);

}

void UMMOUIChat::ASonRollChatPanel(UBOOL focus)
{
	//MMOChatManager::getSingleton().SetFocus(focus);
}

void UMMOUIChat:: ASChatShowTips(class UGFxObject *mc, BYTE type)
{
	if( mc == NULL ){
		return;
	}
	switch(type)
	{
	case CTT_CHAT:
		mc->SetString( TEXT("tipsHtml"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Chat.Hide") ) );
		break;
	case CTT_CHAT_CLEAN:
		mc->SetString( TEXT("tipsHtml"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Clean") ) );
		break;
	}

	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}

void UMMOUIChat::ASChatHideTips()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOUIChat::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	
}

void UMMOUIChat::_onShowChatInput(UBOOL show)
{
	MMOFacade::getSingleton().GetMainController()->SetInTextInput(show);
}

//=========================================================
void UMMOUIChat::ASTimeEvent( INT eventType )
{
	const UINT32 hint30MinLeft = 60 * 30;
	const UINT32 hint5MinLeft = 60 * 5;
	const INT diffThreshold = 15;
	UINT32 leftSec = MMOVipManager::getSingleton().GetMyLeftGameTimeInSec();
	if( leftSec <= hint30MinLeft ){
		FString hintMsgFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Recharge.LittleGameTimeHint") );
		//FString timeStr = appItoa( ( leftSec + diffThreshold + 1 ) / 60 );
		//timeStr += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Time.CountDown5") );

		FString hintMsg = FString::Printf( *hintMsgFormat, ( leftSec + diffThreshold + 1 ) / 60 );
		
		MMOUI::GetUIManager()->delegateAddScreenMessage( hintMsg, MsgInfoType_Table, TEXT("MMOGame.Recharge.LittleGameTimeHint") );
		MMOChatManager::getSingleton().AddSystemMessage( hintMsg );
		
		if( eventType == eGameTimeHintType_init ){
			INT newEventType = ( leftSec <= hint5MinLeft ? eGameTimeHintType_5 : eGameTimeHintType_30 );
			INT32 timeInterval = ( leftSec <= hint5MinLeft ? 60 : 60 * 5 );

			eventCallASSetTimeEvent( newEventType, leftSec, timeInterval );
		}

		INT timeDiff = leftSec % 60;
		if( timeDiff >= diffThreshold ){
			INT32 timeInterval = ( leftSec <= hint5MinLeft ? ( leftSec % 60 ) : leftSec % ( 60 * 5 ) );

			eventCallASSetTimeEvent( eGameTimeHintType_init, leftSec, timeInterval );
		}
	}
	else if( leftSec > 60 * 45 ){
		//no longer need prompt
		eventCallASSetTimeEvent( eGameTimeHintType_init, 0, 60 );
	}
}

void UMMOUIChat::ASSendChat( const FString& TextContent, UBOOL showMsg)
{
	SendMessage(0,TextContent,showMsg);
	ShowInputArea(FALSE);
}


void UMMOUIChat::UpdateChatDropDownList()
{
	UGFxObject* data = GFXPlayer->CreateArray();
	if(data){
		int nIndex = 0;
		TArray<ChatFilterChannelMessage>& filterChannelList = MMOChatManager::getSingleton().GetChatFilterChannelList();
		ChatFilterChannelMessage& ChatFilterChannel = filterChannelList(nIndex);

		for ( INT i = 0 ; i < ECC_Num-1 ; i++ )
		{
			if( ( ChatFilterChannel.ChannelFilterType | (0x1<<i) ) == ChatFilterChannel.ChannelFilterType )
			{
				data->SetElementBool(i,TRUE);
			}
			else
			{
				data->SetElementBool(i,FALSE);
			}
		}

		eventcallSetDropDownList(data);
	}
}

void UMMOUIChat::ASChatOptionClickFilter(INT id,UBOOL sel)
{
	TArray<ChatFilterChannelMessage>& filterChannelList = MMOChatManager::getSingleton().GetChatFilterChannelList();

	int nIndex = 0;

	if( sel )
	{
		filterChannelList(nIndex).ChannelFilterType |= ( 0x1<<id );

		if (id == ECC_Private)
		{
			MMOChatManager::getSingleton().AddPrivateMsgToBufList();
		}
	}
	else
	{
		filterChannelList(nIndex).ChannelFilterType &= ~( 0x1<<id );
	}
}

void UMMOUIChat::ASChatLockChat(UBOOL lock)
{
	bLock = lock;
}

void UMMOUIChat::checkInput()
{
	if ( InputArea->GetObject(TEXT("NewInput_mc"))->GetText().Len() <= 0)
		ShowInputArea(FALSE);
}

void  UMMOUIChat::ASChatItemShow(UBOOL showStr)
{
	if (showStr)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.CanNotSendItem") ) , MsgInfoType_Table , TEXT("MMOGame.Chat.CanNotSendItem") );
	}
}

void UMMOUIChat::ASRequestNewChat(UBOOL v)
{
	if (v)
	{
		MMOUI::MoveToTop(this);
	}
	else
	{
		MMOUI::RemoveTop(this);
	}
	MMOChatManager::getSingleton().SetNewChat(v);
}

void UMMOUIChat::ShowChatErrorMsg(FString msg)
{
	FString error = MMOUI::ParseLocalizedPropertyPath(msg);

	if (ChatNew && ChatNew->GetBool("visible"))
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,msg);
	}
	else
	{
		MMOChatManager::getSingleton().AddSystemMessage(error);
	}
}