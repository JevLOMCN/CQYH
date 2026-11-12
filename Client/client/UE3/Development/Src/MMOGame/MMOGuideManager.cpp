#include "stdafx.h"
#include "MMOGuideManager.h"
#include "MMOFacade.h"
#include "Inc\MMOGameUIClasses.h"
#include "MMOQuestManager.h"

#if WITH_GFx
#include "GFxUI.h"
#include "GFxUIClasses.h"
#include "ScaleformEngine.h"
#include "GFx/GFx_Event.h"
#endif //WITH_GFx

#include "ScaleformEngine.h"
#include "GFxUIClasses.h"
#include "GFxUIUISequenceClasses.h"
#include "EngineUIPrivateClasses.h"
//#include "ScaleformImageLoader.h"
#include "ScaleformFile.h"
#include "ScaleformFont.h"
#include "ScaleformFullscreenMovie.h"
#include "ScaleformStats.h"
#include "ScaleformAllocator.h"
#include "../../Engine/Src/SceneRenderTargets.h"

#include "Kernel/SF_Types.h"
#include "Kernel/SF_RefCount.h"
#include "Render/Render_HAL.h"
#include "Render/Renderer2D.h"
#include "GFx/GFx_Player.h"
#include "Kernel/SF_File.h"
#include "GFx/GFx_Loader.h"
#include "GFx/GFx_FontLib.h"
#include "MMOStats.h"
#include "TeamManager.h"
#include "MMOClientVariableManager.h"
#include "MMOSkillManager.h"

using namespace ExternalMessage;

MMOGuideManager::MMOGuideManager()
{
	MMOUI::RegisterEventHandler(this, GameEvent_LeaveSelectingRole);
	MMOUI::RegisterEventHandler(this, GameEvent_Guide);
}

MMOGuideManager::~MMOGuideManager()
{
	MMOUI::UnRegisterEventHandler(this, GameEvent_LeaveSelectingRole);
	MMOUI::UnRegisterEventHandler(this, GameEvent_Guide);
}

void MMOGuideManager::Init()
{
	MMOGuideData::getSingleton().Init();
	_IsGuideEnable = TRUE;
}

void MMOGuideManager::Shutdown()
{
	MMOGuideData::getSingleton().Shutdown();
}

void MMOGuideManager::Clear()
{
	_DetachAllGuidePanel();
	_CurNpcId = 0;
	_CurDoodadType = E_dt_Invalid;
	_CurZoneGuideId = 0;
	_IsLoadRecord = FALSE;
}

void MMOGuideManager::Update(FLOAT deltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_GuideTime);
	CTRL_TICK("MMOGuideManager");

	_UpdateAllGuidePanel();
}

void MMOGuideManager::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	if (!evt)
	{
		return;
	}

	switch (evt->GetEventType())
	{
	case GameEvent_Guide:
		{
			MMOGameEvent_Guide* evtGuide = (MMOGameEvent_Guide*)evt;
			if (evtGuide)
			{
				if (!_IsLoadRecord)
				{
					return;
				}

				GuideEventMap deactiveGuideEventMap = MMOGuideData::getSingleton().GetDeactiveGuideEventMap();
				GuideEventMap::iterator iterDeactive = deactiveGuideEventMap.find(evtGuide->_EventType);
				if (iterDeactive != deactiveGuideEventMap.end())
				{
					if (evtGuide->_EventType == EGUET_CloseGuidePanel)
					{
						_DeactiveGuide(evtGuide->_Value1);
					} 
					else
					{
						GuideList& list = iterDeactive->second;
						GuideList::iterator iterList = list.begin();
						for (; iterList != list.end(); ++ iterList)
						{
							_DeactiveGuide(*iterList);
						}
					}
				}

				switch (evtGuide->_EventType)
				{
				case EGUET_EnterZone:
					_CurZoneGuideId = evtGuide->_Value1;
					break;

				case EGUET_KillMonster:
				case EGUET_OpenQuestDialogSinglePage:
				case EGUET_OpenQuestDialogMultiPage:
				case EGUET_OpenNpcDialog:
					_CurNpcId = evtGuide->_Value1;
					break;
				}

				GuideEventMap activateGuideEventMap = MMOGuideData::getSingleton().GetActivateGuideEventMap();
				GuideEventMap::iterator iterActivate = activateGuideEventMap.find(evtGuide->_EventType);
				if (iterActivate != activateGuideEventMap.end())
				{
					GuideList& list = iterActivate->second;
					GuideList::iterator iterList = list.begin();
					for (; iterList != list.end(); ++ iterList)
					{
						_ActivateGuide(*iterList);
					}
				}
			}
		}
		break;
	}
}

