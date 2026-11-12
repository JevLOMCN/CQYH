#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOHandbookManager.h"
#include "MMONavigationManager.h"
#include "MMOMapDataManager.h"
#include "MMOSkillManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_Handbook)

void UMMOGFXPlayer_Handbook::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_Handbook::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_Handbook);
	_PlayerRotStates = TBS_None;
	_NpcRotStates = TBS_None;
}

void UMMOGFXPlayer_Handbook::UnInit()
{
	Super::UnInit();
}

void UMMOGFXPlayer_Handbook::ASSetHandbookClass(int handbookClass)
{
	if (_HandbookPanel)
	{
		UINT32 curClass = _HandbookPanel->GetCurClass();
		UINT32 curType = _HandbookPanel->GetCurType();
		UINT32 curIndex = _HandbookPanel->GetCurIndex();
		_HandbookPanel->SetLastHandbook(curClass, curType, curIndex);
		_HandbookPanel->eventCallCanBack();

		_HandbookPanel->_CurClass = handbookClass;
		_HandbookPanel->UpdateHandbookClass(handbookClass);
		_HandbookPanel->UpdateHandbookClassDefaultInfo(handbookClass);
	}
}

void UMMOGFXPlayer_Handbook::ASSetHandbookType(int selectIndex)
{
	if (_HandbookPanel)
	{
		UINT32 curClass = _HandbookPanel->GetCurClass();
		UINT32 curType = _HandbookPanel->GetCurType();
		UINT32 curIndex = _HandbookPanel->GetCurIndex();
		_HandbookPanel->SetLastHandbook(curClass, curType, curIndex);
		_HandbookPanel->eventCallCanBack();

		switch (_HandbookPanel->_CurClass)
		{
		case HC_Monster:
			{
				int index = 0;
				if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNpc_Normal) > 0)
				{
					if (index == selectIndex)
					{
						_HandbookPanel->UpdateHandbookList(htNpc_Normal);
						_HandbookPanel->UpdateHandbookInfo(htNpc_Normal, 0);
					}
					index ++;
				}
				if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNpc_Elite) > 0)
				{
					if (index == selectIndex)
					{
						_HandbookPanel->UpdateHandbookList(htNpc_Elite);
						_HandbookPanel->UpdateHandbookInfo(htNpc_Elite, 0);
					}
					index ++;
				}
				if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNPc_Boss) > 0)
				{
					if (index == selectIndex)
					{
						_HandbookPanel->UpdateHandbookList(htNPc_Boss);
						_HandbookPanel->UpdateHandbookInfo(htNPc_Boss, 0);
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
						_HandbookPanel->UpdateHandbookList(htNpc_Function);
						_HandbookPanel->UpdateHandbookInfo(htNpc_Function, 0);
					}
					index ++;
				}
				if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htNpc_Story) > 0)
				{
					if (index == selectIndex)
					{
						_HandbookPanel->UpdateHandbookList(htNpc_Story);
						_HandbookPanel->UpdateHandbookInfo(htNpc_Story, 0);
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
						_HandbookPanel->UpdateHandbookList(htMap_Wild);
						_HandbookPanel->UpdateHandbookInfo(htMap_Wild, 0);
					}
					index ++;
				}
				if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htMap_Dungeon) > 0)
				{
					if (index == selectIndex)
					{
						_HandbookPanel->UpdateHandbookList(htMap_Dungeon);
						_HandbookPanel->UpdateHandbookInfo(htMap_Dungeon, 0);
					}
					index ++;
				}
				if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htMap_Activity) > 0)
				{
					if (index == selectIndex)
					{
						_HandbookPanel->UpdateHandbookList(htMap_Activity);
						_HandbookPanel->UpdateHandbookInfo(htMap_Activity, 0);
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
						_HandbookPanel->UpdateHandbookList(htItem_Compose);
						_HandbookPanel->UpdateHandbookInfo(htItem_Compose, 0);
					}
					index ++;
				}
				*/
				if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htItem_Stone) > 0)
				{
					if (index == selectIndex)
					{
						_HandbookPanel->UpdateHandbookList(htItem_Stone);
						_HandbookPanel->UpdateHandbookInfo(htItem_Stone, 0);
					}
					index ++;
				}
				if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htItem_Story) > 0)
				{
					if (index == selectIndex)
					{
						_HandbookPanel->UpdateHandbookList(htItem_Story);
						_HandbookPanel->UpdateHandbookInfo(htItem_Story, 0);
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
						_HandbookPanel->UpdateHandbookList(htEquip_Set);
						_HandbookPanel->UpdateHandbookInfo(htEquip_Set, 0);
					}
					index ++;
				}
				*/
				if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htEquip_Weapon) > 0)
				{
					if (index == selectIndex)
					{
						_HandbookPanel->UpdateHandbookList(htEquip_Weapon);
						_HandbookPanel->UpdateHandbookInfo(htEquip_Weapon, 0);
					}
					index ++;
				}
				if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htEquip_Jewelry) > 0)
				{
					if (index == selectIndex)
					{
						_HandbookPanel->UpdateHandbookList(htEquip_Jewelry);
						_HandbookPanel->UpdateHandbookInfo(htEquip_Jewelry, 0);
					}
					index ++;
				}
				if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htEquip_Armor) > 0)
				{
					if (index == selectIndex)
					{
						_HandbookPanel->UpdateHandbookList(htEquip_Armor);
						_HandbookPanel->UpdateHandbookInfo(htEquip_Armor, 0);
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
						_HandbookPanel->UpdateHandbookList(htRune_Warrior);
						_HandbookPanel->UpdateHandbookInfo(htRune_Warrior, 0);
					}
					index ++;
				}
				if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Mage) > 0)
				{
					if (index == selectIndex)
					{
						_HandbookPanel->UpdateHandbookList(htRune_Mage);
						_HandbookPanel->UpdateHandbookInfo(htRune_Mage, 0);
					}
					index ++;
				}
				if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Rogue) > 0)
				{
					if (index == selectIndex)
					{
						_HandbookPanel->UpdateHandbookList(htRune_Rogue);
						_HandbookPanel->UpdateHandbookInfo(htRune_Rogue, 0);
					}
					index ++;
				}
				if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Archer) > 0)
				{
					if (index == selectIndex)
					{
						_HandbookPanel->UpdateHandbookList(htRune_Archer);
						_HandbookPanel->UpdateHandbookInfo(htRune_Archer, 0);
					}
					index ++;
				}
				if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htRune_Taoist) > 0)
				{
					if (index == selectIndex)
					{
						_HandbookPanel->UpdateHandbookList(htRune_Taoist);
						_HandbookPanel->UpdateHandbookInfo(htRune_Taoist, 0);
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
						_HandbookPanel->UpdateHandbookList(htAir_Green);
						_HandbookPanel->UpdateHandbookInfo(htAir_Green, 0);
					}
					index ++;
				}
				if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htAir_Blue) > 0)
				{
					if (index == selectIndex)
					{
						_HandbookPanel->UpdateHandbookList(htAir_Blue);
						_HandbookPanel->UpdateHandbookInfo(htAir_Blue, 0);
					}
					index ++;
				}
				if (MMOHandbookData::getSingleton().GetMaxHandbookCount(htAir_Purple) > 0)
				{
					if (index == selectIndex)
					{
						_HandbookPanel->UpdateHandbookList(htAir_Purple);
						_HandbookPanel->UpdateHandbookInfo(htAir_Purple, 0);
					}
					index ++;
				}
			}
			break;
		}
	}
}

