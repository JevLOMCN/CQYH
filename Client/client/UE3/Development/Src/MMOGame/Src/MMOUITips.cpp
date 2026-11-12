#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "AbilityProcessor.h"
#include "UITextParser.h"
#include "MMOFacade.h"
#include "MMOBuffManager.h"
#include "TipsFormatter.h"
#include "ScaleformEngine.h"
#include "FSMIO.h"

IMPLEMENT_CLASS(UMMOUITips)

	void UMMOUITips::NativeInitLoad()
{
}
//Abandaned------------------------------------------------------------------------ 
void UMMOUITips::ShowTips(INT tipstype,INT objectId,class UGFxObject* widge)
{

}
//========================================================================================================
UBOOL UMMOUITips::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	//return TRUE;
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}
//========================================================================================================
//HideTip
void UMMOUITips::HideTips()
{
#ifdef UI_OPTIMIZATION
	if(IsVisible==FALSE)return;
#endif

	ShowCompareTip = FALSE;
	ShowNextSkillTip = FALSE;
	IsTipsAnchored = FALSE;
	//SetAnchoredTipsAutoHideByMousePos( FALSE );
	SetVisible( FALSE );
	eventShowPanel(FALSE, 4);
	if( CurrentTipsType == TFT_SkillEquipped ){
		if( CurrentFromObject != NULL){
			INT tipsFromPanelType = CurrentFromObject->GetInt(TEXT("SlotType"));
			if( tipsFromPanelType == ESPType_SkillBar ){
				MMOGameEvent_OneInt showSkillRangeEvt( GameEvent_ShowSkillRange, 0 );
				MMOUI::PostGameEvent( &showSkillRangeEvt );
			}
		}
	}
}

UBOOL UMMOUITips::NativeIsTipsShow()
{
	if (IsVisible == FALSE)return FALSE;
	if (CurrentTipsType == TFT_FreeStyle)return FALSE;
	return TRUE;
}

//ShowFreeStyleTip
void UMMOUITips::ShowFreeStyleTips(const FString& title,const FString& text,INT showStyle, int displayPriority)
{
	LOG_PROCESS_ERROR(GFXPlayer);
	UGFxObject * ItemObj = GFXPlayer->CreateObject("Object");
	LOG_PROCESS_ERROR(ItemObj);

	ItemObj->SetString(TEXT("title"),	title  );
	ItemObj->SetString(TEXT("text"),	text );
	ItemObj->SetInt(TEXT("style"),		showStyle );

	ShowTipsHTML(TFT_FreeStyle, ItemObj, FALSE, displayPriority);

	FSMIO::getSingleton().Trigger.SetShowTips(TRUE);
	return;

Exit0:
	SetVisible(FALSE);
	eventShowPanel(FALSE, -1);
	return;
}

//
void UMMOUITips::ShowEquipSlotEnhanceTips( INT enhanceClassID, INT curEnhanceLv )
{
	LOG_PROCESS_ERROR(GFXPlayer);
	UGFxObject * ItemObj = GFXPlayer->CreateObject("Object");
	LOG_PROCESS_ERROR(ItemObj);


	ItemObj->SetInt(TEXT("tipsVal1"), enhanceClassID );
	ItemObj->SetInt(TEXT("tipsVal2"), curEnhanceLv );
	ShowTipsHTML(TFT_EquipSlotEnhance, ItemObj, FALSE, CurrentDisplayPriority);
	FSMIO::getSingleton().Trigger.SetShowTips(TRUE,TRUE);
	return;

Exit0:
	SetVisible(FALSE);
	eventShowPanel(FALSE, -1);
	return;
}

void UMMOUITips::ShowTextTips(const FString& text)
{
	LOG_PROCESS_ERROR(GFXPlayer);
	UGFxObject * ItemObj = GFXPlayer->CreateObject("Object");
	LOG_PROCESS_ERROR(ItemObj);

	ItemObj->SetString(TEXT("text"), text);
	ShowTipsHTML(TFT_NormalText, ItemObj, FALSE, CurrentDisplayPriority);
	FSMIO::getSingleton().Trigger.SetShowTips(TRUE);

	return;

Exit0:
	SetVisible(FALSE);
	eventShowPanel(FALSE, -1);
	return;
}

