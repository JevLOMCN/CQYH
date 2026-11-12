#include "stdafx.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMODropManager.h"
#include "MMOWeatherManager.h"
#include "MMOChatManager.h"
#include "TeamManager.h"
#include "FriendManager.h"
#include "FSMIO.h"
#include "MMOClientVariableManager.h"
#include "MMOMsgBoxManager.h"

IMPLEMENT_CLASS(UMMOKeyManager)

extern bool g_disableAllInput;
extern UBOOL g_focusInTextInput;

static INT GetTypeHash(const UMMOKeyManager::KKeyworkd& n)
{
	return (INT)*(WORD*)&n;
}

UMMOKeyManager::UMMOKeyManager()
	:nKeyModifying(KF_Unknown)
{
	nCurVersion = 0;
	nKeyModifyingKeyPressed=FALSE;
	// init key names
	vmKeyNames.Set(VK_LBUTTON,	_T("LMB"));
	vmKeyNames.Set(VK_RBUTTON,	_T("RMB"));
	vmKeyNames.Set(VK_UP,		_T("¡ü"));
	vmKeyNames.Set(VK_DOWN,		_T("¡ý"));
	vmKeyNames.Set(VK_LEFT,		_T("¡û"));
	vmKeyNames.Set(VK_RIGHT,	_T("¡ú"));
	vmKeyNames.Set(VK_TAB,		_T("TAB"));
	vmKeyNames.Set(VK_CAPITAL,	_T("CAPS"));
	vmKeyNames.Set(VK_ESCAPE,	_T("ESC"));
	vmKeyNames.Set(VK_F9,		_T("F9"));
	vmKeyNames.Set(VK_OEM_3,	_T("~"));
	vmKeyNames.Set(VK_NUMLOCK,	_T("NumLock"));
	
	TCHAR chs[2] = {0};
	TCHAR& k = chs[0];
	for (k = _T('A'); k <= _T('Z'); ++k)
		vmKeyNames.Set(k, chs);
	for (k = _T('0'); k <= _T('9'); ++k)
		vmKeyNames.Set(k, chs);

	TCHAR fks[16] = {0};
	for (k = VK_F1; k <= VK_F12; ++k)
	{
		appSprintf(fks, _T("F%d"), k-VK_F1+1);
		vmKeyNames.Set(k, fks);
	}
	for (k = VK_NUMPAD0; k <= VK_NUMPAD0; ++k)
	{
		appSprintf(fks, _T("NUM%d"), k-VK_NUMPAD0+1);
		vmKeyNames.Set(k, fks);
	}

	// init function names
	// Hotkey bar does not need initialization
	vmFuncNames.Set(KF_OpenRole,			_T("Character"));
	vmFuncNames.Set(KF_OpenBag,				_T("Bag"));
	vmFuncNames.Set(KF_OpenHeaven,			_T("Dragon Guard Art"));
	vmFuncNames.Set(KF_OpenTask,			_T("Quests"));
	//vmFuncNames.Set(KF_OpenPlayerInfoView,	_T("View Info"));
	//vmFuncNames.Set(KF_OpenIntensify,		_T("Body Enhancement"));
	//vmFuncNames.Set(KF_OpenMount,			_T("Soul Inlay"));
	//vmFuncNames.Set(KF_OpenBaptize,			_T("Equipment Refinement"));
	vmFuncNames.Set(KF_OpenGuild,			_T("Guild"));
	vmFuncNames.Set(KF_OpenFriend,			_T("Friends"));
	vmFuncNames.Set(KF_OpenMaster,			_T("Master & Apprentice"));
	vmFuncNames.Set(KF_OpenTeam,			_T("Party"));
	vmFuncNames.Set(KF_OpenRide,			_T("Mount"));
//	vmFuncNames.Set(KF_OpenHero,			_T("Hero"));
	vmFuncNames.Set(KF_OpenAchieve,			_T("Achievement"));
	vmFuncNames.Set(KF_OpenOption,			_T("Settings"));
	vmFuncNames.Set(KF_QuickPick,			_T("Pick Up"));
	vmFuncNames.Set(KF_OpenMinimap,			_T("Map"));
	vmFuncNames.Set(KF_ToggleMinimapType,	_T("Map")); //_T("Map Mode"));
	//vmFuncNames.Set(KF_OpenSkill,			_T("Ability Setup"));
	//vmFuncNames.Set(KF_OpenArena,			_T("Arena"));
	vmFuncNames.Set(KF_OpenActivity,		_T("Event Schedule"));
	vmFuncNames.Set(KF_OpenRank,			_T("Rankings"));
	vmFuncNames.Set(KF_OpenSkillUp,			_T("Skills"));
	vmFuncNames.Set(KF_OpenReputation,		_T("Mafa Legends"));
	vmFuncNames.Set(KF_OpenStoryQuest,		_T("Story Synopsis"));
	vmFuncNames.Set(KF_GoldStore,			_T("Gold Exchange"));

	vmFuncNames.Set(KF_Mail,		_T("Mail"));
	vmFuncNames.Set(KF_Mall,		_T("Mall"));
	vmFuncNames.Set(KF_Handbook,	_T("Baixiaosheng"));
	vmFuncNames.Set(KF_Assist,	    _T("Game Settings"));

	vmFuncNames.Set(KF_ChangePkMode,		_T("Switch PK Mode"));

	vmFuncNames.Set(KF_ChangeRole,			_T("Change Character"));
	vmFuncNames.Set(KF_EnterGroup,			_T("Quick Party"));
	vmFuncNames.Set(KF_LeaveGroup,			_T("Quick Leave Party"));
	vmFuncNames.Set(KF_AddAttention,		_T("Add Follow"));
	vmFuncNames.Set(KF_AddEnemy,			_T("Add Enemy"));
	vmFuncNames.Set(KF_AddBlackList,		_T("Add to Blacklist"));
	vmFuncNames.Set(KF_ShowHidePlayer,		_T("Show/Hide Players"));
	vmFuncNames.Set(KF_ChangePetAI,		_T("Switch Pet State"));
	vmFuncNames.Set(KF_KeepRunning,			_T("Auto-Run Forward"));
	vmFuncNames.Set(KF_AUCTION, _T("Supreme Auction House"));

	// bind default keys
	DefaultBar(FALSE);
	DefaultFunc(FALSE);

	nModfGCTType = MMOFacade::getSingleton().GCTType;
}

UMMOKeyManager* UMMOKeyManager::getSingleton()
{
	return Cast<UMMOKeyManager>(UMMOKeyManager::StaticClass()->GetDefaultObject());
}

void UMMOKeyManager::InitKeyManager()
{
	GEngine->GameViewport->eventInsertInteraction(this);
}

void UMMOKeyManager::ASMdfFunction(INT type)
{
	MdfKeyBegin((EKeyFunction)type);
}

void UMMOKeyManager::ASDefaultInput(INT input_config)
{
	switch (input_config)
	{
	case 1:
		DefaultBar(TRUE);
		break;
	case 2:
		DefaultFunc(TRUE);
		break;
	default:
		return;
	}
	UMMOUIAssistPanel* assistPanel = Cast<UMMOUIAssistPanel>(MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Assist));
	if (assistPanel)
	{
		assistPanel->ModifyAssistInputData(TRUE, nModfGCTType);
	}
}

void UMMOKeyManager::ASSetContralType(INT ctrlType)
{
	if(ctrlType>=GC_DIABLO3 && ctrlType<=GC_MIRFOREVER){
		nModfGCTType = ctrlType;// MMOFacade::getSingleton().GCTType;
		//MMOFacade::getSingleton().GCTType = GAME_CONTROL_TYPE(ctrlType);
		
		
		//DefaultBar(TRUE);
		//DefaultFunc(TRUE);
		UMMOUIAssistPanel* assistPanel = Cast<UMMOUIAssistPanel>(MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Assist));
		if (assistPanel)
		{
			assistPanel->ModifyAssistInputData(TRUE, nModfGCTType);
		}
		
	}
}

