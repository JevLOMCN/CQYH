//
// UI manager
//
class MMOUIManager extends Object	
	config(MMOUIManager)
	native(UI)
	dependson(TypeDefine)
	abstract;
//var const name MoviePlayer_DamageLevel ;
//var const name MoviePlayer_SceneHint ;
//var const name MoviePlayer_Death ;


var const name Panel_Login_User ;
var const name Panel_Login_Select ;
var const name Panel_Login_Create ;
var const name Panel_Login_Revert;
var const name Panel_Login_Queue;
var const name Panel_Bag ;
var const name Panel_BagResource;
var const name Panel_MoneyExchange ;
var const name Panel_Quest ;
var const name Panel_QuestStory ;
var const name Panel_QuestItemCollection ;
var const name Panel_QuestReward ;
var const name Panel_QuestRewardBuyTimes ;
var const name Panel_SoulLevelUp ;
var const name Panel_SoulEmbed ;
var const name Panel_QuestTrace ;
var const name Panel_MessageboxZone;
var const name Panel_Death ;
var const name Panel_Chat ;
var const name Panel_Chat_ChatOption ;
var const name Panel_Friend_FriendChatHint ;
var const name Panel_Friend_FriendListContent ;
var const name Panel_Friend_NearContactContent;
var const name Panel_Friend_BlackListContent;
var const name Panel_Friend_EnemyListContent;
var const name Panel_Friend ;
var const name Panel_HintButtonBar ;
var const name Panel_Friend_FriendChatLevel;
var const name Panel_WorldMap ;
var const name Panel_UnitedMap ;
var const name Panel_Minimap ;
var const name Panel_PathMap ;
var const name Panel_TeleportNet;
var const name Panel_FBEnter_FBEnter ;
var const name Panel_FBEnter_FBEnterMsg ;
var const name Panel_FBTask ;
var const name Panel_FBEndAll_FBSummary;
var const name Panel_FBEndAll_FBAward ;
var const name Panel_FBEndAll_FBBattleComfirm ;
var const name Panel_VSState;
var const name Panel_VSResult;
var const name Panel_VSReward;
var const name Panel_Arena_ArenaEntrance;
var const name Panel_Arena_ArenaState;
var const name Panel_PVP_PVPBattle;
var const name Panel_PVP_Arena ;
var const name Panel_PVP_ArenaScore ;
var const name Panel_PVP_ArenaHint ;
var const name Panel_BattleTeam;
var const name Panel_BattleTeam_BattleTeamCreate;
var const name Panel_PVP_ArenaQueue;
var const name Panel_BattleSummary_BattleSummary ;
var const name Panel_BattleSummary_BattleScoreBoard;
var const name Panel_FightingScreenHint;
var const name Panel_EquipEnhance ;
var const name Panel_EquipLevelUp ;
var const name Panel_MutiLineMsgBox ;
//var const name Panel_EquipSlotEnhance ;
var const name Panel_Skill ;
var const name Panel_SkillSetting ;
var const name Panel_SkillTree ;
var const name Panel_WeaponSkill ;
var const name Panel_WeaponSkillRplc ;
var const name Panel_WeaponSkillSetting ;
var const name Panel_Stall_StallSetting ;
var const name Panel_Stall_StallBuy ; 
var const name Panel_Stall_StallMsg;
var const name Panel_Stall_StallBuyMsg;
var const name Panel_Stall_StallUpgrade ;
//var const name Panel_DynamicUI_Buff;
var const name Panel_DynamicUI_ProgressBar;
var const name Panel_DynamicUI_FBSummaryProgressBar ;
var const name Panel_DynamicUI_StrengthBar;
var const name Panel_DynamicUI_QTEBar ;
var const name Panel_DynamicUI_LevelUp ;
var const name Panel_DynamicUI_WeatherChange ;
var const name Panel_DynamicUI_CenterStaticNotice ;
var const name Panel_DynamicUI_NewEvent ;
var const name Panel_DynamicUI_BossBlood ;
var const name Panel_FightingScreenHint_ScreenPopUpBoxMsg ;
var const name Panel_DynamicUI_Scene ;
var const name Panel_DynamicUI_Tips  ;
var const name Panel_DynamicUI_Transducer  ;
var const name Panel_DurabilityShowing;
var const name Panel_HudInfo_TargetInfo  ;
var const name Panel_HudInfo_MyInfo ;
var const name Panel_HudInfo_HeroInfo;
var const name Panel_HudInfo_TeamMate ;
var const name Panel_HudInfo_TeamHint ;
var const name Panel_RClickMenu ;
var const name Panel_RClickChildMenu ;
var const name Panel_Team ;
var const name Panel_TeamAuction ;
var const name Panel_TeamAuctionList;
var const name Panel_TeamInvite;
var const name Panel_TeamSet;
var const name Panel_TeamTarget;
var const name Panel_Warehouse ;
var const name Panel_Skillbar ;
var const name Panel_Skillbar_Menubar ;
var const name Panel_MsgBall_MsgButtonBar ;
var const name Panel_Skillbar_Buff;
var const name Panel_DamageLevel ;

//var const name Panel_Charactor ;
var const name Panel_Hero ;
var const name Panel_Mount ;
var const name Panel_Reputation ;

var const name Panel_Character ;

var const name Panel_TaskPopup ;
var const name Panel_OptionMenu;
var const name Panel_OptionConfig;
var const name Panel_OptionQuit;
var const name Panel_Shop ;
var const name Panel_TradeP2P ;
var const name Panel_Dialog  ;
var const name Panel_DialogNew;
var const name Panel_Achievement;
var const name Panel_AchievementHint;
var const name Panel_MsgInfo ;
var const name Panel_NoticeInfo;
var const name Panel_PlayerInfo;
var const name Panel_SimplePlayerInfo;
var const name Panel_SceneHint  ;
var const name Panel_EquipGainHintBox ;
var const name Panel_WeaponExp  ;
var const name Panel_PickWidget_DorpItemTips ;
var const name Panel_MasterStudent ;
var const name Panel_MasterCongMain ;
var const name Panel_MasterCongSub ;
var const name Panel_MasterTips;
var const name Panel_MasterInvite;
var const name Panel_DynamicUI_BattleState ;
var const name Panel_DynamicUI_UnlockAOGString;
var const name Panel_Mail  ;
var const name Panel_Mail_MailRead ;
var const name Panel_Benchmark;
var const name Panel_Guild_Warehouse;
var const name Panel_Guild_Create;
var const name Panel_Guild_View;
var const name Panel_Guild_WarView;
var const name Panel_Guild_SabacEnroll;
var const name Panel_Guild_Main;
var const name Panel_Guild_Territory;
var const name Panel_Guild_Support;
var const name Panel_Guild_Welfare;
var const name Panel_Guild_DungeonBegin;
var const name Panel_Guild_DungeonEnd;
var const name Panel_Guild_DungeonEnter;
var const name Panel_Guild_DungeonExit;
var const name Panel_Guild_WelfareUpgrade;
var const name Panel_Guild_BuildingCreate;
var const name Panel_Guild_BuildingUpgradeMulti;
var const name Panel_Guild_BuildingUpgradeSingle;
var const name Panel_Guild_WarEnd;
var const name Panel_Guild_WarHint;
var const name Panel_Guild_WarStart;
var const name Panel_Guild_Instruction;
//var const name Panel_UndergroundTrade;
//var const name Panel_UTSell;
//var const name Panel_UTBid;
var const name Panel_EquipCompose;
var const name Panel_EquipDecompose;
var const name Panel_EquipDecomposeHint;
var const name Panel_BackDecorated;
var const name Panel_Activity ;
var const name Panel_HeavenGasExchang;
var const name Panel_HeavenPractise ;
var const name Panel_ActivityQuit ;
var const name Panel_GameAnnouncement;
var const name Panel_HintCombat_BossQTE;
var const name Panel_RankList;
var const name Panel_RankConfig;
var const name Panel_HeroRank;
var const name Panel_Mall;          
var const name Panel_MallFriend;    
var const name Panel_MallEquipRole;    
var const name Panel_MallBuyGoods;  
var const name Panel_Vip;
var const name Panel_Recharge;
var const name Panel_DailySignIn;
var const name Panel_SpecialExpApply;
var const name Panel_AuctionView;
var const name Panel_Hurt;
var const name Panel_GoldStore;
var const name Panel_KillRecordPanel;
var const name Panel_KillCountPanel;
//var const name Panel_ItemStore;
var const name Panel_GuideOption;
var const name Panel_Helper;
var const name Panel_HelperPlayer;
var const name Panel_ShopMessage;
var const name Panel_Handbook;
var const name Panel_Journey;
var const name Panel_JourneyView;
var const name Panel_JourneyComplete;
var const name Panel_Assist;
var const name Panel_EquipRemake;
var const name Panel_RemakeBook;
var const name Panel_RemakeFactory;
var const name Panel_Jigsaw;
var const name Panel_DynamicPwd;

