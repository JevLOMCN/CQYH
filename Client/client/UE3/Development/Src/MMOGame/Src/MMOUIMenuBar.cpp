#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOGameClasses.h"
#include "MMOFacade.h"
#include "MailManager.h"
#include "WebHtmlManager.h"
#include "MMOSkillManager.h"
#include "MMOSystemActivateManager.h"
#include "client_option_def.h"
#include "MMOVipManager.h"

IMPLEMENT_CLASS(UMMOUIMenuBar)

std::map<INT, INT> mapLevelChange;
void UMMOUIMenuBar::ClickMenu( INT menuId,INT menuType )
{
	FMenuInfo* menuInfo = NULL;

	menuInfo = FindMenuInfoByType( menuId , menuType );
	
	if(menuInfo)
	{	
		FMenuInfo* child = GetLastShowChildMenu( menuInfo );
		if(child)
		{
			ClickMenuChild( menuId , menuType , 0 , child->menuType );
		}
	}


}

void UMMOUIMenuBar::OverMenu(INT idx)
{
	//if( MenuInfoList.Num() > idx )
	//{
	//	if( MenuInfoList(idx).ChildList.Num() > 0 && MenuListChild )
	//	{
	//		if( OpenChildId != idx )
	//		{
	//			OpenChildId = idx;

	//			MenuListChild->SetVisible(TRUE);
	//			int childNum = MenuInfoList(idx).ChildList.Num();

	//			for ( INT i = 0 ; i < UCONST_ChildMenuButtonNum ; i++ )
	//			{
	//				UGFxObject* childButton = MenuListChild->GetObject( FString::Printf( TEXT("MenuSlot%d") , i ) );
	//				if(childButton)
	//				{
	//					if( i < childNum )
	//					{
	//						UGFxObject* icon = childButton->GetObject( TEXT("icon") );
	//						if( icon )
	//						{
	//							icon->GotoAndPlay( MenuInfoList(idx).ChildList(i).IconName );
	//						}
	//						UGFxObject* icon_over = childButton->GetObject("icon_over");
	//						if(icon_over)
	//						{
	//							icon_over->GotoAndPlay( MenuInfoList(idx).ChildList(i).IconName );
	//						}

	//						childButton->SetString( TEXT("label") , MenuInfoList(idx).ChildList(i).MenuName );

	//						UGFxObject* key = childButton->GetObject("tF_Key");
	//						if(key)
	//						{
	//							key->SetText( MenuInfoList(idx).ChildList(i).KeyName );
	//						}

	//						childButton->SetVisible(TRUE);
	//					}
	//					else
	//					{
	//						childButton->SetVisible(FALSE);
	//					}						
	//				}

	//			}

	//		}
	//		else
	//		{
	//			OpenChildId = -1;
	//			MenuListChild->SetVisible(FALSE);
	//		}

	//	}
	//}
}

void UMMOUIMenuBar::OutMenu(INT idx)
{
	//OpenChildId = -1;
	//MenuListChild->SetVisible(FALSE);
}

