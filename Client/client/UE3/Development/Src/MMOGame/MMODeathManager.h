#ifndef _MMODEATHMANAGER_H_
#define _MMODEATHMANAGER_H_

#include "MMOSingleton.h"
#include "MMOGameEvent.h"
#include "MMOGameClasses.h"

	class MMODeathManager: public SingletonMeyers<MMODeathManager>
	{
	public:
		MMODeathManager();
		void Init();
		void Shutdown();

		void NoityfyShowDeathPanel();
		
	public:
		void SetReviveInfo(G2C_REVIVE_INFO* pInfo);
		REVIVE_INFO* GetReviveInfo(CHAR_REVIVE_TYPE tp);

		INT32 GetWaitTime(){return WaitTime;}
		BOOL  GetPlayerKilled(){return PlayerKilled;};
		 
		INT GetReviveTime(){ return ReviveTime;}
	private:
		REVIVE_INFO		ReviveInfo[crtTotal];
		INT32			WaitTime;
		UINT32          KillerID;
		BOOL            PlayerKilled;
		INT				ReviveTime;
	};

#endif