var config int              LuckyOilItemId;

var config array<int>	    CareerFontColor;
var config int	            OfflineFontColor;
var config int	            TotalChannelFontColor;
var config int	            PrivateChannelFontColor;
var config int	            TeamChannelFontColor;
var config int	            NearChannelFontColor;
var config int	            SystemChannelFontColor;
var config array<int>	    EquipLevelColor;
var config string	        PrivateChannelFontColorString;
var config string	        TeamChannelFontColorString;
var config string	        NearChannelFontColorString;
var config string	        SystemChannelFontColorString;
var config string	        TotalChannelFontColorString;
var config int	            IncreaseAttrColor;
var config int	            DecreaseAttrColor;
var config int	            OriginAttrColor;

var config float              PhyDefConstPara;
var config float              MagDefConstPara;
var config float              CrtConstPara;
var config float              DodgeConstPara;

var config string	        WorldChannelIconPath;
var config string	        GuildChannelIconPath;
var config string	        NearChannelIconPath;
var config string	        TeamChannelIconPath;
var config string	        PrivateChannelIconPath;
var config string	        SystemChannelIconPath;
var config string           GMIconPath;

var config array<int>       MonsterQuality ;

// Add game-related HUD display control functionality
// add by showlong
var bool                    HintHideNameSelf;
var bool                    HintHideNameSelfTitle;
var bool                    HintHideNameOther;
var bool                    HintHideNameOtherTitle;
var bool                    HintHideNameFamilySelf;
var bool                    HintHideNameFamilyOther;
var bool                    HintHideNameMonster;
var bool                    HintHideNameDrop;
var bool                    BloodDrawSelf;
var bool                    BloodDrawPlayer;
var bool                    BloodDrawMonster;
var bool                    HurtHideNameSelf;
var bool                    HurtHideNameOther;
var bool                    SkillEffectHideSelf;
var bool                    SkillEffectHideOther;
var bool                    SkillEffectHideCameraShake;
var bool                    DrawGuide;
var bool                    DrawChatBubble;
var bool                    DrawAITip;
var bool                    DrawWeather;
var bool                    DrawBloodNumber;
var bool                    DrawHurt;
var bool                    DrawGameFrame;
var bool                    HintFriendOnline;
var bool                    HintHideNearPlayer;
var bool                    DeclineStrangeChat;
var bool                    DeclineInviteTeam;
var bool                    DeclineInviteFamily;
var bool                    DeclineInviteDisciple;
var bool                    DeclineInviteMaster;
var bool                    DeclineShowEnemy;
var bool                    HintMarkSelf;
var bool                    NoShift;
var bool                    QuestMode;
var bool					DeclineShowTrade;

var bool                    bSoundAll;
var bool                    bSoundBack;
var bool                    bSoundEnv;
var bool                    bSoundVoice;
var bool                    bSoundSfx;
var bool                    bSoundFocusOut;

var int                     nSoundAllVL;
var int                     nSoundBackVL;
var int                     nSoundEnvVL;
var int                     nSoundVoiceVL;
var int                     nSoundSfxVL;


var EMoveCtrlKey WSADKey[5];
var array<IMMOActorHUD> UILevel;
var name LastDelayLoadPlayerName;
var const IItemSlotActionListener DefaultListener;
var ItemSlotActionListener_Waiting WaitingListener;
var int   ClearCurrentItemSlotActionListenerCountDown;
var bool  bWaitingFlag;
var int   TraceRelationForceID;

//var MMOMsgBoxManager MsgBoxMgr;

//Do some initialization such as string table mapping
native static final function InitUIManager();

native static final function RegisterMMOActorHUD( IMMOActorHUD hud );
native static final function UnRegisterMMOActorHUD( IMMOActorHUD hud );

native static final function SetLastPlayerName(name LastPlayerName);
native static final function SetLastPlayerNameByPanelName(name PanelName);

native static final function CloseLastPlayer(name CurPlayerName);
native static final function CloseLastPlayerByPanelName(name PanelName);

//native final function RegisterMsgBoxManager( MMOMsgBoxManager msgBoxMgr );
// game events
// native static final function RegisterEventHandler(IGameEventHandler handler, EGameEvent requiredEventType);
// native static final function UnRegisterEventHandler(IGameEventHandler handler, EGameEvent requiredEventType);
// native static final function PostGameEvent(NativeGameEvent evt);

// game state
native static final function EGameState GetCurrentGameState();
native static final function ActiveGameState(EGameState gs);
native static final function AddGameState(MMOGameState gs);
native static final function MMOGameState FindGameStateObject(EGameState gs);

native static final function Item FindItemTemplate(ECONTAINER_SLOT ContainerID, int SlotID);
native static final function Item FindItemTemplateByID(int ItemTemplateID);

native static final function AutoEquipMount();

native static final function SaveConfigToServer();
native static final function ApplyConfigByServer(int n,int n2);

native static final function SaveSoundConfigToServer();
native static final function ApplySoundConfigByServer(int n1,int n2);



native static final function SetTraceRelationForceID(int ForceID);
native static final function int GetTraceRelationForceID();

event ItemSlotActionListener_Waiting GetWaitingListener()
{
	if( WaitingListener == none )
	{
		WaitingListener = new class'ItemSlotActionListener_Waiting';
	}
	return WaitingListener;
}

native static final function SetItemSlotWaitingState(bool bWaiting);


//battlestate
static final delegate ShowEnterBattle(bool Enter);
native static final function SetShowEnterBattleDelegate(delegate<ShowEnterBattle> fun);

//combat message(combo kill)
static final delegate ShowCombatMsg(string hintMsg);
native static final function SetShowCombatMsgDelegate(delegate<ShowCombatMsg> fun);

//cavas
native static final function SetSceneView( Canvas cvs );

