#include "stdafx.h"
#include "MMOFacade.h"
#include "UITextParser.h"
#include "LinkManager.h"
#include "MMOMapDataManager.h"
#include "DoodadManager.h"		
#include "CommonGameEvents.h"
#include "MMOGuideManager.h"
#include "MMOChatManager.h"
#include "MMOGameClasses.h"
#include "MMOMsgBoxManager.h"
#include "MMOEquipManager.h"
#include "EquipSlotManager.h"
#include "EquipDecomposeManager.h"
#include "ClientActionConditionCheck.h"
#include "MMOQuestManager.h"

extern bool g_lockMouseCursorForUI;
EquipDecomposeManager::EquipDecomposeManager()
{
	MMOUI::RegisterEventHandler(this,GameEvent_EquipMaker);
	m_eDecomposeStatus = EDS_None;
	m_iCurComposedId = -1;
	Init();
}

EquipDecomposeManager::~EquipDecomposeManager()
{
	MMOUI::UnRegisterEventHandler(this,GameEvent_EquipMaker);
}

void EquipDecomposeManager::Init()
{
	ReadConfig();
}

void EquipDecomposeManager::ReadConfig()
{
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("Compose.txt"), TRUE ) )
	{
		warnf(TEXT("EquipDecomposeManager::ReadConfig() error"));
		return;
	}
	const INT itemConsumeIdxBegin = 4;
	INT rowCnt = tabFile->GetRowCount();
	for(INT i = 3; i < rowCnt; ++i)
	{
		INT itemId = appAtoi( tabFile->GetData( TEXT("FormulaID"), i ) );

		EquipComposeMap::iterator itor = m_EquipComposeMap.find(itemId);
		if(itor != m_EquipComposeMap.end())
		{
			warnf(TEXT("Equip Compose Config Duplicate"));
		}
		else
		{
			EquipComposeTemplate ComposeTemplate;
			ComposeTemplate.itemId = itemId;
			ComposeTemplate.equipId = appAtoi( tabFile->GetData( TEXT("ResultItem"), i ) );
			ComposeTemplate.cost = appAtoi( tabFile->GetData( TEXT("Money"), i ) );
			for(INT j = 0; j < COMPOSE_CONSUME_COUNT; ++j)
			{
				ComposeTemplate.matId[j] = appAtoi( tabFile->GetData( itemConsumeIdxBegin + 2 * j, i ) );
				ComposeTemplate.matCnt[j] = appAtoi(tabFile->GetData( itemConsumeIdxBegin + 1 + 2 * j, i ) );
			}
			INT bCast = appAtoi( tabFile->GetData( TEXT("BroadCast"), i ) );
			if(bCast > 0)
				ComposeTemplate.bBroadCast = TRUE;
			else
				ComposeTemplate.bBroadCast = FALSE;

			m_EquipComposeMap.insert(std::make_pair(itemId,ComposeTemplate));
		}
	}

	delete tabFile;
}

void EquipDecomposeManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch(evt->GetEventType())
	{
	case GameEvent_EquipMaker:
		{
			MMOGameEvent_EquipMaker* equipMakerEvt = (MMOGameEvent_EquipMaker*)evt;
			switch(equipMakerEvt->EventType)
			{
			case MMOGameEvent_EquipMaker::EEDT_DecomposeSuccess:
				{
					if( equipMakerEvt->IntRst )
					{
						FString screenMsg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.DecomposeSuccess") );
						MMOUI::GetUIManager()->delegateAddScreenMessage( screenMsg , MsgInfoType_Table , TEXT("MMOGame.Bag.DecomposeSuccess") );

						//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_EquipDecompose,TRUE);
						MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_EquipDecompose, TRUE, "");
						UMMOUIEquipDecomposePanel* panel = Cast<UMMOUIEquipDecomposePanel>(MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_EquipDecompose));
						if(panel)
						{
							panel->eventCallResetTimer();
						}
						//EndDecompose();
					}
					else
					{
						FString screenMsg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.DecomposeFailed") );
						MMOUI::GetUIManager()->delegateAddScreenMessage( screenMsg , MsgInfoType_Table , TEXT("MMOGame.Bag.DecomposeFailed") );
					}

				}
				break;
			case MMOGameEvent_EquipMaker::EEDT_ComposeSuccess:
				{
					if( equipMakerEvt->IntRst )
					{
						INT curComposeItemID = GetComposedId();

						UMMOUIEquipComposePanel *pEquipCompPanel = Cast<UMMOUIEquipComposePanel>( MMOUI::GetUIManager()->FindUIPanel( MMOUI::GetUIManager()->Panel_EquipCompose ) );
						if( pEquipCompPanel != NULL ){
							pEquipCompPanel->NotifyComposeResult( equipMakerEvt->IntRst );
						}

						FString screenMsg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.ComposeSucceed") );
						EquipComposeTemplate& ComposeTemplate = GetComposeTemplateById( curComposeItemID );
						UItem* pEquip = MMOFacade::getSingleton().FindItemTemplate(ComposeTemplate.equipId);
						if( pEquip != NULL )
						{
							screenMsg += pEquip->GetDisplayName();
						}
						
						MMOUI::GetUIManager()->delegateAddScreenMessage( screenMsg , MsgInfoType_Table , TEXT("MMOGame.Bag.ComposeSucceed") );
					}
					else
					{
						FString screenMsg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.ComposeFailed") );
						MMOUI::GetUIManager()->delegateAddScreenMessage( screenMsg , MsgInfoType_Table , TEXT("MMOGame.Bag.ComposeFailed") );
					}
				}
				break;
			}
		}
		break;
	}
}

