#ifndef _MESSAGE_HANDLER_H_
#define _MESSAGE_HANDLER_H_

#ifndef _ROBOT

#include "net.h"
#include "team_def.h"
#include "c2g_message.h"
#include "g2c_message.h"

class IMessageNotifier;
class IPacketAllocator;

class CScene;
class CRegion;
class CRole;
class CNpc;
class CHero;
class CCharacter;
class CSceneObject;
class CBullet;
struct Item;
class CDrop;
class CSpellInstance;
class CAuraInstance;
struct QUEST_INFO;
class CCliSession;
class CGSParty;
class CMissile;
class CDoodad;
class CSpell;
class CTeamInGS;
struct DropContent;
struct ItemInfo;
struct GS_SCENE_TEAM;
struct TeamAuctionItem;
struct JigsawImage;
struct JigsawAnswer;


class CCliMessageHandler
{
public:
	CCliMessageHandler();
	virtual ~CCliMessageHandler();

	static CCliMessageHandler& GetInstance(void);

	BOOL Init(IMessageNotifier* poNotifier, IPacketAllocator* poPacketAllocator);
	BOOL UnInit(void);
	BOOL OnRecv(CRole* poRole, const char* pData);

	BOOL RealKickUser(UINT32 dwRoleID);

	inline void SetMsgPrintState(INT32 nMsgID, BOOL nState);
	inline INT32 GetMsgPrintState(INT32 nMsgID);

	/************************************************************************/
	// gs 2 ms
	BOOL DoQueryScene(CRole* poRole, INT32 nSceneTempID, INT32 nSceneID, 
		INT32 nDestX, INT32 nDestY, INT32 nDestZ, INT32 nFaceDirection);

	BOOL DoMSChatMessage(UINT32 dwSourceID, const char* pSrcName, UINT32 dwTargetID, UINT32 dwLocation, INT32 nServerGroupID,
		BOOL bGM, const char* pcszMessage, INT32 nMessageSize);

	BOOL DoMSDropLimit(INT32 nItemID, INT32 nItemCount);

	BOOL DoMSAuctionSell(INT32 nResult, UINT32 dwRoleID, UINT8 byTimeType, UINT8 byOpen, INT64 llPrice, INT64 llBuyoutPrice, INT64 llDepositePrice, CHAR* szItemInfo, UINT32 dwItemInfoSize);
	BOOL DoMSAuctionBuy(INT32 nResult, UINT32 dwRoleID, UINT32 dwAuctionID, INT64 llPrice);
	BOOL DoMSAuctionPay(INT32 nResult, UINT32 dwRoleID, UINT32 dwAuctionID, INT64 llPrice);

    BOOL DoMSExchangeAddOrder(INT32 nResult, UINT32 dwRoleID, INT32 nItemIDRequire, INT32 nItemCountRequire, UINT8 byOrderType, INT32 nPriceOrder, INT32 nCountOrder);

    BOOL DoMSGetBlackMarketLimitInfo(UINT32 dwRoleID);
    BOOL DoMSCheckBlackMarketProduct(UINT32 dwRoleID, INT32 nBlackMarketProductID);

    BOOL DoMSAddExerciseTime(UINT32 dwRoleID, INT32 nHour);

	BOOL DoMSUpdateRankListData(CRole* poRole);
	BOOL DoMSUpdateRankListNode(INT32 nServerGroup, INT32 nRankListType, INT64 llKey, INT64 llValue);
	BOOL DoMSAdventureTrigger(UINT32 dwRoleID, INT32 nAdventureID, INT32 nVarID, INT32 nMaxCount, INT32 nChance);

	BOOL DoMSSendAward2Ump(UINT32 dwRoleID, INT32 nAwardType, char* pAwardNumber);
	BOOL DoMSQueryRoleLocation(CRole* poRole, const char* pcszRoleName, BOOL bFromItem);
    BOOL DoMSTakeRole(CRole* poRole, const char* pcszRoleName);
	BOOL DoMSRecallTeamMember(CRole* poRole);

	BOOL DoMSSetServerVariable(INT32 nServerGroup, UINT64 qwKey, BOOL bAbsoluteValue, INT32 nVarValue);

	BOOL DoMSFreezeLogin(CRole* poRole, UINT32 dwFreezeTime);
	BOOL DoMSTemporaryCharge(UINT32 dwRoleID);

	/************************************************************************/
	// gs 2 client

	// login
	BOOL DoChangeSceneEnter(CRole* poRole, BOOL bIsInJump);

	BOOL DoGpkDynCode(CRole* poRole);
	BOOL DoGpkAuthData(CRole* poRole);
    BOOL DoSyncDataError(CRole* poRole);

	// move & sync pos
	BOOL DoSyncFrame(CRole* poRole);
	BOOL DoPingAck(CRole* poRole, UINT32 dwPingTick);
	BOOL DoEnterScene(CRole* poRole, CScene* poScene);
	BOOL DoLeaveScene(CRole* poRole);
	BOOL DoQuerySceneDifficulty(CRole* poRole, UINT32 dwSceneTemplateID);
	BOOL DoSelectSceneDifficulty(CRole* poRole, INT32 nSceneDifficulty);
	BOOL DoSyncNextJumpPoint(CRole* poRole);

	BOOL DoSyncDoorState(CRole* poRole, CScene* poScene);

	BOOL DoSyncRolePos(CRole* poRole);
	BOOL DoCombatPoseSwitch(CRole* poRole, INT32 nFlag);

	BOOL DoCharWalk(CCharacter* poChar);
	BOOL DoCharRun(CCharacter* poChar);
	BOOL DoCharStop(CCharacter* poChar, BOOL bForce);
	BOOL DoCharPassiveMove(CCharacter* poChar, INT32 nDurationFrm2);
	BOOL DoObjectRotate(CSceneObject* poObj, INT32 nDestDirection, INT32 nDuration);
	BOOL DoCharFreeze(CCharacter* poChar, BOOL bFreeze);
	BOOL DoCharClientShowTime(CCharacter* poChar, BOOL bShowTime);
	BOOL DoCharFear(CCharacter* poChar, BOOL bFreeze);
	BOOL DoCharDie(CCharacter* poChar, INT32 nType);
	BOOL DoCharRevive(CCharacter* poChar, INT32 nType);
//	BOOL DoReviveResponse(CRole* poRole, INT32 nDelayTime);
	BOOL DoReviveInfo(CRole* poRole);
	BOOL DoActivateRevivePt(CRole* poRole, INT32 nX, INT32 nY, INT32 nZ);
	BOOL DoCharMine(CCharacter* poChar, INT32 nMineInterval);

	BOOL DoCharHard(CCharacter* poCharacter, INT32 nDuration);
	BOOL DoSyncFlagPos(CCharacter* poCharacter);

	BOOL DoPlayAction(CCharacter* pChar, UINT32 dwAnimationID);
	BOOL DoCharAppear(CCharacter* poCharacter, CRegion* poDestRegion, INT32 nAppearType);
	BOOL DoSyncNewCharList(CRole* poRole, CRegion* poRegion);
	BOOL DoObjectDisappear(CSceneObject* poSceneObject, CRegion* poDestRegion, INT32 nDisappearType);
	BOOL DoObjectDisappearList(CRole* poRole, CRegion* poDestRegion);
	BOOL DoSyncRoleData(CRole* poRole, CRole* poDataRole, UINT32 dwCRC);
	BOOL DoSyncNpcData(CRole* poRole, CNpc* poDataNpc, UINT32 dwCRC);
    BOOL BroadcastNpcData(CNpc* poDataNpc);
    BOOL DoSyncPetData(CRole* poRole, CNpc *poDataNpc, UINT32 dwCRC);
	BOOL DoSyncOpenDoodadCount(CDoodad* poDoodad, CCharacter* poChar);
	BOOL DoSyncRolePow(CRole* poRole, CRole* poTargetRole);


