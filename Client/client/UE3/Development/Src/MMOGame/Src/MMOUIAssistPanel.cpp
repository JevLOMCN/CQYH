#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include <time.h>
#include "UITextParser.h"
#include "MMOAssistManager.h"
#include "FSMState.h"
#include "MMODropManager.h"
#include "MMOWeatherManager.h"
#include "MMOSkillManager.h"
#include "MMOGuideManager.h"
#include "Mir2Attack.h"

IMPLEMENT_CLASS(UMMOUIAssistPanel)

void UMMOUIAssistPanel::Init()
{
	Super::Init();

	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_MainPlayerDataChange);
	EventAdapter = new TGameEventAdapter<UMMOUIAssistPanel>(this, evts);
}

void UMMOUIAssistPanel::UnInit()
{
	Super::UnInit();
}

void UMMOUIAssistPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if (v)
	{	
		UpdateAssistSkillData();
		UpdateAssistDropClassList();
		UpdateAssistDropList(EADT_ALL);

		AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if (mainPlayer && mainPlayer->ActorAttributes)
		{
			eventCallUpdatePlayerHpMp(mainPlayer->ActorAttributes->GetMaxHp(), mainPlayer->ActorAttributes->GetMaxMp());
		}
		UpdateAssistItemData();
		UpdateAssistItemList();

		UpdateAssistGameData();
		UpdateAssistHelp();
		ShowKeyWarning(FALSE);
		UMMOKeyManager::getSingleton()->OnOpenUI();
	}
}

void UMMOUIAssistPanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (!IsVisible)
	{
		return;
	}

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
						eventCallUpdatePlayerHpMp(mainPlayer->ActorAttributes->GetMaxHp(), mainPlayer->ActorAttributes->GetMaxMp());
					}
				}	
				break;
			}
		}
		break;
	}
}

void UMMOUIAssistPanel::UpdateAssistDropClassList()
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	UGFxObject* objectArray = GFXPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	int index = 0;
	for (INT i = EADT_ALL; i < EADT_MAX; ++ i)
	{
		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		if (object)
		{
			object->SetInt(TEXT("index"), index);
			switch (i)
			{
			case EADT_ALL:
				object->SetString(TEXT("label"), TEXT("All Categories"));
				break;
			case EADT_OTHER:
				object->SetString(TEXT("label"), TEXT("Other"));
				break;
			case EADT_DRUG:
				object->SetString(TEXT("label"), TEXT("Potions"));
				break;
			case EADT_CLOTHING:
				object->SetString(TEXT("label"), TEXT("Clothing"));
				break;
			case EADT_WEAPON:
				object->SetString(TEXT("label"), TEXT("Weapons"));
				break;
			case EADT_ACCESSORY:
				object->SetString(TEXT("label"), TEXT("Jewelry"));
				break;
			}

			objectArray->SetElementObject(index, object);
			index ++;
		}
	}

	eventCallUpdateAssistDropClassList(objectArray);
}

void UMMOUIAssistPanel::UpdateAssistDropList(UINT32 itemClass)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	UGFxObject* objectArray = GFXPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	std::set<UINT32> groupSet;
	groupSet.clear();

	AssistDropInfoMap assistItemInfoMap = MMOAssistData::getSingleton().GetAssistDropInfoMap();
	AssistDropInfoMap::iterator iter = assistItemInfoMap.begin();
	int index = 0;
	for (; iter != assistItemInfoMap.end(); ++ iter)
	{
		AssistDropData data;
		if (!MMOAssistData::getSingleton().GetAssistDropData(iter->first, data))
		{
			continue;
		}

		AssistDropInfo info = iter->second;
		UItem* item = MMOFacade::getSingleton().FindItemTemplate(info.Id);
		if (!item)
		{
			continue;
		}

		if (itemClass != EADT_ALL && itemClass != data.ItemClass)
		{
			continue;
		}

		UGFxObject* object = GFXPlayer->CreateObject(TEXT("UI_Hang.AS3.HangItemData"));
		if (object)
		{
			//object->SetString(TEXT("label"), item->GetDisplayName());
			//object->SetInt(TEXT("index"), index);
			//object->SetInt(TEXT("type"), 0);
			//object->SetInt(TEXT("deepth"), 0);

			object->SetInt(TEXT("itemID"), info.Id);
			object->SetString(TEXT("itemName"), item->GetDisplayName());
			object->SetInt(TEXT("itemType"), data.ItemClass);
			object->SetBool(TEXT("isShowName"), info.Visible);
			object->SetBool(TEXT("isBest"), info.Valuable);
			object->SetBool(TEXT("isAuto"), info.AutoPick);

			if (data.Group != 0)
			{
				std::set<UINT32>::iterator iterGroup = groupSet.find(data.Group);
				if (iterGroup != groupSet.end())
				{
					object->SetBool(TEXT("visible"), FALSE);
				}
				else
				{
					object->SetBool(TEXT("visible"), TRUE);
					groupSet.insert(data.Group);
				}
			}
			else
			{
				object->SetBool(TEXT("visible"), TRUE);
			}

			objectArray->SetElementObject(index, object);
			index ++;
		}
	}

	eventCallUpdateAssistDropList(objectArray);
}