void UMMOUIMenuBar::ClickMenuChild( INT menuId,INT menuType,INT childMenuId,INT childMenuType )
{
	FMenuInfo* menuInfo = FindMenuInfoByType( menuId , menuType );
	if(menuInfo)
	{
		FMenuInfo* childMenuInfo = FindChildMenuInfoByType( menuInfo , childMenuId , childMenuType );
		if(childMenuInfo)
		{
			if( !childMenuInfo->PanelName.IsEmpty() )
			{
				if (CheckClientOption(childMenuType))
				{
					MMOUI::GetUIManager()->ToggleShowPanelEx( *childMenuInfo->PanelName , childMenuInfo->TabName );
				}
			}
			else
			{
				/*
				Calling out non-UI windows such as web pages
				*/
				// Unlock level already restricted
				switch( childMenuType )
				{
				case EMenuType_Child_GMService:
					{
						if(IsMenuActivated(EMenuType_Child_GMService))
						{
							OpenGameServiceWnd();
						}
					}
					break;
				case EMenuType_Child_GMWebBug:
					{
						if(IsMenuActivated(EMenuType_Child_GMWebSurvey))
						{
							WebHtmlManager::getSingleton().OpenBugSubmitUrl();
						}
					}
					break;
				case EMenuType_Child_GMWebSurvey:
					{
						if(IsMenuActivated(EMenuType_Child_GMWebSurvey))
						{
							WebHtmlManager::getSingleton().OpenSurveyUrl();
						}
					}
					break;
				case EMenuType_Child_SearchScreenShot:
					{
						if(IsMenuActivated(EMenuType_Child_SearchScreenShot))
						{
							FString screenShotDirPath = appScreenShotDir();
#ifdef SHIPPING_PC_GAME
							screenShotDirPath =  GFileManager->ConvertAbsolutePathToUserPath( *GFileManager->ConvertToAbsolutePath( *screenShotDirPath ) );
#endif
							if(appDirectoryExists(*screenShotDirPath))
							{
								TArray<FString> allFiles;
								appFindFilesInDirectory(allFiles, *screenShotDirPath, TRUE, TRUE);
								if(allFiles.Num() > 0)
								{
									appExploreFolder( *screenShotDirPath );
									break;
								}
							}

							FString msgErrorName = TEXT("MMOErrorInfo.Game.NoScreenShot");
							FString errorMsg = MMOUI::ParseLocalizedPropertyPath(msgErrorName);
							MMOUI::GetUIManager()->delegateAddScreenMessage(errorMsg, MsgInfoType_Table, msgErrorName);
							break;
						}
					}
					break;
				case EMenuType_Child_SearchLog:
					{
						if(IsMenuActivated(EMenuType_Child_SearchLog))
						{
							FString logDirPath = appGameLogDir();
#ifdef SHIPPING_PC_GAME
							logDirPath =  GFileManager->ConvertAbsolutePathToUserPath(  *GFileManager->ConvertToAbsolutePath( *logDirPath ) );
#endif
							if(!appDirectoryExists(*logDirPath))
							{
								FString msgErrorName = TEXT("MMOErrorInfo.ERROR_CODE_DEF.errSpellNoTarget");
								FString errorMsg = logDirPath + MMOUI::ParseLocalizedPropertyPath(msgErrorName);
								MMOUI::GetUIManager()->delegateAddScreenMessage(errorMsg, MsgInfoType_Table, msgErrorName);
								break;
							}

							appExploreFolder( *logDirPath );
						}
					}
					break;
				case EMenuType_Child_RechargeYuanBao:
					{
						if(IsMenuActivated(EMenuType_Child_RechargeYuanBao))
						{
							WebHtmlManager::getSingleton().OpenRechargeYuanBao();
							//WebHtmlManager::getSingleton().OpenRechargeYuanBaoByIE();
						}
					}
					break;
				}
			}

			MenuListChild->SetVisible(FALSE);
		}
	}

	//if( MenuInfoList.Num() > menuId && menuId >= 0 && idx >= 0 )
	//{
	//	
	//}
}

//====================================================================================
void UMMOUIMenuBar::Init()
{
	Super::Init();

	MMOUI::GetUIManager()->SetUpdateMainMenuKeyDownDelegate(FScriptDelegate(this, TEXT("UpdateMainMenuKeyDown")));
	MMOUI::GetUIManager()->SetUpdateMenuEffectDelegate(FScriptDelegate(this, TEXT("CallUpdateMenuEffect")));

	check(NULL == EventAdapter);
	if (NULL == EventAdapter)
	{
		TArray<EGameEvent> evts;
		evts.AddItem(GameEvent_Mail);
		evts.AddItem(GameEvent_Item_Gain);
		evts.AddItem(GameEvent_SkillUiUpdate);
		evts.AddItem(GameEvent_NewSignedIn);
		EventAdapter = new TGameEventAdapter<UMMOUIMenuBar>(this, evts);
	}

	RightMenuStartIdx = GetInt( TEXT("RightMenuStartIdx") );
}
//==============================================================================================================
void UMMOUIMenuBar::UnInit()
{
	Super::UnInit();

	MMOUI::GetUIManager()->SetUpdateMainMenuKeyDownDelegate(FScriptDelegate(EC_EventParm));
	MMOUI::GetUIManager()->SetUpdateMenuEffectDelegate(FScriptDelegate(EC_EventParm));
}

