/********************************************************************
	Created:	2013/06
	Filename: 	UMMOGFXPlayer_PickWidget
	file ext:	cpp
	Author:		hemu@snda
	Purpose:	Drop pickup Panel-MMOUIDropItemTips
*********************************************************************/

#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "FSMIO.h"
#include "UITextParser.h"
#include "MMOStats.h"

#include "MMODropManager.h"

IMPLEMENT_CLASS(UMMOUIDropItemTips)

void UMMOUIDropItemTips::Init()
{
	Super::Init();

	MMODropManager::getSingleton().SetDropTipsUI(this);
}

void UMMOUIDropItemTips::UnInit()
{
	Super::UnInit();

	MMODropManager::getSingleton().SetDropTipsUI(NULL);
}

void UMMOUIDropItemTips::PostOnShow(UBOOL show)
{

	MMODropManager::getSingleton().SetVisibleForceRefresh();
	Super::PostOnShow(show);

}

// UBOOL UMMOUIDropItemTips::Validate(  )
// {
// // 	if( MainPlayer == NULL)
// // 		return FALSE;
// //     if( MainPlayerController == NULL )
// //         return FALSE;
// 	return TRUE;
// }

// void UMMOUIDropItemTips::EnterScene()
// {
//     //annoying, this broadcast packet is gone
//     //DisableAll( );
//     Show(DefaultVisible);
// }

// void UMMOUIDropItemTips::NativeIni()
// {
// //     if( IsAutoHide )
// //     {
// //         DefaultVisible = false;	
// //     }
// //     else
// //     {
// //         DefaultVisible = true;		
// //     }
// //     TipCtrlPool.AddZeroed( MaxTipCtrlPoolSize );
// }

// void UMMOUIDropItemTips::Toggle()
// {
//     ChangeVisible();
//     if( IsVisible && IsAutoHide )
//     {
//         TipRemainedShowTime = TipAutoHideTime;
//     }
// }

// void UMMOUIDropItemTips::OnShutDown()
// {
//     for( int i = 0; i < TipCtrlPoolSize; ++i  )
//     {
//         if( NULL != TipCtrlPool( i ).Tip )
//         {
//            eventRemoveChild( TipCtrlPool( i ).Tip );
//         }
//     }
// }

// void UMMOUIDropItemTips::DisableAll( )
// {
//     for( int i = 0; i < TipCtrlPoolSize; ++i  )
//     {
//         TipCtrlPool( i ).IsUsed = false;
//         TipCtrlPool( i ).Tip->SetVisible( FALSE );
//     }
// }