void MMOGuideManager::LoadRecord(BYTE* buf, UINT size)
{
	_GuideObjectMap.clear();
	MMOGuideData::getSingleton().ClearGuideState();
	
	_IsLoadRecord = TRUE;

	if (!buf || size <= 0)
	{
		return;
	}
	
	for (UINT index = 0; index < size; ++ index)
	{
		for (UINT pos = 0; pos < 8; ++ pos)
		{
			UINT32 id = (index * 8) + pos + 1;
			GuideState state = (buf[index] & (1 << pos)) > 0 ? GS_Complete : GS_Unactivated;
			MMOGuideData::getSingleton().UpdateGuideState(id, state);
		}
	}
}

void MMOGuideManager::SaveRecord()
{
	for (UINT index = 0; index < (ECVT_Guide_End - ECVT_Guide_Begin); ++ index)
	{
		UINT32 variable = 0;

		for (UINT pos = 0; pos < 32; ++ pos)
		{
			UINT32 id = (index * 32) + pos + 1;
			GuideStateMap guideStateMap = MMOGuideData::getSingleton().GetGuideStateMap();
			GuideStateMap::iterator iter = guideStateMap.find(id);
			if (iter != guideStateMap.end())
			{
				if (iter->second == GS_Activated ||
					iter->second == GS_Complete)
				{
					variable |= 1 << pos;
				}
			}
		}

		MMOClientVariableManager::getSingleton().SetClientVariable((EClientVariableType)(ECVT_Guide_Begin + index), variable);
	}
}

void MMOGuideManager::SetGuideEnable(UBOOL isEnable)
{
	_IsGuideEnable = isEnable;
	SaveRecord();
}