void UMMOUIAssistPanel::UpdateAssistSkillData()
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("UI_Hang.AS3.SkillConfigData"));
	if (!object)
	{
		return;
	}

	object->SetBool(TEXT("warrior_0"), FSM::Ctrl.ctrlFlag_DDCS);
	object->SetBool(TEXT("warrior_1"), FSM::Ctrl.ctrlFlag_ZNBY);
	object->SetBool(TEXT("warrior_2"), FSM::Ctrl.ctrlFlag_ZDLH);
	object->SetBool(TEXT("warrior_3"), FSM::Ctrl.ctrlFlag_ZDZR);
	object->SetBool(TEXT("master_0"), FSM::Ctrl.ctrlFlag_ZDKD);
	object->SetBool(TEXT("taoist_0"), FSM::Ctrl.ctrlFlag_ZDYS);

	object->SetBool(TEXT("autoTimer"), Mir2Attack::getSingleton().IsAutoPractice());
	object->SetInt(TEXT("timer"), Mir2Attack::getSingleton().GetAutoPracticeTime());
	int index = 0;
	UGFxObject* skillArray = GFXPlayer->CreateArray();
	if (skillArray)
	{
		for (INT i = ECK_Slot1; i <= ECK_Slot12; ++ i)
		{
			UGFxObject* skillObject = GFXPlayer->CreateObject(TEXT("Object"));
			if (skillObject)
			{
				UMMOAbility* equippedSkill = MMOSkillManager::getSingleton().GetEquippedSkill(i);
				if (equippedSkill)
				{
					FString skillName = TEXT("");
					UBOOL isRuneSkill = (equippedSkill->GetRuneSlot() > 0);
					UMMOAbility* mainSkill = NULL;
					if (isRuneSkill)
					{
						mainSkill = MMOSkillManager::getSingleton().GetMainAbilityFor(equippedSkill);
						if (mainSkill)
						{
							skillName = mainSkill->eventGetAbilityName()
								+ TEXT("[")
								+ equippedSkill->eventGetAbilityName()
								+ TEXT("]");

						}
					}
					else
					{
						skillName = equippedSkill->eventGetAbilityName();
					}

					equippedSkill->eventGetAbilityName();

					skillObject->SetInt(TEXT("index"), equippedSkill->GetTemplateID());
					skillObject->SetString(TEXT("label"), skillName);

					skillArray->SetElementObject(index, skillObject);
					index ++;
				}
			}
		}

		object->SetObject(TEXT("skillArr"), skillArray);
	}

	if (Mir2Attack::getSingleton().GetAutoPracticeAbilityID() > 0)
	{
		object->SetInt(TEXT("skillId"), Mir2Attack::getSingleton().GetAutoPracticeAbilityID());
	}
	else
	{
		for (INT i = ECK_Slot1; i <= ECK_Slot12; ++ i)
		{
			UMMOAbility* equippedSkill = MMOSkillManager::getSingleton().GetEquippedSkill(i);
			if (equippedSkill)
			{
				object->SetInt(TEXT("skillId"), equippedSkill->GetTemplateID());
				break;
			}
		}
	}

	eventCallUpdateAssistSkillData(object);
}

void UMMOUIAssistPanel::UpdateAssistHelp()
{
	FString content = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Assist.Help"));
	eventCallUpdateAssistHelp(content);
}

void UMMOUIAssistPanel::ASApplyHangItemConfig(class UGFxObject* dataProvider)
{
	if (!dataProvider)
	{
		return;
	}

	for (INT i = 0; dataProvider->GetElementObject(i) != NULL; ++ i)
	{
		UGFxObject* object = dataProvider->GetElementObject(i);
		if (object)
		{
			UINT32 id = object->GetInt(TEXT("itemID"));
			UBOOL valuable = object->GetBool(TEXT("isBest"));
			UBOOL autoPick = object->GetBool(TEXT("isAuto"));
			UBOOL visible = object->GetBool(TEXT("isShowName"));
			MMOAssistData::getSingleton().UpdateAssistDropInfo(id, valuable, autoPick, visible);

			MMOAssistManager::getSingleton().UpdateDropDisplay(id);
		}
	}

	MMOAssistManager::getSingleton().SaveData();
}