	BOOL DoSyncMissileList(CRole* poRole, CRegion* poRegion);
	BOOL DoMissileAppear(CMissile* poMissile, CRegion* poDestRegion);
	BOOL DoMissileMove(CMissile* poMissile);

	BOOL DoSyncPkValueChange(CRole* poRole);
    BOOL DoSyncPkResult(CRole* poKiller, CRole* poVictim, INT32 nChangePkValue, INT32 nResult);
	BOOL DoSyncGreyName(CRole* poRole, BOOL bGreyName);
	BOOL DoSyncEquipTitle(CRole *poRole);
	BOOL DoSyncComboKill(CRole* poRole);

    BOOL DoSyncMsgPaoPao(CRole *poRole, INT32 nType, UINT32 dwParam1);

    BOOL DoSyncPetLevel(CNpc* poNpc, UINT32 dwNpcID, UINT8 byLevel);
    BOOL DoSyncNpcTransformState(CNpc *poNpc, INT32 nState);


	// spell
	BOOL DoLearnSpell(CRole* poRole, INT32 nSpellID, INT32 nSpellLevel);
	BOOL DoRemoveSpell(CRole* poRole, INT32 nSpellID);

	BOOL DoLearnItemSpell(CRole* poRole, INT32 nSpellID, INT32 nSpellLevel, INT32 nItemSlot);
	BOOL DoRemoveItemSpell(CRole* poRole, INT32 nSpellID, INT32 nItemSlot);

	BOOL DoSyncSpellBook(CRole* poRole);
	BOOL DoSyncSpellSlot(CRole* poRole);
	BOOL DoSyncAuraList(CRole* poRole);
	BOOL DoClearSpellCache(CRole* poRole, INT32 nTag);

	BOOL DoCharPrepareSpell(CSceneObject* poFromObj, CSpellInstance* poSpellInstance, UINT32 dwTargetID, INT64 llTargetX, INT64 llTargetY, INT64 llTargetZ, INT32 nWeaponType);

	BOOL DoCharExCastSpell(CSceneObject* poFromObj, CSpellInstance* poSpellInstance, CCharacter* poCaster);
	BOOL DoCharCastSpell(CSceneObject* poFromObj, CSpellInstance* poSpellInstance, CCharacter* poCaster);
	BOOL DoCastOver(CRole* poRole, INT32 nSpellID, INT32 nSerialID);
//	BOOL DoCharSpellPreEffect(CCharacter* poTarget, CBullet* poBullet);
	BOOL DoCharSpellEffect(CCharacter* poTarget, CBullet* poBullet);
	BOOL DoCharAuraEffect(CCharacter* poTarget, CAuraInstance* poAura);
	BOOL DoEquipSpell(CRole* poRole, INT32 nSlotID, INT32 nSpellID, INT32 nRuneSlot, INT32 nSpellLevel);
	BOOL DoEquipRune(CRole* poRole, INT32 nSpellID, INT32 nRuneSlot, INT32 nSpellLevel);
	BOOL DoUnlockSpellSlot(CRole* poRole, INT32 nSlotID);
	BOOL DoLeechSoul(CRole* poRole, UINT32 dwFromObjID, INT32 nSoulPts);
	BOOL DoDirectlyHpChangeEffect(CCharacter* poChar, INT32 nHpChange);

	//BOOL DoCharChannelOver(CCharacter* poCharacter, INT32 nSpellID, INT32 nSpellLevel, INT32 nSerial);
	BOOL DoCharBreakCast(CCharacter* poCharacter, CSpellInstance* poSpellInstance, BOOL bIsBreak);
	BOOL DoCharSelectFocus(CCharacter* poCharacter, UINT32 dwFocusID);
	BOOL DoSyncFocusInfo(CRole* poRole, CCharacter* poFocusChar);
	BOOL DoSyncHp(CCharacter* poChar);
	BOOL DoSyncMp(CRole* poRole);
	BOOL DoAddCooldown(CRole* poRole, UINT32 dwCooldownID, INT32 nCooldownFrm);
	BOOL DoChangePkMode(CRole* poRole);
	BOOL DoEnterCombat(CCharacter* poChar);
	BOOL DoLeaveCombat(CCharacter* poChar);
	BOOL DoSyncFaction(CRole* poRole);

	BOOL DoSpellLevelUp(CRole* poRole, INT32 nSpellID, INT32 nSpellLevel);
	BOOL DoSyncSpellPts(CRole* poRole, INT32 nSpellID, INT32 nPts);

	BOOL DoAddToThreatList(CRole* poRole, UINT32 dwCharID);
	BOOL DoDelFromThreatList(CRole* poRole, UINT32 dwCharID);

	BOOL DoNewAura(CCharacter* poChar, CAuraInstance* poAura);
	BOOL DoDelAura(CCharacter* poRole, CAuraInstance* poAura);
	BOOL DoAuraChange(CCharacter* poRole, CAuraInstance* poAura);

	BOOL DoSyncGameDataBegin(CRole* poRole);
	BOOL DoSyncGameDataEnd(CRole* poRole);

	BOOL DoSyncAttrList(CRole* poRole);
	BOOL DoAttrChange(CRole* poRole, INT32 eAttrType);

	BOOL DoDrawDebugLine(CSceneObject* poChar, INT32 nType, INT32 nSrcX, INT32 nSrcY, INT32 nSrcZ, INT32 nDstX, INT32 nDstY, INT32 nDstZ);

	// item
    BOOL DoSyncRefineLevels(CRole* poRole);
	BOOL DoSyncMoney(CRole* poRole);
	BOOL DoSyncRealMoney(CRole* pRole);
	BOOL DoSyncContainerSizes(CRole* poRole);
	BOOL DoSyncItemBag(CRole* poRole);
	BOOL DoSendMoneyChange(CRole* poRole, INT32 moneytype, INT64 money, INT32 dailyearned, INT32 dailyused, INT32 weeklyearned, INT32 weeklyused);
	BOOL DoSendItemChange(CRole* poRole, Item* item);
    BOOL DoSendEquipDurChange(CRole* poRole, UINT8 conslot, UINT8 itemslot, INT32 dur);
	BOOL DoSendItemSwap(CRole* poRole, UINT8 srcconslot, UINT8 srcitemslot, UINT8 dstconslot, UINT8 dstitemslot);
	BOOL DoSendItemDelete(CRole* poRole, UINT8 conslot, UINT8 itemslot);
    BOOL DoSendDestroyDeathItem(CRole* poRole, Item* item);
    BOOL DoSendLosePkItem(CRole* poRole, Item* item);
	BOOL DoSendSoldItemList(CRole* poRole, const std::list<Item*>& items);
    BOOL DoSendRepairItem(CRole* poRole);
	BOOL DoSendContainerSizeChange(CRole* poRole, UINT8 conslot, UINT8 consize);
	BOOL DoSyncRoleAvatar(CRole* poRole, UINT8 conslot, UINT8 itemslot, INT32 nItemTemplateID, UINT8 byRefineFx);
	BOOL DoSyncHeroAvatar(CRole* poRole, UINT8 conslot, UINT8 itemslot, INT32 nItemTemplateID);
	BOOL DoSendTradeStateChange(CRole* poRole, UINT32 dwRoleID, UINT8 byState);
	BOOL DoSendTradeItem(CRole* poRole, UINT32 dwRoleID, UINT8 byTradeSlot, UINT8 bAdd, Item* item);
	BOOL DoSendTradeMoney(CRole* poRole, UINT32 dwRoleID, INT32 nMoney);
	BOOL DoSendUpgradeSoulResult(CRole* poRole, UINT8 byResult);
	BOOL DoSendAddSoulResult(CRole* poRole, UINT8 byResult);
	BOOL DoSendRemoveSoulResult(CRole* poRole, UINT8 byResult);
    BOOL DoSendRuneItem(CRole* poRole, UINT8 byResult);

