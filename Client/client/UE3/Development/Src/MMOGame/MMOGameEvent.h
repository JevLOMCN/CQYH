#ifndef __MMOGAMEEVENT_H__
#define __MMOGAMEEVENT_H__

#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"

//------------------------------------------------------------------------
// game event definitions
//------------------------------------------------------------------------
enum EGameEvent
{
	GameEvent_Invalid,
	GameEvent_ServerError,
	GameEvent_VariableChanged,
	GameEvent_VariableInit,
	GameEvent_Splash,
	GameEVent_SceneLoadFinish,
	GameEvent_LeaveGame,
	GameEvent_LeaveServerScene,

	GameEvent_SelfCompatStateChange,

	GameEvent_Quest,
	GameEvent_QuestStateChanged,
	GameEvent_GuildSabacStateChanged,
	GameEvent_BattleTeam,
	GameEvent_CharacterDie,
	GameEvent_TargetChange,
	GameEvent_OtherPlayerLevelUp,
	GameEvent_ActorDestroyed,
	GameEvent_ActorAppear,
	GameEvent_ActorTypeChanged,
	GameEvent_NPCDestroyed,
	GameEvent_NPCAppear,
	GameEvent_PlayerBaseInfo,
	GameEvent_SendMailTo,
	GameEvent_Mail,
	GameEvent_Mail_UpdateSelectAll,

	GameEvent_MySpiritLevelUp,
	GameEvent_VipStateChanged,
	GameEvent_UpdateVipLevel,
	GameEvent_SyncGameLeftTime,
	GameEvent_ApplySpecialExpRst,

	GameEvent_SyncSignInDay,
	GameEvent_SignedInToday,
	GameEvent_NewSignedIn,

	GameEvent_GamePlayStateChange,
	GameEvent_RechargeGameTimeResult,
	GameEvent_PrisonTermChanged,

	GameEvent_UpdateMoneyExchangeLog,
	GameEvent_UpdateMoney,
	
	GameEvent_ExpExtraChanged,
	GameEvent_ExpExtraInit,
	GameEvent_ExpContainerChanged,

	GameEvent_UpdateMovePath,
	GameEvent_UpdateCurUIMap,

	GameEvent_ActiveRevivePnt,

	GameEvent_EnterSceneZone,
	GameEvent_LeaveSceneZone,

	GameEvent_UpdateSceneLines,
	GameEvent_EnterScene,
	GameEvent_ChangeFB_Difficulty,
	GameEvent_SceneFinished,
	GameEvent_FinishLoading,
	GameEvent_EnterGameSuccess,
	GameEvent_EnterSelectingRole,
	GameEvent_LeaveSelectingRole,
	GameEvent_ChangeRole,
	GameEvent_FBAchieve,

	GameEvent_ArenaStateChanged,
	GameEvent_ArenaDateUpdated,
	GameEvent_ArenaRoundResult,
	GameEvent_MsgBoxArenaWaitEnter,

	GameEvent_VSNotify,
	GameEvent_VSOneOnOneResult,
	GameEvent_VSOneOnOneRewardInfo,
	GameEvent_VSOneOnOneState,
	//GameEvent_VSOneOnOneGetRewardResult,

	GameEvent_AddMsgBall,
	GameEvent_RemoveMsgBall,
	GameEvent_UpdateMsgLine,
	GameEvent_ToggleMsgLine,
 	
	GameEvent_ClearBag,
	GameEvent_SortItemsInBagSuccess,
	GameEvent_BagSlotHintChange,
	GameEvent_BagSortEnd,

	GameEvent_BagOpenPack,
	GameEvent_BagWeight,

	GaemEvent_BagTabChange,
	GaemEvent_BagResourceSizeChange,
	GameEvent_BagResourceHide,
	GameEvent_BagResSellAll,

	GameEvent_NPCInteract,
	GameEvent_SortTimer,
	GameEvent_SortTimerBegin,
	GameEvent_SortTimerEnd,
	GameEvent_MatineeSubtitle,
	GameEvent_Navigation,
	GameEvent_NoMsg,
	//GameEvent_ExitMsg,
	GameEvent_InviteTeamMsg,
	GameEvent_AddTeamMsg,
	GameEvent_TeamNameChange,
	GameEvent_InviteMaster,
	GameEvent_InviteTrade,
	GameEvent_AttentionByMsg,
	GameEvent_InvitedTeam,
	GameEvent_AddedTeam,
	GameEvent_MainPlayerDataChange,
	GameEvent_RequestMainPlayerData,
	GameEvent_UpdatePlayerVipHint,
	GameEvent_CharactorRemoved,