void UMMOUIMenuBar::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	
	if (evt)
	{
		switch (evt->GetEventType())
		{
		case EGameEvent::GameEvent_Mail:
			{
				MMOGameEvent_Mail* pMailEvt = (MMOGameEvent_Mail*)evt;
				ASSERT_CHECK_ERROR_RETURN(pMailEvt);
				MailCount = pMailEvt->_MailCount;
				if (UpdateMaxMailCount == pMailEvt->eMailType)
				{
					UpdateMenuButtonMaxCount( EMenuType_Mail , pMailEvt->_MailCount , TRUE );
				}
				else if (NewMail == pMailEvt->eMailType)
				{
					SetMenuButtonNewCount( EMenuType_Mail , pMailEvt->_MailCount , TRUE );
				}
				break;
			}
		case GameEvent_Item_Gain:
			{
				if( MMOUI::GetUIManager()->GetCurrentGameState() != EGameState_InGame )
				{
					return;
				}
				MMOGameEvent_Item *itemGainEvt = (MMOGameEvent_Item*)evt;

				FClientItemInfo *pkNewItem = MMOFacade::getSingleton().FindItem( itemGainEvt->ContainerIdx, itemGainEvt->SlotIdx );
				if( pkNewItem)
				{
					UItem* item = MMOFacade::getSingleton().FindItemTemplate(pkNewItem->m_nTemplateID);
					if(item)
					{
						UGFxObject* slotDat = GFXPlayer->CreateObject(TEXT("ActionScript3.ItemSlotData"));
						if(slotDat)
						{
							slotDat->SetInt(TEXT("itemColor"), item->ItemColor + 1);
							slotDat->SetString(TEXT("iconName"), MMOFacade::getSingleton().GetItemIconNamePersonalized( item, pkNewItem ));
							eventCallAddNewItemHint(slotDat);
						}
					}
				}
				break;
			}
		case GameEvent_SkillUiUpdate:
			{
				MMOGameEvent_SkillUiUpdate* skillEvent = (MMOGameEvent_SkillUiUpdate*)evt;
				switch(skillEvent->EventType)
				{
				case MMOGameEvent_SkillUiUpdate::ESkillUiEvent_NewSkillLearned:
					{
						//UMMOUISkillTree *skillTreePanel = (UMMOUISkillTree*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_SkillTree );
						//UBOOL isShowHint = !( skillTreePanel != NULL && skillTreePanel->IsVisible && skillTreePanel->CurPanel == ESkillPnl_Active );
						UMMOUISkillSettingPanel *skillSettingPanel = (UMMOUISkillSettingPanel*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_SkillSetting );
						UBOOL isShowHint = !( skillSettingPanel != NULL && skillSettingPanel->IsVisible );

						if( isShowHint ){
							OnNotifyNewSkillLearned( skillEvent->MagicInt );
						}
					}
					break;
				}
			}
			break;
		case GameEvent_NewSignedIn:
			{
				MMOGameEvent_OneInt *newSignInEvt = (MMOGameEvent_OneInt*)evt;
				ShowMenuBarHint( EMenuType_DailySignIn, EMenuType_Child_DailySignInGetReward, TRUE );
			}
			break;
		//case GameEvent_EnterGameSuccess:
		//	{
		//		ShowMenuBarHint( EMenuType_Skill, EMenuType_Child_Skill, FALSE );
		//	}
		//	break;
		}
	}
}

void UMMOUIMenuBar::ClearUI()
{

}

