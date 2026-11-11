#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOAchievementManager.h"
#include <time.h>
#include "MMOHandbookManager.h"
#include "MMOSkillManager.h"
#include "EquipDecomposeManager.h"
#include "MMOEquipManager.h"
#include "MMOHeavenManager.h"
#include "UITextParser.h"
#include "MMOMapDataManager.h"
#include "MMONavigationManager.h"

IMPLEMENT_CLASS(UMMOUIHandbookPanel)

void UMMOUIHandbookPanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Handbook);
	EventAdapter = new TGameEventAdapter<UMMOUIHandbookPanel>(this, evts);

	if (GFXPlayer)
	{
		GFXPlayer->AddPostAdvanceDelegate(FScriptDelegate(this, TEXT("PostAdvance")));
	}
}

void UMMOUIHandbookPanel::UnInit()
{
	Super::UnInit();

	if (_ShowLevel)
	{
		_ShowLevel->SetEnabled(FALSE);
	}

	if (GWorld && _ShowNpc && !_ShowNpc->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
	{
		GWorld->DestroyActor(_ShowNpc);
	}
	_ShowNpc = NULL;

	if (GWorld && _ShowPlayer && !_ShowPlayer->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
	{
		GWorld->DestroyActor(_ShowPlayer);
	}
	_ShowPlayer = NULL;

	if (GFXPlayer)
	{
		GFXPlayer->RemovePostAdvanceDelegate(FScriptDelegate(EC_EventParm));
	}
}

void UMMOUIHandbookPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if (v)
	{	
		if (_CurClass == 0)
		{
			_CurClass = HC_Monster;
			UpdateHandbookClass(HC_Monster);
			UpdateHandbookClassDefaultInfo(HC_Monster);
		} 
		else
		{
			UpdateHandbookClass(_CurClass);
			UpdateHandbookList(_CurType, _CurIndex);
			UpdateHandbookInfo(_CurType, _CurIndex);
		}

		_PlayerRotStates = TBS_None;
		_NpcRotStates = TBS_None;
	}
	else
	{
		MMOHandbookManager::getSingleton().StopHandbookSound(_CurType, _CurIndex);

		_CurClass = 0;
		_CurType = 0;
		_CurIndex = 0;

		if (_ShowLevel)
		{
			_ShowLevel->SetEnabled(FALSE);
		}
		
		if (GWorld && _ShowNpc && !_ShowNpc->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
		{
			GWorld->DestroyActor(_ShowNpc);
		}
		_ShowNpc = NULL;

		if (GWorld && _ShowPlayer && !_ShowPlayer->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
		{
			GWorld->DestroyActor(_ShowPlayer);
		}
		_ShowPlayer = NULL;
	}
}

void UMMOUIHandbookPanel::OnNativeGameEvent(struct MMOGameEvent* evt)
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
	case GameEvent_Handbook:
		{
			MMOGameEvent_Handbook* evtHandbook = (MMOGameEvent_Handbook*)evt;
			if (evtHandbook)
			{
				switch (evtHandbook->_EventType)
				{
				case MMOGameEvent_Handbook::EHET_UpdateHandbookInfo:
					{
						if (evtHandbook->_Value1 == _CurType)
						{
							UpdateHandbookList(_CurType, evtHandbook->_Value2);
							UpdateHandbookInfo(_CurType, evtHandbook->_Value2);
						}
					}
					break;
				}
			}
		}
		break;
	}
}

void UMMOUIHandbookPanel::CheckPendingRT(FLOAT deltaTime)
{
	if (_RTSet)
	{
		return;
	}

	if (_PendingRTCounter > 0)
	{
		-- _PendingRTCounter;
	}
	else if (_PendingRTCounter == 0 /*&& ShowLevel && ShowLevel->RenderTarget*/)
	{
		GFXPlayer->SetExternalTexture(TEXT("HandbookAvatar"), _ShowLevel->RenderTarget);
		_PendingRTCounter = -1;
		_RTSet = true;
	}
}

void UMMOUIHandbookPanel::UpdateHandbookClassDefaultInfo(UINT32 handbookClass)
{
	switch (handbookClass)
	{
	case HC_Monster:
		{
			UpdateHandbookList(htNpc_Normal);
			UpdateHandbookInfo(htNpc_Normal, 0);
		}
		break;
	case HC_Npc:
		{
			UpdateHandbookList(htNpc_Function);
			UpdateHandbookInfo(htNpc_Function, 0);
		}
		break;
	case HC_Map:
		{
			UpdateHandbookList(htMap_Wild);
			UpdateHandbookInfo(htMap_Wild, 0);
		}
		break;
	case HC_Item:
		{
			//UpdateHandbookList(htItem_Compose);
			//UpdateHandbookInfo(htItem_Compose, 0);
			UpdateHandbookList(htItem_Stone);
			UpdateHandbookInfo(htItem_Stone, 0);
		}
		break;
	case HC_Equip:
		{
			UpdateHandbookList(htEquip_Weapon);
			UpdateHandbookInfo(htEquip_Weapon, 0);
		}
		break;
	case HC_Rune:
		{
			UpdateHandbookList(htRune_Warrior);
			UpdateHandbookInfo(htRune_Warrior, 0);
		}
		break;
	case HC_Spirit:
		{
			UpdateHandbookList(htAir_Green);
			UpdateHandbookInfo(htAir_Green, 0);
		}
		break;
	}
}

void UMMOUIHandbookPanel::UpdateHandbookClass(UINT32 handbookClass)
{
	UGFxObject* objectArray = GFXPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	// 图鉴列表
	switch (handbookClass)
	{
	case HC_Monster:
		{
			int index = 0;
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNpc_Normal) > 0)
			{
				objectArray->SetElementString(index, TEXT("普通"));
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNpc_Elite) > 0)
			{
				objectArray->SetElementString(index, TEXT("精英"));
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNPc_Boss) > 0)
			{
				objectArray->SetElementString(index, TEXT("BOSS"));
				index ++;
			}
			eventCallUCUpdateHandbookType(objectArray);
		}
		break;
	case HC_Npc:
		{
			int index = 0;
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNpc_Function) > 0)
			{
				objectArray->SetElementString(index, TEXT("功能"));
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNpc_Story) > 0)
			{
				objectArray->SetElementString(index, TEXT("故事"));
				index ++;
			}
			eventCallUCUpdateHandbookType(objectArray);
		}
		break;
	case HC_Map:
		{
			int index = 0;
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htMap_Wild) > 0)
			{
				objectArray->SetElementString(index, TEXT("野外"));
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htMap_Dungeon) > 0)
			{
				objectArray->SetElementString(index, TEXT("副本"));
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htMap_Activity) > 0)
			{
				objectArray->SetElementString(index, TEXT("活动"));
				index ++;
			}
			eventCallUCUpdateHandbookType(objectArray);
		}
		break;
	case HC_Item:
		{
			int index = 0;
			/*
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htItem_Compose) > 0)
			{
				objectArray->SetElementString(index, TEXT("合成"));
				index ++;
			}
			*/
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htItem_Stone) > 0)
			{
				objectArray->SetElementString(index, TEXT("灵石"));
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htItem_Story) > 0)
			{
				objectArray->SetElementString(index, TEXT("故事"));
				index ++;
			}
			eventCallUCUpdateHandbookType(objectArray);
		}
		break;
	case HC_Equip:
		{
			int index = 0;
			/*
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htEquip_Set) > 0)
			{
				objectArray->SetElementString(index, TEXT("套装"));
				index ++;
			}
			*/
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htEquip_Weapon) > 0)
			{
				objectArray->SetElementString(index, TEXT("武器"));
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htEquip_Jewelry) > 0)
			{
				objectArray->SetElementString(index, TEXT("首饰"));
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htEquip_Armor) > 0)
			{
				objectArray->SetElementString(index, TEXT("防具"));
				index ++;
			}
			eventCallUCUpdateHandbookType(objectArray);
		}
		break;
	case HC_Rune:
		{
			int index = 0;
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Warrior) > 0)
			{
				objectArray->SetElementString(index, TEXT("战士铭文"));
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Mage) > 0)
			{
				objectArray->SetElementString(index, TEXT("法师铭文"));
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Rogue) > 0)
			{
				objectArray->SetElementString(index, TEXT("刺客铭文"));
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Archer) > 0)
			{
				objectArray->SetElementString(index, TEXT("弓手铭文"));
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Taoist) > 0)
			{
				objectArray->SetElementString(index, TEXT("道士铭文"));
				index ++;
			}
			eventCallUCUpdateHandbookType(objectArray);
		}
		break;
	case HC_Spirit:
		{
			int index = 0;
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htAir_Green) > 0)
			{
				objectArray->SetElementString(index, TEXT("绿色"));
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htAir_Blue) > 0)
			{
				objectArray->SetElementString(index, TEXT("蓝色"));
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htAir_Purple) > 0)
			{
				objectArray->SetElementString(index, TEXT("紫色"));
				index ++;
			}
			eventCallUCUpdateHandbookType(objectArray);
		}
		break;
	}
}

