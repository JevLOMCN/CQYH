#ifndef _ROLE_H_
#define _ROLE_H_

#include "role_def.h"
#include "scene_def.h"
#include "teleport_def.h"
#include "hero_def.h"

#include "character.h"
#include "spell_book_role.h"
#include "spell_slot.h"
#include "processed_mails.h"
#include "item_bag.h"
#include "trade.h"
#include "stall.h"
#include "quest_list.h"
#include "kill_npc_quest.h"
#include "reputation.h"
#include "friend.h"
#include "rsa_sign.h"
#include "title.h"
#include "achievement_list.h"
#include "spirit.h"
#include "hero_attr_data.h"
#include "shield_list.h"
#include "role_guild_data.h"
#include "bt_manager.h"
#include "GPKitSvr.h"
#include "arena.h"
#include "master.h"
#include "asimov_role.h"
#include "role_kick_mgr.h"
#include "gamble.h"

class CAchievementsList;
class CAuraInstance;
class CHero;
struct ZONE_INSTANCE_DATA;

#ifdef _ROBOT
    #define KICK_ROLE(__role__, __kick_type__) 
#else // not _ROBOT
    #define KICK_ROLE(__role__, __kick_type__)                                          \
        do                                                                              \
        {                                                                               \
            if (__role__)                                                               \
                __role__->Kick(	__kick_type__, __FILE__, __FUNCTION__, __LINE__);		\
        } while (0)
#endif  // _ROBOT

class CRole : public CCharacter
{
public:
	CRole();
	virtual ~CRole();

	BOOL Init(UINT32 dwObjID, UINT32 dwSessionID);
	BOOL UnInit(void);
	BOOL MainLoop(void);

	BOOL Load(const char* pcData, UINT32 dwSize);
	BOOL Save(char* pData, UINT32& rdwSize);
	static BOOL CheckDBDataLength(void);
	void inline Prefetch(void);

    virtual INT32 GetObjType(void);
    virtual INT32 GetSpecies(void);
    virtual BOOL SetLevel(INT32 nNewLevel);
	virtual INT32 GetPow(void);
	virtual BOOL IsLoading(void);

    inline void SetUserName(const char* pcszUserName);
    inline const char* GetUserName(void);

	inline void SetIpAddr(UINT32 dwIpAddr);
	inline UINT32 GetIpAddr(void);
	inline void SetIpWhiteList(BOOL bIpWhiteList);
	inline BOOL IsIpWhiteList(void);
	inline void SetUserWhiteList(BOOL bUserWhiteList);
	inline BOOL IsUserWhiteList(void);

    inline UINT8 GetHairType(void) const;
    inline UINT8 GetHairColor(void) const;
    inline UINT8 GetFaceType(void) const;
    inline UINT8 GetSkinColor(void) const;

    void SetScene(CScene* poScene);

	inline void SetState(ERoleState eState);
	inline ERoleState GetState();

    inline void SetFcmState(UINT8 eFcmState);
    inline UINT8 GetFcmState(void);
	inline INT32 GetFcmRate(void);

    inline void SetFreeMode(UINT8 byFreeMode);
    inline BOOL IsFreeMode(void);

    inline INT32 GetGMLevel();
	inline void SetGMLevel(INT32 nGMLevel);

    inline INT32 GetFaction();
    void SetFaction(INT32 nFaction);
    inline void SetInJump(void);
    inline void ClearInJump(void);
	inline BOOL IsInJump(void);

    inline void SetInLogin(void);
    inline void ClearInLogin(void);
    inline BOOL IsInLogin(void);

	inline UINT32 GetSessionID(void);
    inline INT32 GetServerGroup(void);
	inline BASE_LIST_NODE& GetSceneListNode(void);
    inline BASE_LIST_NODE& GetSaveListNode(void);

	inline UINT32 GetCreateTime(void);
	inline UINT32 GetPkValue(void);
	void SetPkValue(UINT32 dwPkValue);

	inline INT32 GetComboKill(void);
	void IncComboKill(void);

	inline void SetSpellSerial(INT32 nSerial);

    INT32 GetVIPLevel(void);
    INT32 SubscribeVIP(INT32 nVIPLevel, UINT32 dwMonth);
	inline UINT32 GetVIPTime(INT32 nVIPLevel);
	inline BOOL SetVIPTime(INT32 nVIPLevel, UINT32 dwVipTime);

	INT32 GetTotalOnlineTime(void);
	INT32 GetLevelOnlineTime(void);

	void Kick();
	void Kick(INT32 eKickType, const char* pcszFileName, const char* pcszFuncName, INT32 nLineNum);
	BOOL ErrorCodeNotify(INT32 nErrCode, INT32 nParam1, INT32 nParam2);