void UMMOUIMenuBar::InitMenuSlot()
{
	if( MenuList )
	{
		INT nLevel = MMOFacade::getSingleton().GetMyLevel();
		UpdateMenuButton(MenuInfoList, TRUE, nLevel, nLevel );
		UpdateMenuButton(RightMenuInfoList, FALSE, nLevel, nLevel);

		//Preprocessing for level unlocking
		mapLevelChange.clear();
		for(INT i=0; i<MenuInfoList.Num(); i++)
		{
			mapLevelChange.insert(make_pair(MenuInfoList(i).RequireLevel,1));
			for(INT j=0;j<MenuInfoList(i).ChildList.Num() ;j++)
			{
				mapLevelChange.insert(make_pair(MenuInfoList(i).ChildList(j).RequireLevel,2));
			}
		}

		for(INT i=0; i<RightMenuInfoList.Num(); i++)
		{
			mapLevelChange.insert(make_pair(RightMenuInfoList(i).RequireLevel,1));
			for(INT j=0;j<RightMenuInfoList(i).ChildList.Num() ;j++)
			{
				mapLevelChange.insert(make_pair(RightMenuInfoList(i).ChildList(j).RequireLevel,2));
			}
		}

		return;
	}
	
}

void UMMOUIMenuBar::PlayerLevelChange(INT lastLv,INT newLv)
{
	if( lastLv != newLv )
	{
		INT LevelChangeFlag = 0;
		if(newLv==lastLv+1)//normal level up
		{
			std::map<INT, INT>::iterator pos = mapLevelChange.find(newLv);
			if(pos!=mapLevelChange.end())
			{
				LevelChangeFlag = pos->second;
			}
		}
		else
		{
			LevelChangeFlag=2;
		}

		if(LevelChangeFlag>0)
		{
			UpdateMenuButton(MenuInfoList, TRUE, lastLv, newLv);
			UpdateMenuButton(RightMenuInfoList, FALSE, lastLv, newLv);
		}

//		UpdateMenuButtonMaxCount( EMenuType_Mail , MailCount , TRUE );
//		SetMenuButtonNewCount( EMenuType_Mail , MailCount , TRUE );
	}
	
}

void UMMOUIMenuBar::UpdateMenuButton( TArrayNoInit<struct FMenuInfo>& menuInfo , UBOOL left , INT lastLv , INT newLv )
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(!player)
	{
		return;
	}
	UMMOKeyManager* UKM = UMMOKeyManager::getSingleton();
	FString kkey, kname;
	UGFxObject* menuListData = GFXPlayer->CreateArray();
	if( menuListData )
	{
		//var a:Object = {iconName:"bag",childMenuList:[{menuName:"Bag",iconName:"hero",keyName:"B",panelName:"Bag"},{menuName:"Hero",iconName:"hero",keyName:"Y",panelName:"Hero"}]};
		INT factIdx = 0;
		for ( INT idx = 0 ; idx < menuInfo.Num() ; idx++ )
		{
			//if( menuInfo(idx).RequireLevel > newLv )
			//{
			//	//Required level not reached, do not show
			//	//If changing from hidden to shown here, an effect can be added
			//	continue;
			//}

			UGFxObject* menu = GFXPlayer->CreateObject( TEXT("Object") );
			if( menu )
			{
				menu->SetString( TEXT("iconName") , menuInfo(idx).IconName );
				menu->SetInt( TEXT("menuType") , (INT)(menuInfo(idx).menuType) );

				if (menuInfo(idx).RequireLevel > newLv)
				{
					menu->SetBool(TEXT("MenuVisible"),TRUE);//required level not reached, do not show
				}
				else
				{
					menu->SetBool(TEXT("MenuVisible"),TRUE);
				}

				UGFxObject* componentList = GFXPlayer->CreateArray();
				if(componentList)
				{
					for ( INT componentIdx = 0 ; componentIdx < menuInfo(idx).ComponentList.Num() ; componentIdx++ )
					{
						componentList->SetElementString( componentIdx , menuInfo(idx).ComponentList(componentIdx) );
					}
					menu->SetObject( TEXT("componentList") , componentList );
				}

				UGFxObject* childMenuList = GFXPlayer->CreateArray();
				if( childMenuList )
				{
					INT factChildIdx = 0;
					for ( INT child = 0 ; child < menuInfo(idx).ChildList.Num() ; child++ )
					{
						//if( menuInfo(idx).ChildList(child).RequireLevel > newLv )
						//{
						//	//Required level not reached, do not show
						//	continue;
						//}

						UGFxObject* childMenu = GFXPlayer->CreateObject( TEXT("Object") );
						if( childMenu )
						{
							childMenu->SetString( TEXT("menuName") , menuInfo(idx).ChildList(child).MenuName );
							childMenu->SetString( TEXT("iconName") , menuInfo(idx).ChildList(child).IconName );
							childMenu->SetString( TEXT("keyName") , menuInfo(idx).ChildList(child).KeyName );
							childMenu->SetString( TEXT("panelName") , menuInfo(idx).ChildList(child).PanelName );
							childMenu->SetInt( TEXT("menuType") , (INT)(menuInfo(idx).ChildList(child).menuType) );
							
							if (menuInfo(idx).ChildList(child).RequireLevel > newLv || IsMenuActivated(menuInfo(idx).ChildList(child).menuType)==FALSE	)
							{
								childMenu->SetBool(TEXT("MenuVisible"),TRUE);//required level not reached, do not show
							}
							else
							{
								childMenu->SetBool(TEXT("MenuVisible"),TRUE);
							}
							

							if (UKM->GetFuncInfoByPanel(menuInfo(idx).ChildList(child).PanelName,
								menuInfo(idx).ChildList(child).TabName,	kkey, kname))
							{
								childMenu->SetString( TEXT("menuName") , kname );
								childMenu->SetString( TEXT("keyName") , kkey );
							}
							else
								childMenu->SetString( TEXT("keyName") , _T(""));
						}
						childMenuList->SetElementObject( factChildIdx , childMenu );
						factChildIdx++;
					}
					menu->SetObject( TEXT("childMenuList") , childMenuList );
				}

				menuListData->SetElementObject( factIdx , menu );
				factIdx++;
			}				
		}

		if(left)
		{
			eventCallSetLeftMenuData(menuListData);
		}
		else
		{
			eventCallSetRightMenuData(menuListData);
		}
		
	}
}

