#include "stdafx.h"
#include "MMOAssistManager.h"
#include "MMOFacade.h"
#include "MMOChatManager.h"
#include "LinkManager.h"
//#include "MMOScreenMsgMgr.h"
#include "ScreenMsgQueue.h"
#include "MMOGuideManager.h"
#include "MMODropManager.h"
#include "UITextParser.h"
#include "MMOSkillManager.h"
#include "Mir2Attack.h"
#include "MMOClientVariableManager.h"

using namespace ExternalMessage;

MMOAssistManager::MMOAssistManager()
{
	_FirstEnterGame = FALSE;
	_AssistItemCheckTimeProcess = 0.0f;
}

MMOAssistManager::~MMOAssistManager()
{
}

void MMOAssistManager::Init()
{
	MMOAssistData::getSingleton().Init();
}

void MMOAssistManager::Shutdown()
{
	MMOAssistData::getSingleton().Shutdown();
}

void MMOAssistManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (!evt)
	{
		return;
	}

	switch (evt->GetEventType())
	{
	case GameEvent_MainPlayerDataChange:
		{
			MMOGameEvent_MainPlayerDataChange* e = (MMOGameEvent_MainPlayerDataChange*)evt;
			switch (e->DataChangeEventType)
			{
			case MMOGameEvent_MainPlayerDataChange::EMSET_HP:
			case MMOGameEvent_MainPlayerDataChange::EMSET_MP:
				{
					AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
					if (mainPlayer && mainPlayer->ActorAttributes)
					{
						// #19851
						for (INT i = EPT_SLOW_HP; i < EPT_TOTAL; ++ i)
						{
							AssistItemInfo info;
							if (!MMOAssistData::getSingleton().GetAssistItemInfo(i, info))
							{
								continue;
							}

							if (info.Value != 0)
							{
								continue;
							}

							switch (i)
							{
							case EPT_SLOW_HP:
							case EPT_FAST_HP1:
							case EPT_FAST_HP2:
							case EPT_PROTECT_HP:
								{
									// #20091
									//info.Value = (UINT32)((FLOAT)(mainPlayer->ActorAttributes->GetMaxHp()) * 0.5);
								}
								break;

							case EPT_SLOW_MP:
							case EPT_FAST_MP1:
							case EPT_FAST_MP2:
							case EPT_PROTECT_MP:
								{
									// #20091
									//info.Value = (UINT32)((FLOAT)(mainPlayer->ActorAttributes->GetMaxMp()) * 0.5);
								}
								break;
							}

							MMOAssistData::getSingleton().UpdateAssistItemInfo(i, info);
						}
					}
				}	
				break;
			}
		}
		break;
	}
}

void MMOAssistManager::Clear()
{
	MMOAssistData::getSingleton().Clear();
	MMOAssistData::getSingleton().UpdateAssistDropInfoDefault();

	appMemset(&_AssistItemTimeProcess, 0, sizeof(UINT32) * EPT_TOTAL);

	_FirstEnterGame = FALSE;
	_AssistItemCheckTimeProcess = 0.0f;
}

void MMOAssistManager::Update(FLOAT deltaTime)
{
	CTRL_TICK("MMOAssistManager");
	_UpdateAssistItem(deltaTime);
}

void MMOAssistManager::NotifyEnterGame()
{
	if (!_FirstEnterGame)
	{
		//LoadData();
		_FirstEnterGame = TRUE;
	}
}

void MMOAssistManager::LoadData(UINT32 skillData, const UINT32(&protectData)[PROTECT_DATA_COUNT])
{
	_LoadAssistSkill(skillData);
	_LoadAssistProtect(protectData);
	_LoadAssistDrop();
}

void MMOAssistManager::SaveData()
{
	_SaveAssistDrop();
	_SaveAssistProtect();
	_SaveAssistSkill();
}

