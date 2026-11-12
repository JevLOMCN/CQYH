#include "stdafx.h"
//#include "MMOGameClasses.h"
#include "MMOFacade.h"
#include "LinkManager.h"
#include "FSMIO.h"
#include "UITextParser.h"
#include "TipsFormatter.h"
#include "MMOQuestManager.h"
#include "MMOMapDataManager.h"
#include "MMOAchievementManager.h"
#include "MMONavigationManager.h"
#include "MMOGuideManager.h"
#include "MMOChatManager.h"
#include "DoodadManager.h"
#include "WebHtmlManager.h"
#include "MMOMapDataManager.h"
#include "MMOAuctionManager.h"

#define LINK_START(TYPE)	FString(TEXT("<A HREF=\"event:,"))+LinkNode((UINT)(TYPE))
#define LINK_START_CHAT(TYPE)	FString(TEXT(" <A HREF=\"event:,"))+LinkNode((UINT)(TYPE))
#define LINK_NODE(A)		FString(TEXT(","))+LinkNode(A)
#define LINK_STRINGNODE(A)  FString(TEXT(",")) + A
#define LINK_END(TXT)		FString(TEXT("\">"))+TXT+FString(TEXT("</A>"))
#define LINK_ENDU(TXT)		FString(TEXT("\"><U>"))+TXT+FString(TEXT("</U></A>"))

#define GET_LINK_START()	int Index=2;
#define GET_LINK_NODE(A)	GetLinkNode(parameterArray->GetElementString(Index++),A)

template<> LinkManager* Singleton<LinkManager>::ms_Singleton = 0;

LinkManager::FCallbackLinkFuns LinkManager::g_linksCallbackFun[ELinkType_MAX];


UBOOL LinkManager::NullCallBackLinkOverOut( class UGFxObject* parameterArray )
{
	return TRUE;
}

LinkManager::LinkManager(void)
{
#define M_REG_LINK_PROC_FUN(e, click) g_linksCallbackFun[e] = FCallbackLinkFuns(click, click##Over, click##Out);
#define M_REG_LINK_PROC_FUNEX(e, click, over, out) g_linksCallbackFun[e] = FCallbackLinkFuns(click, over, out);

	M_REG_LINK_PROC_FUN(ELinkType_NPCNav, CallBackLink_NPCNav);
	M_REG_LINK_PROC_FUN(ELinkType_NavFly, CallBackLink_NavFly);
	M_REG_LINK_PROC_FUN(ELinkType_ChatPlayer, CallBackLink_ChatPlayer);
	M_REG_LINK_PROC_FUN(ELinkType_KillInfoPlayer, CallBackLink_KillInfoPlayer);
	M_REG_LINK_PROC_FUN(ELinkType_CoordNav, CallBackLink_CoordNav);
	M_REG_LINK_PROC_FUN(ELinkType_NavPointName, CallBackLink_NavPointName);
	M_REG_LINK_PROC_FUN(ELinkType_ItemTip, CallBackLink_Item);
	M_REG_LINK_PROC_FUN(ELinkType_QuestTraceTitle, CallBackLink_QuestTraceTitle);
	M_REG_LINK_PROC_FUN(ELinkType_QuestTraceAccept, CallBackLink_QuestTraceAccept);
	M_REG_LINK_PROC_FUN(ELinkType_Achievement, CallBackLink_Achievement);
	M_REG_LINK_PROC_FUN(ELinkType_AchievementClass, CallBackLink_AchievementClass);
	M_REG_LINK_PROC_FUN(ELinkType_OpenPanel, CallBackLink_OpenPanel);
	M_REG_LINK_PROC_FUN(ELinkType_DoodadNav, CallBackLink_DoodadNav);
	M_REG_LINK_PROC_FUN(ELinkType_ItemNav, CallBackLink_ItemNav);
	M_REG_LINK_PROC_FUN(ELinkType_WebPath, CallBackLink_OpenWebPath);
	M_REG_LINK_PROC_FUN(ELinkType_WebPath, CallBackLink_OpenWebPath);
	M_REG_LINK_PROC_FUN(ELinkType_OpenMap, CallBackLink_OpenMap);
	M_REG_LINK_PROC_FUN(ELinkType_ChatItemtTip, CallBackLink_ChatItem);
	M_REG_LINK_PROC_FUN(ELinkTpye_AuctionItem, CallBackLink_ChatItem_Auction);
	M_REG_LINK_PROC_FUN(ELinkType_AuctionPlayer, CallBackLink_AuctionPlayer);
}

LinkManager::~LinkManager(void)
{
}

FString	LinkManager::LinkNode(FString V)
{
	return V;
}
FString	LinkManager::LinkNode(UINT V)
{
	return appItoa(V);
}
FString	LinkManager::LinkNode(INT V)
{
	return appItoa(V);
}
FString	LinkManager::LinkNode(FLOAT V)
{
	return FString::Printf( TEXT("%f"), V );
}


void	LinkManager::GetLinkNode(FString Node, FString& V)
{
	V = Node;
}

void	LinkManager::GetLinkNode(FString Node, UINT& V)
{
	V = (UINT)appAtoi(*Node);
}

void	LinkManager::GetLinkNode(FString Node, INT& V)
{
	V = appAtoi(*Node);
}