	/************************************************************************/
	// move begin

	virtual BOOL JumpTo(INT64 llX, INT64 llY, INT64 llZ, INT32 nFaceDirection);
	void GetNextJumpPoint(INT32& rnX, INT32& rnY, INT32& rnZ);
	UINT32 GetJumpRandomNumber(void);
	BOOL TransferRole(INT64 llDestX, INT64 llDestY, INT64 llDestZ, INT32 nRange, INT32 nCount);
	BOOL ForceSyncPos(void);
    virtual BOOL Die(INT32 nParam);
    BOOL CheckVolume(void);
	void Hide(BOOL bHidden);
	inline BOOL IsHidden(void);

    // move end
	/************************************************************************/

	inline void SetSceneTempID(INT32 nSceneTempID);
	inline void SetSceneID(INT32 nSceneID);

	inline void SetName(const TCHAR* pcszName);
    virtual const TCHAR* GetName(void);

	inline INT32 GetSceneTempID(void);
	inline INT32 GetSceneID(void);

	//inline INT32 GetSide(void);
	//inline void SetSide(UINT8 bySide);

    inline UINT32 GetForbidChatTime();       // 0 permission
    inline void SetForbidChatTime(UINT32 dwForbidChatTime);

	inline UINT32 GetNormalChatTimer(void);
	inline void SetNormalChatTimer(INT32 nNormalChatTimer);

	inline UINT32 GetGroupChatTimer(void);
	inline void SetGroupChatTimer(INT32 nGroupChatTimer);

    inline UINT32 GetLastAdjacentNoTeamRoleFrame(void);
    inline void	SetLastAdjacentNoTeamRoleFrame(UINT32 dwFrame);

    inline UINT32 GetLastAdjacentTeamFrame(void);
    inline void	SetLastAdjacentTeamFrame(UINT32 dwFrame);

	BOOL ChangePkMode(INT32 nMode);
	inline INT32 GetPkMode(void);
    inline ERoleNameColor GetNameColor(void);
	inline BOOL IsRedName(void);
	BOOL IsGreyName(void);
	void SetGreyName(void);

	void CombatPoseSwitch(BOOL bOn, INT32 nFlag);

	CSpellSet* CreateSpellSet(INT32 nSpellID);

	INT32 LearnSpell(INT32 nSpellID, INT32 nSpellLevel, BOOL bNeedSave);
	INT32 EquipSpell(INT32 nSlotID, INT32 nSpellID);
	BOOL RemoveSpell(INT32 nSpellID);
	virtual INT32 EquipRune(const RUNE_DATA* pRuneData, Item* poItem);
	virtual BOOL UnEquipRune(const RUNE_DATA* pRuneData, Item* poItem);

	BOOL EquipRune(INT32 nSpell, INT32 nRuneSlot);

	BOOL UnlockSpellSlot(INT32 nSlotID);
	BOOL AddSpellPts(INT32 nSpellID, INT32 nPts);

	BOOL AddItemSpell(Item* poItem, INT32 nSpellID, INT32 nLevel);
	BOOL DelItemSpell(Item* poItem, INT32 nSpellID);

	inline void SetClientUploadTargetID(UINT32 dwTargetID);
	inline UINT32 GetClientUploadTargetID(void);

    virtual BOOL DoSyncAttrChange(INT32 eAttrType);
    virtual INT32 GetWeaponType(void);
    virtual ISpellBook& GetSpellBook(void);
    virtual CSpellSlot* GetSpellSlot(INT32 nIndex);
    virtual void ClearSpellForReloading(void);
    virtual INT32 CastSpell(CSpell* poSpell, CCharacter* poTarget, INT64 nTargetX, INT64 nTargetY, INT64 nTargetZ);
    virtual BOOL AddCooldown(UINT32 dwCooldownID, INT32& nCooldownFrm);
    virtual BOOL ChangeCooldown(UINT32 dwCooldownID, INT32& nCooldownFrm);
	virtual BOOL ResetCooldown(UINT32 dwCooldownID);
	virtual void SetInCutScene(BOOL bInCutScene, INT32 nDurationFrm);

    virtual void OnHurt(CCharacter* poDealer, CBullet* poBullet, INT32 nEffectValue);
	virtual BOOL Transform(BOOL bTransform);

	inline void ClearSpellCache(void);
	inline BOOL CacheSpell(INT32 nSpellID, INT32 nSpellSerial, UINT32 dwTargetID, INT64 llX, INT64 llY, INT64 llZ);
	BOOL CastCachedSpell(void);