void UMMOUIHandbookPanel::SetLastHandbook(UINT32 handbookClass, UINT32 type, UINT32 index)
{
	_LastClass = handbookClass;
	_LastType = type;
	_LastIndex = index;
}

void UMMOUIHandbookPanel::UpdateHandbookList(UINT32 type, UINT32 defaultIndex, const FString& keyWord)
{
	UGFxObject* objectArray = GFXPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}
	
	HandbookDataList handbookDataList;
	if (keyWord != TEXT(""))
	{
		MMOHandbookData::getSingleton().GetHandbookDataListByKey(type, keyWord, handbookDataList);
		if (handbookDataList.empty())
		{
			FString text = TEXT("未搜索到相关内容");
			MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.Quest.LevelLimit"));
		}
	} 
	else
	{
		MMOHandbookData::getSingleton().GetHandbookDataList(type, handbookDataList);
	}

	if (handbookDataList.empty())
	{
		eventCallSetMainPageType(FALSE);
		eventCallUpdateHandbookList(objectArray, 0, 0, 0);
		return;
	}

	int rowCount = (int)(handbookDataList.size()) % MAX_HANDBOOK_COUNT_PER_ROW == 0 ? ((int)(handbookDataList.size()) / MAX_HANDBOOK_COUNT_PER_ROW) : ((int)(handbookDataList.size()) / MAX_HANDBOOK_COUNT_PER_ROW + 1);
	int colCount = MAX_HANDBOOK_COUNT_PER_ROW;

	if (type == htMap_Wild || type == htMap_Dungeon || type == htMap_Activity)
	{
		rowCount = handbookDataList.size();
		colCount = 1;
	}

	for (int i = 0; i < rowCount; ++ i)
	{
		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		if (object)
		{
			object->SetString(TEXT("label"), appItoa(i));
			object->SetInt(TEXT("index"), i);
			object->SetInt(TEXT("type"), 0);
			object->SetInt(TEXT("deepth"), 0);
			object->SetBool(TEXT("visible"), true);

			objectArray->SetElementObject(i, object);
		}
	}

	bool hasAchievement = false;
	HandbookDataList::iterator iter = handbookDataList.begin();
	INT index = 0;
	for (; iter != handbookDataList.end(); ++ iter, ++ index)
	{
		INT objectIndex = (INT)(index / MAX_HANDBOOK_COUNT_PER_ROW);
		if (type == htMap_Wild || type == htMap_Dungeon || type == htMap_Activity)
		{
			objectIndex = index;
		}

		UGFxObject* object = objectArray->GetElementObject(objectIndex);
		if (object)
		{
			UGFxObject* subObject = GFXPlayer->CreateObject(TEXT("ActionScript3.Handbook.HandbookData"));
			if (subObject)
			{
				HandbookData data = *iter;
				hasAchievement = data.AchievementId > 0;
				subObject->SetInt(TEXT("handbook_class"), _ConvertHandbookTypeToClass(data.Type));
				subObject->SetInt(TEXT("handbook_type"), data.Type);
				subObject->SetInt(TEXT("handbook_index"), data.Index);
				subObject->SetInt(TEXT("handbook_ui_type"), _ConvertHandbookTypeToUiType(data.Type));
				subObject->SetString(TEXT("handbook_name"), data.Name);
				
				if (type == htItem_Compose ||
					type == htItem_Stone ||
					type == htItem_Story ||
					type == htEquip_Set ||
					type == htEquip_Weapon ||
					type == htEquip_Jewelry ||
					type == htEquip_Armor)
				{
					if (data.TemplateId.Num() > 0)
					{
						UItem* item = MMOFacade::getSingleton().FindItemTemplate(data.TemplateId(0));
						if (item)
						{
							if (type == htEquip_Set)
							{
								subObject->SetString(TEXT("handbook_icon"), data.Icon);
							}
							else
							{
								FString iconName = FString("img://");
								iconName += MMOFacade::getSingleton().GetItemIconNamePersonalized( item );
								subObject->SetString(TEXT("handbook_icon"), iconName);
							}

							if (MMOFacade::getSingleton().GetMainActor())
							{
								EPlayerClass playerClass = (EPlayerClass)(MMOFacade::getSingleton().GetMainActor()->GetClassId());
								EGenderType playerGender = (EGenderType)(MMOFacade::getSingleton().GetMainActor()->GetSex());
								subObject->SetBool(TEXT("handbook_invalid"), 
									!MMO::CanBeUsedByPlayerClass(playerClass, item->RequireClass) || !MMO::CanBeUsedByPlayerGender(playerGender, item->RequireGender));
							}
						}
					}
				}
				else if (type == htRune_Warrior ||
					type == htRune_Mage ||
					type == htRune_Rogue ||
					type == htRune_Rogue ||
					type == htRune_Taoist)
				{
					const WeaponSkillCfg* rune = MMOEquipManager::getSingleton().GetWeaponSkillCfg(data.TemplateId(0));
					if (rune)
					{
						UINT key = MMOSkillManager::CombinateAbilityID(rune->_SkillID, 0, rune->_SkillRuneID);
						UMMOAbility* skill = MMOSkillManager::getSingleton().FindAbilityTemplate(key);
						if (skill)
						{
							FString skillIcon = FString(TEXT("img://")) + skill->GetAbilityIconName();
							subObject->SetString(TEXT("handbook_icon"), skillIcon);
							subObject->SetBool(TEXT("handbook_invalid"), FALSE);
						}
					}
				}
				else
				{
					subObject->SetString(TEXT("handbook_icon"), data.Icon);
					subObject->SetBool(TEXT("handbook_invalid"), FALSE);
				}

				subObject->SetBool(TEXT("handbook_obtain"), MMOHandbookData::getSingleton().IsHandbookObtain(data.Type, data.Index));

				INT itemIndex = index % MAX_HANDBOOK_COUNT_PER_ROW;
				if (type == htMap_Wild || type == htMap_Dungeon || type == htMap_Activity)
				{
					itemIndex = 0;
				}
				object->SetObject(*FString::Printf(TEXT("handbook_item_data%d"), itemIndex), subObject);
			}
		}
	}

	eventCallSetMainPageType(hasAchievement);

	INT typeIndex = _ConvertHandbookTypeToSelectIndex(type);
	INT lineIndex = (INT)(defaultIndex / MAX_HANDBOOK_COUNT_PER_ROW);
	INT rowIndex = (INT)(defaultIndex % MAX_HANDBOOK_COUNT_PER_ROW);
	if (type == htMap_Wild || type == htMap_Dungeon || type == htMap_Activity)
	{
		lineIndex = defaultIndex;
		rowIndex = 0;
	}
	eventCallUpdateHandbookList(objectArray, typeIndex, lineIndex, rowIndex);

	UINT32 cur = MMOHandbookData::getSingleton().GetCurHandbookCount(type);
	UINT32 max = MMOHandbookData::getSingleton().GetMaxHandbookCount(type);
	eventCallUpdateHandbookProgress(cur, max);
}

void UMMOUIHandbookPanel::UpdateHandbookAchievementInfo(UINT32 type)
{
}

