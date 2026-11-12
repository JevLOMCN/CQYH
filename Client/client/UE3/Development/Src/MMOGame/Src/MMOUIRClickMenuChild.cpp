#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "FriendManager.h"
#include "TeamManager.h"
#include "UITextParser.h"

IMPLEMENT_CLASS(UMMOUIRClickMenuChild)

void UMMOUIRClickMenuChild::Init()
{
	this->SetInt(TEXT("MenuLevel") , ERCMLevel_Child);
	Show(FALSE);
	eventCallResetChildMenuButton();

	MMOUI::GetUIManager()->SetOpenRClickChildMenu_FriendDelegate(FScriptDelegate(this, TEXT("MoveFriend")));
	MMOUI::GetUIManager()->SetOpenRClickChildMenu_TeamDelegate(FScriptDelegate(this, TEXT("ChangeTeamAuctionLimit")));
	MMOUI::GetUIManager()->SetHideRClickChildMenuDelegate(FScriptDelegate(this, TEXT("CleanUI")));
}

void UMMOUIRClickMenuChild::PostOnShow(UBOOL v)
{

}

void UMMOUIRClickMenuChild::OnNativeGameEvent(struct MMOGameEvent* evt)
{

}
void UMMOUIRClickMenuChild::ModifyChildPos( INT x , INT y , INT height , INT width )
{
	INT screenX = MMOFacade::getSingleton().GetScreenSize().X;
	INT screenY = MMOFacade::getSingleton().GetScreenSize().Y;
	INT offset = 10;
	if( x + width > screenX )
	{
		x = screenX - width - offset;
	}
	if( y + height > screenY )
	{
		y = screenY - height - offset;
	}

	//`log("y:"@y);
	SetInt( TEXT("x") , x );
	SetInt( TEXT("y") , y );
}

void UMMOUIRClickMenuChild::ASClickRightChildMenuItem(INT MenuLevel,INT ItemId)
{
	eventClickItem( MenuLevel , ItemId );
}

void UMMOUIRClickMenuChild::AddChildMenuItem(FScriptDelegate func,const FString& StrName,UBOOL UseStringMap)
{
	FuncListChild.AddItem( func );
	ShowChildClickItem( UseStringMap?(MMOUI::ParseLocalizedPropertyPath( FString(TEXT("MMOGame.GamePlay.")) + StrName )):StrName );
}

void UMMOUIRClickMenuChild::ShowChildClickItem( FString label  )
{
	UGFxObject* ButtonMc;

	ButtonMc = this->GetObject( FString::Printf( TEXT("RClicklistItem%d") , ChildStep ));
	if(ButtonMc)
	{
		ButtonMc->SetVisible(TRUE);
		ButtonMc->SetString( TEXT("label") , label );
		ChildHigh = ButtonMc->GetInt( TEXT("height") ) + ButtonMc->GetInt( TEXT("y") );
		ChildStep++;
	}
}
void UMMOUIRClickMenuChild::MoveFriend(INT x, INT y,INT id)
{
	OwnerRoleId = id;

	this->SetInt( TEXT("x") , x );
	this->SetInt( TEXT("y") , y );

	ChildStep = 0;

	for(INT i = 0;i< ButtonMaxNum;i++)
	{
		UGFxObject* mc = GetObject( FString::Printf( TEXT("RClicklistItem%d"),i) );
		if( mc )
		{
			mc->SetVisible(false);
		}
	}
	for( INT i = 0 ; i< 5 ; i++ )
	{
		UGFxObject* mc = GetObject( FString::Printf( TEXT("splitLine%d"),i) );
		if( mc )
		{
			mc->SetVisible(false);
		}
	}
	for(INT i = 0, j = 0 ; i < FriendGroupNum ; i++)
	{
		if( ChildStep >= ButtonMaxNum )
			return;
		if( FriendManager::getSingleton().GetFriendGroup(i) && !FriendManager::getSingleton().GetFriendGroup(i)->GroupName.IsEmpty() )
		{
			FScriptDelegate func(EC_EventParm);
			func.Object = this;
			func.FunctionName = "ClickMoveFriendToGroup";
			AddChildMenuItem( func , FriendManager::getSingleton().GetFriendGroup(i)->GroupName , false );
			UGFxObject* mc = GetObject( FString::Printf( TEXT("RClicklistItem%d"),j) );
			if( mc )
			{
				mc->SetInt( TEXT("itemid") , j++ );
				mc->SetInt( TEXT("groupid") , i );
			}			
		}	
	}

	UGFxObject* ChildMenuOwner = GetObject( TEXT("tF_menuOwner") );
	if( ChildMenuOwner )
	{
		ChildMenuOwner->SetText( TEXT("") );
	}
	UGFxObject* ChildMenuBg = GetObject( TEXT("RCMenuBg") );
	if( ChildMenuBg )
	{
		ChildMenuBg->SetInt( TEXT("height") , ChildHigh+10 );
	}
	
	Show(TRUE);
}