void UMMOUIAssistPanel::ASHangConfigItemData(int page)
{
	MMOAssistData::getSingleton().UpdateAssistDropInfoDefault();
	UpdateAssistDropList(EADT_ALL);
}

void UMMOUIAssistPanel::ASApplyHangSkillConfig(class UGFxObject* dataProvider)
{
	if (!dataProvider)
	{
		return;
	}

	if (dataProvider)
	{
		FSM::Ctrl.ctrlFlag_DDCS = dataProvider->GetBool(TEXT("warrior_0"));
		FSM::Ctrl.ctrlFlag_ZNBY = dataProvider->GetBool(TEXT("warrior_1"));
		FSM::Ctrl.ctrlFlag_ZDLH = dataProvider->GetBool(TEXT("warrior_2"));
		FSM::Ctrl.ctrlFlag_ZDZR = dataProvider->GetBool(TEXT("warrior_3"));
		FSM::Ctrl.ctrlFlag_ZDKD = dataProvider->GetBool(TEXT("master_0"));
		FSM::Ctrl.ctrlFlag_ZDYS = dataProvider->GetBool(TEXT("taoist_0"));

		UBOOL isPractise = dataProvider->GetInt(TEXT("autoTimer"));
		UINT8 timer = dataProvider->GetInt(TEXT("timer"));
		UINT16 skillId = dataProvider->GetInt(TEXT("skillId"));
		Mir2Attack::getSingleton().AutoPractice(skillId, timer, isPractise);
	}

	MMOAssistManager::getSingleton().SaveData();
}

void UMMOUIAssistPanel::ASHangConfigSkillData()
{
	FSM::Ctrl.ctrlFlag_DDCS = FALSE;
	FSM::Ctrl.ctrlFlag_ZNBY = FALSE;
	FSM::Ctrl.ctrlFlag_ZDLH = FALSE;
	FSM::Ctrl.ctrlFlag_ZDZR = FALSE;
	FSM::Ctrl.ctrlFlag_ZDKD = FALSE;
	FSM::Ctrl.ctrlFlag_ZDYS = FALSE;
	Mir2Attack::getSingleton().AutoPractice(0, DEFAULT_PRACTISE_TIME, FALSE);

	UpdateAssistSkillData();
}

void UMMOUIAssistPanel::UpdateAssistItemData()
{
	UGFxObject* objectArray = GFXPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	int index = 0;
	for (INT i = EPT_SLOW_HP; i < EPT_TOTAL; ++ i)
	{
		AssistItemInfo info;
		if (!MMOAssistData::getSingleton().GetAssistItemInfo(i, info))
		{
			continue;
		}

		UGFxObject* object = GFXPlayer->CreateObject(TEXT("UI_Hang.AS3.ProtectData"));
		if (object)
		{
			object->SetBool(TEXT("isSelect"), info.Enable);
			object->SetInt(TEXT("id"), info.ItemId);
			object->SetInt(TEXT("num"), info.Value);
			object->SetInt(TEXT("timer"), info.Time);

			objectArray->SetElementObject(index, object);
			index ++;
		}
	}

	eventCallUpdateAssistItemData(objectArray);
}

void UMMOUIAssistPanel::UpdateAssistItemList()
{
	UGFxObject* objectArray = GFXPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	AssistItemDataMap assistItemDataMap = MMOAssistData::getSingleton().GetAssistItemDataMap();
	AssistItemDataMap::iterator iter = assistItemDataMap.begin();
	int index = 0;
	for (; iter != assistItemDataMap.end(); ++ iter)
	{
		AssistItemData data = iter->second;
		UItem* item = MMOFacade::getSingleton().FindItemTemplate(data.ItemId);
		if (!item)
		{
			continue;
		}

		UGFxObject* object = GFXPlayer->CreateObject(TEXT("UI_Hang.AS3.ItemData"));
		if (object)
		{
			object->SetInt(TEXT("index"), data.ItemId);
			object->SetString(TEXT("label"), item->GetDisplayName());
			object->SetInt(TEXT("itemType"), data.ItemType);

			objectArray->SetElementObject(index, object);
			index ++;
		}
	}

	eventCallUpdateAssistItemList(objectArray);
}