	inline void PushEquippingSpell(CSpell* poSpell);
	virtual CSpell* GetEquippingSpell(void);
	inline void PopEquippingSpell(void);

	inline void SetLastSlot(INT32 nSlotID);
	inline INT32 GetLastSlot(void);

	void ClearAllCooldown(void);

	INT64 AddExp(INT64 nExp, BOOL bByKillNpc, BOOL bDailyLimitExp);
    void SubExp(INT32 nExp);
    inline INT32 GetExp(void);
    inline INT32 GetTodayExp(void);
    inline INT32 GetExtraExp(void);
	void AddExtraExp(INT32 nExp);
	inline void SubExtraExp(INT32 nExp);

    void AddActivity(INT32 nActivity);
	inline INT32 GetActivity();

    inline void AddActivityReward(INT32 nActivityRewardIndex);
    inline BOOL GetActivityRewardByIndex(INT32 nActivityRewardIndex);
    inline BOOL GetActivityReward();
    inline void ClearActivity();

	INT32 ActivateMount(INT32 nMountID);
	INT32 OnMount(INT32 nMountID);
	void OffMount(void);
	INT32 EquipWing(INT32 nWingID);
	void UnequipWing(void);
	// wing panel (1.05 UnrealWing): level / owned wings are role variables (ROLE_VAR_WING_*), see role.cpp
	INT32 GetWingLevel(void);
	UINT32 GetWingMask(void);
	BOOL HasWing(INT32 nWingID);
	BOOL GrantWing(INT32 nWingID);
	void WingOperate(INT32 nOp, INT32 nParam);
	INT32 UpgradeWing(void);
	void ApplyWingLevelAttr(INT32 nFrom, INT32 nTo);
	void SyncWingInfo(BOOL bUpgraded);
	inline void BeginMount(void);
	inline INT32 NextMount(void);
	inline INT32 GetMountCount(void);

    inline CProMails* GetProMails(void);
	inline CItemBag* GetItemBag(void);
	inline CStall* GetStall(void);
    inline CTrade* GetTrade(void);
	inline CFriend* GetFriend(void);

    inline CShieldList* GetShieldList(void);

    inline CArena& GetArena(void);

    inline CAsimovRole* GetAsimovRole();

    inline CMaster& GetMaster(void);
    inline CTitle& GetTitle(void);
    inline UINT32 GetGuildID(void);
    inline void SetGuildID(UINT32 dwGuildID);
    inline CRoleGuildData& GetRoleGuildData();
	BOOL IsSabacGuildMember(void);

	INT32 PickUp(UINT32 dwDropID);

	INT32 BuyItem(INT32 nShopID, INT32 nShopItemIndex, INT32 nItemCount);
	INT32 SellItem(UINT8 byConSlot, UINT8 byItemSlot, INT16 shItemCount);
    INT32 ViewSoldItems();
    INT32 BuyBackItem(UINT8 bySlot);
    INT32 RepairItem(UINT8 byConSlot, UINT8 byItemSlot, BOOL bSpecial);
    INT32 RepairAllEquippedItem();

    void OpenStall();
    void CloseStall();

    INT32 ExchangeAddOrder(INT32 nItemIDRequire, INT32 nItemCountRequire, UINT8 byOrderType, INT32 nPriceOrder, INT32 nCountOrder);

    BOOL CheckDistance(CSceneObject* poSceneObject);
    INT32 CanInteract(CSceneObject* poSceneObject);
    BOOL Interact(CNpc* poNpc);
    INT32 SelectOption(INT32 nOption);
    BOOL CheckOption(INT32 nOption);
    BOOL StopInteract(void);

	inline CQuestList& GetQuestList();
	inline CKillNpcQuest& GetKillNpcQuest(void);
	inline CReputation& GetReputation(void);
	inline CAchievementsList& GetAchievementList(void);	
	inline CSpirit& GetSpirit(void);

	inline UINT32 GetInteractNpc();
	inline BOOL IsRobot();

	inline void SetTeamID(UINT32 dwTeamID);
	inline UINT32 GetTeamID();

	BOOL IsMasterRelationship(CRole* poRole);
	BOOL HasMasterRelationshipInTeam(void);

	inline void SetBattleTeamID(UINT32 dwTeamID);
	inline UINT32 GetBattleTeamID();

	inline void SetTeleportID(UINT32 dwTeleport);
	inline UINT32 GetTeleportID(void);
	void SaveReturnPos(void);
	const ABSOLUTE_SCENE_POS& GetStaticReturnPos(void);
	const ABSOLUTE_SCENE_POS& GetRevivePos(void);
    BOOL JumpToReturnPos(void);
    BOOL JumpToRevivePos(void);