// mouse cursor
native static final function ShowMouseCursor(bool ShowMouse);
native static final function SetMouseCursor(EMMOCursorType cursorType);

// common used movieplayer
native static final function SetCommonUIComponentMoviePlayer(MMOGFXPlayer_RootBase inMoviePlayer);
native static final function MMOGFXPlayer_RootBase GetCommonUIComponentMoviePlayer();

// tips delegate
static final delegate bool IsTipsShow();
native static final function SetIsTipsShowDelegate(delegate<IsTipsShow> fun);

static final delegate ShowTipsHTML(ItemSlotType TipsType, GFxObject FromObject ,bool IsRefreshing = false, int displayPriority = 4 );
native static final function SetShowTipsHTMLDelegate(delegate<ShowTipsHTML> fun);

static final delegate ToggleTipAid( );
native static final function SetToggleTipAidDelegate(delegate<ToggleTipAid> fun);

static final delegate AnchorTips( Vector toAnchorPos, bool isPos3D );
native static final function SetAnchorTipsDelegate(delegate<AnchorTips> fun);

static final delegate ShowFreeStyleTips(string title,string text,int showStyle, int displayPriority);
native static final function SetShowFreeStyleTipsDelegate(delegate<ShowFreeStyleTips> fun);

static final delegate ShowTextTips(string text);
native static final function SetShowTextTipsDelegate(delegate<ShowTextTips> fun);

static final delegate ShowTips(int TipsType, int  ObjectId, GfxObject Widge );
native static final function SetShowTipsDelegate(delegate<ShowTips> fun);

static final delegate HideTips();
native static final function SetHideTipsDelegate(delegate<HideTips> fun);

static final delegate ShowLinkTips(GFxObject FromObject);
native static final function SetShowLinkTipsDelegate(delegate<ShowLinkTips> fun);

native static final function ShowItemLinkTips( GFxObject FromObject);
native static final function ShowItemLinkTipsWithoutAnchored( GFxObject FromObject);
// bag delegate
//static final delegate ToggleBag();
//native static final function SetToggleBagDelegate(delegate<ToggleBag> fun);

static final delegate OnShift(bool bRelease);
native static final function SetOnShiftDelegate(delegate<OnShift> fun);

static final delegate OpenBag(ECONTAINER_SLOT ContainerType);
native static final function SetOpenBagDelegate(delegate<OpenBag> fun);

static final delegate ECONTAINER_SLOT GetOpenBag();
native static final function SetGetOpenBagDelegate(delegate<GetOpenBag> fun);

static final delegate DeleteItem(ECONTAINER_SLOT ContainerID, int SlotID);
native static final function SetDeleteItemDelegate(delegate<DeleteItem> fun);

static final delegate TurntoPage( ECONTAINER_SLOT BagType );
native static final function SetTurntoPageDelegate(delegate<TurntoPage> fun);

static final delegate LockItem(ECONTAINER_SLOT ContainerID, int SlotID, int isToLockIn);
native static final function SetLockItemDelegate(delegate<LockItem> fun);

static final delegate HintNewItemLabel(int containerId);
native static final function SetHintNewItemLabelDelegate(delegate<HintNewItemLabel> fun);

static final delegate UpdateBagCapSlot( int containerID );
native static final function SetUpdateBagCapSlotDelegate(delegate<UpdateBagCapSlot> fun);
//
static final delegate OpenRClickMenu( ERClickMenuType type, int x, int y , int RoleId  , String RoleName  , int index );
native static final function SetOpenRClickMenuDelegate(delegate<OpenRClickMenu> fun);

static final delegate OpenRClickChildMenu_Friend( int x, int y,int id);
native static final function SetOpenRClickChildMenu_FriendDelegate(delegate<OpenRClickChildMenu_Friend> fun);

static final delegate OpenRClickChildMenu_Team( int x, int y,int w);
native static final function SetOpenRClickChildMenu_TeamDelegate(delegate<OpenRClickChildMenu_Team> fun);


static final delegate HideRClickChildMenu();
native static final function SetHideRClickChildMenuDelegate(delegate<HideRClickChildMenu> fun);

static final delegate CloseRClickMenu( ERClickMenuType type);
native static final function SetCloseRClickMenuDelegate(delegate<CloseRClickMenu> fun);

static final delegate HideRClickMenu();
native static final function SetHideRClickMenuDelegate(delegate<HideRClickMenu> fun);

// chat message delegate
//static final delegate AddChatMessage(int chatChannel, string chatMsg , string senderName );
//native static final function SetChatMessageDelegate(delegate<AddChatMessage> fun);

static final delegate SetFilterChannelNum( int num );
native static final function SetSetFilterChannelNumDelegate(delegate<SetFilterChannelNum> fun);

static final delegate EnterPrivateChat( string playerName );
native static final function SetEnterPrivateChatDelegate(delegate<EnterPrivateChat> fun);

static final delegate SetFilterChannel( int pos , string ChannelName );
native static final function SetSetFilterChannelDelegate(delegate<SetFilterChannel> fun);

//hintbar
static final delegate int AddNewHintButton( string iconResouce , int time , bool add );
native static final function SetAddNewHintButtonDelegate(delegate<AddNewHintButton> fun);

//GameAnnouncement
native static final function OpenGameAnnouncement( int updateVer, int reserve );

static final delegate RemoveHintButton( int pos );
native static final function SetRemoveHintButtonDelegate(delegate<RemoveHintButton> fun);

// screen message / msginfo delegate
static final delegate AddScreenMessage(string ScreenMsg , MsgInfoItemType type = MsgInfoType_Table , string SndName = "", int idx = -1  );
native static final function SetScreenMessageDelegate(delegate<AddScreenMessage> fun);

// shop delegate
static final delegate OpenShop(int ShopTemplateID);
native static final function SetOpenShopDelegate(delegate<OpenShop> fun);

// blackMarket delegate 
static final delegate OpenBlackMarket(int BlackMarketID);
native static final function SetOpenBlackMarketDelegate(delegate<OpenBlackMarket> fun);

// Death delegate
static final delegate ShowDeath();
native static final function SetShowDeathDelegate(delegate<ShowDeath> fun);

static final delegate HideDeath();
native static final function SetHideDeathDelegate(delegate<HideDeath> fun);

// Hero delegate
static final delegate CallShowWaitingRevive(int CountDownTime);
native static final function SetCallShowWaitingReviveDelegate(delegate<CallShowWaitingRevive> fun);


static final delegate ShowHintBtn(bool bShow);
native static final function SetShowHintBtnDelegate(delegate<ShowHintBtn> fun);

static final delegate UpdateHintText(MMOActor pl,ESceneHintUpdate type=ESHU_ALL);
native static final function SetUpdateHintTextDelegate(delegate<UpdateHintText> fun);

static final delegate UpdateHintTextByID(int ServerID,ESceneHintUpdate type=ESHU_ALL);
native static final function SetUpdateHintTextByIDDelegate(delegate<UpdateHintTextByID> fun);

static final delegate CallBack_PreLoad();
native static final function SetCallBack_PreLoadDelegate(delegate<CallBack_PreLoad> fun);

static final delegate UpdateMinimap(float DeltaTime );
native static final function SetUpdateMinimapDelegate(delegate<UpdateMinimap> fun);

// Hero delegate
static final delegate RefreshHeroAttribute();
native static final function SetRefreshHeroAttributeDelegate(delegate<RefreshHeroAttribute> fun);