void MMOAssistManager::_LoadAssistDrop()
{
	FArchive* archive = NULL;
	FString dataName = TEXT("AssistDrop");
	if (MMOFacade::getSingleton().BeginLoadAccountBinary(&archive, dataName))
	{
		INT size;
		archive->Serialize(&size, sizeof(INT));
		for (INT i = 0; i < size; ++ i)
		{
			AssistDropInfo info;
			archive->Serialize(&info, sizeof(AssistDropInfo));
			MMOAssistData::getSingleton().UpdateAssistDropInfo(info.Id, info.Valuable, info.AutoPick, info.Visible);
		}
	}
	MMOFacade::getSingleton().EndLoadAccountBinary(archive);
}

void MMOAssistManager::_SaveAssistDrop()
{
	FArchive* archive = NULL;
	if (MMOFacade::getSingleton().BeginExportAccountData(&archive, TEXT("AssistDrop")))
	{
		AssistDropInfoMap assistItemInfoMap = MMOAssistData::getSingleton().GetAssistDropInfoMap();

		INT size = assistItemInfoMap.size();
		archive->Serialize((void*)(&size), sizeof(INT));

		AssistDropInfoMap::iterator iter = assistItemInfoMap.begin();
		for (; iter != assistItemInfoMap.end(); ++ iter)
		{
			AssistDropInfo info = iter->second;
			archive->Serialize((void*)(&info), sizeof(AssistDropInfo));
		}
	}
	MMOFacade::getSingleton().EndExportAccountData(archive);
}

void MMOAssistManager::_LoadAssistSkill(UINT32 skillData)
{
#ifdef SAVE_TO_SERVER
	FSM::Ctrl.ctrlFlag_DDCS = skillData & (1 << 0);
	FSM::Ctrl.ctrlFlag_ZNBY = skillData & (1 << 1);
	FSM::Ctrl.ctrlFlag_ZDLH = skillData & (1 << 2);
	FSM::Ctrl.ctrlFlag_ZDZR = skillData & (1 << 3);
	FSM::Ctrl.ctrlFlag_ZDKD = skillData & (1 << 4);
	FSM::Ctrl.ctrlFlag_ZDYS = skillData & (1 << 5);
	UINT16 skillId = HIWORD(skillData);
	UINT8 time = (skillData >> 7) & 255;
	UBOOL isAuto = skillData & (1 << 6);
	Mir2Attack::getSingleton().AutoPractice(skillId, time, isAuto);
#else
	FArchive* archive = NULL;
	FString dataName = TEXT("AssistSkill");
	if (MMOFacade::getSingleton().BeginLoadAccountBinary(&archive, dataName))
	{
		UINT32 skillData;
		archive->Serialize(&skillData, sizeof(UINT32));

		FSM::Ctrl.ctrlFlag_DDCS = skillData & (1 << 0);
		FSM::Ctrl.ctrlFlag_ZNBY = skillData & (1 << 1);
		FSM::Ctrl.ctrlFlag_ZDLH = skillData & (1 << 2);
		FSM::Ctrl.ctrlFlag_ZDZR = skillData & (1 << 3);
		FSM::Ctrl.ctrlFlag_ZDKD = skillData & (1 << 4);
		FSM::Ctrl.ctrlFlag_ZDYS = skillData & (1 << 5);
		UINT16 skillId = HIWORD(skillData);
		UINT8 time = (skillData >> 7) & 255;
		UBOOL isAuto = skillData & (1 << 6);
		Mir2Attack::getSingleton().AutoPractice(skillId, time, isAuto);
	}
	MMOFacade::getSingleton().EndLoadAccountBinary(archive);
#endif
}