	// Advanced inscription refinement and confirmation
	BOOL DoSendRuneItemAdvance(CRole* poRole, UINT8 byResult, UINT16 wRuneA, UINT16 wRuneB);
	BOOL DoSendRuneItemConfirm(CRole* poRole, UINT8 byResult);

    BOOL DoSendWeaponLevelUpPutIn(CRole* poRole);
    BOOL DoSendWeaponLevelUpTakeOut(CRole* poRole);
    BOOL DoSendWeaponLevelUpResult(CRole* poRole, UINT8 byResult);
    BOOL DoSendDecomposeResult(CRole* poRole, UINT8 byResult, INT32 original_itemid, INT32 itemid[], INT32 itemcount[]);
    BOOL DoSendComposeResult(CRole* poRole, UINT8 byResult, INT32 nItemID);
	BOOL DoSendRefineResult(CRole* poRole, UINT8 bySlot, UINT8 byResult, UINT16 wRefineLevel);
	BOOL DoSendRandomizeItem(CRole* poRole, Item* item);
    BOOL DoSendIdentifyResult(CRole* poRole, UINT8 byResult);
    BOOL DoSendLuckOilResult(CRole* poRole, INT8 result);

	BOOL DoGenerateDropFromObject(CDrop* poDrop, UINT32 dwSrcObjID);
	BOOL DoSyncDropList(CRole* poRole, CRegion* poRegion);
	BOOL DoPickUpItem(CRole* poRole, UINT32 pickerid, const Item* item);
	BOOL DoPickUpMoney(CRole* poRole, INT32 moneytype, INT32 count);
	BOOL DoSignItem(CRole* poRole, UINT8 byConSlot, UINT8 byItemSlot, UCHAR* ucSign, UINT32 dwSignSize);

    BOOL DoBlackMarketLimitInfo(CRole* poRole);
    BOOL DoBlackMarketCooldownInfo(CRole* poRole);
    BOOL DoBlackMarketBuy(CRole* poRole, INT32 nBlackMarketProductID, INT32 nCount);

    BOOL DoGamble(CRole* poRole, INT32 nResult, INT32 nItemID);
	BOOL DoSpellBookGamble(CRole* poRole, INT32 nResult, INT32 nItemID);

    BOOL DoExerciseInfo(CRole* poRole, INT32 nTotalHour, INT32 nCurHour);
    BOOL DoAddExerciseTime(CRole* poRole, INT32 nTotalHour);
    BOOL DoGetExerciseExp(CRole* poRole, INT32 nHour, INT32 nExp);

    BOOL DoSyncRoleAttrData(CRole* poRole, CRole* poTargetRole, UINT32 dwCRC);
	BOOL DoSyncRoleEquipData(CRole* poRole, CRole* poTargetRole, UINT32 dwCRC);
    BOOL DoSyncRoleSpiritData(CRole* poRole, CRole* poTargetRole, UINT32 dwCRC);
    BOOL DoSyncRoleSpellData(CRole* poRole, CRole* poTargetRole, UINT32 dwCRC);
	BOOL DoSyncCharAuraBrief(CRole* poRole, CCharacter* poTargetChar);

	// doodad
	BOOL DoSyncDoodadList(CRole* poRole, CRegion* poRegion);
	BOOL DoDoodadAppear(CDoodad* poDoodad);
	BOOL DoOperateDoodadBegin(CCharacter* poChar, UINT32 dwDoodadID, INT32 nDuration);
	BOOL DoOperateDoodadEnd(CCharacter* poChar, UINT32 dwDoodadID, BOOL bIsBreak);

	// stall
	BOOL DoBroadcastStallStateChange(CRole* poRole, UINT8 stallstate);
	BOOL DoBroadcastStallNameChange(CRole* poRole, const char* stallname);
    BOOL DoBroadcastStallLevelChange(CRole* poRole, UINT8 stalllevel);
	BOOL DoSendAddStallItem(CRole* poRole, UINT8 stallslot, UINT8 conslot, UINT8 itemslot, INT16 itemcount, INT32 price);
	BOOL DoSendRemvoeStallItem(CRole* poRole, UINT8 stallslot);
	BOOL DoSendStallItemList(CRole* poViewer, CRole* poOwner);
	BOOL DoSendBuyStallItem(CRole* poBuyer, UINT32 sellerid, UINT8 stallslot, INT32 leftcount);
	BOOL DoSendSoldStallItem(CRole* poSeller, UINT8 stallslot, INT32 itemcount, INT32 totalincome);
	BOOL DoSyncStall(CRole* poRole);

	// quest
	BOOL DoSyncQuestList(CRole* poRole);
	BOOL DoAcceptQuest(CRole* poRole, INT32 nQuestID);
	BOOL DoAbandonQuest(CRole* poRole, INT32 nQuestID);
	BOOL DoCompleteQuest(CRole* poRole, INT32 nQuestID);
	BOOL DoNotifyNextCircleQuest(CRole* poRole, INT32 nCircleID, INT32 nQuestID);
	BOOL DoDailyQuestCompleteTimeChange(CRole* poRole, INT32 nRemainTime);
	BOOL DoSyncDailyQuest(CRole* poRole, BOOL bClearCDTime=FALSE);
	BOOL DoQuestRecycle(CRole* poRole, INT32 nSlotIndex, INT32 nTemplateID);
	BOOL DoSetQuestFailed(CRole* poRole, INT32 nQuestID);

	BOOL DoOpenDialog(CRole* poRole, UINT32 dwNpcID, const char* pcszContent);

	BOOL DoSyncSceneVariables(CRole* poRole, CScene* poScene);
	BOOL DoSyncRoleVariables(CRole* poRole);
	BOOL DoSyncClientVariables(CRole* poRole);
	BOOL DoUpdateRoleVariable(CRole* poRole, INT32 nVarIndex);
	BOOL DoUpdateSceneVariable(CScene* poScene, INT32 nVarIndex);
	BOOL DoUpdateQuestVariable(CRole* poRole, INT32 nVarIndex, INT32 nValue, INT32 nQuestID);
	BOOL DoUpdateKillNpcCount(CRole* poRole, INT32 nVarIndex, INT32 nValue, INT32 nQuestID);
	BOOL DoUpdateKillPlayerCount(CRole* poRole, INT32 nVarIndex, INT32 nValue, INT32 nQuestID);
	BOOL DoUpdateKillDoodadCount(CRole* poRole, INT32 nVarIndex, INT32 nValue, INT32 nQuestID);
	BOOL DoShareQuest(CRole* poRole, INT32 nQuestID, INT32 nSrcID);
	BOOL DoUpdateSceneWeather(CScene* poScene);
	BOOL DoUpdateKillBufPlayerCount(CRole* poRole, INT16 nQuestID, INT32 nCount);
	BOOL DoUpdateKillItemNpcCount(CRole* poRole, INT32 nVarIndex, INT32 nValue, INT32 nQuestID);

	BOOL DoMSPublishQuest(CRole* poRole, INT32 nQuestID);
	BOOL DoPublishQuest(CRole* poRole, INT32 nQuestID);

	BOOL DoSyncKillNpcQuest(CRole* poRole);
	BOOL DoSyncKillNpcCount(CRole* poRole, INT32 nQuestID, INT32 nKillCount);
	BOOL DoCompleteKillNpcQuest(CRole* poRole, INT32 nQuestID, INT32 nIndex);
	BOOL DoResetKillNpcCount(CRole* poRole, INT32 nQuestID);