void UMMOUIHandbookPanel::UpdateHandbookInfo(UINT32 type, UINT32 index)
{
	if (GWorld && _ShowNpc && !_ShowNpc->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
	{
		GWorld->DestroyActor(_ShowNpc);
	}
	_ShowNpc = NULL;

	if (GWorld && _ShowPlayer && !_ShowPlayer->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
	{
		GWorld->DestroyActor(_ShowPlayer);
	}
	_ShowPlayer = NULL;

	switch (type)
	{
	case htNpc_Normal:
	case htNpc_Elite:
	case htNPc_Boss:
		_UpdateMonsterInfo(type, index);
		break;

	case htNpc_Function:
	case htNpc_Story:
		_UpdateNpcInfo(type, index);
		break;

	case htMap_Wild:
	case htMap_Dungeon:
	case htMap_Activity:
		_UpdateMapInfo(type, index);
		break;

	case htItem_Compose:
		_UpdateComposeInfo(type, index);
		break;

	case htItem_Stone:
		_UpdateStoneInfo(type, index);
		break;

	case htItem_Story:
		_UpdateStoryInfo(type, index);
		break;

	case htEquip_Set:
		_UpdateSuitInfo(type, index);
		break;

	case htEquip_Weapon:
	case htEquip_Jewelry:
	case htEquip_Armor:
		_UpdateEquipInfo(type, index);
		break;

	case htRune_Warrior:
	case htRune_Mage:
	case htRune_Rogue:
	case htRune_Archer:
	case htRune_Taoist:
		_UpdateRuneInfo(type, index);
		break;

	case htAir_Green:
	case htAir_Blue:
	case htAir_Purple:
		_UpdateSpiritInfo(type, index);
		break;
	}

	// #16276
	//MMOHandbookManager::getSingleton().PlayHandbookSound(type, index);
}

UINT32 UMMOUIHandbookPanel::_ConvertHandbookTypeToClass(UINT32 type)
{
	UINT32 handbookClass = HC_Invalid;
	switch (type)
	{
	case htNpc_Normal:
	case htNpc_Elite:
	case htNPc_Boss:
		handbookClass = HC_Monster;
		break;

	case htNpc_Function:
	case htNpc_Story:
		handbookClass = HC_Npc;
		break;

	case htMap_Wild:
	case htMap_Dungeon:
	case htMap_Activity:
		handbookClass = HC_Map;
		break;

	case htItem_Compose:
	case htItem_Stone:
	case htItem_Story:
		handbookClass = HC_Item;
		break;

	case htEquip_Set:
	case htEquip_Weapon:
	case htEquip_Jewelry:
	case htEquip_Armor:
		handbookClass = HC_Equip;
		break;

	case htRune_Warrior:
	case htRune_Mage:
	case htRune_Rogue:
	case htRune_Archer:
	case htRune_Taoist:
		handbookClass = HC_Rune;
		break;

	case htAir_Green:
	case htAir_Blue:
	case htAir_Purple:
		handbookClass = HC_Spirit;
		break;
	}

	return handbookClass;
}

UINT32 UMMOUIHandbookPanel::_ConvertHandbookTypeToUiType(UINT32 type)
{
	UINT32 uiType = HUT_Invalid;
	switch (type)
	{
	case htNpc_Normal:
	case htNpc_Elite:
	case htNPc_Boss:
		uiType = HUT_Monster;
		break;

	case htNpc_Function:
	case htNpc_Story:
		uiType = HUT_Npc;
		break;

	case htMap_Wild:
	case htMap_Dungeon:
	case htMap_Activity:
		uiType = HUT_Map;
		break;

	case htItem_Compose:
		uiType = HUT_Compose;
		break;

	case htItem_Stone:
		uiType = HUT_Stone;
		break;

	case htItem_Story:
		uiType = HUT_Story;
		break;

	case htEquip_Set:
		uiType = HUT_Suit;
		break;

	case htEquip_Weapon:
	case htEquip_Jewelry:
	case htEquip_Armor:
		uiType = HUT_Equip;
		break;

	case htRune_Warrior:
	case htRune_Mage:
	case htRune_Rogue:
	case htRune_Archer:
	case htRune_Taoist:
		uiType = HUT_Rune;
		break;

	case htAir_Green:
	case htAir_Blue:
	case htAir_Purple:
		uiType = HUT_Spirit;
		break;
	}

	return uiType;
}

UINT32 UMMOUIHandbookPanel::_ConvertHandbookTypeToSelectIndex(UINT32 type)
{
	UINT32 index = 0;
	switch (type)
	{
	case htNpc_Normal:
	case htNpc_Elite:
	case htNPc_Boss:
		{
			index = 0;
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNpc_Normal) > 0)
			{
				if (type == htNpc_Normal)
				{
					return index;
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNpc_Elite) > 0)
			{
				if (type == htNpc_Elite)
				{
					return index;
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNPc_Boss) > 0)
			{
				if (type == htNPc_Boss)
				{
					return index;
				}
				index ++;
			}
		}
		break;

	case htNpc_Function:
	case htNpc_Story:
		{
			index = 0;
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNpc_Function) > 0)
			{
				if (type == htNpc_Function)
				{
					return index;
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNpc_Story) > 0)
			{
				if (type == htNpc_Story)
				{
					return index;
				}
				index ++;
			}
		}
		break;

	case htMap_Wild:
	case htMap_Dungeon:
	case htMap_Activity:
		{
			index = 0;
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htMap_Wild) > 0)
			{
				if (type == htMap_Wild)
				{
					return index;
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htMap_Dungeon) > 0)
			{
				if (type == htMap_Dungeon)
				{
					return index;
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htMap_Activity) > 0)
			{
				if (type == htMap_Activity)
				{
					return index;
				}
				index ++;
			}
		}
		break;

	case htItem_Compose:
	case htItem_Stone:
	case htItem_Story:
		{
			index = 0;
			/*
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htItem_Compose) > 0)
			{
				if (type == htItem_Compose)
				{
					return index;
				}
				index ++;
			}
			*/
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htItem_Stone) > 0)
			{
				if (type == htItem_Stone)
				{
					return index;
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htItem_Story) > 0)
			{
				if (type == htItem_Story)
				{
					return index;
				}
				index ++;
			}
		}
		break;

	case htEquip_Set:
	case htEquip_Weapon:
	case htEquip_Jewelry:
	case htEquip_Armor:
		{
			index = 0;
			/*
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htEquip_Set) > 0)
			{
				if (type == htEquip_Set)
				{
					return index;
				}
				index ++;
			}
			*/
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htEquip_Weapon) > 0)
			{
				if (type == htEquip_Weapon)
				{
					return index;
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htEquip_Jewelry) > 0)
			{
				if (type == htEquip_Jewelry)
				{
					return index;
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htEquip_Armor) > 0)
			{
				if (type == htEquip_Armor)
				{
					return index;
				}
				index ++;
			}
		}
		break;

	case htRune_Warrior:
	case htRune_Mage:
	case htRune_Rogue:
	case htRune_Archer:
	case htRune_Taoist:
		{
			index = 0;
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Warrior) > 0)
			{
				if (type == htRune_Warrior)
				{
					return index;
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Mage) > 0)
			{
				if (type == htRune_Mage)
				{
					return index;
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Rogue) > 0)
			{
				if (type == htRune_Rogue)
				{
					return index;
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Archer) > 0)
			{
				if (type == htRune_Archer)
				{
					return index;
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Taoist) > 0)
			{
				if (type == htRune_Taoist)
				{
					return index;
				}
				index ++;
			}
		}
		break;

	case htAir_Green:
	case htAir_Blue:
	case htAir_Purple:
		{
			index = 0;
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htAir_Green) > 0)
			{
				if (type == htAir_Green)
				{
					return index;
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htAir_Blue) > 0)
			{
				if (type == htAir_Blue)
				{
					return index;
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htAir_Purple) > 0)
			{
				if (type == htAir_Purple)
				{
					return index;
				}
				index ++;
			}
		}
		break;
	}

	return index;
}

void UMMOUIHandbookPanel::SetShowActor(AMMOActor* actor, FLOAT scale)
{
	static const FLOAT StandardHeight = 100.f;
	static const FLOAT StandardRadius = 100.f;
	if (!actor)
	{
		return;
	}
	
	_ShowLevel = GFXPlayer->eventGetShowLevel("PawnShowLevel_Handbook");
	if (!_RTSet && _ShowLevel && _ShowLevel->RenderTarget)
	{
		// set the texture 1 frame later
		_PendingRTCounter = 1;	
	}

	if (_ShowLevel)
	{
		_ShowLevel->SetEnabled(TRUE);
		FName tag("C_01", FNAME_Find);
		if (actor->UseingAvatar)
		{
			tag = actor->UseingAvatar->PlacementNodeTag;
		}

		if (actor->LightEnvironment)
		{
			actor->LightEnvironment->SetEnabled(FALSE);
		}

		// scale to the proper size
		UCylinderComponent* CylComp = Cast<UCylinderComponent>(actor->CollisionComponent);
		if (CylComp)
		{
			FLOAT heightRatio = StandardHeight / CylComp->CollisionHeight * scale;
			FLOAT weightRatio = StandardRadius / CylComp->CollisionRadius * scale;

			debugf( TEXT("Height: %.2f Radius: %.2f ( %.2f, %.2f )"), 
				CylComp->CollisionHeight, CylComp->CollisionRadius,
				heightRatio, weightRatio );

			actor->SetDrawScale(Min(heightRatio, weightRatio));
			AArmorActor* weapon1 = Cast<AArmorActor>(actor->GetEquip(EBMT_Weapon));
			if (weapon1)
			{
				weapon1->SetDrawScale(Min(heightRatio, weightRatio));
			}

			AArmorActor* weapon2 = Cast<AArmorActor>(actor->GetEquip(EBMT_Weapon2));
			if (weapon2)
			{
				weapon2->SetDrawScale(Min(heightRatio, weightRatio));
			}
		}

		_ShowLevel->SetShowActor(actor, tag);
	}
}