void	LinkManager::GetLinkNode(FString Node, FLOAT& V)
{
	V = appAtof(*Node);
}

UBOOL	LinkManager::CallBackLink( class UGFxObject* parameterArray , INT buttonIdx )
{
	if(parameterArray==NULL)
	{
		return FALSE;
	}

	FString linkTypeStr = parameterArray->GetElementString(1);
	INT linkType = appAtoi(*linkTypeStr);

	if((linkType >= 0) && (linkType < ELinkType_MAX))
	{
		if(g_linksCallbackFun[linkType].MouseClickProcess)
		{
			g_linksCallbackFun[linkType].MouseClickProcess(parameterArray,buttonIdx);
		}
		return TRUE;
	}
		
	return FALSE;
}



UBOOL LinkManager::CallBackLinkOver( class UGFxObject* parameterArray )
{
	if(parameterArray==NULL)
	{
		return FALSE;
	}
	
	FString linkTypeStr = parameterArray->GetElementString(1);
	
	INT linkType = appAtoi(*linkTypeStr);

	if((linkType >= 0) && (linkType < ELinkType_MAX))
	{
		if(g_linksCallbackFun[linkType].MouseOverProcess)
		{
			g_linksCallbackFun[linkType].MouseOverProcess(parameterArray);
		}
		return TRUE;
	}

	return FALSE;
}

UBOOL LinkManager::CallBackLinkOut( class UGFxObject* parameterArray )
{
	if(parameterArray==NULL)
	{
		return FALSE;
	}

	FString linkTypeStr = parameterArray->GetElementString(1);
	INT linkType = appAtoi(*linkTypeStr);

	if((linkType >= 0) && (linkType < ELinkType_MAX))
	{
		if(g_linksCallbackFun[linkType].MouseOutProcess)
		{
			g_linksCallbackFun[linkType].MouseOutProcess(parameterArray);
		}
		return TRUE;
	}

	return FALSE;
}


void LinkManager::_ShowMapInfoTips(INT mapId, INT x, INT y)
{
	// Temporarily disabled
#if 0
	FExportMap* mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer(mapId);
	if (!mapInfo)
	{
		return;
	}

	FString mapName = mapInfo->MapDisplayName;
	FString pos = TEXT("");
	if (x != 0 || y != 0)
	{
		x = x / 100;
		y = y / 100;
		pos = FString::Printf(TEXT("\n(%d, %d)"), x, y);
	}

	FString tipsText = mapName + pos;
	MMOUI::GetUIManager()->delegateShowTextTips(tipsText);
#endif
}

void LinkManager::_HideMapInfoTips()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//
// Npc nav
//
FString LinkManager::HtmlLink_NPCNav(const FString& linkString, UINT NpcTemplateID, UINT MapID, UBOOL needMove)
{
	if (MapID == 0)
	{
		MapID = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	}

	FString res = LINK_START(ELinkType_NPCNav)
		+LINK_NODE(NpcTemplateID)
		+LINK_NODE(MapID)
		+LINK_NODE((UINT)needMove)
		+LINK_ENDU(linkString);
	
	return res;
}

UBOOL LinkManager::CallBackLink_NPCNav(class UGFxObject* parameterArray,INT buttonIdx)
{
	UINT MapID;
	UINT NpcTemplateID;
	UINT NeedMove;
	GET_LINK_START();
	GET_LINK_NODE(NpcTemplateID);
	GET_LINK_NODE(MapID);
	GET_LINK_NODE(NeedMove);

	// Tutorial event
	MMOGameEvent_Guide evtGuide(EGUET_ClickNaviLink);
	MMOUI::PostGameEvent(&evtGuide);

	return MMONavigationManager::getSingleton().Navigate(MapID, NpcTemplateID, NeedMove > 0);
}


UBOOL LinkManager::CallBackLink_NPCNavOver( class UGFxObject* parameterArray )
{
	UINT MapID;
	UINT NpcTemplateID; 
	UINT NeedMove;
	GET_LINK_START();
	GET_LINK_NODE(NpcTemplateID);
	GET_LINK_NODE(MapID);
	GET_LINK_NODE(NeedMove);

	INT x = 0;
	INT y = 0;
	const TArray<FNPCInstanceInfo*>& npcInMapInfoList = *(MMOMapDataManager::getSingleton().GetNpcInstanceInMap(MapID));
	for(INT i = 0; i < npcInMapInfoList.Num(); ++ i)
	{
		FNPCInstanceInfo* npcInfo = npcInMapInfoList(i);
		if (npcInfo && npcInfo->TemplateID == NpcTemplateID)
		{
			x = npcInfo->Position.X;
			y = npcInfo->Position.Y;
			break;
		}
	}

	_ShowMapInfoTips(MapID, x, y);

	return TRUE;
}

UBOOL LinkManager::CallBackLink_NPCNavOut( class UGFxObject* parameterArray )
{
	_HideMapInfoTips();
	return TRUE;
}

//
// Fly nav
//
FString LinkManager::HtmlLink_NavFly( const FString& linkString, UINT questId, UINT destId )
{
	FString res = LINK_START(ELinkType_NavFly)
		+LINK_NODE(questId)
		+LINK_NODE(destId)
		+LINK_ENDU(TEXT("<IMG SRC='img://UI_Quest.FlyShoes' />"));
	
	return res;
}