	// relation
	BOOL DoSyncReputation(CRole* poRole);
	BOOL DoModifyReputation(CRole* poRole, INT32 nForceID, INT32 nNewValue);

	// chat
    BOOL DoChatMessage(UINT32 dwSessionID, const char* pSrcName, UINT32 dwSrcID, UINT32 dwTargetID, UINT32 dwLocation, 
		const char* pcszMessage, INT32 nMessageSize);
	BOOL DoChatMessage(CScene*  pScene,  const char* pSrcName, UINT32 dwSrcID, UINT32 dwTargetID, UINT32 dwLocation, 
		const char* pcszMessage, INT32 nMessageSize);
	BOOL DoChatMessage(CRegion* pRegion, const char* pSrcName, UINT32 dwSrcID, UINT32 dwTargetID, UINT32 dwLocation, 
		const char* pcszMessage, INT32 nMessageSize);

	// hero
	BOOL DoSyncHeroData(CRole* poRole, CHero* poHero, UINT32 dwCRC);
	BOOL DoHeroExpChange(CRole* poRole, INT32 nExpChange, UINT32 dwMaxExp);
	BOOL DoHeroLevelUp(CRole* poRole);
	BOOL DoSyncHeroAttrList(CRole* poRole);
	BOOL DoHeroAttrChange(CRole* poRole, INT32 eAttrType);
	BOOL DoChangeHeroAi(CRole* poRole, INT32 nAiType);
	BOOL DoHireHero(CRole* poRole);
	BOOL DoFireHero(CRole* poRole);
	BOOL DoSummonHero(CRole* poRole);
	BOOL DoRecallHero(CRole* poRole);

	// misc
	BOOL DoErrorCodeNotify(CRole* poRole, INT32 nErrCode, INT32 nParam1, INT32 nParam2);
	BOOL DoRoleLevelUp(CRole* poRole);
	BOOL DoExpChange(CRole* poRole, INT32 nExpChange, INT32 nMaxExp, 
        INT32 nTodayExp, INT32 nTodayMaxExp, INT32 nExtraExp);
	BOOL DoExtraExpChange(CRole* poRole);
	//BOOL DoAblityPtsChange(CRole* poRole);
	BOOL DoMSUpdateRoleInfo(CRole* poRole);
	// team
	BOOL DoMSTeamSyncMember(CRole* poRole);
	BOOL DoClientTeamSyncMember(CRole* poRole);
	BOOL DoMSLeaveTeam(CRole* poRole);
	BOOL DoMSClearTeamDungeon(CRole* poRole);
	//BOOL DoClientSyncTeamInfo(CRole* poRole);
	BOOL DoClientSyncAdjacentRoleNoTeam(CRole* poRole);
	BOOL DoClientSyncAdjacentTeam(CRole* poRole);
	BOOL DoClientSyncRoleTeam(CRole* poRole);

	// team auction
	BOOL DoTeamAuctionAddItem(CTeamInGS* pTeam, TeamAuctionItem* poAuctionItem);
	BOOL DoTeamAuctionBegin(CTeamInGS* pTeam, TeamAuctionItem* poAuctionItem);
	BOOL DoTeamAuctionEnd(CTeamInGS* pTeam, TeamAuctionItem* poAuctionItem, UINT32 dwTax);
	BOOL DoTeamAuctionNotify(CTeamInGS* pTeam, TeamAuctionItem* poAuctionItem);
	BOOL DoTeamAuctionAbandon(CTeamInGS* pTeam, TeamAuctionItem* poAuctionItem, UINT32 dwBidderID);

	//battle team
	BOOL DoClientBattleTeamJoin(CRole* poRole, CTeamInGS* pstBattleTeam);
	BOOL DoClientBattleTeamAdd(CTeamInGS* pstBattleTeam, CRole* poNewMember);
	BOOL DoClientBattleTeamRemove(CTeamInGS* pstBattleTeam, UINT32 dwRemoveMemberID);
	BOOL DoClientBattleTeamLeaderChange(CTeamInGS* pstBattleTeam, UINT32 dwNewLeaderID);

	// friend
	BOOL DoSyncFriendGroupName(CRole* poRole);
	BOOL DoSaveRole(CRole* poRole, BOOL bSaveFile);

	//score
	BOOL DoSceneFinish(CScene* poScene);
    BOOL DoDungeonFinishData(CRole* poRole, INT32 nTotalTime, INT32 nTimeScore, INT32 nKillNpcCount, 
        INT32 nDestroyDoodadCount, INT32 nAttackScore, UINT32 dwGoalResult, INT32 nGoalScore, INT32 nTotalScore, 
        INT32 nNormalExp, INT32 nTeamExp, INT32 nGuildExp, INT32 nMasterExp, INT32 nVipExp, INT32 nFirstPassExp, INT32 nActivityExp, INT32 nTotalExp);
	BOOL DoSyncDungeonScore(CScene* poScene);
	BOOL DoDungeonReward(CRole* poRole, INT32 nType, UINT32 dwRewardID, UINT32 dwCount);
    BOOL DoSyncDungeonGoal(CRole* poRole);

//	INT32 OnCastSpell(CRole* poRole, CSpell* poSpell, UINT32 dwTargetID, INT32 nX, INT32 nY, INT32 nZ);

	//title
	BOOL DoSyncTitle(CRole* poRole);
	BOOL DoAddTitle(CRole* poRole, UINT8 nTitleID, INT32 nExpireTime);
	BOOL DoDelTitle(CRole* poRole, UINT8 nTitleID, UINT8 nDelType);

    // master
    BOOL DoSyncMaster(CRole *poRole);
	BOOL DoSyncMasterRewardExp(CRole *poRole);
	BOOL DoReceiveMasterRewardExpAck(CRole *poRole, BOOL bSuccess);

	// achievement
	BOOL DoSyncAchievementList(CRole* poRole);
	BOOL DoCompleteAchievement(CRole* poRole, INT32 nAchievementID, UINT32 dwCompleteTime);
	BOOL DoCompareAchievement(CRole* poRole, CRole* pTargetRole);	
	BOOL DoRewardAchievement(CRole* poRole, INT32 nAchievementID);
	BOOL DoUpdateAchievementVariable(CRole* poRole, INT32 nIndex, INT32 nValue);	

	BOOL DoActivateHankbook(CRole* poRole, INT32 nType, INT32 nIndex);

	// guild
    BOOL DoSyncGuild(CRole *poRole);
	BOOL DoSyncGuildName(CRole* poRole, UINT32 dwGuilID);
	BOOL DoGuildBuyAuraAck(CRole *poRole, INT32 nType, UINT8 byCount);
	BOOL DoSyncPurchaseAuraRecord(CRole *poRole);
    BOOL DoSyncGuildAchievement(CRole *poRole);
    BOOL DoGuildDrawSalaryAck(CRole *poRole, INT32 nSalary);
    BOOL DoGuildContributeAck(CRole *poRole, INT32 nMoney, INT32 nContribution);

    // arena
    BOOL DoClientArenaSyncData(CRole *poRole);
    BOOL DoClientArenaJoinSeasonAck(CRole *poRole, UINT8 bySeasonNum);
    BOOL DoClientArenaQuitSeasonAck(CRole *poRole, UINT8 bySeasonNum, UINT8 byPointVictoryTimes);
    BOOL DoClientArenaWin(CRole *poRole);
    BOOL DoClientArenaDefeat(CRole *poRole);