void MMOAssistManager::_SaveAssistSkill()
{
	UINT32 skillData = 0;
	skillData |= FSM::Ctrl.ctrlFlag_DDCS ? (1 << 0) : 0;
	skillData |= FSM::Ctrl.ctrlFlag_ZNBY ? (1 << 1) : 0;
	skillData |= FSM::Ctrl.ctrlFlag_ZDLH ? (1 << 2) : 0;
	skillData |= FSM::Ctrl.ctrlFlag_ZDZR ? (1 << 3) : 0;
	skillData |= FSM::Ctrl.ctrlFlag_ZDKD ? (1 << 4) : 0;
	skillData |= FSM::Ctrl.ctrlFlag_ZDYS ? (1 << 5) : 0;
	skillData |= Mir2Attack::getSingleton().IsAutoPractice() ? (1 << 6) : 0;
	UINT32 time = Mir2Attack::getSingleton().GetAutoPracticeTime();
	skillData |= time << 7;
	UINT32 skillId = Mir2Attack::getSingleton().GetAutoPracticeAbilityID();
	skillData |= skillId << 16;

#ifdef SAVE_TO_SERVER
	MMOClientVariableManager::getSingleton().SetClientVariable((EClientVariableType)(ECVT_Assist_Skill), skillData);
#else
	FArchive* archive = NULL;
	if (MMOFacade::getSingleton().BeginExportAccountData(&archive, TEXT("AssistSkill")))
	{
		archive->Serialize((void*)(&skillData), sizeof(UINT32));
	}
	MMOFacade::getSingleton().EndExportAccountData(archive);
#endif
}

void MMOAssistManager::_LoadAssistProtect(const UINT32 (&protectData)[PROTECT_DATA_COUNT])
{
	MMOAssistData::getSingleton().UpdateAssistItemInfoDefault();

#ifdef SAVE_TO_SERVER
	for (INT i = EPT_SLOW_HP; i < EPT_TOTAL; ++ i)
	{
		AssistItemInfo info;
		info.Enable = (protectData[ECVT_Assist_Protect_Item - ECVT_Assist_Protect_Begin] >> (4 * i + 3)) & 0x1;
		UINT8 itemIndex = (protectData[ECVT_Assist_Protect_Item - ECVT_Assist_Protect_Begin] >> (4 * i)) & 0x7;
		info.ItemId = MMOAssistData::getSingleton().GetAssistItemIdByIndex(i, itemIndex);
		info.ProtectType = i;

		switch (i)
		{
		case EPT_SLOW_HP:
			info.Value = HIWORD(protectData[ECVT_Assist_Protect_Value1 - ECVT_Assist_Protect_Begin]);
			info.Time = HIBYTE(HIWORD(protectData[ECVT_Assist_Protect_Time1 - ECVT_Assist_Protect_Begin]));
			break;
		case EPT_SLOW_MP:
			info.Value = LOWORD(protectData[ECVT_Assist_Protect_Value1 - ECVT_Assist_Protect_Begin]);
			info.Time = LOBYTE(HIWORD(protectData[ECVT_Assist_Protect_Time1 - ECVT_Assist_Protect_Begin]));
			break;
		case EPT_FAST_HP1:
			info.Value = HIWORD(protectData[ECVT_Assist_Protect_Value2 - ECVT_Assist_Protect_Begin]);
			info.Time = HIBYTE(LOWORD(protectData[ECVT_Assist_Protect_Time1 - ECVT_Assist_Protect_Begin]));
			break;
		case EPT_FAST_MP1:
			info.Value = LOWORD(protectData[ECVT_Assist_Protect_Value2 - ECVT_Assist_Protect_Begin]);
			info.Time = LOBYTE(LOWORD(protectData[ECVT_Assist_Protect_Time1 - ECVT_Assist_Protect_Begin]));
			break;
		case EPT_FAST_HP2:
			info.Value = HIWORD(protectData[ECVT_Assist_Protect_Value3 - ECVT_Assist_Protect_Begin]);
			info.Time = HIBYTE(HIWORD(protectData[ECVT_Assist_Protect_Time2 - ECVT_Assist_Protect_Begin]));
			break;
		case EPT_FAST_MP2:
			info.Value = LOWORD(protectData[ECVT_Assist_Protect_Value3 - ECVT_Assist_Protect_Begin]);
			info.Time = LOBYTE(HIWORD(protectData[ECVT_Assist_Protect_Time2 - ECVT_Assist_Protect_Begin]));
			break;
		case EPT_PROTECT_HP:
			info.Value = HIWORD(protectData[ECVT_Assist_Protect_Value4 - ECVT_Assist_Protect_Begin]);
			info.Time = HIBYTE(LOWORD(protectData[ECVT_Assist_Protect_Time2 - ECVT_Assist_Protect_Begin]));
			break;
		case EPT_PROTECT_MP:
			info.Value = LOWORD(protectData[ECVT_Assist_Protect_Value4 - ECVT_Assist_Protect_Begin]);
			info.Time = LOBYTE(LOWORD(protectData[ECVT_Assist_Protect_Time2 - ECVT_Assist_Protect_Begin]));
			break;
		}
		
		// #20091
		if (i == EPT_SLOW_HP || i == EPT_SLOW_MP)
		{
			if (info.Value == 0)
			{
				info.Value = DEFAULT_PROTECT_VALUE;
			}
		}

		if (info.Time == 0)
		{
			info.Time = DEFAULT_PROTECT_TIME;
		}
		
		MMOAssistData::getSingleton().UpdateAssistItemInfo(i, info);
	}
#else
	FArchive* archive = NULL;
	FString dataName = TEXT("AssistItem");
	if (MMOFacade::getSingleton().BeginLoadAccountBinary(&archive, dataName))
	{
		INT size;
		archive->Serialize(&size, sizeof(INT));

		for (INT i = 0; i < size; ++ i)
		{
			AssistItemInfo info;
			archive->Serialize(&info, sizeof(AssistItemInfo));
			MMOAssistData::getSingleton().UpdateAssistItemInfo(i, info);
		}
	}
	MMOFacade::getSingleton().EndLoadAccountBinary(archive);
#endif
}