UBOOL	LinkManager::CallBackLink_NavFly(class UGFxObject* parameterArray,INT buttonIdx)
{
	UINT questId; 
	UINT destId;
	GET_LINK_START();
	GET_LINK_NODE(questId);
	GET_LINK_NODE(destId);
	
	// Tutorial event
	MMOGameEvent_Guide evtGuide(EGUET_ClickFlyLink);
	MMOUI::PostGameEvent(&evtGuide);

	if (destId > 7)
	{
		return FALSE;
	}

	return MMOQuestManager::getSingleton().FlyToDestination(questId, destId);
}


UBOOL LinkManager::CallBackLink_NavFlyOver( class UGFxObject* parameterArray )
{
	return TRUE;
}

UBOOL LinkManager::CallBackLink_NavFlyOut( class UGFxObject* parameterArray )
{
	return TRUE;
}

//
// Coord nav
//
FString LinkManager::HtmlLink_CoordNav( const FString& linkString, UINT MapID,FLOAT x,FLOAT y,FLOAT z )
{
	if (MapID == 0)
	{
		MapID = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	}

	FString res = LINK_START(ELinkType_CoordNav)		
		+LINK_NODE(MapID)
		+LINK_NODE(x)
		+LINK_NODE(y)
		+LINK_NODE(z)
		+LINK_ENDU(linkString);
	
	return res;
}

UBOOL LinkManager::CallBackLink_CoordNav( class UGFxObject* parameterArray , INT buttonIdx )
{
	UINT MapID;FLOAT x;FLOAT y;FLOAT z;
	GET_LINK_START();
	GET_LINK_NODE(MapID);
	GET_LINK_NODE(x);
	GET_LINK_NODE(y);
	GET_LINK_NODE(z);
	
	//todo: mapid

	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if(ctrl)
	{		
		ctrl->eventMoveToPoint(FVector(x, y, z));
		return TRUE;
	}

	return FALSE;
}


UBOOL LinkManager::CallBackLink_CoordNavOver( class UGFxObject* parameterArray )
{
	UINT MapID;FLOAT x;FLOAT y;FLOAT z;
	GET_LINK_START();
	GET_LINK_NODE(MapID);
	GET_LINK_NODE(x);
	GET_LINK_NODE(y);
	GET_LINK_NODE(z);

	_ShowMapInfoTips(MapID, (INT)x, (INT)y);
	return TRUE;
}

UBOOL LinkManager::CallBackLink_CoordNavOut( class UGFxObject* parameterArray )
{
	_HideMapInfoTips();
	return TRUE;
}

//
// Item
//
FString LinkManager::HtmlLink_Item( UItem* itemTemplate )
{
	FString str;
	if(itemTemplate)
	{
		FString itemName = UITextParser::HTMLFormatItemName(itemTemplate);
		str = LINK_START(ELinkType_ItemTip)
			+ LINK_NODE(itemTemplate->GetTemplateID())
			+ LINK_ENDU(itemName);		
	}

	return str;
}
FString LinkManager::HtmlLink_ColorItem(UItem* itemTemplate)
{
	FString str;
	if(itemTemplate)
	{
		FString itemName = UITextParser::HTMLFormatItemName(itemTemplate);
		str = FString(TEXT(" "))
			//+ UITextParser::HTMLBeginFont( UITextParser::HTMLGetItemColor( (EItemColor)itemTemplate->ItemColor ) )
			+ LINK_START(ELinkType_ItemTip)
			+ LINK_NODE(itemTemplate->GetTemplateID())
			+ LINK_ENDU(itemName);
			//+ UITextParser::HTMLEndFont();
	}

	return str;
}
FString LinkManager::HtmlLink_ColorItem( UItem* itemTemplate, FClientItemInfo* pItemInfo, LINK_ITEM_TYPE type)
{
	FString str;
	if(itemTemplate && pItemInfo)
	{
		FString itemName = UITextParser::HTMLFormatItemName(itemTemplate);
		FString itemType;
		if (type == LIT_ITEM)
		{
			itemType = LINK_START(ELinkType_ItemTip);
		}
		else if(type == LIT_CHATITEM)
		{
			itemType = LINK_START_CHAT(ELinkType_ChatItemtTip);
		}
		else if(type == LIT_AHITEM)
		{
			itemType = LINK_START(ELinkTpye_AuctionItem);
		}
		str = FString(TEXT(" "))
			+ itemType
			+ LINK_NODE(itemTemplate->GetTemplateID());
		if (type == LIT_AHITEM)
		{
			str = str + LINK_NODE(pItemInfo->AuctionId); 
		}
		str = str
			+ LINK_STRINGNODE(pItemInfo->ToReadableString())
			+ FString(TEXT("\"><U>"))
			//+ UITextParser::HTMLBeginFont( UITextParser::HTMLGetItemColor( (EItemColor)itemTemplate->ItemColor ) )
			+ FString(TEXT("&lt;"))
			+ itemName
			+ FString(TEXT("&gt;"));
			//+ UITextParser::HTMLEndFont();
		if (type == LIT_AHITEM)
		{
			str += FString(TEXT("</U></A>"));
			str += FString(TEXT("<img src='img://MMO_UIIcon.ChatChannelIcon.auction'/> "));
		}
		else
		{
			str += FString(TEXT("</U></A> "));
		}
			
	}

	return str;
}