	GameEvent_TeamMemberUpdate,
	GameEvent_TeamJoin,
	GameEvent_TeamAddMember,
	GameEvent_TeamLeave,
	GameEvent_TeamRemoveMember,
	GameEvent_TeamStateChaged,
	GameEvent_TeamCommon,
	GameEvent_TeamLeaveAsk,
	GameEvent_TeamNearPlayerChange,
	GameEvent_TeamExpand,
	GameEvent_SendTeamExpand,

	//team auction
	GameEvent_TeamAuctionNewAuction,
	GameEvent_TeamAuctionOtherBid,
	GameEvent_TeamAuctionNewItem,
	GameEvent_TeamAuctionEnd,
	GameEvent_TeamAuctionListChange,

	GameEvent_BattleTeamCreate,
	GameEvent_BattleTeamAddMember,
	GameEvent_BattleTeamInviteMember,

	//friend
	GameEvent_UpdateFriendGroup,
	GameEvent_UpdateBlackList,
	GameEvent_UpdateNearContact,
	GameEvent_UpdateEnemyContent,
	GameEvent_FriendDelGroup,
	GameEvent_FriendInfoChange,
	GameEvent_FriendAdd,
	GameEvent_UpdateKillInfo,

	GameEvent_MarqueeNoticeInfo,
	GameEvent_RollingNoticeInfo,

	GameEvent_EquipDurChanged,
	GameEvent_Item_Delete,
	GameEvent_Item_Gain,
	GameEvent_Item_SlotSwapped,
	GameEvent_Item_SlotUpdated,

	GameEvent_MsgBoxItemIdentifyAsk,
	GameEvent_Equip,
	GameEvent_UnEquip,
	GameEvent_SyncEquippedWeapon,
	GameEvent_OpenChainPay,
	GameEvent_GrayBagItemByShop,

	GameEvent_PlayerDetailInfoEquipChange,
	GameEvent_PlayerDetailInfoAttrChange,
	GameEvent_PlayerDetailInfoSpellChange,
	GameEvent_PlayerDetailInfoSpiritChange,

	GameEvent_ChainPayChange,

	GameEvent_Sync_SkillEquippedSkills,

	GameEvent_UnlockSkillSlot,
	GameEvent_ChangeSkillSlot,

	GameEvent_LearnedNewSkill,
	GameEvent_ForgetSkill,
	GameEvent_SyncMyLearnedSkillsByBook,

	GameEvent_SkillLevelUp,
	GameEvent_LearnedSkillExpChanged,
	GameEvent_MsgBoxChangeSkillStyle,
	GameEvent_SkillUiUpdate,
	GameEvent_ShowSkillRange,

	GameEvent_SoulLevelUp,
	GameEvent_SoulEmbed,
	GameEvent_SoulUnEmbed,

	GameEvent_WeaponSkillAdd,
	GameEvent_WeaponSkillRplcCacheChanged,

	GameEvent_WeaponLevelUpPutIn,
	GameEvent_WeaponLevelUpTakeOut,
	GameEvent_WeaponLevelUpResult,

	GameEvent_Guild,
	GameEvent_EquipSkill,
	GameEvent_EquipRuneSkill,

	GameEvent_UseFlyItem,
	GameEvent_Achievement,
	GameEvent_InviteToGuild,

	GameEvent_EquipSlotSyncLevel,
	GameEvent_EquipSlotEnhanceLvUp,
	GameEvent_EquipMaker,

	GameEvent_MasterStudent,

	GameEvent_Doodad,
	GameEvent_RankList,
	GameEvent_RankList_UpdateCategoryButton,
	GameEvent_RankList_RefreshList,
	GameEvent_Guide,

	GameEvent_ActivityRewardState,
	GameEvent_ActivityActive,
	GameEvent_Activity,

	GameEvent_UpdateDayTime,
	GameEvent_UpdateWeather,

	GameEvent_NewCD,

	GameEvent_Heaven,
	GameEvent_MallStore,

	GameEvent_StartNavigate,
	GameEvent_StopNavigate,

	GameEvent_Stall,

	GameEvent_ShopUpdate,
	GameEvent_ShopTypeUpdate,
	GameEvent_BlackMarket_UpdateItemCount,
	GameEvent_BlackMarket_UpdateItemCoolDown,