void MMOAssistManager::_SaveAssistProtect()
{
#ifdef SAVE_TO_SERVER
	UINT32 data[PROTECT_DATA_COUNT];
	appMemset(data, 0, sizeof(UINT32) * PROTECT_DATA_COUNT);
	
	for (INT i = EPT_SLOW_HP; i < EPT_TOTAL; ++ i)
	{
		AssistItemInfo info;
		if (!MMOAssistData::getSingleton().GetAssistItemInfo(i, info))
		{
			continue;
		}

		UINT8 itemIndex = MMOAssistData::getSingleton().GetAssistItemIndexById(i, info.ItemId);
		data[ECVT_Assist_Protect_Item - ECVT_Assist_Protect_Begin] |= itemIndex << (4 * i);
		data[ECVT_Assist_Protect_Item - ECVT_Assist_Protect_Begin] |= info.Enable ? 1 << (4 * i + 3) : 0;

		if (info.Time < MIN_PROTECT_TIME)
		{
			info.Time = MIN_PROTECT_TIME;
		}

		switch (i)
		{
		case EPT_SLOW_HP:
			data[ECVT_Assist_Protect_Value1 - ECVT_Assist_Protect_Begin] |= MAKELONG(0, info.Value);
			data[ECVT_Assist_Protect_Time1 - ECVT_Assist_Protect_Begin] |= MAKELONG(0, MAKEWORD(0, info.Time));
			break;
		case EPT_SLOW_MP:
			data[ECVT_Assist_Protect_Value1 - ECVT_Assist_Protect_Begin] |= MAKELONG(info.Value, 0);
			data[ECVT_Assist_Protect_Time1 - ECVT_Assist_Protect_Begin] |= MAKELONG(0, MAKEWORD(info.Time, 0));
			break;
		case EPT_FAST_HP1:
			data[ECVT_Assist_Protect_Value2 - ECVT_Assist_Protect_Begin] |= MAKELONG(0, info.Value);
			data[ECVT_Assist_Protect_Time1 - ECVT_Assist_Protect_Begin] |= MAKELONG(MAKEWORD(0, info.Time), 0);
			break;
		case EPT_FAST_MP1:
			data[ECVT_Assist_Protect_Value2 - ECVT_Assist_Protect_Begin] |= MAKELONG(info.Value, 0);
			data[ECVT_Assist_Protect_Time1 - ECVT_Assist_Protect_Begin] |= MAKELONG(MAKEWORD(info.Time, 0), 0);
			break;
		case EPT_FAST_HP2:
			data[ECVT_Assist_Protect_Value3 - ECVT_Assist_Protect_Begin] |= MAKELONG(0, info.Value);
			data[ECVT_Assist_Protect_Time2 - ECVT_Assist_Protect_Begin] |= MAKELONG(0, MAKEWORD(0, info.Time));
			break;
		case EPT_FAST_MP2:
			data[ECVT_Assist_Protect_Value3 - ECVT_Assist_Protect_Begin] |= MAKELONG(info.Value, 0);
			data[ECVT_Assist_Protect_Time2 - ECVT_Assist_Protect_Begin] |= MAKELONG(0, MAKEWORD(info.Time, 0));
			break;
		case EPT_PROTECT_HP:
			data[ECVT_Assist_Protect_Value4 - ECVT_Assist_Protect_Begin] |= MAKELONG(0, info.Value);
			data[ECVT_Assist_Protect_Time2 - ECVT_Assist_Protect_Begin] |= MAKELONG(MAKEWORD(0, info.Time), 0);
			break;
		case EPT_PROTECT_MP:
			data[ECVT_Assist_Protect_Value4 - ECVT_Assist_Protect_Begin] |= MAKELONG(info.Value, 0);
			data[ECVT_Assist_Protect_Time2 - ECVT_Assist_Protect_Begin] |= MAKELONG(MAKEWORD(info.Time, 0), 0);
			break;
		}
	}

	for (INT i = 0; i < PROTECT_DATA_COUNT; ++ i)
	{
		MMOClientVariableManager::getSingleton().SetClientVariable((EClientVariableType)(ECVT_Assist_Protect_Begin + i), data[i]);
	}
#else
	FArchive* archive = NULL;
	if (MMOFacade::getSingleton().BeginExportAccountData(&archive, TEXT("AssistItem")))
	{
		INT size = EPT_TOTAL;
		archive->Serialize((void*)(&size), sizeof(INT));

		for (INT i = EPT_SLOW_HP; i < EPT_TOTAL; ++ i)
		{
			AssistItemInfo info;
			if (MMOAssistData::getSingleton().GetAssistItemInfo(i, info))
			{
				archive->Serialize((void*)(&info), sizeof(AssistItemInfo));
			}
		}
	}
	MMOFacade::getSingleton().EndExportAccountData(archive);
#endif
}