UBOOL LinkManager::CallBackLink_Item( class UGFxObject* parameterArray , INT buttonIdx )
{
	return TRUE;
}

UBOOL LinkManager::CallBackLink_ItemOver( class UGFxObject* parameterArray )
{
	UINT itemTempID = 0;
	GET_LINK_START();
	GET_LINK_NODE(itemTempID);

	//UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(itemTempID);
	//if(itemTemp)
	//{
	//	UGFxMoviePlayer* gfxPlayer = MMOUI::GetUIManager()->GetCommonUIComponentMoviePlayer();
	//	UGFxObject* data = gfxPlayer->CreateObject(TEXT("ActionScript3.Tips.ItemTipData"));
	//	TipsFormatter_Item::FormatItemTip(data, itemTemp);
	//}
	MMOUI::GetUIManager()->ShowItemLinkTips(parameterArray);

	return TRUE;
}

UBOOL LinkManager::CallBackLink_ItemOut( class UGFxObject* parameterArray )
{
	//MMOUI::GetUIManager()->delegateHideTips();
	UMMOUITips *tipsPanel = (UMMOUITips*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_DynamicUI_Tips );
	if( tipsPanel != NULL ){
		tipsPanel->HideTips();
		tipsPanel->IsVisible = FALSE;
	}
	return TRUE;
}

UBOOL LinkManager::CallBackLink_ChatItem( class UGFxObject* parameterArray , INT buttonIdx )
{
	return TRUE;
}

UBOOL LinkManager::CallBackLink_ChatItemOver( class UGFxObject* parameterArray )
{
	UINT itemTempID = 0;
	GET_LINK_START();
	GET_LINK_NODE(itemTempID);

	MMOUI::GetUIManager()->ShowItemLinkTips(parameterArray);

	return TRUE;
}

UBOOL LinkManager::CallBackLink_ChatItemOut( class UGFxObject* parameterArray )
{
	//MMOUI::GetUIManager()->delegateHideTips();
	//UMMOUITips *tipsPanel = (UMMOUITips*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_DynamicUI_Tips );
	//if( tipsPanel != NULL ){
	//	tipsPanel->IsVisible = FALSE;
	//	tipsPanel->HideTips();
	//}
	return TRUE;
}

UBOOL LinkManager::CallBackLink_ChatItem_Auction(class UGFxObject* parameterArray,INT buttonIdx)
{
	UINT itemTempID = 0;
	INT auctionId = 0;
	GET_LINK_START();
	GET_LINK_NODE(itemTempID);
	GET_LINK_NODE(auctionId);

	MMOAuctionManager::getSingleton().SetShowByLink(TRUE);

	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_AuctionView, TRUE, "");

	MMOGameEvent_Auction_QuickSearchByChat evt(itemTempID);
	MMOUI::PostGameEvent(&evt);

	MMOAuctionManager::getSingleton().SetLinkItemShow(TRUE);
	MMOAuctionManager::getSingleton().SetLinkItemId(auctionId);
	return TRUE;
}
UBOOL LinkManager::CallBackLink_ChatItem_AuctionOver(class UGFxObject* parameterArray)
{
	UINT itemTempID = 0;
	INT auctionId = 0;
	GET_LINK_START();
	GET_LINK_NODE(itemTempID);
	GET_LINK_NODE(auctionId);

	MMOUI::GetUIManager()->ShowItemLinkTips(parameterArray);

	return TRUE;
}
UBOOL LinkManager::CallBackLink_ChatItem_AuctionOut(class UGFxObject* parameterArray)
{
	return TRUE;
}

//chatplayer 
FString LinkManager::HtmlLink_ChatPlayer(const FString& linkString , INT playerId )
{
	if(linkString.IsEmpty())
	{
		return TEXT("");
	}

	FString str,colorName;
	FString playerName = linkString;

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(!player)
	{
		return TEXT("");
	}
	if( player->ServerID == playerId )
	{
		colorName = FString::Printf( TEXT("<FONT COLOR='%s'>[%s]</FONT>") , *MMOChatManager::getSingleton().ChatMyNameColorString , *playerName );
	}
	else
	{
		colorName = FString::Printf( TEXT("<FONT COLOR='%s'>[%s]</FONT>") , *MMOChatManager::getSingleton().ChatOtherPlayerColorString , *playerName );
	}

	
	str = LINK_START(ELinkType_ChatPlayer)
			+ LINK_NODE(playerId)
			+ LINK_NODE(playerName)
			+ LINK_ENDU( colorName );		
	

	return str;
}

UBOOL LinkManager::CallBackLink_ChatPlayer(class UGFxObject* parameterArray,INT buttonIdx)
{
	UINT playerId = 0;
	FString playerName;
	GET_LINK_START();
	GET_LINK_NODE(playerId);
	GET_LINK_NODE(playerName);

	if( buttonIdx == 0 )
	{
		MMOUI::GetUIManager()->delegateEnterPrivateChat(playerName);
	}
	else
	{
		FIntPoint point = MMOFacade::getSingleton().GetMouseCursorPosition();
		MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_ChatPlayer , point.X , point.Y , playerId , playerName , 0 );
	}

	return TRUE;
}