    inline INT32 GetVariable(INT32 nIndex);
    void SetVariable(INT32 nIndex, INT32 nValue);
    void AddVariable(INT32 nIndex, INT32 nCount);
    inline INT32 GetClientVariable(INT32 nIndex);
    BOOL SetClientVariable(INT32 nIndex, INT32 nValue);

    void UpdateLastSaveFrame(void);
    inline INT32 GetLastSaveFrame(void);
	
	inline void SetLoadingSceneEndFrame(INT32 nFrame);
	inline INT32 GetLoadingSceneEndFrame(void);

	inline INT32 GetTryReviveFrm(INT32 nType);
	inline INT32 GetTryReviveCount(INT32 nType);

    BOOL FinishScene(void);

	//BOOL CreateArenaTeamPay(void);
	BOOL ForceLeaveTeam(void);

	BOOL UpdateDungeonRecordMask(UINT32 dwSceneTempID);
	UINT8 GetDungeonRecordMask(UINT32 dwSceneTempID);
    BOOL CheckDungeonRecordMask(UINT32 dwSceneTempID);

    BOOL UpdateDungeonEnterCount(UINT32 dwSceneTempID);
    void ClearDungeonEnterCount(void);
    UINT8 GetDungeonEnterCount(UINT32 dwSceneTempID);

    BOOL IsFirstPassDungeon(UINT32 dwSceneTempID);
    void GetDungeonRecord(UINT8* pbyDungeonRecord, INT32 nSize);

	inline void SetBattleEnterPos(INT32 nSceneTempID, INT32 nSceneID, UINT8 bySide);
	inline const ENTER_POS& GetBattleEnterPos();

	BOOL UpdateRankListNode(INT32 nRankListType, INT32 nValue1, INT32 nValue2);

	BOOL Logout(ERoleLogoutType eType, UINT32 dwDelayFrame);
    inline ERoleLogoutType GetLogoutType();
	inline void ClearLogoutFrame();
	inline INT32 GetLogoutFrame();
    void SetLogoutFrame(UINT32 dwDelayFrame);
    BOOL Exit(void);

	BOOL ActivateTeleportPlatform(UINT8 byIndex);
	void* GetTeleportform();
	BOOL CheckTeleportPlatform(UINT8 byIndex);

	INT32 TryRevive(INT32 nType);
    virtual BOOL Revive(INT32 nParam, INT32 nHpPercent);
	inline INT32 GetNextReviveFrm(void);

    void TryDailyRefresh();
    void TryWeeklyRefresh();

	INT32 OnKillNpc(CNpc* pNpc, INT32 nExp);
	INT32 OnKillPlayer(CRole* poVic);

	inline INT32 GetConsecutiveKillNpcCount();

	inline INT32 GetConsecutiveKillPlayerCount();

	inline UINT32 GetLastLineSceneTempID(void);
	inline void SetLastLineSceneTempID(UINT32 dwSceneTempID);
	inline UINT32 GetLastLineSceneID(void);
	inline void SetLastLineSceneID(UINT32 dwSceneID);

	/************************************************************************/
	// Hero Begin
	inline CHero* GetHero(void);

	INT32 HireHero(INT32 nLevel, INT32 nClass, INT32 nSex, UINT32 dwHeroExp, INT32 eAiType, BOOL bFirstTime);
	INT32 FireHero(void);

	INT32 SummonHero(void);
	INT32 RecallHero(BOOL bNotify);

	// Hero End 
	/************************************************************************/
	inline INT32 GetPetState(void);
	inline void SetPetState(INT32 nState);

	inline INT32 GetPetCount(void);
    inline INT32 GetMaxPetCount(void);
    inline INT32 GetPetIndex(UINT32 dwNpcID);

    inline UINT32 GetPetNpcID(INT32 nIndex);
    inline UINT32 GetPetType(INT32 nIndex);

    BOOL AddPet(UINT32 dwNpcID, INT32 nPetType);
    BOOL DelPet(UINT32 dwNpcID);
    inline BOOL IsMyPet(UINT32 dwNpcID);

    BOOL RecoverPet(void);
    BOOL PackUpPet(void);
    BOOL ClearPet(void);
    BOOL ClearPet(INT32 nSpellEffectGroup);

	BOOL ClearRandomPet(void);
	BOOL CheckPet(INT32 nSpellEffectGroup);

	INT32 QueryRoleLocation(const char* pcszRoleName);
	INT32 RecallTeamMember(void);

#ifndef _ROBOT
	inline SGPK::IGPKCSAuth* GetGpkAuth(void);
#endif
    BOOL GMCreateGuild(char *pszGuildName, char *pszAnnounce);