static final delegate RefreshHeroSlot();
native static final function SetRefreshHeroSlotDelegate(delegate<RefreshHeroSlot> fun);

static final delegate RefreshHero(bool bForceUpdate);
native static final function SetRefreshHeroDelegate(delegate<RefreshHero> fun);

static final delegate RefreshSummon();
native static final function SetRefreshSummonDelegate(delegate<RefreshSummon> fun);

// Mount delegate
static final delegate RefreshMount();
native static final function SetRefreshMountDelegate(delegate<RefreshMount> fun);

static final delegate RefreshEquipedMount();
native static final function SetRefreshEquipedMountDelegate(delegate<RefreshEquipedMount> fun);

// option 
//static final delegate ShowKeyWarning(bool bSHow);
//native static final function SetShowKeyWarningDelegate(delegate<ShowKeyWarning> fun);

// Damage delegate
static final delegate ShowAttackNumber( int PosX , int PosY , int Number, Vector Location);
native static final function SetShowAttackNumberDelegate(delegate<ShowAttackNumber> fun);

static final delegate ShowAttackedNumber( int PosX , int PosY , int Number, Vector Location); 
native static final function SetShowAttackedNumberDelegate(delegate<ShowAttackedNumber> fun);

static final delegate ShowDodge( int PosX , int PosY, int Number, Vector Location);
native static final function SetShowDodgeDelegate(delegate<ShowDodge> fun);

static final delegate ShowMiss( int PosX , int PosY, Vector Location);
native static final function SetShowMissDelegate(delegate<ShowMiss> fun);

static final delegate ShowParry( int PosX , int PosY, Vector Location);
native static final function SetShowParryDelegate(delegate<ShowParry> fun);

static final delegate ShowImmunity( int PosX , int PosY, Vector Location);
native static final function SetShowImmunityDelegate(delegate<ShowImmunity> fun);

static final delegate ShowCriticalNumber( int PosX , int PosY , int Number, Vector Location);
native static final function SetShowCriticalNumberDelegate(delegate<ShowCriticalNumber> fun);

static final delegate ShowCriticaledNumber( int PosX , int PosY , int Number, Vector Location);
native static final function SetShowCriticaledNumberDelegate(delegate<ShowCriticaledNumber> fun);

static final delegate ShowBlockNumber( int PosX , int PosY , int Number, Vector Location);
native static final function SetShowBlockNumberDelegate(delegate<ShowBlockNumber> fun);

static final delegate ShowExpNumber( int PosX , int PosY , int Number, Vector Location);
native static final function SetShowExpNumberDelegate(delegate<ShowExpNumber> fun);

static final delegate ShowHPRestore( int PosX , int PosY , int Number, Vector Location);
native static final function SetShowHPRestoreDelegate(delegate<ShowHPRestore> fun);

static final delegate ShowGetMoney( int PosX , int PosY , int Number , int subType , Vector Location);
native static final function SetShowGetMoneyDelegate(delegate<ShowGetMoney> fun);

//Friend delegate
static final delegate MoveUnReadMsgToRead( int RoleId , string PlayerName );
native static final function SetMoveUnReadMsgToReadDelegate(delegate<MoveUnReadMsgToRead> fun);

//static final delegate ToggleFriend( );
//native static final function SetToggleFriendDelegate(delegate<ToggleFriend> fun);

//static final delegate ChangeFriendGroup( int RoleId , int GroupId );
//native static final function SetChangeFriendGroupDelegate(delegate<ChangeFriendGroup> fun);

//static final delegate RequestAddAttention( int RoleId , string TargetName );
//native static final function SetRequestAddAttentionDelegate(delegate<RequestAddAttention> fun);

//static final delegate RequestDelAttention( int RoleId );
//native static final function SetRequestDelAttentionDelegate(delegate<RequestDelAttention> fun);

//static final delegate UpdateBlackList();
//native static final function SetUpdateBlackListDelegate(delegate<UpdateBlackList> fun);

//static final delegate UpdateEnemyList( );
//native static final function SetUpdateEnemyListDelegate(delegate<UpdateEnemyList> fun);

//static final delegate UpdateNearContactList();
//native static final function SetUpdateNearContactDelegate(delegate<UpdateNearContactList> fun);

static final delegate ChangeGroupName( string GroupName , int GroupId);
native static final function SetChangeGroupNameDelegate(delegate<ChangeGroupName> fun);

//static final delegate DeleteGroup(  int GroupId );
//native static final function SetDeleteGroupDelegate(delegate<DeleteGroup> fun);

static final delegate SelectFriendChat( int RoleId , string RoleName );
native static final function SetSelectFriendChatDelegate(delegate<SelectFriendChat> fun);

// dialog delegate
//static final delegate OpenDialog( NPC NpcInstance,  DialogContent DiagCotent);
//native static final function SetOpenDialogDelegate(delegate<OpenDialog> fun);

//static final delegate CloseDialog(optional bool playSound=true);
//native static final function SetCloseDialogDelegate(delegate<CloseDialog> fun);

delegate OnClickGfxObject( GfxObject Mc );

//static final delegate GFxObject AddInputMessageBox( string TitleText , int InfoId , string InfoText , delegate<OnClickGfxObject> OnClickOK, delegate<OnClickGfxObject> OnClickNO, int format );
//native static final function SetAddInputMessageBoxDelegate(delegate<AddInputMessageBox> fun);

//static final delegate GFxObject AddMessageBox( string Text,int InfoId, delegate<OnClickGfxObject> OnClickOK, delegate<OnClickGfxObject> OnClickNO );
//native static final function SetAddMessageBoxDelegate(delegate<AddMessageBox> fun);

static final delegate  ReplaceHotSkill();
native static final function SetReplaceHotSkillDelegate(delegate<ReplaceHotSkill> fun);

// static final delegate  ToggleDropItemTips();
// native static final function SetToggleDropItemTipsDelegate(delegate<ToggleDropItemTips> fun);

//static final delegate GFxObject AddModalMessageBox( string Text,int InfoId, delegate<OnClickGfxObject> OnClickOK, delegate<OnClickGfxObject> OnClickNO );
//native static final function SetAddModalMessageBoxDelegate(delegate<AddModalMessageBox> fun);


//static final delegate DispatchMsgBoxButtonOk( GFxObject mc );
//static final delegate DispatchMsgBoxButtonNo( GFxObject mc );
//native static final function SetDispatchMsgOkDelegate(delegate<DispatchMsgBoxButtonOk> fun);
//native static final function SetDispatchMsgNoDelegate(delegate<DispatchMsgBoxButtonNo> fun);

// warehouse delegate
//static final delegate OpenWarehouse();
//native static final function SetOpenWarehouseDelegate(delegate<OpenWarehouse> fun);

static final delegate UpdateWarehouseCapSlot();
native static final function SetUpdateWarehouseCapSlotDelegate(delegate<UpdateWarehouseCapSlot> fun);

// buff delegate
static final delegate AddBuffPanel(int buffSlotID,string buffIcon,int buffMaxTime,int buffCurTime,int stackCount,int ownerID,int buffID, int showTimeUp,bool debuff);
native static final function SetAddBuffPanelDelegate(delegate<AddBuffPanel> fun);
static final delegate DeleteBuffPanel(int buffSlotID);
native static final function SetDeleteBuffPanelDelegate(delegate<DeleteBuffPanel> fun);