UBOOL LinkManager::CallBackLink_ChatPlayerOver(class UGFxObject* parameterArray)
{
	return TRUE;
}

UBOOL LinkManager::CallBackLink_ChatPlayerOut(class UGFxObject* parameterArray)
{
	return TRUE;
}

//AuctuionPlayer
FString LinkManager::HtmlLink_AuctionPlayer(const FString& linkString , INT playerId )
{
	if(linkString.IsEmpty())
	{
		return TEXT("");
	}

	FString playerName = linkString;

	FString str = LINK_START(ELinkType_AuctionPlayer)
		+ LINK_NODE(playerId)
		+ LINK_NODE(playerName)
		+ LINK_ENDU( playerName );		


	return str;
}

UBOOL LinkManager::CallBackLink_AuctionPlayer(class UGFxObject* parameterArray,INT buttonIdx)
{
	UINT playerId = 0;
	FString playerName;
	GET_LINK_START();
	GET_LINK_NODE(playerId);
	GET_LINK_NODE(playerName);

	if( buttonIdx == 0 )
	{
		//MMOUI::GetUIManager()->delegateEnterPrivateChat(playerName);
	}
	else
	{
		FIntPoint point = MMOFacade::getSingleton().GetMouseCursorPosition();
		MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_AuctionPlayer , point.X , point.Y , playerId , playerName , 0 );
	}

	return TRUE;
}

UBOOL LinkManager::CallBackLink_AuctionPlayerOver(class UGFxObject* parameterArray)
{
	return TRUE;
}

UBOOL LinkManager::CallBackLink_AuctionPlayerOut(class UGFxObject* parameterArray)
{
	return TRUE;
}

FString LinkManager::HtmlLink_KillInfoPlayer(const FString& linkString , INT playerId )
{
	if(linkString.IsEmpty())
	{
		return TEXT("");
	}

	FString str,colorName;
	FString playerName = linkString;

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(!player)
	{
		return TEXT("");
	}
	//if( player->ServerID == playerId )
	//{
	//	colorName = FString::Printf( TEXT("<FONT COLOR='%s'>[%s]</FONT>") , *MMOChatManager::getSingleton().ChatMyNameColorString , *playerName );
	//}
	//else
	{
		colorName = FString::Printf( TEXT("<FONT COLOR='%s'>%s</FONT>") , *MMOChatManager::getSingleton().ChatOtherPlayerColorString , *playerName );
	}


	str = LINK_START(ELinkType_KillInfoPlayer)
		+ LINK_NODE(playerId)
		+ LINK_NODE(playerName)
		+ LINK_ENDU( colorName );		


	return str;
}

UBOOL	LinkManager::CallBackLink_KillInfoPlayer(class UGFxObject* parameterArray,INT buttonIdx)
{
	UINT playerId = 0;
	FString playerName;
	GET_LINK_START();
	GET_LINK_NODE(playerId);
	GET_LINK_NODE(playerName);

	if( buttonIdx == 0 )
	{
		//MMOUI::GetUIManager()->delegateEnterPrivateChat(playerName);
	}
	else
	{
		FIntPoint point = MMOFacade::getSingleton().GetMouseCursorPosition();
		MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_Friend , point.X , point.Y , playerId , playerName , 0 );
	}

	return TRUE;
}

UBOOL	LinkManager::CallBackLink_KillInfoPlayerOver(class UGFxObject* parameterArray)
{
	return TRUE;
}

UBOOL	LinkManager::CallBackLink_KillInfoPlayerOut(class UGFxObject* parameterArray)
{
	return TRUE;
}

//
// Navpoint
//
FString LinkManager::HtmlLink_NavPointName(const FString& linkString, const FString& navPointName, UINT mapId, UBOOL needMove)
{
	if (mapId == 0)
	{
		mapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	}

	FString res = LINK_START(ELinkType_NavPointName)		
		+LINK_NODE(navPointName)
		+LINK_NODE(mapId)
		+LINK_NODE((UINT)needMove)
		+LINK_ENDU(linkString);

	return res;
}

UBOOL LinkManager::CallBackLink_NavPointName( class UGFxObject* parameterArray , INT buttonIdx )
{
	FString navPointName;
	UINT mapId;
	UINT needMove;
	GET_LINK_START();
	GET_LINK_NODE(navPointName);
	GET_LINK_NODE(mapId);
	GET_LINK_NODE(needMove);
	
	// Tutorial event
	MMOGameEvent_Guide evtGuide(EGUET_ClickNaviLink);
	MMOUI::PostGameEvent(&evtGuide);

	return MMONavigationManager::getSingleton().Navigate(mapId, navPointName, needMove > 0);
}