	BOOL IsCheckInValid(void);
	INT32 CheckIn(void);
	BOOL CanCheckInToday(void);
	inline INT32 GetLastCheckInIndex(void);

    inline UINT32 GetTotalExerciseTime() const;
    inline void SetTotalExerciseTime(UINT32 dwTime);
    inline UINT32 GetStartExerciseTime() const;
    inline void SetStartExerciseTime(UINT32 dwTime);
    inline BOOL GetDoingSubTime() const;
    inline void SetDoingSubTime(BOOL bDoingSubTime);

    UINT32 GetCurExerciseTime() const;

	void StartAntiCheat(void);
	void EndAntiCheat(void);

	void SuccessAntiCheat(void);
	INT32 FailAntiCheat(BOOL bExit, BOOL bIsTimeout);

	void GenerateAntiCheatJigsaw(void);

	inline INT32 GetJigsawAnswerIdx(void);
	inline UINT32 GetJigsawSn(void);
	inline void SetAntiCheatLevel(INT32 nLevel);

	/************************************************************************/
	// For Robot

#ifdef _ROBOT
public:
    inline void SetExp(INT32 nExp);
    inline void SetServerGroup(UINT8 byGroupID);
	inline void IncServerFrame(void) {m_nServerFrame++;}
    inline void SetServerFrame(INT32 nServerFrame){m_nServerFrame = nServerFrame;}
	inline INT32 GetServerFrame(void) {return m_nServerFrame;}

    inline UINT32 GetGuildCD(void)  { return m_dwGuildCD; }
    inline void SetGuildCD(UINT32 dwGuildCD)    { m_dwGuildCD = dwGuildCD; }

	inline void SetBattleQueueState(BOOL bQueue);
	inline BOOL	IsInBattleQueue();
	inline void SetArenaQueueState(BOOL bQueue);
	inline BOOL IsInArenaQueue();

    inline void AddMail(UINT32 mailid) { m_lstMails.insert(mailid); }
    inline void DelMail(UINT32 mailid) { m_lstMails.erase(mailid); }
    inline void ClearMail() { m_lstMails.clear(); }
    inline UINT32 GetFirstMail() const;

    inline void AddAttach(UINT32 mailid) { m_lstAttachments.insert(mailid); }
    inline void DelAttach(UINT32 mailid) { m_lstAttachments.erase(mailid); }
    inline void ClearAttach() { m_lstAttachments.clear(); }
    inline UINT32 GetFirstAttach() const;

    inline void AddAuctionItemID(INT32 itemid) { m_lstAuctionItemID.insert(itemid); }
    inline void ClearAuctionItemID() { m_lstAuctionItemID.clear(); }
    inline INT32 GetRandomAuctionItemID() const;

    inline void AddSellingAuction(UINT32 auctionid) { m_lstAuctionSellingItems.insert(auctionid); }
    inline void DelSellingAuction(UINT32 auctionid) { m_lstAuctionSellingItems.erase(auctionid); }
    inline void ClearSellingAuction() { m_lstAuctionSellingItems.clear(); }
    inline UINT32 GetFirstSellingAuction() const;

    inline void AddBuyingAuction(UINT32 auctionid) { m_lstAuctionBuyingItems.insert(auctionid); }
    inline void DelBuyingAuction(UINT32 auctionid) { m_lstAuctionBuyingItems.erase(auctionid); }
    inline void ClearBuyingAuction() { m_lstAuctionBuyingItems.clear(); }
    inline UINT32 GetFirstBuyingAuction() const;

    inline const char* GetCBFuncName() const { return m_szCBFuncName; }
    inline void SetCBFuncName(const char* funcname) { strxcpy(m_szCBFuncName, funcname, sizeof(m_szCBFuncName)); }
    inline INT32 GetWaitNpcTemplateID() const { return m_nWaitNpcTemplateID; }
    inline void SetWaitNpcTemplateID(INT32 templateid) { m_nWaitNpcTemplateID = templateid; }

private:
    BOOL _TryMove(INT64 llDestX, INT64 llDestY, BOOL bLastStep);

private:
	INT32   m_nServerFrame;
	BOOL	m_bInBattleQueue;
	BOOL	m_bInArenaQueue;

	UINT32	m_dwReviveFrame;

    INT32	m_nFindState;
    INT32	m_nFindTimer;
    INT32	m_nFindTimes;
    INT32	m_nPathSide;

    std::set<UINT32> m_lstMails;
    std::set<UINT32> m_lstAttachments;

    std::set<INT32> m_lstAuctionItemID;
    std::set<UINT32> m_lstAuctionSellingItems;
    std::set<UINT32> m_lstAuctionBuyingItems;