EquipComposeTemplate& EquipDecomposeManager::GetComposeTemplateById(INT itemId)
{
	EquipComposeMap::iterator itor = m_EquipComposeMap.find(itemId);

	if( itor == m_EquipComposeMap.end() ){
		warnf( NAME_Error, TEXT("EquipDecomposeManager::GetComposeTemplateById NULL at ID:%d"), itemId );

		EquipComposeTemplate tempComposeTemplate;
		tempComposeTemplate.itemId = itemId;
		tempComposeTemplate.equipId = 0;
		tempComposeTemplate.cost = 0;
		tempComposeTemplate.bBroadCast = FALSE;

		m_EquipComposeMap.insert( std::make_pair(itemId, tempComposeTemplate) );

		itor = m_EquipComposeMap.find(itemId);
	}

	return itor->second;
}

void EquipDecomposeManager::ComposeEquip(INT itemId)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	EquipComposeMap::iterator itor = m_EquipComposeMap.find(itemId);

	if(itor != m_EquipComposeMap.end())
	{
		EquipComposeTemplate& ComposeTemplate = itor->second;

		if(ComposeTemplate.cost > mainPlayer->GetMoney(MONEYTYPE_02))
		{
			//Not enough gold
			MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipDecompose.GeneralError0")),MsgInfoType_Table,TEXT("MMOGame.EquipDecompose.GeneralError0"));
			return;
		}

		for(INT i = 0; i < COMPOSE_CONSUME_COUNT; ++i)
		{
			if(ComposeTemplate.matId[i] <= 0)
				continue;
			if(MMOFacade::getSingleton().GetItemCountInBag( ComposeTemplate.matId[i], E_CONSLOT_DEFAULT ) < ComposeTemplate.matCnt[i])
			{
				//Not enough materials
				MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipDecompose.GeneralError1")),MsgInfoType_Table,TEXT("MMOGame.EquipDecompose.GeneralError1"));
				return;
			}
		}

		//Send synthesis message
		CMessageHandler::GetInstance().Send_Compose((UINT32)itemId);
	}
	else
	{
		warnf(TEXT("EquipDecomposeManager::ComposeEquip: Item ID Not Found"));
	}
}

void EquipDecomposeManager::BeginDecompose()
{
	if(m_eDecomposeStatus == EDS_Compose)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipDecompose.GeneralError5")),MsgInfoType_Table,TEXT("MMOGame.EquipDecompose.GeneralError5"));
		return;
	}
	//m_eDecomposeStatus = EDS_Decompose;
	ChangeState(EDS_Decompose);
	
	//Hammer cursor
	MMOFacade::getSingleton().SetMouseCursor(EMC_Cross);
	g_lockMouseCursorForUI = true;


}

void EquipDecomposeManager::EndDecompose()
{
	//m_eDecomposeStatus = EDS_None;
	ChangeState(EDS_None);
	//Normal cursor
	MMOFacade::getSingleton().SetMouseCursor(EMC_Arrow);
	g_lockMouseCursorForUI = false;
}