UBOOL LinkManager::CallBackLink_NavPointNameOver( class UGFxObject* parameterArray )
{
	FString navPointName;
	UINT mapId;
	UINT needMove;
	GET_LINK_START();
	GET_LINK_NODE(navPointName);
	GET_LINK_NODE(mapId);
	GET_LINK_NODE(needMove);

	INT x = 0;
	INT y = 0;
	UGlobalMapInfo* globalMapInfo = UGlobalMapInfo::GetSingleton();
	const TArray<FNaviPointInfo>* mapNavPoints = NULL;
	if( globalMapInfo != NULL ){
		mapNavPoints = globalMapInfo->GetAllNavPointsInMap( mapId );
	}
	if( mapNavPoints == NULL ){
		return FALSE;
	}
	const TArray<FNaviPointInfo> &mapNavPointsList = *mapNavPoints;

	for (INT i = 0; i < mapNavPointsList.Num(); ++ i)
	{
		const FNaviPointInfo* navInfo = &mapNavPointsList(i);
		if (navInfo && navInfo->Name == navPointName)
		{
			x = navInfo->Position.X;
			y = navInfo->Position.Y;
			break;
		}
	}

	_ShowMapInfoTips(mapId, x, y);
	return TRUE;
}

UBOOL LinkManager::CallBackLink_NavPointNameOut( class UGFxObject* parameterArray )
{
	_HideMapInfoTips();
	return TRUE;
}

//
// Doodad
//
FString LinkManager::HtmlLink_DoodadNav(const FString& linkString, UINT doodadId, const FString& navPointName, UINT mapId, UBOOL needMove)
{
	if (mapId == 0)
	{
		mapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	}

	FString res = LINK_START(ELinkType_DoodadNav)	
		+LINK_NODE(doodadId)
		+LINK_NODE(mapId)	
		+LINK_NODE(navPointName)
		+LINK_NODE((UINT)needMove)
		+LINK_ENDU(linkString);

	return res;
}

UBOOL LinkManager::CallBackLink_DoodadNav(class UGFxObject* parameterArray,INT buttonIdx)
{
	FString navPointName;
	UINT doodadId;
	UINT mapId;
	UINT needMove;
	GET_LINK_START();
	GET_LINK_NODE(doodadId);
	GET_LINK_NODE(mapId);
	GET_LINK_NODE(navPointName);
	GET_LINK_NODE(needMove);

	UDoodad* doodad = DoodadManager::getSingleton().FindDoodadTemplate(doodadId);
	if (!doodad)
	{
		return FALSE;
	}

	return MMONavigationManager::getSingleton().NavigateDoodad(mapId, navPointName, doodadId, needMove > 0);
}

UBOOL LinkManager::CallBackLink_DoodadNavOver(class UGFxObject* parameterArray)
{
	return TRUE;
}

UBOOL LinkManager::CallBackLink_DoodadNavOut(class UGFxObject* parameterArray)
{
	return TRUE;
}

//
// Item
//
FString LinkManager::HtmlLink_ItemNav(const FString& linkString, UINT itemId, const FString& navPointName, UINT mapId, UBOOL needMove)
{
	if (mapId == 0)
	{
		mapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	}

	FString res = LINK_START(ELinkType_ItemNav)	
		+LINK_NODE(itemId)
		+LINK_NODE(mapId)	
		+LINK_NODE(navPointName)
		+LINK_NODE((UINT)needMove)
		+LINK_ENDU(linkString);

	return res;
}

UBOOL LinkManager::CallBackLink_ItemNav(class UGFxObject* parameterArray,INT buttonIdx)
{
	FString navPointName;
	UINT itemId;
	UINT mapId;
	UINT needMove;
	GET_LINK_START();
	GET_LINK_NODE(itemId);
	GET_LINK_NODE(mapId);
	GET_LINK_NODE(navPointName);
	GET_LINK_NODE(needMove);

	UItem* item = MMOFacade::getSingleton().FindItemTemplate(itemId);
	if (!item)
	{
		return FALSE;
	}

	return MMONavigationManager::getSingleton().Navigate(mapId, navPointName, needMove > 0);
}

UBOOL LinkManager::CallBackLink_ItemNavOver(class UGFxObject* parameterArray)
{
	UINT itemId = 0;
	GET_LINK_START();
	GET_LINK_NODE(itemId);

	UItem* item = MMOFacade::getSingleton().FindItemTemplate(itemId);
	if (!item)
	{
		return FALSE;
	}

	MMOUI::GetUIManager()->delegateShowLinkTips(parameterArray);
	return TRUE;
}

UBOOL LinkManager::CallBackLink_ItemNavOut(class UGFxObject* parameterArray)
{
	MMOUI::GetUIManager()->delegateHideTips();
	return TRUE;
}

//
// Quest trace title
//
FString LinkManager::HtmlLink_QuestTraceTitle( const UQuest* questTemplate )
{
	FString res = LINK_START(ELinkType_QuestTraceTitle)		
		+LINK_NODE(questTemplate->GetTemplateID())
		+LINK_ENDU(questTemplate->QuestName);

	return res;
}