	GameEvent_Auction_SearchResult,
	GameEvent_Auction_ItemSell,
	GameEvent_Auction_ItemCancelSell,
	GameEvent_Auction_ItemBuy,
	GameEvent_Auction_ListMySelling,
	GameEvent_Auction_ListMyBuying,
	GameEvent_Auction_ListFailed,
	GameEvent_Auction_Reset,
	GameEvent_Auction_UpdatePageNumb,
	GameEvent_Auction_PlayerNameGot,
	GameEvent_Auction_UpdateTreeList,
	GameEvent_Auction_SearchTreeList,
	GameEvent_Auction_FilterTreeList,
	GameEvent_Auction_UpdateItemCategoryList,
	GameEvent_Auction_QuickSearch,
	GameEvent_Auction_QuickSearchByBag,
	GameEvent_Auction_QuickSearchByChat,
	GameEvent_Auction_ShowBag,
	GameEvent_Auction_UpdateQueryItem,
	GameEvent_Auction_ListMyBought,
	GameEvent_Auction_ShowLinkItem,

	GameEvent_UpdateTraceRelation,

	GameEvent_ResetHurt,
	GameEvent_ShowGuildView,
	

	GameEvent_GoldStore_ListBuying,
	GameEvent_GoldStore_ListSelling,
	GameEvent_GoldStore_AddOrder,
	GameEvent_GoldStore_CancelSell,
	//GameEvent_GoldStore_ListBuying_My,
	//GameEvent_GoldStore_ListSelling_My,
	GameEvent_GoldStore_ListMyOrder,
	GameEvent_GoldStore_UpdatePlayerMoney,
	GameEvent_GoldStore_UpdatePlayerRealMoney,
	GameEvent_GoldStore_UpdateMyOrderCount,
	GameEvent_GoldStore_UpdateMySellCount,
	GameEvent_GoldStore_SetNoticeInfo,
	GameEvent_GoldStore_UpdateMyLogs,
	GameEvent_GoldStore_DefaultPrice,

	GameEvent_DynamicBossBlood,
	GameEvent_KillRecord,

	GameEvent_Refresh_SkillSlotLoop,
	GameEvent_Refresh_SkillSlotIcon,

	GameEvent_ShowNewGlobe,

	GameEvent_ShowShopMessage,
	GameEvent_UpdateShopMessageMoney,

	GameEvent_UpdateChannelTab,
	GameEvent_SetChannelByTab,

	GameEvent_ShowOptionPanel,
	GameEvent_HideOptionPanel,
	GameEvent_Handbook,
	GameEvent_CanAddEnemy,

	GameEvent_ShowPrivateTabEffect,
	GameEvent_Chat_AppendInput,
	GameEvent_Chat_UpdateDropDownList,
	GameEvent_Chat_ErrorMsg,
	GameEvent_Journey,

	GameEvent_UpdateLoginQueue,
	GameEvent_ShowLoginSetUp,
	GameEvent_ShowLoginInputBox,

	GameEvent_CharactorPower,
	GameEvent_ShowActivation,
	GameEvent_GetMessageAck,
	GameEvent_ShowFriendChatHint,
	GameEvent_HideFriendChatHint,
	GameEvent_SetFriendChatHint,
	GameEvent_EquipRemakeResult,

	GameEvent_BookRemakeResult,
	GameEvent_MedicineRemakeResult,

	GameEvent_RealMoneyChange,

	GameEvent_QuestModeChange,
	GameEvent_GuildOccupy,
};

struct MMOGameEvent
{
	virtual EGameEvent GetEventType() const = 0;
};

struct MMOGameEvent_Any : public MMOGameEvent
{
	MMOGameEvent_Any(EGameEvent evtType)
		: EventType(evtType)
	{
	}

	EGameEvent EventType;
	virtual EGameEvent GetEventType() const{ return EventType; }
};

class INativeGameEventHandler
{
public:

	virtual void OnNativeGameEvent(struct MMOGameEvent* evt) = 0;
};

class IGameEventAdapter
{
public:

	virtual void Delete() = 0;
};

template<class T>
class TGameEventAdapter : public INativeGameEventHandler, public IGameEventAdapter
{
	T* _adaptee;
	TArray<EGameEvent> _events;
public:

	TGameEventAdapter(T* adaptee, EGameEvent evt)
		: _adaptee(adaptee)
	{
		_events.AddItem(evt);
		_RegisterAll();
	}

	TGameEventAdapter(T* adaptee, const TArray<EGameEvent>& evts)
		: _adaptee(adaptee)
		, _events(evts)
	{
		_RegisterAll();
	}

	void _RegisterAll() 
	{
		for (INT i = 0; i < _events.Num(); ++i)
		{
			MMOUI::RegisterEventHandler(this, _events(i));
		}
	}

	~TGameEventAdapter()
	{
		for (INT i = 0; i < _events.Num(); ++i)
		{
			MMOUI::UnRegisterEventHandler(this, _events(i));
		}
	}

	virtual void OnNativeGameEvent(struct MMOGameEvent* evt)
	{
		_adaptee->OnNativeGameEvent(evt);
	}

	virtual void Delete()
	{
		delete this;
	}
};

#endif
