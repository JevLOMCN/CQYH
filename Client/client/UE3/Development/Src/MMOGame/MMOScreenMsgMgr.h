#ifndef _USCREENMSGMGR_H_
#define _USCREENMSGMGR_H_

#include "MMOSingleton.h"
//#include "MMOGameClasses.h"
#include "MMOGameEvent.h"
#include "MMOGameClasses.h"

	struct ScreenMsg
	{
		ScreenMsg() : _Type(EScnMsgType_Invalid), _MsgValue(0) {}
		EScreenMsgType _Type;
		INT _MsgValue;
		//UBOOL _IsValid;
	};


	class MMOScreenMsgMgr: public SingletonMeyers<MMOScreenMsgMgr>, public INativeGameEventHandler
	{
	public:
		MMOScreenMsgMgr() : _IsPlaying(FALSE) {}
		void Init();
		void Shutdown();
		void OnNativeGameEvent( struct MMOGameEvent* evt );
		//
		void NotifyEndPlayMsg();
		void PushMsg( EScreenMsgType msgType, INT msgValue );
		void ForceClearMsg();
	private:
		void TryPlayMsg();

	private:
		TArray< ScreenMsg > _Messages;
		UBOOL _IsPlaying;
	};

#endif