UBOOL LinkManager::CallBackLink_QuestTraceTitle( class UGFxObject* parameterArray,INT buttonIdx )
{
	INT questId = 0;

	GET_LINK_START();
	GET_LINK_NODE(questId);

	if (questId != INVALID_QUEST_ID)
	{
		const UQuest* quest = MMOQuestData::getSingleton().MMOQuestData::getSingleton().GetQuestData(questId);
		if (quest)
		{
			if (quest->QuestType == E_qt_Trunk)
			{
				UMMOUIQuestStoryPanel* questPanel = (UMMOUIQuestStoryPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_QuestStory);
				if (questPanel)
				{
					MMOQuestManager::getSingleton().SetCurTrunkQuestId(questId);
					questPanel->Show(TRUE);
				}
			}
			else
			{
				UMMOUIQuestPanel* questPanel = (UMMOUIQuestPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Quest);
				if (questPanel)
				{
					MMOQuestManager::getSingleton().SetCurOtherQuestId(questId);
					questPanel->Show(TRUE);
					questPanel->UpdateQuestList();
					questPanel->UpdateQuestDetail();
				}
			}
		}
		return TRUE;
	}

	return FALSE;
}

UBOOL LinkManager::CallBackLink_QuestTraceTitleOver( class UGFxObject* parameterArray )
{
	return TRUE;
}

UBOOL LinkManager::CallBackLink_QuestTraceTitleOut( class UGFxObject* parameterArray )
{
	return TRUE;
}

//
// Quest trace accept
//
FString LinkManager::HtmlLink_QuestTraceAccept( const UQuest* questTemplate )
{
	FString res = LINK_START(ELinkType_QuestTraceAccept)		
		+LINK_NODE(questTemplate->GetTemplateID())
		+LINK_ENDU(TEXT("Accept directly"));

	return res;
}

UBOOL LinkManager::CallBackLink_QuestTraceAccept( class UGFxObject* parameterArray,INT buttonIdx )
{
	INT questId = 0;

	GET_LINK_START();
	GET_LINK_NODE(questId);
	
	if (questId == INVALID_QUEST_ID)
	{
		return FALSE;
	}

	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return FALSE;
	}

	UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_DialogNew);
	if (dialog)
	{
		dialog->TryPopupQuestWithoutNpc(questId);
	}

	return FALSE;
}

UBOOL LinkManager::CallBackLink_QuestTraceAcceptOver( class UGFxObject* parameterArray )
{
	return TRUE;
}

UBOOL LinkManager::CallBackLink_QuestTraceAcceptOut( class UGFxObject* parameterArray )
{
	return TRUE;
}