UBOOL UMMOUIMenuBar::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	if (strHoverControllerName == "menubk_mc" || 
		strHoverControllerName == "guide_right_mc" ||
		strHoverControllerName == "guide_left_mc" ||
		strHoverControllerName == "title_mc" ||
		strHoverControllerName == "name_tf" ||
		strHoverControllerName == "loader_mc" ||
		strHoverControllerName == "new_system_mc" ||
		strHoverControllerName == "new_system_bg_mc" )
	{
		return TRUE;
	}
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace );
}


void UMMOUIMenuBar::UpdateMainMenuKeyDown(UBOOL bRelease, INT InCmdID, INT InSlotID)
{
	
	UGFxObject* mc = NULL;
	UGFxObject* mc2 = NULL;


	switch(InSlotID)
	{
	case ECK_HotKey_System:
		mc = MenuList->GetObject( TEXT("MenuSlot0") );
		break;
	case ECK_HotKey_Achievement:
		mc = MenuList->GetObject( TEXT("MenuSlot1") );
		break;
	case ECK_HotKey_Hero:
		mc = MenuList->GetObject( TEXT("MenuSlot2") );
		break;
	case ECK_HotKey_Team:
		mc = MenuList->GetObject( TEXT("MenuSlot3") );
		break;

	case ECK_HotKey_GuildMain:
		mc = MenuList->GetObject( TEXT("MenuSlot4") );
		mc2 = MenuListChild->GetObject( TEXT("MenuSlot0") );
		break;
	case ECK_HotKey_Friend:
		mc = MenuList->GetObject( TEXT("MenuSlot4") );
		mc2 = MenuListChild->GetObject( TEXT("MenuSlot1") );
		break;
	case ECK_HotKey_Master:
		mc = MenuList->GetObject( TEXT("MenuSlot4") );
		mc2 = MenuListChild->GetObject( TEXT("MenuSlot2") );
		break;


	case ECK_HotKey_Quest:
		mc = MenuList->GetObject( TEXT("MenuSlot5") );
		break;
	case ECK_HotKey_SkillSetting:
		mc = MenuList->GetObject( TEXT("MenuSlot6") );
		break;
	case ECK_HotKey_Bag:
		mc = MenuList->GetObject( TEXT("MenuSlot7") );
		break;
	case ECK_HotKey_Charactor:
		mc = MenuList->GetObject( TEXT("MenuSlot8") );
		break;
	}

	if(mc)
	{
		if(bRelease) mc->GotoAndPlay("kb_down");
		else		  mc->GotoAndPlay("kb_release");	
	}

	if(mc2)
	{
		if(bRelease) mc2->GotoAndPlay("kb_down");
		else		  mc2->GotoAndPlay("kb_release");	
	}

}

