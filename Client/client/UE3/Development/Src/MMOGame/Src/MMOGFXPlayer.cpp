#include "stdafx.h"
#include "MMOFacade.h"

#if WITH_GFx
//#include "GFxUIClasses.h"
#include "ScaleformEngine.h"
#include "GFx/GFx_Event.h"
#endif //WITH_GFx

#include "MMOGameUIClasses.h"
#include "TipsFormatter.h"
#include "MMOFacade.h"
#include "LinkManager.h"
#include "MMOGameClasses.h"
#include "MMOMapDataManager.h"
#include "MMOMsgBoxManager.h"
#include "MMOBattleManager.h"
#include "MMOSkillManager.h"
#include "MMOHeavenManager.h"
#include "MMOGuideManager.h"
#include "MMOEquipManager.h"
#include "MMOGuildData.h"
#include "UMMOUIHelperPanel.h"
#include "MMOBuffManager.h"
#include "AbilityProcessor.h"
#include "FSMIO.h"
#include "MMOClientVariableManager.h"
#include "MMODeathManager.h"
#include "MMOGuildData.h"

using namespace Scaleform;

IMPLEMENT_CLASS(UMMOGFXPlayer)

typedef std::list<FString> PanelList;
extern PanelList g_PanelList;

extern bool g_displayDragDropIcon;
extern bool g_lockMouseCursorForUI;
class UGFxObject* g_TopmostItemSlot = NULL;

void PlayDragDropSound(BYTE SrcContainerID, INT SrcSlotID, EActionSound actionType);


class FxPlayerUserEventHandler : public GFx::UserEventHandler
{	
public:

	FxPlayerUserEventHandler()
	{
	}

	virtual void HandleEvent(GFx::Movie* /*pmovie*/, const GFx::Event& event)
	{
		bool handled = false;
		const GFx::MouseCursorEvent& mcEvent = static_cast<const GFx::MouseCursorEvent&>(event);

		switch(event.Type)
		{
		case GFx::Event::DoShowMouse:
			if (mcEvent.MouseIndex == 0)
			{
				//SetCursor(MovieCursorType);
			}
			handled = true;
			break;

		case GFx::Event::DoHideMouse:
			if (mcEvent.MouseIndex == 0)
			{
				//SetCursor(Platform::Cursor_Hidden);
			}
			handled = true;
			break;

		case GFx::Event::DoSetMouseCursor:
			if (mcEvent.MouseIndex == 0)
			{
				extern INT g_currentCursorIndex;
				switch(mcEvent.GetCursorShape())
				{
				default:
				case GFx::MouseCursorEvent::ARROW:
					g_currentCursorIndex = (INT)EMC_Arrow;
					g_lockMouseCursorForUI = false;
					break;

				case GFx::MouseCursorEvent::HAND:
					g_currentCursorIndex = (INT)EMC_Hand;
					g_lockMouseCursorForUI = true;
					break;

				case GFx::MouseCursorEvent::IBEAM:
					g_currentCursorIndex = (INT)EMC_IBEAM;
					g_lockMouseCursorForUI = true;
					break;
				case GFx::MouseCursorEvent::BUTTON: //add by JimZhang 用于舒工口谕，让拾取点在名字和物品上同样是拾取的cursor
					g_currentCursorIndex = (INT)EMC_Pick;
					g_lockMouseCursorForUI = true;
					break;
				}

				/*if (GetCursor() != Platform::Cursor_Hidden)
				SetCursor(MovieCursorType);*/
			}
			handled = true;
			break;

		default:
			break;
		}
	}
};


void UMMOGFXPlayer::BindingWidgetsInfo()
{
}

UBOOL UMMOGFXPlayer::GetlockMouseCursorForUI()
{
	return g_lockMouseCursorForUI;
}

FxPlayerUserEventHandler g_playerUserEventHandler;


void UMMOGFXPlayer::Init()
{
	if(this->pMovie)
	{
		if(this->pMovie->pView)
			this->pMovie->pView->SetUserEventHandler(&g_playerUserEventHandler);
	}
}

void UMMOGFXPlayer::PostInit()
{
	for(INT i = 0;i< UIObjectList.Num(); ++i)
	{
		if(UIObjectList(i) != NULL)
		{
			UIObjectList(i)->Init();
			UIObjectList(i)->eventInitOnce();		
			//UIObjectList[i].InitWidget();
		}
	}
}

void UMMOGFXPlayer::OnClose()
{
	UnInit();
}

void UMMOGFXPlayer::UnInit()
{
	for(INT i = 0;i< UIObjectList.Num(); ++i)
	{
		if(UIObjectList(i) != NULL)
			UIObjectList(i)->UnInit();
	}

	UIObjectList.Empty();

	MMOUI::RegisterMoviePlayer(NULL, FriendlyName);

	DeleteEventAdapter();	
}


void UMMOGFXPlayer::RegisterEnd()
{
	Init();

	PostInit();

	RegisterEndCalled = TRUE;
}


//------------------------------------------------------------------------
class UGFxObject* UMMOGFXPlayer::GetDragIcon()
{
	UGFxObject* dropIcon = NULL;
	if(MMOUI::GetUIManager()->GetCommonUIComponentMoviePlayer() != NULL)
	{
		//MMOUI::GetUIManager()->GetCommonUIComponentMoviePlayer()->eventCallTraceAllChild();
		dropIcon = MMOUI::GetUIManager()->GetCommonUIComponentMoviePlayer()->eventCallGetDropIcon();
		//GetVariableObject(TEXT("root.DropIcon_mc"));
	}
	return dropIcon;
}

void UMMOGFXPlayer::ResetDragState()
{
	if(g_displayDragDropIcon)
	{
		g_displayDragDropIcon = FALSE;
		UGFxObject* dropIcon = GetDragIcon();
		if(dropIcon){
			dropIcon->SetVisible(false);
		}
		MMOFacade::ShowMouseCursor(TRUE);
	}
}

UItem* GetItemTemplateFromSlot(class UGFxObject* SrcItemSlot,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID)
{
	//是否是物品
	UItem* itemTemp = NULL;
	switch( SrcSlotType )
	{
	case ESPType_SkillBar:
		{
			if( SrcItemSlot == NULL ){
				break;
			}
			INT curSlotDataType = SrcItemSlot->GetInt(TEXT("EquipID"));

			if( curSlotDataType == ESBarSlotTp_Item ){
				INT itemTempID = SrcItemSlot->GetInt(TEXT("itemIndex"));
				itemTemp = MMOFacade::getSingleton().FindItemTemplate(itemTempID);
			}
		}
		break;
	case ESPType_GuildWarehouse:
		{
			if( SrcItemSlot == NULL ){
				break;
			}

			INT itemTempID = SrcItemSlot->GetInt(TEXT("itemIndex"));
			itemTemp = MMOFacade::getSingleton().FindItemTemplate(itemTempID);
		}
		break;
	default:
		{
			itemTemp = MMOFacade::getSingleton().FindItemTemplate(SrcContainerID, SrcSlotID);
		}
		break;
	}

	return itemTemp;
}

void PlayDragDropSound(class UGFxObject* SrcItemSlot,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID, EActionSound actionType)
{
	//是否是物品
	UItem* itemTemp = GetItemTemplateFromSlot(SrcItemSlot, SrcSlotType, SrcContainerID, SrcSlotID);
	
	if(itemTemp)
	{
		AMMOActor* mainPlayer = MMOFacade::getSingleton().GetMainActor();

		UGamePlayDataManager::GetSingleton()->PlayItemActionSounds(mainPlayer, itemTemp->SoundActionType, actionType);
	}
}

void PlayDragDropSound(BYTE SrcContainerID, INT SrcSlotID, EActionSound actionType)
{
	//是否是物品
	UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(SrcContainerID, SrcSlotID);

	if(itemTemp)
	{
		AMMOActor* mainPlayer = MMOFacade::getSingleton().GetMainActor();

		UGamePlayDataManager::GetSingleton()->PlayItemActionSounds(mainPlayer, itemTemp->SoundActionType, actionType);
	}
}



//新版拖动接口-------------------------------------------------------------------------------------------------------------------------------------------------------
void UMMOGFXPlayer::ASSlot_MouseLeftPress(class UGFxObject* mcSlot)
{
	if(mcSlot==NULL)return;
	UGFxObject* datSlot = mcSlot->GetObject(TEXT("SlotData"));
	if(datSlot==NULL)return;
	INT SlotType	= datSlot->GetInt(TEXT("slotType"));
	INT ContainerID = datSlot->GetInt(TEXT("containerID"));
	INT SlotID		= datSlot->GetInt(TEXT("slotID"));
	UBOOL bDefault  = FALSE;
	//MouseLeftPressItem(mcSlot, SlotType, ContainerID, SlotID, bDefault);

}