UBOOL UMMOKeyManager::InputKey(INT ControllerId,FName Key,EInputEvent Event,FLOAT AmountDepressed,UBOOL bGamepad)
{
	//debugf(TEXT("UMMOKeyManager::InputKey:%s %d"), *Key.ToString(), (INT)Event);

	BYTE code = GEngine->GameViewport->Viewport->KeyCode(Key);
	UBOOL in_game = (MMO::GetCurrentGameState() == EGameState_InGame);


	// oscar@snda
	if(g_disableAllInput)
	{
		if (Key == TEXT("Escape"))
			;
		else
			return FALSE;
	}
	// oscar@snda


	// Key settings that take effect regardless of input state
	if (Event == IE_Released)
	{
		switch (code)
		{
		case VK_LMENU:
		case VK_RMENU:
			{
				DoAltFunc( FALSE );

				return TRUE;
			}
			break;
		//case VK_ESCAPE:
		//	//if (!g_disableESC)
		//	//{
		//		return MMOUI::GetUIManager()->OnEsc();
		//	//}
		//	break;
		}
	}
	else if(Event == IE_Pressed)
	{
		switch (code)
		{
		case VK_ESCAPE:
			//if (!g_disableESC)
			//{
			return MMOUI::GetUIManager()->OnEsc();
			//}
			break;

			// ENTER toggles chat display, ESC hiding is done in AS
		case VK_RETURN:
			{
				// Determine whether currently in chat input mode, switch, and set focus
// 				UMMOUIBase* PanelChat = MMOUI::FindUIPanel(TEXT("Chat"));
// 				UMMOUIBase* PanelSkilbar = MMOUI::FindUIPanel(TEXT("Skillbar"));
				UMMOUIChat* Chat = Cast<UMMOUIChat>(MMOUI::FindUIPanel(TEXT("Chat")));

				// Chat can only be toggled in game
				// Login screen: Enter presses the Login button
				if (MMO::GetCurrentGameState() == EGameState_Login)
				{
					UMMOUILoginPanel* LoginPanel = Cast<UMMOUILoginPanel>(MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Login_User));
					if (LoginPanel && LoginPanel->GetVisible() && MMOMsgBoxManager::getSingleton().GetMsgNumByType(EMsgboxType_Modal) == 0)
					{
						UGFxObject* LoginField = LoginPanel->GetObject(TEXT("LoginField_mc"));
						if (LoginField && LoginField->GetBool(TEXT("visible")))
						{
							FASValue NullArg(EC_EventParm);
							NullArg.Type = AS_Null;
							TArray<FASValue> asArgs;
							asArgs.AddItem(NullArg);
							LoginField->Invoke(TEXT("PushEnter"), asArgs);
						}
					}
					return TRUE;
				}
				if(!in_game || !Chat)
					return TRUE;

				if (!MMOFacade::getSingleton().GetMainController()->GetInTextInput())
				{
					TArray<struct FASValue> Args;
					Chat->ShowInputArea(TRUE);
					//FGFxEngine::GetEngine()->SetFocusMovie(PanelChat->GFXPlayer->pMovie);
					//Chat->eventCallShowPrivateName();
					//MMOUI::MoveToTop(Chat);
					//Chat->Invoke(TEXT("SetTextInputFocus"),Args);	// this should be reached from chat->OnFoucsGained(), hack for now
				}
				/*else
				{
					Chat->checkInput();
				}*/

				return TRUE;
			}
			break;

		case VK_LMENU:
		case VK_RMENU:
			{
				DoAltFunc( TRUE );
				return TRUE;
			}
			break;
		}
	}


	/*if (Event == IE_Pressed || Event == IE_Repeat)
	{
		switch (code)
		{
		case VK_TAB:
			{
				if(MMOChatManager::getSingleton().IsChatFocus())
				{
					MMOChatManager::getSingleton().NotifySetChannelByTab();
					return TRUE;
				}
			}
			break;
		}
	
	}*/

	AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
	if (!pc)
		return FALSE;
	// oscar@snda handled by the lower layer
// 	if (pc->GetInTextInput())
// 		return FALSE;

	// Combination keys trigger on press, single keys trigger on release
	if (Event == IE_Pressed)
	{
		if (in_game)
			return DoKeyPress(code, TRUE);
	}
	else if (Event == IE_Released)
	{
		switch (code)
		{
		//case _T('X'):
		//	if (in_game)
		//	{
		//		FViewport* vp = GEngine->GameViewport->Viewport;
		//		if (vp && (vp->KeyState(KEY_LeftAlt) || vp->KeyState(KEY_RightAlt)))
		//		{
		//			//Not needed in the new version
		//			MMOFacade::getSingleton().GetMainController()->eventReturnSelectHotKey();
		//			return TRUE;
		//		}
		//	}
		//	break;
		case _T('U'):
			if (in_game)
			{
				FViewport* vp = GEngine->GameViewport->Viewport;
				if (vp && (vp->KeyState(KEY_LeftControl) || vp->KeyState(KEY_RightControl)))
				{
					//Not needed in the new version
					//MMOUI::GetUIManager()->delegateToggleTipAid();
					return TRUE;
				}
			}
			break;
#if !SHIPPING_PC_GAME
		case VK_F6:
			if (in_game)
			{
				//MMOWeatherManager::getSingleton().StopAllWeather();
				//MMOWeatherManager::getSingleton().StartWeather(swtRain);
			}
			break;
		case VK_F7:
			if (in_game)
			{
				//MMOWeatherManager::getSingleton().StopAllWeather();
				//MMOWeatherManager::getSingleton().StartWeather(swtSnow);
			}
			break;
		case VK_F8:
			if (in_game)
			{
				//MMOWeatherManager::getSingleton().StopAllWeather();
				//MMOWeatherManager::getSingleton().StartWeather(swtThunder);
			}
			break;
		case VK_F9:
			if (in_game)
			{
				//MMOWeatherManager::getSingleton().StopAllWeather();
				//MMOWeatherManager::getSingleton().StartWeather(swtStorm);
			}
			break;
		case VK_F10:
			if (in_game)
			{
				//MMOWeatherManager::getSingleton().StopAllWeather();
			}
			break;
#endif
		}
		if (in_game)
			return DoKeyPress(code, FALSE);
	}
	return FALSE;
}

void UMMOKeyManager::OnOpenUI()
{
	nModfGCTType = MMOFacade::getSingleton().GCTType;
	vmKeyTemp.Empty();
	for (MAP_KEYWORLD::TIterator it(vmKeyBind); it; ++it)
		vmKeyTemp.Set(it.Key(), it.Value());
}

void UMMOKeyManager::OnApplyByUI()
{
	MdfKeyEnd();
	vmKeyBind.Empty();
	for (MAP_KEYWORLD::TIterator it(vmKeyTemp); it; ++it)
		vmKeyBind.Set(it.Key(), it.Value());


	MMOFacade::getSingleton().GCTType = (GAME_CONTROL_TYPE)nModfGCTType;
	MMOClientVariableManager::getSingleton().SetClientVariable(ECVT_ContralType, nModfGCTType);

	UMMOUIManager* UM = MMOUI::GetUIManager();
	UMMOUIMenuBar* mb = Cast<UMMOUIMenuBar>(UM->FindUIPanel(UM->Panel_Skillbar_Menubar));
	if (mb)
		mb->InitMenuSlot();
	UMMOUISkillBar* skb = Cast<UMMOUISkillBar>(UM->FindUIPanel(UM->Panel_Skillbar));
	if (skb)
		skb->InitSkillBarSlotHotKey();
}

void UMMOKeyManager::MdfKeyBegin(EKeyFunction func)
{
	if (nKeyModifying != KF_Unknown)
		MdfKeyEnd(0);
	
	UMMOUIAssistPanel* assistPanel = Cast<UMMOUIAssistPanel>(MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Assist));
	if (assistPanel)
	{
		assistPanel->ShowKeyWarning(FALSE);
	}

	//if(nModfGCTType == GC_DIABLO3)
	//{
	//	if (func == KF_BarSkill1 && func == KF_BarSkill2)
	//	{
	//		return;
	//	}
	//}
	//else if(nModfGCTType == GC_LOL)
	//{
	//	if (func == KF_BarSkill1)
	//	{
	//		return;
	//	}
	//}
	//else if(nModfGCTType == GC_MIR2)
	//{
	//	if (func == KF_BarSkill1  )
	//	{
	//		return;
	//	}
	//}
	//else if(nModfGCTType == GC_MIRFOREVER)
	//{
	//	if (func == KF_BarSkill1 || func == KF_BarSkillEx1 )
	//	{
	//		return;
	//	}
	//}

	//if (func != KF_BarSkill5 && func != KF_BarSkill6)
		nKeyModifying = func;
}

void UMMOKeyManager::MdfKeyEnd(BYTE key)
{
	if (key && nKeyModifying != KF_Unknown)
	{
		//if( DelKeyFunction(vmKeyTemp, nKeyModifying) )
		//{
		//	UMMOUIAssistPanel* assistPanel = Cast<UMMOUIAssistPanel>(MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Assist));
		//	if (assistPanel)
		//	{
		//		assistPanel->ShowKeyWarning(TRUE);
		//	}
		//}

		// Only accept allowed keys as key codes
		if (vmKeyNames.Find(key))
		{
			const UBOOL kctrl = MMO::GetAsyncKeyStateEx(VK_CONTROL) & 0x8000;
			const UBOOL kalt = MMO::GetAsyncKeyStateEx(VK_MENU) & 0x8000;
			const UBOOL kshift = MMO::GetAsyncKeyStateEx(VK_SHIFT) & 0x8000;

			// Only function keys can be used in combinations
			KKeyworkd kk;
			if (nKeyModifying >= KF_Func_Begin)
			{
				if (kctrl)	kk.flag |= KNF_Ctrl;
				if (kalt)	kk.flag |= KNF_Alt;
				if (kshift)	kk.flag |= KNF_Shift;
			}
			else
			{
				//if (nModfGCTType==GC_MIRFOREVER) kk.flag |= KNF_CtrlType_MirForever;
				//if (nModfGCTType==GC_MIR2) kk.flag |= KNF_CtrlType_Mir2;
			}
			kk.code = key;

			DelKeyFunction(vmKeyTemp, nKeyModifying);
			UBOOL bWarn= FALSE;
			if(vmKeyTemp.Find(kk))
			{
				bWarn = TRUE;
			}
			
			vmKeyTemp.Set(kk, nKeyModifying);

			UMMOUIAssistPanel* assistPanel = Cast<UMMOUIAssistPanel>(MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Assist));
			if (assistPanel)
			{
				assistPanel->ModifyAssistInputData(TRUE, nModfGCTType);
				if(bWarn)
				{
					assistPanel->ShowKeyWarning(TRUE);
				}
			}
		}
		else
		{

		}
		nKeyModifyingKeyPressed = TRUE;
	}
	nKeyModifying = KF_Unknown;
}