	// spirit
	BOOL DoSyncSpirit(CRole* poRole);
	BOOL DoSpiritExpChange(CRole* poRole, INT32 nExp);
	BOOL DoSpiritUp(CRole* poRole, INT32 nLevel);
	BOOL DoSpiritAirChange(CRole* poRole, INT32 nBagType, INT32 nIndex, INT32 nTemplateID, INT32 nExp);
	BOOL DoSpiritAirLevelUP(CRole* poRole, INT32 nLevel);
	BOOL DoSpiritBrushSlot(CRole* poRole, INT32 nPage, INT32 nIndex, INT32 nNumber);
	BOOL DoSpiritSlotSizeChange(CRole* poRole, INT32 nBagType, INT32 nSize);
	BOOL DoSpiritNotifyExpChangeResult(CRole* poRole, INT32 nResultCode, INT32 nRemainTime);
	BOOL DoSpiritBloodOpen(CRole* poRole, INT32 nPage);

	// anti-cheat
	BOOL DoAntiCheatJigsaw(CRole* poRole, JigsawImage* pImgData, JigsawAnswer* pAnswerData, UINT32 dwTimeoutInterval);
	
	// call ms lua function
	BOOL DoMSCallLuaFunction(const char* pcszScriptName, const char* pcszFuncName, INT32 nValue1, INT32 nValue2, INT32 nValue3);

	// mail
	BOOL DoMSSendSystemMail(UINT32 dwRoleID, const char* szMailSubject, const char* szMailContent);
	BOOL DoMSSendSystemMailWithSimpleAttach(UINT32 dwRoleID, const char* szMailSubject, const char* szMailContent, INT32 nItemID, INT32 nItemCount);
    BOOL DoMSSendSystemMailWithBufferAttach(UINT32 dwRoleID, const char* szMailSubject, const char* szMailContent, const char* pItemBuffer, UINT8 byBufferLen);

    // master
    BOOL DoMSMstSubmitTeacherAppAck(UINT8 bySourceType, UINT32 dwTeacherID, UINT32 dwStudentID);
    BOOL DoMSMstAgreeTeacherAppAck(UINT32 dwTeacherID, UINT32 dwStudentID);
    BOOL DoMSMstSubmitStudAppAck(UINT32 dwTeacherID, UINT32 dwStudentID);
    BOOL DoMSMstAgreeStudAppAck(UINT32 dwTeacherID, UINT32 dwStudentID);
    BOOL DoMSSyncMasterExp(UINT32 dwRoleID, INT64 llMasterExp);
    BOOL DoMSMasterCZ(UINT32 dwRoleID, UINT32 dwTargetID, INT32 nCZType, INT32 nCZLevel, const char *pcszText);
    BOOL DoMSTryToFindTeacher(UINT32 dwRoleID);
    BOOL DoMSMasterTwitter(UINT32 dwRoleID, BOOL bTwitter);

	// guild
	BOOL DoMSGuildCreate(UINT32 dwRoleID, const char *pcszGuildName, const char *pcszAnnounce);
	BOOL DoMSGuildDrawSalary(UINT32 dwRoleID);
	BOOL DoMSGuildContribute(UINT32 dwRoleID, INT32 nType, INT32 nMoney);

	BOOL DoMSGuildViewStorage(UINT32 dwRoleID, UINT8 byStorageTag);
	BOOL DoMSGuildPrePushItem(UINT32 dwRoleID, UINT16 wSourceConSlot, UINT16 wSourceSlot, UINT8 byTargetConSlot, UINT8 byTargetSlot);
	BOOL DoMSGuildPushItem(INT32 nResult, UINT32 dwRoleID, UINT8 byConSlot, UINT8 bySlot, ItemInfo &iteminfo);
	BOOL DoMSGuildPrePopItem(UINT32 dwRoleID, UINT8 byConSlot, UINT8 bySlot, UINT16 wTargetConSlot, UINT16 wTargetSlot);
	BOOL DoMSGuildPopItem(INT32 nResult, UINT32 dwRoleID, UINT8 byConSlot, UINT8 bySlot, UINT16 wConSlot, UINT16 wSlot);
    BOOL DoMSGuildCreateBuilding(UINT32 dwRoleID, UINT64 qwNpcNameID, INT32 nType);
    BOOL DoMSGuildUpgradeBuilding(UINT32 dwRoleID, UINT64 qwNpcNameID, INT32 nType);
    BOOL DoMSGuildDelBuilding(UINT32 dwRoleID, UINT64 qwNameID, INT32 nType);
    BOOL DoMSGuildPreOpenDungeon(UINT32 dwGuildID, UINT32 dwTemplateID, UINT32 dwOwnerID, UINT32 dwLeaderID, INT64 llMoney);
    BOOL DoMSGuildOpenDungeon(UINT32 dwOwnerID, UINT32 dwLeaderID, UINT32 dwTemplateID);
    BOOL DoMSGuildEnterDungeon(UINT32 dwRoleID,  UINT32 dwTemplateID);
    BOOL DoMSGuildCloseDungeonCommission(UINT32 dwTemplateID, UINT32 dwSceneID, UINT32 arrItemID[], INT32 arrItemCount[], INT32 nItemIDNum);

    BOOL DoMSNpcChangeGuildRes(UINT32 dwNpcID, UINT32 dwGuildID, INT32 nChangeType, INT32 nResType, INT32 nCount, INT32 nReason, INT32 nCtxType);
    BOOL DoMSGuildEndWar(UINT32 dwDefenderID, INT32 nWarResult);
    BOOL DoMSGuildChangeBuildingState(UINT32 dwGuildID, UINT64 qwNameID, UINT32 dwReviveTime);
    BOOL DoMSGuildAchievementReward(UINT32 dwRoleID, INT32 nID);

	BOOL DoMSGuildLeaveGuild(UINT32 dwRoleID);

    // arena
    BOOL DoMSArenaSyncData(UINT32 dwRoleID, UINT8 byPointSeasonTimes, INT32 nPointSeasonEndTime);
    BOOL DoMSArenaJoinSeason(UINT32 dwRoleID);
    BOOL DoMSArenaMatch(UINT32 dwRoleID);
    BOOL DoMSArenaWin(UINT32 dwRoleID);
    BOOL DoMSArenaDefeated(UINT32 dwRoleID, UINT8 byPointSeasonTimes, INT32 nPointSeasonEndTime);
    BOOL DoMSArenaReward(UINT32 dwRoleID);

	BOOL DoSyncTeleportPlatform(CRole* poRole);
	BOOL DoActiveTeleportPlatform(CRole* poRole, UINT8 byTeleportPlatformID);
	BOOL DoSyncCombatStat(CRole* poRole);

	// mount
	BOOL DoSyncMountList(CRole* poRole);
	BOOL DoActivateMount(CRole* poRole, INT32 nMountID);
	BOOL DoCharOnMount(CCharacter* poChar, INT32 nMountID);
	BOOL DoCharOffMount(CCharacter* poChar);
	BOOL DoCharWing(CCharacter* poChar, INT32 nWingID);
	BOOL DoWingInfo(CRole* poRole, BOOL bUpgraded);

	BOOL DoShieldAck(CRole *poRole, UINT32 dwTargetRoleID, char *pTargetRoleName);
	BOOL DoUnShieldAck(CRole *poRole, UINT32 dwTargetRoleID);
	BOOL DoSyncShieldList(CRole *poRole);

    BOOL DoMSAddBeKilledToList(CRole *poRole, UINT32 dwKillerID);

	// scene
	BOOL DoMSCloseScene(UINT8 byServerGroup, INT32 nSceneTempID, INT32 nSceneID);

	// Logout
	BOOL DoReturnSelectRole(CRole* poRole);
	BOOL DoLogout(CRole* poRole);
	BOOL DoSaveAndUnlockRole(CRole* poRole);

	BOOL DoClientLogoutTimer(CRole* poRole, UINT8 byStart, UINT8 byDelayTime);