//Get the Tip sub-panel for the specified type
UGFxObject* UMMOUITips::GetSubTipPanelByType( BYTE TipsType )
{
	////0:bag 1:ability hotbar 2:character equipment 3:ability panel. 4:NPC shop 5,warehouse  6, Stall; category of the panel it belongs to 7, rune Tip is unconventional
	UGFxObject* subPanel = 0;
	switch ( TipsType )
	{
	case TFT_ItemSlot:
	case TFT_Link:
		{
			subPanel = GetObject("itemTip");
			break;
		}
		break;
	case TFT_ItemBar:
		{
			if( CurrentFromObject != NULL )
			{
				INT itemBarContainType = CurrentFromObject->GetInt( TEXT("EquipID") );
				if( itemBarContainType == ESBarSlotTp_Item )
				{
					subPanel = GetObject("itemTip");
				}
				else if( itemBarContainType == ESBarSlotTp_Mount)
				{
					subPanel = GetObject("mountTip");
				}
				else if( itemBarContainType == ESBarSlotTp_Skill )
				{
					subPanel = GetObject("skillTip");
				}
			}
		}

		break;
	case TFT_SkillEquipped:
	case TFT_SkillSlotTemplate:
	case TFT_SkillTemplate:
	case TFT_SkillTemplateWithNext:
	case TFT_RuneButton:
	case TFT_AvartaRuneButton:
		subPanel = GetObject("skillTip");
		break;
	case TFT_RuneActive:	
		subPanel = GetObject("commonTip");
		break;

	case TFT_Tab:	
		subPanel = GetObject("commonTip");
		break;
	case TFT_MenuBar:	
		subPanel = GetObject("commonTip");
		break;

	case TFT_AwardIcon:	
		subPanel = GetObject("itemTip");
		break;
	case TFT_MountSlot:
	case TFT_BuffSlot:
		subPanel = GetObject("mountTip");
		break;
	case TFT_Attribute:
	case TFT_PlainText:	
	case TFT_NormalText:
	case TFT_FreeStyle:	
	case TFT_Other:	
	case TFT_HeavenGasSlot:
	case TFT_EquipSlotEnhance:
	case TFT_EquipDurability:
	case TFT_GoldStore:
		subPanel = GetObject("commonTip");
		break;

	default:
		subPanel = GetObject("commonTip");
		break;
	}   

	return subPanel;
}

//
void UMMOUITips::ToggleTipAid( )
{
	if( GetBool("visible") )
	{
		ShowDetail = !ShowDetail;
		RefreshTip( );
	}
}

void UMMOUITips::RefreshTip( )
{
	ShowTipsHTML( CurrentTipsType, CurrentFromObject, true, CurrentDisplayPriority );
}


//Show Tips
void UMMOUITips::ShowTipsHTML(BYTE TipsType,class UGFxObject* FromObject, UBOOL IsRefreshing /*= FALSE*/, INT displayPriority )
{
	//if(TFT_HeavenGasSlot == TipsType)
	//	return;
	//Record, convenient for refreshing Tip later
	CurrentTipsType = TipsType;
	CurrentFromObject = FromObject;

	//Switch Tips control after skipping a frame
	subTipPanel = GetSubTipPanelByType( TipsType );

	if ( subTipPanel && FromObject )
	{
		eventCallCloseAll();

		SetVisible(TRUE);
		eventShowPanel(TRUE, displayPriority);
		subTipPanel->SetVisible( true );

		TipsFormatter::FormatTips((ItemSlotType)TipsType, FromObject, this, subTipPanel );
		AutoPosition();

		FSMIO::getSingleton().Trigger.SetShowTips(TRUE);
	}
}

void UMMOUITips::ShowLinkTips( class UGFxObject* fromObject )
{
	//ShowTipsHTML( TFT_Link, fromObject );

	CurrentTipsType = TFT_Link;
	CurrentFromObject = fromObject;

	//Switch Tips control after skipping a frame
	subTipPanel = GetSubTipPanelByType( TFT_Link );

	if ( subTipPanel && fromObject )
	{
		eventCallCloseAll();
		TipsFormatter::FormatTips(TFT_Link, fromObject, this, subTipPanel );
		subTipPanel->SetVisible( true );
		AutoPosition();
		SetVisible(TRUE);
		eventShowPanel(TRUE, -1);
		//FSMIO::getSingleton().Trigger.SetShowTips(TRUE,TRUE);

	}
}

