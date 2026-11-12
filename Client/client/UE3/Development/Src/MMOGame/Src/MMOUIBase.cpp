#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "FSMIO.h"
#include "MMOFreeTipsUIManager.h"
#include "MMOGuideManager.h"
#include "MMOHelperData.h"

IMPLEMENT_CLASS(UMMOUIBase)
IMPLEMENT_CLASS(UMMOUIBaseSlotContainer)
//extern bool g_lockMouseCursorForUI;

void UMMOUIBase::OnNativeGameEvent(struct MMOGameEvent* evt)
{

}

void UMMOUIBaseSlotContainer::ClearUI()
{
	
}


UBOOL UMMOUIBaseSlotContainer::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
    return FALSE; 
} 

 UBOOL UMMOUIBaseSlotContainer::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	return FALSE;
}
//------------------------------------------------------------------------
UBOOL UMMOUIBaseSlotContainer::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	return FALSE;
}
//------------------------------------------------------------------------

UBOOL UMMOUIBaseSlotContainer::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC, BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	return FALSE;
}

UBOOL UMMOUIBaseSlotContainer::OnDoubleClickSlot(class UGFxObject* SrcSlotMC,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,UBOOL bDefault)
{
	return FALSE;
}


void UMMOUIBaseSlotContainer::PreOnShow(UBOOL show)
{
	Super::PreOnShow(show);

	//if(show)
	//{
	//}
	//else
	//{
	//	if( MMOUI::GetUIManager()->GetCurrentItemSlotActionListener() == this )			
	//	{
	//		MMOUI::GetUIManager()->SetCurrentItemSlotActionListener(LastItemActionListener);
	//	}
	//}
}

void UMMOUIBaseSlotContainer::PostOnShow(UBOOL show)
{
	Super::PostOnShow(show);

//	if(show)
//	{
//		if(MMOUI::GetUIManager()->GetCurrentItemSlotActionListener() != this)
//		{
//			LastItemActionListener =  MMOUI::GetUIManager()->GetCurrentItemSlotActionListener();
//			MMOUI::GetUIManager()->SetCurrentItemSlotActionListener(this);	
//		}
//	}
//	else
//	{
//// 		if( MMOUI::GetUIManager()->GetCurrentItemSlotActionListener() == this )			
//// 			MMOUI::GetUIManager()->SetCurrentItemSlotActionListener(LastItemActionListener);
//	}
}

void UMMOUIBaseSlotContainer::OnFocusGained()
{
	UMMOUIBase::OnFocusGained();


	//UBOOL IsVisible = GetBool("visible");
	//if( IsVisible ){

	//	//add by JimZhang@sdna.com
	//	if(MMOUI::GetUIManager()->GetCurrentItemSlotActionListener() != this)
	//	{
	//		//MMOUI::MoveToTop(this);
	//		LastItemActionListener =  MMOUI::GetUIManager()->GetCurrentItemSlotActionListener();
	//		MMOUI::GetUIManager()->SetCurrentItemSlotActionListener(this);	
	//	}
	//	//LastItemActionListener =  MMOUI::GetUIManager()->GetCurrentItemSlotActionListener();
	//	//MMOUI::GetUIManager()->SetCurrentItemSlotActionListener(this);	
	//}

	//UMMOUIBase::OnFocusGained();
}

void UMMOUIBaseSlotContainer::OnFocusLost()
{
	UMMOUIBase::OnFocusLost();
	
	//if( MMOUI::GetUIManager()->GetCurrentItemSlotActionListener() == this ){
	//	MMOUI::GetUIManager()->SetCurrentItemSlotActionListener(LastItemActionListener);
	//}
}

 INT UMMOUIBaseSlotContainer::GetSlotType()
 {
// 	check(FALSE);
 	return (INT)E_CONSLOT_EQUIPPED;
 }
//------------------------------------------------------------------------
// UMMOUIBase
//------------------------------------------------------------------------
void UMMOUIBase::ClearUI()
{

}

void UMMOUIBase::ToogleVisible()
{
	ChangeVisible();
	if( !IsVisible)
		return;

	GotoAndStopI(0);
}

//------------------------------------------------------------------------
class UMMOUIBase* UMMOUIBase::FindUIPanel(FName inFriendlyName)
{
	return MMOUI::FindUIPanel(inFriendlyName);
}
//------------------------------------------------------------------------
class UItem* UMMOUIBase::FindItemTemplate(BYTE ContainerID,INT SlotID)
{
	return MMOFacade::getSingleton().FindItemTemplate(ContainerID, SlotID);
}
//------------------------------------------------------------------------
class UItem* UMMOUIBase::FindItemTemplateByID(INT ItemTemplateID)
{
	return MMOFacade::getSingleton().FindItemTemplate(ItemTemplateID);
}