FMenuInfo* UMMOUIMenuBar::FindMenuInfoByType( INT menuIdx , INT menuType )
{
	
	if( menuIdx >= RightMenuStartIdx )
	{
		for ( INT i = 0 ; i < RightMenuInfoList.Num() ; i++ )
		{
			if( RightMenuInfoList(i).menuType == menuType )
			{
				return &RightMenuInfoList(i);
			}
		}
	}
	else
	{
		for ( INT i = 0 ; i < MenuInfoList.Num() ; i++ )
		{
			if( MenuInfoList(i).menuType == menuType )
			{
				return &MenuInfoList(i);
			}
		}
	}
	return NULL;
}

FMenuInfo* UMMOUIMenuBar::GetLastShowChildMenu( FMenuInfo* menuInfo )
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(player)
	{
		INT level = player->GetActorLevel();
		for ( INT i = menuInfo->ChildList.Num() - 1 ; i >= 0  ; i-- )
		{
			if( level >= menuInfo->ChildList(i).RequireLevel )
			{
				return &menuInfo->ChildList(i);
			}
		}
	}
	return NULL;
}

FMenuInfo* UMMOUIMenuBar::FindChildMenuInfoByType( FMenuInfo* menuInfo , INT childMenuIdx , INT childMenuType )
{
	if( menuInfo )
	{
		for ( INT i = 0 ; i < menuInfo->ChildList.Num() ; i++ )
		{
			if( menuInfo->ChildList(i).menuType == childMenuType )
			{
				return &menuInfo->ChildList(i);
			}
		}
	}
	return NULL;
}

INT UMMOUIMenuBar::GetMenuIdxByMenuType( INT type , UBOOL left )
{
	INT factIdx = 0;
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(!player)
		return -1;

	INT level = player->GetActorLevel();
	if( left )
	{
		for ( INT i = 0 ; i < MenuInfoList.Num() ; i++ )
		{
			//if( level >= MenuInfoList(i).RequireLevel )
			//{
				if( MenuInfoList(i).menuType == type  )
				{
					return factIdx;
				}				
				factIdx++;
			//}
		}
	}
	else
	{
		for ( INT i = 0 ; i < RightMenuInfoList.Num() ; i++ )
		{
			//if( level >= RightMenuInfoList(i).RequireLevel )
			//{
				if( RightMenuInfoList(i).menuType == type )
				{
					return factIdx;
				}
				factIdx++;
			//}
			
		}
	}

	return -1;
}

UBOOL UMMOUIMenuBar::IsInLeftMenuBar( INT type )
{
	switch( type )
	{
	case EMenuType_Charactor:
	case EMenuType_Bag:
	case EMenuType_Skill:
	case EMenuType_Guild:
	case EMenuType_Quest:
	case EMenuType_Riding:
	case EMenuType_Option:
		return FALSE;
		break;
	default:
		return TRUE;
		break;
	}

	return TRUE;
}
void UMMOUIMenuBar::UpdateMenuButtonMaxCount( INT type , INT count , UBOOL left )
{
	INT idx = GetMenuIdxByMenuType( type , left );
	if( idx >= 0 )
	{
		eventCallUpdateMaxMailCount( idx , count , left );
	}
	
}

void UMMOUIMenuBar::SetMenuButtonNewCount( INT type , INT count , UBOOL left )
{
	INT idx = GetMenuIdxByMenuType( type , left );
	if( idx >= 0 )
	{
		eventCallASSetNewMailCount( idx , count , left );
	}
	
}