UBOOL UMMOKeyManager::DelKeyFunction(MAP_KEYWORLD& kmp, EKeyFunction func)
{
	for (MAP_KEYWORLD::TIterator it(kmp); it; ++it)
	{
		if (it.Value() == func)
		{
			kmp.Remove(it.Key());
			return TRUE;
		}
	}
	return FALSE;
}


UBOOL UMMOKeyManager::DoKeyPress(BYTE key, UBOOL down)
{
	// Do not handle hotkeys if there is input focus
	if(g_focusInTextInput)
	{
		return FALSE;
	}
	
	switch (key)
	{
	case VK_ESCAPE:
		if (nKeyModifying != KF_Unknown)
		{
			MdfKeyEnd(VK_CANCEL);
			return TRUE;
		}
		return FALSE;
	// Combination key pressed, pass
	case VK_CONTROL:
	case VK_LCONTROL:
	case VK_RCONTROL:
	case VK_MENU:
	case VK_LMENU:
	case VK_RMENU:
	case VK_SHIFT:
	case VK_LSHIFT:
	case VK_RSHIFT:
	case VK_LBUTTON:
	case VK_RBUTTON:
		return FALSE;
	
	}

	// Any non-combination key must stop
	if (nKeyModifying != KF_Unknown)
	{
		MdfKeyEnd(key);
		return TRUE;
	}

	if (nKeyModifyingKeyPressed)
	{
		nKeyModifyingKeyPressed=FALSE;
		if(!down)
		{
			return TRUE;
		}
	}

	if (MMO::GetCurrentGameState() != EGameState_InGame)
		return FALSE;

	// Execute hotkey function
	const UBOOL kctrl = MMO::GetAsyncKeyStateEx(VK_CONTROL) & 0x8000;
	const UBOOL kalt = MMO::GetAsyncKeyStateEx(VK_MENU) & 0x8000;
	const UBOOL kshift = MMO::GetAsyncKeyStateEx(VK_SHIFT) & 0x8000;
	for (MAP_KEYWORLD::TIterator it(vmKeyBind); it; ++it)
	{
		const KKeyworkd& kk = it.Key();
		if (kk.code != key)
			continue;
		// Single key, triggers on release
		if(MMOFacade::getSingleton().GCTType == GC_MIRFOREVER  )
		{
			if (kk.flag == KNF_None || kk.flag == KNF_CtrlType_MirForever )
			{
				if (!kctrl && !kalt && !kshift)
					return DoFunction(it.Value(), down, FALSE);
				continue;
			}

		}
		else if(MMOFacade::getSingleton().GCTType == GC_MIR2  )
		{
			if (kk.flag == KNF_None || kk.flag == KNF_CtrlType_Mir2 )
			{
				if (it.Value()>=KF_BarSkill1 && it.Value()<=KF_BarSkill12)
				{
					if (!kctrl)
					{
						return DoFunction(it.Value(), down, FALSE);
					}
				}
				if (!kctrl && !kalt && !kshift)
					return DoFunction(it.Value(), down, FALSE);
				continue;
			}

		}

		//if (kk.flag == KNF_None )
		//{
		//	if (!kctrl && !kalt && !kshift)
		//		return DoFunction(it.Value(), down, FALSE);
		//	continue;
		//}

		int apk_c = 0;
		if ((kk.flag & KNF_Ctrl) && kctrl)
			apk_c++;
		if ((kk.flag & KNF_Alt) && kalt)
			apk_c++;
		if ((kk.flag & KNF_Shift) && kshift)
			apk_c++;
		// Combination key, triggers on press
		if (apk_c)
			return DoFunction(it.Value(), down, TRUE);
	}

	// 1.05 wing panel (UnrealWing): fixed key H while no function is bound to it (not part of the configurable key table)
	if (down && key == 'H' && !kctrl && !kalt && !kshift)
	{
		MMOUI::GetUIManager()->ToggleShowPanelEx(FName(TEXT("UnrealWing")), "");
		return TRUE;
	}
	return FALSE;
}



UBOOL UMMOKeyManager::CanDoFunction(EKeyFunction k)
{
	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (!uiManager)
	{
		return FALSE;
	}

	UMMOUIMenuBar* menuBar = Cast<UMMOUIMenuBar>(uiManager->FindUIPanel(uiManager->Panel_Skillbar_Menubar));
	if (!menuBar)
	{
		return FALSE;
	}

	switch (k)
	{
	case KF_OpenRank:// leaderboard
		{
			return menuBar->IsMenuActivated(EMenuType_Child_Ranking);
		} 
		break;
	case KF_OpenAchieve:// achievements
		{
			return menuBar->IsMenuActivated(EMenuType_Child_Achievement);
		} 
		break;
	case KF_OpenReputation:// reputation
		{
			return menuBar->IsMenuActivated(EMenuType_Child_Repute);
		} 
		break;
	case KF_OpenActivity:// schedule (activities)
		{
			return menuBar->IsMenuActivated(EMenuType_Child_Schedule);
		}
		break;
	case KF_Mail:// mail
		{
			return menuBar->IsMenuActivated(EMenuType_Child_Mail);
		} 
		break;
	case KF_Mall:// mall
		{
			return menuBar->IsMenuActivated(EMenuType_Child_Mall);
		}
		break;
	case KF_Handbook:// handbook
		{
			return menuBar->IsMenuActivated(EMenuType_Child_Handbook);
		} 
		break;
	case KF_GoldStore:// gold exchange
		{
			return menuBar->IsMenuActivated(EMenuType_Child_GoldStore);
		}
		break;
	case KF_AUCTION://Supreme Auction House
	{
		return menuBar->IsMenuActivated(EMenuType_Child_Auction);
	}
		break;
	case KF_OpenRole:// character
		{
			return menuBar->IsMenuActivated(EMenuType_Child_Charactor);
		} 
		break;
	case KF_OpenBag:// bag
		{
			return menuBar->IsMenuActivated(EMenuType_Child_Bag);
		} 
		break;
	case KF_OpenHeaven:// Dragon Guard mental art
		{
			return menuBar->IsMenuActivated(EMenuType_Child_Heaven);
		}
		break;
	case KF_OpenSkillUp:// ability
		{
			return menuBar->IsMenuActivated(EMenuType_Child_Skill);
		}
		break;
	case KF_OpenTask:// normal quest
		{
			return menuBar->IsMenuActivated(EMenuType_Child_NormalQuest);
		}
		break;
	case KF_OpenStoryQuest:// story quest
		{
			return menuBar->IsMenuActivated(EMenuType_Child_StoryQuest);
		}
		break;
	case KF_OpenTeam:// team
		{
			return menuBar->IsMenuActivated(EMenuType_Child_Team);
		}
		break;
	case KF_OpenFriend:// friend
		{
			return menuBar->IsMenuActivated(EMenuType_Child_Friend);
		}
		break;
	case KF_OpenMaster:// master/apprentice
		{
			return menuBar->IsMenuActivated(EMenuType_Child_MasterStudent);
		}
		break;
	case KF_OpenGuild:// guild
		{
			return menuBar->IsMenuActivated(EMenuType_Child_Guild);
		}
		break;
	case KF_OpenRide:// mount
		{
			return menuBar->IsMenuActivated(EMenuType_Child_Riding);
		} 
		break;
	}
	return TRUE;
}