void EquipDecomposeManager::BeginCompose(INT itemId)
{
	//ChangeState(EDS_Compose);
	m_iCurComposedId = itemId;
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_EquipCompose,TRUE, "");

	MMOGameEvent_Guide evtGuide(EGUET_ClickCompItem);
	MMOUI::PostGameEvent(&evtGuide);
}

void EquipDecomposeManager::EndCompose()
{
	//ChangeState(EDS_None);
}

void EquipDecomposeManager::RealDecompose(INT iConId, INT iSlotId)
{
	CMessageHandler::GetInstance().Send_Decompose((UINT8)iConId,(UINT8)iSlotId);
}

void EquipDecomposeManager::Decompose(INT iConId, INT iSlotId)
{
	UItem* item = MMOFacade::getSingleton().FindItemTemplate(iConId, iSlotId);
	if(item == NULL)
	{
		return;
	}

	if(MMOFacade::getSingleton().IsBagFull())
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipDecompose.GeneralError3")), MsgInfoType_Table , TEXT("MMOGame.EquipDecompose.GeneralError3") );
		MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipDecompose.GeneralError3")) );
		return;
	}

	UBOOL isOK = FALSE;
	if(item->ItemType == TEXT("ITEMTYPE_EQUIP"))
	{
		isOK = DecomposeEquip(iConId, iSlotId);
	}
	else{
		isOK = DecomposeSkillBook(iConId, iSlotId);
	}

	//
	if(isOK)
	{
		RealDecompose(iConId, iSlotId);
	}
	
}
//-----------------------------------------------------------------------------------------------------------
UBOOL EquipDecomposeManager::DecomposeSkillBook(INT iConId, INT iSlotId)
{
	AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	FClientItemInfo* info = MMOFacade::getSingleton().FindItem(iConId, iSlotId);
	if(info == NULL)
	{
		return FALSE;
	}

	UItem* item = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);
	if(item == NULL)
	{
		return FALSE;
	}

	if( item->ItemType != TEXT("ITEMTYPE_SPELLBOOK") )
	{
		FString errorName = TEXT("MMOGame.EquipDecompose.GeneralError7");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(errorName), MsgInfoType_Table, errorName);
		MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath(errorName) );
		return FALSE;
	}

	if( !item->CanDecompose )
	{
		FString errorName = TEXT("MMOGame.EquipDecompose.GeneralError8");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(errorName), MsgInfoType_Table, errorName );
		MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath(errorName) );
		return FALSE;
	}

	return TRUE;
}
//-----------------------------------------------------------------------------------------------------------
UBOOL EquipDecomposeManager::DecomposeEquip(INT iConId, INT iSlotId)
{
	AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	//UEquip* pEquip = Player->ItemManager.GetEquipBySlot(slotId, false);
	//FClientItemInfo* info = MMOFacade::getSingleton().FindItem(E_CONSLOT_DEFAULT, slotId);
	FClientItemInfo* info = MMOFacade::getSingleton().FindItem(iConId, iSlotId);
	if(info == NULL)
	{
		return FALSE;
	}

	UItem* item = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);
	if(item == NULL)
	{
		return FALSE;
	}

	if( item->ItemType != TEXT("ITEMTYPE_EQUIP") )
	{
		//Not an Equip
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipDecompose.GeneralError2"))
			, MsgInfoType_Table , TEXT("MMOGame.EquipDecompose.GeneralError2") );
		MMOChatManager::getSingleton().AddSystemMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipDecompose.GeneralError2")));
		return FALSE;
	}

	FClientActionConditionContex contex(ECACCT_UseItem_CheckBind);
	CheckClientActionCondition(contex);
	//if( info->m_byBindType == E_BINDTYPE_BINDED )
	//{
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipDecompose.GeneralError4"))
	//		, MsgInfoType_Table , TEXT("MMOGame.EquipDecompose.GeneralError4") );
	//	MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipDecompose.GeneralError4")) );
	//	return;
	//}

	UEquip* ep = Cast<UEquip>(item);
	if(ep == NULL)
	{
		return FALSE;
	}

	if(ep && !ep->CanDecompose)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipDecompose.GeneralError6"))
			, MsgInfoType_Table , TEXT("MMOGame.EquipDecompose.GeneralError6") );
		MMOChatManager::getSingleton().AddSystemMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipDecompose.GeneralError6")));
		return FALSE;
	}

	//Equipment that has a spirit stone
	for(INT i = 0; i < MAX_SOCKET_COUNT; ++i)
	{
		if(info->m_nSoulID[i] > 0)
		{
			//@TODO
			//MMOMsgBoxManager::getSingleton().AddMessageBox(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipDecompose.General1")), 0, OkFunc, CancelFunc);
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_EquipDecomposeHint, TRUE, "");
			UMMOUIEquipDecomposeHintPanel* hintPanel = Cast<UMMOUIEquipDecomposeHintPanel>(MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_EquipDecomposeHint));
			if(hintPanel)
			{
				FString StrFormat = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipDecompose.General1"));
				FString itemName = UITextParser::HTMLBeginFont( UITextParser::HTMLGetItemColor( (EItemColor)item->ItemColor ) )
					+ item->GetDisplayName()
					+ UITextParser::HTMLEndFont();
				hintPanel->SetContent(FString::Printf(*StrFormat,*itemName));
				hintPanel->SetDecomposedEquipPos(iConId,iSlotId);
			}
			return FALSE;
		}
	}

	//Whether it is an inscription weapon
	for( int i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i ){
		UINT weaponSkillID = info->m_wRune[i];
		//if( weaponSkillID == RUNE_BIND_SLOT_CLOSE || weaponSkillID == RUNE_BIND_SLOT_OPEN ){
		if( weaponSkillID != 0 ){
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_EquipDecomposeHint, TRUE, "");
			UMMOUIEquipDecomposeHintPanel* hintPanel = Cast<UMMOUIEquipDecomposeHintPanel>(MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_EquipDecomposeHint));
			if(hintPanel)
			{
				FString StrFormat = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipDecompose.General3"));
				FString itemName = UITextParser::HTMLBeginFont( UITextParser::HTMLGetItemColor( (EItemColor)item->ItemColor ) )
					+ item->GetDisplayName()
					+ UITextParser::HTMLEndFont();
				hintPanel->SetContent(FString::Printf(*StrFormat,*itemName));
				hintPanel->SetDecomposedEquipPos(iConId,iSlotId);
			}
			return FALSE;
		}

	}

	//Green-text attribute equipment
	for (int i=0;i < MAX_RANDOM_ATTR_COUNT;i++)
	{
		if (info->m_wRandomAttrID[i] > 0)
		{
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_EquipDecomposeHint, TRUE, "");
			UMMOUIEquipDecomposeHintPanel* hintPanel = Cast<UMMOUIEquipDecomposeHintPanel>(MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_EquipDecomposeHint));
			if(hintPanel)
			{
				FString StrFormat = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipDecompose.General2"));
				FString itemName = UITextParser::HTMLBeginFont( UITextParser::HTMLGetItemColor( (EItemColor)item->ItemColor ) )
					+ item->GetDisplayName()
					+ UITextParser::HTMLEndFont();
				hintPanel->SetContent(FString::Printf(*StrFormat,*itemName));
				hintPanel->SetDecomposedEquipPos(iConId,iSlotId);
			}
			return FALSE;
		}
	}

	INT hintLevel = 20;
	GConfig->GetInt(TEXT("EquipDecompose"),TEXT("HitLevel"), hintLevel , GDefaultGameIni);
	//High-level equipment
	if(item->ItemLevel >= hintLevel)
	{
		//@TODO
		//MMOMsgBoxManager::getSingleton().AddMessageBox(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipDecompose.General2")), 0, OkFunc, CancelFunc);
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_EquipDecomposeHint, TRUE, "");
		UMMOUIEquipDecomposeHintPanel* hintPanel = Cast<UMMOUIEquipDecomposeHintPanel>(MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_EquipDecomposeHint));
		if(hintPanel)
		{
			FString StrFormat = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipDecompose.General2"));
			FString itemName = UITextParser::HTMLBeginFont( UITextParser::HTMLGetItemColor( (EItemColor)item->ItemColor ) )
				+ item->GetDisplayName()
				+ UITextParser::HTMLEndFont();
			hintPanel->SetContent(FString::Printf(*StrFormat,*itemName));
			hintPanel->SetDecomposedEquipPos(iConId,iSlotId);
		}
		return FALSE;
	}

	////
	//RealDecomposeEquip(iConId, iSlotId);
	////UEquip* pEquip = Cast<UEquip>(item);
	return TRUE;
}