void UMMOGFXPlayer::ASSlot_StartDrag(class UGFxObject* mcSlot)
{
	MMOFacade::getSingleton().GameCaptureInput = true; // oscar@snda

	if(mcSlot==NULL)
		return;


	UGFxObject* datSlot = mcSlot->GetObject(TEXT("SlotData"));
	if(datSlot==NULL)return;
	INT SlotType	= datSlot->GetInt(TEXT("slotType"));
	INT ContainerID = datSlot->GetInt(TEXT("containerID"));
	INT SlotID		= datSlot->GetInt(TEXT("slotID"));
	FString IconName= datSlot->GetString(TEXT("iconName"));


	if(IconName.Len()>0){
		UGFxObject* dropIcon = GetDragIcon();
		if(dropIcon)
		{
			g_displayDragDropIcon = true;
			FIntPoint pt = MMOFacade::GetMouseCursorPosition();
			dropIcon->SetPosition(pt.X , pt.Y );
			UGFxObject* uiLoader = dropIcon->GetObject( TEXT("IconLoader_mc") );
			if( uiLoader ){
				uiLoader->SetString(TEXT("source"), IconName);
			}
			dropIcon->SetVisible(TRUE);
			dropIcon->SetInt(TEXT("containerID"), E_CONSLOT_MAX);
			dropIcon->SetInt(TEXT("slotID"), -1);
			dropIcon->SetInt(TEXT("SlotType"), ESPType_Invalid);

			UGFxObject* uiGasIcon = dropIcon->GetObject( TEXT("gasIcon_mc") );
			if(uiGasIcon){
				uiGasIcon->SetVisible((false));
			}
		}
		MMOFacade::ShowMouseCursor(FALSE);
	}
}

void UMMOGFXPlayer::ASSlot_MouseLeftClick(class UGFxObject* mcSlot)
{
	if(mcSlot==NULL)return;
	INT SlotType	= mcSlot->GetInt(TEXT("slotType"));
	INT ContainerID = mcSlot->GetInt(TEXT("containerID"));
	INT SlotID		= mcSlot->GetInt(TEXT("slotID"));
	UBOOL bDefault  = FALSE;
	MouseLeftClickItem(mcSlot, SlotType, ContainerID, SlotID, bDefault);
}

void UMMOGFXPlayer::ASSlot_ReleaseDrag(class UGFxObject* mcSlotFrom, class UGFxObject* mcSlotTo)
{
	MMOFacade::getSingleton().GameCaptureInput = false; // oscar@snda

	if(mcSlotFrom)
	{
		BYTE srcToContainerID	= mcSlotFrom ? mcSlotFrom->GetInt(TEXT("containerID")) : E_CONSLOT_MAX;
		BYTE srcToSlotID		= mcSlotFrom ? mcSlotFrom->GetInt(TEXT("slotID")) : -1;
		PlayDragDropSound(srcToContainerID, srcToSlotID, EActionSound_MousePutdown);

		BYTE ToContainerID	= mcSlotTo ? mcSlotTo->GetInt(TEXT("containerID")) : E_CONSLOT_MAX;
		BYTE ToSlotID		= mcSlotTo ? mcSlotTo->GetInt(TEXT("slotID")) : -1;
		BYTE ToSlotType		= mcSlotTo ? (ESlotPanelType)mcSlotTo->GetInt(TEXT("slotType")) : ESPType_Invalid;


		if(ToSlotType == ESPType_Invalid) 
		{
			OnDragDrop(mcSlotFrom, mcSlotTo, mcSlotFrom->GetInt(TEXT("slotType")), mcSlotFrom->GetInt(TEXT("containerID")), mcSlotFrom->GetInt(TEXT("slotID")), ESPType_Invalid, E_CONSLOT_MAX, -1 );
		}
		else
		{
			OnDragDrop(mcSlotFrom, mcSlotTo, mcSlotFrom->GetInt(TEXT("slotType")), mcSlotFrom->GetInt(TEXT("containerID")), mcSlotFrom->GetInt(TEXT("slotID")), 
											   mcSlotTo->GetInt(TEXT("slotType")),   mcSlotTo->GetInt(TEXT("containerID")),   mcSlotTo->GetInt(TEXT("slotID")) );
		}
	}

	UGFxObject* dropIcon = GetDragIcon();
	if(dropIcon != NULL){
		g_displayDragDropIcon = FALSE;
		dropIcon->SetVisible(FALSE);
	}
	//MMOUI::GetUIManager()->SetAutoSearchSlot(FALSE);
	MMOUI::GetUIManager()->CloseAllHintForDragging();
	MMOFacade::ShowMouseCursor(TRUE);
}


void UMMOGFXPlayer::ASSlot_MouseRightClick(class UGFxObject* mcSlot)
{
	if(mcSlot==NULL)return;
	UGFxObject* datSlot = mcSlot->GetObject(TEXT("SlotData"));
	if(datSlot==NULL)return;
	INT SlotType	= datSlot->GetInt(TEXT("slotType"));
	INT ContainerID = datSlot->GetInt(TEXT("containerID"));
	INT SlotID		= datSlot->GetInt(TEXT("slotID"));
	UBOOL bDefault  = FALSE;


	//测试
	UGFxObject* renderSlot = mcSlot->GetObject(TEXT("SlotRender"));
	if(renderSlot)
	{
		UGFxObject* renderIcon = renderSlot->GetObject(TEXT("IconLoader_mc"));
		if(renderIcon)
		{
			FString cs = renderIcon->GetString(TEXT("source"));
		}
	}
	


	MouseRightClickItem(mcSlot, SlotType, ContainerID, SlotID, bDefault);
}

//--------------------------------------------------------------------------------------------------------------------



void UMMOGFXPlayer::MouseLeftPressItem(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	TScriptInterface<IIItemSlotActionListener> curLsn = MMOUI::GetUIManager()->GetCurrentItemSlotActionListener();
	if((UBOOL)curLsn && curLsn->OnLeftMousePress(mc, SlotType, ContainerID, SlotID, bDefault))	
	{
		return;
	}

	UBOOL res = FALSE;
	if(!g_PanelList.empty())
	{
		PanelList panelList = g_PanelList;
		//先执行普通层，再执行默认层
		for(int i=0;i<2;i++)
		{
			UBOOL bDefault = i > 0;

			PanelList::reverse_iterator iter = panelList.rbegin();
			for(;iter!=panelList.rend(); ++iter)
			{
				UMMOUIBase* Panel = MMOUI::GetUIManager()->FindUIPanel(FName(**iter));
				if(Panel)
				{
					UMMOUIBaseSlotContainer* pSlotContainer = Cast<UMMOUIBaseSlotContainer>(Panel);
					if(	pSlotContainer )
					{
						res = pSlotContainer->OnLeftMousePress(mc, SlotType, ContainerID, SlotID, bDefault);
						if(res)
						{
							return;
						}
					}
				}
			}
		}
	}
}



void UMMOGFXPlayer::ASDoubleClickSlot(class UGFxObject* SrcSlotMC,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID)
{
	TScriptInterface<IIItemSlotActionListener> curLsn = MMOUI::GetUIManager()->GetCurrentItemSlotActionListener();
	if((UBOOL)curLsn && curLsn->OnDoubleClickSlot(SrcSlotMC, SrcSlotType, SrcContainerID, SrcSlotID, FALSE))	
	{
		return;
	}

	UBOOL res = FALSE;
	if(!g_PanelList.empty())
	{
		PanelList panelList = g_PanelList;
		//先执行普通层，再执行默认层
		for(int i = 0; i < 2; i++)
		{
			UBOOL bDefault = i > 0;
			PanelList::reverse_iterator iter = panelList.rbegin();
			for(;
				iter != panelList.rend();
				++iter)
			{
				UMMOUIBase* Panel = MMOUI::GetUIManager()->FindUIPanel(FName(**iter));
				if(Panel)
				{
					UMMOUIBaseSlotContainer* pSlotContainer = Cast<UMMOUIBaseSlotContainer>(Panel);
					if(	pSlotContainer )
					{
						res = pSlotContainer->OnDoubleClickSlot(SrcSlotMC, SrcSlotType, SrcContainerID, SrcSlotID, bDefault);
						if(res)
						{
							return;
						}
					}
				}
			}
		}
	}
}