void UMMOUIAssistPanel::ASApplyProtectConfig(class UGFxObject* dataProvider)
{
	if (!dataProvider)
	{
		return;
	}

	for (INT i = 0; dataProvider->GetElementObject(i) != NULL; ++ i)
	{
		UGFxObject* object = dataProvider->GetElementObject(i);
		if (object)
		{
			if (i >= EPT_SLOW_HP && i < EPT_TOTAL)
			{
				AssistItemInfo info;
				info.ProtectType = i;
				info.Enable = object->GetBool(TEXT("isSelect"));
				info.ItemId = object->GetInt(TEXT("id"));
				info.Value = object->GetInt(TEXT("num"));
				info.Time = object->GetInt(TEXT("timer"));
				if (info.Time < MIN_PROTECT_TIME)
				{
					info.Time = MIN_PROTECT_TIME;
				}
				MMOAssistData::getSingleton().UpdateAssistItemInfo(i, info);
			}
		}
	}

	// refresh the Time correction value here
	UpdateAssistItemData();
	UpdateAssistItemList();
	MMOAssistManager::getSingleton().SaveData();
}


void UMMOUIAssistPanel::ASProtectConfiglData()
{
	MMOAssistData::getSingleton().UpdateAssistItemInfoDefault();
	UpdateAssistItemData();
	UpdateAssistItemList();
}

void UMMOUIAssistPanel::UpdateAssistGameData()
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("UI_Hang.AS3.ConfigGameData"));
	if (!object)
	{
		return;
	}

	UpdateAssistDisplayData(object);
	UpdateAssistInputData(object, FALSE, MMOFacade::getSingleton().GCTType);
	eventCallUpdateAssistGameData(object);
}

void UMMOUIAssistPanel::ModifyAssistInputData(UBOOL mdfConfig, INT modfGCTType)
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("UI_Hang.AS3.ConfigGameData"));
	if (!object)
	{
		return;
	}

	UpdateAssistInputData(object, mdfConfig, modfGCTType);
	eventCallUpdateAssistGameData(object);
}

void UMMOUIAssistPanel::UpdateAssistDisplayData(class UGFxObject* object)
{
	if (!object)
	{
		return;
	}

	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (!uiManager)
	{
		return;
	}

	object->SetBool(_T("game1_enable"), TRUE);
	object->SetBool(_T("HideName_my"), uiManager->HintHideNameSelf);
	object->SetBool(_T("HideName_hero"), uiManager->HintHideNameSelfTitle);
	object->SetBool(_T("HideNameOther_my"), uiManager->HintHideNameOther);
	object->SetBool(_T("HideNameOther_hero"), uiManager->HintHideNameOtherTitle);
	object->SetBool(_T("HideNameFamily_my"), uiManager->HintHideNameFamilySelf);
	object->SetBool(_T("HideNameFamily_other"), uiManager->HintHideNameFamilyOther);
	object->SetBool(_T("HideNameMonster"), uiManager->HintHideNameMonster);
	object->SetBool(_T("HideNameDrop"), uiManager->HintHideNameDrop);
	object->SetBool(_T("DrawBlood_my"), uiManager->BloodDrawSelf);
	object->SetBool(_T("DrawBlood_other"), uiManager->BloodDrawPlayer);
	object->SetBool(_T("DrawBlood_monster"), uiManager->BloodDrawMonster);
	object->SetBool(_T("HideHurtNumber_my"), uiManager->HurtHideNameSelf);
	//object->SetBool(_T("HideHurtNumber_other"), uiManager->HurtHideNameOther);
	object->SetBool(_T("HideSkillEffect_my"), uiManager->SkillEffectHideSelf);
	object->SetBool(_T("HideSkillEffect_other"), uiManager->SkillEffectHideOther);
	object->SetBool(_T("HideSkillEffect_CameraShake"), uiManager->SkillEffectHideCameraShake);
	object->SetBool(_T("DrawGuide"), uiManager->DrawGuide);
	object->SetBool(_T("DrawChatBubble"), uiManager->DrawChatBubble);
	//object->SetBool(_T("DrawAITip"), uiManager->DrawAITip);
	//object->SetBool(_T("DrawWeather"), uiManager->DrawWeather);
	object->SetBool(_T("DrawBloodNumber"), uiManager->DrawBloodNumber);
	object->SetBool(_T("DrawHurt"), uiManager->DrawHurt);
	object->SetBool(_T("DrawGameFrame"), uiManager->DrawGameFrame);
	object->SetBool(_T("HintFriendOnline"),	uiManager->HintFriendOnline);
	object->SetBool(_T("HintHidePlayer"), uiManager->HintHideNearPlayer);
	object->SetBool(_T("HintMarkSelf"), uiManager->HintMarkSelf);

	object->SetBool(_T("game2_enable"), TRUE);
	object->SetBool(_T("DeclineStrangeChat"), uiManager->DeclineStrangeChat);
	object->SetBool(_T("DeclineInviteTeam"), uiManager->DeclineInviteTeam);
	object->SetBool(_T("DeclineInviteFamily"), uiManager->DeclineInviteFamily);
	object->SetBool(_T("DeclineInviteDisciple"), uiManager->DeclineInviteDisciple);
	object->SetBool(_T("DeclineInviteMaster"), uiManager->DeclineInviteMaster);
	object->SetBool(_T("DeclineShowEnemy"), uiManager->DeclineShowEnemy);	
	object->SetBool(_T("DefaultShift"), FSM::Ctrl.ctrlFlag_IgnoreShiftBtn);
	object->SetBool(_T("DeclineShowTrade"), uiManager->DeclineShowTrade);
	
	object->SetBool(_T("game3_enable"), TRUE);
	object->SetInt(_T("taskModeType"), uiManager->QuestMode ? 0 : 1);
}