void UMMOUIBase::Init()
{
	IsInitialized = TRUE;
	MMOFreeTipsUIManager::getSingleton().AddTipsListenerByName(this);
	if( GFXPlayer ){
		SWFName = GFXPlayer->GetPanelPriorityAndSWF(FriendlyName,CurrentDisplayPriority);
		//CurrentDisplayPriority = GFXPlayer->GetPanelPriority( FriendlyName );
	}
}

void UMMOUIBase::UnInit()
{
	IsInitialized = FALSE;

	MMOUI::RegisterUIPanel(NULL, FriendlyName);
	DeleteEventAdapter();
}

UBOOL UMMOUIBase::OnEsc()
{
	if(CloseOnEsc && IsVisible ){
		MMOUI::GetUIManager()->ShowPanelEx(FriendlyName,FALSE, "");
		return TRUE;
	}
	return FALSE;
}

void UMMOUIBase::PreOnShow(UBOOL v)
{
	if(v==FALSE && GetVisible())
	{
		MMOUI::RemoveTop(this);
		CanMoveTop = FALSE;
	}
}

void UMMOUIBase::PostOnShow(UBOOL v)
{
	if(v)
	{
		CanMoveTop = TRUE;
		MMOUI::MoveToTop(this);

		// determine whether the current panel should show the help button
		HelperData data;
		if (MMOHelperData::getSingleton().GetHelperData(FriendlyName.ToString(), data))
		{
			eventCallShowHelperButton(TRUE);
		}
		else
		{
			eventCallShowHelperButton(FALSE);
		}
	}
	else
	{
		OnFocusLost();
	}
}

//-----------------------------------------------------------------
void UMMOUIBase::MoveCenterToScreenCenter( INT offsetX, INT offsetY)
{
	FVector2D scrSz = MMOFacade::GetScreenSize();

	INT panelX = scrSz.X / 2 + offsetX;
	INT panelY = scrSz.Y / 2 + offsetY;

	SetPosition( panelX, panelY );
}


void UMMOUIBase::NativeShow(UBOOL show)
{
	Show(show);
}

void UMMOUIBase::Show( UBOOL V, INT panelPriority )
{
	PreOnShow(V);

	//IsVisible = V;

	SetVisible(V);

	if(panelPriority == -1)
	{
		panelPriority = this->GFXPlayer->GetPanelPriority(FriendlyName);
	}

	if(V){
		
		static INT CurPanelPriority = panelPriority;   
		static FName CurPanelName = FriendlyName; 

		if(CurPanelName!=FriendlyName)
		{
			INT CurPriority = this->GetInt("CurPriority");
			if(CurPriority-1!=panelPriority || CurPanelPriority==panelPriority)
			{
				this->SetInt("CurPriority",panelPriority+1);	
				eventShowPanel(V, panelPriority); // use the newly refactored interface
			}
			CurPanelName = FriendlyName;
		}
		CurPanelPriority = panelPriority;

	}


	//if(V)
	//{
	//	//delete the dynamic UI closed last time
	//	MMOUI::GetUIManager()->CloseLastPlayerByPanelName(FriendlyName);
	//}
	//else
	//{
	//	//set this UI as the UI pending deletion
	//	MMOUI::GetUIManager()->SetLastPlayerNameByPanelName(FriendlyName);
	//}


	//iterate and close other open windows
	//MMOUI::GetUIManager()->OnUIShow(self, v);
	MMOUI::GetUIManager()->OnUIShow(this, V);

	PostOnShow(V);
	//NativeOnShow(v);
	//OnShow(v);
}

void UMMOUIBase::DeleteEventAdapter()
{
	if(EventAdapter)
	{
		EventAdapter->Delete();
		EventAdapter = NULL;
	}
}

void UMMOUIBase::NativeInitOnce()
{

}