UBOOL MMOAssistManager::IsDropValuable(UINT32 id)
{
	return MMOAssistData::getSingleton().IsDropValuable(id);
}

UBOOL MMOAssistManager::IsDropAutoPick(UINT32 id)
{
	return MMOAssistData::getSingleton().IsDropAutoPick(id);
}

UBOOL MMOAssistManager::IsDropVisible(UINT32 id)
{
	return MMOAssistData::getSingleton().IsDropVisible(id);
}

void MMOAssistManager::UpdateDropDisplay(UINT32 id)
{
	UMMOUIDropItemTips* dropTipsUI = MMODropManager::getSingleton().GetDropTipsUI();
	if (!dropTipsUI)
	{
		return;
	}

	DropActorMap dropActorMap = MMODropManager::getSingleton().GetDropActorMap();
	DropActorMap::iterator iter = dropActorMap.begin();
	for (; iter != dropActorMap.end(); ++ iter)
	{
		AMMODropActor* dropActor = iter->second.DropActor;
		if (iter->second.Hud &&
			dropActor &&
			dropActor->ItemTemplate && 
			dropActor->ItemTemplate->ItemId == id)
		{
			FString iconName = "";
			INT realColor = UITextParser::HTMLGetItemRealColor((EItemColor)dropActor->ItemTemplate->ItemColor);

			iter->second.IsVisible = MMOAssistManager::getSingleton().IsDropVisible(id);
			iter->second.Hud->SetVisible(iter->second.IsVisible);

			if (MMOAssistManager::getSingleton().IsDropValuable(id))
			{
				iter->second.IsVisible = TRUE;
				iter->second.Hud->SetVisible(TRUE);
				realColor = _tcstol( TEXT("00ff00"), NULL, 16 );
			}

			dropTipsUI->eventSetHpInfo(iter->second.Hud, 
				dropActor->DropID, 
				UITextParser::HTMLFormatItemName(dropActor->ItemTemplate), 
				realColor, 
				iconName, 
				dropActor->ItemTemplate->ItemColor);
		}
	}
}