void UMMOUIHandbookPanel::RotatePlayer(int dir)
{
	if (!_ShowPlayer)
	{
		return;
	}

	FRotator rot;
	if (dir == TBS_Left)
	{
		rot = _ShowPlayer->Rotation;
		rot.Yaw -= 200;
		_ShowPlayer->SetRotation(rot);
	}
	else if (dir == TBS_Right)
	{
		rot = _ShowPlayer->Rotation;
		rot.Yaw += 200;
		_ShowPlayer->SetRotation(rot);
	}
}

void UMMOUIHandbookPanel::RotateNpc(int dir)
{
	if (!_ShowNpc)
	{
		return;
	}

	FRotator rot;
	if (dir == TBS_Left)
	{
		rot = _ShowNpc->Rotation;
		rot.Yaw -= 200;
		_ShowNpc->SetRotation(rot);
	}
	else if (dir == TBS_Right)
	{
		rot = _ShowNpc->Rotation;
		rot.Yaw += 200;
		_ShowNpc->SetRotation(rot);
	}
}

void UMMOUIHandbookPanel::_UpdateMonsterInfo(UINT32 type, UINT32 index)
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Handbook.HandbookData"));
	if (!object)
	{
		return;
	}

	HandbookData data;
	if (!MMOHandbookData::getSingleton().GetHandbookData(type, index, data))
	{
		return;
	}

	object->SetInt(TEXT("handbook_type"), data.Type);
	object->SetInt(TEXT("handbook_index"), data.Index);
	object->SetInt(TEXT("handbook_ui_type"), _ConvertHandbookTypeToUiType(data.Type));
	object->SetString(TEXT("handbook_name"), data.Name);
	object->SetString(TEXT("handbook_text0"), data.Text0);
	object->SetString(TEXT("handbook_text1"), data.Text1);
	object->SetString(TEXT("handbook_text2"), data.Text2);
	object->SetString(TEXT("handbook_sound"), data.Sound);
	
	// npc
	if (data.TemplateId.Num() > 0)
	{
		UNPCTemplate* npcTemplate = MMOFacade::getSingleton().FindNPCTemplate(data.TemplateId(0));
		if (npcTemplate)
		{
			ANPC* npc = Cast<ANPC>(GWorld->SpawnActor(ANPC::StaticClass(), NAME_None, FVector(0.0, 0.0, 0.0), FRotator(1.0, 0.0, 0.0), NULL, TRUE));
			if (npc)
			{
				npc->InitNpcByTemplate(npcTemplate);
				npc->Mesh->bUseFullLevelMipmaps = TRUE;

				_ShowNpc = eventCloneNPC(npc);
				SetShowActor(_ShowNpc, data.Scale);
			}
		}
	}

	// 技能
	if (data.Data0.Num() > 0)
	{
		UGFxObject* array0 = GFXPlayer->CreateArray();
		if (array0)
		{
			int index = 0;
			for (int i = 0; i < data.Data0.Num(); ++ i)
			{
				UINT key = MMOSkillManager::CombinateAbilityID(data.Data0(i), 1);
				UMMOAbility* skill = MMOSkillManager::getSingleton().FindAbilityTemplate(key);
				if (!skill)
				{
					continue;
				}

				UGFxObject* element = GFXPlayer->CreateObject(TEXT("Object"));
				if (element)
				{
					INT uniqueId = MMOSkillManager::CombinateAbilityID(skill->GetTemplateID(), 
						skill->SpellCommonAttribute.Level, 
						skill->GetRuneSlot());
					element->SetInt(TEXT("skill_id"), uniqueId);
					FString skillIcon = FString(TEXT("img://")) + skill->GetAbilityIconName();
					element->SetString(TEXT("skill_icon"), skillIcon);

					array0->SetElementObject(index, element);
					index ++;
				}
			}
			object->SetObject(TEXT("handbook_data0"), array0);
		}
	}

	// 掉落
	if (data.Data1.Num() > 0)
	{
		UGFxObject* array1 = GFXPlayer->CreateArray();
		if (array1)
		{
			int index = 0;
			for (int i = 0; i < data.Data1.Num(); ++ i)
			{
				if (data.Data1(i) == 0)
				{
					continue;
				}

				UItem* item = MMOFacade::getSingleton().FindItemTemplate(data.Data1(i));
				if (!item)
				{
					continue;
				}

				if (_UsableOnly)
				{
					if (MMOFacade::getSingleton().GetMainPlayer() && MMOFacade::getSingleton().GetMainPlayer()->ActorAttributes)
					{
						EPlayerClass playerClass = (EPlayerClass)MMOFacade::getSingleton().GetMainPlayer()->GetClassId();
						EGenderType playerGender = (EGenderType)MMOFacade::getSingleton().GetMainPlayer()->ActorAttributes->GenderType;
						if (MMO::CanBeUsedByPlayerClass(playerClass, item->RequireClass) == FALSE ||
							MMO::CanBeUsedByPlayerGender(playerGender, item->RequireGender) == FALSE)
						{
							continue;
						}
					}
				}

				UGFxObject* element = GFXPlayer->CreateObject(TEXT("Object"));
				if (element)
				{
					element->SetInt(TEXT("item_id"), item->GetTemplateID());
					element->SetString(TEXT("item_icon"), MMOFacade::getSingleton().GetItemIconNamePersonalized( item ));

					array1->SetElementObject(index, element);
					index ++;
				}
			}
			object->SetObject(TEXT("handbook_data1"), array1);
		}
	}

	eventCallUpdateHandbookInfo(object);

	_CurType = type;
	_CurIndex = index;
}

void UMMOUIHandbookPanel::_UpdateNpcInfo(UINT32 type, UINT32 index)
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Handbook.HandbookData"));
	if (!object)
	{
		return;
	}
	
	HandbookData data;
	if (!MMOHandbookData::getSingleton().GetHandbookData(type, index, data))
	{
		return;
	}

	object->SetInt(TEXT("handbook_type"), data.Type);
	object->SetInt(TEXT("handbook_index"), data.Index);
	object->SetInt(TEXT("handbook_ui_type"), _ConvertHandbookTypeToUiType(data.Type));
	object->SetString(TEXT("handbook_name"), data.Name);
	object->SetString(TEXT("handbook_text0"), data.Text0);
	object->SetString(TEXT("handbook_text1"), data.Text1);
	object->SetString(TEXT("handbook_text2"), data.Text2);
	object->SetString(TEXT("handbook_sound"), data.Sound);
	
	// npc
	if (data.TemplateId.Num() > 0)
	{
		UNPCTemplate* npcTemplate = MMOFacade::getSingleton().FindNPCTemplate(data.TemplateId(0));
		if (npcTemplate)
		{
			ANPC* npc = Cast<ANPC>(GWorld->SpawnActor(ANPC::StaticClass(), NAME_None, FVector(0.0, 0.0, 0.0), FRotator(1.0, 0.0, 0.0), NULL, TRUE));
			if (npc)
			{
				npc->InitNpcByTemplate(npcTemplate);
				npc->Mesh->bUseFullLevelMipmaps = TRUE;

				_ShowNpc = eventCloneNPC(npc);
				SetShowActor(_ShowNpc, data.Scale);
			}
		}
	}

	eventCallUpdateHandbookInfo(object);

	_CurType = type;
	_CurIndex = index;
}