void UMMOUIBase::OnShutDown()
{
}
//-----------------------------------------------------------------------
// void UMMOUIBase::PlayerHPChange(INT NewHP, INT MaxHP)
// {
// }
//------------------------------------------------------------------------
// void UMMOUIBase::PlayerMPChange(INT NewMP, INT MaxMP)
// {
// }
//------------------------------------------------------------------------
void UMMOUIBase::BagSlotChange(INT BagId,INT SlotId)
{
}
//------------------------------------------------------------------------
void UMMOUIBase::PlayerLevelChange(INT lastLv,INT newLv)
{
}
//-----------------------------------------------------------------------
void UMMOUIBase::PlayerTitleChange( INT TitleID )
{
}
//------------------------------------------------------------------------
void UMMOUIBase::OtherPlayerTitleChange(class AMMOPlayer* Pl,INT TitleID )
{
}
//-----------------------------------------------------------------------
void UMMOUIBase::PlayerPkModeChange( INT PkMode )
{
}
//------------------------------------------------------------------------
void UMMOUIBase::PlayerPkValueChange( int PkValue )
{
}
//-----------------------------------------------------------------------
void UMMOUIBase::OtherPlayerPkModeChange(class AMMOPlayer* Pl, INT PkMode )
{
}
//------------------------------------------------------------------------
void UMMOUIBase::OtherPlayerPkValueChange(class AMMOPlayer* Pl, int PkValue )
{
}
//------------------------------------------------------------------------
void UMMOUIBase::PlayerAttributeChange( INT AtrrId, INT inValue )
{

}
//------------------------------------------------------------------------
void UMMOUIBase::PlayerExpChange( INT CurrentExp, INT MaxExp,INT ChangeExp )
{

}
//------------------------------------------------------------------------
void UMMOUIBase::PlayerMoneyChange( BYTE MoneyType, INT DeltaMoney )
{

}
//------------------------------------------------------------------------
void UMMOUIBase::NPCHpChange( class ANPC* NPC, INT HP)
{

}
//------------------------------------------------------------------------
void UMMOUIBase::TargetActorChange(BYTE Type,class AMMOActor* actor)
{}
//------------------------------------------------------------------------
void UMMOUIBase::OtherPlayerHpChange(class AMMOPlayer* pl,INT HP)
{

}
//------------------------------------------------------------------------
void UMMOUIBase::OtherPlayerMpChange(class AMMOPlayer* pl,INT MP)
{

}

void UMMOUIBase::ItemChange(INT ContainerID,INT SlotID)
{
}

void UMMOUIBase::SoldItemChange()
{
}


void UMMOUIBase::ItemDelete(INT ContainerID, INT SlotID)
{
	
}
///------------------------------------------------------------------------
//void UMMOUIBase::UpdateMyTeam()
//{
//
//}
///------------------------------------------------------------------------
void UMMOUIBase::UpdateTeamMate(INT UIPos , INT Pos )
{

}
///------------------------------------------------------------------------
void UMMOUIBase::UpdateMyFriend()
{

}

///------------------------------------------------------------------------

//
//void UMMOUIBase::SetFriendItemValue(INT GroupID,INT pos,const FString& PlayerName,INT Lv,const FString& Carrer,const FString& Location,const FString& Sign,INT RoleId,INT Status)
//{
//
//}
//------------------------------------------------------------------------
//void UMMOUIBase::NativeReceiveMessage(INT ChannelId,INT TargetId,const FString& MessageContent,const FString& SenderName )
//{
//
//}
//---------------------------------------------------------------------------------------------------------------
void UMMOUIBase::PlayerLevelUp(INT NewLv)
{

}
//---------------------------------------------------------------------------------------------------------------
//void UMMOUIBase::AddedAttentionBy(INT RoleId,const FString& RoleName)
//{
//
//}
//---------------------------------------------------------------------------------------------------------------
void UMMOUIBase::FriendChatMsg(INT RoleId,const FString& Content)
{

}

//---------------------------------------------------------------------------------------------------------------
void UMMOUIBase::UnReadFriendChatMsgChange()
{

}
//---------------------------------------------------------------------------------------------------------------
void UMMOUIBase::AddNewNpcData(class ANPC* npc)
{

}

//---------------------------------------------------------------------------------------------------------------
void UMMOUIBase::UpdatePlayerReputation(class AMMOPlayer* Pl,INT ForceID,INT newReputation, INT lastReputation)
{

}

//---------------------------------------------------------------------------------------------------------------
void UMMOUIBase::PlayerBaseInfoChange()
{

}
//---------------------------------------------------------------------------------------------------------------
void UMMOUIBase::SkillSlotChange()
{

}
//---------------------------------------------------------------------------------------------------------------
void UMMOUIBase::UpdateTeamContent()
{

}

//---------------------------------------------------------------------------------------------------------------
//void UMMOUIBase::UpdateAllTeamMate()
//{
//
//}
////---------------------------------------------------------------------------------------------------------------
//void UMMOUIBase::UpdateTeamMateNum( int MateNum )
//{
//
//}