UBOOL MMOAssistManager::_UseItem(UINT32 itemId)
{
	UItem* item = MMOFacade::getSingleton().FindItemTemplate(itemId);
	if (!item)
	{
		return FALSE;
	}

	MMOFacade::getSingleton().SendOpenPackage(itemId);

	INT containerId = E_CONSLOT_DEFAULT;
	INT slotId = -1;
	MMOFacade::getSingleton().FindFirstItemPosInContainer(itemId, containerId, slotId);
	if (slotId >= 0)
	{
		CMessageHandler::GetInstance().Send_UseItem(containerId, slotId, itemId);
		return TRUE;
	}

	return FALSE;
}

void MMOAssistManager::_UseSlowHpItem(FLOAT deltaTime)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	if (!(mainPlayer->ActorAttributes))
	{
		return;
	}

	AssistItemInfo info;
	if (!MMOAssistData::getSingleton().GetAssistItemInfo(EPT_SLOW_HP, info))
	{
		return;
	}

	// If not enabled
	if (!info.Enable)
	{
		_AssistItemTimeProcess[info.ProtectType] = info.Time;
		return;
	}

	_AssistItemTimeProcess[info.ProtectType] += deltaTime;
	if (_AssistItemTimeProcess[info.ProtectType] >= info.Time)
	{
		_AssistItemTimeProcess[info.ProtectType] = info.Time;

		int curHp = mainPlayer->ActorAttributes->GetCurHp();
		if (curHp > 0 && curHp <= (INT)(info.Value))
		{
			AssistItemDataList slowHpList = MMOAssistData::getSingleton().GetAssistItemSlowHpList();
			AssistItemDataList::iterator iter = slowHpList.begin();
			for (; iter != slowHpList.end(); ++iter)
			{
				if (_UseItem(iter->ItemId))
				{
					_AssistItemTimeProcess[info.ProtectType] = 0.0f;
					break;
				}
			}
		}
	}
}

void MMOAssistManager::_UseSlowMpItem(FLOAT deltaTime)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	if (!(mainPlayer->ActorAttributes))
	{
		return;
	}

	AssistItemInfo info;
	if (!MMOAssistData::getSingleton().GetAssistItemInfo(EPT_SLOW_MP, info))
	{
		return;
	}

	if (!info.Enable)
	{
		_AssistItemTimeProcess[info.ProtectType] = info.Time;
		return;
	}

	_AssistItemTimeProcess[info.ProtectType] += deltaTime;
	if (_AssistItemTimeProcess[info.ProtectType] >= info.Time)
	{
		_AssistItemTimeProcess[info.ProtectType] = info.Time;

		int curMp = mainPlayer->ActorAttributes->GetCurMp();
		if (curMp <= (INT)(info.Value))
		{
			AssistItemDataList slowMpList = MMOAssistData::getSingleton().GetAssistItemSlowMpList();
			AssistItemDataList::iterator iter = slowMpList.begin();
			for (; iter != slowMpList.end(); ++iter)
			{
				if (_UseItem(iter->ItemId))
				{
					_AssistItemTimeProcess[info.ProtectType] = 0.0f;
					break;
				}
			}
		}
	}
}

