
#ifndef _MMOMsgBoxManager_H_
#define _MMOMsgBoxManager_H_

#include "MMOSingleton.h"
#include "MMOGameEvent.h"
#include "MMOMsgButtonManager.h"

enum MsgboxType
{
	EMsgboxType_Modeless,
	EMsgboxType_Modal,
	EMsgboxType_Input,
	EMsgboxType_InputModal,
	EMsgboxType_WithCheckBox,
	EMsgboxType_Modeless_Extr,
	EMsgboxType_Big,
};

struct MsgInfoEvent
{
	MsgInfoEvent() : MsgId(0), OnClickEventId(GameEvent_Invalid) {}
	INT MsgId;
	EGameEvent OnClickEventId;
	
};

struct MsgInfo
{
	MsgInfo() : MsgId(0), OnClick(EC_EventParm), OnClickNo(EC_EventParm), type(EMsgboxType_Modeless) {}
	INT MsgId;
	FScriptDelegate OnClick;
	FScriptDelegate OnClickNo;
	MsgboxType type;
};

struct MMOMsgboxManager_Parms
{
	UGFxObject* mc;
	MMOMsgboxManager_Parms(EEventParm):mc(NULL)
	{
	}
};

enum InputFormat
{
	eInputFormatAll,
	eInputFormatNumOnly
};



typedef std::map< INT , UGFxObject* > MapGfxObject;

class MMOMsgBoxManager: public SingletonMeyers<MMOMsgBoxManager>
{
public:

	MMOMsgBoxManager();
	virtual ~MMOMsgBoxManager(){};

	void init( UMMOGFXPlayer* mp );
	void uninit();

	void ClearAll();

	//event
	UGFxObject* AddMessageBoxEvent( FString text , INT infoId , EGameEvent clickOkEvent , eMsgBtnShowType type = E_MsgBtnShow_Max,UBOOL twoButton = TRUE);
	//UGFxObject* Add2BtnMessageBoxEventBase( FString msgClass , EGameEvent clickEvent , UBOOL twoButton = TRUE );
	UGFxObject* AddModalMessageBoxEvent( FString text , INT infoId , EGameEvent clickOkEvent , UBOOL twoButton = TRUE );
	UGFxObject* AddModalInputMsgboxEvent( FString titleText , INT infoId , FString conText , EGameEvent clickOkEvent , INT format = eInputFormatAll , UBOOL twoButton = TRUE );
	UGFxObject* AddInputMessageBox( FString titleText , INT infoId , FString conText , EGameEvent clickEvent , INT format = eInputFormatAll , UBOOL twoButton = TRUE );

	UGFxObject* AddMessageBox( const FString& Text , INT InfoId , FScriptDelegate onClickOK , FScriptDelegate OnClickNO , UBOOL twoButton = TRUE );
	UGFxObject* AddInputMessageBox( FString titleText, INT InfoId , FString InfoText , FScriptDelegate OnClickOK, FScriptDelegate OnClickNO, INT format = eInputFormatAll , UBOOL twoButton = TRUE );
	UGFxObject* AddModalMessageBox( const FString& text, INT infoId , FScriptDelegate onClickOK , FScriptDelegate onClickNO , UBOOL twoButton = TRUE );
	UGFxObject* AddModalInputMsgbox( FString titleText, INT InfoId , FString InfoText , FScriptDelegate OnClickOK, FScriptDelegate OnClickNO, INT format = eInputFormatAll , UBOOL twoButton = TRUE );
	UGFxObject* AddMessageBoxWithCheck(FString titleText, INT InfoId , FString InfoText , FScriptDelegate OnClickOK, FScriptDelegate OnClickNO, UBOOL twoButton = TRUE);
	UGFxObject* AddBigMessageBox(const FString& Text ,const FString& TextContent , INT InfoId , FScriptDelegate onClickOK , FScriptDelegate OnClickNO , UBOOL twoButton = TRUE );

	void RestrictInput( UGFxObject *pInputMsgBox, INT format, INT charNum );

	BOOL AutoAdjustSystemSettingMessageBox();
	BOOL AddOpenChainPayMessageBox();

	void GetMessageAck( UBOOL ok , UGFxObject* mc );

	void DispatchMsgBoxButtonOk( UGFxObject* mc );
	void DispatchMsgBoxButtonNo( UGFxObject* mc );

    UBOOL HasMessage(){ return MsgInfoList.Num() > 0; }

	BOOL checkMsgBoxType(eMsgBtnShowType type);
	INT	 GetMsgNumByType(MsgboxType type );

	BOOL IsModalMsgBox();
protected:

	INT CurMsgId;
	UMMOGFXPlayer* MoviePlayer;

	TArray<MsgInfoEvent> MsgInfoEventList;
	TArray<MsgInfo> MsgInfoList;

	MapGfxObject MsgBoxMap;
	std::map<INT, INT> MsgBoxTypeMap;
	UGFxObject*  confirmBox;
	void _AddMsgInfoEventList( INT msgId , EGameEvent eventId );
	void _AddMsgInfoDelegateList( FScriptDelegate OnClick , FScriptDelegate OnClickNo , MsgboxType type );
	void _AddMsgBoxContent( UGFxObject* mc , FString title , FString content , INT infoId , INT itemId );
	void _AddMsgBoxContent_Big( UGFxObject* mc , FString title , FString content1 , FString content2,INT infoId , INT itemId );
	void _RandomMsgPos( UGFxObject* mc );
	UGFxObject*  _CreateNewMsgBox( MsgboxType type , UBOOL twoButton );

};

#endif