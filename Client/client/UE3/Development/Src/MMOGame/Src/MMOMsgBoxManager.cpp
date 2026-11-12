#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMsgBoxManager.h"
#include "CommonGameEvents.h"
#include "MMOPerformance.h"

IMPLEMENT_CLASS(UMMOUIMessageBoxZone)

extern UBOOL g_focusInTextInput;

UGFxObject* UMMOUIMessageBoxZone::CreateAutoAdjustFunction()
{
	if(_ConfirmBox)
		return NULL;
	 
	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT( "AutoAdjustSystemSettingOk" );

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT( "AutoAdjustSystemSettingCancel" );

	FString strTitle = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Setting.AutoAdjustSettingHint"));
	FString strContent = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Setting.AutoAdjustSettingDescrible")),GSystemSettings.LowerCheckFPS);

	_ConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBoxWithCheck(strTitle,0,strContent,funYes,funNo,TRUE);

	FASValue v1(EC_EventParm);
	v1.B = FALSE;
	v1.Type = AS_Boolean;
	TArray<FASValue> asArgs;
	asArgs.AddItem(v1);
	
	_ConfirmBox->Invoke(TEXT("UCShowCloseBtn"), asArgs);

	return _ConfirmBox;
}


UGFxObject* UMMOUIMessageBoxZone::AddNewMessageBox( INT type , FString boxName , UBOOL twoButton )
{
	CanMoveTop = TRUE;
	MMOUI::MoveToTop(this);
	
	switch(type)
	{
	case EMsgboxType_Input:
		return eventCallAddInputMsgbox( boxName , twoButton );
	case EMsgboxType_Modeless:
		return eventCallAddModelessMsgbox( boxName , twoButton );
	case EMsgboxType_Modal:
		return eventCallAddModalMsgbox( boxName , twoButton );
	case EMsgboxType_InputModal:
		return eventCallAddModalInputMsgbox( boxName , twoButton );
	case EMsgboxType_WithCheckBox:
		return eventCallAddMsgboxWithCheckBox(boxName , twoButton);
	case EMsgboxType_Modeless_Extr:
		return eventCallAddModelessExtrMsgbox(boxName , twoButton);
	case EMsgboxType_Big:
		return eventCallAddBigMsgbox(boxName,twoButton);
	}
	return NULL;
}

void UMMOUIMessageBoxZone::Show(UBOOL v, INT displayPriority)
{
	UMMOUIBase::Show(v, displayPriority);

	//if( v )
	//{

	//}
	//else
	//{
	//	UGFxObject* obj = this->eventCallGetFirstChild();

	//	if( obj )
	//	{
	//		eventRemoveChild(obj);
	//	}
	//}
	_ConfirmBox = NULL;

}

void UMMOUIMessageBoxZone::SetVisible(UBOOL bValue /* = TRUE */)
{
		int i = 0;
}

void UMMOUIMessageBoxZone::ShowUI(FName inFriendlyName,UBOOL inShow/* =TRUE */)
{
	
}

UBOOL UMMOUIMessageBoxZone::OnEsc()
{
	if(MMOMsgBoxManager::getSingleton().HasMessage())
	{
		return TRUE;
	}
	return FALSE;
}

void UMMOUIMessageBoxZone::ASUnLockMsgAck( UBOOL Ok , class UGFxObject* mc )
{

	MMOMsgBoxManager::getSingleton().GetMessageAck( Ok , mc );
}

void UMMOUIMessageBoxZone::ASInputMsgAck( UBOOL Ok , class UGFxObject* mc )
{

	MMOMsgBoxManager::getSingleton().GetMessageAck( Ok , mc );
}

void UMMOUIMessageBoxZone::ASCountDownMsgBoxAck( UBOOL Ok , class UGFxObject* mc )
{
	MMOMsgBoxManager::getSingleton().GetMessageAck( Ok , mc );
}

void UMMOUIMessageBoxZone::ASCountDownMsgBoxTimeUp( class UGFxObject* mc )
{
	MMOMsgBoxManager::getSingleton().GetMessageAck( false , mc );
}
void UMMOUIMessageBoxZone::ASMessageBoxCheck(class UGFxObject* mc,UBOOL bCheck)
{
	if(mc)
	{
		mc->SetBool("forbidHint",bCheck);
	}
}