    UINT32  m_dwGuildCD;

    char m_szCBFuncName[fUNC_NAME_LEN];
    INT32 m_nWaitNpcTemplateID;

#else
    inline UINT32 GetFirstMail() const { return 0; }
    inline UINT32 GetFirstAttach() const { return 0; }
    inline INT32 GetRandomAuctionItemID() const { return 0; }
    inline UINT32 GetFirstSellingAuction() const { return 0; }
    inline UINT32 GetFirstBuyingAuction() const { return 0; }

#endif  // _ROBOT
	/************************************************************************/


	// Additional custom functions
public:
	UINT32 GetGameTimeCard_UINT32();	       // Get the current time-card expiration time (seconds)
	void AddGameTimeCard(UINT32 seconds);	   // Add time-card duration (seconds)
	void NotifyRole_GameTimeCard();			   // Send a private message with remaining time
	BOOL GetGameTimeCard_HaveTime();           // Check whether the time card has expired

	INT32 GetRuneItemHighResult();			                // Get advanced inscription refinement result
	void SetRuneItemHighResult(INT32 RuneID);               // Set advanced inscription refinement result
	INT32 GetRuneRootCount(INT32 nType, INT32 cls);         // Get inscription node value
	void IncRuneRootCount(INT32 nType, INT32 cls);          // Increment inscription node value
	void NotifyRole_RuneRootCount(INT32 nType, INT32 cls);  // Send a private message with the inscription node value

	void ResetLuckRootCount();          // Reset blessing oil node value
	INT32 GetLuckRootCount();           // Get blessing oil node value
	void IncLuckRootCount();	        // Increment blessing oil node value
	void NotifyRole_LuckRootCount();    // Send a private message with the blessing oil node value

protected:
    void _On1SecMainLoop(void);
    void _On8SecMainLoop(void);
    void _On5MinMainLoop(void);
    void _On1HourMainLoop(void);

    virtual BOOL _ChangeRegion(CRegion* poDestRegion);

    virtual BOOL _OnCastOver(void);
	virtual BOOL _ComputeAttribute(INT32 nNewLevel);

	virtual BOOL _GridMove(INT64 llDestX, INT64 llDestY);

    void _CheckInteractBT(void);

	typedef BOOL (CRole::*_ReviveFunc)(void);
	static _ReviveFunc m_ReviveFunc[crtTotal];

//	BOOL _OnInstantRevive(void);
	BOOL _OnAtCurrentPosRevive(void);
	BOOL _OnToGraveyardRevive(void);

	BOOL _CaculateKillNpc(void);

private:
	BOOL				m_bIsLoading;
    UINT8				m_byHairType;
    UINT8				m_byHairColor;
    UINT8				m_byFaceType;
    UINT8				m_bySkinColor;
    INT32               m_nGMLevel;
	INT32		        m_nSceneTempID;
	INT32		        m_nSceneID;
	char		        m_szName[ROLE_NAME_LEN];
    char                m_szUserName[USER_NAME_LEN];
	ERoleState          m_eState;
    UINT8               m_eFcmState;
    BOOL                m_bFreeMode;    // (TRUE = features restricted; FALSE = no restrictions)
    BOOL                m_bInJump;
    BOOL                m_bInLogin;
	UINT32				m_dwJumpRandomNumber;
    BOOL                m_bKicked;
	ERoleLogoutType		m_eLogoutType;
    INT32			    m_nLogoutFrame;
	UINT32              m_dwSessionID;
	BASE_LIST_NODE      m_SceneListNode;
    BASE_LIST_NODE      m_SaveListNode;
	UINT32              m_dwTeamID;
	BOOL				m_bHidden;
	BOOL				m_bIpWhiteList;
	BOOL				m_bUserWhiteList;
	UINT32				m_dwIpAddr;

	UINT32              m_dwTeleportID;
	UINT32				m_dwCreateTime;
	UINT32				m_dwSaveTime;
    UINT32              m_dwVIPTime[vlTotal];

	ABSOLUTE_SCENE_POS  m_StaticReturnPos;

	struct REVIVE_POS : public ABSOLUTE_SCENE_POS
	{
		INT32			nRevivePtID;
	};

	REVIVE_POS			m_RevivePos;

	ENTER_POS			m_EnterBattlePos;
	//ENTER_POS			m_EnterArenaPos;

	INT32		        m_nExp;
	INT32               m_nExpToday;
    INT32               m_nExtraExp;

	INT32               m_nActivity;;
    INT32               m_nActivityReward;

	CRoleSpellBook	    m_SpellBook;