    // vip
    BOOL DoSyncVIPState(CRole* poRole);
    BOOL DoSyncVIPLevel(CRole* poRole);

    //battle
    void OnEnterBattle(CRole* poRole);

	// activity
	BOOL DoActivityChange(CRole* poRole);
	BOOL DoActivityReward(CRole* poRole, INT32 nIndex);

	// kill npc
    BOOL DoUpdateConsecutiveKillNpcCount(CRole* poRole);
	BOOL DoUpdateConsecutiveKillNpcEnd(CRole* poRole, INT32 nExp);
	BOOL DoUpdateMomentaryKillNpcEnd(CRole* poRole, INT32 nCount, INT32 nExp);

	BOOL DoUpdateConsecutiveKillPlayerCount(CRole* poRole);

	// survey
	BOOL DoQuerySurveyPath(CRole* poRole);

	/************************************************************************/
	// gs 2 dl

	BOOL DoDLRoleLogout(CRole* poRole, BOOL bInJump);

	// chat
	BOOL DoDLChatMessage(CRole* poRole, UINT32 dwTargetID, const char* pcszMessage, INT32 nMessageSize);

	BOOL DoDLCastSpell(CRole* poRole, INT32 nSpellID, INT32 nSpellLevel, INT32 nRuneSlot, UINT32 dwTargetID, 
		INT32 nTargetX, INT32 nTargetY, INT32 nTargetZ, INT32 nMp, INT32 nCdRemain, INT32 nErrCode, 
		UINT32 dwClientTime, BOOL bFromServer, BOOL bShiftPressed);
	BOOL DoDLSpellEffect(CRole* poRole, INT32 nSpellID, INT32 nSpellLevel, INT32 nRuneSlot, UINT32 dwTargetID, 
		INT32 nSpellSerial, INT32 nParam1, INT32 nParam2, INT32 nFlag);
	BOOL DoDLLearnSpell(CRole* poRole, INT32 nSpellID, INT32 nSpellLevel);

	// levelup
	BOOL DoDLLevelUp(CRole* poRole, UINT32 dwRoleID, INT32 nLevel);

	// die
	BOOL DoDLDie(CRole* poRole, INT32 nSceneTemplateID, INT32 nSceneID, UINT32 dwKillerID, const char* pKillName, INT32 nKillerLevel, INT32 nKillClass, INT64 llX, INT64 llY, INT64 llZ);

	// trade
	BOOL DoDLBuyConSlot(CRole* poRole, INT32 nConType, INT32 nConSlotSize);

    BOOL DoDLLogItem(UINT32 dwRoleID, INT32 nGroupID, INT64 Guid, 
		INT32 nTemplateID, INT32 nItemCount, INT32 nReason, INT32 nAction, UINT64 qwUserDefinedID);
    BOOL DoDLStatItem(INT32 nGroupID, INT32 nTemplateID, INT32 nItemCount, INT32 nAction);
    BOOL DoDLRuneItem(UINT32 dwRoleID, INT32 nGroupID, INT32 nRoleLevel, INT32 nEquipTemplateID, INT32 nRuneItemID, INT32 nRuneID);
    BOOL DoDLChangeItem(INT32 nGroupID, INT32 dwRoleID, INT32 nReason, const ItemInfo& item);

	// money
	BOOL DoDLAddMoney(UINT32 dwSessionID, UINT32 dwSrcID, UINT32 dwTarID, INT32 nMoneyType, INT32 nMoneyCount, INT32 nReason, INT32 nParam);
    BOOL DoDLSubMoney(UINT32 dwSessionID, UINT32 dwSrcID, UINT32 dwTarID, INT32 nMoneyType, INT32 nMoneyCount, INT32 nReason, INT32 nParam);

	// quest
	BOOL DoDLAcceptQuest(CRole* poRole, INT32 nQuestID);
	BOOL DoDLCompleteQuest(CRole* poRole, INT32 nQuestID);
	BOOL DoDLAbandonQuest(CRole* poRole, INT32 nQuestID);
	BOOL DoDLCompleteAchievement(CRole* poRole, INT32 nAchievementID);

	// gm
	BOOL DoDLGmCommand(CRole* poRole, const char* szCommand, BOOL nResult);

	// position
	BOOL DoDLRolePosition(CRole* poRole);

    BOOL DoDLKickRole(INT32 eKickType, const char* pcszUserName, UINT32 dwClientSessionID, const char* pcszRoleName, UINT32 dwRoleID, 
        const char* pcszFileName, INT32 nLineNum, const char* pcszFuncName);

	// server monitor
	BOOL DoDLNpcDie(INT32 nServerGroupID, UINT32 dwNpcID, BOOL bDieIDMonitor);
    BOOL DoDLNpcMoney(INT32 nServerGroupID, UINT32 dwNpcID, BOOL bMoneyStat, INT32 nMoneyCount);
    BOOL DoDLConsecutiveKillNpcCount(UINT32 dwSessionID, INT32 nConsecutiveKillNpcCount);
	BOOL DoDLMomentaryKillNpcCount(UINT32 dwSessionID, INT32 nMomentaryKillNpcCount);

	// hardware info
	BOOL DoDLHardwareInfo(UINT32 dwSessionID, UINT32 dwRoleID, void* pData);
	BOOL DoDLFpsInfo(UINT32 dwSessionID, UINT32 dwRoleID, UINT8 byGameFps, UINT8 byCpuFps, UINT8 byGpuFps, UINT16 wGateDelay, UINT16 wGameDelay);

	// anti-cheat
	BOOL DoDLAntiCheatInfo(CRole* poRole, INT32 nAction, INT32 nAnswerIdx, INT32 nErrCode, JigsawImage* poImg, JigsawAnswer* poAnswer);

	// sabac
	BOOL DoSyncSabacEnrollList(CRole* poRole);
	BOOL DoOccupySabac(INT32 nServerGroup, UINT32 dwOldGuildID, UINT32 dwNewGuildID);
	BOOL DoSabacStateChange(INT32 nServerGroup, BOOL bStart);
	BOOL DoMSEnrollSabac(UINT32 dwGuildID, UINT32 dwEnrollTime);
	BOOL DoMSOccupySabac(INT32 nServerGroup, UINT32 dwOldGuildID, UINT32 dwNewGuildID);
	BOOL DoMSErectStatue(CRole* poRole);

	// checkin
	BOOL DoSyncCheckInInfo(CRole* poRole);
	BOOL DoCheckIn(CRole* poRole, INT32 nIndex);

private:
	/************************************************************************/
	// client 2 gameserver
	// login
	void OnLoadFinish(CRole* poRole, const char* pData);
	void OnReturnRoleSelect(CRole* poRole, const char* pData);
	void OnGpkAuthData(CRole* poRole, const char* pData);

	// move & pos sync
	void OnRoleStop(CRole* poRole, const char* pData);
	void OnRoleRotate(CRole* poRole, const char* pData);
	void OnRoleWalk(CRole* poRole, const char* pData);
	void OnRoleRun(CRole* poRole, const char* pData);
	void OnUpdatePos(CRole* poRole, const char* pData);
	void OnPing(CRole* poRole, const char* pData);
	void OnQueryCharData(CRole* poRole, const char* pData);
	void OnQueryRoleDetailedData(CRole* poRole, const char* pData);
	void OnQueryRolePow(CRole* poRole, const char* pData);