void UMMOUIAssistPanel::UpdateAssistInputData(class UGFxObject* object, UBOOL mdfConfig, INT modfGCTType)
{
	if (!object)
	{
		return;
	}

	UMMOKeyManager* keyManager = UMMOKeyManager::getSingleton();
	if (!keyManager)
	{
		return;
	}

	FString cstr;
	object->SetBool(_T("input1_enable"), TRUE);
	UGFxObject* slotKeyArray = GFXPlayer->CreateArray();
	for (INT i = 0; i < 12; ++ i)
	{
		UGFxObject* slotKeyObject = GFXPlayer->CreateObject("Object");
		if (slotKeyObject)
		{
			slotKeyObject->SetString(TEXT("KeyName"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::EKeyFunction(UMMOKeyManager::KF_BarSkill1 + i), mdfConfig), cstr, 1));
			slotKeyObject->SetString(TEXT("ComKeyName"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::EKeyFunction(UMMOKeyManager::KF_BarSkill1 + i), mdfConfig), cstr, 2));
			slotKeyObject->SetInt(TEXT("KeyEnum"), UMMOKeyManager::KF_BarSkill1 + i);
		}
		slotKeyArray->SetElementObject(i, slotKeyObject);
	}
	object->SetObject(TEXT("SlotKeyList"), slotKeyArray);
	
	// CQ_SKILL_HJJ;Begin
	/*
	if (modfGCTType == GC_MIRFOREVER)
	{
		dt->SetString(_T("BarSkill1"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkillEx1, mdf_config), cstr, 1));
		dt->SetString(_T("BarSkill2"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkillEx2, mdf_config), cstr, 1));
		dt->SetString(_T("BarSkill3"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkillEx3, mdf_config), cstr, 1));
		dt->SetString(_T("BarSkill4"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkillEx4, mdf_config), cstr, 1));
		dt->SetString(_T("BarSkill5"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkillEx5, mdf_config), cstr, 1));
		dt->SetString(_T("BarSkill6"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkillEx6, mdf_config), cstr, 1));
		dt->SetString(_T("BarSkillA1"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkillEx1, mdf_config), cstr, 2));
		dt->SetString(_T("BarSkillA2"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkillEx2, mdf_config), cstr, 2));
		dt->SetString(_T("BarSkillA3"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkillEx3, mdf_config), cstr, 2));
		dt->SetString(_T("BarSkillA4"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkillEx4, mdf_config), cstr, 2));
		dt->SetString(_T("BarSkillA5"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkillEx5, mdf_config), cstr, 2));
		dt->SetString(_T("BarSkillA6"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkillEx6, mdf_config), cstr, 2));
		dt->SetInt(_T("BarSkillT1"), UMMOKeyManager::KF_BarSkillEx1);
		dt->SetInt(_T("BarSkillT2"), UMMOKeyManager::KF_BarSkillEx2);
		dt->SetInt(_T("BarSkillT3"), UMMOKeyManager::KF_BarSkillEx3);
		dt->SetInt(_T("BarSkillT4"), UMMOKeyManager::KF_BarSkillEx4);
		dt->SetInt(_T("BarSkillT5"), UMMOKeyManager::KF_BarSkillEx5);
		dt->SetInt(_T("BarSkillT6"), UMMOKeyManager::KF_BarSkillEx6);

		dt->SetString(_T("BarItem1"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItemEx1, mdf_config), cstr, 1));
		dt->SetString(_T("BarItem2"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItemEx2, mdf_config), cstr, 1));
		dt->SetString(_T("BarItem3"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItemEx3, mdf_config), cstr, 1));
		dt->SetString(_T("BarItem4"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItemEx4, mdf_config), cstr, 1));
		dt->SetString(_T("BarItem5"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItemEx5, mdf_config), cstr, 1));
		dt->SetString(_T("BarItem6"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItemEx6, mdf_config), cstr, 1));
		dt->SetString(_T("BarItemA1"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItemEx1, mdf_config), cstr, 2));
		dt->SetString(_T("BarItemA2"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItemEx2, mdf_config), cstr, 2));
		dt->SetString(_T("BarItemA3"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItemEx3, mdf_config), cstr, 2));
		dt->SetString(_T("BarItemA4"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItemEx4, mdf_config), cstr, 2));
		dt->SetString(_T("BarItemA5"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItemEx5, mdf_config), cstr, 2));
		dt->SetString(_T("BarItemA6"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItemEx6, mdf_config), cstr, 2));
		dt->SetInt(_T("BarItemT1"), UMMOKeyManager::KF_BarItemEx1);
		dt->SetInt(_T("BarItemT2"), UMMOKeyManager::KF_BarItemEx2);
		dt->SetInt(_T("BarItemT3"), UMMOKeyManager::KF_BarItemEx3);
		dt->SetInt(_T("BarItemT4"), UMMOKeyManager::KF_BarItemEx4);
		dt->SetInt(_T("BarItemT5"), UMMOKeyManager::KF_BarItemEx5);
		dt->SetInt(_T("BarItemT6"), UMMOKeyManager::KF_BarItemEx6);

	}
	else
	{
		dt->SetString(_T("BarSkill1"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkill1, mdf_config), cstr, 1));
		dt->SetString(_T("BarSkill2"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkill2, mdf_config), cstr, 1));
		dt->SetString(_T("BarSkill3"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkill3, mdf_config), cstr, 1));
		dt->SetString(_T("BarSkill4"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkill4, mdf_config), cstr, 1));
		dt->SetString(_T("BarSkill5"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkill5, mdf_config), cstr, 1));
		dt->SetString(_T("BarSkill6"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkill6, mdf_config), cstr, 1));
		dt->SetString(_T("BarSkillA1"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkill1, mdf_config), cstr, 2));
		dt->SetString(_T("BarSkillA2"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkill2, mdf_config), cstr, 2));
		dt->SetString(_T("BarSkillA3"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkill3, mdf_config), cstr, 2));
		dt->SetString(_T("BarSkillA4"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkill4, mdf_config), cstr, 2));
		dt->SetString(_T("BarSkillA5"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkill5, mdf_config), cstr, 2));
		dt->SetString(_T("BarSkillA6"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarSkill6, mdf_config), cstr, 2));
		dt->SetInt(_T("BarSkillT1"), UMMOKeyManager::KF_BarSkill1);
		dt->SetInt(_T("BarSkillT2"), UMMOKeyManager::KF_BarSkill2);
		dt->SetInt(_T("BarSkillT3"), UMMOKeyManager::KF_BarSkill3);
		dt->SetInt(_T("BarSkillT4"), UMMOKeyManager::KF_BarSkill4);
		dt->SetInt(_T("BarSkillT5"), UMMOKeyManager::KF_BarSkill5);
		dt->SetInt(_T("BarSkillT6"), UMMOKeyManager::KF_BarSkill6);

		dt->SetString(_T("BarItem1"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItem1, mdf_config), cstr, 1));
		dt->SetString(_T("BarItem2"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItem2, mdf_config), cstr, 1));
		dt->SetString(_T("BarItem3"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItem3, mdf_config), cstr, 1));
		dt->SetString(_T("BarItem4"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItem4, mdf_config), cstr, 1));
		dt->SetString(_T("BarItem5"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItem5, mdf_config), cstr, 1));
		dt->SetString(_T("BarItem6"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItem6, mdf_config), cstr, 1));
		dt->SetString(_T("BarItemA1"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItem1, mdf_config), cstr, 2));
		dt->SetString(_T("BarItemA2"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItem2, mdf_config), cstr, 2));
		dt->SetString(_T("BarItemA3"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItem3, mdf_config), cstr, 2));
		dt->SetString(_T("BarItemA4"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItem4, mdf_config), cstr, 2));
		dt->SetString(_T("BarItemA5"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItem5, mdf_config), cstr, 2));
		dt->SetString(_T("BarItemA6"), keyManager->KeyToString(keyManager->FindKeyworld(UMMOKeyManager::KF_BarItem6, mdf_config), cstr, 2));
		dt->SetInt(_T("BarItemT1"), UMMOKeyManager::KF_BarItem1);
		dt->SetInt(_T("BarItemT2"), UMMOKeyManager::KF_BarItem2);
		dt->SetInt(_T("BarItemT3"), UMMOKeyManager::KF_BarItem3);
		dt->SetInt(_T("BarItemT4"), UMMOKeyManager::KF_BarItem4);
		dt->SetInt(_T("BarItemT5"), UMMOKeyManager::KF_BarItem5);
		dt->SetInt(_T("BarItemT6"), UMMOKeyManager::KF_BarItem6);
	}
	*/
	// CQ_SKILL_HJJ;Begin
	
	object->SetBool(_T("input2_enable"), TRUE);
	UGFxObject* keyArray = GFXPlayer->CreateArray();
	//check(klst);
	if (!keyArray)
	{
		return;
	}

	BYTE kIndex = UMMOKeyManager::KF_Func_Begin;
	for (BYTE k = UMMOKeyManager::KF_Func_Begin; k < UMMOKeyManager::KF_Func_End; ++ k)
	{
		UMMOKeyManager::KKeyworkd* keyWord = keyManager->FindKeyworld((UMMOKeyManager::EKeyFunction)k, mdfConfig);
		if (keyManager->CanDoFunction((UMMOKeyManager::EKeyFunction)k))
		{
			UGFxObject* keyObject = GFXPlayer->CreateObject(TEXT("UI_Hang.AS3.InputKeyItemData"));
			//check(ko);
			if (keyObject)
			{
				keyObject->SetInt(_T("ktype"), k);
				if (keyWord)
				{
					keyObject->SetString(_T("kkey"), keyManager->KeyToString(keyWord, cstr));
				}
				else
				{
					keyObject->SetString(_T("kkey"), _T(""));
				}
				keyObject->SetString(_T("kdesc"), keyManager->FuncToString((UMMOKeyManager::EKeyFunction)k, cstr));
				keyArray->SetElementObject(kIndex - UMMOKeyManager::KF_Func_Begin, keyObject);
				kIndex ++;
			}
		}
	}
	object->SetObject(_T("KeyList"), keyArray);
}