void UMMOGFXPlayer::MouseLeftClickItem(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	TScriptInterface<IIItemSlotActionListener> curLsn = MMOUI::GetUIManager()->GetCurrentItemSlotActionListener();
	if((UBOOL)curLsn && curLsn->OnLeftMouseClick(mc, SlotType, ContainerID, SlotID, bDefault))	
	{
		return;
	}


	UBOOL res = FALSE;
	if(!g_PanelList.empty())
	{
		PanelList panelList = g_PanelList;
		//先执行普通层，再执行默认层
		for(int i = 0; i < 2; i++)
		{
			UBOOL bDefault = i > 0;

			PanelList::reverse_iterator iter = panelList.rbegin();
			for(;iter!=panelList.rend(); ++iter)
			{
				UMMOUIBase* Panel = MMOUI::GetUIManager()->FindUIPanel(FName(**iter));
				if(Panel)
				{
					UMMOUIBaseSlotContainer* pSlotContainer = Cast<UMMOUIBaseSlotContainer>(Panel);
					if(	pSlotContainer )
					{
						res =pSlotContainer->OnLeftMouseClick(mc, SlotType, ContainerID, SlotID, bDefault);
						if(res)
						{
							return;
						}
					}
				}
			}
		}
	}
}
void UMMOGFXPlayer::MouseRightClickItem(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	PlayDragDropSound(ContainerID, SlotID, EActionSound_MousePutdown);

	////按住Shift右击的特殊处理，没处去就放这吧
	//UBOOL isOperated = MMOEquipManager::getSingleton().ShortcutToPutIntoUIPanel( SlotType, ContainerID, SlotID );
	//if( isOperated == TRUE ){
	//	return;
	//}

	TScriptInterface<IIItemSlotActionListener> curLsn = MMOUI::GetUIManager()->GetCurrentItemSlotActionListener();
	if((UBOOL)curLsn && curLsn->OnRightMouseClick(mc, SlotType, ContainerID, SlotID, bDefault))	
	{
		return;
	}

	UBOOL res = FALSE;
	if(!g_PanelList.empty())
	{
		PanelList panelList = g_PanelList;
		//先执行普通层，再执行默认层
		for(int i = 0; i < 2; i++)
		{
			UBOOL bDefault = i > 0;
			
			PanelList::reverse_iterator iter = panelList.rbegin();
			for(;iter!=panelList.rend(); ++iter)
			{
				UMMOUIBase* Panel = MMOUI::GetUIManager()->FindUIPanel(FName(**iter));
				if(Panel)
				{
					UMMOUIBaseSlotContainer* pSlotContainer = Cast<UMMOUIBaseSlotContainer>(Panel);
					if(	pSlotContainer )
					{
						res =pSlotContainer->OnRightMouseClick(mc, SlotType, ContainerID, SlotID, bDefault );
						if(res)
						{
							return;
						}
					}
				}
			}
		}
	}

	////default处理
	//if(res==FALSE)
	//{
	//	UMMOUIBase* Panel = MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_Character);
	//	if(Panel)
	//	{
	//		UMMOUIBaseSlotContainer* pSlotContainer = Cast<UMMOUIBaseSlotContainer>(Panel);
	//		if(pSlotContainer){
	//			pSlotContainer->OnRightMouseClick(mc, SlotType, ContainerID, SlotID, TRUE);
	//		}
	//	}
	//}

}


void UMMOGFXPlayer::MouseRightClickBuff(class UGFxObject* mc, INT curBuffID, INT curBuffSlotID)
{
	BuffData* pBuffData =  MMOFacade::getSingleton().GetMMOBuffManager()->GetBuffExBySlot(curBuffSlotID);
	if(pBuffData && pBuffData->Buff && pBuffData->Buff->TemplateID == curBuffID && pBuffData->Buff->CanCancel){
		CMessageHandler::GetInstance().Send_CancelAura(pBuffData->Id);
		return;
	}

	pBuffData =  MMOFacade::getSingleton().GetMMOBuffManager()->GetBuffBySlot(curBuffSlotID);
	if(pBuffData && pBuffData->Buff && pBuffData->Buff->TemplateID == curBuffID && pBuffData->Buff->CanCancel){
		CMessageHandler::GetInstance().Send_CancelAura(pBuffData->Id);
		return;
	}
}

void UMMOGFXPlayer::ASStartDrag(class UGFxObject* SrcItemSlot,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID)
{
	MMOFacade::getSingleton().GameCaptureInput = true; // oscar@snda

	FString iconImage;
	//heaven gasItem return
	UItem* itemTemp = NULL;
	UMMOAbility *skill = NULL;

	//是否是技能
	switch( SrcSlotType )
	{
	case ESPType_SkillSetting:
	case ESPType_SkillTree:
		{
			INT uniqueSkillID = SrcItemSlot->GetInt(TEXT("itemIndex"));
			skill = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
			if( skill != NULL ){
				iconImage = FString::Printf(TEXT("img://%s"), *skill->GetAbilityIconName());
			}
		}
		break;
	case ESPType_SkillBar:
		{
			if( SrcItemSlot == NULL ){
				break;
			}

			INT skillBarContainType = SrcItemSlot->GetInt( TEXT("EquipID") );
			if( skillBarContainType == ESBarSlotTp_Skill ){
				INT uniqueSkillID = SrcItemSlot->GetInt(TEXT("itemIndex"));

				UMMOAbility *pAbility = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
				if( pAbility != NULL ){
					iconImage = FString::Printf( TEXT("img://%s"), *pAbility->GetAbilityIconName() );
				}
			}
		}
		break;
	default:
		{
			//是否是物品
			AMMOActor* mainPlayer = MMOFacade::getSingleton().GetMainActor();
			itemTemp = GetItemTemplateFromSlot(SrcItemSlot, SrcSlotType, SrcContainerID, SrcSlotID);
			if(itemTemp)
			{
				//
				FString iconPath = MMOFacade::getSingleton().GetItemIconNamePersonalized( itemTemp );

				iconImage = FString::Printf(TEXT("img://%s"), *iconPath);

				UGamePlayDataManager::GetSingleton()->PlayItemActionSounds(mainPlayer, itemTemp->SoundActionType, EActionSound_MousePickup);

				//MMOUI::GetUIManager()->SetAutoSearchSlot(TRUE);
			}
		}
		break;
	}

	if(itemTemp || skill)
	{
		UGFxObject* dropIcon = GetDragIcon();
		if(dropIcon)
		{
			g_displayDragDropIcon = true;
			FIntPoint pt = MMOFacade::GetMouseCursorPosition();
			//INT w = dropIcon->GetInt(TEXT("width")) / 2;
			//INT h = dropIcon->GetInt(TEXT("height")) / 2;
			dropIcon->SetPosition(pt.X /*- w*/, pt.Y /*- h*/ );
			UGFxObject* uiLoader = dropIcon->GetObject( TEXT("IconLoader_mc") );
			if( uiLoader )
				uiLoader->SetString(TEXT("source"), iconImage);
			dropIcon->SetVisible(true);
			dropIcon->SetInt(TEXT("containerID"), E_CONSLOT_MAX);
			dropIcon->SetInt(TEXT("slotID"), -1);
			dropIcon->SetInt(TEXT("SlotType"), ESPType_Invalid);

			UGFxObject* uiGasIcon = dropIcon->GetObject( TEXT("gasIcon_mc") );
			if(uiGasIcon)
				uiGasIcon->SetVisible((false));
			//MMOUI::GetUIManager()->GetCommonUIComponentMoviePlayer()->eventCallTraceAllChild();
		}

		MMOFacade::ShowMouseCursor(FALSE);
	}
}

void UMMOGFXPlayer::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{

	if(!g_PanelList.empty())
	{
		PanelList panelList = g_PanelList;
		PanelList::reverse_iterator iter = panelList.rbegin();
		for(;iter!=panelList.rend(); ++iter)
		{
			UMMOUIBase* Panel = MMOUI::GetUIManager()->FindUIPanel(FName(**iter));
			if(Panel)
			{
				UMMOUIBaseSlotContainer* pSlotContainer = Cast<UMMOUIBaseSlotContainer>(Panel);
				if(	pSlotContainer )
				{
					UBOOL res = pSlotContainer->OnDragDrop(SrcSlotMC, DestSlotMC, SrcSlotType, SrcContainerID, SrcSlotID, DestSlotType, DestContainerID, DestSlotID);
					if(res)
					{
						MMOUI::GetUIManager()->delegateHideTips();
						break;
					}
				}
			}
		}
	}
}




void UMMOGFXPlayer::ASDragOver(class UGFxObject* SrcItemSlot,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID)
{
	if(g_displayDragDropIcon)
	{
		g_TopmostItemSlot = SrcItemSlot;
	}
}

void UMMOGFXPlayer::ASDragOut(class UGFxObject* SrcItemSlot,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID)
{
	if(g_TopmostItemSlot && g_TopmostItemSlot==SrcItemSlot)
	{
		g_TopmostItemSlot = NULL;
	}
}

void UMMOGFXPlayer::ASGetMouseTopMostEntity(class UGFxObject* TopMostObject)
{
	MMOFacade::getSingleton().TopMostObject = TopMostObject;
}