//=====================================================================================
//void UMMOUITips::ShowFixPosItemTips(class UItem* pItem, class UGFxObject* FromObject)
//{
//	ShowItemTips( pItem, FromObject );	
//Exit0:
//	SetVisible(FALSE);
//	return;
//}

//=====================================================================================
//Adjust position to fit
void UMMOUITips::AutoPosition()
{
	GetCurrentTipsSize();

	FIntPoint tipPos = MMOFacade::GetMouseCursorPosition();
	tipPos.X += TipPosOffsetX;
	tipPos.Y += TipPosOffsetY;

	FitTipsForScreen( *subTipPanel, tipPos, TipCurrentSize.X, TipCurrentSize.Y );

	SetTipsPostion( tipPos );
}

void UMMOUITips::GetCurrentTipsSize()
{
	if( NULL == subTipPanel ){
		return;
	}

	//Internally calculate the actual width and height of the transformed Tip
	TipCurrentSize.X = subTipPanel->GetInt( TEXT("TipWidth") );
	TipCurrentSize.Y = subTipPanel->GetInt( TEXT("TipHeight") );
	if( TipCurrentSize.Y <= 0 || TipCurrentSize.X <= 0 )
	{
		TipCurrentSize.Y = subTipPanel->GetInt( TEXT("height") ) ;
		TipCurrentSize.X = subTipPanel->GetInt( TEXT("width") );
	}

	if( ShowCompareTip )
	{
		TipCurrentSize.X += eventCallASGetExtraItemTipsWidth();
		INT compareHeight = eventCallASGetExtraItemTipsHeight();
		TipCurrentSize.Y = TipCurrentSize.Y >= compareHeight ? TipCurrentSize.Y : compareHeight;
	}

	if( ShowNextSkillTip && NextSkillTip)
	{
		TipCurrentSize.X += TipCurrentSize.X;
		INT nextSkillHeight = NextSkillTip->GetInt( TEXT("TipHeight") );
		TipCurrentSize.Y = TipCurrentSize.Y >= nextSkillHeight ? TipCurrentSize.Y : nextSkillHeight;
	}
}