void MMOAssistManager::_UseFastHpItem1(FLOAT deltaTime)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	if (!(mainPlayer->ActorAttributes))
	{
		return;
	}

	AssistItemInfo info;
	if (!MMOAssistData::getSingleton().GetAssistItemInfo(EPT_FAST_HP1, info))
	{
		return;
	}

	if (!info.Enable)
	{
		_AssistItemTimeProcess[info.ProtectType] = info.Time;
		return;
	}

	_AssistItemTimeProcess[info.ProtectType] += deltaTime;
	if (_AssistItemTimeProcess[info.ProtectType] >= info.Time)
	{
		_AssistItemTimeProcess[info.ProtectType] = info.Time;

		int curHp = mainPlayer->ActorAttributes->GetCurHp();
		if (curHp > 0 && curHp <= (INT)(info.Value))
		{
			if (_UseItem(info.ItemId))
			{
				_AssistItemTimeProcess[info.ProtectType] = 0.0f;
			}
		}
	}
}

void MMOAssistManager::_UseFastMpItem1(FLOAT deltaTime)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	if (!(mainPlayer->ActorAttributes))
	{
		return;
	}

	AssistItemInfo info;
	if (!MMOAssistData::getSingleton().GetAssistItemInfo(EPT_FAST_MP1, info))
	{
		return;
	}

	if (!info.Enable)
	{
		_AssistItemTimeProcess[info.ProtectType] = info.Time;
		return;
	}

	_AssistItemTimeProcess[info.ProtectType] += deltaTime;
	if (_AssistItemTimeProcess[info.ProtectType] >= info.Time)
	{
		_AssistItemTimeProcess[info.ProtectType] = info.Time;

		int curMp = mainPlayer->ActorAttributes->GetCurMp();
		if (curMp <= (INT)(info.Value))
		{
			if (_UseItem(info.ItemId))
			{
				_AssistItemTimeProcess[info.ProtectType] = 0.0f;
			}
		}
	}
}

void MMOAssistManager::_UseFastHpItem2(FLOAT deltaTime)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	if (!(mainPlayer->ActorAttributes))
	{
		return;
	}

	AssistItemInfo info;
	if (!MMOAssistData::getSingleton().GetAssistItemInfo(EPT_FAST_HP2, info))
	{
		return;
	}

	if (!info.Enable)
	{
		_AssistItemTimeProcess[info.ProtectType] = info.Time;
		return;
	}

	_AssistItemTimeProcess[info.ProtectType] += deltaTime;
	if (_AssistItemTimeProcess[info.ProtectType] >= info.Time)
	{
		_AssistItemTimeProcess[info.ProtectType] = info.Time;

		int curHp = mainPlayer->ActorAttributes->GetCurHp();
		if (curHp > 0 && curHp <= (INT)(info.Value))
		{
			if (_UseItem(info.ItemId))
			{
				_AssistItemTimeProcess[info.ProtectType] = 0.0f;
			}
		}
	}
}

void MMOAssistManager::_UseFastMpItem2(FLOAT deltaTime)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	if (!(mainPlayer->ActorAttributes))
	{
		return;
	}

	AssistItemInfo info;
	if (!MMOAssistData::getSingleton().GetAssistItemInfo(EPT_FAST_MP2, info))
	{
		return;
	}

	if (!info.Enable)
	{
		_AssistItemTimeProcess[info.ProtectType] = info.Time;
		return;
	}

	_AssistItemTimeProcess[info.ProtectType] += deltaTime;
	if (_AssistItemTimeProcess[info.ProtectType] >= info.Time)
	{
		_AssistItemTimeProcess[info.ProtectType] = info.Time;

		int curMp = mainPlayer->ActorAttributes->GetCurMp();
		if (curMp <= (INT)(info.Value))
		{
			if (_UseItem(info.ItemId))
			{
				_AssistItemTimeProcess[info.ProtectType] = 0.0f;
			}
		}
	}
}