UBOOL UMMOUIMenuBar::IsMenuActivated(INT childMenuType)
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if (!player)
	{
		return FALSE;
	}

	for (INT i = 0; i < MenuInfoList.Num(); ++ i)
	{
		for (INT j = 0; j < MenuInfoList(i).ChildList.Num(); ++ j)
		{
			INT type = (UINT32)(MenuInfoList(i).ChildList(j).menuType);
			if (type == childMenuType )
			{
				if (player->GetActorLevel() >= MenuInfoList(i).ChildList(j).RequireLevel)
				{
					if (CheckClientOption(childMenuType))
					{
						return TRUE;
					}
					
				}
				
			}
		}
	}

	for (INT i = 0; i < RightMenuInfoList.Num(); ++ i)
	{
		for (INT j = 0; j < RightMenuInfoList(i).ChildList.Num(); ++ j)
		{
			INT type = (UINT32)(RightMenuInfoList(i).ChildList(j).menuType);
			if (type == childMenuType)
			{
				if(player->GetActorLevel() >= RightMenuInfoList(i).ChildList(j).RequireLevel)
					return TRUE;
			}
		}
	}

	return FALSE;
}

void UMMOUIMenuBar::ShowNewSystem(INT systemType)
{
	// Do not show new system unlock hints in Debug, developers found it annoying
#ifdef _DEBUG
	return;
#else
	INT childMenuType = MMOSystemActivateManager::getSingleton().GetChildMenuType(systemType);

	FString icon = TEXT("");
	FString name = TEXT("");
	MMOSystemActivateManager::getSingleton().GetSystemInfo(systemType, icon, name);

	for (INT i = 0; i < MenuInfoList.Num(); ++ i)
	{
		for (INT j = 0; j < MenuInfoList(i).ChildList.Num(); ++ j)
		{
			if ((UINT32)(MenuInfoList(i).ChildList(j).menuType) == childMenuType)
			{
				UGFxObject* object = GFXPlayer->CreateObject("Object");
				if (object)
				{
					FVector2D screenSize = MMOFacade::GetScreenSize();
					object->SetInt(TEXT("src_x"), screenSize.X / 2);
					object->SetInt(TEXT("src_y"), screenSize.Y - 360);
					object->SetInt(TEXT("new_system_id"), systemType);
					object->SetString(TEXT("new_system_icon"), icon);
					object->SetString(TEXT("new_system_name"), name);
					eventCallShowNewSystem(object);
				}
				return;
			}
		}
	}

	for (INT i = 0; i < RightMenuInfoList.Num(); ++ i)
	{
		for (INT j = 0; j < RightMenuInfoList(i).ChildList.Num(); ++ j)
		{
			if ((UINT32)(RightMenuInfoList(i).ChildList(j).menuType) == childMenuType)
			{
				UGFxObject* object = GFXPlayer->CreateObject("Object");
				if (object)
				{
					FVector2D screenSize = MMOFacade::GetScreenSize();
					object->SetInt(TEXT("src_x"), screenSize.X / 2);
					object->SetInt(TEXT("src_y"), screenSize.Y - 360);
					object->SetInt(TEXT("new_system_id"), systemType);
					object->SetString(TEXT("new_system_icon"), icon);
					object->SetString(TEXT("new_system_name"), name);
					eventCallShowNewSystem(object);
				}
				return;
			}
		}
	}
#endif
}