//
void UMMOUITips::FitTipsForScreen( UGFxObject &itemTips, FIntPoint &tipPos, INT actualWidth, INT actualHeight )
{
	INT screenSizeX = FGFxEngine::GetEngine()->GetRenderViewport()->GetSizeX();
	INT screenSizeY = FGFxEngine::GetEngine()->GetRenderViewport()->GetSizeY();

	UGFxObject* pMainItemPanel = itemTips.GetObject(TEXT("mainItem"));
	UGFxObject* pSuitAddPanel = itemTips.GetObject(TEXT("suitAdd"));
	//const INT posOffset = 20;
	//
	UBOOL isRightOut = ( tipPos.X + actualWidth > screenSizeX );
	UBOOL isBottomOut = ( tipPos.Y + actualHeight > screenSizeY );
	if( isBottomOut ){
		tipPos.Y -= TipPosOffsetY;
	}
	if( isRightOut ){
		tipPos.X -= TipPosOffsetX;
	}
	//
	if( isBottomOut )
	{
		FIntPoint mousePos = tipPos;
		tipPos.Y = tipPos.Y - actualHeight;
		if (tipPos.Y < 0)
		{
			tipPos.Y = screenSizeY - actualHeight;
		}

		//ItemTips may be shorter
		if( NULL != pMainItemPanel )
		{
			if( ( ShowNextSkillTip ) || ( ShowCompareTip ) )
			{
				INT mainItemTipsPosY = mousePos.Y - tipPos.Y;// + posOffset;
				INT mainItemTipsHeight = pMainItemPanel->GetInt(TEXT("TipHeight"));
				if( mainItemTipsPosY < 0 )
				{
					mainItemTipsPosY = 0;
				}
				else if( mainItemTipsPosY + mainItemTipsHeight > actualHeight )
				{
					mainItemTipsPosY = actualHeight - mainItemTipsHeight;// - posOffset;
				}
				pMainItemPanel->SetInt(TEXT("y"), mainItemTipsPosY);
			}
			else
			{
				pMainItemPanel->SetInt(TEXT("y"), 0);
			}
		}
	}
	else
	{
		//tipPos.Y += posOffset;

		//
		if( NULL != pMainItemPanel )
		{
			pMainItemPanel->SetInt(TEXT("y"), 0);
		}
	}

	//Handle exceeding the right boundary
	UBOOL isNeedFlip = FALSE;
	if( isRightOut )
	{
		if( tipPos.X >= actualWidth ){
			//Directly mirror flip
			tipPos.X -= actualWidth;
			isNeedFlip = TRUE;
		}
		else{
			//Move left until it fits
			tipPos.X = screenSizeX - actualWidth;
		}
	}

	if( itemTips.GetInt(TEXT("x")) != 0 )
	{
		itemTips.SetInt(TEXT("x"), 0);
	}

	if( isNeedFlip ){
		if( NULL != pSuitAddPanel && NULL != pMainItemPanel )
		{
			if( ShowCompareTip )
			{
				pMainItemPanel->SetInt(TEXT("x"), 0);
				if(pSuitAddPanel->GetBool(TEXT("visible")))
					pSuitAddPanel->SetInt(TEXT("x"), pMainItemPanel->GetInt(TEXT("TipWidth")));
			}
			else
			{
				pMainItemPanel->SetInt(TEXT("x"), 0);
				//if(pSuitAddPanel->GetBool(TEXT("visible")))
				//	pMainItemPanel->SetInt(TEXT("x"), pSuitAddPanel->GetInt(TEXT("TipWidth")));
			}
		}

		if( ShowCompareTip )
		{
			INT compareItemTipsWidth = eventCallASGetExtraItemTipsWidth();
			itemTips.SetInt(TEXT("x"), compareItemTipsWidth);
			eventCallASSetExtraItemTipsPosX( 0 );
		}
		else if( ShowNextSkillTip && NextSkillTip )
		{
			itemTips.SetInt(TEXT("x"), NextSkillTip->GetInt(TEXT("TipWidth")));
			NextSkillTip->SetInt(TEXT("x"), 0);
		}
	}
	else
	{
		if( NULL != pSuitAddPanel && NULL != pMainItemPanel )
		{
			if(ShowCompareTip)
			{
				if(pSuitAddPanel->GetBool(TEXT("visible")))
				{
					pSuitAddPanel->SetInt(TEXT("x"), 0);
					pMainItemPanel->SetInt(TEXT("x"), pSuitAddPanel->GetInt(TEXT("TipWidth")));
				}
				else
				{
					pMainItemPanel->SetInt(TEXT("x"), 0);
				}
			}
			else
			{
				pMainItemPanel->SetInt(TEXT("x"), 0);
				if( pSuitAddPanel->GetBool(TEXT("visible")) )
				{
					pSuitAddPanel->SetInt(TEXT("x"), pMainItemPanel->GetInt(TEXT("TipWidth")));
				}
			}
		}

		if( ShowCompareTip )
		{
			eventCallASSetExtraItemTipsPosX( itemTips.GetInt(TEXT("TipWidth") ) );
		}
		else if( ShowNextSkillTip && NextSkillTip )
		{
			NextSkillTip->SetInt(TEXT("x"), itemTips.GetInt(TEXT("TipWidth")));
		}
	}
}

//
//void UMMOUITips::FitSkillTipsForScreen( UGFxObject &itemTips, FIntPoint &tipPos, INT ActualWidth, INT ScreenSizeX )
//{
//	if( ShowNextSkillTip && NextSkillTip )
//	{
//		NextSkillTip->SetInt(TEXT("x"), itemTips.GetInt(TEXT("TipWidth")));
//	}
//}

//Update Tips data; passing values via Native is cumbersome, kept here for Debug
void UMMOUITips::CallUpdateItemTipDataNative( UGFxObject* dataProvider )
{
	FASValue AsVal(EC_EventParm);	
	AsVal.S =  "";
	AsVal.Type = AS_String;
	TArray<struct FASValue> Arguments;
	Arguments.AddItem(AsVal);

	UGFxObject* subTipPanel = GetSubTipPanelByType( TFT_ItemSlot );
	if ( subTipPanel )
	{
		subTipPanel->SetObject( TEXT("TipData"), dataProvider );
		subTipPanel->Invoke( TEXT("updateData"), Arguments );
	}
}