void UMMOGFXPlayer::ASDragRelease(class UGFxObject* SrcItemSlot,	BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID)
{
	MMOFacade::getSingleton().GameCaptureInput = false; // oscar@snda

	PlayDragDropSound(SrcContainerID, SrcSlotID, EActionSound_MousePutdown);

	class UGFxObject* TopmostItemSlot = g_TopmostItemSlot;//MMOFacade::getSingleton().GetMouseTopMostEntity();
	if(TopmostItemSlot==NULL)
	{
		TopmostItemSlot = MMOFacade::getSingleton().GetMouseTopMostEntity();
	}
	UMMOGFXPlayer* mmoGfxPlayer = 0;

	if(TopmostItemSlot)
	{
		UPackage* pPack =TopmostItemSlot->GetOutermost();
		UGFxMoviePlayer* pPlayer = TopmostItemSlot->GetOuterUGFxMoviePlayer();
		mmoGfxPlayer = Cast<UMMOGFXPlayer>(pPlayer);

		FString cs = TopmostItemSlot->GetFullName();
	}

	BYTE TopmostContainerID = TopmostItemSlot ? TopmostItemSlot->GetInt(TEXT("containerID")) : E_CONSLOT_MAX;
	BYTE TopmostSlotID = TopmostItemSlot ? TopmostItemSlot->GetInt(TEXT("slotID")) : -1;
	BYTE TopmostSlotType = TopmostItemSlot ? (ESlotPanelType)TopmostItemSlot->GetInt(TEXT("SlotType")) : ESPType_Invalid;

	NativeASDragRelease(SrcItemSlot, SrcSlotType, SrcContainerID, SrcSlotID);
	UGFxObject* dropIcon = GetDragIcon();
	if(dropIcon != NULL)
		dropIcon->SetVisible(false);
	//GetDragIcon()->SetVisible(false);

	//
	MMOUI::GetUIManager()->CloseAllHintForDragging();

	if( TopmostItemSlot )
	{
		FString itemType = TopmostItemSlot->GetString(TEXT("itemtype"));
		if( itemType != TEXT("itemslot") )
		{
			return;
		}
	}

	//`log("================ ASDragRelease in UC");
	if(TopmostSlotType == ESPType_Invalid) 
	{
		// 先让最上层的面板处理下, 还要想想是否有更好的方式处理
		UBOOL proced = FALSE;
		if(mmoGfxPlayer)
		{
			proced = mmoGfxPlayer->OnDragDropOnPanel(SrcItemSlot, TopmostItemSlot, SrcSlotType, SrcContainerID, SrcSlotID, ESPType_Invalid, E_CONSLOT_MAX, -1);
		}
		
		if(!proced)
		{
			OnDragDrop(SrcItemSlot, TopmostItemSlot, SrcSlotType, SrcContainerID, SrcSlotID, ESPType_Invalid, E_CONSLOT_MAX, -1);
		}
	}
	else
	{
		OnDragDrop(SrcItemSlot, TopmostItemSlot, SrcSlotType, SrcContainerID, SrcSlotID, TopmostSlotType, TopmostContainerID, TopmostSlotID);
		//PlayDragDropSound(TopmostItemSlot, TopmostSlotType, TopmostContainerID, TopmostSlotID, EActionSound_MousePutdown);
	}

	//MMOUI::GetUIManager()->SetAutoSearchSlot(FALSE);
}

UBOOL UMMOGFXPlayer::OnDragDropOnPanel(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	return FALSE;
}
//
//void UMMOGFXPlayer::ASDragRelease(class UGFxObject* SrcItemSlot,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,class UGFxObject* TopmostItemSlot,BYTE TopmostSlotType,BYTE TopmostContainerID,INT TopmostSlotID)
//{
// //	GFx::Value *v = ( GFx::Value* ) TopmostItemSlot->Value;
// //	GFx::Value::DisplayInfo info;
// //	v->GetDisplayInfo(&info);
//	//warnf(NAME_Warning, TEXT("___ASDragRelease:%s"), v->pObjectInterface);
//	if(TopmostItemSlot)
//	{
//		 UPackage* pPack =TopmostItemSlot->GetOutermost();
//		 UGFxMoviePlayer* pPlayer = TopmostItemSlot->GetOuterUGFxMoviePlayer();
//
//		 FString cs = TopmostItemSlot->GetFullName();
//	}
//
//#ifdef _DEBUG
//	if( SrcItemSlot && TopmostItemSlot )
//	{
//		GFx::Value* vSrc = (GFx::Value*)&SrcItemSlot->Value;
//		GFx::Value* vDest = (GFx::Value*)&TopmostItemSlot->Value;
//
//		warnf(NAME_Warning, TEXT("===--%s===%s==="), 
//			UTF8_TO_TCHAR(vSrc->ToString().ToCStr()),
//			UTF8_TO_TCHAR(vDest->ToString().ToCStr()));
//	}
//
//#endif
//
//
//
//	//TopmostContainerID = TopmostItemSlot ? (ECONTAINER_SLOT)TopmostItemSlot->GetInt(TEXT("containerID")) : E_CONSLOT_MAX;
//	//FString topName = TopmostItemSlot->GetString(TEXT("name"));
//	TopmostContainerID = TopmostItemSlot ? TopmostItemSlot->GetInt(TEXT("containerID")) : E_CONSLOT_MAX;
//	TopmostSlotID = TopmostItemSlot ? TopmostItemSlot->GetInt(TEXT("slotID")) : -1;
//	TopmostSlotType = TopmostItemSlot ? (ESlotPanelType)TopmostItemSlot->GetInt(TEXT("SlotType")) : ESPType_Invalid;
//
//	NativeASDragRelease(SrcItemSlot, SrcSlotType, SrcContainerID, SrcSlotID);
//	UGFxObject* dropIcon = GetDragIcon();
//	if(dropIcon != NULL)
//		dropIcon->SetVisible(false);
//	//GetDragIcon()->SetVisible(false);
//
//	if( TopmostItemSlot )
//	{
//		FString itemType = TopmostItemSlot->GetString(TEXT("itemtype"));
//		if( itemType != TEXT("itemslot") )
//		{
//			return;
//		}
//	}
//
//	//`log("================ ASDragRelease in UC");
//	if(TopmostSlotType == ESPType_Invalid) 
//	{
//		OnDragDrop(SrcItemSlot, TopmostItemSlot, SrcSlotType, SrcContainerID, SrcSlotID, ESPType_Invalid, E_CONSLOT_MAX, -1);
//	}
//	else
//	{
//		OnDragDrop(SrcItemSlot, TopmostItemSlot, SrcSlotType, SrcContainerID, SrcSlotID, TopmostSlotType, TopmostContainerID, TopmostSlotID);
//		//PlayDragDropSound(TopmostItemSlot, TopmostSlotType, TopmostContainerID, TopmostSlotID, EActionSound_MousePutdown);
//	}
//
//	MMOUI::GetUIManager()->SetAutoSearchSlot(FALSE);
//
//	//PlayDragDropSound(SrcItemSlot, SrcSlotType, SrcContainerID, SrcSlotID, EActionSound_MousePutdown);
//}
//------------------------------------------------------------------------
extern INT MMOUIWingPanel_ResolveHoverWing(class UGFxObject* mc, BYTE SlotType, BYTE ContainerID, INT SlotID);	// MMOUIWingPanel.cpp
void UMMOGFXPlayer::ASRollOverItemSlot(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	{
		// UnrealWing window: list items and the three wing slots show the item tip of the wing item
		const INT WingTip = MMOUIWingPanel_ResolveHoverWing(mc, SlotType, ContainerID, SlotID);
		if(WingTip != 0)
		{
			if(WingTip > 0)
			{
				mc->SetInt(TEXT("ItemTemplateID"), WingTip);
				MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_AwardIcon, mc);
			}
			return;
		}
	}
	//MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_ItemSlot, mc);

    //使用ItemSlot的面板很多 
    //对应的类型可能是Item 可能是Skill Rune Active Avarta Passive 
    INT TipsType = TFT_ItemSlot;
    switch ( SlotType )
    {
        //内部一些具体扩展 再到内部去做
    case ESPType_Bag:	
    case ESPType_Shop:
    case ESPType_ShopSell:
    case ESPType_Warehouse:	
    case ESPType_StallSetting:
    case ESPType_Enhance:
    case ESPType_TradeP2P:
	case ESPType_Quest:
	case ESPType_Achievement:
	case ESPType_SoulEmbed:
	case ESPType_SoulLevelUp:
	case ESPType_SoulUnEmbed:
	case ESPType_ReadMail:
	case ESPType_EquipGainHintBox:
	case ESPType_ItemCollection:
	case ESPType_AuctionItem:
	case ESPType_TeamAuctionPanel:
	case ESPType_TeamAuctionList:
	case ESPType_BagResource:
        TipsType = TFT_ItemSlot;
        break;
    case ESPType_SkillPanel:
		TipsType = TFT_SkillEquipped;
		break;
	case ESPType_Charactor:
	case ESPType_ViewProfile:
	case ESPType_Recover:
		{
			INT tempID = mc->GetInt(TEXT("ItemTemplateID"));
			if( tempID <= 0 )
				tempID = mc->GetInt(TEXT("itemIndex"));
			if( tempID < 0 )
			{
				TipsType = TFT_PlainText;				
			}
			else
			{
				TipsType = TFT_ItemSlot;
			}
			break;
		}
	//case ESPType_WeaponSkillSetting:
	//	{
	//		if( mc != NULL ){
	//			INT slotType = mc->GetInt( TEXT("EquipID") );
	//			if( slotType == EWpSkillSetting_SkillLeveUpItem ){
	//				TipsType = TFT_ItemSlot;
	//			}
	//			else if( slotType == EWpSkillSetting_Skill ){
	//				TipsType = TFT_SkillOfMyWithNext;
	//			}
	//		}
	//	}
	//	break;
	case ESPType_WeaponSkill:
		{
			INT slotSubType = mc->GetInt(TEXT("EquipID"));
			if( slotSubType == EWpSkill_WeaponOp || slotSubType == EWpSkill_WepSkillCusm1 || slotSubType == EWpSkill_WepSkillCusm2 )
			{
				TipsType = TFT_ItemSlot;
			}
			else{
				TipsType = TFT_SkillSlotTemplate;
			}
		}
		break;
	case ESPType_WeaponSkillRplc:
		{
			INT slotSubType = mc->GetInt(TEXT("EquipID"));
			if( slotSubType == EWpSklRplcSltTp_WeaponOp || slotSubType == EWpSklRplcSltTp_WpSkillCusm1 || slotSubType == EWpSklRplcSltTp_WpSkillCusm2 )
			{
				TipsType = TFT_ItemSlot;
			}
			else{
				TipsType = TFT_SkillSlotTemplate;
			}
		}
		break;
	case ESPType_SkillSetting:
	case ESPType_SkillTree:
		{
			//if( SlotID>= 0 ){
			//	TipsType = TFT_ItemSlot;
			//}
			//else{
			//	TipsType = TFT_SkillSlotTemplate;
			//}
			TipsType = TFT_SkillTemplateWithNext;
		}
		break;
	//case ESPType_SkillBar:
	//	{
	//		TipsType = TFT_SkillEquipped;

	//		INT skillMainID = mc->GetInt(TEXT("itemIndex"));
	//		MMOGameEvent_OneInt showSkillRangeEvt( GameEvent_ShowSkillRange, skillMainID );
	//		MMOUI::PostGameEvent( &showSkillRangeEvt );
	//	}
	//	break;
	case ESPType_SkillBar:
		{
			if( mc != NULL ){
				INT itemBarContainType = mc->GetInt( TEXT("EquipID") );

				if( itemBarContainType == ESBarSlotTp_Item ){
					TipsType = TFT_ItemSlot;
				}
				else if( itemBarContainType == ESBarSlotTp_Skill ){
					TipsType = TFT_SkillEquipped;

					INT uniqueSkillID = mc->GetInt(TEXT("itemIndex"));
					UINT skillID;
					UINT skillLv;
					UINT skillRuneID;
					MMOSkillManager::DecomposeAbilityID( uniqueSkillID, skillID, skillLv, skillRuneID );
					MMOGameEvent_OneInt showSkillRangeEvt( GameEvent_ShowSkillRange, skillID );
					MMOUI::PostGameEvent( &showSkillRangeEvt );
				}
				else if( itemBarContainType == ESBarSlotTp_Mount ){
					TipsType = TFT_MountSlot;
				}
			}
			//TipsType = TFT_ItemBar;
		}
		break;
	case ESPType_FBAwards:
	case ESPType_PVPBattle:
		TipsType = TFT_AwardIcon;
		break;
	case ESPType_Mount:
		TipsType = TFT_MountSlot;// TFT_PlainText;
		break;
	case ESPType_HeavenGasEquip:
	case ESPType_HeavenGasBag:
	case ESPType_HeavenGasBank:
		 TipsType = TFT_HeavenGasSlot;
		 break;
	//case ESPType_Recover:
	//	break;
	case ESPType_Buff:
		TipsType = TFT_BuffSlot;
		break;
	
    default:
        TipsType = TFT_ItemSlot;
        break;
    }   

    if( mc->GetBool( "isSlotLocked") )
    {
          TipsType = TFT_LockedSlot;
    }




    MMOUI::GetUIManager()->delegateShowTipsHTML( TipsType, mc);
}