UBOOL MMOGuideManager::_PreGuideCheck(UINT32 id)
{
	GuideData data;
	if (MMOGuideData::getSingleton().GetGuideData(id, data))
	{
		if (data.GuideReq == 0)
		{
			return TRUE;
		}
		else
		{
			if (MMOGuideData::getSingleton().GetGuideState(data.GuideReq) == GS_Complete)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

UBOOL MMOGuideManager::_LevelCheck(UINT32 id)
{
	GuideData data;
	if (MMOGuideData::getSingleton().GetGuideData(id, data))
	{
		if (data.LevelReq == 0)
		{
			return TRUE;
		}
		else
		{
			AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if (mainPlayer)
			{
				UActorAttribute* attri = mainPlayer->ActorAttributes;
				if (attri && attri->LevelAttributes.Level >= (INT)(data.LevelReq))
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

UBOOL MMOGuideManager::_QuestCheck(UINT32 id)
{
	GuideData data;
	if (MMOGuideData::getSingleton().GetGuideData(id, data))
	{
		if (data.QuestReq == 0)
		{
			return TRUE;
		}
		else if (MMOQuestData::getSingleton().IsQuestAccepted(data.QuestReq))
		{
			return TRUE;
		}
	}

	return FALSE;
}

UBOOL MMOGuideManager::_ItemCheck(UINT32 id)
{
	GuideData data;
	if (MMOGuideData::getSingleton().GetGuideData(id, data))
	{
		if (MMOFacade::getSingleton().GetMainActor())
		{
			int playerJob = MMOFacade::getSingleton().GetMainActor()->GetClassId();
			// 1.05 Lancer / Sorcerer: guide tables only have 5 per-class columns -> no requirement
			if (playerJob == EPCT_PlayerClassG || playerJob == EPCT_PlayerClassH) { return TRUE; }
			if (playerJob >= EPCT_PlayerClassA && playerJob <= EPCT_PlayerClassE)
			{
				if (data.ItemReq[playerJob] == 0)
				{
					return TRUE;
				}
				else
				{
					INT itemSlot = -1;
					FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindFirstItemPosInContainer(data.ItemReq[playerJob], E_CONSLOT_DEFAULT, itemSlot);
					if (itemInfo && itemSlot >= 0)
					{
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}

UBOOL MMOGuideManager::_LearnSkillCheck(UINT32 id)
{
	GuideData data;
	if (MMOGuideData::getSingleton().GetGuideData(id, data))
	{
		if (MMOFacade::getSingleton().GetMainActor())
		{
			int playerJob = MMOFacade::getSingleton().GetMainActor()->GetClassId();
			// 1.05 Lancer / Sorcerer: guide tables only have 5 per-class columns -> no requirement
			if (playerJob == EPCT_PlayerClassG || playerJob == EPCT_PlayerClassH) { return TRUE; }
			if (playerJob >= EPCT_PlayerClassA && playerJob <= EPCT_PlayerClassE)
			{
				if (data.SkillReq[playerJob] == 0)
				{
					return TRUE;
				}
				else
				{
					const UMMOAbility* skill = MMOSkillManager::getSingleton().FindMyLearnedMainSkill(data.SkillReq[playerJob]);
					if (skill)
					{
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}

UBOOL MMOGuideManager::_ClassCheck(UINT32 id)
{
	GuideData data;
	if (MMOGuideData::getSingleton().GetGuideData(id, data))
	{
		if (data.ClassReq == EPCT_None)
		{
			return TRUE;
		}
		else
		{
			int playerJob = MMOFacade::getSingleton().GetMainActor()->GetClassId();
			if (data.ClassReq == (EPlayerClass)playerJob)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

UBOOL MMOGuideManager::_NpcCheck(UINT32 id)
{
	GuideData data;
	if (MMOGuideData::getSingleton().GetGuideData(id, data))
	{
		if (data.NpcReq == 0)
		{
			return TRUE;
		}
		else if (data.NpcReq == _CurNpcId)
		{
			_CurNpcId = 0;
			return TRUE;
		}
	}

	return FALSE;
}

UBOOL MMOGuideManager::_DoodadTypeCheck(UINT32 id)
{
	GuideData data;
	if (MMOGuideData::getSingleton().GetGuideData(id, data))
	{
		if (data.DoodadTypeReq == E_dt_Invalid)
		{
			return TRUE;
		}
		else if (data.DoodadTypeReq == _CurDoodadType)
		{
			_CurDoodadType = E_dt_Invalid;
			return TRUE;
		}
	}

	return FALSE;
}

UBOOL MMOGuideManager::_EnterZoneCheck(UINT32 id)
{
	GuideData data;
	if (MMOGuideData::getSingleton().GetGuideData(id, data))
	{
		if (data.ActivateType != EGUET_EnterZone)
		{
			return TRUE;
		}
		else if (data.Id == _CurZoneGuideId)
		{
			_CurZoneGuideId = 0;
			return TRUE;
		}
	}
	return FALSE;
}

UGFxObject* MMOGuideManager::_GetTargetComponent(UGFxObject* targetPanel, const GuideData& data)
{
	UGFxObject* targetComp = targetPanel;
	for (INT i = 0; i < MAX_TARGET_COMPONENT_COUNT; ++ i)
	{
		UGFxObject* targetCompTemp = targetComp->GetObject(data.TargetComponentName[i]);
		if (targetCompTemp)
		{
			targetComp = targetCompTemp;
		}
		else
		{
			break;
		}
	}

	return targetComp;
}

void MMOGuideManager::_AttachGuidePanel(UINT32 id)
{
	GuideData data;
	if (!MMOGuideData::getSingleton().GetGuideData(id, data))
	{
		return;
	}

	if (data.Type == EGDT_Null)
	{
		return;
	}

	UMMOUIBase* targetPanel = MMOUI::FindUIPanelEx(FName(*(data.TargetWindowName)));
	if (!targetPanel)
	{
		return;
	}

	UGFxObject* targetComp = _GetTargetComponent(targetPanel, data);
	if (!targetComp)
	{
		return;
	}

	UGFxObject* guide = NULL;
	if (data.TraceMenuBar)
	{
		FString name = FString::Printf(TEXT("guide_menu_%d_%d_mc"), data.TraceMenuType, data.TraceMenuChildType);
		guide = targetComp->AttachMovie(TEXT("BeginnersGuide"), *name);
	} 
	else if (data.TraceJourney)
	{
		FString name = FString::Printf(TEXT("guide_journey_%d_mc"), data.TraceJourneyType);
		guide = targetComp->AttachMovie(TEXT("BeginnersGuide"), *name);
	}
	else
	{
		guide = targetComp->AttachMovie(TEXT("BeginnersGuide"), TEXT("guide_mc"));
	}

	if (guide)
	{
		guide->AddToRoot();
		_GuideObjectMap.insert(std::make_pair(id, guide));

		UGFxObject* guidePanel = guide->GetObject(TEXT("guide_panel_mc"));
		if (guidePanel)
		{
			TArray<struct FASValue> args;
			
			FASValue guideId(EC_EventParm);
			guideId.Type = AS_Int;
			guideId.I = id;
			args.AddItem(guideId);

			FASValue guideType(EC_EventParm);
			guideType.Type = AS_Int;
			guideType.I = data.Type;
			args.AddItem(guideType);

			data.Desc.ReplaceInline(TEXT("MouseL"), TEXT("<IMG SRC='img://UI_GuideWidget.MouseL' />"));
			data.Desc.ReplaceInline(TEXT("MouseR"), TEXT("<IMG SRC='img://UI_GuideWidget.MouseR' />"));

			FASValue desc(EC_EventParm);
			desc.Type = AS_String;
			desc.S = data.Desc;
			args.AddItem(desc);

			FASValue pictureName(EC_EventParm);
			pictureName.Type = AS_String;
			INT playerClass = MMOFacade::getSingleton().GetMainActor()->GetClassId();
			if (playerClass < MAX_PLAYER_CLASS_COUNT)
			{
				pictureName.S = FString(TEXT("img://")) + data.Picture[playerClass];
			}
			args.AddItem(pictureName);

			FASValue offsetX(EC_EventParm);
			offsetX.Type = AS_Int;
			offsetX.I = data.Offset.X;
			args.AddItem(offsetX);

			FASValue offsetY(EC_EventParm);
			offsetY.Type = AS_Int;
			offsetY.I = data.Offset.Y;
			args.AddItem(offsetY);

			FASValue withBox(EC_EventParm);
			withBox.Type = AS_Boolean;
			withBox.B = data.IsBoxed;
			args.AddItem(withBox);

			FASValue boxOffsetX(EC_EventParm);
			boxOffsetX.Type = AS_Int;
			boxOffsetX.I = data.BoxOffset.X;
			args.AddItem(boxOffsetX);

			FASValue boxOffsetY(EC_EventParm);
			boxOffsetY.Type = AS_Int;
			boxOffsetY.I = data.BoxOffset.Y;
			args.AddItem(boxOffsetY);

			FASValue boxWidth(EC_EventParm);
			boxWidth.Type = AS_Int;
			boxWidth.I = data.BoxSize.X;
			args.AddItem(boxWidth);

			FASValue boxHeight(EC_EventParm);
			boxHeight.Type = AS_Int;
			boxHeight.I = data.BoxSize.Y;
			args.AddItem(boxHeight);

			FASValue boxType(EC_EventParm);
			boxType.Type = AS_Int;
			boxType.I = (INT)(data.BoxType);
			args.AddItem(boxType);
			
			FASValue imgType(EC_EventParm);
			imgType.Type = AS_Int;
			imgType.I = (INT)(data.ArrowTipsType);
			args.AddItem(imgType);

			guidePanel->Invoke(TEXT("addGuideElement"), args);
		}
	}

	if (data.TraceMenuBar)
	{
		UMMOUIManager* uiManager = MMOUI::GetUIManager();
		if (uiManager)
		{
			UMMOUIMenuBar* menuBar = Cast<UMMOUIMenuBar>(uiManager->FindUIPanel(uiManager->Panel_Skillbar_Menubar));
			if (menuBar)
			{
				INT menuIndex = menuBar->GetMenuIdxByMenuType(data.TraceMenuType, data.TraceMenuBarType == ETMBT_Left);
				FString name = FString::Printf(TEXT("guide_menu_%d_%d_mc"), data.TraceMenuType, data.TraceMenuChildType);
				menuBar->eventCallAddGuideMenuEffect(menuIndex, data.TraceMenuChildType, data.TraceMenuBarType == ETMBT_Left, TRUE, name);
			}
		}
	}
	else if (data.TraceJourney)
	{
		UMMOUIManager* uiManager = MMOUI::GetUIManager();
		if (uiManager)
		{
			UMMOUIJourneyTracePanel* journeyTrace = Cast<UMMOUIJourneyTracePanel>(uiManager->FindUIPanel(uiManager->Panel_Journey));
			if (journeyTrace)
			{
				FString name = FString::Printf(TEXT("guide_journey_%d_mc"), data.TraceJourneyType);
				journeyTrace->eventCallShowActivateJourneyType(TRUE, data.TraceJourneyType, name);
			}
		}
	}
}

void MMOGuideManager::_DetachGuidePanel(UINT32 id)
{
	GuideData data;
	if (!MMOGuideData::getSingleton().GetGuideData(id, data))
	{
		return;
	}

	if (data.Type == EGDT_Null)
	{
		return;
	}

	// not found
	GuideObjectMap::iterator iter = _GuideObjectMap.find(id);
	if (iter == _GuideObjectMap.end())
	{
		return;
	}

	UMMOUIBase* targetPanel = MMOUI::FindUIPanelEx(FName(*(data.TargetWindowName)));
	if (!targetPanel)
	{
		return;
	}

	UGFxObject* guide = iter->second;
	if (guide)
	{
		if (data.TraceMenuBar)
		{
			UMMOUIManager* uiManager = MMOUI::GetUIManager();
			if (uiManager)
			{
				UMMOUIMenuBar* menuBar = Cast<UMMOUIMenuBar>(uiManager->FindUIPanel(uiManager->Panel_Skillbar_Menubar));
				if (menuBar)
				{
					INT menuIndex = menuBar->GetMenuIdxByMenuType(data.TraceMenuType, data.TraceMenuBarType == ETMBT_Left);
					FString name = FString::Printf(TEXT("guide_menu_%d_%d_mc"), data.TraceMenuType, data.TraceMenuChildType);
					menuBar->eventCallAddGuideMenuEffect(menuIndex, data.TraceMenuChildType, data.TraceMenuBarType == ETMBT_Left, FALSE, name);
				}
			}
		}
		else if (data.TraceJourney)
		{
			UMMOUIManager* uiManager = MMOUI::GetUIManager();
			if (uiManager)
			{
				UMMOUIJourneyTracePanel* journeyTrace = Cast<UMMOUIJourneyTracePanel>(uiManager->FindUIPanel(uiManager->Panel_Journey));
				if (journeyTrace)
				{
					FString name = FString::Printf(TEXT("guide_journey_%d_mc"), data.TraceJourneyType);
					journeyTrace->eventCallShowActivateJourneyType(FALSE, data.TraceJourneyType, name);
				}
			}
		}

		UGFxObject* guidePanel = guide->GetObject(TEXT("guide_panel_mc"));
		if (guidePanel)
		{
			TArray<struct FASValue> args;
			FASValue dispType(EC_EventParm);
			dispType.Type = AS_Int;
			dispType.I = data.Type;
			args.AddItem(dispType);

			guidePanel->Invoke(TEXT("removeGuideElement"), args);
		}

		guide->RemoveFromRoot();
		targetPanel->eventRemoveChild(guide);
	}

	_GuideObjectMap.erase(iter);
}

void MMOGuideManager::_DetachAllGuidePanel()
{
	GuideObjectMap::iterator iter = _GuideObjectMap.begin();
	for (; iter != _GuideObjectMap.end();)
	{
		GuideData data;
		if (MMOGuideData::getSingleton().GetGuideData(iter->first, data))
		{
			UGFxObject* guide = iter->second;
			if (guide)
			{
				UGFxObject* guidePanel = guide->GetObject(TEXT("guide_panel_mc"));
				if (guidePanel)
				{
					TArray<struct FASValue> args;
					FASValue dispType(EC_EventParm);
					dispType.Type = AS_Int;
					dispType.I = data.Type;
					args.AddItem(dispType);

					guidePanel->Invoke(TEXT("removeGuideElement"), args);
				}
				guide->RemoveFromRoot();
			}

			UMMOUIManager* uiManager = MMOUI::GetUIManager();
			if (uiManager)
			{
				UMMOUIMenuBar* menuBar = Cast<UMMOUIMenuBar>(uiManager->FindUIPanel(uiManager->Panel_Skillbar_Menubar));
				if (menuBar)
				{
					menuBar->eventCallClearGuideMenuEffect();
				}

				UMMOUIJourneyTracePanel* journeyTrace = Cast<UMMOUIJourneyTracePanel>(uiManager->FindUIPanel(uiManager->Panel_Journey));
				if (journeyTrace)
				{
					journeyTrace->eventCallClearGuideEffect();
				}
			}

			UMMOUIBase* targetPanel = MMOUI::FindUIPanelEx(FName(*(data.TargetWindowName)));
			if (targetPanel)
			{
				targetPanel->eventRemoveChild(guide);
			}
		}

		iter = _GuideObjectMap.erase(iter);
	}
	_GuideObjectMap.clear();
}

void MMOGuideManager::_PlayGuideSound(UINT32 id)
{
	GuideData data;
	if (!MMOGuideData::getSingleton().GetGuideData(id, data))
	{
		return;
	}

	if (!(data.Sound.IsEmpty()))
	{
		UMMOUIHelperPlayerPanel* panel = (UMMOUIHelperPlayerPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_HelperPlayer);
		if (panel)
		{
			USoundCue* sound = LoadUEObject<USoundCue>(data.Sound, TEXT("guide.txt"));
			panel->ShowPlayerPanel(data.SoundIcon, sound);
		}
	}
}

UBOOL MMOGuideManager::_ConditionCheck(UINT32 id)
{
	if (!_PreGuideCheck(id))
	{
		return FALSE;
	}

	if (!_LevelCheck(id))
	{
		return FALSE;
	}

	if (!_QuestCheck(id))
	{
		return FALSE;
	}

	if (!_ItemCheck(id))
	{
		return FALSE;
	}

	if (!_NpcCheck(id))
	{
		return FALSE;
	}

	if (!_DoodadTypeCheck(id))
	{
		return FALSE;
	}

	if (!_EnterZoneCheck(id))
	{
		return FALSE;
	}

	if (!_LearnSkillCheck(id))
	{
		return FALSE;
	}

	if (!_ClassCheck(id))
	{
		return FALSE;
	}

	return TRUE;
}

void MMOGuideManager::_UpdateGuideState(UINT32 id)
{
	GuideData data;
	if (!MMOGuideData::getSingleton().GetGuideData(id, data))
	{
		return;
	}

	// voice type guide, completes once activated
	if (data.Type == EGDT_Sound)
	{
		MMOGuideData::getSingleton().UpdateGuideState(id, GS_Complete);
		return;
	}

	MMOGuideData::getSingleton().UpdateGuideState(id, GS_Activated);
}

void MMOGuideManager::_ActivateGuide(UINT32 id)
{
	if (!_IsGuideEnable)
	{
		return;
	}

	if (!MMOGuideData::getSingleton().IsGuideEnable(id))
	{
		return;
	}

	if (MMOGuideData::getSingleton().GetGuideState(id) != GS_Unactivated)
	{
		return;
	}

	if (!_ConditionCheck(id))
	{
		return;
	}

	_AttachGuidePanel(id);
	_PlayGuideSound(id);
	_UpdateGuideState(id);
	SaveRecord();
}

void MMOGuideManager::_DeactiveGuide(UINT32 id)
{
	// currently not activated
	if (MMOGuideData::getSingleton().GetGuideState(id) != GS_Activated)
	{
		return;
	}

	_DetachGuidePanel(id);

	// record state
	MMOGuideData::getSingleton().UpdateGuideState(id, GS_Complete);
	SaveRecord();
}


void MMOGuideManager::_UpdateAllGuidePanel()
{
	GuideObjectMap::iterator iter = _GuideObjectMap.begin();
	for (; iter != _GuideObjectMap.end(); ++ iter)
	{
		GuideData data;
		if (MMOGuideData::getSingleton().GetGuideData(iter->first, data))
		{
			UMMOUIBase* targetPanel = MMOUI::FindUIPanelEx(FName(*(data.TargetWindowName)));
			if (targetPanel && targetPanel->IsVisible)
			{
				UGFxObject* guide = iter->second;
				if (guide)
				{
					if (data.IsBoxed)
					{
						_UpdateGuideBox(guide, data);
					}

					if (data.TraceBagSlot)
					{
						_UpdateBagSlotGuide(targetPanel, guide, data);
					}
				}
			}
		}
	}
}


void MMOGuideManager::_UpdateGuideBox(UGFxObject* guide, const GuideData& data)
{
	if (!guide)
	{
		return;
	}

	UGFxObject* guidePanel = guide->GetObject(TEXT("guide_panel_mc"));
	if (guidePanel)
	{
		UGFxObject* guideArrow = guidePanel->GetObject(TEXT("guide_arrow_mc"));
		if (guideArrow)
		{
			UGFxObject* guideBox = guideArrow->GetObject(TEXT("guide_box_mc"));
			if (guideBox)
			{
				guideBox->SetCustomFlag(0x20000, true);
			}
		}
	}
}

void MMOGuideManager::_UpdateBagSlotGuide(UMMOUIBase* targetPanel, UGFxObject* guide, const GuideData& data)
{
	if (!targetPanel)
	{
		return;
	}

	if (!guide)
	{
		return;
	}

	UMMOUIBag* bagPanel = Cast<UMMOUIBag>(targetPanel);
	if (!bagPanel)
	{
		return;
	}

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	// needs to aim at the bag slot
	int playerJob = mainPlayer->GetClassId();
	if (playerJob >= EPCT_PlayerClassA && playerJob <= EPCT_PlayerClassE)
	{
		INT itemSlot = -1;
		FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindFirstItemPosInContainer(data.ItemReq[playerJob], E_CONSLOT_DEFAULT, itemSlot);
		if (itemInfo && itemSlot >= 0)
		{
			UGFxObject* slotObject = bagPanel->ItemSlotList(itemSlot);
			if (slotObject)
			{
				FLOAT slotX = 0.0f;
				FLOAT slotY = 0.0f;
				slotObject->GetPosition(slotX, slotY);

				FLOAT slotWidth = slotObject->GetFloat("width");
				FLOAT slotHeight = slotObject->GetFloat("height");

				guide->SetPosition(slotX + data.Offset.X, slotY + data.Offset.Y);
			}
		}
	}
}

void MMOGuideManager::TestGuideArrow(INT id, INT offsetX, INT offsetY)
{
	GuideDataMap& guideDataMap = MMOGuideData::getSingleton().GetGuideDataMap();
	GuideDataMap::iterator iter = guideDataMap.find(id);
	if (iter != guideDataMap.end())
	{
		iter->second.Offset.X = offsetX;
		iter->second.Offset.Y = offsetY;
	}

	_DetachGuidePanel(id);
	_AttachGuidePanel(id);
}

void MMOGuideManager::TestGuideBox(INT id, INT offsetX, INT offsetY, INT width, INT height)
{
	GuideDataMap& guideDataMap = MMOGuideData::getSingleton().GetGuideDataMap();
	GuideDataMap::iterator iter = guideDataMap.find(id);
	if (iter != guideDataMap.end())
	{
		iter->second.IsBoxed = true;
		iter->second.BoxOffset.X = offsetX;
		iter->second.BoxOffset.Y = offsetY;
		iter->second.BoxSize.X = width;
		iter->second.BoxSize.Y = height;
	}

	_DetachGuidePanel(id);
	_AttachGuidePanel(id);
}