	void OnTeleport(CRole* poRole, const char* pData);
	void OnSelectSceneDifficulty(CRole* poRole, const char* pData);
	void OnEnterDungeon(CRole* poRole, const char* pData);
	void OnChangeLine(CRole* poRole, const char* pData);
	void OnLeaveScene(CRole* poRole, const char* pData);
	void OnRevive(CRole* poRole, const char* pData);
	void OnCombatPoseSwitch(CRole* poRole, const char* pData);
	void OnCancelCutScene(CRole* poRole, const char* pData);
	void OnRoleTeleport(CRole* poRole, const char* pData);
	void OnQueryRoleLocation(CRole* poRole, const char* pData);
	void OnRecallTeamMember(CRole* poRole, const char* pData);
	void OnRoleMine(CRole* poRole, const char* pData);

	// spell
	void OnCastSpell(CRole* poRole, const char* pData);
	void OnCastSpellEx(CRole* poRole, const char* pData);
	void OnDirectlyCastSpell(CRole* poRole, const char* pData);
	void OnCastSwitchSpell(CRole* poRole, const char* pData);
	void OnCancelCast(CRole* poRole, const char* pData);
	void OnSelectFocus(CRole* poRole, const char* pData);
	void OnAdjustTargetPt(CRole* poRole, const char* pData);
	void OnChangePkMode(CRole* poRole, const char* pData);
	void OnEquipSpell(CRole* poRole, const char* pData);
	void OnCancelAura(CRole* poRole, const char* pData);
	void OnSpellLog(CRole* poRole, const char* pData);
	void OnClearSpellCache(CRole* poRole, const char* pData);

	// item
	void OnUseItem(CRole* poRole, const char* pData);
	void OnMoveItem(CRole* poRole, const char* pData);
	void OnSplitItem(CRole* poRole, const char* pData);
	void OnDeleteItem(CRole* poRole, const char* pData);
	void OnPickUpItem(CRole* poRole, const char* pData);
	void OnBuyItem(CRole* poRole, const char* pData);
	void OnSellItem(CRole* poRole, const char* pData);
	void OnViewSoldItems(CRole* poRole, const char* pData);
	void OnBuyBackItem(CRole* poRole, const char* pData);
	void OnRepairItem(CRole* poRole, const char* pData);
    void OnRepairAllEquippedItem(CRole* poRole, const char* pData);
    void OnSpecialRepairItem(CRole* poRole, const char* pData);
	void OnRefine(CRole* poRole, const char* pData);
	void OnBuyContainerSlot(CRole* poRole, const char* pData);
	void OnOrganizeBag(CRole* poRole, const char* pData);
	void OnIdentifyItem(CRole* poRole, const char* pData);
	void OnRandomizeItem(CRole* poRole, const char* pData);
	void OnConfirmRandomizeItem(CRole* poRole, const char* pData);
	void OnSignItem(CRole* poRole, const char* pData);
    void OnUseMoneytype01Only(CRole* poRole, const char* pData);
    void OnCheckNpcShopData(CRole* poRole, const char* pData);
    void OnBlackMarketLimitInfo(CRole* poRole, const char* pData);
    void OnBlackMarketCooldownInfo(CRole* poRole, const char* pData);
    void OnBlackMarketBuy(CRole* poRole, const char* pData);

	// Reforge c2g_potion_gamble
    void OnGamble(CRole* poRole, const char* pData);
    void OnPotionGamble(CRole* poRole, const char* pData);
    void OnGambleAdvance(CRole* poRole, const char* pData);

    void OnExerciseInfo(CRole* poRole, const char* pData);
    void OnAddExerciseTime(CRole* poRole, const char* pData);
    void OnGetExerciseExp(CRole* poRole, const char* pData);

	// title
	void OnEquipTitle(CRole* poRole, const char* pData);
	void OnUnEquipTitle(CRole* poRole, const char* pData);

	// stall
	void OnOpenStall(CRole* poRole, const char* pData);
	void OnCloseStall(CRole* poRole, const char* pData);
	void OnRealOpenStall(CRole* poRole, const char* pData);
	void OnRealCloseStall(CRole* poRole, const char* pData);
	void OnAddStallItem(CRole* poRole, const char* pData);
	void OnRemoveStallItem(CRole* poRole, const char* pData);
	void OnChangeStallName(CRole* poRole, const char* pData);
    void OnUpgradeStall(CRole* poRole, const char* pData);
	void OnViewStallItem(CRole* poRole, const char* pData);
	void OnBuyStallItem(CRole* poRole, const char* pData);

	// doodad
	void OnOperateDoodad(CRole* poRole, const char* pData);

	// quest
	void OnAcceptQuest(CRole* poRole, const char* pData);
	void OnPreCompleteQuest(CRole* poRole, const char* pData);
	void OnCompleteQuest(CRole* poRole, const char* pData);
	void OnAbandonQuest(CRole* poRole, const char* pData);
	void OnInteractWithNpc(CRole* poRole, const char* pData);
    void OnStopInteractWithNpc(CRole* poRole, const char* pData);
	void OnSelectDialogOption(CRole* poRole, const char* pData);
	void OnShareQuest(CRole* poRole, const char* pData);
	void OnConfirmPublishQuest(CRole* poRole, const char* pData);	
	void OnTalkToNpc(CRole* poRole, const char* pData);
	void OnExploreZone(CRole* poRole, const char* pData);
	void OnFlyToQuestZone(CRole* poRole, const char* pData);
	void OnBuyDailyQuestCompleteTime(CRole* poRole, const char* pData);
	void OnDailyQuestCompleteShotCut(CRole* poRole, const char* pData);	
	void OnBrushDailyQuest(CRole* poRole, const char* pData);	
	void OnGenerateCircleQuest(CRole* poRole, const char* pData);	
	void OnQuestRecycle(CRole* poRole, const char* pData);	
	void OnCompleteKillNpcQuest(CRole* poRole, const char* pData);

	// chat
	void OnChatMessage(CRole* poRole, const char* pData);

	// friend
	void OnFriendChangeGroupName(CRole* poRole, const char* pData);

	// misc
	//void OnGMCommand(CRole* poRole, const char* pData);
	void OnQueryCombatStat(CRole* poRole, const char* pData);

	//team
	void OnAdjacentRoleNoTeam(CRole* poRole, const char* pData);
	void OnAdjacentTeam(CRole* poRole, const char* pData);    

	// team auction
	void OnTeamAuctionBid(CRole* poRole, const char* pData);
	void OnTeamAuctionAbandon(CRole* poRole, const char* pData);    

	//score
	void OnDungeonSelectReward(CRole* poRole, const char* pData);
	void OnDungeonReplay(CRole* poRole, const char* pData);

	// trade
	void OnTradeAsk(CRole* poRole, const char* pData);
	void OnTradeAccept(CRole* poRole, const char* pData);
	void OnTradeSetItem(CRole* poRole, const char* pData);
	void OnTradeSetMoney(CRole* poRole, const char* pData);
	void OnTradeLock(CRole* poRole, const char* pData);
	void OnTradeConfirm(CRole* poRole, const char* pData);
	void OnTradeUnlock(CRole* poRole, const char* pData);
	void OnTradeCancel(CRole* poRole, const char* pData);

	// achievement
	void OnCompleteAchievement(CRole* poRole, const char* pData);
	void OnCompareAchievement(CRole* poRole, const char* pData);
	void OnRewardAchievement(CRole* poRole, const char* pData);

	// arena_team
	//void OnCreateArenaTeam(CRole* poRole, const char* pData);

    // client variable
	void OnUpdateClientVaraible(CRole* poRole, const char* pData);

    // master
    void OnMasterTwitter(CRole *poRole, const char *pData);
	void OnReceiveMasterRewardExp(CRole *poRole, const char *pData);
	void OnQueryMasterRewardExp(CRole *poRole, const char *pData);