UBOOL UMMOKeyManager::DoFunction(EKeyFunction k, UBOOL down, UBOOL combo)
{
	if (MMO::GetCurrentGameState() != EGameState_InGame)
		return FALSE;

	if (CanDoFunction(k) == FALSE)
		return FALSE;

	UMMOUIManager* UM = MMOUI::GetUIManager();
	UMMOUISkillBar* skb = Cast<UMMOUISkillBar>(UM->FindUIPanel(UM->Panel_Skillbar));

	UUIKeyMapManager* kmm = MMOFacade::getSingleton().GetMainController()->KeyMapManager;

	// Ability keys are a bit special, some need charging.
	switch (k)
	{
	case KF_BarSkill1:
	case KF_BarSkill2:
	case KF_BarSkill3:
	case KF_BarSkill4:
	case KF_BarSkill5:
	case KF_BarSkill6:
	case KF_BarSkill7:
	case KF_BarSkill8:
	case KF_BarSkill9:
	case KF_BarSkill10:
	case KF_BarSkill11:
	case KF_BarSkill12:
	{
		if (skb){
			skb->DoSlotAction(k - KF_BarSkill1, !down);
		}
		return TRUE;
	}
		break;
		//case KF_BarSkill1:
		//case KF_BarSkillEx1:
		//	//if (down) kmm->OnSlotKey(ECK_Slot1, FALSE); else kmm->OnSlotKey(ECK_Slot1, TRUE);
		//	if (skb) skb->UseSkill(0, down);	return TRUE;
		//	return TRUE;
		//case KF_BarSkill2:
		//case KF_BarSkillEx2:
		//	//if (down) kmm->OnSlotKey(ECK_Slot2, FALSE); else kmm->OnSlotKey(ECK_Slot2, TRUE);
		//	if (skb) skb->UseSkill(1, down);	return TRUE;
		//	return TRUE;
		//case KF_BarSkill3:
		//case KF_BarSkillEx3:
		//	//if (down) kmm->OnSlotKey(ECK_Slot3, FALSE); else kmm->OnSlotKey(ECK_Slot3, TRUE);
		//	if (skb) skb->UseSkill(2, down);	return TRUE;
		//	return TRUE;
		//case KF_BarSkill4:
		//case KF_BarSkillEx4:
		//	//if (down) kmm->OnSlotKey(ECK_Slot4, FALSE); else kmm->OnSlotKey(ECK_Slot4, TRUE);
		//	if (skb) skb->UseSkill(3, down);	return TRUE;
		//	return TRUE;
		//case KF_BarSkill5:
		//case KF_BarSkillEx5:
		//	//if (down) kmm->OnSlotKey(ECK_Slot5, FALSE); else kmm->OnSlotKey(ECK_Slot5, TRUE);
		//	if (skb) skb->UseSkill(4, down);	return TRUE;
		//	return TRUE;
		//case KF_BarSkill6:
		//case KF_BarSkillEx6:
		//	//if (down) kmm->OnSlotKey(ECK_Slot6, FALSE); else kmm->OnSlotKey(ECK_Slot6, TRUE);
		//	if (skb) skb->UseSkill(5, down);	return TRUE;
		//	return TRUE;
	}

	//BOOL can_use = FALSE;
	//// For combination keys, trigger on press
	//if (combo && down)
	//	can_use = TRUE;
	//// For single keys, trigger on release
	//if (!combo && !down)
	//	can_use = TRUE;
	//if (!can_use)
	//	return FALSE;

	if (!down)
		return FALSE;
	switch (k)
	{
		//case KF_BarItem1:case KF_BarItemEx1:	if (skb) skb->UseBarItem(0);	return TRUE;
		//case KF_BarItem2:case KF_BarItemEx2:	if (skb) skb->UseBarItem(1);	return TRUE;
		//case KF_BarItem3:case KF_BarItemEx3:	if (skb) skb->UseBarItem(2);	return TRUE;
		//case KF_BarItem4:case KF_BarItemEx4:	if (skb) skb->UseBarItem(3);	return TRUE;
		//case KF_BarItem5:case KF_BarItemEx5:	if (skb) skb->UseBarItem(4);	return TRUE;
		//case KF_BarItem6:case KF_BarItemEx6:	if (skb) skb->UseBarItem(5);	return TRUE;

	case KF_OpenRole:			UM->ToggleShowPanelEx(UM->Panel_Character, _T("ClickTab0")); return TRUE;
	case KF_OpenBag:			UM->ToggleShowPanelEx(UM->Panel_Bag, ""); return TRUE;
	case KF_OpenHeaven:			UM->ToggleShowPanelEx(UM->Panel_SkillTree, _T("ClickPanelTab0")); return TRUE;
	case KF_OpenTask:			UM->ToggleShowPanelEx(UM->Panel_Quest, ""); return TRUE;
		//case KF_OpenPlayerInfoView:	UM->ShowTargetPlayerInfo(); return TRUE;
		//case KF_OpenIntensify:		UM->ToggleShowPanelEx(UM->Panel_Character, _T("ClickTab0") ); return TRUE;
		//case KF_OpenMount:			UM->ToggleShowPanelEx(UM->Panel_SoulEmbed, ""); return TRUE;
		//case KF_OpenBaptize:		return TRUE;
	case KF_OpenGuild:			UM->ToggleShowPanelEx(UM->Panel_Guild_Main, ""); return TRUE;
	case KF_OpenFriend:
	{
		UM->ToggleShowPanelEx(UM->Panel_Friend, "");
		UMMOUIContactWidget* fPanel = Cast<UMMOUIContactWidget>(UM->FindUIPanel(UM->Panel_Friend));
		UMMOUIRClickMenu* rPanel = Cast<UMMOUIRClickMenu>(UM->FindUIPanel(UM->Panel_RClickMenu));
		if (fPanel && fPanel->GetVisible() == FALSE && rPanel)
		{
			UM->delegateHideRClickMenu();
		}
		return TRUE;
	}

	case KF_OpenMaster:			UM->ToggleShowPanelEx(UM->Panel_MasterStudent, ""); return TRUE;
	case KF_OpenTeam:			UM->ToggleShowPanelEx(UM->Panel_Team, ""); return TRUE;
	case KF_OpenRide:			UM->ToggleShowPanelEx(UM->Panel_Mount, ""); return TRUE;
		//	case KF_OpenHero:			UM->ToggleShowPanelEx(UM->Panel_Hero, ""); return TRUE;
	case KF_OpenAchieve:		UM->ToggleShowPanelEx(UM->Panel_Achievement, ""); return TRUE;
	case KF_OpenOption:
	{
		//debugf(TEXT("KF_OpenOption down:%d Key:%d"), down, (INT)k);

		UM->ToggleShowPanelEx(UM->Panel_OptionConfig, "");
		return TRUE;
	}
	case KF_QuickPick:			MMODropManager::getSingleton().PickupAll(); return TRUE;
	case KF_OpenMinimap:		UM->ToggleShowPanelEx(UM->Panel_WorldMap, ""); return TRUE;
	case KF_ToggleMinimapType:	UM->ToggleShowPanelEx(UM->Panel_WorldMap, ""); return TRUE;
		//case KF_ToggleMinimapType:
		//	{
		//		//Check whether the current focus is on the chat UI
		//		if(MMOChatManager::getSingleton().IsChatFocus()  == FALSE)
		//		{
		//			UMMOUIMinimap *minimapPanel = (UMMOUIMinimap*)UM->FindUIPanel( UM->Panel_Minimap );
		//			if( minimapPanel != NULL ){
		//				minimapPanel->ToggleMinimapShowType();
		//			}
		//			
		//		}
		//		
		//	}
		//	return TRUE;
		//case KF_OpenSkill:			UM->ToggleShowPanelEx(UM->Panel_SkillSetting, ""); return TRUE;	
		//case KF_OpenArena:			UM->ToggleShowPanelEx(UM->Panel_BattleTeam, ""); return TRUE;//commented out in this version
	case KF_OpenActivity:		UM->ToggleShowPanelEx(UM->Panel_Activity, ""); return TRUE;
	case KF_OpenRank:			UM->ToggleShowPanelEx(UM->Panel_RankList, ""); return TRUE;
		//case KF_OpenSkillUp:		UM->ToggleShowPanelEx(UM->Panel_SkillTree, _T("ClickPanelTab1")); return TRUE;
	case KF_OpenSkillUp:		UM->ToggleShowPanelEx(UM->Panel_SkillSetting, ""); return TRUE;
	case KF_OpenStoryQuest:		UM->ToggleShowPanelEx(UM->Panel_QuestStory, ""); return TRUE;
	case KF_OpenReputation:		UM->ToggleShowPanelEx(UM->Panel_JourneyView, ""); return TRUE;
	case KF_Mail:				UM->ToggleShowPanelEx(UM->Panel_Mail, ""); return TRUE;
	case KF_GoldStore:			UM->ToggleShowPanelEx(UM->Panel_GoldStore, ""); return TRUE;
	case KF_Handbook:			UM->ToggleShowPanelEx(UM->Panel_Handbook, ""); return TRUE;
	case KF_Assist:				UM->ToggleShowPanelEx(UM->Panel_Assist, ""); return TRUE;

	case KF_ChangePkMode:
	{
		UMMOUIMyInfo* pMyInfo = Cast<UMMOUIMyInfo>(UM->FindUIPanel(UM->Panel_HudInfo_MyInfo));
		if (pMyInfo)
		{
			pMyInfo->ASSetPkMode((MMOFacade::getSingleton().GetMyPkMode() + 1) % (E_rpm_Total - 1));
		}

		//UMMOGFXPlayer_HudInfo *pHudInfo = Cast<UMMOGFXPlayer_HudInfo>(UM->FindMoviePlayer(UM->MoviePlayer_HudInfo));
		//if(pHudInfo)
		//{
		//	pHudInfo->ASSetPkMode((MMOFacade::getSingleton().GetMyPkMode()+1)%E_rpm_Total);
		//}
		return TRUE;
	}
		//CMessageHandler::GetInstance().Send_PkMode((MMOFacade::getSingleton().GetMyPkMode()+1)%E_rpm_Total);return TRUE;
	case KF_Mall:				UM->ToggleShowPanelEx(UM->Panel_Mall, ""); return TRUE;


	case KF_ChangeRole:
	{
		if (MMOMsgBoxManager::getSingleton().IsModalMsgBox())
		{
			return TRUE;
		}
		else{
			MMOFacade::getSingleton().GetMainController()->eventReturnSelectHotKey();	return TRUE;//change character
		}
	}
	case KF_EnterGroup:
	{
		TeamManager::getSingleton().NotifyQuickTeam(TRUE);
		return TRUE;//quick team up
	}
	case KF_LeaveGroup:
	{
		TeamManager::getSingleton().NotifyQuickTeam(FALSE);
		return TRUE;//quick leave team
	}
	case KF_AddAttention:
	{
		AMMOActor* pDesireActor = FSMIO::getSingleton().Target.GetTargetMMOActor();// .GetDesireMMOActor();
		if (pDesireActor){
			FriendManager::getSingleton().RequestAddAttention(pDesireActor->ServerID, TEXT(""));
		}
		return TRUE;//add to follow list
	}
	case KF_AddEnemy:
	{
		AMMOActor* pDesireActor = FSMIO::getSingleton().Target.GetTargetMMOActor();//GetDesireMMOActor();
		if (pDesireActor){

			AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
			if( pl && pl->ServerID != pDesireActor->ServerID)
				CMessageHandler::GetInstance().SendAddEnemy(pDesireActor->ServerID);
			
		}
		return TRUE;//add enemy
	}
	case KF_AddBlackList:
	{
		AMMOActor* pDesireActor = FSMIO::getSingleton().Target.GetTargetMMOActor();//GetDesireMMOActor();
		if (pDesireActor){
			CMessageHandler::GetInstance().SendMoveToBlackList(pDesireActor->ServerID, TEXT(""));
		}
		return TRUE;//add to blacklist
	}
	case KF_ShowHidePlayer:
	{
		AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
		if (ctrl)
		{
			ctrl->SetShowTheOtherPlayersByHotKey(!(ctrl->bShowTheOtherPlayersByHotKey));
			if (UM->HintHideNearPlayer)
			{
				UM->HintHideNearPlayer = FALSE;
				UM->SaveConfigToServer();
			}
		}
		return TRUE;
	}
	case KF_ChangePetAI:
	{
		CMessageHandler::GetInstance().Send_Change_Pet_State(E_ps_Invalid);
		return TRUE;
	}
	case KF_KeepRunning:
	{
		FSMIO::getSingleton().Target.ToggleKeepRunning();
		return TRUE;
	}
	case KF_AUCTION:
	{
		UM->ToggleShowPanelEx(UM->Panel_AuctionView, ""); return TRUE;
		return TRUE;
	}

	}
	return FALSE;
}