void UMMOGFXPlayer_Handbook::ASClickHandbookItem(int type, int index)
{
	if (_HandbookPanel)
	{
		UINT32 curClass = _HandbookPanel->GetCurClass();
		UINT32 curType = _HandbookPanel->GetCurType();
		UINT32 curIndex = _HandbookPanel->GetCurIndex();
		_HandbookPanel->SetLastHandbook(curClass, curType, curIndex);
		_HandbookPanel->eventCallCanBack();

		_HandbookPanel->UpdateHandbookInfo(type, index);
	}
}

void UMMOGFXPlayer_Handbook::ASShowTips_SkillSettingActive( UGFxObject *mc )
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

void UMMOGFXPlayer_Handbook::ASHideTips_SkillSettingActive( UGFxObject *mc )
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOGFXPlayer_Handbook::ASShowTips_MonsterSkill( UGFxObject *mc )
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

void UMMOGFXPlayer_Handbook::ASHideTips_MonsterSkill( UGFxObject *mc )
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOGFXPlayer_Handbook::ASGotoMonsterPage(const FString& monsterName)
{
	if (_HandbookPanel)
	{
		UINT32 type = 0;
		UINT32 index = 0;
		if (MMOHandbookData::getSingleton().GetMonsterHandbookInfoByName(monsterName, type, index))
		{
			UINT32 curClass = _HandbookPanel->GetCurClass();
			UINT32 curType = _HandbookPanel->GetCurType();
			UINT32 curIndex = _HandbookPanel->GetCurIndex();
			_HandbookPanel->SetLastHandbook(curClass, curType, curIndex);
			_HandbookPanel->eventCallCanBack();

			_HandbookPanel->_CurClass = HC_Monster;
			_HandbookPanel->UpdateHandbookClass(HC_Monster);
			_HandbookPanel->UpdateHandbookList(type, index);
			_HandbookPanel->UpdateHandbookInfo(type, index);
		}
		else
		{
			FString error = TEXT("Handbook BOSS info not found");
			MMOUI::GetUIManager()->delegateAddScreenMessage(error, MsgInfoType_Table, TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
		}
	}
}

void UMMOGFXPlayer_Handbook::ASGotoMapPage(const FString& mapName)
{
	if (_HandbookPanel)
	{
		UINT32 type = 0;
		UINT32 index = 0;
		if (MMOHandbookData::getSingleton().GetMapHandbookInfoByName(mapName, type, index))
		{
			UINT32 curClass = _HandbookPanel->GetCurClass();
			UINT32 curType = _HandbookPanel->GetCurType();
			UINT32 curIndex = _HandbookPanel->GetCurIndex();
			_HandbookPanel->SetLastHandbook(curClass, curType, curIndex);
			_HandbookPanel->eventCallCanBack();

			_HandbookPanel->_CurClass = HC_Map;
			_HandbookPanel->UpdateHandbookClass(HC_Map);
			_HandbookPanel->UpdateHandbookList(type, index);
			_HandbookPanel->UpdateHandbookInfo(type, index);
		}
		else
		{
			FString error = TEXT("Handbook map info not found");
			MMOUI::GetUIManager()->delegateAddScreenMessage(error, MsgInfoType_Table, TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
		}
	}
}

void UMMOGFXPlayer_Handbook::ASCheckUseableOnly(UBOOL useableOnly)
{
	if (_HandbookPanel)
	{
		_HandbookPanel->SetUsableOnly(useableOnly);
		int curType = _HandbookPanel->GetCurType();
		int curIndex = _HandbookPanel->GetCurIndex();
		_HandbookPanel->UpdateHandbookInfo(curType, curIndex);
	}
}

void UMMOGFXPlayer_Handbook::ASSearchByKey(const FString& key)
{
	if (_HandbookPanel)
	{
		int curType = _HandbookPanel->GetCurType();
		_HandbookPanel->UpdateHandbookList(curType, 0, key);
	}
}

void UMMOGFXPlayer_Handbook::ASBackButtonClick()
{
	if (_HandbookPanel)
	{
		if (_HandbookPanel->GetLastType() > 0)
		{
			int lastClass = _HandbookPanel->GetLastClass();
			int lastType = _HandbookPanel->GetLastType();
			int lastIndex = _HandbookPanel->GetLastIndex();

			_HandbookPanel->_CurClass = lastClass;
			_HandbookPanel->UpdateHandbookClass(lastClass);
			_HandbookPanel->UpdateHandbookList(lastType, lastIndex);
			_HandbookPanel->UpdateHandbookInfo(lastType, lastIndex);
		}
	}
}

void UMMOGFXPlayer_Handbook::ASOpenWorldMap(INT mapId)
{
	UMMOUIWorldMap* worldMapPanel = (UMMOUIWorldMap*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_WorldMap);
	if (worldMapPanel)
	{
		worldMapPanel->Show(TRUE);
		worldMapPanel->ShowMapPanelContent(ePnlTyp_UnitedMap);

		int curType = _HandbookPanel->GetCurType();
		int curIndex = _HandbookPanel->GetCurIndex();
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

void UMMOGFXPlayer_Handbook::ASGotoPosition(INT mapId)
{
	INT curMapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	if (curMapId == mapId)
	{
		FString error = TEXT("Already on the current map");
		MMOUI::GetUIManager()->delegateAddScreenMessage(error, MsgInfoType_Table, TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
		return;
	}

	if (!MMONavigationManager::getSingleton().FindAndMoveToTeleport(curMapId, mapId, 0, FALSE))
	{
		FString error = TEXT("Map cannot be reached");
		MMOUI::GetUIManager()->delegateAddScreenMessage(error, MsgInfoType_Table, TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
		return;
	}
}

void UMMOGFXPlayer_Handbook::ASPlayDesciptionSound()
{
	if (_HandbookPanel)
	{
		int curType = _HandbookPanel->GetCurType();
		int curIndex = _HandbookPanel->GetCurIndex();
		MMOHandbookManager::getSingleton().PlayHandbookSound(curType, curIndex);
	}
}

void UMMOGFXPlayer_Handbook::ASGotoEquipPage(int itemId)
{
	if (_HandbookPanel)
	{
		UINT32 type = 0;
		UINT32 index = 0;
		if (MMOHandbookData::getSingleton().GetEquipHandbookInfoById(itemId, type, index))
		{
			UINT32 curClass = _HandbookPanel->GetCurClass();
			UINT32 curType = _HandbookPanel->GetCurType();
			UINT32 curIndex = _HandbookPanel->GetCurIndex();
			_HandbookPanel->SetLastHandbook(curClass, curType, curIndex);
			_HandbookPanel->eventCallCanBack();

			_HandbookPanel->_CurClass = HC_Equip;
			_HandbookPanel->UpdateHandbookClass(HC_Equip);
			_HandbookPanel->UpdateHandbookList(type, index);
			_HandbookPanel->UpdateHandbookInfo(type, index);
		}
		else
		{
			FString error = TEXT("Equipment info not found");
			MMOUI::GetUIManager()->delegateAddScreenMessage(error, MsgInfoType_Table, TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
		}
	}
}

void UMMOGFXPlayer_Handbook::PostAdvance(FLOAT deltaTime)
{
	UGFxMoviePlayer::PostAdvance(deltaTime);

	if (_HandbookPanel)
	{
		if (_PlayerRotStates != TBS_None)
		{
			_HandbookPanel->RotatePlayer(_PlayerRotStates == TBS_Left ? 1 : 0);
		}

		if (_NpcRotStates != TBS_None)
		{
			_HandbookPanel->RotateNpc(_NpcRotStates == TBS_Left ? 1 : 0);
		}
	}
}

void UMMOGFXPlayer_Handbook::ASPlayerRotate(INT dir)
{
	if (_HandbookPanel)
	{
		_PlayerRotStates = dir > 0 ? TBS_Right : TBS_Left;
	}
}

void UMMOGFXPlayer_Handbook::ASNpcRotate(INT dir)
{
	if (_HandbookPanel)
	{
		_NpcRotStates = dir > 0 ? TBS_Right : TBS_Left;
	}
}

void UMMOGFXPlayer_Handbook::ASReleaseRotate()
{
	if (_HandbookPanel)
	{
		_PlayerRotStates = TBS_None;
		_NpcRotStates = TBS_None;
	}
}