static final delegate FlashHPBar();
native static final function SetFlashHPBarDelegate(delegate<FlashHPBar> fun);

static final delegate UpdatePetState(int stateID);
native static final function SetUpdatePetStateDelegate(delegate<UpdatePetState> fun);

static final delegate UpdateMenuEffect(int idx , int childType , bool left , bool pl);
native static final function SetUpdateMenuEffectDelegate(delegate<UpdateMenuEffect> fun);

// function buff delegate
static final delegate AddFunctionBuffPanel(int buffSlotID,string buffIcon,int buffMaxTime,int buffCurTime,int stackCount,int ownerID,int buffID,int showTimeUp,bool debuff);
native static final function SetAddFunctionBuffPanelDelegate(delegate<AddFunctionBuffPanel> fun);
static final delegate DeleteFunctionBuffPanel(int buffSlotID);
native static final function SetDeleteFunctionBuffPanelDelegate(delegate<DeleteFunctionBuffPanel> fun);


// target buff delegate
static final delegate AddTargetBuffPanel(int buffSlotID,string buffIcon,int buffMaxTime,int buffCurTime,int stackCount,int ownerID,int buffID,int showTimeUp,bool debuff);
native static final function SetAddTargetBuffPanelDelegate(delegate<AddTargetBuffPanel> fun);
static final delegate DeleteTargetBuffPanel(int buffSlotID);
native static final function SetDeleteTargetBuffPanelDelegate(delegate<DeleteTargetBuffPanel> fun);

static final delegate HeroInfoUpdateData();
native static final function SetHeroInfoUpdateDataDelegate(delegate<HeroInfoUpdateData> fun);

//// ShowProgressBar delegate
//static final delegate ShowProgressBar( int countDownTime,string strTitle, int type );
//native static final function SetShowProgressBarDelegate(delegate<ShowProgressBar> fun);
//static final delegate CloseProgressBar(int closeType);
//native static final function SetCloseProgressBarDelegate(delegate<CloseProgressBar> fun);

//// ShowProgressBarStrength delegate
//static final delegate ShowProgressBarStrength(int time, int max);
//native static final function SetShowProgressBarStrengthDelegate(delegate<ShowProgressBarStrength> fun);
//static final delegate CloseProgressBarStrength(int closeType);
//native static final function SetCloseProgressBarStrengthDelegate(delegate<CloseProgressBarStrength> fun);

//// ShowProgressBarQTE delegate
//static final delegate ShowProgressBarQTE(int time,string keyName,string imgName,int nActorID,int nAbilityID);
//native static final function SetShowProgressBarQTEDelegate(delegate<ShowProgressBarQTE> fun);
//static final delegate CloseProgressBarQTE(int closeType);
//native static final function SetCloseProgressBarQTEDelegate(delegate<CloseProgressBarQTE> fun);

//// ShowHintSceneWidget delegate
//static final delegate ShowHintSceneWidget();
//native static final function SetShowHintSceneWidgetDelegate(delegate<ShowHintSceneWidget> fun);
//static final delegate CloseHintSceneWidget();
//native static final function SetCloseHintSceneWidgetDelegate(delegate<CloseHintSceneWidget> fun);

//// ShowHintLevelUpWidget delegate
//static final delegate ShowHintLevelUpWidget(string levelUp,INT strengthAdd,INT agilityAdd,	INT intellectAdd,	INT staminaAdd, 	INT spiritAdd,	 INT maxHpAdd);
//native static final function SetShowHintLevelUpWidgetDelegate(delegate<ShowHintLevelUpWidget> fun);
//static final delegate CloseHintLevelUpWidget();
//native static final function SetCloseHintLevelUpWidgetDelegate(delegate<CloseHintLevelUpWidget> fun);

//static final delegate AddSkillLearnedMsg( int msgId, ESkillMsgType type );
//native static final function SetAddSkillLearnedMsgDelegate(delegate<AddSkillLearnedMsg> fun);

static final delegate ASClickQTE();
native static final function SetASClickQTEDelegate(delegate<ASClickQTE> fun);

static final delegate ASPkModeChange();
native static final function SetASPkModeChangeDelegate(delegate<ASPkModeChange> fun);

//static final delegate RequireOpenStall();
//native static final function SetRequireOpenStallDelegate(delegate<RequireOpenStall> fun);

//static final delegate OpenStallSetting( bool bOpen =true );
//native static final function SetOpenStallSettingDelegate(delegate<OpenStallSetting> fun);

//static final delegate ShowUnlockAOGString(string content);
//native static final function SetShowUnlockAOGStringDelegate(delegate<ShowUnlockAOGString> fun);

//static final delegate HideUnlockAOGString();
//native static final function SetHideUnlockAOGStringDelegate(delegate<HideUnlockAOGString> fun);



//static final delegate OpenSkill( );
//native static final function SetOpenSkillDelegate(delegate<OpenSkill> fun);

//static final delegate OpenMasterStudent( );
//native static final function SetOpenMasterStudentDelegate(delegate<OpenMasterStudent> fun);

//Arena
static final delegate OpenArenaQueue( int mode , int size );
native static final function SetOpenArenaQueueDelegate(delegate<OpenArenaQueue> fun);

static final delegate LeaveArenaQueue( );
native static final function SetLeaveArenaQueueDelegate(delegate<LeaveArenaQueue> fun);

static final delegate AskEnterArena();
native static final function SetAskEnterArenaDelegate(delegate<AskEnterArena> fun);

static final delegate OpenArenaScoreSingle( int winID , int LoseID , int score );
native static final function SetOpenArenaScoreSingleDelegate(delegate<OpenArenaScoreSingle> fun);

static final delegate CloseArenaScore();
native static final function SetCloseArenaScoreDelegate(delegate<CloseArenaScore> fun);


native static final function SendReturnSelRole();
native static final function SendReturnLogin();
native static final function SendQuitGame();
native static final function PromptFreePK();

native static final function ImmReturnSelRole();
native static final function ImmReturnLogin();
native static final function ImmQuitGame();

//team delegate
//static final delegate ToggleTeam();
//native static final function SetToggleTeamDelegate(delegate<ToggleTeam> fun);

//static final delegate InviteAddTeam( int RoleId );
//native static final function SetInviteAddTeamDelegate(delegate<InviteAddTeam> fun);

//static final delegate LeaveTeam( int RoleId );
//native static final function SetLeaveTeamDelegate(delegate<LeaveTeam> fun);

//static final delegate MoveLeader( int RoleId );
//native static final function SetMoveLeaderDelegate(delegate<MoveLeader> fun);

//static final delegate InviteTeamRequest(string RoleName, int RoleId);
//native static final function SetInviteTeamRequestDelegate(delegate<InviteTeamRequest> fun);

//static final delegate AddTeamRequest( string RoleName , int RoleId );
//native static final function SetAddTeamRequestDelegate(delegate<AddTeamRequest> fun);

//P2P Trade
static final delegate ApplyDoTrade( int RoleId );
native static final function SetApplyDoTradeDelegate(delegate<ApplyDoTrade> fun);

//Charactor delegate
//static final delegate ToggleCharactor();
//native static final function SetToggleCharactorDelegate(delegate<ToggleCharactor> fun);

//Worldmap delegate
//static final delegate ToggleWorldMap();
//native static final function SetToggleWorldMapDelegate(delegate<ToggleWorldMap> fun);