UBOOL UMMOKeyManager::GetFuncInfoByPanel(FString& panel, FString& tab_name, FString& key, FString& name)
{
	if(panel == MMOUI::GetUIManager()->Panel_OptionMenu.ToString())
	{
		key = TEXT("ESC");
		FuncToString(KF_OpenOption, name);
		return TRUE; 
	}

	EKeyFunction kf = KF_Unknown;
	UMMOUIManager* UM = MMOUI::GetUIManager();
	if (panel == UM->Panel_Character.ToString() && tab_name == _T("ClickTab0") )
		kf = KF_OpenRole;
	else if (panel == UM->Panel_Bag.ToString())
		kf = KF_OpenBag;
	else if (panel == UM->Panel_SkillTree.ToString() && tab_name == _T("ClickPanelTab0"))
		kf = KF_OpenHeaven;
	else if (panel == UM->Panel_Quest.ToString())
		kf = KF_OpenTask;
	//else if (panel == UM->Panel_PlayerInfo.ToString())
	//	kf = KF_OpenPlayerInfoView;
	//else if (panel == UM->Panel_EquipSlotEnhance.ToString())
	//	kf = KF_OpenIntensify;
	//else if (panel == UM->Panel_SoulEmbed.ToString())
	//	kf = KF_OpenMount;
	//else if (panel == UM->)
	//	kf = KF_OpenBaptize;
	else if (panel == UM->Panel_Guild_Main.ToString())
		kf = KF_OpenGuild;
	else if (panel == UM->Panel_Friend.ToString())
		kf = KF_OpenFriend;
	else if (panel == UM->Panel_MasterStudent.ToString())
		kf = KF_OpenMaster;
	else if (panel == UM->Panel_Team.ToString())
		kf = KF_OpenTeam;
	else if (panel == UM->Panel_Mount.ToString())
		kf = KF_OpenRide;
	else if (panel == UM->Panel_Achievement.ToString())
		kf = KF_OpenAchieve;
	else if (panel == UM->Panel_OptionConfig.ToString())
		kf = KF_OpenOption;
	else if (panel == UM->Panel_WorldMap.ToString())
		kf = KF_OpenMinimap;
	else if (panel == UM->Panel_WorldMap.ToString())
		kf = KF_ToggleMinimapType;
	////else if (panel == UM->Panel_PathMap.ToString())
	////	kf = KF_ToggleMinimapType;
	//else if (panel == UM->Panel_SkillSetting.ToString())
	//	kf = KF_OpenSkill;
	//else if (panel == UM->Panel_BattleTeam.ToString())
	//	kf = KF_OpenArena;
	else if (panel == UM->Panel_Activity.ToString())
		kf = KF_OpenActivity;
	else if (panel == UM->Panel_RankList.ToString())
		kf = KF_OpenRank;
	//else if (panel == UM->Panel_SkillTree.ToString() && tab_name == _T("ClickPanelTab1"))
	else if (panel == UM->Panel_SkillSetting.ToString())
		kf = KF_OpenSkillUp;
	else if (panel == UM->Panel_QuestStory.ToString())
		kf = KF_OpenStoryQuest;
	else if ( panel == UM->Panel_JourneyView.ToString())
		kf = KF_OpenReputation;
	else if (panel == UM->Panel_Mall.ToString())
		kf = KF_Mall;
	else if (panel == UM->Panel_Mail.ToString())
		kf = KF_Mail;
	else if (panel == UM->Panel_GoldStore.ToString())
		kf = KF_GoldStore;
	else if (panel == UM->Panel_Handbook.ToString())
		kf = KF_Handbook;
	else if (panel == UM->Panel_Assist.ToString())
		kf = KF_Assist;
	else if (panel == UM->Panel_AuctionView.ToString())
		kf = KF_AUCTION;

	if (kf == KF_Unknown)
	{
		key = TEXT("");
		return FALSE;
	}
	KKeyworkd* pkey = FindKeyworld(kf, FALSE);
	if (!pkey)
		return FALSE;
	KeyToString(pkey, key);
	FuncToString(kf, name);
	return TRUE;
}

void UMMOKeyManager::SaveKeyword()
{
	CONST INT buf_size = (ECVT_KeyManagerEnd-ECVT_KeyManagerBegin+1)<<2;
	BYTE buf[buf_size] = {0};
	check(buf_size >= vmKeyBind.Num() * sizeof(KKeyworkd));
	if (ExtractKey(buf, buf_size))
	{
		CMessageHandler::GetInstance().Send_UpdateClientVarRange(ECVT_KeyManagerBegin, ECVT_KeyManagerEnd, buf, sizeof(buf));
	}
}

BYTE  UMMOKeyManager::GetClientVariablesVersion()
{
	return nCurVersion;
}

UBOOL UMMOKeyManager::ExtractKey(BYTE* buf, UINT size)
{
	if (!buf || size <= 0)
		return FALSE;

	buf[0] = nCurVersion = KNF_Version;	// marks data validity or version number

	size_t kpos = 1;
	for (MAP_KEYWORLD::TIterator it(vmKeyBind); it; ++it)
	{
		if (kpos + 3 > size)
			return TRUE;
		buf[kpos++] = it.Key().flag | KNF_Using;
		buf[kpos++] = it.Key().code;
		buf[kpos++] = (BYTE)(it.Value() & 0xff);
	}
	return TRUE;
}

UBOOL UMMOKeyManager::RestoreKey(BYTE* buf, UINT size)
{
	if (!buf || size <= 0)
		return FALSE;

	nCurVersion=0;
	if (buf[0] != KNF_Version)
		return FALSE;
	nCurVersion=KNF_Version;

	KKeyworkd key;
	vmKeyBind.Empty();
	INT nKeyNum = 0;

	for (size_t kpos = 1; (kpos + 3) <= size; kpos += 3)
	{
		key.flag = buf[kpos+0];
		key.code = buf[kpos+1];
		if (key.flag & KNF_Using)
		{
			EKeyFunction kf = (EKeyFunction)buf[kpos + 2];
			if (kf < KF_Func_End)
			{
				nKeyNum++;
				key.flag &= ~KNF_Using;
				vmKeyBind.Set(key, kf);
			}
		}
	}

	if (nKeyNum != KF_Func_End)
	{
		DefaultFunc(TRUE);
		for (MAP_KEYWORLD::TIterator it(vmKeyTemp); it; ++it)
		{
			UBOOL bFind = FALSE;
			for (MAP_KEYWORLD::TIterator it2(vmKeyBind); it2; ++it2)
			{
				if (it.Value() == it2.Value())
				{
					bFind = TRUE;
					break;
				}
			}
			if (bFind == FALSE)
			{
				if (vmKeyBind.Find(it.Key()))
				{

				}
				else
				{
					vmKeyBind.Set(it.Key(), it.Value());
				}
			}
		}
		vmKeyTemp.Empty();
		SaveKeyword();
	}

	UMMOUIManager* UM = MMOUI::GetUIManager();
	UMMOUIMenuBar* mb = Cast<UMMOUIMenuBar>(MMOUI::FindUIPanelEx(UM->Panel_Skillbar_Menubar));
	if (mb){
		mb->InitMenuSlot();
	}
	else{
		warnf( NAME_Error, TEXT("UMMOKeyManager::RestoreKey MenuBar panel not loaded") );
	}
	UMMOUISkillBar* skb = Cast<UMMOUISkillBar>(MMOUI::FindUIPanelEx(UM->Panel_Skillbar));
	if (skb){
		skb->InitSkillBarSlotHotKey() ;
	}
	else{
		warnf( NAME_Error, TEXT("UMMOKeyManager::RestoreKey SkillBar panel not loaded") );
	}

	return TRUE;
}

UMMOKeyManager::KKeyworkd* UMMOKeyManager::FindKeyworld(EKeyFunction func, UBOOL mdf_config)
{
	KKeyworkd* pBuf = NULL;
	if (mdf_config){
		pBuf = (UMMOKeyManager::KKeyworkd*)vmKeyTemp.FindKey(func);
		return pBuf;
	}
	pBuf = (UMMOKeyManager::KKeyworkd*)vmKeyBind.FindKey(func);
	return pBuf;
}