void UMMOGFXPlayer::ASRollOutItemSlot(class UGFxObject* mc)
{
	if(g_TopmostItemSlot && g_TopmostItemSlot==mc)
	{
		g_TopmostItemSlot = NULL;
	}
	MMOUI::GetUIManager()->delegateHideTips();
}

//------------------------------------------------------------------------
void UMMOGFXPlayer::ASShowTips_Tab(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_Tab, mc);
}

void UMMOGFXPlayer::ASHideTips_Tab(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOGFXPlayer::ASShowTips_Attribute(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_Attribute, mc);
}

void UMMOGFXPlayer::ASHideTips_Attribute()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOGFXPlayer::ASShowTips_OrderItem(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_GoldStore, mc);
}

void UMMOGFXPlayer::ASHideTips_OrderItem()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//------------------------------------------------------------------------
void UMMOGFXPlayer::ASShowTips_MenuBar(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_MenuBar, mc);
}

void UMMOGFXPlayer::ASHideTips_MenuBar(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//------------------------------------------------------------------------
void UMMOGFXPlayer::ASShowTips_AwardIcon(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_AwardIcon, mc);
}

void UMMOGFXPlayer::ASHideTips_AwardIcon(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//------------------------------------------------------------------------
void UMMOGFXPlayer::ASShowTips_Other(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_Other, mc);
}

void UMMOGFXPlayer::ASHideTips_Other(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//------------------------------------------------------------------------
void UMMOGFXPlayer::ASShowTips_BuffSlot(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_BuffSlot, mc);
	FSMIO::getSingleton().Trigger.SetShowTips(TRUE,TRUE);
}

void UMMOGFXPlayer::ASHideTips_BuffSlot(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//------------------------------------------------------------------------
void UMMOGFXPlayer::ASShowTips_Building(class UGFxObject* mc, INT buildingType)
{
	if (mc)
	{
		UINT32 buildingLv = MMOGuildData::getSingleton().GetGuildBuildingMaxLevel(buildingType);
		GuildBuildingData buildingData;
		if (MMOGuildData::getSingleton().GetGuildBuildingData(buildingType, buildingLv > 0 ? buildingLv : 1, buildingData))
		{
			mc->SetString(TEXT("title"), buildingData.Name);
			mc->SetString(TEXT("text"),	buildingData.Description);
		}
	}
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_FreeStyle, mc);
}

void UMMOGFXPlayer::ASHideTips_Building(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//------------------------------------------------------------------------
void UMMOGFXPlayer::ASShowTips_RuneActive(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_RuneActive, mc);
}

void UMMOGFXPlayer::ASHideTips_RuneActive(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//------------------------------------------------------------------------
void UMMOGFXPlayer::ASShowTips_RuneButton(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_RuneButton, mc);
}

void UMMOGFXPlayer::ASHideTips_RuneButton(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//------------------------------------------------------------------------
void UMMOGFXPlayer::ASShowTips_AvartaRuneButton(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_AvartaRuneButton, mc);
}

void UMMOGFXPlayer::ASHideTips_AvartaRuneButton(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//------------------------------------------------------------------------
void UMMOGFXPlayer::ASShowTips_PlainText(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_PlainText, mc);
}

void UMMOGFXPlayer::ASHideTips_PlainText(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOGFXPlayer::ASShowTips_Text(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_NormalText, mc);
}

void UMMOGFXPlayer::ASShowReputionPanel(UBOOL bShow)
{
	MMOUI::GetUIManager()->ToggleShowPanelEx(MMOUI::GetUIManager()->Panel_JourneyView, "");
}

void UMMOGFXPlayer::ASHideTips_Text(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOGFXPlayer::ASShowTips_PlainText_WithString(class UGFxObject* mc , const FString& TipsString)
{
	if(mc)mc->SetString("tipsHtml", TipsString);
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_PlainText, mc);
}

void UMMOGFXPlayer::ASShowFreeTips(class UGFxObject* mc , UBOOL bShow)
{
	if(mc){
		if(bShow){
			UGFxObject* mcTips = mc->GetObject("FreeTipsObj");
			if(mcTips){
				FString cs = mcTips->GetString("tipsString");
				FString tp = mcTips->GetString("tipsType");
				ASShowTips_PlainText_WithString(mc,cs);
			}
		}
		else
		{
			ASHideTips_PlainText(mc);
		}
	}

}
//------------------------------------------------------------------------


void UMMOGFXPlayer::NativeASDragRelease(class UGFxObject* SrcItemSlot,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID)
{
	g_displayDragDropIcon = false;
	MMOFacade::ShowMouseCursor(TRUE);
}
//------------------------------------------------------------------------

void UMMOGFXPlayer::ClosePanel(class UGFxObject* Panel)
{
	if(Panel)
	{
		UMMOUIBase* thePanel = Cast<UMMOUIBase>(Panel);
		if(thePanel)
		{
			thePanel->Show(FALSE);
		}
	}
}

void UMMOGFXPlayer::ASOnBringWindowTop(const FString& PanelName)
{
	UMMOUIBase* Panel = MMOUI::GetUIManager()->FindUIPanel( FName(*PanelName)  );
	if( Panel )
	{
		MMOUI::MoveToTop(Panel);
	}	
}



