
#ifndef _MMOMSGBUTTONMANAGER_H_
#define _MMOMSGBUTTONMANAGER_H_

#include "MMOSingleton.h"
#include "MMOUtil.h"
#include "MMOGameEvent.h"

enum eMsgBtnShowType
{
	E_MsgBtnShow_System,
	E_MsgBtnShow_Trade,
	E_MsgBtnShow_Guild,
	E_MsgBtnShow_Friend,
	E_MsgBtnShow_Team,
	E_MsgBtnShow_Mail,
	E_MsgBtnShow_Hint,	
	E_MsgBtnShow_GMBall,
	E_MsgBtnShow_Master,
	E_MsgBtnShow_Enemy,
	E_MsgBtnShow_GuildAlly,
	E_MsgBtnShow_GameUpdateInfo,
	E_MsgBtnShow_SignIn,
	E_MsgBtnShow_Jigsaw,
	E_MsgBtnShow_ArenaWait,
	E_MsgBtnShow_Max,
};

enum eEventHandleType
{
	//EVENTHANDLE_DELEGATE,					//respond via function delegate
	EVENTHANDLE_GAMEEVENT,				//respond via event
	EVENTHANDLE_NOMSGBOX					//after clicking the message orb, no message box, respond via delegate
};

//////////////////////////////////////////////////////////////////////////
//parameter of the event delegate for message orbs without a message box, convenient for future extension
//////////////////////////////////////////////////////////////////////////
struct MMOMsgButtonManager_Parms
{
	UINT32 Parm1;
	UINT32 Parm2;
	UINT32 Parm3;

	MMOMsgButtonManager_Parms(EEventParm)
	{
		Parm1 = 0;
		Parm2 = 0;
		Parm3 = 0;
	}
};

struct MsgButtonInfo
{
	MsgButtonInfo()
		:_Parms(EC_EventParm),infoId(0)
	{

		EventId = GameEvent_Invalid;
		_eventHandleType = EVENTHANDLE_GAMEEVENT;
		leftButtonName = "MMOGame.OKMsg";
		rightButtonName = "MMOGame.CancelMsg";
		type = E_MsgBtnShow_Max;
	};
	FString msgText;
	INT infoId;
	FString senderName;
	FScriptDelegate OnEventHandle;
	MMOMsgButtonManager_Parms _Parms;
	EGameEvent EventId;
	eEventHandleType _eventHandleType;
	FString leftButtonName;
	FString rightButtonName;
	eMsgBtnShowType type;
};

enum EMsgButtonType
{
	EMsgType_Normal,
	EMsgType_Input,

	EMsgType_Count,
};

typedef std::vector< MsgButtonInfo, M_UE3_STL_ALLOC(MsgButtonInfo)  > MsgButtonInfoVec;

//const int msgButtonCount = 4;

class MMOMsgButtonManager : public SingletonMeyers<MMOMsgButtonManager>
{
public:

	MMOMsgButtonManager();
	virtual ~MMOMsgButtonManager();

	void Init();
	void Shutdown();

	//INT AddNewButton( const FString& Text , INT InfoId , FScriptDelegate OnClickOK , FScriptDelegate OnClickNO, eMsgBtnShowType type = E_MsgBtnShow_Normal );
	INT AddNewButton( const FString& Text , INT InfoId , FString sendName , EGameEvent eventId , eMsgBtnShowType type ,FString leftButtonMsg="MMOGame.OKMsg", FString rightButtonMsg="MMOGame.CancelMsg");
	INT AddNewButton( FScriptDelegate OnEventHandle, MMOMsgButtonManager_Parms Parms, eMsgBtnShowType type ,FString leftButtonMsg="MMOGame.OKMsg", FString rightButtonMsg="MMOGame.CancelMsg" );

	
	//INT AddMsgButtonInfo(   );

	void RemoveButton( INT index );
	//MsgButtonInfo *GetNewButtonData( UINT idx ){
	//	MsgButtonInfo *pRtn = NULL;
	//	if( idx >=0 && idx < _MsgButtonInfoList.size() ){
	//		pRtn = &_MsgButtonInfoList.at( idx );
	//	}
	//	return pRtn;
	//}
	void RemoveOneTypeButtons(INT type);
	void ClickButton( INT type );
	void ClickMsgLine( INT pos , INT type );

	void TickTime( float deltaTime );
	
	void ClearAll();
	void UpdateButton( INT type );

	void ToggleMsgLine( INT type );
	void UpdateMsgLine( INT type );
	MsgButtonInfoVec& GetMsgButtonInfo( eMsgBtnShowType type );

protected:
	
	//typedef std::vector< INT > HintPosVec;
	//typedef std::map< INT , IMMOHintButtonCommand* > HintButtonCommandMap;

	MsgButtonInfoVec _MsgButtonInfoList[E_MsgBtnShow_Max];

	void _EmitDelegate(INT nPos);
};

#endif