	CSpellSlot*			m_poCurrentSlot;
	CSpellSlot			m_SpellSlot[qssSlotCount];
	CSpellSlot			m_SpellSlotForTransform[qssSlotCount];

	typedef std::stack<CSpell*> TSpellStack;
	TSpellStack			m_EquippingSpellStack;

	INT32				m_nLastCastSlotID;

	INT32				m_nPkMode;
	INT32				m_dwPkValue;
	INT32				m_nComboKill;
	INT32			    m_nGreyNameFrame;  // Gray-name frame count

    CProMails           m_ProMails;
	CItemBag            m_ItemBag;
    CTrade              m_Trade;
    CStall              m_Stall;
	CFriend             m_Friend;
    CShieldList         m_ShieldList;

    CMaster             m_Master;

    UINT32              m_dwGuildID;
	CRoleGuildData      m_Guild;

    CArena              m_Arena;
	CGamble             m_Gamble;

#ifdef _ROBOT
    CAsimovRole         m_AsimovRole;
#endif // _ROBOT

	CQuestList          m_QuestList;
	CKillNpcQuest		m_KillNpcQuest;
	CReputation         m_Reputation;

    CTitle              m_Title;
	CAchievementsList	m_AchievementList;
	CSpirit				m_Spirit;

	CHero*				m_poHero;

	UINT32              m_dwInteractNpcID;
    INT32               m_nInteractOption;

    UINT32              m_dwForbidChatTime;
	INT32				m_nNormalChatTimer;
	INT32				m_nGroupChatTimer;

    UINT32              m_dwLastAdjacentNoTeamRoleFrame;
    UINT32              m_dwLastAdjacentTeamFrame;

    INT32				m_nLoginFrame;
	INT32				m_nLevelUpFrame;
	INT32				m_nLoadingSceneEndFrame;
    INT32               m_nVarList[MAX_ROLE_VARIABLE_COUNT];
    INT32               m_nClientVarList[MAX_CLIENT_VARIABLE_COUNT];
	
	UINT8				m_nTeleportPlatform[MAX_TELEPORT_PLATFORM_COUNT/8];

	INT32				m_nTryReviveFrm[crtTotal];
	INT32				m_nTryReviveCount[crtTotal];
	INT32				m_eReviveType;

	INT32				m_nNextFreeReviveFrm;
	INT32				m_nKilledByPlayer;
	INT32				m_nNextResetKilledTimesByPlayerFrm;

	UINT32				m_dwBattleTeamID;

	struct SpellCache
	{
		INT32		m_nSpellID;
		INT32		m_nSpellSerial;
		UINT32		m_dwTargetID;
		INT64		m_llX;
		INT64		m_llY;
		INT64		m_llZ;
	};
	SpellCache		m_SpellCache;
	UINT32			m_dwSpellCacheTimeoutFrm;
	UINT32			m_dwClientUploadTarget;

    INT32           m_nLastSaveFrame;

#pragma pack(push, 1)
    struct DUNGEON_RECORD
    {
        unsigned char   bEntered : 1;
        unsigned char   uEnterCount     : 7;
    }               m_lstDungeonRecord[MAX_SCENE_TEMPLATE_COUNT];
#pragma pack(pop)

    INT32           m_nServerGroupID;

	typedef std::set<INT32> TActivatedMountList;
	TActivatedMountList m_ActivedMountList;
	TActivatedMountList::iterator m_MountItr;

    BTCustomCtx     m_InteractCtx;

    UINT32          m_dwLastDailyRefreshTime;
    UINT32          m_dwLastWeeklyRefreshTime;

	INT32			m_nConsecutiveKillNpcCount;
	INT32			m_nConsecutiveKillNpcFrame;
	INT32			m_nConsecutiveKillNpcExp;

	INT32			m_nConsecutiveKillPlayerFrame;
	INT32			m_nConsecutiveKillPlayerCount;

	INT32			m_nMomentaryKillNpcCount[MOMENTARY_KILL_NPC_TIEM];
	INT32			m_nMomentaryKillNpcExp[MOMENTARY_KILL_NPC_TIEM];

    INT64			m_llLastSuccessPickupFrame;

	UINT32			m_dwLastLineSceneTempID;
	UINT32			m_dwLastLineSceneID;

	// anti-cheat
	UINT32			m_dwJigsawTimeoutTime;
	INT32			m_nJigsawRemainTime;
	UINT32			m_dwNextJigsawTime;
	INT32			m_nAntiCheatLevel;
	INT32			m_nAntiCheatPunishLevel;
	INT32			m_nJigsawAnswerIndex;	
	UINT32			m_dwJigsawSn;