// void UMMOUIDropItemTips::DisableDropItemTip( int id )
// {
//     for( int i = 0; i < TipCtrlPoolSize; ++i  )
//     {
//         if( TipCtrlPool( i ).Id == id)
//         {
//             TipCtrlPool( i ).IsUsed = false;
//             TipCtrlPool( i ).Tip->SetVisible( FALSE );
//         }
//     }
// }
// 
// void UMMOUIDropItemTips::EnableNewDropItemTip(  FPickableItem *dropItem )
// {
//     if( !dropItem->PickItem || !dropItem->PickItem->ItemData  )
//     {
//         return;
//     }
// 
//     //Find an available control; if none, expand the current CtrlPool
//     FTipCtrl* tipCtrl = NULL; 
//     for( int i = 0; i < TipCtrlPoolSize; ++i  )
//     {
//         if( !TipCtrlPool( i ).IsUsed )
//         {
//             tipCtrl = &TipCtrlPool( i );
//             if( NULL == tipCtrl->Tip )
//             {
//                 //Should not appear normally
//                 tipCtrl->Tip = this->AttachMovie( TEXT("DropItemHint"), FString::Printf( TEXT("PickableTips%d"),i) );
//             }
//             break;
//         }
//     }
// 
//     if( !tipCtrl )
//     {
//         for( int i = TipCtrlPoolSize; i < TipCtrlPoolSize + DefaultTipCtrlPoolIncrement ; ++i  )
//         {
//             TipCtrlPool( i ).IsUsed = false;
//             TipCtrlPool( i ).Tip    = this->AttachMovie( TEXT("DropItemHint"), FString::Printf( TEXT("PickableTips%d"),i) );
//         }
//         tipCtrl = &TipCtrlPool( TipCtrlPoolSize );
//         TipCtrlPoolSize += DefaultTipCtrlPoolIncrement;
//     }
// 
//     if( !tipCtrl || ! tipCtrl->Tip )
//     {
//         return;
//     }
// 
//     //Fill in related data
//     UGFxObject* tTips = tipCtrl->Tip;
//     tipCtrl->IsUsed   = TRUE;
//     tipCtrl->Id       = dropItem->PickItem->Id;
//     dropItem->Tips    = tTips;
// 
//     TArray<struct FASValue> Arguments0;
//     FASValue ASValue0(EC_EventParm);
// 
//     ASValue0.I =  dropItem->PickItem->Id; 
//     ASValue0.Type = AS_Int;
//     Arguments0.AddItem(ASValue0);
// 
//     //ASValue0.S = dropItem->PickItem->ItemData->GetDisplayName(); 
//     ASValue0.S = UITextParser::HTMLFormatItemName( dropItem->PickItem->ItemData );
//     ASValue0.Type = AS_String;
//     Arguments0.AddItem(ASValue0);
// 
//     /*INT realColor     = 0;
//     if( dropItem->PickItem->ItemData )
//     {
//         realColor     = UITextParser::HTMLGetItemRealColor( (EItemColor)dropItem->PickItem->ItemData->ItemColor );
//     }
//     ASValue0.I =  realColor; 
//     ASValue0.Type = AS_Int;
//     Arguments0.AddItem(ASValue0);*/
// 
//     tTips->Invoke("SetTipContent", Arguments0 );
//     tTips->SetVisible( true );
// }
// 
// void UMMOUIDropItemTips::Update(  UCanvas *cvs ,float deltaTime )
// {
// 	if( !Validate() )
// 		return;
// 
//     //SCOPE_CYCLE_COUNTER(STAT_DropItemSystemTime)
// 
//     //Continuous display time countdown
// 	if( IsAutoHide && TipRemainedShowTime > 0 )
// 	{
// 		TipRemainedShowTime -= deltaTime;
// 		if( TipRemainedShowTime <= 0 )
//         {
// 			ChangeVisible();
//         }
// 	}
// 
//     DropItemMgr->UpdateDoodad( deltaTime );
//     DropItemMgr->UpdateDropItemsAndAutoPick( deltaTime );
// 
// 	if( IsVisible && DropItemMgr->HasDropItem() )
// 	{
//         DropItemMgr->UpdateDropItemTips( cvs, deltaTime );
// 
// 		if(DropItemMgr->DropItemList.Num() > 0 )
//         {
// 			BaseItem = DropItemMgr->DropItemList(0);
//         }
// 
// 		for( int i=0 ; i< DropItemMgr->DropItemList.Num() ;i++)
// 		{
// 			FPickableItem *dropItem = &DropItemMgr->DropItemList(i);
// 			if(  NULL == dropItem )
// 				continue;
// 
//             if( !dropItem->IsVisible )
//             {
//                 if( dropItem->Tips )
//                 {
//                     dropItem->Tips->SetVisible( false );
//                 }
//             }
//             else 
//             {
//                 if(  NULL == dropItem->Tips )
//                 {
//                     EnableNewDropItemTip( dropItem ); 
//                 }
//                 SelectScreenPos( *dropItem, cvs );
//             }
// 		}
// 	}
// }
// 
// void UMMOUIDropItemTips::SelectScreenPos( FPickableItem& item ,UCanvas *cvs )
// {
//     if( !Validate() )
//         return;
// 
//     //Too many drops, pushing aside would fill the screen, not sorting for now..
//     if( EnableAutoGridTip ) 
//     {
//         for( int i = 0;i < DropItemMgr->DropItemList.Num();i++ )
//         {
//             FPickableItem dropItem = DropItemMgr->DropItemList(i);
//             if( dropItem.IsVisible )
//             {
//                 if( CheckNoOverlap( dropItem , item )>0 )
//                 {
//                     item.ScreenPos.Y = dropItem.ScreenPos.Y + GridHeight;
//                     i = -1;	
//                 }
//                 else if( CheckNoOverlap( dropItem , item ) <0 )
//                 {
//                     item.ScreenPos.Y = dropItem.ScreenPos.Y - GridHeight;
//                     i = -1;
//                 }
//                 if( item.ScreenPos.Y < -GridHeight || item.ScreenPos.Y > 800 )
//                 {
//                     break;
//                 }
//             }
//         }
//     }
// 
//     if( item.Tips )
//     {
//         FVector ScreenPos =  item.ScreenPos + FVector( 0, -GridHeight, 0.0f );
//         item.Tips->SetPosition(  ScreenPos.X  , ScreenPos.Y );
//         item.Tips->SetVisible( TRUE );
//     }
// }
// 
// float UMMOUIDropItemTips::CheckNoOverlap( FPickableItem DrawnItem, FPickableItem Item )
// {
//     if( !Validate() )
//         return 0;
// 
//     SCOPE_CYCLE_COUNTER(STAT_DropItemTipsTime)
// 
//     //No overlap, return=0, higher than original layer return <0, lower than original layer return>0
//     if( Item.ScreenPos.X <= DrawnItem.ScreenPos.X   )//on the left
//     {
//         if( Item.ScreenPos.X + Item.Width <= DrawnItem.ScreenPos.X )
//         {
//             return 0;
//         }
//         else if(  Item.ScreenPos.Y + GridHeight <= DrawnItem.ScreenPos.Y  )//above
//         {
//             return 0;
//         }
//         else if(  Item.ScreenPos.Y >= DrawnItem.ScreenPos.Y + GridHeight )
//         {
//             return 0;
//         }
//     }
//     else//right
//     {
//         if( Item.ScreenPos.X >= DrawnItem.ScreenPos.X + DrawnItem.Width )
//         {
//             return 0;
//         }
//         else if(  Item.ScreenPos.Y + GridHeight <= DrawnItem.ScreenPos.Y  )//above
//         {
//             return 0;
//         }
//         else if(  Item.ScreenPos.Y >= DrawnItem.ScreenPos.Y + GridHeight )
//         {
//             return 0;
//         }
// 
//     }
//     return Item.ScreenPos.Y - BaseItem.ScreenPos.Y;
// }
// 
// void UMMOUIDropItemTips::TipsPickItem( int ItemID )
// {
//     if( !Validate() )
//         return;
// 
//     ADropItemActor* tActor = DropItemMgr->GetPickableItemById( EItemActorType_Drop, ItemID );
//     if( tActor )
//         MMOTargetManager::getSingleton().SetDesireTarget( NULL, tActor, false );
// }

void UMMOUIDropItemTips::Pickup(INT ItemID)
{
	if(FSM::Target.CurTargetMMOActor==NULL)
	{
		MMODropManager::getSingleton().PickupDropActorByUI(ItemID);
	}
}

//===============================================================================================================
UBOOL UMMOUIDropItemTips::SkipCaptureInputKey( const FString& key, BYTE eIE, const FString& strHoverControllerName, FVector2D ClickPosInMovieSpace )
{
	if( strHoverControllerName == TEXT( "mc_hint" )  )
	{
		return TRUE;
	}
	if( strHoverControllerName == TEXT( "tf_Pick_Text" )  )
	{
		return TRUE;
	}

	
	return Super::SkipCaptureInputKey( key, eIE, strHoverControllerName, ClickPosInMovieSpace );
}