FString& UMMOKeyManager::KeyToString(KKeyworkd* key, FString& out, BYTE t)
{
	out = _T("");
	if (key)
	{
		const FString* pstr = vmKeyNames.Find(key->code);
		if (!pstr)
			return out;
		if (key->flag & KNF_Ctrl)
			out += _T("Ctrl");
		if (key->flag & KNF_Alt)
		{
			if (!out.IsEmpty())
				out += _T("+");
			out += _T("Alt");
		}
		if (key->flag & KNF_Shift)
		{
			if (!out.IsEmpty())
				out += _T("+");
			out += _T("Shift");
		}
		switch (t)
		{
		case 0:	// all combination keys ctrl+A
			if (!out.IsEmpty())
				out += _T("+");
			out += *pstr;
			break;
		case 1:	// normal keys only, ABCD...
			out = *pstr;
			break;
		case 2:	// combination keys only ctr\alt\shift
			break;
		default:
			out = _T("");
		}

	}
	return out;
}

FString& UMMOKeyManager::FuncToString(EKeyFunction func, FString& out)
{
	FString* ps = (FString*)vmFuncNames.Find(func);
	//check(ps);
	if (ps)
		out = *ps;
	return out;
}

void UMMOKeyManager::DefaultBarAllType(UBOOL mdf_config)
{
	INT nBuffModfGCTType = nModfGCTType;
	nModfGCTType = GC_MIRFOREVER;
	DefaultBar(mdf_config);
	nModfGCTType = GC_MIR2;
	DefaultBar(mdf_config);
	nModfGCTType = nBuffModfGCTType;
	DefaultFunc(mdf_config);
}

void UMMOKeyManager::DefaultBar(UBOOL mdf_config)
{
	MAP_KEYWORLD* pvmp = &vmKeyBind;
	if (mdf_config)
		pvmp = &vmKeyTemp;
	MAP_KEYWORLD& vmp = *pvmp;

	DelKeyFunction(vmp, KF_BarSkill1);
	DelKeyFunction(vmp, KF_BarSkill2);
	DelKeyFunction(vmp, KF_BarSkill3);
	DelKeyFunction(vmp, KF_BarSkill4);
	DelKeyFunction(vmp, KF_BarSkill5);
	DelKeyFunction(vmp, KF_BarSkill6);

	DelKeyFunction(vmp, KF_BarSkill7);
	DelKeyFunction(vmp, KF_BarSkill8);
	DelKeyFunction(vmp, KF_BarSkill9);
	DelKeyFunction(vmp, KF_BarSkill10);
	DelKeyFunction(vmp, KF_BarSkill11);
	DelKeyFunction(vmp, KF_BarSkill12);

	vmp.Set(KKeyworkd(KNF_None, VK_F1),		KF_BarSkill1);
	vmp.Set(KKeyworkd(KNF_None, VK_F2),		KF_BarSkill2);
	vmp.Set(KKeyworkd(KNF_None, VK_F3),		KF_BarSkill3);
	vmp.Set(KKeyworkd(KNF_None, VK_F4),		KF_BarSkill4);
	vmp.Set(KKeyworkd(KNF_None, VK_F5),		KF_BarSkill5);
	vmp.Set(KKeyworkd(KNF_None, VK_F6),		KF_BarSkill6);

	vmp.Set(KKeyworkd(KNF_None, _T('1')),		KF_BarSkill7);
	vmp.Set(KKeyworkd(KNF_None, _T('2')),		KF_BarSkill8);
	vmp.Set(KKeyworkd(KNF_None, _T('3')),		KF_BarSkill9);
	vmp.Set(KKeyworkd(KNF_None, _T('4')),		KF_BarSkill10);
	vmp.Set(KKeyworkd(KNF_None, _T('5')),		KF_BarSkill11);
	vmp.Set(KKeyworkd(KNF_None, _T('6')),		KF_BarSkill12);

	//if(nModfGCTType == GC_MIRFOREVER){
	//	DelKeyFunction(vmp, KF_BarSkillEx1);
	//	DelKeyFunction(vmp, KF_BarSkillEx2);
	//	DelKeyFunction(vmp, KF_BarSkillEx3);
	//	DelKeyFunction(vmp, KF_BarSkillEx4);
	//	DelKeyFunction(vmp, KF_BarSkillEx5);
	//	DelKeyFunction(vmp, KF_BarSkillEx6);

	//	DelKeyFunction(vmp, KF_BarItemEx1);
	//	DelKeyFunction(vmp, KF_BarItemEx2);
	//	DelKeyFunction(vmp, KF_BarItemEx3);
	//	DelKeyFunction(vmp, KF_BarItemEx4);
	//	DelKeyFunction(vmp, KF_BarItemEx5);
	//	DelKeyFunction(vmp, KF_BarItemEx6);	
	//}
	//else{
	//	DelKeyFunction(vmp, KF_BarSkill1);
	//	DelKeyFunction(vmp, KF_BarSkill2);
	//	DelKeyFunction(vmp, KF_BarSkill3);
	//	DelKeyFunction(vmp, KF_BarSkill4);
	//	DelKeyFunction(vmp, KF_BarSkill5);
	//	DelKeyFunction(vmp, KF_BarSkill6);

	//	DelKeyFunction(vmp, KF_BarItem1);
	//	DelKeyFunction(vmp, KF_BarItem2);
	//	DelKeyFunction(vmp, KF_BarItem3);
	//	DelKeyFunction(vmp, KF_BarItem4);
	//	DelKeyFunction(vmp, KF_BarItem5);
	//	DelKeyFunction(vmp, KF_BarItem6);
	//}

	//if(nModfGCTType == GC_DIABLO3){
	//	vmp.Set(KKeyworkd(KNF_None, _T('1')),		KF_BarSkill3);
	//	vmp.Set(KKeyworkd(KNF_None, _T('2')),		KF_BarSkill4);
	//	vmp.Set(KKeyworkd(KNF_None, _T('3')),		KF_BarSkill5);
	//	vmp.Set(KKeyworkd(KNF_None, _T('4')),		KF_BarSkill6);
	//	vmp.Set(KKeyworkd(KNF_None, VK_LBUTTON),	KF_BarSkill1);
	//	vmp.Set(KKeyworkd(KNF_None, VK_RBUTTON),	KF_BarSkill2);

	//	vmp.Set(KKeyworkd(KNF_None, VK_F1),		KF_BarItem1);
	//	vmp.Set(KKeyworkd(KNF_None, VK_F2),		KF_BarItem2);
	//	vmp.Set(KKeyworkd(KNF_None, VK_F3),		KF_BarItem3);
	//	vmp.Set(KKeyworkd(KNF_None, VK_F4),		KF_BarItem4);
	//	vmp.Set(KKeyworkd(KNF_None, VK_F5),		KF_BarItem5);
	//	vmp.Set(KKeyworkd(KNF_None, VK_F6),		KF_BarItem6);
	//}
	//else if(nModfGCTType == GC_LOL){
	//	vmp.Set(KKeyworkd(KNF_None, _T('W')),		KF_BarSkill3);
	//	vmp.Set(KKeyworkd(KNF_None, _T('E')),		KF_BarSkill4);
	//	vmp.Set(KKeyworkd(KNF_None, _T('R')),		KF_BarSkill5);
	//	vmp.Set(KKeyworkd(KNF_None, _T('D')),		KF_BarSkill6);
	//	vmp.Set(KKeyworkd(KNF_None, VK_RBUTTON),	KF_BarSkill1);
	//	vmp.Set(KKeyworkd(KNF_None, _T('Q')),		KF_BarSkill2);

	//	vmp.Set(KKeyworkd(KNF_None, _T('1')),		KF_BarItem1);
	//	vmp.Set(KKeyworkd(KNF_None, _T('2')),		KF_BarItem2);
	//	vmp.Set(KKeyworkd(KNF_None, _T('3')),		KF_BarItem3);
	//	vmp.Set(KKeyworkd(KNF_None, _T('4')),		KF_BarItem4);
	//	vmp.Set(KKeyworkd(KNF_None, _T('5')),		KF_BarItem5);
	//	vmp.Set(KKeyworkd(KNF_None, _T('6')),		KF_BarItem6);
	//}
	//else if(nModfGCTType == GC_MIR2){
	//	vmp.Set(KKeyworkd(KNF_CtrlType_Mir2, VK_F2),		KF_BarSkill3);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_Mir2, VK_F3),		KF_BarSkill4);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_Mir2, VK_F4),		KF_BarSkill5);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_Mir2, VK_F5),		KF_BarSkill6);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_Mir2, VK_LBUTTON),	KF_BarSkill1);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_Mir2, VK_F1),		KF_BarSkill2);

	//	vmp.Set(KKeyworkd(KNF_CtrlType_Mir2, _T('1')),		KF_BarItem1);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_Mir2, _T('2')),		KF_BarItem2);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_Mir2, _T('3')),		KF_BarItem3);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_Mir2, _T('4')),		KF_BarItem4);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_Mir2, _T('5')),		KF_BarItem5);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_Mir2, _T('6')),		KF_BarItem6);
	//}
	//else if(nModfGCTType == GC_MIRFOREVER){
	//	vmp.Set(KKeyworkd(KNF_CtrlType_MirForever, _T('2')),		KF_BarSkillEx3);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_MirForever, _T('3')),		KF_BarSkillEx4);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_MirForever, _T('4')),		KF_BarSkillEx5);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_MirForever, _T('5')),		KF_BarSkillEx6);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_MirForever, VK_RBUTTON),	KF_BarSkillEx1);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_MirForever, _T('1')),		KF_BarSkillEx2);

	//	vmp.Set(KKeyworkd(KNF_CtrlType_MirForever, VK_F1),		KF_BarItemEx1);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_MirForever, VK_F2),		KF_BarItemEx2);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_MirForever, VK_F3),		KF_BarItemEx3);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_MirForever, VK_F4),		KF_BarItemEx4);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_MirForever, VK_F5),		KF_BarItemEx5);
	//	vmp.Set(KKeyworkd(KNF_CtrlType_MirForever, VK_F6),		KF_BarItemEx6);

	//}

	UMMOUIManager* UM = MMOUI::GetUIManager();
	UMMOUISkillBar* skb = Cast<UMMOUISkillBar>(UM->FindUIPanel(UM->Panel_Skillbar));

	check(vmKeyBind.Num() <= KF_Max);
}