//------------------------------------------------------------------------
class UMMOUIBase* UMMOGFXPlayer::FindUIPanel(FName inFriendlyName)
{
	return MMOUI::FindUIPanel(inFriendlyName);
}
//------------------------------------------------------------------------------
void UMMOGFXPlayer::SetItemSlotData(class UGFxObject* mc,class UItem* tItem,BYTE SlotType,BYTE ContainerID,INT SlotID, BYTE  inISS )
{
	if( !mc )
	{
		return;
	}

	// TODO: 检查这个是不是加在这里合适
// 	for (INT i = 0; i < UIObjectList.Num(); ++i)
// 	{
// 		UMMOUIBase* pUI = UIObjectList(i);
// 		pUI->SetItemSlotData(mc, tItem, SlotType, ContainerID, SlotID, inISS);
// 	}

//--add by jimZhang 防止同item重复刷新---------------------------------------------------------
#ifdef UI_OPTIMIZATION
	
	if (ContainerID != E_CONSLOT_RESOURCE)
	{
		INT CRC32_Cur  = tItem==NULL?88888+inISS:crc32(0, (const Bytef*)tItem, sizeof(UItem));

		if (tItem && tItem->ItemDuration != E_DURTYPE_EQUIP)
		{
			FClientItemInfo* itemInfo  = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
			if (itemInfo)
				CRC32_Cur += itemInfo->m_nCurDur;
		}

		INT CRC32_Last = mc->GetInt("CRC32");
		if(CRC32_Cur==CRC32_Last)
		{
			return;
		}
		else
		{
			mc->SetInt("CRC32",CRC32_Cur);
		}
	}
	
#endif
//---------------------------------------------------------------------------------------------

    mc->SetBool("isSlotLocked", FALSE );
    mc->SetBool("isSlotUnusable", FALSE );


    //置于Loader之上
    if( ISS_ItemLocked == inISS || ISS_SlotLocked == inISS )
    {
        mc->SetBool("isSlotLocked", TRUE );
    }
    else if( ISS_Unusable == inISS)
    {
       mc->SetBool("isSlotUnusable", TRUE );
    }

    //直接替换Loader
	if( ISS_SlotLocked == inISS || !tItem )
	{
		eventCallSetItemData( mc , 0 , TEXT("") , -1, SlotType, TEXT("") , ITEMCOLOR_INVALID , ContainerID , SlotID );
        return;
	}
	else
	{
		AMMOActor* mainActor = MMOFacade::getSingleton().GetMainActor();
		if( mainActor== NULL )
		{
			warnf(NAME_Error, TEXT("Main Actor is null @ UMMOGFXPlayer::SetItemSlotData!"));
		}
		
        //有iteminfo为本地商品
        FClientItemInfo* itemInfo = NULL;
		
		if( SlotType == ESPType_ShopSell )
		{
			//回购商品
			itemInfo = (FClientItemInfo*)MMOFacade::getSingleton().FindSoldItem(SlotID);
		}
		else if ( SlotType == ESPType_GuildWarehouse )
		{
			UMMOUIGuildWarehouse* panel = (UMMOUIGuildWarehouse*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_Warehouse);
			if (panel && panel->IsVisible)
			{
				itemInfo = MMOGuildData::getSingleton().GetWarehouseItem(panel->_CurPage, SlotID);
			}
		}
		else
		{
			itemInfo = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
		}
		
		UBOOL useLargeIcon = mc->GetBool(TEXT("useLargeIcon"));
		FString iconPath;
		if( useLargeIcon )
		{
			if(mainActor)
			{
				iconPath = tItem->GetLargeIconName(mainActor->GetSex() == EGT_Male, mainActor->GetClassId());
			}
		}
		else
		{
			iconPath = MMOFacade::getSingleton().GetItemIconNamePersonalized( tItem, itemInfo );
		}

		switch( tItem->ItemDuration )
		{
		case E_DURTYPE_STACK:
			{
				if( itemInfo )
				{ 
					//摆摊时 如果当前物品已经上架
					if( itemInfo->IsOnSale && ESPType_StallSetting == SlotType )
					{
						eventCallSetItemData( mc , itemInfo->OnSaleCount , iconPath , tItem->ItemId , SlotType, TEXT("") , tItem->ItemColor + 1, ContainerID, SlotID );
					}
                    //灵魂被拖入灵魂融合界面
                    else if( itemInfo->IsDragIntoOtherUI >= 0 && (ESPType_SoulEmbed == SlotType || ESPType_SoulLevelUp == SlotType || ESPType_SoulUnEmbed == SlotType))
                    {
                        eventCallSetItemData( mc , 1 , iconPath , tItem->ItemId , SlotType, TEXT("") , tItem->ItemColor + 1, ContainerID, SlotID );
                    }
                    //融灵时，则其他界面都不显示 
                    else if( itemInfo->IsDragIntoOtherUI >= 0 && itemInfo->m_nCurDur <= 0 ) 
                    {
						if (itemInfo->IsDragIntoOtherUI == ESPType_StallSetting)
						{// 摆摊时，背包里的图标不隐藏
							eventCallSetItemData( mc , itemInfo->m_nCurDur , iconPath , tItem->ItemId , SlotType, TEXT("") , tItem->ItemColor + 1, ContainerID, SlotID );
						}
						else
						{
							//显示空
							eventCallSetItemData( mc , 0 , TEXT("") , -1, SlotType, TEXT("") , ITEMCOLOR_INVALID, ContainerID, SlotID );
						}
                    }
                    else
                    {
					    eventCallSetItemData( mc , itemInfo->m_nCurDur , iconPath , tItem->ItemId , SlotType, TEXT("") , tItem->ItemColor + 1, ContainerID, SlotID );
                    }

					//被锁定的需要打叉
					if( itemInfo != NULL && mc != NULL ){
						mc->SetBool( TEXT("MarkLocked"), itemInfo->IsDragIntoOtherUI >= 0 );
					}
                }
				else
				{
					if( ESPType_EquipCompose == SlotType )
					{
						eventCallSetItemData( mc , 1 , iconPath  , tItem->ItemId, SlotType, TEXT("") , tItem->ItemColor + 1, ContainerID, SlotID );
					}
					else
					{
						//DefaultDuration按理很早就不能用了，设1不知道对不对，如果有bug的话，或者像前面那样用if = shop里强制？
						eventCallSetItemData( mc , 1/*tItem->DefaultDuration*/ , iconPath  , tItem->ItemId, SlotType, TEXT("") , tItem->ItemColor + 1, ContainerID, SlotID );
					}
				}
			}
			break;
		case E_DURTYPE_EQUIP:
		case E_DURTYPE_USE:
		case E_DURTYPE_NONE:
		case E_DURTYPE_DEPOSIT:
		case E_DURTYPE_RECOVER:
		case E_DURTYPE_PURITY:
			{
				// junjie@snda：IsDragIntoOtherUI在融灵界面还是需要显示图标的，只是打上一个大叉
				// zhangrui05@snda: 摆摊时，背包也要现实打叉的icon，#20879
				eventCallSetItemData( mc , 1 , iconPath , tItem->ItemId, SlotType, TEXT("") , tItem->ItemColor + 1, ContainerID, SlotID );
           
				//被锁定的需要打叉
				if (itemInfo != NULL && mc != NULL)
				{
					mc->SetBool( TEXT("MarkLocked"), itemInfo->IsDragIntoOtherUI >= 0 );
				}
			}
            break;
		default:
			{
				if(mc&&tItem)
					eventCallSetItemData( mc , 1 , iconPath , tItem->ItemId, SlotType, TEXT("") , tItem->ItemColor + 1, ContainerID, SlotID );
			}
			break;
		}

	}
}
//
void UMMOGFXPlayer::SetIconShowByItem( class UGFxObject *pSlot_mc, class UItem *pSlotItem )
{
	if( pSlotItem == NULL){
		eventASSetSlotShowData( pSlot_mc , 0 , "", -1 );
	}
	else{
		eventASSetSlotShowData( pSlot_mc , pSlotItem->DefaultDuration , MMOFacade::getSingleton().GetItemIconNamePersonalized( pSlotItem ) , pSlotItem->ItemId );
	}
}

//
void UMMOGFXPlayer::SetNoninteractiveIconSlotByItem( UGFxObject *mc, UItem *item, const FString &desc, INT panelType )
{
	if( mc == NULL || item == NULL ){
		return;
	}

	//itemID
	FASValue asItemID(EC_EventParm);
	asItemID.Type = AS_Int;
	asItemID.I = item->GetTemplateID();

	//iconPath
	FASValue asItemIcon(EC_EventParm);
	asItemIcon.Type = AS_String;
	asItemIcon.S = MMOFacade::getSingleton().GetItemIconNamePersonalized( item );

	//itemDesc
	FASValue asItemName(EC_EventParm);
	asItemName.Type = AS_String;
	asItemName.S = desc;

	//slot type
	FASValue asSlotType(EC_EventParm);
	asSlotType.Type = AS_Int;
	asSlotType.I = panelType;

	TArray<struct FASValue> asArgs;
	asArgs.AddItem( asItemID );
	asArgs.AddItem( asSlotType );
	asArgs.AddItem( asItemIcon );
	asArgs.AddItem( asItemName );

	mc->Invoke( TEXT("SetData"), asArgs );
}
//
void UMMOGFXPlayer::SetNoninteractiveIconSlot( UGFxObject *mc, INT itemID, const FString &iconPath, const FString &desc, INT panelType )
{
	if( mc == NULL ){
		return;
	}

	//itemID
	FASValue asItemID(EC_EventParm);
	asItemID.Type = AS_Int;
	asItemID.I = itemID;

	//iconPath
	FASValue asItemIcon(EC_EventParm);
	asItemIcon.Type = AS_String;
	asItemIcon.S = iconPath;

	//itemDesc
	FASValue asItemName(EC_EventParm);
	asItemName.Type = AS_String;
	asItemName.S = desc;

	//slot type
	FASValue asSlotType(EC_EventParm);
	asSlotType.Type = AS_Int;
	asSlotType.I = panelType;

	TArray<struct FASValue> asArgs;
	asArgs.AddItem( asItemID );
	asArgs.AddItem( asSlotType );
	asArgs.AddItem( asItemIcon );
	asArgs.AddItem( asItemName );

	mc->Invoke( TEXT("SetData"), asArgs );
}
//一些公用接口,便于在as3里为所欲为///////////////////////////////////////////////////////////////////////////////////////
void UMMOGFXPlayer::DebugString(const FString& str)
{
	debugf( TEXT("test"));
}