void UMMOUIMenuBar::ActivateNewSystem(INT systemType)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (mainPlayer)
	{
		//UpdateMenuButton(MenuInfoList, TRUE, 0, mainPlayer->GetActorLevel());
		//UpdateMenuButton(RightMenuInfoList, FALSE, 0, mainPlayer->GetActorLevel());
	}
	
	INT childMenuType = MMOSystemActivateManager::getSingleton().GetChildMenuType(systemType);
	FVector2D screenSize = MMOFacade::GetScreenSize();
	FVector2D dest = FVector2D(0.0, 0.0);

	for (INT i = 0; i < MenuInfoList.Num(); ++ i)
	{
		for (INT j = 0; j < MenuInfoList(i).ChildList.Num(); ++ j)
		{
			if (MenuInfoList(i).ChildList(j).menuType == childMenuType)
			{
				dest.X = 111;
				dest.Y = 0;

				UGFxObject* object = GFXPlayer->CreateObject("Object");
				if (object)
				{
					FVector2D screenSize = MMOFacade::GetScreenSize();
					object->SetInt(TEXT("dest_x"), dest.X);
					object->SetInt(TEXT("dest_y"), dest.Y);
					eventCallActivateNewSystem(object);
				}
				return;
			}
		}
	}

	for (INT i = 0; i < RightMenuInfoList.Num(); ++ i)
	{
		for (INT j = 0; j < RightMenuInfoList(i).ChildList.Num(); ++ j)
		{
			if (RightMenuInfoList(i).ChildList(j).menuType == childMenuType)
			{
				dest.X = screenSize.X - 111;
				dest.Y = screenSize.Y - 28;

				UGFxObject* object = GFXPlayer->CreateObject("Object");
				if (object)
				{
					FVector2D screenSize = MMOFacade::GetScreenSize();
					object->SetInt(TEXT("dest_x"), dest.X);
					object->SetInt(TEXT("dest_y"), dest.Y);
					eventCallActivateNewSystem(object);
				}
				return;
			}
		}
	}
}

//New Skil Hint
//=========================================================
void UMMOUIMenuBar::OnNotifyNewSkillLearned( INT uniqueSkillID )
{
	ShowMenuBarHint( EMenuType_Skill, EMenuType_Child_Skill, TRUE );

	//if( NewSkillHintBtn == NULL ){
	//	return;
	//}
	//if( NewSkillHintBtn->GetBool( TEXT("visible") ) ){
	//	return;
	//}
	//NewSkillHintBtn->SetVisible( TRUE );

	////
	//UGFxObject *pNewSkillHintMc = NewSkillHintBtn->GetObject( TEXT("newSkillAnimHintMc") );
	//if( pNewSkillHintMc != NULL ){
	//	pNewSkillHintMc->SetVisible( FALSE );
	//	pNewSkillHintMc->GotoAndPlayI( 1 );
	//}
}

//----------------------------------------------------------------
void UMMOUIMenuBar::ShowMenuBarHint( BYTE majorMenuType, BYTE minorMenuType, UBOOL isShow )
{
	UBOOL isLeftMenuBar = IsInLeftMenuBar( majorMenuType );
	INT majorMenuBtnIdx = GetMenuIdxByMenuType( majorMenuType, isLeftMenuBar );
	eventCallMenuBarHintEffect( majorMenuBtnIdx, minorMenuType, isLeftMenuBar, isShow );
}
//
void UMMOUIMenuBar::OpenGameServiceWnd()
{
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainPlayer == NULL ){
		return;
	}

	INT playerLv = mainPlayer->GetActorLevel();

	FString userInfo;

	WebHtmlManager::getSingleton().OpenGameService( mainPlayer->ActorName, playerLv );
}
//===============================================================================================================
//===============================================================================================================
// menu bar
void UMMOUIMenuBar::ASActivateNewSystem(INT systemType)
{
	ActivateNewSystem( systemType );
}
//=================================================================================
void UMMOUIMenuBar::ASShowNextNewSystem()
{
	INT systemType = MMOSystemActivateManager::getSingleton().GetNextSystemType();
	if (systemType == EAST_Tatol){
		return;
	}
	ShowNewSystem( systemType );
}
//=================================================================================
void UMMOUIMenuBar::ASMenuClick( INT menuId,INT menuType )
{
	ClickMenu( menuId , menuType );
}

void UMMOUIMenuBar::ASMenuChildClick( INT menuId,INT menuType,INT childMenuId,INT childMenuType )
{
	ClickMenuChild( menuId , menuType , childMenuId , childMenuType );
}
//===============================================================================================================
//===============================================================================================================

BOOL UMMOUIMenuBar::CheckClientOption(INT type)
{
	if (type == EMenuType_Child_Auction)
	{
		if ((CMessageHandler::GetInstance().GetClientOption() & CLIENT_OPTION_BIT_AUCTION))	
		{
			if (MMOVipManager::getSingleton().IsLimitedGamePlayMode())
			{
				FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errFreeMode"));
				MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errFreeMode"));
				return FALSE;
			}
			else
			{
				return TRUE;
			}
			
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return TRUE;
	}
}