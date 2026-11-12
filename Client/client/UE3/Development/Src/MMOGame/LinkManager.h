#ifndef _LINKMANAGER_H_
#define _LINKMANAGER_H_
#include "MMOSingleton.h"

struct FClientItemInfo;

enum LINK_ITEM_TYPE
{
	LIT_ITEM,
	LIT_CHATITEM,
	LIT_AHITEM,
};

class LinkManager: public Singleton<LinkManager>
{
public:
	LinkManager(void);
	virtual ~LinkManager(void);
private:
	static FString	LinkNode(FString V);
	static FString	LinkNode(UINT V);
	static FString	LinkNode(INT V);
	static FString	LinkNode(FLOAT V);

	static void	GetLinkNode(FString Node, FString& V);
	static void	GetLinkNode(FString Node, UINT& V);
	static void	GetLinkNode(FString Node, INT& V);
	static void	GetLinkNode(FString Node, FLOAT& V);

	typedef UBOOL	(*CallBackLinkFun)(class UGFxObject* parameterArray);
	typedef UBOOL	(*CallBackLinkButtonIdxFun)(class UGFxObject* parameterArray,INT buttonIdx);

	struct FCallbackLinkFuns
	{
		CallBackLinkButtonIdxFun MouseClickProcess;
		CallBackLinkFun MouseOverProcess;
		CallBackLinkFun MouseOutProcess;

		FCallbackLinkFuns()
		{
		}

		FCallbackLinkFuns(CallBackLinkButtonIdxFun click, CallBackLinkFun over, CallBackLinkFun out)
			: MouseClickProcess(click)
			, MouseOverProcess(over)
			, MouseOutProcess(out)
		{
		}
	};

	static FCallbackLinkFuns g_linksCallbackFun[ELinkType_MAX];
	
public:
	static UBOOL	CallBackLink( class UGFxObject* parameterArray , INT buttonIdx );
	static UBOOL	CallBackLinkOver(class UGFxObject* parameterArray);
	static UBOOL	CallBackLinkOut(class UGFxObject* parameterArray);

	static UBOOL	NullCallBackLinkOverOut(class UGFxObject* parameterArray);