void UMMOUIMessageBoxZone::AutoAdjustSystemSettingOk(class UGFxObject* mc)
{
	if(mc&&_ConfirmBox)
	{
		BOOL bRes = !(_ConfirmBox->GetBool("forbidHint"));

		INT nLevel = GSystemSettings.CompatLevelComposite;

		//system settings auto-adjust here
		AutoLowScaleLevel(true);//GSystemSettings.AutoAdjustCompatLevel();

		if(bRes != GSystemSettings.bHintBeforeAutoAdjust)
		{
			FSystemSettings old;
			GSystemSettings.ModifyBegin(old);
			GSystemSettings.SetBool(TEXT("HintBeforeAutoAdjust"),bRes);
			GSystemSettings.SetBool(TEXT("AutoAdjustPerformance"),true);
			GSystemSettings.ModifyEnd(old);
			GConfig->Flush(FALSE, GEngineIni);
		}
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_OptionConfig,FALSE, "");
	}

	_ConfirmBox = NULL;
}
void UMMOUIMessageBoxZone::AutoAdjustSystemSettingCancel(class UGFxObject* mc)
{
	if(mc&&_ConfirmBox)
	{
		BOOL bRes = !(_ConfirmBox->GetBool("forbidHint"));
		if(bRes != GSystemSettings.bHintBeforeAutoAdjust)
		{
			FSystemSettings old;
			GSystemSettings.ModifyBegin(old);
			GSystemSettings.SetBool(TEXT("HintBeforeAutoAdjust"),bRes);
			GSystemSettings.SetBool(TEXT("AutoAdjustPerformance"),false);
			GSystemSettings.ModifyEnd(old);
			GConfig->Flush(FALSE, GEngineIni);
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_OptionConfig,FALSE, "");
		}
	}
	_ConfirmBox = NULL;
}
BOOL MMOMsgBoxManager::AddOpenChainPayMessageBox()
{
	if(confirmBox)
	{
		confirmBox->SetVisible(FALSE);
		confirmBox = NULL;
	}

	confirmBox = AddModalMessageBoxEvent( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.AskOpenChainPay")) , 0 , GameEvent_OpenChainPay );
	
	if(confirmBox)
		confirmBox->SetInt(TEXT("EventType"),GameEvent_OpenChainPay);

	return (confirmBox != NULL);
}
MMOMsgBoxManager::MMOMsgBoxManager()
{
	CurMsgId = 0;
	MoviePlayer = NULL;
	confirmBox = NULL;
}

void MMOMsgBoxManager::init( UMMOGFXPlayer* mp )
{
	MoviePlayer = mp;
	confirmBox = NULL;
}

void MMOMsgBoxManager::ClearAll()
{
	//for( INT i = 0;i< MsgInfoList.Num();i++)
	//{
	//	INT MsgId =  MsgInfoList(i).MsgId;
	//	MsgInfoList.Remove(i,1);
	//	MsgBoxMap.erase( MsgId );
	//	MsgBoxTypeMap.erase( MsgId );
	//}

	//for( INT i = 0 ; i< MsgInfoEventList.Num();i++)
	//{
	//	INT MsgId =  MsgInfoEventList(i).MsgId;
	//	MsgInfoEventList.Remove(i,1);
	//	MsgBoxMap.erase( MsgId );
	//	MsgBoxTypeMap.erase( MsgId );
	//}

	MsgInfoList.Empty();
	MsgInfoEventList.Empty();
	MsgBoxMap.clear();
	MsgBoxTypeMap.clear();

	UMMOUIMessageBoxZone* zone = Cast<UMMOUIMessageBoxZone>( MMOUI::GetUIManager()->FindUIPanel( MMOUI::GetUIManager()->Panel_MessageboxZone ) );
	if( !zone )
	{
		return ;
	}
	zone->eventCallClearAllMsgbox();
	confirmBox = NULL;
}

void MMOMsgBoxManager::GetMessageAck( UBOOL ok , UGFxObject *mc /*int InfoId,int MsgId*/ )
{
	INT MsgId = mc->GetInt("itemid");
	UObject *obj = NULL;
	
	for( INT i = 0;i< MsgInfoList.Num();i++)
	{
		if( MsgInfoList(i).MsgId == MsgId )
		{
			UObject* contextObject = NULL;
			FName funName;
			if( ok)
			{
				contextObject = MsgInfoList(i).OnClick.Object;
				funName = MsgInfoList(i).OnClick.FunctionName;

				MMOMsgboxManager_Parms Parms(EC_EventParm);
				Parms.mc = mc;
				if( contextObject && funName.IsValid() && funName != NAME_None )
				{
					contextObject->ProcessEvent( contextObject->FindFunctionChecked(funName), &Parms , NULL );
				}
			}	
			else
			{
				contextObject = MsgInfoList(i).OnClickNo.Object;
				funName = MsgInfoList(i).OnClickNo.FunctionName;

				MMOMsgboxManager_Parms Parms(EC_EventParm);
				Parms.mc = mc;
				if( contextObject && funName.IsValid() && funName != NAME_None )
				{
					contextObject->ProcessEvent( contextObject->FindFunctionChecked(funName), &Parms , NULL );
				}
			}
				
			
			MsgInfoList.Remove(i,1);


			MsgBoxMap.erase( MsgId );
			MsgBoxTypeMap.erase( MsgId );
			break;
		}
	}

	for( INT i = 0 ; i< MsgInfoEventList.Num();i++)
	{
		if( MsgInfoEventList(i).MsgId == MsgId )
		{
			
			if( ok)
			{
				MMOGameEvent_MsgboxCallBack evt( MsgInfoEventList(i).OnClickEventId , mc , TRUE );
				MMOUI::PostGameEvent(&evt);
			}
			else
			{
				MMOGameEvent_MsgboxCallBack evt( MsgInfoEventList(i).OnClickEventId , mc , FALSE );
				MMOUI::PostGameEvent(&evt);
			}

			if(mc&&(mc->GetInt(TEXT("EventType")) == GameEvent_OpenChainPay))
				confirmBox = NULL;

			//mc->SetVisible(false);
			MsgInfoEventList.Remove(i,1);

			MsgBoxMap.erase( MsgId );
			MsgBoxTypeMap.erase( MsgId );
			break;
		}
	}

	MMOGameEvent_Any evt(GameEvent_GetMessageAck);
	MMOUI::PostGameEvent(&evt);

}
//-------------------------------------------------------------------------------
UGFxObject* MMOMsgBoxManager::AddMessageBox(const FString& text,INT InfoId,FScriptDelegate OnClickOK,FScriptDelegate OnClickNO , UBOOL twoButton  )
{

	_AddMsgInfoDelegateList( OnClickOK , OnClickNO , EMsgboxType_Modeless );

	UGFxObject* mc = _CreateNewMsgBox(EMsgboxType_Modeless,twoButton);

	_AddMsgBoxContent( mc , TEXT("") , text , InfoId , CurMsgId );

	_RandomMsgPos(mc);

	return mc;
}

UGFxObject* MMOMsgBoxManager::AddBigMessageBox(const FString& Text ,const FString& TextContent , INT InfoId , FScriptDelegate onClickOK , FScriptDelegate OnClickNO , UBOOL twoButton)
{
	_AddMsgInfoDelegateList( onClickOK , OnClickNO , EMsgboxType_Big );

	UGFxObject* mc = _CreateNewMsgBox(EMsgboxType_Big,twoButton);

	_AddMsgBoxContent_Big( mc , TEXT("") , Text , TextContent, InfoId , CurMsgId );

	_RandomMsgPos(mc);

	return mc;
}


UGFxObject* MMOMsgBoxManager::AddMessageBoxEvent( FString text , INT infoId , EGameEvent clickEvent ,eMsgBtnShowType type, UBOOL twoButton )
{
	_AddMsgInfoEventList( CurMsgId , clickEvent );

	MsgboxType msbType;
	if (checkMsgBoxType(type))
		 msbType = EMsgboxType_Modeless_Extr;
	else
		msbType = EMsgboxType_Modeless;

	UGFxObject* mc = _CreateNewMsgBox( msbType , twoButton );
	
	if( !mc )
	{
		return NULL;
	}

	_AddMsgBoxContent( mc , TEXT("") , text , infoId , CurMsgId );
	_RandomMsgPos(mc);
	
	return mc;
}

BOOL MMOMsgBoxManager::checkMsgBoxType(eMsgBtnShowType type)
{
	if ( type == E_MsgBtnShow_Team)
		return TRUE;
	else
		return FALSE;
}

//-------------------------------------------------------------------------------
UGFxObject* MMOMsgBoxManager::AddInputMessageBox( FString titleText, INT infoId , FString infoText , FScriptDelegate OnClickOK, FScriptDelegate OnClickNO, INT format , UBOOL twoButton   )
{
	
	_AddMsgInfoDelegateList( OnClickOK , OnClickNO , EMsgboxType_Input );

	UGFxObject* mc = _CreateNewMsgBox( EMsgboxType_Input , twoButton );

	if( !mc )
	{
		return NULL;
	}
	
	RestrictInput( mc, format, 0 );

	_RandomMsgPos(mc);

	_AddMsgBoxContent( mc , titleText , infoText , infoId , CurMsgId );
	

	return mc;
}

//-------------------------------------------------------------------------------
UGFxObject* MMOMsgBoxManager::AddInputMessageBox( FString titleText , INT infoId , FString infoText , EGameEvent clickEvent , INT format ,UBOOL twoButton)
{
	//UGFxObject* mc = Add2BtnMessageBoxEventBase( TEXT("MyInputMsgBox") , clickEvent , twoButton);

	_AddMsgInfoEventList( CurMsgId , clickEvent  );

	UGFxObject* mc = _CreateNewMsgBox( EMsgboxType_Input , twoButton );

	if( !mc )
	{
		return NULL;
	}
	
	RestrictInput( mc, format, 0 );

	_RandomMsgPos(mc);

	_AddMsgBoxContent( mc , titleText , infoText , infoId , CurMsgId );

	return mc;
}

//-------------------------------------------------------------------------------
//UGFxObject* MMOMsgBoxManager::Add2BtnMessageBoxBase( const FString &msgClass, FScriptDelegate OnClickOK , FScriptDelegate OnClickNO ,UBOOL twoButton)
//{
	
	//MsgInfo Msg;
	//Msg.MsgId = CurMsgId;
	//Msg.OnClick = OnClickOK;
	//Msg.OnClickNo = OnClickNO;
	//MsgInfoList.AddItem(Msg);

	//FString msgBoxName = msgClass + appItoa(CurMsgId);
	//
	//

	//FASDisplayInfo DI = mc->GetDisplayInfo();
	////INT width = mc->GetInt(TEXT("width"));
	////INT height =  mc->GetInt(TEXT("height"));
	//DI.X = DI.X + RandRange(-15,15);
	//DI.Y = DI.Y  + RandRange(-15,15); 
	//mc->SetDisplayInfo(DI);
	//CurMsgId++;

//	return mc;
//}
//-------------------------------------------------------------------------------
//pop-up MessageBox covering the full screen
UGFxObject* MMOMsgBoxManager::AddModalMessageBox( const FString& Text, INT InfoId, FScriptDelegate OnClickOK, FScriptDelegate OnClickNO ,UBOOL twoButton)
{
	_AddMsgInfoDelegateList( OnClickOK , OnClickNO, EMsgboxType_Modal );

	UGFxObject*  mc = _CreateNewMsgBox( EMsgboxType_Modal , twoButton );

	if(!mc)
	{
		return NULL;
	}

	_AddMsgBoxContent( mc , TEXT("") , Text , InfoId , CurMsgId );
	
	return mc;
}

UGFxObject* MMOMsgBoxManager::AddModalMessageBoxEvent( FString text , INT infoId , EGameEvent clickOkEvent , UBOOL twoButton /* = TRUE */ )
{

	_AddMsgInfoEventList( CurMsgId , clickOkEvent );

	UGFxObject* mc = _CreateNewMsgBox( EMsgboxType_Modal , twoButton );

	_AddMsgBoxContent( mc , TEXT("") , text , infoId , CurMsgId );

	return mc;
}

UGFxObject* MMOMsgBoxManager::AddModalInputMsgboxEvent( FString titleText , INT infoId , FString conText , EGameEvent clickOkEvent , INT format , UBOOL twoButton )
{
	_AddMsgInfoEventList( CurMsgId , clickOkEvent );

	UGFxObject* mc = _CreateNewMsgBox( EMsgboxType_InputModal , twoButton );

	if( !mc )
	{
		return NULL;
	}

	RestrictInput( mc, format, 0 );

	_AddMsgBoxContent( mc , titleText , conText , infoId , CurMsgId );

	return mc;
}

UGFxObject* MMOMsgBoxManager::AddModalInputMsgbox( FString titleText, INT infoId , FString infoText , FScriptDelegate OnClickOK, FScriptDelegate OnClickNO, INT format /* = eInputFormatAll  */, UBOOL twoButton /* = TRUE */ )
{
	_AddMsgInfoDelegateList( OnClickOK , OnClickNO,EMsgboxType_InputModal );

	UGFxObject* mc = _CreateNewMsgBox( EMsgboxType_InputModal , twoButton );

	if( !mc )
	{
		return NULL;
	}

	RestrictInput( mc, format, 0 );

	_AddMsgBoxContent( mc , titleText , infoText , infoId , CurMsgId );

	return mc;
}

void MMOMsgBoxManager::RestrictInput( UGFxObject *pInputMsgBox, INT format, INT charNum )
{
	if( !pInputMsgBox ){
		return;
	}

	UGFxObject *pInputTextMc = pInputMsgBox->GetObject( TEXT("textInput") );
	if( pInputTextMc == NULL ){
		return;
	}

	UGFxObject *pInputTextField = pInputTextMc->GetObject( TEXT("textField") );
	if( pInputTextField == NULL ){
		return;
	}

	switch( format )
	{
	case eInputFormatNumOnly:
		{
			pInputTextField->SetString( TEXT("restrict"), TEXT("0-9") );
			pInputTextMc->SetInt( TEXT("maxChars"), charNum );
		}
		break;
	case eInputFormatAll:
	default:
		{
			pInputTextField->SetString( TEXT("restrict"), TEXT("^ ")/*TEXT("A-Za-z0-9")*/ );
			pInputTextMc->SetInt( TEXT("maxChars"), charNum );
		}
		break;
	}

	g_focusInTextInput = TRUE;
}
UGFxObject* MMOMsgBoxManager::AddMessageBoxWithCheck(FString titleText, INT InfoId , FString InfoText , FScriptDelegate OnClickOK, FScriptDelegate OnClickNO,UBOOL twoButton)
{
	_AddMsgInfoDelegateList( OnClickOK , OnClickNO ,EMsgboxType_WithCheckBox);

	UGFxObject* mc = _CreateNewMsgBox( EMsgboxType_WithCheckBox , twoButton );
	if( !mc )
	{
		return NULL;
	}

	_AddMsgBoxContent( mc ,titleText , InfoText , InfoId , CurMsgId );
	_RandomMsgPos(mc);

	return mc;
}
BOOL MMOMsgBoxManager::AutoAdjustSystemSettingMessageBox()
{
	UMMOUIMessageBoxZone* zone = Cast<UMMOUIMessageBoxZone>( MMOUI::GetUIManager()->FindUIPanel( MMOUI::GetUIManager()->Panel_MessageboxZone ) );
	if( !zone )
	{
		return FALSE;
	}
	zone->CreateAutoAdjustFunction();
	return TRUE;
}

void MMOMsgBoxManager::DispatchMsgBoxButtonOk(class UGFxObject* mc)
{
	if( !MoviePlayer || !mc )
	{
		return;
	}
	UGFxObject* okButton = mc->GetObject( TEXT("yesButton_mc") );
	MoviePlayer->eventCallWidgetDispatchEvent( okButton , TEXT("click") );
}

void MMOMsgBoxManager::DispatchMsgBoxButtonNo(class UGFxObject* mc)
{
	if( !MoviePlayer || !mc )
	{
		return;
	}
	UGFxObject* noButton = mc->GetObject( TEXT("noButton_mc") );
	MoviePlayer->eventCallWidgetDispatchEvent( noButton , TEXT("click") );
}

void MMOMsgBoxManager::_AddMsgInfoEventList( INT msgId , EGameEvent eventId )
{
	MsgInfoEvent Msg;
	Msg.MsgId = CurMsgId;
	Msg.OnClickEventId = eventId;

	MsgInfoEventList.AddItem(Msg);
}

void MMOMsgBoxManager::_AddMsgInfoDelegateList( FScriptDelegate OnClick , FScriptDelegate OnClickNo , MsgboxType type )
{
	MsgInfo Msg;
	Msg.MsgId = CurMsgId;
	Msg.OnClick = OnClick;
	Msg.OnClickNo = OnClickNo;
	Msg.type = type;
	MsgInfoList.AddItem(Msg);
}

INT MMOMsgBoxManager::GetMsgNumByType(MsgboxType type )
{
	INT nMsgBoxNum = 0;
	for( INT i = 0;i< MsgInfoList.Num();i++)
	{
		if( MsgInfoList(i).type == type )
		{
			nMsgBoxNum++;
		}
	}
	return nMsgBoxNum;
}



void MMOMsgBoxManager::_AddMsgBoxContent( UGFxObject* mc , FString title , FString content , INT infoId , INT itemId )
{
	if(!mc)
		return;

	UGFxObject* bg = mc->GetObject( TEXT("MyWindowsFrame_mc") );
	if(bg)
	{
		bg->GetObject( TEXT("tF_windowTitle") )->SetString( TEXT("htmlText") , title );
		bg->GetObject( TEXT("content") )->SetString( TEXT("htmlText") , content );
	}

	mc->SetInt( TEXT("iteminfoid"), infoId );
	mc->SetInt( TEXT("itemid"), itemId );

	CurMsgId++;
}

void MMOMsgBoxManager::_AddMsgBoxContent_Big( UGFxObject* mc , FString title , FString content1 , FString content2,INT infoId , INT itemId )
{
	if(!mc)
		return;

	UGFxObject* bg = mc->GetObject( TEXT("MyWindowsFrame_mc") );
	if(bg)
	{
		bg->GetObject( TEXT("tF_windowTitle") )->SetString( TEXT("htmlText") , title );
		bg->GetObject( TEXT("content") )->SetString( TEXT("htmlText") , content1 );
	}
	mc->GetObject( TEXT("txt_description") )->SetString( TEXT("htmlText") , content2 );

	mc->SetInt( TEXT("iteminfoid"), infoId );
	mc->SetInt( TEXT("itemid"), itemId );

	CurMsgId++;
}

void MMOMsgBoxManager::_RandomMsgPos( UGFxObject* mc )
{
	if(!mc)
	{
		return;
	}
	FASDisplayInfo DI = mc->GetDisplayInfo();
	DI.X = DI.X + RandRange(-15,15);
	DI.Y = DI.Y + RandRange(-15,15); 
	mc->SetDisplayInfo(DI);
}

UGFxObject*  MMOMsgBoxManager::_CreateNewMsgBox( MsgboxType type , UBOOL twoButton )
{
	UMMOUIMessageBoxZone* zone = Cast<UMMOUIMessageBoxZone>( MMOUI::GetUIManager()->FindUIPanel( MMOUI::GetUIManager()->Panel_MessageboxZone ) );
	if( !zone )
	{
		return NULL;
	}

	FString msgBoxName = FString(TEXT("MsgBox")) + appItoa(CurMsgId);

	UGFxObject* mc = zone->AddNewMessageBox( type , msgBoxName ,twoButton); // RootMc->AttachMovie( msgClass, *msgBoxName );

	if( !mc ){
		return NULL;
	}
	mc->SetInt( TEXT("itemid"),CurMsgId);
	MsgBoxMap.insert( std::make_pair( CurMsgId , mc ) );
	MsgBoxTypeMap.insert( std::make_pair( CurMsgId , type ) );

	zone->eventShowPanel(true, 4);

	return mc;
}

BOOL MMOMsgBoxManager::IsModalMsgBox()
{
	std::map<INT, INT>::iterator iter = MsgBoxTypeMap.begin();
	for(;iter!=MsgBoxTypeMap.end();iter++)
	{
		INT type = iter->second;
		if(type==EMsgboxType_Modal||type==EMsgboxType_InputModal)
		{
			return TRUE;
		}
	}
	return FALSE;
}