//
// Achievement
//
FString LinkManager::HtmlLink_Achievement(UINT32 id)
{
	FString str;
	FString achievementName = TEXT("");

	if (id < ACHIEVEMENT_MAX)
	{
		AchievementData data;
		if (MMOAchievementData::getSingleton().GetCommonAchievementData(LOWORD(id), data))
		{
			achievementName = data.Name;
		}
	}
	else if (id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
	{
		UINT16 dataId = MMOAchievementData::getSingleton().GetGuildAchievementDataId(id);
		GuildAchievementData data;
		if (MMOAchievementData::getSingleton().GetGuildAchievementData(dataId, data))
		{
			achievementName = data.Name;
		}
	}
	else
	{
		UINT16 killId = HIWORD(id);
		UINT16 killIndex = LOWORD(id);
		KillNpcAchievementData data;
		if (MMOAchievementData::getSingleton().GetKillNpcAchievementData(killId, data))
		{
			achievementName = data.Name[killIndex];
		}
	}

	FString text = FString::Printf(TEXT("<FONT COLOR='%s'>&lt;%s&gt;</FONT>"),
		*MMOChatManager::getSingleton().ChatAchievementColorString, *achievementName);

	str = LINK_START(ELinkType_Achievement)
		+ LINK_NODE(id)
		+ LINK_ENDU(text);

	return str;
}

UBOOL LinkManager::CallBackLink_Achievement( class UGFxObject* parameterArray , INT buttonIdx )
{
	INT achievementId = 0;
	GET_LINK_START();
	GET_LINK_NODE(achievementId);

	if (achievementId != -1)
	{
		if (MMOUI::GetUIManager())
		{
			UMMOUIAchievementPanel* panel = (UMMOUIAchievementPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Achievement);
			if (panel)
			{
				panel->Show(TRUE);
				panel->ShowAchievementListById(achievementId);
			}
		}
	}

	return TRUE;
}

UBOOL LinkManager::CallBackLink_AchievementOver( class UGFxObject* parameterArray )
{
	return TRUE;
}

UBOOL LinkManager::CallBackLink_AchievementOut( class UGFxObject* parameterArray )
{
	return TRUE;
}

//
// AchievementClass
//
FString LinkManager::HtmlLink_AchievementClass(const FString& displayName, UINT32 baseClass, UINT32 subClass)
{
	FString str = LINK_START(ELinkType_AchievementClass)
		+ LINK_NODE(baseClass)
		+ LINK_NODE(subClass)
		+ LINK_ENDU(displayName);

	return str;
}

UBOOL LinkManager::CallBackLink_AchievementClass( class UGFxObject* parameterArray , INT buttonIdx )
{
	INT baseClass = -1;
	INT subClass = -1;
	GET_LINK_START();
	GET_LINK_NODE(baseClass);
	GET_LINK_NODE(subClass);

	if (baseClass != -1)
	{
		if (MMOUI::GetUIManager())
		{
			UMMOUIAchievementPanel* panel = (UMMOUIAchievementPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Achievement);
			if (panel)
			{
				panel->Show(TRUE);
				panel->ShowAchievementListByClass(baseClass, subClass);
			}
		}
	}

	return TRUE;
}

UBOOL LinkManager::CallBackLink_AchievementClassOver( class UGFxObject* parameterArray )
{
	return TRUE;
}

UBOOL LinkManager::CallBackLink_AchievementClassOut( class UGFxObject* parameterArray )
{
	return TRUE;
}

//Open the specified UI via hyperlink
FString LinkManager::HtmlLink_OpenPanel(const FString& displayName, FString panelName)
{
	FString	str = LINK_START(ELinkType_OpenPanel)
			+ LINK_NODE(panelName)
			+ LINK_ENDU(displayName);		
	return str;
}
UBOOL LinkManager::CallBackLink_OpenPanel(class UGFxObject* parameterArray,INT buttonIdx)
{
	FString panelName;
	GET_LINK_START();
	GET_LINK_NODE(panelName);
	MMOUI::GetUIManager()->ShowPanelEx(*panelName,TRUE, "");
	return TRUE;
}
UBOOL LinkManager::CallBackLink_OpenPanelOver(class UGFxObject* parameterArray)
{
	return TRUE;
}
UBOOL LinkManager::CallBackLink_OpenPanelOut(class UGFxObject* parameterArray)
{
	return TRUE;
}

FString LinkManager::HtmlLink_OpenMap(const FString& linkString, UINT npcId, UINT mapId, const FString& mapRegin, FVector2D point)
{
	if (mapId == 0)
	{
		mapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	}

	FString	str = LINK_START(ELinkType_OpenMap)
		+ LINK_NODE(npcId)
		+ LINK_NODE(mapId)
		+ LINK_NODE(mapRegin)
		+ LINK_NODE((UINT)point.X)
		+ LINK_NODE((UINT)point.Y)
		+ LINK_ENDU(linkString);		
	return str;
}

UBOOL LinkManager::CallBackLink_OpenMap(class UGFxObject* parameterArray,INT buttonIdx)
{
	UINT npcId;
	UINT mapId;
	FString mapRegin;
	FString pointName;
	UINT pointX;
	UINT pointY;
	GET_LINK_START();
	GET_LINK_NODE(npcId);
	GET_LINK_NODE(mapId);
	GET_LINK_NODE(mapRegin);
	GET_LINK_NODE(pointX);
	GET_LINK_NODE(pointY);

	MMOMapDataManager::getSingleton().InvalidAllWorldMapMarkInfo();

	UBOOL foundNpc = FALSE;
	UNPCTemplate* npc = MMOFacade::getSingleton().FindNPCTemplate(npcId);
	if (npc)
	{
		FString infoStr = npc->GetDisplayName();
		if( pointX == 0 && pointY == 0 )
		{
			infoStr = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WorldMap.MonsterMapIconPositionUndefine") );
		}
		MMOMapDataManager::getSingleton().AddWorldMapMarkInfo(FVector(pointX, pointY, 0.0), infoStr);
		foundNpc = TRUE;
	}

	UMMOUIWorldMap* worldMapPanel = (UMMOUIWorldMap*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_WorldMap);
	if (worldMapPanel)
	{
		if( !worldMapPanel->IsVisible )
		{
			worldMapPanel->Show(TRUE);
		}

		INT regionInMapIdx = MMOMapDataManager::getSingleton().GetReginInMapIdx(mapId, mapRegin);
		if (regionInMapIdx >= 0)
		{
			worldMapPanel->ShowWorldMapInUnitedMapContent(mapId, regionInMapIdx);
		}
		else
		{
			worldMapPanel->ShowMapPanelContent(ePnlTyp_UnitedMap);
		}

		worldMapPanel->ShowMapMarksInfomation(foundNpc);

		// Tutorial
		MMOGameEvent_Guide evtGuide(EGUET_OpenNpcMap);
		MMOUI::PostGameEvent(&evtGuide);
	}
	return TRUE;
}

UBOOL LinkManager::CallBackLink_OpenMapOver(class UGFxObject* parameterArray)
{
	return TRUE;
}

UBOOL LinkManager::CallBackLink_OpenMapOut(class UGFxObject* parameterArray)
{
	return TRUE;
}

//Web
FString LinkManager::HtmlLink_WebPath(FString displayName,UINT32 id,FString path)
{
	FString res = LINK_START(ELinkType_WebPath)	
		+LINK_NODE(id)
		+LINK_NODE(path)
		+LINK_ENDU(displayName);

	return res;
}
UBOOL LinkManager:: CallBackLink_OpenWebPath(class UGFxObject* parameterArray,INT buttonIdx)
{
	INT id = 0;
	FString path;
	GET_LINK_START();
	GET_LINK_NODE(id);
	GET_LINK_NODE(path);

	if(id == 1)
	{
		WebHtmlManager::getSingleton().OpenSurveyUrl();
	}
	else if (id == 2)
	{
		WebHtmlManager::getSingleton().OpenBugSubmitUrl();
	}
	else if (id == 0)
	{
		WebHtmlManager::getSingleton().OpenUrl(path);
	}
	return TRUE;
}
UBOOL LinkManager::CallBackLink_OpenWebPathOver(class UGFxObject* parameterArray)
{
	return TRUE;
}
UBOOL LinkManager::CallBackLink_OpenWebPathOut(class UGFxObject* parameterArray)
{
	return TRUE;
}