//---------------------------------------------------------------------------------------------------------------
void UMMOUIBase::EnterScene()
{
	Show(DefaultVisible);
}
//---------------------------------------------------------------------------------------------------------------
void UMMOUIBase::ChangeScene()
{

}
//---------------------------------------------------------------------------------------------------------------
void UMMOUIBase::EnterLogin()
{
	Show(false);
}

void UMMOUIBase::TickForTestInteractNPCDistance(float deltaTime)
{
	if(IsTargetOutDistanceClose&&(this->IsVisible))
	{
		//portable shop can directly open certain UIs
		if (IsShowWithOutTarget)
		{
			return;
		}

		AMMOActor* selectActor = Cast<AMMOActor>(MMOFacade::getSingleton().GetMainController()->GetSelectTarget());
		if(!selectActor)
		{
			Show(FALSE);
		}
		else
		{
			static INT fTimeSpan = 0;
			if(++fTimeSpan > 10)
			{
				fTimeSpan = 0;//delay the distance check once
				INT CurDistance = (selectActor->Location-MMOFacade::getSingleton().GetMainController()->Pawn->Location).Size2D();
				if(CurDistance >= nTargetMaxDistance)
					Show(FALSE);
			}
		}
	}//end of if IsVisible
}

//------------------------------------------------------------------------

void UMMOUIBase::OnStallStateChange(BYTE state)
{

}


 void UMMOUIBase::OnFocusGained()
 {	 
	 //TArray<struct FASValue> Args;
	 //Invoke(TEXT("OnFocusGained"),Args);
	 //MMOUI::OnFocusGained(this);
 }

 void UMMOUIBase::OnFocusLost()
 {
// 	  MMOUI::OnFocusLost(this);
// 	 TArray<struct FASValue> Args;
// 	 Invoke(TEXT("OnFocusLost"),Args);
 }
 
UBOOL UMMOUIBase::CanFocusGained()
{
	TArray<struct FASValue> Args;
	FASValue ret = Invoke(TEXT("CanFocusGained"),Args);

	if (ret.Type == AS_Boolean)
	{
		return ret.B ;
	}
	else
	{
		return TRUE;
	}
}

UBOOL UMMOUIBase::CanFocusLost()
{
	TArray<struct FASValue> Args;
	FASValue ret = Invoke(TEXT("CanFocusLost"),Args);

	if (ret.Type == AS_Boolean)
	{
		return ret.B;
	}
	else
	{
		return true;
	}
}
 
UBOOL UMMOUIBase::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	return FALSE;
}

 
void UMMOUIBase::NativeInitLoad()
{	
	Show(FALSE);
}

void UMMOUIBase::UnitWidget()
{
	Show(DefaultVisible);
}

void UMMOUIBase::ChangeVisible()
{
	IsVisible = GetBool(TEXT("visible"));
	Show(!IsVisible, CurrentDisplayPriority);
}

UBOOL UMMOUIBase::GetVisible()
{
    IsVisible = GetBool(TEXT("visible"));
    return IsVisible;
}

void UMMOUIBase::SetVisible(UBOOL bValue)
{
	IsVisible = bValue;
	SetBool(TEXT("visible"),bValue);
}


void UMMOUIBase::SetItemSlotCD( UGFxObject* itemSlot , INT curCD , INT maxCD )
{
	if(!itemSlot)
	{
		return;
	}

	FASValue asLeftMiniSec(EC_EventParm);
	asLeftMiniSec.Type = AS_Int;
	asLeftMiniSec.I = curCD;

	FASValue asTotalMiniSec(EC_EventParm);
	asTotalMiniSec.Type = AS_Int;
	asTotalMiniSec.I = maxCD;

	TArray<struct FASValue> asArguments;
	asArguments.AddItem(asLeftMiniSec);
	asArguments.AddItem(asTotalMiniSec);

	itemSlot->Invoke( TEXT("SetCD") , asArguments );
}

void UMMOUIBase::StopItemSlotCD( UGFxObject* itemSlot )
{
	if(itemSlot)
	{
		TArray<struct FASValue> asArguments;
		itemSlot->Invoke( TEXT("CancelCD") , asArguments );
	}
}

void UMMOUIBase::SetShowWithOutTarget(UBOOL bValue)
{
	IsShowWithOutTarget = bValue;
}

void UMMOUIBase::SetItemSlotData(class UGFxObject* mc,class UItem* tItem,BYTE SlotType,BYTE ContainerID,INT SlotID, BYTE  inISS )
{
	GFXPlayer->SetItemSlotData( mc, tItem, SlotType, ContainerID, SlotID,inISS);
}