	//Pathfinding
	static FString HtmlLink_NPCNav(const FString& linkString, UINT NpcTemplateID, UINT MapID, UBOOL needMove);
	static UBOOL	CallBackLink_NPCNav(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL	CallBackLink_NPCNavOver(class UGFxObject* parameterArray);
	static UBOOL	CallBackLink_NPCNavOut(class UGFxObject* parameterArray);

	static FString HtmlLink_CoordNav(const FString& linkString, UINT MapID, FLOAT x, FLOAT y, FLOAT z);
	static UBOOL CallBackLink_CoordNav(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL CallBackLink_CoordNavOver(class UGFxObject* parameterArray);
	static UBOOL CallBackLink_CoordNavOut(class UGFxObject* parameterArray);

	static FString HtmlLink_NavPointName(const FString& linkString, const FString& navPointName, UINT mapId, UBOOL needMove);
	static UBOOL CallBackLink_NavPointName(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL CallBackLink_NavPointNameOver(class UGFxObject* parameterArray);
	static UBOOL CallBackLink_NavPointNameOut(class UGFxObject* parameterArray);
	
	static FString HtmlLink_DoodadNav(const FString& linkString, UINT doodadId, const FString& navPointName, UINT mapId, UBOOL needMove);
	static UBOOL CallBackLink_DoodadNav(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL CallBackLink_DoodadNavOver(class UGFxObject* parameterArray);
	static UBOOL CallBackLink_DoodadNavOut(class UGFxObject* parameterArray);

	static FString HtmlLink_ItemNav(const FString& linkString, UINT itemId, const FString& navPointName, UINT mapId, UBOOL needMove);
	static UBOOL CallBackLink_ItemNav(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL CallBackLink_ItemNavOver(class UGFxObject* parameterArray);
	static UBOOL CallBackLink_ItemNavOut(class UGFxObject* parameterArray);

	static FString HtmlLink_NavFly(const FString& linkString, UINT questId, UINT destId);
	static UBOOL	CallBackLink_NavFly(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL	CallBackLink_NavFlyOver(class UGFxObject* parameterArray);
	static UBOOL	CallBackLink_NavFlyOut(class UGFxObject* parameterArray);

	//Name in chat
	static FString HtmlLink_ChatPlayer(const FString& linkString , INT playerId );
	static UBOOL	CallBackLink_ChatPlayer(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL	CallBackLink_ChatPlayerOver(class UGFxObject* parameterArray);
	static UBOOL	CallBackLink_ChatPlayerOut(class UGFxObject* parameterArray);

	//auction house player name
	static FString HtmlLink_AuctionPlayer(const FString& linkString , INT playerId );
	static UBOOL	CallBackLink_AuctionPlayer(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL	CallBackLink_AuctionPlayerOver(class UGFxObject* parameterArray);
	static UBOOL	CallBackLink_AuctionPlayerOut(class UGFxObject* parameterArray);

	//kill info
	static FString HtmlLink_KillInfoPlayer(const FString& linkString , INT playerId );
	static UBOOL	CallBackLink_KillInfoPlayer(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL	CallBackLink_KillInfoPlayerOver(class UGFxObject* parameterArray);
	static UBOOL	CallBackLink_KillInfoPlayerOut(class UGFxObject* parameterArray);
	//Item link
	static FString HtmlLink_Item(UItem* itemTemplate);
	static FString HtmlLink_ColorItem(UItem* itemTemplate);
	static FString HtmlLink_ColorItem(UItem* itemTemplate, FClientItemInfo* pItemInfo, LINK_ITEM_TYPE type = LIT_ITEM);
	static UBOOL	CallBackLink_Item(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL	CallBackLink_ItemOver(class UGFxObject* parameterArray);
	static UBOOL	CallBackLink_ItemOut(class UGFxObject* parameterArray);

	static UBOOL	CallBackLink_ChatItem(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL	CallBackLink_ChatItemOver(class UGFxObject* parameterArray);
	static UBOOL	CallBackLink_ChatItemOut(class UGFxObject* parameterArray);

	static UBOOL	CallBackLink_ChatItem_Auction(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL	CallBackLink_ChatItem_AuctionOver(class UGFxObject* parameterArray);
	static UBOOL	CallBackLink_ChatItem_AuctionOut(class UGFxObject* parameterArray);


	//click quest title to open quest panel
	static FString HtmlLink_QuestTraceTitle(const UQuest* questTemplate);
	static UBOOL	CallBackLink_QuestTraceTitle(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL	CallBackLink_QuestTraceTitleOver(class UGFxObject* parameterArray);
	static UBOOL	CallBackLink_QuestTraceTitleOut(class UGFxObject* parameterArray);

	//link to accept quest directly (for the acceptable quest list)
	static FString HtmlLink_QuestTraceAccept(const UQuest* questTemplate);
	static UBOOL	CallBackLink_QuestTraceAccept(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL	CallBackLink_QuestTraceAcceptOver(class UGFxObject* parameterArray);
	static UBOOL	CallBackLink_QuestTraceAcceptOut(class UGFxObject* parameterArray);

	//Achievement
	static FString HtmlLink_Achievement(UINT32 id);
	static UBOOL	CallBackLink_Achievement(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL	CallBackLink_AchievementOver(class UGFxObject* parameterArray);
	static UBOOL	CallBackLink_AchievementOut(class UGFxObject* parameterArray);

	static FString HtmlLink_AchievementClass(const FString& displayName, UINT32 baseClass, UINT32 subClass);
	static UBOOL	CallBackLink_AchievementClass(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL	CallBackLink_AchievementClassOver(class UGFxObject* parameterArray);
	static UBOOL	CallBackLink_AchievementClassOut(class UGFxObject* parameterArray);

	//Open the specified UI via hyperlink
	static FString  HtmlLink_OpenPanel(const FString& displayName, FString panelName);
	static UBOOL	CallBackLink_OpenPanel(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL	CallBackLink_OpenPanelOver(class UGFxObject* parameterArray);
	static UBOOL	CallBackLink_OpenPanelOut(class UGFxObject* parameterArray);

	//click NPC hyperlink, open map and mark NPC position
	static FString  HtmlLink_OpenMap(const FString& linkString, UINT npcId, UINT mapId, const FString& mapRegin, FVector2D point);
	static UBOOL	CallBackLink_OpenMap(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL	CallBackLink_OpenMapOver(class UGFxObject* parameterArray);
	static UBOOL	CallBackLink_OpenMapOut(class UGFxObject* parameterArray);

	//Web
	static FString  HtmlLink_WebPath(FString displayName,UINT32 id,FString path); 
	static UBOOL	CallBackLink_OpenWebPath(class UGFxObject* parameterArray,INT buttonIdx);
	static UBOOL	CallBackLink_OpenWebPathOver(class UGFxObject* parameterArray);
	static UBOOL	CallBackLink_OpenWebPathOut(class UGFxObject* parameterArray);

protected:
	static void _ShowMapInfoTips(INT mapId, INT x, INT y);
	static void _HideMapInfoTips();
};


#endif