void UMMOUIHandbookPanel::_UpdateMapInfo(UINT32 type, UINT32 index)
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Handbook.HandbookData"));
	if (!object)
	{
		return;
	}

	HandbookData data;
	if (!MMOHandbookData::getSingleton().GetHandbookData(type, index, data))
	{
		return;
	}

	object->SetInt(TEXT("handbook_type"), data.Type);
	object->SetInt(TEXT("handbook_index"), data.Index);
	object->SetInt(TEXT("handbook_ui_type"), _ConvertHandbookTypeToUiType(data.Type));
	object->SetString(TEXT("handbook_name"), data.Name);
	object->SetString(TEXT("handbook_text0"), data.Text0);
	object->SetString(TEXT("handbook_text1"), data.Text1);
	object->SetString(TEXT("handbook_text2"), data.Text2);
	object->SetString(TEXT("handbook_sound"), data.Sound);

	if (data.TemplateId.Num() > 0)
	{
		object->SetInt(TEXT("handbook_temp"), data.TemplateId(0));
	}

	// 特产
	if (data.Data0.Num() > 0)
	{
		UGFxObject* array0 = GFXPlayer->CreateArray();
		if (array0)
		{
			int index = 0;
			for (int i = 0; i < data.Data0.Num(); ++ i)
			{
				UItem* item = MMOFacade::getSingleton().FindItemTemplate(data.Data0(i));
				if (!item)
				{
					continue;
				}

				UGFxObject* element = GFXPlayer->CreateObject(TEXT("Object"));
				if (element)
				{
					element->SetInt(TEXT("item_id"), item->GetTemplateID());
					element->SetString(TEXT("item_icon"), MMOFacade::getSingleton().GetItemIconNamePersonalized( item ));

					array0->SetElementObject(index, element);
					index ++;
				}
			}
			object->SetObject(TEXT("handbook_data0"), array0);
		}
	}

	// BOSS
	if (data.Data1.Num() > 0)
	{
		UGFxObject* array1 = GFXPlayer->CreateArray();
		if (array1)
		{
			int index = 0;
			for (int i = 0; i < data.Data1.Num(); ++ i)
			{
				UNPCTemplate* npc = MMOFacade::getSingleton().FindNPCTemplate(data.Data1(i));
				if (!npc)
				{
					continue;
				}

				UGFxObject* element = GFXPlayer->CreateObject(TEXT("Object"));
				if (element)
				{
					element->SetString(TEXT("npc_name"), npc->GetDisplayName());

					FString bossIcon = TEXT("img://");
					bossIcon += npc->GetIconName();
					UINT32 handbookType = 0;
					UINT32 handbookIndex = 0;
					if (MMOHandbookData::getSingleton().GetMonsterHandbookInfoByName(npc->GetDisplayName(), handbookType, handbookIndex))
					{
						HandbookData bossData;
						if (MMOHandbookData::getSingleton().GetHandbookData(handbookType, handbookIndex, bossData))
						{
							bossIcon = bossData.Icon;
						}
					}
					element->SetString(TEXT("npc_icon"), bossIcon);

					array1->SetElementObject(index, element);
					index ++;
				}
			}
			object->SetObject(TEXT("handbook_data1"), array1);
		}
	}

	eventCallUpdateHandbookInfo(object);

	_CurType = type;
	_CurIndex = index;
}

void UMMOUIHandbookPanel::_UpdateStoryInfo(UINT32 type, UINT32 index)
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Handbook.HandbookData"));
	if (!object)
	{
		return;
	}

	HandbookData data;
	if (!MMOHandbookData::getSingleton().GetHandbookData(type, index, data))
	{
		return;
	}

	object->SetInt(TEXT("handbook_type"), data.Type);
	object->SetInt(TEXT("handbook_index"), data.Index);
	object->SetInt(TEXT("handbook_ui_type"), _ConvertHandbookTypeToUiType(data.Type));
	object->SetString(TEXT("handbook_name"), data.Name);
	object->SetString(TEXT("handbook_text0"), data.Text0);
	object->SetString(TEXT("handbook_text1"), data.Text1);
	object->SetString(TEXT("handbook_text2"), data.Text2);
	object->SetString(TEXT("handbook_sound"), data.Sound);

	eventCallUpdateHandbookInfo(object);

	_CurType = type;
	_CurIndex = index;
}

void UMMOUIHandbookPanel::_UpdateComposeInfo(UINT32 type, UINT32 index)
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Handbook.HandbookData"));
	if (!object)
	{
		return;
	}
	
	HandbookData data;
	if (!MMOHandbookData::getSingleton().GetHandbookData(type, index, data))
	{
		return;
	}


	if (data.TemplateId.Num() <= 0)
	{
		return;
	}

	object->SetInt(TEXT("handbook_type"), data.Type);
	object->SetInt(TEXT("handbook_index"), data.Index);
	object->SetInt(TEXT("handbook_ui_type"), _ConvertHandbookTypeToUiType(data.Type));
	object->SetString(TEXT("handbook_name"), data.Name);
	object->SetString(TEXT("handbook_sound"), data.Sound);

	// 合成: 0: 合成的武器，1~n: 合成材料
	UGFxObject* array0 = GFXPlayer->CreateArray();
	if (array0)
	{
		EquipComposeTemplate& compose = EquipDecomposeManager::getSingleton().GetComposeTemplateById(data.TemplateId(0));

		int index = 0;
		// 装备
		UItem* item = MMOFacade::getSingleton().FindItemTemplate(compose.equipId);
		if (item)
		{
			UGFxObject* element = GFXPlayer->CreateObject(TEXT("Object"));
			if (element)
			{
				element->SetInt(TEXT("item_id"), item->GetTemplateID());
				element->SetString(TEXT("item_icon"), MMOFacade::getSingleton().GetItemIconNamePersonalized( item ));
				element->SetString(TEXT("item_name"), item->GetDisplayName());

				array0->SetElementObject(index, element);
				index ++;
			}
		}

		// 材料
		for (int i = 0; i < COMPOSE_CONSUME_COUNT; ++ i)
		{
			if (compose.matId[i] == 0)
			{
				continue;
			}

			UItem* item = MMOFacade::getSingleton().FindItemTemplate(compose.matId[i]);
			if (item)
			{
				UGFxObject* element = GFXPlayer->CreateObject(TEXT("Object"));
				if (element)
				{
					element->SetInt(TEXT("item_id"), item->GetTemplateID());
					element->SetString(TEXT("item_icon"), MMOFacade::getSingleton().GetItemIconNamePersonalized( item ));

					INT curCount = MMOFacade::getSingleton().GetItemCount(compose.matId[i]);
					INT maxCount = compose.matCnt[i];
					if(maxCount > curCount)
					{
						element->SetString(TEXT("item_info"), *FString::Printf(TEXT("<font color='0xFF0000'>%d/%d</font>"), curCount, maxCount));
					}
					else
					{
						element->SetString(TEXT("item_info"), *FString::Printf(TEXT("<font color='0x00FF00'>%d/%d</font>"), curCount, maxCount));
					}

					array0->SetElementObject(index, element);
					index ++;
				}
			}
		}

		object->SetString(TEXT("handbook_text0"), *appItoa(compose.cost));

		object->SetObject(TEXT("handbook_data0"), array0);
	}

	eventCallUpdateHandbookInfo(object);

	_CurType = type;
	_CurIndex = index;
}

void UMMOUIHandbookPanel::_UpdateStoneInfo(UINT32 type, UINT32 index)
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Handbook.HandbookData"));
	if (!object)
	{
		return;
	}

	HandbookData data;
	if (!MMOHandbookData::getSingleton().GetHandbookData(type, index, data))
	{
		return;
	}

	object->SetInt(TEXT("handbook_type"), data.Type);
	object->SetInt(TEXT("handbook_index"), data.Index);
	object->SetInt(TEXT("handbook_ui_type"), _ConvertHandbookTypeToUiType(data.Type));
	object->SetString(TEXT("handbook_name"), data.Name);
	object->SetString(TEXT("handbook_text0"), data.Text0);
	object->SetString(TEXT("handbook_text1"), data.Text1);
	object->SetString(TEXT("handbook_text2"), data.Text2);
	object->SetString(TEXT("handbook_sound"), data.Sound);

	// 灵石
	if (data.TemplateId.Num() > 0)
	{
		UGFxObject* array0 = GFXPlayer->CreateArray();
		if (array0)
		{
			int index = 0;
			for (int i = 0; i < data.TemplateId.Num(); ++ i)
			{
				const SoulInfo* soulInfo = MMOEquipManager::getSingleton().FindSoulInfoByItemTemplateID(data.TemplateId(i));
				if (!soulInfo)
				{
					continue;
				}

				UItem* item = MMOFacade::getSingleton().FindItemTemplate(soulInfo->_ItemID);
				if (!item)
				{
					continue;
				}

				UGFxObject* element = GFXPlayer->CreateObject(TEXT("Object"));
				if (element)
				{
					element->SetInt(TEXT("stone_id"), soulInfo->_ItemID);
					element->SetString(TEXT("stone_icon"), MMOFacade::getSingleton().GetItemIconNamePersonalized( item ));

					const ItemRandomAttr* attr = MMOFacade::getSingleton().GetRandomAttributById(soulInfo->_AttrID);
					if (attr)
					{
						element->SetString(TEXT("stone_effect"), attr->strText);
					}

					array0->SetElementObject(index, element);
					index ++;
				}
			}
			object->SetObject(TEXT("handbook_data0"), array0);
		}
	}

	// 等级列表
	UGFxObject* array1 = GFXPlayer->CreateArray();
	if (array1)
	{
		int index = 0;
		for (int i = 0; i < data.TemplateId.Num(); ++ i)
		{
			const SoulInfo* soulInfo = MMOEquipManager::getSingleton().FindSoulInfoByItemTemplateID(data.TemplateId(i));
			if (!soulInfo)
			{
				continue;
			}

			array1->SetElementString(index, *appItoa(soulInfo->_SoulLevel));
			index ++;
		}

		object->SetObject(TEXT("handbook_data1"), array1);
	}

	eventCallUpdateHandbookInfo(object);

	_CurType = type;
	_CurIndex = index;
}