////FBEnter delegate
//static final delegate ToggleTeleportNet();
//native static final function SetToggleTeleportNetDelegate(delegate<ToggleTeleportNet> fun);

//FBEnter delegate
//static final delegate ToggleFBEnter();
//native static final function SetToggleFBEnterDelegate(delegate<ToggleFBEnter> fun);

////PVP delegate
//static final delegate TogglePVP();
//native static final function SetTogglePVPDelegate(delegate<TogglePVP> fun);

//BattleSummary delegate
static final delegate ToggleBattleSummary();
native static final function SetToggleBattleSummaryDelegate(delegate<ToggleBattleSummary> fun);

//Skill delegate
static final delegate ToggleSkill();
native static final function SetToggleSkillDelegate(delegate<ToggleSkill> fun);

//Quest delegate
//static final delegate ToggleQuest();
//native static final function SetToggleQuestDelegate(delegate<ToggleQuest> fun);

//static final delegate OpenQuestPopupUp(bool Accept , int Id , bool share , int shareSrcId);
//native static final function SetOpenQuestPopupUpDelegate(delegate<OpenQuestPopupUp> fun);

static final delegate AddTraceQuest(string questTitle, string questDescription, INT questState);
native static final function SetAddTraceQuestDelegate(delegate<AddTraceQuest> fun);


//skillbar delegate
static final delegate BuffEnableAbility();
native static final function SetBuffEnableAbilityDelegate(delegate<BuffEnableAbility> fun);

// BossQTE delegate
static final delegate UpdateBossQTE(int CounterNum , int CounterDownNum);
native static final function SetUpdateBossQTEDelegate(delegate<UpdateBossQTE> fun);


////EquipEnhance delegate
//static final delegate ToggleEquipEnhance();
//native static final function SetToggleEquipEnhanceDelegate(delegate<ToggleEquipEnhance> fun);


//native static final function RegisterMoviePlayer(name PlayerName, MMOGFXPlayer MoviePlayer);
native static final function MMOGFXPlayer FindMoviePlayer(name PanelName);
native static final function MMOGFXPlayer LoadMoviePlayers(name PlayerName);

//native static final function RegisterUIPanel(name PanelName, MMOUIBase Panel);
native static final function MMOUIBase FindUIPanel(name PanelName);
native static final function MMOUIBase ShowPanel(name PanelName, bool Show, string TabName);
native static final function MMOUIBase ShowPanelEx(name PanelName,bool Show, string TabName, optional int displayPriority = -1);
native static final function MMOUIBase ShowPanelByDefaultVisible(name PanelName, string TabName);
native static final function MMOUIBase ToggleShowPanel(name panelName, string TabName);
native static final function MMOUIBase ToggleShowPanelEx(name PanelName, string TabName);
native static final function ShowAllPanels(bool Show,bool bRecover=false);
native static final function ShowPatternPanel(MMOUIBase RefPanel, MMOUIBase PatternPanel);
native static final function OnLeftShift(bool bRelease);


//native static final function SetDefaultItemSlotActionListener(IItemSlotActionListener listener);
//native static final function IItemSlotActionListener GetDefaultItemSlotActionListener();

native static final function SetCurrentItemSlotActionListener(IItemSlotActionListener listener);
native static final function IItemSlotActionListener GetCurrentItemSlotActionListener();
//native static final function IItemSlotActionListener GetLastItemSlotActionListener();
native static final function ClearCurrentItemSlotActionListener();
native static final function bool IsItemSlotActionListener();
native static final function ClearCurrentItemSlotActionListenerDelay(int nFrame);
native static final function ClearCurrentItemSlotActionListenerUpdate();


static final delegate UpdateSkillBarKeyDown(bool bRelease,  int InCmdID, int slotCmdKey);
native static final function SetUpdateSkillBarKeyDownDelegate(delegate<UpdateSkillBarKeyDown> fun);

static final delegate UpdateMainMenuKeyDown(bool bRelease,  int InCmdID, int InSlotID);
native static final function SetUpdateMainMenuKeyDownDelegate(delegate<UpdateMainMenuKeyDown> fun);



static final delegate UpdateReputationData();
native static final function SetUpdateReputationDataDelegate(delegate<UpdateReputationData> fun);

static final delegate RefreshReputationSelected();
native static final function SetRefreshReputationSelectedDelegate(delegate<RefreshReputationSelected> fun);

static final delegate UpdateTraceReputationData(GfxObject data);
native static final function SetUpdateTraceReputationDataDelegate(delegate<UpdateTraceReputationData> fun);

//static final delegate DisplayReputationData(int ForceID, int ReputationNow, int ReputationLast);
//native static final function SetDisplayReputationDataDelegate(delegate<DisplayReputationData> fun);

static final delegate UpdateTitleData(bool bReset);
native static final function SetUpdateTitleDataDelegate(delegate<UpdateTitleData> fun);

static final delegate ResetPlayerHint(MMOPlayer pl );
native static final function SetResetPlayerHintDelegate(delegate<ResetPlayerHint> fun);

static final delegate OpenStallBuy(MMOPlayer pl );
native static final function SetOpenStallBuyDelegate(delegate<OpenStallBuy> fun);

// Achievement
//static final delegate ShowAchievementPanel();
//native static final function SetShowAchievementPanelDelegate(delegate<ShowAchievementPanel> fun);

//msgButton
static final delegate SetMsgButtonNum( int num );
native static final function SetSetMsgButtonNumDelegate(delegate<SetMsgButtonNum> fun);

static final delegate SetMsgButtonIcon( int index , int frame );
native static final function SetSetMsgButtonIconDelegate(delegate<SetMsgButtonIcon> fun);

// Guild Create
static final delegate ShowGuildCreatePanel();
native static final function SetShowGuildCreatePanelDelegate(delegate<ShowGuildCreatePanel> fun);

// Guild View
static final delegate ShowGuildViewPanel();
native static final function SetShowGuildViewPanelDelegate(delegate<ShowGuildViewPanel> fun);

// Guild Main
//static final delegate ShowGuildMainPanel();
//native static final function SetShowGuildMainPanelDelegate(delegate<ShowGuildMainPanel> fun);

// Guild Territory
static final delegate ShowGuildTerritoryPanel();
native static final function SetShowGuildTerritoryPanelDelegate(delegate<ShowGuildTerritoryPanel> fun);

// Forbid chat
static final delegate ForbidChat(int roleId, bool isForbid);
native static final function SetForbidChatDelegate(delegate<ForbidChat> fun);

// Cancel Ally
static final delegate CancelAlliance(int guildId);
native static final function SetCancelAllianceDelegate(delegate<CancelAlliance> fun);

static final delegate UpdateAccessInfo();
native static final function SetUpdateAccessInfoDelegate(delegate<UpdateAccessInfo> fun);

static final delegate FreshWarehouse();
native static final function SetFreshWarehouseDelegate(delegate<FreshWarehouse> fun);


native static function MoveByKey( EMoveCtrlKey MoveType,bool MoveFlag);
native static final function bool OnEsc();
native static final function OnUIShow(MMOUIBase ui, bool show);

native static function CloseAllHintForDragging();

native static function ShowHurtPanel(bool show);

native static function SetPanelShowWithOutTarget(name PanelName);

//--------------------------------------------------------------------------
native final function ShowProgressBar( int countDownTime,string strTitle, int type );
native final function CloseProgressBar(int closeType);