void UMMOKeyManager::DefaultFunc(UBOOL mdf_config)
{
	MAP_KEYWORLD* pvmp = &vmKeyBind;
	if (mdf_config)
		pvmp = &vmKeyTemp;
	MAP_KEYWORLD& vmp = *pvmp;

	DelKeyFunction(vmp, KF_OpenRole		);
	DelKeyFunction(vmp, KF_OpenBag		);
	DelKeyFunction(vmp, KF_OpenHeaven	);
	DelKeyFunction(vmp, KF_OpenTask		);
	//DelKeyFunction(vmp, KF_OpenPlayerInfoView	);
//	DelKeyFunction(vmp, KF_OpenIntensify);
//	DelKeyFunction(vmp, KF_OpenMount	);
//	DelKeyFunction(vmp, KF_OpenBaptize	);
	DelKeyFunction(vmp, KF_OpenGuild	);
	DelKeyFunction(vmp, KF_OpenFriend	);
	DelKeyFunction(vmp, KF_OpenMaster	);
	DelKeyFunction(vmp, KF_OpenTeam		);
	DelKeyFunction(vmp, KF_OpenRide		);
//	DelKeyFunction(vmp, KF_OpenHero		);
	DelKeyFunction(vmp, KF_OpenAchieve	);
	DelKeyFunction(vmp, KF_OpenOption	);
	DelKeyFunction(vmp, KF_QuickPick	);
	DelKeyFunction(vmp, KF_OpenMinimap	);
	DelKeyFunction(vmp, KF_ToggleMinimapType  );
	//DelKeyFunction(vmp, KF_OpenSkill	);
	//DelKeyFunction(vmp, KF_OpenArena	);
	DelKeyFunction(vmp, KF_OpenActivity	);
	DelKeyFunction(vmp, KF_OpenRank		);
	DelKeyFunction(vmp, KF_OpenSkillUp	);
	DelKeyFunction(vmp, KF_OpenReputation	);
	DelKeyFunction(vmp, KF_OpenStoryQuest	);
	DelKeyFunction(vmp, KF_Mail	);
	DelKeyFunction(vmp, KF_Mall	);
	DelKeyFunction(vmp, KF_GoldStore	);
	DelKeyFunction(vmp, KF_Handbook	);
	DelKeyFunction(vmp, KF_Assist	);
	DelKeyFunction(vmp, KF_ChangePkMode );

	DelKeyFunction(vmp, KF_ChangeRole );
	DelKeyFunction(vmp, KF_EnterGroup );
	DelKeyFunction(vmp, KF_LeaveGroup );
	DelKeyFunction(vmp, KF_AddAttention );
	DelKeyFunction(vmp, KF_AddEnemy );
	DelKeyFunction(vmp, KF_AddBlackList );
	DelKeyFunction(vmp, KF_ShowHidePlayer );
	DelKeyFunction(vmp, KF_ChangePetAI );
	DelKeyFunction(vmp, KF_KeepRunning);
	DelKeyFunction(vmp, KF_AUCTION);

	if(nModfGCTType == GC_DIABLO3){
		//vmp.Set(KKeyworkd(KNF_None, _T('C')),		KF_OpenRole);
		//vmp.Set(KKeyworkd(KNF_None, _T('B')),		KF_OpenBag);
		//vmp.Set(KKeyworkd(KNF_None, _T('V')),		KF_OpenHeaven);
		//vmp.Set(KKeyworkd(KNF_None, _T('L')),		KF_OpenTask);
		//vmp.Set(KKeyworkd(KNF_None, _T('U')),		KF_OpenIntensify);
		//vmp.Set(KKeyworkd(KNF_None, _T('I')),		KF_OpenMount);
		//vmp.Set(KKeyworkd(KNF_None, _T('R')),		KF_OpenBaptize);
		//vmp.Set(KKeyworkd(KNF_None, _T('G')),		KF_OpenGuild);
		//vmp.Set(KKeyworkd(KNF_None, _T('P')),		KF_OpenFriend);
		//vmp.Set(KKeyworkd(KNF_None, _T('X')),		KF_OpenMaster);
		//vmp.Set(KKeyworkd(KNF_None, _T('T')),		KF_OpenTeam);
		//vmp.Set(KKeyworkd(KNF_None, _T('N')),		KF_OpenRide);
		//vmp.Set(KKeyworkd(KNF_None, _T('H')),		KF_OpenHero);
		//vmp.Set(KKeyworkd(KNF_None, _T('Y')),		KF_OpenAchieve);
		//vmp.Set(KKeyworkd(KNF_None, _T('D')),		KF_OpenOption);
		//vmp.Set(KKeyworkd(KNF_None, _T('F')),		KF_QuickPick);
		//vmp.Set(KKeyworkd(KNF_None, _T('M')),		KF_OpenMinimap);
		//vmp.Set(KKeyworkd(KNF_None, VK_TAB),		KF_ToggleMinimapType);
		//vmp.Set(KKeyworkd(KNF_None, _T('K')),		KF_OpenSkill);
		//vmp.Set(KKeyworkd(KNF_None, _T('J')),		KF_OpenArena);
		vmp.Set(KKeyworkd(KNF_None, _T('Q')),		KF_OpenActivity);
		//vmp.Set(KKeyworkd(KNF_None, _T('O')),		KF_OpenRank);
		//vmp.Set(KKeyworkd(KNF_None, _T('Z')),		KF_OpenSkillUp);
		//vmp.Set( KKeyworkd(KNF_None,_T('W')),		KF_OpenReputation );
		//vmp.Set(KKeyworkd(KNF_None, _T('A')),		KF_OpenStoryQuest);

		vmp.Set(KKeyworkd(KNF_None, _T('C')),		KF_OpenRole);
		vmp.Set(KKeyworkd(KNF_None, _T('I')),		KF_OpenBag);
		vmp.Set(KKeyworkd(KNF_None, _T('S')),		KF_OpenSkillUp);
		vmp.Set(KKeyworkd(KNF_None, _T('V')),		KF_OpenHeaven);

		vmp.Set(KKeyworkd(KNF_None, _T('L')),		KF_OpenTask);
		vmp.Set(KKeyworkd(KNF_None, _T('J')),		KF_OpenStoryQuest);
		//vmp.Set(KKeyworkd(KNF_None, _T('E')),		KF_OpenPlayerInfoView);
		vmp.Set(KKeyworkd(KNF_None, _T('G')),		KF_OpenGuild);

		vmp.Set(KKeyworkd(KNF_None, _T('X')),		KF_OpenMaster);
		vmp.Set(KKeyworkd(KNF_None, _T('O')),		KF_OpenFriend);
		vmp.Set(KKeyworkd(KNF_None, _T('T')),		KF_OpenTeam);
		vmp.Set(KKeyworkd(KNF_None, _T('N')),		KF_OpenRide);

		vmp.Set(KKeyworkd(KNF_None, _T('U')),		KF_OpenReputation );
		vmp.Set(KKeyworkd(KNF_None, _T('Y')),		KF_OpenAchieve);
		vmp.Set(KKeyworkd(KNF_None, _T('K')),		KF_OpenRank);
		vmp.Set(KKeyworkd(KNF_None, _T('A')),		KF_OpenActivity);

		vmp.Set(KKeyworkd(KNF_None, _T('B')),		KF_Mail);
		vmp.Set(KKeyworkd(KNF_None, _T('P')),		KF_Mall);
		vmp.Set(KKeyworkd(KNF_None, _T('D')),		KF_GoldStore);
		vmp.Set(KKeyworkd(KNF_None, _T('E')),		KF_Handbook);
		vmp.Set(KKeyworkd(KNF_None, VK_F12),		KF_Assist);
		vmp.Set(KKeyworkd(KNF_None, _T('F')),		KF_QuickPick);

		vmp.Set(KKeyworkd(KNF_None, _T('M')),		KF_OpenMinimap);
		vmp.Set(KKeyworkd(KNF_None, VK_TAB),		KF_ToggleMinimapType);
		vmp.Set(KKeyworkd(KNF_None, VK_ESCAPE),		KF_OpenOption);
		vmp.Set(KKeyworkd(KNF_Ctrl, _T('H')),		KF_ChangePkMode);

		vmp.Set(KKeyworkd(KNF_Alt, _T('X')),		KF_ChangeRole);
		vmp.Set(KKeyworkd(KNF_Alt, _T('W')),		KF_EnterGroup);
		vmp.Set(KKeyworkd(KNF_Alt, _T('E')),		KF_LeaveGroup);
		vmp.Set(KKeyworkd(KNF_Ctrl, _T('F')),		KF_AddAttention);

		vmp.Set(KKeyworkd(KNF_Ctrl, _T('C')),		KF_AddEnemy);
		vmp.Set(KKeyworkd(KNF_Ctrl, _T('S')),		KF_AddBlackList);
		vmp.Set(KKeyworkd(KNF_None, VK_F8),			KF_ShowHidePlayer);

		//vmp.Set(KKeyworkd(KNF_None,			),		KF_OpenWeapon);
		//vmp.Set(KKeyworkd(KNF_None,			),		KF_OpenIntensify);
		//vmp.Set(KKeyworkd(KNF_Ctrl, _T('R')),		KF_OpenBaptize);
		//vmp.Set(KKeyworkd(KNF_Ctrl, _T('G')),		KF_OpenMount);
		//vmp.Set(KKeyworkd(KNF_None, _T('H')),		KF_OpenHero);
		////vmp.Set(KKeyworkd(KNF_None, _T('A')),		KF_OpenArena);
		////vmp.Set(KKeyworkd(KNF_None, _T('K')),		KF_OpenSkill);

	}
	else if(nModfGCTType == GC_LOL){
		//vmp.Set(KKeyworkd(KNF_None, _T('E')),		KF_OpenPlayerInfoView);
		vmp.Set(KKeyworkd(KNF_None, _T('C')),		KF_OpenRole);
		vmp.Set(KKeyworkd(KNF_None, _T('B')),		KF_OpenBag);
		vmp.Set(KKeyworkd(KNF_None, _T('S')),		KF_OpenSkillUp);
		
		vmp.Set(KKeyworkd(KNF_None, _T('V')),		KF_OpenHeaven);
		vmp.Set(KKeyworkd(KNF_None, _T('L')),		KF_OpenTask);
		vmp.Set(KKeyworkd(KNF_None, _T('J')),		KF_OpenStoryQuest);
		vmp.Set(KKeyworkd(KNF_None, _T('G')),		KF_OpenGuild);

		vmp.Set(KKeyworkd(KNF_None, _T('X')),		KF_OpenMaster);
		vmp.Set(KKeyworkd(KNF_None, _T('O')),		KF_OpenFriend);
		vmp.Set(KKeyworkd(KNF_None, _T('T')),		KF_OpenTeam);
		vmp.Set(KKeyworkd(KNF_None, _T('N')),		KF_OpenRide);

		vmp.Set(KKeyworkd(KNF_None, _T('U')),		KF_OpenReputation );
		vmp.Set(KKeyworkd(KNF_None, _T('Y')),		KF_OpenAchieve);
		vmp.Set(KKeyworkd(KNF_None, _T('K')),		KF_OpenRank);
		vmp.Set(KKeyworkd(KNF_None, _T('A')),		KF_OpenActivity);

		vmp.Set(KKeyworkd(KNF_None, _T('I')),		KF_Mail);
		vmp.Set(KKeyworkd(KNF_None, _T('P')),		KF_Mall);
		vmp.Set(KKeyworkd(KNF_None, _T('D')),		KF_GoldStore);
		vmp.Set(KKeyworkd(KNF_None, _T('E')),		KF_Handbook);
		vmp.Set(KKeyworkd(KNF_None, VK_F12),		KF_Assist);
		vmp.Set(KKeyworkd(KNF_None, _T('F')),		KF_QuickPick);

		vmp.Set(KKeyworkd(KNF_None, _T('M')),		KF_OpenMinimap);
		vmp.Set(KKeyworkd(KNF_None, VK_TAB),		KF_ToggleMinimapType);
		vmp.Set(KKeyworkd(KNF_None, VK_ESCAPE),		KF_OpenOption);
		vmp.Set(KKeyworkd(KNF_Ctrl, _T('H')),		KF_ChangePkMode);

		vmp.Set(KKeyworkd(KNF_Alt, _T('X')),		KF_ChangeRole);
		vmp.Set(KKeyworkd(KNF_Alt, _T('W')),		KF_EnterGroup);
		vmp.Set(KKeyworkd(KNF_Alt, _T('E')),		KF_LeaveGroup);
		vmp.Set(KKeyworkd(KNF_Ctrl, _T('F')),		KF_AddAttention);

		vmp.Set(KKeyworkd(KNF_Ctrl, _T('C')),		KF_AddEnemy);
		vmp.Set(KKeyworkd(KNF_Ctrl, _T('S')),		KF_AddBlackList);
		vmp.Set(KKeyworkd(KNF_None, VK_F8),			KF_ShowHidePlayer);
	}
	else {
		//vmp.Set(KKeyworkd(KNF_None, _T('E')),		KF_OpenPlayerInfoView);
		vmp.Set(KKeyworkd(KNF_None, VK_F10),		KF_OpenRole);		//  _T('C')),		KF_OpenRole);
		vmp.Set(KKeyworkd(KNF_None, VK_F9),			KF_OpenBag);		//_T('B')),		KF_OpenBag);
		vmp.Set(KKeyworkd(KNF_None, VK_F11),		KF_OpenSkillUp);	//_T('S')),		KF_OpenSkillUp);

		vmp.Set(KKeyworkd(KNF_None, _T('V')),		KF_OpenHeaven);
		vmp.Set(KKeyworkd(KNF_None, _T('L')),		KF_OpenTask);
		vmp.Set(KKeyworkd(KNF_None, _T('J')),		KF_OpenStoryQuest);
		vmp.Set(KKeyworkd(KNF_None, _T('G')),		KF_OpenGuild);

		vmp.Set(KKeyworkd(KNF_None, _T('X')),		KF_OpenMaster);
		vmp.Set(KKeyworkd(KNF_None, _T('O')),		KF_OpenFriend);
		vmp.Set(KKeyworkd(KNF_None, _T('T')),		KF_OpenTeam);
		vmp.Set(KKeyworkd(KNF_None, _T('N')),		KF_OpenRide);

		vmp.Set(KKeyworkd(KNF_None, _T('U')),		KF_OpenReputation );
		vmp.Set(KKeyworkd(KNF_None, _T('Y')),		KF_OpenAchieve);
		vmp.Set(KKeyworkd(KNF_None, _T('K')),		KF_OpenRank);
		vmp.Set(KKeyworkd(KNF_None, _T('A')),		KF_OpenActivity);

		vmp.Set(KKeyworkd(KNF_None, _T('I')),		KF_Mail);
		vmp.Set(KKeyworkd(KNF_None, _T('P')),		KF_Mall);
		vmp.Set(KKeyworkd(KNF_None, _T('D')),		KF_GoldStore);
		vmp.Set(KKeyworkd(KNF_None, _T('E')),		KF_Handbook);
		vmp.Set(KKeyworkd(KNF_None, VK_F12),		KF_Assist);
		vmp.Set(KKeyworkd(KNF_None, _T('F')),		KF_QuickPick);

		vmp.Set(KKeyworkd(KNF_None, _T('M')),		KF_OpenMinimap);
		vmp.Set(KKeyworkd(KNF_None, VK_TAB),		KF_ToggleMinimapType);
		vmp.Set(KKeyworkd(KNF_None, VK_ESCAPE),		KF_OpenOption);
		vmp.Set(KKeyworkd(KNF_Ctrl, _T('H')),		KF_ChangePkMode);

		vmp.Set(KKeyworkd(KNF_Alt, _T('X')),		KF_ChangeRole);
		vmp.Set(KKeyworkd(KNF_Alt, _T('W')),		KF_EnterGroup);
		vmp.Set(KKeyworkd(KNF_Alt, _T('E')),		KF_LeaveGroup);
		vmp.Set(KKeyworkd(KNF_Ctrl, _T('F')),		KF_AddAttention);

		vmp.Set(KKeyworkd(KNF_Ctrl, _T('C')),		KF_AddEnemy);
		vmp.Set(KKeyworkd(KNF_Ctrl, _T('S')),		KF_AddBlackList);
		vmp.Set(KKeyworkd(KNF_Ctrl, _T('I')),		KF_ShowHidePlayer);
		vmp.Set(KKeyworkd(KNF_Ctrl, _T('A')),		KF_ChangePetAI);
		vmp.Set(KKeyworkd(KNF_None, VK_NUMLOCK),		KF_KeepRunning);
		vmp.Set(KKeyworkd(KNF_None, _T('C')), KF_AUCTION);
	}



	UMMOUIManager* UM = MMOUI::GetUIManager();
	UMMOUIMenuBar* mb = Cast<UMMOUIMenuBar>(UM->FindUIPanel(UM->Panel_Skillbar_Menubar));
	if (mb)
		mb->InitMenuSlot();

	check(vmKeyBind.Num() <= KF_Max);
}