void UMMOUIAssistPanel::ASApplyGameConfig(class UGFxObject* dataProvider)
{	
	if (!dataProvider)
	{
        return;
	}

	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (!uiManager)
	{
		return;
	}

    if (dataProvider->GetBool(_T("game1_enable")))
    {
        uiManager->HintHideNameSelf = dataProvider->GetBool(_T("HideName_my"));
        uiManager->HintHideNameSelfTitle = dataProvider->GetBool(_T("HideName_hero"));
        uiManager->HintHideNameOther = dataProvider->GetBool(_T("HideNameOther_my"));
        uiManager->HintHideNameOtherTitle = dataProvider->GetBool(_T("HideNameOther_hero"));
        uiManager->HintHideNameFamilySelf = dataProvider->GetBool(_T("HideNameFamily_my"));
        uiManager->HintHideNameFamilyOther = dataProvider->GetBool(_T("HideNameFamily_other"));
        uiManager->HintHideNameMonster = dataProvider->GetBool(_T("HideNameMonster"));
        uiManager->HintHideNameDrop = dataProvider->GetBool(_T("HideNameDrop"));
        uiManager->BloodDrawSelf = dataProvider->GetBool(_T("DrawBlood_my"));
		uiManager->BloodDrawPlayer = dataProvider->GetBool(_T("DrawBlood_other"));
        uiManager->BloodDrawMonster = dataProvider->GetBool(_T("DrawBlood_monster"));
        uiManager->HurtHideNameSelf = dataProvider->GetBool(_T("HideHurtNumber_my"));
        //uiManager->HurtHideNameOther = dataProvider->GetBool(_T("HideHurtNumber_other"));
        uiManager->SkillEffectHideSelf = dataProvider->GetBool(_T("HideSkillEffect_my"));
		uiManager->SkillEffectHideOther = dataProvider->GetBool(_T("HideSkillEffect_other"));
		uiManager->SkillEffectHideCameraShake = dataProvider->GetBool(_T("HideSkillEffect_CameraShake"));
        uiManager->DrawGuide = dataProvider->GetBool(_T("DrawGuide"));
        uiManager->DrawChatBubble = dataProvider->GetBool(_T("DrawChatBubble"));
        //uiManager->DrawAITip = dataProvider->GetBool(_T("DrawAITip"));
        //uiManager->DrawWeather = dataProvider->GetBool(_T("DrawWeather"));
        uiManager->DrawBloodNumber = dataProvider->GetBool(_T("DrawBloodNumber"));
		uiManager->DrawHurt = dataProvider->GetBool(_T("DrawHurt"));
		uiManager->DrawGameFrame = dataProvider->GetBool(_T("DrawGameFrame"));
		uiManager->HintFriendOnline = dataProvider->GetBool(_T("HintFriendOnline"));
		uiManager->HintHideNearPlayer = dataProvider->GetBool(_T("HintHidePlayer"));
		uiManager->HintMarkSelf = dataProvider->GetBool(_T("HintMarkSelf"));
		
        MMOGuideManager::getSingleton().SetGuideEnable(uiManager->DrawGuide);
        MMODropManager::getSingleton().SetShowDropHud(!uiManager->HintHideNameDrop);
        if (uiManager->DrawWeather)
		{
            MMOWeatherManager::getSingleton().StartWeather(MMOWeatherManager::getSingleton().GetCurWeatherType());
		}
        else
		{
            MMOWeatherManager::getSingleton().StopAllWeather();
		}

        UMMOUISceneHint* sceneHint = Cast<UMMOUISceneHint>(uiManager->FindUIPanel(uiManager->Panel_SceneHint));
        if (sceneHint)
        {
			sceneHint->RefleshAllHints();
            //pPanel->UpdatePlayerTitle(TRUE);
            //pPanel->UpdatePlayerTitle(FALSE);
   //         if (!uiManager->DrawChatBubble)
			//{
   //             sceneHint->CloseAllTalkBubble();
			//}
        }
		// set HurtUI display
		MMOUI::GetUIManager()->ShowHurtPanel(uiManager->DrawHurt);
		
        UMMOUISkillBar* skillBar = Cast<UMMOUISkillBar>(uiManager->FindUIPanel(uiManager->Panel_Skillbar));
        if (skillBar)
		{
            skillBar->UpdateBloodTextVisible();
		}
		UMMOUIMinimap* miniMap = Cast<UMMOUIMinimap>(uiManager->FindUIPanel(uiManager->Panel_Minimap));
		if (miniMap)
		{
			miniMap->UpdateFramePinVisible();
		}

		// highlight own display
		AMMOActor* mainActor = MMOFacade::getSingleton().GetMainActor();
		if (mainActor && mainActor->ActorAttributes)
		{
			mainActor->ActorAttributes->ColorEx1 = uiManager->HintMarkSelf ? TEXT("#50B1FC") : TEXT("#FFFFFF");
			uiManager->delegateUpdateHintText(mainActor);
		}
    }

    if (dataProvider->GetBool(_T("game2_enable")))
    {
        uiManager->DeclineStrangeChat     = dataProvider->GetBool(_T("DeclineStrangeChat"));
        uiManager->DeclineInviteTeam      = dataProvider->GetBool(_T("DeclineInviteTeam"));
        uiManager->DeclineInviteFamily    = dataProvider->GetBool(_T("DeclineInviteFamily"));
        uiManager->DeclineInviteDisciple  = dataProvider->GetBool(_T("DeclineInviteDisciple"));
		uiManager->DeclineInviteMaster    = dataProvider->GetBool(_T("DeclineInviteMaster"));
		uiManager->DeclineShowEnemy		  = dataProvider->GetBool(_T("DeclineShowEnemy"));
		FSM::Ctrl.ctrlFlag_IgnoreShiftBtn = dataProvider->GetBool(_T("DefaultShift"));
		uiManager->DeclineShowTrade		  = dataProvider->GetBool(_T("DeclineShowTrade"));
    }

	if (dataProvider->GetBool(_T("game3_enable")))
	{
		UBOOL oldQuestMode = uiManager->QuestMode;
		uiManager->QuestMode = (dataProvider->GetInt(_T("taskModeType"))) == 0;

		// quest mode setting
		if (oldQuestMode != uiManager->QuestMode)
		{
			MMOGameEvent_Any evt(GameEvent_QuestModeChange);
			MMOUI::PostGameEvent(&evt);
		}
	}

	bool found = false;

	if (dataProvider->GetBool(_T("input1_enable")))
	{
		found = true;
		UMMOKeyManager::getSingleton()->OnApplyByUI();
		UMMOKeyManager::getSingleton()->SaveKeyword();
	}

	if (dataProvider->GetBool(_T("input2_enable")))
	{
		found = true;
		UMMOKeyManager::getSingleton()->OnApplyByUI();
		UMMOKeyManager::getSingleton()->SaveKeyword();
	}

	if(!found)
	{
		UMMOKeyManager::getSingleton()->nModfGCTType = GC_MIR2;
		UMMOKeyManager::getSingleton()->OnApplyByUI();
		UMMOKeyManager::getSingleton()->SaveKeyword();
	}

	uiManager->SaveConfigToServer();
	MMOFacade::getSingleton().NotifyOptionChange();
}

void UMMOUIAssistPanel::ShowKeyWarning(UBOOL bShow)
{
	if (bShow)
	{
		eventUCShowWarning(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Options.ShowWarning")));
	}
	else
	{
		eventUCShowWarning(TEXT(""));
	}
}