native final function ShowProgressBarStrength(int time, int max);
native final function CloseProgressBarStrength(int closeType);

native final function ShowUnlockAOGString(string content);
native final function HideUnlockAOGString();

native final function ShowProgressBarQTE(int time,string keyName,string imgName,int nActorID,int nAbilityID);
native final function CloseProgressBarQTE(int closeType);

native final function ShowHintLevelUpWidget(string levelUp,INT strengthAdd,INT agilityAdd,	INT intellectAdd,	INT staminaAdd, 	INT spiritAdd,	 INT maxHpAdd );
native final function CloseHintLevelUpWidget();

final native function ShowHintSceneWidget();
final native function CloseHintSceneWidget();


DefaultProperties
{
	WSADKey[0] = EMCK_Empty
	WSADKey[1] = EMCK_Empty
	WSADKey[2] = EMCK_Empty
	WSADKey[3] = EMCK_Empty

	DefaultListener = none;
	bWaitingFlag    = false;
	ClearCurrentItemSlotActionListenerCountDown = -1;

    HintHideNameSelf = false;
    HintHideNameSelfTitle = false;
    HintHideNameOther = false;
    HintHideNameOtherTitle = false;
    HintHideNameFamilySelf = false;
    HintHideNameFamilyOther = false;
    HintHideNameMonster = false;
    HintHideNameDrop = false;
    BloodDrawSelf = false;
    BloodDrawPlayer = false;
    BloodDrawMonster = false;
    HurtHideNameSelf = false;
    HurtHideNameOther = false;
    SkillEffectHideSelf = false;
    SkillEffectHideOther = false;
	SkillEffectHideCameraShake = false;
    DrawGuide = false;
    DrawChatBubble = false;
    DrawAITip = false;
    DrawWeather = false;
    DrawBloodNumber = false;
	DrawHurt = false;
    DeclineStrangeChat = false;
    DeclineInviteTeam = false;
    DeclineInviteFamily = false;
    DeclineInviteDisciple = false;
    DeclineInviteMaster = false;
	DeclineShowEnemy = false;
	DrawGameFrame = true;
	HintFriendOnline = true;
	HintHideNearPlayer = true;
	HintMarkSelf  = false;
	NoShift = false;
	QuestMode = true;
	DeclineShowTrade = false;

	bSoundAll=true;
	bSoundBack=true;
	bSoundEnv=true;
	bSoundVoice=true;
	bSoundSfx=true;

	nSoundAllVL=50;
	nSoundBackVL=50;
	nSoundEnvVL=50;
	nSoundVoiceVL=50;
	nSoundSfxVL=50;
	//MoviePlayer_DamageLevel       =   "DamageLevel"
	//MoviePlayer_SceneHint     	= "SceneHint"
	//MoviePlayer_Death = "Death"




  Panel_Login_User  = "User";
  Panel_Login_Select  = "Select";
  Panel_Login_Create  = "Create";
  Panel_Login_Revert  = "Revert";
  Panel_Login_Queue   = "LoginQueue";

  Panel_Bag = "Bag"
  Panel_BagResource = "MaterialPanel";
  Panel_MoneyExchange = "MoneyExchange"
  Panel_Quest = "QuestNormal"
  Panel_QuestStory = "QuestStory"
  Panel_QuestItemCollection = "QuestItemCollection"
  Panel_QuestReward = "QuestReward"
  Panel_QuestRewardBuyTimes = "QuestRewardBuyTimes"
  Panel_SoulLevelUp = "SoulLevelUp"
  Panel_SoulEmbed = "SoulEmbed"
  Panel_QuestTrace             = "QuestTrace"
	Panel_PlayerInfo = "PlayerInfo"
	Panel_SimplePlayerInfo = "SimplePlayerInfo"
  Panel_Death   = "Death"
  Panel_Chat   = "Chat"
  Panel_Chat_ChatOption    = "ChatOption"
  Panel_Friend_FriendChatHint   = "FriendChatHint"
	Panel_Friend_FriendListContent = "Friend_FriendListContent"
	Panel_Friend_NearContactContent = "Friend_NearContactContent"
	Panel_Friend_BlackListContent = "Friend_BlackListContent"
	Panel_Friend_EnemyListContent = "Friend_EnemyListContent"
  Panel_MessageboxZone = "UnlockMsgBoxZone_mc"
  Panel_Friend   = "Friend"
  Panel_HintButtonBar = "HintButtonBar";
  Panel_Friend_FriendChatLevel   = "FriendChatLevel"
  Panel_WorldMap       			= "WorldMap"
  Panel_UnitedMap               = "UnitedMap"
  Panel_Minimap 			    = "Minimap"
  Panel_PathMap                 = "PathMap"
  Panel_TeleportNet             = "TeleportNet"
  Panel_FBEnter_FBEnter                 = "FBEnter"
  Panel_FBEnter_FBEnterMsg              = "FBEnterMsg"
  Panel_FBTask                          = "FBTask"
  Panel_FBEndAll_FBSummary               = "FBSummary"
  Panel_FBEndAll_FBAward                 = "FBAward"
  Panel_FBEndAll_FBBattleComfirm         = "FBBattleComfirm"
  Panel_VSState                         = "VSState"
  Panel_VSResult                        = "VSResult"
  Panel_VSReward                        = "VSReward"
  Panel_Arena_ArenaEntrance             = "ArenaEntrance";
  Panel_Arena_ArenaState                = "ArenaState";
  Panel_PVP_PVPBattle               = "PVPBattle"
  Panel_PVP_Arena                     = "ArenaPanel"
  Panel_PVP_ArenaScore            = "ArenaScore"
  Panel_PVP_ArenaHint             = "ArenaHint"
  Panel_BattleSummary_BattleSummary     	    = "BattleSummary"
  Panel_BattleSummary_BattleScoreBoard     	= "BattleScoreBoard"
  Panel_FightingScreenHint     	= "FightingScreenHint"
  Panel_EquipEnhance            = "EquipEnhance"
  Panel_EquipLevelUp            = "EquipLevelUp"
  Panel_MutiLineMsgBox          = "MutiLineMsgBox"
  //Panel_EquipSlotEnhance         = "EquipSlotEnhance"
  Panel_Skill               	= "Skill"
  Panel_SkillSetting            = "SkillSetting"
  Panel_SkillTree               = "SkillTree"
  Panel_WeaponSkill             = "WeaponSkill"
  Panel_WeaponSkillSetting      = "WeaponSkillSetting"
  Panel_WeaponSkillRplc             = "WeaponSkillRplc"
  Panel_Stall_StallSetting        	= "StallSetting"
  Panel_Stall_StallUpgrade             = "StallUpgrade"
  Panel_Stall_StallBuy             	= "StallBuy" 
  Panel_Stall_StallMsg              = "StallMsg"
  Panel_Stall_StallBuyMsg			= "StallBuyMsg"
  //Panel_DynamicUI_Buff	      	= "Buff"
  Panel_DynamicUI_ProgressBar 	= "_ProgressBar"
  Panel_DynamicUI_FBSummaryProgressBar = "_FBSummaryProgressBar"
  Panel_DynamicUI_StrengthBar 	= "_StrengthBar"
  Panel_DynamicUI_QTEBar      	= "_QTEBar"
  Panel_DynamicUI_LevelUp     	= "_LevelUp"
  Panel_DynamicUI_WeatherChange = "_WeatherChange"
  Panel_DynamicUI_CenterStaticNotice = "_CenterStaticNotice"
  Panel_DynamicUI_NewEvent = "_NewEvent"
  Panel_DynamicUI_BossBlood     = "_BossBlood"
  Panel_FightingScreenHint_ScreenPopUpBoxMsg  = "ScreenPopUpBoxMsg"
  Panel_DynamicUI_Scene       	= "_Scene"
  Panel_DynamicUI_Tips       	= "_Tips"
  Panel_DynamicUI_Transducer       	= "_TransducerInfo"
  Panel_DynamicUI_BattleState             = "_BattleState"
  Panel_DynamicUI_UnlockAOGString         = "_UnlockAOGString"  
  Panel_DurabilityShowing                   = "DurabilityShowing"
  Panel_HudInfo_TargetInfo                  = "TargetInfo"
  Panel_HudInfo_MyInfo                  = "MyInfo"
  Panel_HudInfo_HeroInfo                ="HeroInfo"    
  Panel_HudInfo_TeamMate                  = "TeamMate"
	Panel_HudInfo_TeamHint              = "TeamHint_Mc"
  Panel_RClickMenu                  = "RClickMenu"
  Panel_RClickChildMenu                  = "RClickMenuChild"
  Panel_Team = "Team"
  Panel_TeamInvite = "TeamInvite"
  Panel_TeamSet = "TeamSet"
   Panel_TeamTarget = "TeamTarget"
	Panel_TeamAuction = "TeamAuction"
	Panel_TeamAuctionList = "TeamAuctionList"
  Panel_Warehouse = "Warehouse"
  Panel_Skillbar = "Skillbar"
  Panel_DamageLevel = "DamageLevel"
  Panel_Character               = "Charactor"
  Panel_Hero                    = "Hero"
  Panel_Mount                   = "Mount"
  panel_Reputation              = "Reputation"	
  Panel_TaskPopup               = "TaskPopup"
  Panel_OptionMenu              = "OptionMenu"
  Panel_OptionConfig            = "OptionConfig"
  Panel_OptionQuit              = "OptionQuit"
  Panel_Shop = "Shop"
  Panel_TradeP2P                = "TradeP2P"
  Panel_Dialog                  = "Dialog"
  Panel_DialogNew               = "DialogNew"
  Panel_Achievement             = "_Achievement"
  Panel_AchievementHint       = "_AchievementHint"
  Panel_MsgInfo                 = "MsgInfo"
	Panel_NoticeInfo                = "NoticeInfo"
  Panel_SceneHint               = "SceneHint"
  Panel_EquipGainHintBox        = "EquipGainHintBox"
  Panel_WeaponExp               = "WeaponExp"
  Panel_PickWidget_DorpItemTips            = "DropItemTips"
  Panel_MasterStudent           = "MasterStudent"
  Panel_MasterCongMain          = "MasterCongMain"
  Panel_MasterCongSub           = "MasterCongSub"
  Panel_MasterTips              ="MasterTips"
  Panel_MasterInvite            ="MasterInvite"
  Panel_Mail                    = "Mail"
  Panel_Mail_MailRead                = "MailRead"
  Panel_BattleTeam_BattleTeamCreate        = "BattleTeamCreate"
  Panel_BattleTeam              = "BattleTeam"
  Panel_PVP_ArenaQueue              = "ArenaQueue"
  Panel_Skillbar_Menubar               = "Menubar" 
  Panel_MsgBall_MsgButtonBar          = "MsgButtonBar"
  Panel_Skillbar_Buff                  = "Buff"	
  
  Panel_Benchmark          = "Benchmark"
  Panel_Guild_Warehouse          = "GuildWarehouse"
  Panel_Guild_Create             = "GuildCreate"
  Panel_Guild_View               = "GuildView"
  Panel_Guild_WarView            = "GuildWarView"
  Panel_Guild_SabacEnroll        = "SabacEnrollView"
  Panel_Guild_Main               = "GuildMain"
  Panel_Guild_Territory          = "GuildTerritory"
  Panel_Guild_Support			 = "GuildSupport"
  Panel_Guild_Welfare            = "GuildWelfare"
  Panel_Guild_DungeonBegin       = "GuildDungeonBegin"
  Panel_Guild_WarEnd             = "GuildWarEnd"
  Panel_Guild_WarHint            = "GuildWarHint"
  Panel_Guild_WarStart           = "GuildWarStart"
  Panel_Guild_Instruction        = "GuildInstruction"
  Panel_Guild_DungeonEnd         = "GuildDungeonEnd"
  Panel_Guild_DungeonEnter       = "GuildDungeonEnter"
  Panel_Guild_DungeonExit        = "GuildDungeonExit"
  Panel_Guild_WelfareUpgrade     = "GuildWelfareUpgrade"
  Panel_Guild_BuildingCreate     = "GuildBuildingCreate"
  Panel_Guild_BuildingUpgradeMulti = "GuildBuildingUpgradeMulti"
  Panel_Guild_BuildingUpgradeSingle = "GuildBuildingUpgradeSingle"

  Panel_EquipCompose             = "EquipCompose"
  Panel_EquipDecompose           = "EquipDecompose"
  Panel_EquipDecomposeHint       = "EquipDecomposeHint"
  Panel_BackDecorated            = "BackDecorated"
  Panel_Activity                 = "Activity"
  Panel_HeavenGasExchang         = "HeavenGasExchang"
  Panel_HeavenPractise			 = "HeavenPractise"
  Panel_ActivityQuit             = "ActivityQuit"
  Panel_GameAnnouncement         = "GameAnnouncement"
  //Panel_UndergroundTrade		 = "UndergroundTrade"
  //Panel_UTSell				 = "UTSell"
  //Panel_UTBid					 = "UTBid"
  Panel_HintCombat_BossQTE       = "BossQTE";
  Panel_RankList				 = "RankList";
  Panel_RankConfig				 = "RankConfig";
  Panel_HeroRank                 = "HeroRank";
  Panel_Mall                     = "Mall"
  Panel_MallFriend               = "MallFriend"
  Panel_MallBuyGoods             = "MallBuyGoods"
  Panel_MallEquipRole			 = "MallEquipRole"
  Panel_AuctionView              = "Auction";
  Panel_Vip                      = "Vip"
  Panel_Recharge                 = "Recharge"
  Panel_DailySignIn              = "DailySignIn"
  Panel_SpecialExpApply          = "SpecialExpApply"
  Panel_Hurt                     = "Hurt";
  Panel_GoldStore                = "GoldStore";
  Panel_KillRecordPanel			 = "KillRecord";
  Panel_KillCountPanel			 = "KillCount";
  //Panel_ItemStore                ="ItemStore";
  Panel_GuideOption              = "GuideOption";
	
  Panel_Helper                   = "Helper";
  Panel_HelperPlayer             = "HelperPlayer";

  Panel_ShopMessage              = "ShopMessage";
  Panel_Handbook                 = "Handbook";
  Panel_Journey                  = "Journey";
  Panel_JourneyView              = "JourneyView";
  Panel_JourneyComplete          = "JourneyComplete"; 
  Panel_Assist                   = "Assist"
  Panel_EquipRemake              = "EquipRemake"
  Panel_RemakeBook               = "RemakeBook"
  Panel_RemakeFactory            = "RemakeFactory"
  Panel_Jigsaw                   = "Jigsaw"
  Panel_DynamicPwd               = "DynamicPwd"
}