void UMMOUIHandbookPanel::_UpdateSuitInfo(UINT32 type, UINT32 index)
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Handbook.HandbookData"));
	if (!object)
	{
		return;
	}
	
	HandbookData data;
	if (!MMOHandbookData::getSingleton().GetHandbookData(type, index, data))
	{
		return;
	}

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	object->SetInt(TEXT("handbook_type"), data.Type);
	object->SetInt(TEXT("handbook_index"), data.Index);
	object->SetInt(TEXT("handbook_ui_type"), _ConvertHandbookTypeToUiType(data.Type));
	object->SetString(TEXT("handbook_name"), data.Name);
	object->SetString(TEXT("handbook_sound"), data.Sound);

	if (!_ShowPlayer)
	{
		eventClonePlayer(mainPlayer);
		if(_ShowPlayer)
		{
			_ShowPlayer->PawnAnimInfo = mainPlayer->PawnAnimInfo;
		}
	}

	if (data.TemplateId.Num() > 0)
	{
		UGFxObject* array0 = GFXPlayer->CreateArray();
		if (array0)
		{
			int index = 0;
			for (int i = 0; i < data.TemplateId.Num(); ++ i)
			{
				UItem* item = MMOFacade::getSingleton().FindItemTemplate(data.TemplateId(i));
				if (!item)
				{
					continue;
				}

				UEquip* equip = (UEquip*)(item);
				if (!equip)
				{
					continue;
				}

				if (_ShowPlayer)
				{
					// 通过第一件装备判断职业和性别
					if (i == 0)
					{
						if (MMO::CanBeUsedByPlayerClass(EPCT_PlayerClassA, item->RequireClass))
						{
							_ShowPlayer->NativeSetMesh(EPCT_PlayerClassA, mainPlayer->GetSex());
						} 
						else if (MMO::CanBeUsedByPlayerClass(EPCT_PlayerClassB, item->RequireClass))
						{
							_ShowPlayer->NativeSetMesh(EPCT_PlayerClassB, mainPlayer->GetSex());
						}
						else if (MMO::CanBeUsedByPlayerClass(EPCT_PlayerClassC, item->RequireClass))
						{
							_ShowPlayer->NativeSetMesh(EPCT_PlayerClassC, mainPlayer->GetSex());
						}
						else if (MMO::CanBeUsedByPlayerClass(EPCT_PlayerClassD, item->RequireClass))
						{
							_ShowPlayer->NativeSetMesh(EPCT_PlayerClassD, mainPlayer->GetSex());
						}
						else if (MMO::CanBeUsedByPlayerClass(EPCT_PlayerClassG, item->RequireClass))
							{
								_ShowPlayer->NativeSetMesh(EPCT_PlayerClassG, mainPlayer->GetSex());
							}
							else if (MMO::CanBeUsedByPlayerClass(EPCT_PlayerClassH, item->RequireClass))
							{
								_ShowPlayer->NativeSetMesh(EPCT_PlayerClassH, mainPlayer->GetSex());
							}
							else if (MMO::CanBeUsedByPlayerClass(EPCT_PlayerClassE, item->RequireClass))
						{
							_ShowPlayer->NativeSetMesh(EPCT_PlayerClassE, mainPlayer->GetSex());
						}
					}
					if (item->ItemType == TEXT("ITEMTYPE_EQUIP") &&
						_ShowPlayer->ItemManager &&
						equip->GetEquipAppearenceTemplate())
					{
						_ShowPlayer->ItemManager->RequestTakeOn(-1, equip->GetEquipAppearenceTemplate()->EquipType, true, item, false);
						_ShowPlayer->ItemManager->UpdateOwnerData();
					}
				}

				UGFxObject* element = GFXPlayer->CreateObject(TEXT("Object"));
				if (element)
				{
					element->SetInt(TEXT("item_id"), item->GetTemplateID());
					element->SetString(TEXT("item_icon"), MMOFacade::getSingleton().GetItemIconNamePersonalized( item ));

					array0->SetElementObject(index, element);
					index ++;
				}
			}
			object->SetObject(TEXT("handbook_data0"), array0);
		}
	}

	if (_ShowPlayer)
	{
		SetShowActor(_ShowPlayer, data.Scale);
	}

	eventCallUpdateHandbookInfo(object);

	_CurType = type;
	_CurIndex = index;
}

void UMMOUIHandbookPanel::_UpdateEquipInfo(UINT32 type, UINT32 index)
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Handbook.HandbookData"));
	if (!object)
	{
		return;
	}

	HandbookData data;
	if (!MMOHandbookData::getSingleton().GetHandbookData(type, index, data))
	{
		return;
	}

	if (data.TemplateId.Num() <= 0)
	{
		return;
	}

	UItem* item = MMOFacade::getSingleton().FindItemTemplate(data.TemplateId(0));
	if (!item)
	{
		return;
	}

	UEquip* equip = Cast<UEquip>(item);
	if (!equip)
	{
		return;
	}

	object->SetInt(TEXT("handbook_type"), data.Type);
	object->SetInt(TEXT("handbook_index"), data.Index);
	object->SetInt(TEXT("handbook_ui_type"), _ConvertHandbookTypeToUiType(data.Type));
	object->SetString(TEXT("handbook_name"), data.Name);
	object->SetString(TEXT("handbook_text0"), data.Text0);
	object->SetString(TEXT("handbook_text1"), data.Text1);
	object->SetString(TEXT("handbook_text2"), data.Text2);
	object->SetString(TEXT("handbook_sound"), data.Sound);

	UGFxObject* array0 = GFXPlayer->CreateArray();
	if (array0)
	{
		// 基本属性
		UGFxObject* element = GFXPlayer->CreateObject(TEXT("Object"));
		if (element)
		{
			element->SetInt(TEXT("equip_id"), item->GetTemplateID());
			element->SetString(TEXT("equip_icon"), MMOFacade::getSingleton().GetItemIconNamePersonalized( item ));
			element->SetString(TEXT("equip_part"), UITextParser::HTMLFormatItemType(item));

			FString classText = TEXT("");
			if (MMO::IsForAllPlayerClass(item->RequireClass))
			{
				classText = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.School.SchoolNameAll"));
			}
			else
			{
				classText = UITextParser::HTMLFormatItemRequireProfession(item);
			}
			
			element->SetString(TEXT("equip_class"), classText);
			element->SetString(TEXT("equip_level"), UITextParser::HTMLFormatItemRequireLevel(item));

			INT curSlotCnt = 0;
			/*
			for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i )
			{
				UBOOL isSkillSlotActive = FALSE;
				UMMOAbility* weaponSkill = NULL;
				if( itemInfo != NULL )
				{
					weaponSkill = MMOEquipManager::getSingleton().GetWeaponRuneSkillBySlotIdx(i, itemInfo, isSkillSlotActive);
				}
				if( isSkillSlotActive )
				{
					++ curSlotCnt;
				}
			}
			*/
			element->SetString(TEXT("equip_rune"), *appItoa(curSlotCnt));

			element->SetString(TEXT("equip_prop"), UITextParser::HTMLFormatItemAttributes(equip, NULL));
			// 套装属性
			EquipSuit* suit =  MMOEquipManager::getSingleton().FindEquipSuitByPartOfSuit(equip->ItemId);
			FString suitProp = (suit != NULL) ? UITextParser::HTMLFormatEquipSuitAttribute(equip) : TEXT("");
			suitProp.ReplaceInline(TEXT("#9d9d9d"), TEXT("#FFFFFF"));
			element->SetString(TEXT("equip_suit_prop"), suitProp);

			element->SetString(TEXT("equip_desc"), UITextParser::HTMLFormatItemDescription(item));

			array0->SetElementObject(0, element);
		}

		object->SetObject(TEXT("handbook_data0"), array0);
	}

	eventCallUpdateHandbookInfo(object);

	_CurType = type;
	_CurIndex = index;
}