void MMOAssistManager::_UseProtectHpItem(FLOAT deltaTime)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	if (!(mainPlayer->ActorAttributes))
	{
		return;
	}

	AssistItemInfo info;
	if (!MMOAssistData::getSingleton().GetAssistItemInfo(EPT_PROTECT_HP, info))
	{
		return;
	}

	if (!info.Enable)
	{
		_AssistItemTimeProcess[info.ProtectType] = info.Time;
		return;
	}

	_AssistItemTimeProcess[info.ProtectType] += deltaTime;
	if (_AssistItemTimeProcess[info.ProtectType] >= info.Time)
	{
		_AssistItemTimeProcess[info.ProtectType] = info.Time;
		
		int curHp = mainPlayer->ActorAttributes->GetCurHp();
		if (curHp > 0 && curHp <= (INT)(info.Value))
		{
			if (_UseItem(info.ItemId))
			{
				_AssistItemTimeProcess[info.ProtectType] = 0.0f;
			}
		}
	}
}

void MMOAssistManager::_UseProtectMpItem(FLOAT deltaTime)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	if (!(mainPlayer->ActorAttributes))
	{
		return;
	}

	AssistItemInfo info;
	if (!MMOAssistData::getSingleton().GetAssistItemInfo(EPT_PROTECT_MP, info))
	{
		return;
	}

	if (!info.Enable)
	{
		_AssistItemTimeProcess[info.ProtectType] = info.Time;
		return;
	}

	_AssistItemTimeProcess[info.ProtectType] += deltaTime;
	if (_AssistItemTimeProcess[info.ProtectType] >= info.Time)
	{
		_AssistItemTimeProcess[info.ProtectType] = info.Time;

		int curMp = mainPlayer->ActorAttributes->GetCurMp();
		if (curMp <= (INT)(info.Value))
		{
			if (_UseItem(info.ItemId))
			{
				_AssistItemTimeProcess[info.ProtectType] = 0.0f;
			}
		}
	}
}

void MMOAssistManager::_UpdateAssistItem(FLOAT deltaTime)
{
	if (MMO::GetCurrentGameState() != EGameState_InGame)
	{
		return;
	}

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	if (mainPlayer->eventIsDead())
	{
		return;
	}

	// If potions are strictly used by time, using one instantly on HP loss or low MP is not supported; the two conflict
	// If designers want to change this, have them see Bug #23647, it has been changed back and forth several times
	_AssistItemCheckTimeProcess += deltaTime;
	if (_AssistItemCheckTimeProcess >= ASSIST_ITEM_CHECK_TIME)
	{
		_AssistItemCheckTimeProcess = 0.0f;

		_UseSlowHpItem(ASSIST_ITEM_CHECK_TIME);
		_UseSlowMpItem(ASSIST_ITEM_CHECK_TIME);
		_UseFastHpItem1(ASSIST_ITEM_CHECK_TIME);
		_UseFastMpItem1(ASSIST_ITEM_CHECK_TIME);
		_UseFastHpItem2(ASSIST_ITEM_CHECK_TIME);
		_UseFastMpItem2(ASSIST_ITEM_CHECK_TIME);
		_UseProtectHpItem(ASSIST_ITEM_CHECK_TIME);
		_UseProtectMpItem(ASSIST_ITEM_CHECK_TIME);
	}
}

UBOOL MMOAssistManager::IsQuestMode()
{
	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		return uiManager->QuestMode;
	}

	return FALSE;
}