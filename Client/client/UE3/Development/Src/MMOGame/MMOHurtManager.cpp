#include "MMOHurtManager.h"

MMOHurtManager::MMOHurtManager()
{
	_MaxHurt = 0.0;
	Reset();
}


MMOHurtManager::~MMOHurtManager()
{
}

void MMOHurtManager::Init()
{
	GConfig->GetDouble( TEXT("Hurt") , TEXT("MaxHurt") , _MaxHurt , GDefaultGameIni );
	//Reset();
}

void MMOHurtManager::Reset()
{
	_HurtData.Reset();
	_TimeTemp = 0.0f;
	//_MaxHurt = 0.0;
}

void MMOHurtManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{

}

void MMOHurtManager:: SetHurtTime(float DeltaTime)
{
	_HurtData.HurtTime += DeltaTime;
}

void MMOHurtManager::SetAllHurt(UINT hurt)
{
	if ((_HurtData.AllHurts+hurt) < _MaxHurt)
	{
		_HurtData.AllHurts += hurt;
	}
	else
	{
		_HurtData.AllHurts = _MaxHurt;
	}
}

void MMOHurtManager::RestHurt()
{
	Reset();
	PostUpdateEvent();

}

float MMOHurtManager::GetHurtTime()
{
	return _HurtData.HurtTime;
}

UINT MMOHurtManager::GetAllhurt()
{
	return _HurtData.AllHurts;
}


void MMOHurtManager::Update(float deltaTime)
{
	CTRL_TICK("MMOHurtManager");

	if (MMOUI::GetUIManager()->DrawHurt)
	{
		if (CheckHurt())
		{
			//determine whether the player is in combat
			AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
			if (!player)
			{
				return;
			}
			if (player->IsInFighting())
			{
				CalculateHurtTime(deltaTime);
			}
			else
			{
				_TimeTemp = 0.0f;
			}
		}
		else
		{
			_TimeTemp = 0.0f;
		}
	}
	
}

void MMOHurtManager::CalculateHurtTime(float deltaTime)
{
	_TimeTemp += deltaTime;
	if (_TimeTemp >= Seconds)
	{
		SetHurtTime(Seconds);
		_TimeTemp = 0.0f;
	}
}

void MMOHurtManager::UpdateAllHurt(UINT playerId,INT32 hurt)
{
	if (MMOUI::GetUIManager()->DrawHurt)
	{
		
			AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(playerId);
			AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
			if (actor && player)
			{
				if (actor->ServerID == player->ServerID /*&& player->IsInFighting()*/)
				{	
					if (CheckHurt())
					{
						SetAllHurt(Abs(hurt));
						//update UI
						PostUpdateEvent();
					}
					else
					{
						ShowScreenMsg();
					}

				}
			}
	}
}

void MMOHurtManager::PostUpdateEvent()
{
	MMOGameEvent_Any evt(GameEvent_ResetHurt);
	MMOUI::PostGameEvent(&evt);
}

BOOL MMOHurtManager::CheckHurt()
{
	if (GetAllhurt() >= _MaxHurt)
	{
		return FALSE;
	}
	return TRUE;
}

void MMOHurtManager::ShowScreenMsg()
{
	//Add a scrolling text prompt
	FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errHurtIsMax"));
	MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errHurtIsMax"));
}