void UMMOUIHandbookPanel::_UpdateRuneInfo(UINT32 type, UINT32 index)
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Handbook.HandbookData"));
	if (!object)
	{
		return;
	}

	HandbookData data;
	if (!MMOHandbookData::getSingleton().GetHandbookData(type, index, data))
	{
		return;
	}

	const WeaponSkillCfg* rune = MMOEquipManager::getSingleton().GetWeaponSkillCfg(data.TemplateId(0));
	if (!rune)
	{
		return;
	}

	// bug: #19407
	UINT skillKey = MMOSkillManager::CombinateAbilityID(rune->_SkillID, 0, 0);
	UINT runeKey = MMOSkillManager::CombinateAbilityID(rune->_SkillID, 0, rune->_SkillRuneID);
	
	UMMOAbility* skill = MMOSkillManager::getSingleton().FindAbilityTemplate(skillKey);
	if (!skill)
	{
		return;
	}

	const SpellVarData* skillData = SpellVarManager::getSingleton().FindData(skillKey);
	const SpellVarData* runeData = SpellVarManager::getSingleton().FindData(runeKey);
	if (!skillData || !runeData)
	{
		return;
	}

	object->SetInt(TEXT("handbook_type"), data.Type);
	object->SetInt(TEXT("handbook_index"), data.Index);
	object->SetInt(TEXT("handbook_ui_type"), _ConvertHandbookTypeToUiType(data.Type));
	object->SetString(TEXT("handbook_name"), data.Name);
	object->SetString(TEXT("handbook_sound"), data.Sound);

	object->SetString(TEXT("handbook_text0"), skillData->DetailDescription);
	object->SetString(TEXT("handbook_text1"), runeData->RuneDescription);
	object->SetString(TEXT("handbook_text2"), FString::Printf(TEXT("%d"), rune->_Level));

	UGFxObject* array0 = GFXPlayer->CreateArray();
	if (array0)
	{
		UGFxObject* element = GFXPlayer->CreateObject(TEXT("Object"));
		if (element)
		{
			INT uniqueId = MMOSkillManager::CombinateAbilityID(skill->GetTemplateID(), 
				skill->SpellCommonAttribute.Level);
			element->SetInt(TEXT("skill_id"), uniqueId);
			FString skillIcon = FString(TEXT("img://")) + skill->GetAbilityIconName();
			element->SetString(TEXT("skill_icon"), skillIcon);

			array0->SetElementObject(0, element);
		}

		object->SetObject(TEXT("handbook_data0"), array0);
	}

	eventCallUpdateHandbookInfo(object);

	_CurType = type;
	_CurIndex = index;
}

void UMMOUIHandbookPanel::_UpdateSpiritInfo(UINT32 type, UINT32 index)
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Handbook.HandbookData"));
	if (!object)
	{
		return;
	}

	HandbookData data;
	if (!MMOHandbookData::getSingleton().GetHandbookData(type, index, data))
	{
		return;
	}

	object->SetInt(TEXT("handbook_type"), data.Type);
	object->SetInt(TEXT("handbook_index"), data.Index);
	object->SetInt(TEXT("handbook_ui_type"), _ConvertHandbookTypeToUiType(data.Type));
	object->SetString(TEXT("handbook_name"), data.Name);
	object->SetString(TEXT("handbook_text0"), data.Text0);
	object->SetString(TEXT("handbook_text1"), data.Text1);
	object->SetString(TEXT("handbook_sound"), data.Sound);

	// 龙卫心法
	if (data.TemplateId.Num() > 0)
	{
		UGFxObject* array0 = GFXPlayer->CreateArray();
		if (array0)
		{
			int index = 0;
			for (int i = 0; i < data.TemplateId.Num(); ++ i)
			{
				const HeavenGasItemConfig* heaven = MMOHeavenManager::getSingleton().GetGasTemplate(data.TemplateId(i));
				if (!heaven)
				{
					continue;
				}

				UGFxObject* element = GFXPlayer->CreateObject(TEXT("Object"));
				if (element)
				{
					element->SetInt(TEXT("spirit_id"), heaven->nID);
					FString icon = FString::Printf(TEXT("../%s"),*(heaven->Icon0));
					element->SetString(TEXT("spirit_icon"), icon);

					FString effect0 = TEXT("");
					MMOHeavenManager::getSingleton().GetGasSkillArr(effect0, heaven->nAtriNodeID1);
					FString effect1 = TEXT("");
					MMOHeavenManager::getSingleton().GetGasSkillArr(effect1, heaven->nAtriNodeID2);
					effect0 += effect1;

					element->SetString(TEXT("spirit_effect"), effect0);

					array0->SetElementObject(index, element);
					index ++;
				}
			}
			object->SetObject(TEXT("handbook_data0"), array0);
		}
	}

	// 等级列表
	UGFxObject* array1 = GFXPlayer->CreateArray();
	if (array1)
	{
		int index = 0;
		for (int i = 0; i < data.TemplateId.Num(); ++ i)
		{
			const HeavenGasItemConfig* heaven = MMOHeavenManager::getSingleton().GetGasTemplate(data.TemplateId(i));
			if (!heaven)
			{
				continue;
			}

			array1->SetElementString(index, *appItoa(heaven->nLevel));
			index ++;
		}

		object->SetObject(TEXT("handbook_data1"), array1);
	}

	eventCallUpdateHandbookInfo(object);

	_CurType = type;
	_CurIndex = index;
}

int UMMOUIHandbookPanel::GetCurClass()
{
	return _CurClass;
}

int UMMOUIHandbookPanel::GetCurType()
{
	return _CurType;
}

int UMMOUIHandbookPanel::GetCurIndex()
{
	return _CurIndex;
}

void UMMOUIHandbookPanel::SetUsableOnly(UBOOL usableOnly)
{
	_UsableOnly = usableOnly;
}

int UMMOUIHandbookPanel::GetLastClass()
{
	return _LastClass;
}

int UMMOUIHandbookPanel::GetLastType()
{
	return _LastType;
}

int UMMOUIHandbookPanel::GetLastIndex()
{
	return _LastIndex;
}

//////////////////////////////////////////////////////////////////////////

void UMMOUIHandbookPanel::ASSetHandbookClass(int handbookClass)
{
	_CurClass = handbookClass;
	UpdateHandbookClass(handbookClass);
	UpdateHandbookClassDefaultInfo(handbookClass);

	int curClass = GetCurClass();
	int curType = GetCurType();
	int curIndex = GetCurIndex();
	SetLastHandbook(curClass, curType, curIndex);
	eventCallCanBack();
}

void UMMOUIHandbookPanel::ASSetHandbookType(int selectIndex)
{
	switch (_CurClass)
	{
	case HC_Monster:
		{
			int index = 0;
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNpc_Normal) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htNpc_Normal);
					UpdateHandbookInfo(htNpc_Normal, 0);
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNpc_Elite) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htNpc_Elite);
					UpdateHandbookInfo(htNpc_Elite, 0);
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNPc_Boss) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htNPc_Boss);
					UpdateHandbookInfo(htNPc_Boss, 0);
				}
				index ++;
			}
		}
		break;
	case HC_Npc:
		{
			int index = 0;
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNpc_Function) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htNpc_Function);
					UpdateHandbookInfo(htNpc_Function, 0);
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNpc_Story) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htNpc_Story);
					UpdateHandbookInfo(htNpc_Story, 0);
				}
				index ++;
			}
		}
		break;
	case HC_Map:
		{
			int index = 0;
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htMap_Wild) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htMap_Wild);
					UpdateHandbookInfo(htMap_Wild, 0);
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htMap_Dungeon) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htMap_Dungeon);
					UpdateHandbookInfo(htMap_Dungeon, 0);
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htMap_Activity) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htMap_Activity);
					UpdateHandbookInfo(htMap_Activity, 0);
				}
				index ++;
			}
		}
		break;
	case HC_Item:
		{
			int index = 0;
			/*
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htItem_Compose) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htItem_Compose);
					UpdateHandbookInfo(htItem_Compose, 0);
				}
				index ++;
			}
			*/
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htItem_Stone) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htItem_Stone);
					UpdateHandbookInfo(htItem_Stone, 0);
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htItem_Story) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htItem_Story);
					UpdateHandbookInfo(htItem_Story, 0);
				}
				index ++;
			}
		}
		break;
	case HC_Equip:
		{
			int index = 0;
			/*
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htEquip_Set) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htEquip_Set);
					UpdateHandbookInfo(htEquip_Set, 0);
				}
				index ++;
			}
			*/
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htEquip_Weapon) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htEquip_Weapon);
					UpdateHandbookInfo(htEquip_Weapon, 0);
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htEquip_Jewelry) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htEquip_Jewelry);
					UpdateHandbookInfo(htEquip_Jewelry, 0);
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htEquip_Armor) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htEquip_Armor);
					UpdateHandbookInfo(htEquip_Armor, 0);
				}
				index ++;
			}
		}
		break;
	case HC_Rune:
		{
			int index = 0;
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Warrior) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htRune_Warrior);
					UpdateHandbookInfo(htRune_Warrior, 0);
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Mage) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htRune_Mage);
					UpdateHandbookInfo(htRune_Mage, 0);
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Rogue) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htRune_Rogue);
					UpdateHandbookInfo(htRune_Rogue, 0);
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Archer) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htRune_Archer);
					UpdateHandbookInfo(htRune_Archer, 0);
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Taoist) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htRune_Taoist);
					UpdateHandbookInfo(htRune_Taoist, 0);
				}
				index ++;
			}
		}
		break;
	case HC_Spirit:
		{
			int index = 0;
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htAir_Green) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htAir_Green);
					UpdateHandbookInfo(htAir_Green, 0);
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htAir_Blue) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htAir_Blue);
					UpdateHandbookInfo(htAir_Blue, 0);
				}
				index ++;
			}
			if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htAir_Purple) > 0)
			{
				if (index == selectIndex)
				{
					UpdateHandbookList(htAir_Purple);
					UpdateHandbookInfo(htAir_Purple, 0);
				}
				index ++;
			}
		}
		break;
	}

	int curClass = GetCurClass();
	int curType = GetCurType();
	int curIndex = GetCurIndex();
	SetLastHandbook(curClass, curType, curIndex);
	eventCallCanBack();
}