    struct PET_DATA 
    {
        UINT32      dwNpcID;
        INT32       nPetType;
        UINT32      dwNpcTemplateID;
        INT32       nLevel;
        INT32       nInitLevel; 
        INT32       nMaxLevel;
        UINT32      dwAliveTime;
        INT32       nPetExp;
        INT32       nCurHp;
    };
	INT32			m_nPetState;
	INT32			m_nPetCount;
    UINT32          m_dwPetSaveTime;
    PET_DATA		m_Pet[MAX_PET_PER_ROLE];

#ifndef _ROBOT
	SGPK::IGPKCSAuth*		m_poGpkAuth;
#endif

	struct LAST_CHECKIN_INFO
	{
		UINT32 dwLastCheckInTime;
		INT32 nIndex;
	};

	LAST_CHECKIN_INFO m_LastCheckInInfo;

    UINT32          m_dwTotalExerciseTime;
    UINT32          m_dwStartExerciseTime;
    BOOL            m_bDoingSubTime;


	// Additional custom variables
private:
	UINT32	        m_dwGameTimeCard;         // Time-card balance (seconds remaining)
	BOOL            m_bDoTellGameTimeCard;    // Notify the player once when the time card expires
	BOOL            m_bHaveTime;              // Whether expired
	INT32           m_dwRuneID;               // Advanced inscription refinement result
	INT32           m_nRuneNormal_Warrior;    // Normal inscription refinement node
	INT32           m_nRuneNormal_Mage;
	INT32           m_nRuneNormal_Taoist;
	INT32           m_nRuneAdvanced_Warrior;  // Advanced inscription refinement node
	INT32           m_nRuneAdvanced_Mage;
	INT32           m_nRuneAdvanced_Taoist;
	INT32           m_nLuckRootCount;         // Blessing oil node
	

private:
    BOOL _InitNewRole(void);

private:
    BOOL _LoadBaseData(const char* pcData, UINT32 dwSize);

	BOOL _Load_v_1(const char* pcData, UINT32 dwSize);
	BOOL _Load_v_2(const char* pcData, UINT32 dwSize);
	BOOL _Load_v_3(const char* pcData, UINT32 dwSize);

	BOOL _LoadAura_v_1(const char* pcData, UINT32 dwSize);
	BOOL _LoadSingleAura_v_1(CAura* poAuraInfo, UINT32 dwCaster, UINT32 dwRemainFrm, INT32 nStackCount, ExtraParam& Param, INT32 nRootSpellID, INT32 nRootSpellLevel);
	BOOL _LoadSpell_v_1(const char* pcData, UINT32 dwSize);
	BOOL _LoadSpellSlot_v_1(const char* pcData, UINT32 dwSize);
	BOOL _LoadVariable_v_1(const char* pData, UINT32 dwSize);
	BOOL _LoadDungeonRecord_v_1(const char* pData, UINT32 dwSize);
	BOOL _LoadTeleportPlatform_v_1(const char* pData, UINT32 dwSize);
	BOOL _LoadMount_v_1(const char* pData, UINT32 dwSize);
	BOOL _LoadHero_v_1(const char* pData, UINT32 dwSize);
    BOOL _LoadMisc_v_1(const char* pData, UINT32 dwSize);
    BOOL _LoadPet_v_1(const char* pData, UINT32 dwSize);
    BOOL _LoadCoolDown_v_1(const char* pData, UINT32 dwSize);
    BOOL _LoadCheckIn_v_1(const char* pData, UINT32 dwSize);

	BOOL _LoadMisc_v_2(const char* pData, UINT32 dwSize);
	BOOL _LoadMisc_v_3(const char* pData, UINT32 dwSize);

private:

	BOOL _SaveBaseData(const char* pcData, UINT32& rdwSize);
	BOOL _SaveSpell(char* pData, UINT32& rdwSize);
	BOOL _SaveAura(char* pData, UINT32& rdwSize);
	BOOL _SaveSpellSlot(char* pData, UINT32& rdwSize);
	BOOL _SaveVariable(char* pData, UINT32& rdwSize);
	BOOL _SaveDungeonRecord(char* pData, UINT32& rdwSize);
	BOOL _SaveTeleportPlatform(char* pData, UINT32& rdwSize);
	BOOL _SaveMount(char* pData, UINT32& rdwSize);
	BOOL _SaveHero(char* pData, UINT32& rdwSize);
	BOOL _SaveMisc(char* pData, UINT32& rdwSize);
    BOOL _SavePet(char* pData, UINT32& rdwSize);
    BOOL _SaveCoolDown(char* pData, UINT32& rdwSize);
    BOOL _SaveCheckIn(char* pData, UINT32& rdwSize);
};

#include "role_inl.h"

#endif	// _ROLE_H_