	// guild
	void OnGuildCreate(CRole *poRole, const char *pData);
	void OnGuildDrawSalary(CRole *poRole, const char *pData);
	void OnGuildContribute(CRole *poRole, const char *pData);
	void OnGuildViewStorage(CRole *poRole, const char *pData);
	void OnGuildPushItem(CRole *poRole, const char *pData);
	void OnGuildPopItem(CRole *poRole, const char *pData);
    void OnGuildCreateBuilding(CRole *poRole, const char *pData);
    void OnGuildUpgradeBuilding(CRole *poRole, const char *pData);
    void OnGuildDelBuilding(CRole *poRole, const char *pData);
	void OnGuildBuyAura(CRole *poRole, const char *pData);
    void OnGuildChangeWeather(CRole *poRole, const char *pData);
    void OnGuildEnterMap(CRole *poRole, const char *pData);
    void OnGuildOpenDungeon(CRole *poRole, const char *pData);
    void OnGuildEnterDungeon(CRole *poRole, const char *pData);
    void OnGuildRewardAchievement(CRole *poRole, const char *pData);

	void OnLeavelGuild(CRole* poRole, const char* pData);
	void OnQueryGuildWarRecord(CRole* poRole, const char* pData);

	void OnTeleportByPlatform(CRole* poRole, const char *pData);

    // arena
    void OnArenaJoinSeason(CRole* poRole, const char *pData);
    void OnArenaQuitSeason(CRole* poRole, const char *pData);
    void OnArenaMatch(CRole* poRole, const char *pData);
    void OnArenaGetReward(CRole* poRole, const char *pData);

	// mount
	void OnOnMount(CRole* poRole, const char* pData);
	void OnWingOp(CRole* poRole, const char* pData);
	void OnOffMount(CRole* poRole, const char* pData);

	// hero
	void OnHireHero(CRole* poRole, const char* pData);
	void OnFireHero(CRole* poRole, const char* pData);
	void OnQueryHeroAttrList(CRole* poRole, const char* pData);
	void OnSummonHero(CRole* poRole, const char* pData);
	void OnRecallHero(CRole* poRole, const char* pData);
	void OnChangeHeroAi(CRole* poRole, const char* pData);
	void OnChangeHeroName(CRole* poRole, const char* pData);

	// spirit
	void OnSpiritExpChange(CRole *poRole, const char *pData);

	void OnSpiritAddAir(CRole *poRole, const char *pData);
	void OnSpiritMergeAir(CRole *poRole, const char *pData);
	void OnSpiritMoveAir(CRole *poRole, const char *pData);	
	void OnSpiritExchangeAir(CRole *poRole, const char *pData);
	void OnSpiritBuySlot(CRole *poRole, const char *pData);

	void OnSpiritBrushSlot(CRole *poRole, const char *pData);
	void OnSpiritBloodOpen(CRole *poRole, const char *pData);

	// shield list
	void OnUnShieldRole(CRole *poRole, const char *pData);

	// Soul fusion
	void OnUpgradeSoul(CRole *poRole, const char *pData);
	void OnAddSoul(CRole *poRole, const char *pData);
	void OnRemoveSoul(CRole *poRole, const char *pData);

    // Soul forging
    void OnRuneItem(CRole *poRole, const char *pData);

    // Equipment upgrade
    void OnWeaponLevelUpPutIn(CRole *poRole, const char *pData);
    void OnWeaponLevelUpTakeOut(CRole *poRole, const char *pData);
	void OnWeaponLevelUpTakeOutImmediately(CRole* poRole, const char* pData);

    // Decomposition and crafting
    void OnDecompose(CRole *poRole, const char *pData);
    void OnCompose(CRole *poRole, const char *pData);

	// activity
	void OnActivityReward(CRole *poRole, const char *pData);

	//logout
	void OnLoginOut(CRole* poRole, const char* pData);
	void OnLoginOutCancel(CRole* poRole, const char* pData);

    // vip
    void OnSubscribeVIP(CRole* poRole, const char* pData);

	// client
	void OnHardwareInfo(CRole* poRole, const char* pData);
	void OnClientFpsInfo(CRole* poRole, const char* pData);

	// survey
	void OnQuerySurveyPath(CRole* poRole, const char* pData);

	// reputation award
	void OnReceiveReputationAward(CRole* poRole, const char* pData);

    // pet
    void OnPetChangeState(CRole *poRole, const char *pData);

	// checkin
	void OnCheckIn(CRole* poRole, const char *pData);

	// anti-cheat
	void OnJigsawAnswer(CRole* poRole, const char *pData);

	// Spellbook reforging
	void OnSpellBookGamble(CRole* poRole, const char* pData);

	// c2g_arena_reward
	void OnArenaReward(CRole* poRole, const char* pData);

	// Advanced inscription refinement and confirmation
	void OnRuneItemAdvance(CRole* poRole, const char* pData);
	void OnRuneItemConfirm(CRole* poRole, const char* pData);

	// c2g_sell_resource_item
	void OnSellResourceItem(CRole* poRole, const char* pData);

	// c2g_dice_bet, c2g_lovers_teleprot
	void OnDiceBet(CRole* poRole, const char* pData);
	void OnLoversTeleprot(CRole* poRole, const char* pData);



private:
	/************************************************************************/
	BOOL _BroadcastRegion(CRegion* poRegion, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize);
	BOOL _Broadcast9Region(CRegion* poRegion, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize);
	BOOL _BroadcastScene(CScene* poScene, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize);
	BOOL _BroadcastTeam(CTeamInGS* poTeam, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize);
	BOOL _BroadcastTeamToSameSceneMember(CRole* poRole, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize);
	BOOL _BroadcastTeamToInSceneMember(CTeamInGS* poTeam, CScene* poScene, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize);
	BOOL _BroadcaseGroupClient(INT32 nServerGroup, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize);

	INT32 _OnCastSpell(CRole* poRole, CSpell* poSpell, UINT32 dwTimeStamp, UINT32 dwTargetID, INT32 nX, INT32 nY, INT32 nZ);

	//UINT32 WriteItem(char* buf, const ItemInfo& iteminfo, BOOL bEquip);

private:
	struct CTraverseRole
	{
		BOOL operator()(CRole* poRole);
	}					m_TraverseRole;

	struct CTraverseRegion
	{
		CCliMessageHandler*     poHandler;
		BOOL operator()(CRegion* poRegion);
	}				    m_TraverseRegion;

	struct CTraverseDisappearObj
	{
		ExternalMessage::G2C_OBJECT_DISAPPEAR_LIST*	pMsg;
		INT32						nObjCount;
		BOOL operator()(CSceneObject* poSceneObject);
	};

private:
	IMessageNotifier*	m_poNotifier;
	IPacketAllocator*	m_poPacketAllocator;

	typedef void (CCliMessageHandler::*MESSAGE_FUNC)(CRole* poRole, const char* pData);
	struct MESSAGE_INFO
	{
		MESSAGE_FUNC        pFunc;
		BOOL                bPlayingMsg;
		BOOL				bPrint;
	};

	MESSAGE_INFO        m_MessageInfo[ExternalMessage::c2g_message_end];
};

inline void CCliMessageHandler::SetMsgPrintState(INT32 nMsgID, BOOL nState)
{
	LOG_PROCESS_ERROR(nMsgID >= ExternalMessage::c2g_message_begin && nMsgID < ExternalMessage::c2g_message_end);

	m_MessageInfo[nMsgID].bPrint = nState;

Exit0:
	return;
}

inline INT32 CCliMessageHandler::GetMsgPrintState(INT32 nMsgID)
{
	LOG_PROCESS_ERROR(nMsgID >= ExternalMessage::c2g_message_begin && nMsgID < ExternalMessage::c2g_message_end);

	return m_MessageInfo[nMsgID].bPrint;
Exit0:
	return 0;
}


#endif  // _ROBOT

#endif	// _MESSAGE_HANDLER_H_