void UMMOGFXPlayer::GetCurrentMapType(class UGFxObject* mc,const FString& resName)
{
	//enum EServerMapType
	//{
	//	esmt_None               =0,
	//	esmt_Static             =1,
	//	esmt_Dungeon            =2,
	//	esmt_BattleField        =3,
	//	esmt_Arena              =4,
	//	esmt_Line               =5,
	//	esmt_MAX                =6,
	//};
	AMMOPlayerController* playerCtrl=MMOFacade::getSingleton().GetMainController();
	if(playerCtrl)
	{
		FExportMap *pMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( playerCtrl->CurMapID );
		INT mapType = E_est_Invalid;
		if( pMapInfo != NULL )
		{
			mapType = pMapInfo->ServerMapType;
		}

		if(mc)mc->SetInt(resName,mapType);
	}
}

void UMMOGFXPlayer::ASGetStringByID(class UGFxObject* mc, const FString& menberID,const FString& textID)
{
	if(mc)mc->SetString(menberID, MMOUI::ParseLocalizedPropertyPath(textID)  );
}

void UMMOGFXPlayer::SetTextByID(class UGFxObject* mc, const FString& textID)
{
	if(mc)mc->SetString(TEXT("text"), MMOUI::ParseLocalizedPropertyPath(textID)  );
}

void UMMOGFXPlayer::SetLabelByID(class UGFxObject* mc, const FString& textID)
{
	if(mc)mc->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(textID)  );
}

//显示时间
void UMMOGFXPlayer::TimeDisplay(class UGFxObject* mc, int seconds)
{
	if(mc)mc->SetString(TEXT("text"), MMOFacade::getSingleton().TimeDisplay(seconds) );
}

void UMMOGFXPlayer::GetReviveCountDown(class UGFxObject* mc, const FString& resName)
{
	FExportMap mapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	if( mapInfo.ServerMapType == E_est_BattleField ){
		//战场复活
		INT reviveTime = 30;

		FBattleTemplate* pBattle = MMOBattleManager::getSingleton().GetEnteredBattle();
		if( pBattle != NULL ){
			reviveTime = pBattle->ReviveTime;
		}

		if(mc)mc->SetInt(resName,reviveTime);
	}
	else{
		INT time = MMODeathManager::getSingleton().GetReviveTime();
		if(mc)mc->SetInt(resName,time);
	}
	
}

void UMMOGFXPlayer::GetItemCountByID(class UGFxObject* mc,const FString& resName,INT itemID)
{
	AMMOPlayerController* ctrl=MMOFacade::getSingleton().GetMainController();
	if(ctrl&&ctrl->GetPlayer())
	{
		int nCount = ctrl->GetPlayer()->ItemManager->GetItemCountById(itemID,E_CONSLOT_DEFAULT);
		if(mc)mc->SetInt(resName,nCount);
	}
}

//void UMMOGFXPlayer::GetItemIconByID(class UGFxObject* mc, const FString& resName,INT itemID )
//{
//	AMMOPlayerController* ctrl=MMOFacade::getSingleton().GetMainController();
//	if(ctrl&&ctrl->GetPlayer())
//	{
//		UItem* pItem = ctrl->GetPlayer()->ItemManager->GetItemById(itemID,E_CONSLOT_DEFAULT);
//		if(pItem&&mc)mc->SetString(resName,MMOFacade::getSingleton().GetItemIconNamePersonalized( pItem ));
//	}
//}

void UMMOGFXPlayer::GetSkillIconByID(class UGFxObject* mc, const FString& resName,INT skillID )
{
	AMMOPlayerController* ctrl=MMOFacade::getSingleton().GetMainController();
	if(ctrl&&ctrl->GetPlayer())
	{
		UMMOAbility* pAbility = ctrl->GetPlayer()->GetAbilityById(skillID, 0);
		if(pAbility&&mc)mc->SetString(resName,pAbility->GetAbilityIconName());
	}
}

void UMMOGFXPlayer::GetPlayerMoney(class UGFxObject* mc,const FString& resName)
{
	if(mc)mc->SetInt(resName, MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_01));
}

void UMMOGFXPlayer::GetPlayerGold(class UGFxObject* mc,const FString& resName)
{
	if(mc)mc->SetInt(resName, MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_02));
}

//---------------------------------------------------------------------------
void UMMOGFXPlayer::ASOnPanelClose(const FString& PanelName)
{
	debugf( _T("============================================ASOnPanelClose:%s") , *PanelName);
	MMOUI::GetUIManager()->ShowPanelEx(FName(*PanelName),FALSE, "");

	//UMMOUIBase* Panel = MMOUI::GetUIManager()->FindUIPanel( FName(*PanelName)  );
	//if( Panel && Panel->CloseOnEsc )
	//{
	//	Panel->Show( FALSE );
	//	if(Panel->IsDynamicLoad)
	//	{
	//		MMOUI::GetUIManager()->ShowPanelEx()
	//	}
	//}

	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIHelperPanel* helperPanel = (UMMOUIHelperPanel*)MMOUI::FindUIPanel(uiManager->Panel_Helper);
		if (helperPanel && helperPanel->_TargetPanelName == PanelName && helperPanel->IsVisible)
		{
			helperPanel->Show(FALSE);
		}
	}
}

//---------------------------------------------------------------------------
void UMMOGFXPlayer::ASOnPanelHelp(const FString& PanelName)
{
	warnf(NAME_Log, TEXT("[UnrealWing] ASOnPanelHelp('%s')"), *PanelName);
	if(PanelName.Left(10) == TEXT("UnrealWing"))
	{
		// the wing window has no entry in the helper table: show a short text instead of the helper panel
		UMMOUIManager* um = MMOUI::GetUIManager();
		if(um)
		{
			um->delegateAddScreenMessage(TEXT("Illusion Wings: click a wing to preview it, double-click it to wear it (or take it off), Upgrade raises the wing level."), MsgInfoType_Warning);
		}
		return;
	}
	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIHelperPanel* helperPanel = (UMMOUIHelperPanel*)MMOUI::FindUIPanelEx(uiManager->Panel_Helper);
		if (helperPanel)
		{
			helperPanel->ShowHelperPanel(PanelName);
		}
	}
}

void UMMOGFXPlayer::ASSelectTrackForceID(INT ForceID)
{
	if(MMOUI::GetUIManager()->GetTraceRelationForceID()==ForceID){
		ForceID = 0;
	}
	MMOClientVariableManager::getSingleton().SetClientVariable(ECVT_TraceRelation, ForceID);
	MMOUI::GetUIManager()->SetTraceRelationForceID(ForceID);
	MMOUI::GetUIManager()->delegateUpdateReputationData();
	
}

//------------------------------------------------------------------------
void UMMOGFXPlayer::CaptureAllKeys()
{
#define DEFINE_KEY(k, Unused) AddCaptureKey(FName(#k))
#	include "UnKeys.h"
#undef DEFINE_KEY

RemoveCaptureKey(FName("Escape"));
}

//------------------------------------------------------------------------
void UMMOGFXPlayer::CaptureKeyboardKeys()
{
	AddCaptureKey(FName("A"));
	AddCaptureKey(FName("B"));
	AddCaptureKey(FName("C"));
	AddCaptureKey(FName("D"));
	AddCaptureKey(FName("E"));
	AddCaptureKey(FName("F"));
	AddCaptureKey(FName("G"));
	AddCaptureKey(FName("H"));
	AddCaptureKey(FName("I"));
	AddCaptureKey(FName("J"));
	AddCaptureKey(FName("K"));
	AddCaptureKey(FName("L"));
	AddCaptureKey(FName("M"));
	AddCaptureKey(FName("N"));
	AddCaptureKey(FName("O"));
	AddCaptureKey(FName("P"));
	AddCaptureKey(FName("Q"));
	AddCaptureKey(FName("R"));
	AddCaptureKey(FName("S"));
	AddCaptureKey(FName("T"));
	AddCaptureKey(FName("U"));
	AddCaptureKey(FName("V"));
	AddCaptureKey(FName("W"));
	AddCaptureKey(FName("X"));
	AddCaptureKey(FName("Y"));
	AddCaptureKey(FName("Z"));
	//
	AddCaptureKey(FName("1"));
	AddCaptureKey(FName("2"));
	AddCaptureKey(FName("3"));
	AddCaptureKey(FName("4"));
	AddCaptureKey(FName("5"));
	AddCaptureKey(FName("6"));
	AddCaptureKey(FName("7"));
	AddCaptureKey(FName("8"));
	AddCaptureKey(FName("9"));

	

}