void UMMOUIHandbookPanel::ASClickHandbookItem(int type, int index)
{
	int curClass = GetCurClass();
	int curType = GetCurType();
	int curIndex = GetCurIndex();
	SetLastHandbook(curClass, curType, curIndex);
	eventCallCanBack();

	UpdateHandbookInfo(type, index);
}

void UMMOUIHandbookPanel::ASShowTips_SkillSettingActive( UGFxObject *mc )
{
	if (mc == NULL)
	{
		return;
	}

	INT uniqueSkillID = mc->GetInt( TEXT("UniqueSkillID") );
	if (uniqueSkillID > 0)
	{
		MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_SkillTemplate, mc );
	}
}

void UMMOUIHandbookPanel::ASHideTips_SkillSettingActive( UGFxObject *mc )
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOUIHandbookPanel::ASShowTips_MonsterSkill( UGFxObject *mc )
{
	if (mc == NULL)
	{
		return;
	}

	INT uniqueSkillID = mc->GetInt( TEXT("UniqueSkillID") );
	if (uniqueSkillID > 0)
	{
		UMMOAbility* ability = MMOSkillManager::getSingleton().FindAbilityTemplate(uniqueSkillID);
		if (ability)
		{
			FString skillName = ability->eventGetAbilityName();
			FString skillEffect = ability->eventGetAbilityDescribe();

			MMOUI::GetUIManager()->delegateShowFreeStyleTips(skillName, skillEffect, CM_TIPS_Style1, 3);
		}
	}
}

void UMMOUIHandbookPanel::ASHideTips_MonsterSkill( UGFxObject *mc )
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOUIHandbookPanel::ASGotoMonsterPage(const FString& monsterName)
{
	UINT32 type = 0;
	UINT32 index = 0;
	if (MMOHandbookData::getSingleton().GetMonsterHandbookInfoByName(monsterName, type, index))
	{
		_CurClass = HC_Monster;
		UpdateHandbookClass(HC_Monster);
		UpdateHandbookList(type, index);
		UpdateHandbookInfo(type, index);
	}
	else
	{
		FString error = TEXT("未找到该图鉴BOOS信息");
		MMOUI::GetUIManager()->delegateAddScreenMessage(error, MsgInfoType_Table, TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
	}
}

void UMMOUIHandbookPanel::ASGotoMapPage(const FString& mapName)
{
	UINT32 type = 0;
	UINT32 index = 0;
	if (MMOHandbookData::getSingleton().GetMapHandbookInfoByName(mapName, type, index))
	{
		_CurClass = HC_Map;
		UpdateHandbookClass(HC_Map);
		UpdateHandbookList(type, index);
		UpdateHandbookInfo(type, index);

		int curClass = GetCurClass();
		int curType = GetCurType();
		int curIndex = GetCurIndex();
		SetLastHandbook(curClass, curType, curIndex);
		eventCallCanBack();
	}
	else
	{
		FString error = TEXT("未找到该图鉴地图信息");
		MMOUI::GetUIManager()->delegateAddScreenMessage(error, MsgInfoType_Table, TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
	}
}

void UMMOUIHandbookPanel::ASCheckUseableOnly(UBOOL useableOnly)
{
	SetUsableOnly(useableOnly);
	int curType = GetCurType();
	int curIndex = GetCurIndex();
	UpdateHandbookInfo(curType, curIndex);
}

void UMMOUIHandbookPanel::ASSearchByKey(const FString& key)
{
	int curType = GetCurType();
	UpdateHandbookList(curType, 0, key);
}

void UMMOUIHandbookPanel::ASBackButtonClick()
{
	if (GetLastType() > 0)
	{
		int lastClass = GetLastClass();
		int lastType = GetLastType();
		int lastIndex = GetLastIndex();
		UpdateHandbookClass(lastClass);
		UpdateHandbookList(lastType, lastIndex);
		UpdateHandbookInfo(lastType, lastIndex);
	}
}

void UMMOUIHandbookPanel::ASOpenWorldMap(INT mapId)
{
	UMMOUIWorldMap* worldMapPanel = (UMMOUIWorldMap*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_WorldMap);
	if (worldMapPanel)
	{
		worldMapPanel->Show(TRUE);
		worldMapPanel->ShowMapPanelContent(ePnlTyp_UnitedMap);

		int curType = GetCurType();
		int curIndex = GetCurIndex();
		HandbookData data;
		if (MMOHandbookData::getSingleton().GetHandbookData(curType, curIndex, data))
		{
			INT regionInMapIdx = MMOMapDataManager::getSingleton().GetReginInMapIdx(mapId, data.Text1);
			if (regionInMapIdx >= 0)
			{
				worldMapPanel->ShowWorldMapInUnitedMapContent(mapId, regionInMapIdx);
			}
		}
	}
}

void UMMOUIHandbookPanel::ASGotoPosition(INT mapId)
{
	INT curMapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	if (curMapId == mapId)
	{
		FString error = TEXT("已经到达当前地图");
		MMOUI::GetUIManager()->delegateAddScreenMessage(error, MsgInfoType_Table, TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
		return;
	}

	if (!MMONavigationManager::getSingleton().FindAndMoveToTeleport(curMapId, mapId, 0, FALSE))
	{
		FString error = TEXT("地图不可到达");
		MMOUI::GetUIManager()->delegateAddScreenMessage(error, MsgInfoType_Table, TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
		return;
	}
}

void UMMOUIHandbookPanel::ASPlayDesciptionSound()
{
	int curType = GetCurType();
	int curIndex = GetCurIndex();
	MMOHandbookManager::getSingleton().PlayHandbookSound(curType, curIndex);
}

void UMMOUIHandbookPanel::ASGotoEquipPage(int itemId)
{
	UINT32 type = 0;
	UINT32 index = 0;
	if (MMOHandbookData::getSingleton().GetEquipHandbookInfoById(itemId, type, index))
	{
		_CurClass = HC_Equip;
		UpdateHandbookClass(HC_Equip);
		UpdateHandbookList(type, index);
		UpdateHandbookInfo(type, index);
	}
	else
	{
		FString error = TEXT("未找到对应的装备信息");
		MMOUI::GetUIManager()->delegateAddScreenMessage(error, MsgInfoType_Table, TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
	}
}

void UMMOUIHandbookPanel::PostAdvance(FLOAT deltaTime)
{
	CheckPendingRT(deltaTime);

	if (_PlayerRotStates != TBS_None)
	{
		RotatePlayer(_PlayerRotStates == TBS_Left ? 1 : 0);
	}

	if (_NpcRotStates != TBS_None)
	{
		RotateNpc(_NpcRotStates == TBS_Left ? 1 : 0);
	}
}

void UMMOUIHandbookPanel::ASPlayerRotate(INT dir)
{
	_PlayerRotStates = dir > 0 ? TBS_Right : TBS_Left;
}

void UMMOUIHandbookPanel::ASNpcRotate(INT dir)
{
	_NpcRotStates = dir > 0 ? TBS_Right : TBS_Left;
}

void UMMOUIHandbookPanel::ASReleaseRotate()
{
	_PlayerRotStates = TBS_None;
	_NpcRotStates = TBS_None;
}