//
void UMMOKeyManager::DoAltFunc( UBOOL isPressed )
{
	FViewport* vp = GEngine->GameViewport->Viewport;
	if (vp && (vp->KeyState(KEY_LeftControl) || vp->KeyState(KEY_RightControl)))
	{
		return;
	}

	////MMODropManager::getSingleton().SetShowDropHud(!MMODropManager::getSingleton().GetShowDropHud());
	//UBOOL isToShow = (MMO::GetCurrentGameState() == EGameState_InGame) && isPressed;

	//UBOOL bCurDropShow = MMODropManager::getSingleton().GetShowDropHud();
	//if( bCurDropShow != isPressed ){
	//	MMODropManager::getSingleton().SetShowDropHud( isToShow );
	//}

	////
	//UMMOUIManager* UM = MMOUI::GetUIManager();
	//UMMOUISkillBar* skb = Cast<UMMOUISkillBar>(UM->FindUIPanel(UM->Panel_Skillbar));
	//if( skb && skb->IsSkillSettingBtnShow != isPressed ){
	//	skb->eventUCToggleShowSkillShortcutButtons( isToShow );

	//	//
	//	//UM->delegateShowHintBtn( isToShow );

	//	//
	//	AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
	//	if( pc != NULL && !isPressed ){
	//		pc->eventOnAltKeyPressed( isPressed );
	//	}	
	//}
}