//
void UMMOUITips::PostAdvance(float DeltaTime)
{
	if( !IsVisible ){
		return;
	}

	if( IsTipsAnchored == TRUE ){
		if( IsAnchored3D == TRUE ){
			//3D
			FVector2D tipsPos;
			UBOOL isOK = MMOFacade::getSingleton().GetScreenPixelFromWorld( TipAnchoredPos, tipsPos );
			if( isOK == TRUE ){
				tipsPos = tipsPos - TipCurrentSize / 2;
				SetTipsPostion( tipsPos );
			}
		}
		else{
			//2D
			FIntPoint curMousePos = MMOFacade::GetMouseCursorPosition();
			FIntPoint anchoredPos( LastCallSetTipsPosMousePos.X, LastCallSetTipsPosMousePos.Y );
			FIntPoint dis = curMousePos - anchoredPos;
			//if( dis.X > 1 || dis.X < -1 || dis.Y > 1 || dis.Y < -1 ){
			//static INT ssssseee = 16;
			if( dis.Size() > 16 ){
				HideTips();
			}
		}
	}
	else{
		AutoPosition();
	}
}

//=======================================================
void UMMOUITips::SetTextLinkTipsPos()
{
	if( subTipPanel == NULL ){
		return;
	}

	FIntPoint tipPos = MMOFacade::GetMouseCursorPosition();
	tipPos.X += TipPosOffsetX;
	tipPos.Y += TipPosOffsetY;

	FitTipsForScreen( *subTipPanel, tipPos, TipCurrentSize.X, TipCurrentSize.Y );

	SetAnchored( tipPos, FALSE );
}
//=======================================================
void UMMOUITips::SetPosAnchoredByCurMousePos()
{
	SetAnchored( MMOFacade::GetMouseCursorPosition(), FALSE );
}

//=======================================================
void UMMOUITips::SetAnchored( FVector toAnchorPos, UBOOL isPos3D )
{
	IsTipsAnchored = TRUE;
	TipAnchoredPos = toAnchorPos;
	IsAnchored3D = isPos3D;

	FVector2D tipsPos;
	if( IsAnchored3D ){
		UBOOL isOK = MMOFacade::getSingleton().GetScreenPixelFromWorld( TipAnchoredPos, tipsPos );
		if( subTipPanel != NULL ){
			INT showedHeight = subTipPanel->GetInt( TEXT("TipHeight") );
			INT showedWidth = subTipPanel->GetInt( TEXT("TipWidth") );
			tipsPos.X -= showedWidth / 2;
			tipsPos.Y -= showedHeight / 2;
		}
	}
	else{
		tipsPos.X = TipAnchoredPos.X;
		tipsPos.Y = TipAnchoredPos.Y;
	}

	SetTipsPostion( tipsPos );
}

//=======================================================
void UMMOUITips::SetTipsPostion( FVector2D toPos )
{
	LastCallSetTipsPosMousePos = MMOFacade::GetMouseCursorPosition();
	this->SetPosition( toPos.X , toPos.Y );
}

//=======================================================
void UMMOUITips::Init()
{
	Super::Init();
	if( GFXPlayer ){
		GFXPlayer->AddPostAdvanceDelegate( FScriptDelegate(this, TEXT("PostAdvance")) );
	}

	//
	ShowDetail = FALSE;
	ShowNextSkillTip = FALSE;
	ShowCompareTip = FALSE;

	TipsText    = GetObject( TEXT("tips_item_text") );	
	TipsTextR   = GetObject( TEXT("tips_item_text_r") );
	TipsBg      = GetObject( TEXT("TipsBg") );

	NextSkillTip = GetObject( TEXT("nextSkillTip") );
}

void UMMOUITips::UnInit()
{
	Super::UnInit();
	if( GFXPlayer ){
		GFXPlayer->RemovePostAdvanceDelegate( FScriptDelegate(EC_EventParm) );
	}
}
//=======================================================