void UMMOUIRClickMenuChild::ClickMoveFriendToGroup( INT inMenuIdx )
{

	if( !CurClickButton )
	{
		return;
	}
	INT GroupId = CurClickButton->GetInt( "groupid" );
	FriendManager::getSingleton().ChangeFriendGroup( OwnerRoleId , GroupId );

	Show(FALSE);
	MMOUI::GetUIManager()->delegateCloseRClickMenu(ERCMType_Friend);
}

void UMMOUIRClickMenuChild::CleanUI()
{
	Show(FALSE);
}

void UMMOUIRClickMenuChild::ChangeTeamAuctionLimit(INT x,INT y,INT w)
{

	FuncListChild.Remove( 0, FuncListChild.Num() );

	ChildStep = 0;
	for(INT i = 0;i< ButtonMaxNum;i++)
	{
		UGFxObject* mc = GetObject( FString::Printf( TEXT("RClicklistItem%d"),i) );
		if( mc )
		{
			mc->SetVisible(false);
		}
	}
	for( INT i = 0 ; i< 5 ; i++ )
	{
		UGFxObject* mc = GetObject( FString::Printf( TEXT("splitLine%d"),i) );
		if( mc )
		{
			mc->SetVisible(false);
		}
	}

	TeamAuctionItemColorLimitList& limitList = TeamManager::getSingleton().GetLimitColorMap();
	INT auctionItemColorLimit = TeamManager::getSingleton().GetAuctionItemColorLimit();
	for(INT i = 0 ; i < limitList.Num() ; i++)
	{
		if( ChildStep >= ButtonMaxNum )
			return;


		FScriptDelegate func(EC_EventParm);
		func.Object = this;
		func.FunctionName = "ClickChangeTeamAuctionLimitColor";

		FString str;
		UBOOL sel = FALSE;
		if( auctionItemColorLimit == limitList(i).Color )
		{
			//str = UITextParser::HTMLBeginFont( UITextParser::HTMLGetItemColor( (EItemColor)limitList(i).Color ) )
			//	//+ TEXT("*")
			//	+ MMOUI::ParseLocalizedPropertyPath( limitList(i).ColorStringText )
			//	//+ TEXT("*")
			//	+ UITextParser::HTMLEndFont();

			sel = TRUE;
		}

		str = UITextParser::HTMLBeginFont( UITextParser::HTMLGetItemColor( (EItemColor)limitList(i).Color ) )
			+ MMOUI::ParseLocalizedPropertyPath( limitList(i).ColorStringText )
			+ UITextParser::HTMLEndFont();



		AddChildMenuItem( func , str , false );
		UGFxObject* mc = GetObject( FString::Printf( TEXT("RClicklistItem%d"),i) );
		if( mc )
		{
			mc->SetInt( TEXT("itemid") , i );
			mc->SetBool( TEXT("selected") , sel );
		}	

	}	


	UGFxObject* ChildMenuOwner = GetObject( TEXT("tF_menuOwner") );
	if( ChildMenuOwner )
	{
		ChildMenuOwner->SetText( TEXT("") );
	}
	UGFxObject* ChildMenuBg = GetObject( TEXT("RCMenuBg") );
	if( ChildMenuBg )
	{
		ChildMenuBg->SetInt( TEXT("height") , ChildHigh+10 );
	}

	Show( TRUE );

	//Reset submenu position
	ModifyChildPos( x , y , ChildHigh+10 , w );
}

void UMMOUIRClickMenuChild::ClickChangeTeamAuctionLimitColor(int inMenuIdx )
{
	if( !CurClickButton )
	{
		return;
	}
	INT colorIdx = CurClickButton->GetInt( "itemid" );
	INT auctionItemColorLimit = TeamManager::getSingleton().GetTeamAuctionItemColorByIdx(colorIdx);
	TeamManager::getSingleton().SendOperateTeam(  TeamManager::getSingleton().GetAssignType() , 
		TeamManager::getSingleton().GetMemberLimit() , TeamManager::getSingleton().GetTeamName() , auctionItemColorLimit );
	Show(FALSE);
	MMOUI::GetUIManager()->delegateCloseRClickMenu(ERCMType_TeamMember);
}