//------------------------------------------------------------------------
void UMMOGFXPlayer::OnHUDLoaded()
{

}
//------------------------------------------------------------------------

void UMMOGFXPlayer::ASLinkCallBack( class UGFxObject* paraArr , INT buttonIdx )
{
	LinkManager::getSingleton().CallBackLink(paraArr,buttonIdx);
}

void UMMOGFXPlayer::ASLinkOverCallBack(class UGFxObject* MembersArray)
{
	LinkManager::getSingleton().CallBackLinkOver(MembersArray);
}

void UMMOGFXPlayer::ASLinkOutCallBack(class UGFxObject* MembersArray)
{
	LinkManager::getSingleton().CallBackLinkOut(MembersArray);
}


void UMMOGFXPlayer::ASDebugBreak(class UGFxObject* sender)
{
#if defined(_DEBUG) && 1
	warnf(NAME_Warning, TEXT("===========[ASDebugBreak]:%s"), sender ? *sender->GetFullName() : TEXT(""));
#endif
}

// zhaojun@snda

#if defined(_WIN32)|| defined(_WIN64)

extern HWND GGameWindow;

void EnableIME(HWND hWnd)
{
	if (hWnd && ::IsWindow(hWnd))
	{
		HIMC Imc = ::ImmGetContext( hWnd );

		if( !Imc )
		{
			Imc = ::ImmCreateContext();
			if( Imc )	
			{
				::ImmAssociateContext( hWnd, Imc ); 
			}
		}
		else
		{
			::ImmReleaseContext( hWnd, Imc );
		}

		::SetFocus(hWnd);
	}
}

void DisableIME(HWND hWnd)
{
	if (hWnd && ::IsWindow(hWnd))
	{
		HIMC Imc = ImmGetContext( hWnd );
		
		if (Imc)
		{
			::ImmAssociateContext(hWnd, NULL);
		}

		::ImmReleaseContext(hWnd, Imc);

		::SetFocus(hWnd);
	}
}
#endif

UBOOL g_focusInTextInput = FALSE;

void UMMOGFXPlayer::ASFocusInTextInput(UBOOL In)
{
	g_focusInTextInput = In;

	AMMOPlayerController* Ctrl = MMOFacade::getSingleton().GetMainController();
	
	if(Ctrl != NULL)
		Ctrl->SetInTextInput(In);

#if defined(_WIN32)|| defined(_WIN64)

	if( In)
	{
		EnableIME(GGameWindow);
	}
	else
	{
		DisableIME(GGameWindow);
	}

#endif

}

//~ zhaojun@snda

void UMMOGFXPlayer::ASPlayUISound(const FString& cueName)
{
	//MMOFacade::getSingleton().PlayUISound(cueName);
}


void UMMOGFXPlayer::ASTrace(const FString& logStr)
{
#if !SHIPPING_PC_GAME
#if defined(_DEBUG) || 0
	debugf(*(FString(TEXT("====ASTrace:")) + logStr));
#endif
#endif
}

void UMMOGFXPlayer::LeaveGameWithConfirm(class UGFxObject* mc)
{
	if(MMO::GetCurrentGameState() == EGameState_InGame)
	{
		FString str = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MsgBox.text11"));
		
		UGFxObject* msgMc = MMOMsgBoxManager::getSingleton().AddModalMessageBox(
			str , 0 , FScriptDelegate(this, TEXT("LeaveGameWithoutConfirm")),
			FScriptDelegate(this, NAME_None));		
	}
	else
	{
		MMOFacade::getSingleton().GetMainController()->ConsoleCommand(TEXT("exit"));
	}
}

void UMMOGFXPlayer::SetAllChildren2DefaultVisible()
{
	for(INT i = 0; i < UIObjectList.Num(); ++i)
	{
		if(UIObjectList(i) != NULL)
			UIObjectList(i)->Show( UIObjectList(i)->DefaultVisible );
	}
}

void UMMOGFXPlayer::ShowUI(FName inFriendlyName, UBOOL inShow)
{
	UMMOUIBase* ui = MMOUI::FindUIPanel(inFriendlyName);
	if(ui != NULL)
		ui->Show(inShow);
}

void UMMOGFXPlayer::DeleteEventAdapter()
{
	if(EventAdapter)
	{
		EventAdapter->Delete();
		EventAdapter = NULL;
	}
}

UBOOL UMMOGFXPlayer::IsAllHide()
{
	for(INT i = 0; i < UIObjectList.Num(); ++i)
	{
		if(UIObjectList(i)&&UIObjectList(i)->IsVisible == TRUE)
		{
			return FALSE;
		}
	}
	return TRUE;
}

void UMMOGFXPlayer::ShowAllUI(UBOOL inShow)
{
	for(INT i = 0; i < UIObjectList.Num(); ++i)
	{
		if(UIObjectList(i) != NULL)
			UIObjectList(i)->SetVisible(inShow);
	}
}

void UMMOGFXPlayer::RegisterUI(class UMMOUIBase* UIMenu, FName inFriendlyName)
{
	warnf(NAME_DevGFxUI, TEXT("RegisterUI %s, 修改完UI后应该不会再调用到这里，注意检查代码！"), *inFriendlyName.ToString());
// 	debugf( TEXT("[UMMOGFXPlayer::RegisterUI] RegisterUI:%s "), *inFriendlyName.ToString() );
 	if( UIMenu != NULL )
 	{
		MMOUI::RegisterUIPanel(UIMenu, inFriendlyName);
 		UIMenu->eventInitLoad(this);
 		UIMenu->SetString(TEXT("PanelName"), inFriendlyName.ToString());
 		UIObjectList.AddItem(UIMenu);
 		UIMenu->eventSetDistanceTick(this);
 	}
}

//
FString UMMOGFXPlayer::GetASHeadIconFrame( BYTE playerClass, UBOOL isFemale )
{
	FString iconFrame;
	switch( playerClass )
	{
	case EPCT_PlayerClassA: iconFrame = TEXT("warrior"); break;
	case EPCT_PlayerClassB: iconFrame = TEXT("mage"); break;
	case EPCT_PlayerClassC: iconFrame = TEXT("assassin"); break;
	case EPCT_PlayerClassD: iconFrame = TEXT("archer"); break;
	case EPCT_PlayerClassE: iconFrame = TEXT("taoist"); break;
	// 1.05 classes: no dedicated flash frame in our swfs, reuse nearest (Lancer -> warrior, Sorcerer -> mage)
	case EPCT_PlayerClassG: iconFrame = TEXT("warrior"); break;
	case EPCT_PlayerClassH: iconFrame = TEXT("mage"); break;
	};

	if( isFemale && !iconFrame.IsEmpty() ){
		iconFrame += TEXT("F");
	}

	return iconFrame;
}

void UMMOGFXPlayer::SetGfxFocus(class UGFxObject* mc)
{
	if(mc==NULL)return;
	// 设置本MOVIE为焦点
	//FGFxEngine::GetEngine()->SetFocusMovie(pMovie);
	eventCallSetStageFocus(mc);
}

UBOOL UMMOGFXPlayer::SetFocusPanelByName(const FName controllerName)
{
	UMMOUIBase* pFocusUI = NULL;

	if (controllerName != NAME_None)
	{
		for (INT i = 0; i < UIObjectList.Num(); ++i)
		{
			UMMOUIBase* pUI = UIObjectList(i);

			bool foundUI = false;
			if(pUI->FriendlyName == controllerName)
			{
				foundUI = true;
			}

			if(!foundUI)
			{
				UGFxObject* pObj = pUI->GetObject(controllerName.ToString(), UGFxObject::StaticClass(), TRUE);
				foundUI = pObj != NULL;
			}

			if (foundUI)
			{
				pFocusUI = pUI;

				MMOUI::MoveToTop(pUI);

				return TRUE;
			}
		}
	}
	
	return FALSE;
}

// add by zhangrui05@snda
void UMMOGFXPlayer::ASCloseGuidePanel(INT id)
{
	// 引导事件
	MMOGameEvent_Guide evtGuide(EGUET_CloseGuidePanel, id);
	MMOUI::PostGameEvent(&evtGuide); 
}
void UMMOGFXPlayer::ASCopyToClipboard(const FString& txt)
{
	appClipboardCopy( *txt );
}

//------------------------------------------------------------------------------------------------------------------------
FString UMMOGFXPlayer::ASPasteFromClipboard()
{
	return appClipboardPaste();
}

UBOOL UMMOGFXPlayer::NativeSkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	return FALSE;//MMOFacade::getSingleton().GameCaptureInput;
}

UBOOL UMMOGFXPlayer::PanelSkipCaptureInputKey(const FString& key, BYTE eIE, const FString& strHoverControllerName, FVector2D ClickPosInMovieSpace)
{
	if(strHoverControllerName==TEXT(""))
	{
		return TRUE;
	}

	for(INT i = 0; i < UIObjectList.Num(); ++i)
	{
		UBOOL caped = UIObjectList(i)->SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
		if(caped)			
		{
			return TRUE;
		}
	}
	return FALSE;
}

