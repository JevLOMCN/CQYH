#ifndef _LS_MESSAGE_HANDLER_H_
#define _LS_MESSAGE_HANDLER_H_

#include "MMOUtil.h"
#include "common.h"

#include "../../Common/message/c2g_message.h"
#include "../../Common/message/g2c_message.h"
#include "../../Common/message/c2m_message.h"
#include "../../Common/message/m2c_message.h"
#include "../../Common/message/c2t_message.h"
#include "../../Common/message/t2c_message.h"
#include "../../Common/message/b2c_message.h"
#include "net.h"

#define M_ENABLE_ASYNC_PROC_CHAR 0
#define ACTIVATION_LEN 32

// oscar@snda 是否开启网络协议缓存处理
#define M_ENABLE_NET_MSG_BUFF 0



using namespace ExternalMessage;

namespace SimpleNet
{
    class CNet;
}

class CPacketAllocator;

#define M_DECL_PARSE_FUN(msgID) void Parse_##msgID(const char* buf, UINT32 size);

#if !SHIPPING_PC_GAME
#define MREG_PARSE_FUN(msg)                                         \
do                                                                  \
{                                                                   \
	m_MessageDebugString[msg] = FString(TEXT(#msg));				\
	m_MessageFuncList[msg] = &CMessageHandler::Parse_##msg;         \
} while (0);
#else
#define MREG_PARSE_FUN(msg)                                         \
do                                                                  \
{                                                                   \
	m_MessageFuncList[msg] = &CMessageHandler::Parse_##msg;         \
} while (0);
#endif

size_t strxcpy(char *dst, const TCHAR *src, size_t maxSize);
size_t strxcpy(char *dst, const char *src, size_t size);
size_t strxcat(char *dst, const char *src, size_t size);
size_t CopyUTF8(char* dst,const TCHAR* src, size_t maxSize);



template<typename T>
class CharCacheData
{
	std::vector<INT> IdList;
	std::vector<T*> DataList;
	std::map<INT, INT> SearchMap;
	INT CurrentIdx;

public:

	CharCacheData()
	{
		CurrentIdx = -1;
	}

	~CharCacheData()
	{
		if (DataList.size() > 0)
		{
			Clear();
		}
	}

	void InitSize(INT InMaxSize)
	{
		if (DataList.size() > 0)
		{
			return;
		}

		for (INT i = 0; i < InMaxSize; ++i)
		{
			DataList.push_back(NULL);
			IdList.push_back(0);
		}
	}

	T* GetCacheData(INT InId)
	{
		std::map<INT, INT>::iterator pos = SearchMap.find(InId);
		if (pos == SearchMap.end())
		{
			return NULL;
		}
		else
		{
			INT idx = pos->second;
			check(idx >= 0 && idx < (INT)DataList.size());
			return DataList[idx];
		}
	}

	void AddNewData(INT InId, T* InData)
	{
		if (DataList.size() == 0)
		{
			return;
		}

		std::map<INT, INT>::iterator pos = SearchMap.find(InId);
		if (pos != SearchMap.end())
		{
			INT idx = pos->second;
			DataList[idx] = InData;
			return;
		}


		CurrentIdx++;
		if (CurrentIdx == DataList.size())
		{
			CurrentIdx = 0;
		}

		SearchMap.erase(IdList[CurrentIdx]);
		if (DataList[CurrentIdx] != NULL)
		{
			delete DataList[CurrentIdx];
			DataList[CurrentIdx] = NULL;
			IdList[CurrentIdx] = 0;
		}

		DataList[CurrentIdx] = InData;
		IdList[CurrentIdx] = InId;
		SearchMap.insert(make_pair(InId, CurrentIdx));
	}

	void Clear(INT InId)
	{
		std::map<INT, INT>::iterator pos = SearchMap.find(InId);
		if (pos == SearchMap.end())
		{
			return;
		}

		check(pos->second >= 0 && pos->second < (INT)DataList.size());

		if (DataList[pos->second] != NULL)
		{
			delete DataList[pos->second];
			DataList[pos->second] = NULL;
		}

		IdList[pos->second] = 0;
		SearchMap.erase(InId);
	}

	void Clear()
	{
		SearchMap.clear();
		for (INT i = 0; i < (INT)DataList.size(); ++i)
		{
			if (DataList[i] != NULL)
			{
				delete DataList[i];
			}
		}

		DataList.clear();
		IdList.clear();
	}
};

class CMessageHandler : public INetEventHandler
{
public:
    virtual void OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession);
    virtual void OnConnected(ISocketSession* pSocketSession);
    virtual void OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize);
    virtual void OnDisconnected(ISocketSession* pSocketSession, BOOL bRemoteShutdown);

	void DebugParseMsg(const char* buf, UINT32 dwSize);

	void CloseConnection(bool byUser);
	UBOOL IsConnection();
	void Logout();

public:
    CMessageHandler();
    ~CMessageHandler();

    BOOL Init(void);
    BOOL UnInit(void);
    static CMessageHandler& GetInstance(void);
    void Tick(float fTime);

    const TCHAR* GetSelfUserID() const;
    AMMOActor* GetMMOActor(INT objectId);

#if M_ENABLE_ASYNC_PROC_CHAR

	struct FMsgBuf
	{
		UINT32 Size;
		MESSAGE_HEADER* MsgBuf;
	};

	typedef std::deque<FMsgBuf> CharAppearList;
	CharAppearList _charAppearList;

	void ClearAllCharAppearList() { _charAppearList.clear(); }
	void TickCharAppear(FLOAT deltaTime);
#endif

public:
    void Send_GatePing();
	void Send_GamePing();

    void Send_LoadFinish();

    void SendQueryCharData(UINT CharId, UINT CRC);
	void ProcessCharData(G2C_CHAR_APPEAR* msg, CHAR_APPEAR_DATA* data, INT appearType);

	void OnMissileAppear(MISSILE_APPEAR_DATA& InData);

	void NPCChangeState(INT InObjectId, BYTE InState);

    void SetActorAttribute(AMMOActor* Actor, INT AttributeId, INT Value);

	void Send_MoveTo_Mir2( INT objServerID, const FVector& currentPos, const FVector& targetPos, UBOOL bRunOrWalk );
    void Send_MoveTo(INT objServerID, const FVector& currentPos, const FVector& targetPos);
	void SendStopMove();
    void UpdateServerPosition(FVector& NewPosition);
    void UpdateRotation(FRotator& NewRotation,INT DurationTime=0);
    void UpdateTargetPoint(FVector& NewPoint);
    void Send_CheckTimeDelay();
	void Send_QueryPlayerInfo( UINT32 roleID , INT crc );

    void Send_NpcDialogOption(INT para);
    void Send_ReturnLogin();
    void Send_ReturnSelRole ();
	void SendRoleMine(const FVector& targetPos);

	//
	void Send_RechargeGameTime( INT setType );

	//
	void Send_ApplySpecialExp( UINT64 applyCnt );
	void Send_GetCurAllSpecialExp();
	void Send_GetCurAllSpecialExpInfo();

	//连带支付
	void Send_ChainGameMoney( UBOOL openChain );
    //map
    void Send_SelMapDifficulty(int SelDifficulty);
    void Send_EnterDungeon( int SelDifficulty);
	//fb
	void Send_PlayFBAgain();
	void Send_LeaveScene();
	void Send_GetAward( BYTE byIdx );

    void Send_GMCommand(const TCHAR* cmd);

    void Send_ClientTriggerReq( INT dwEventAreaID );
    void Send_OpenLoadingTriggerReq( INT dwTrapID );
    void Send_LoadingTriggerCompleteReq(INT dwTrapID );

	void Send_Teleport(INT srcTeleportID);

	void Send_PkMode(UINT8 byPkMode);
	void Send_CombatPoseOn(UBOOL bOn ,byte byType);
	void Send_CancelAura(INT BuffInstanceId);

	void Send_EquipTitle(BYTE byIdx);
	void Send_UnEquipTitle();

	void Send_ActiveMount(INT MountID);
	void Send_EquipMount(INT MountID);
	void Send_WingOp(INT Op, INT Param);	// c2g_wing_op: 0 query, 1 wear, 2 take off, 3 upgrade
	void Send_Clear_Spell_Cache();

	void Send_HeroSummon();
	void Send_HeroUncall();
	void Send_HeroFire();
	void Send_HeroHire(UINT8 byClass, UINT8 bySex);
	void Send_ChangeHeroAi(UINT8 byAiType);
	void Send_Query_Hero_Attr_List();

	void Send_HardwareInfo();
	void Send_FPS();

	void Send_ReceiveReputationAward(INT nForceID);
	void Send_Change_Pet_State(UINT8 byState);
	void Send_SpellRequestInfo(UINT16 InSpellId, INT32 InCurrentMp, INT32 InCdRemain, INT32 InTargetId, INT16 InTargetX, INT16 InTargetY, INT16 InTargetZ, UINT8 InNotEnoughMp, UINT8 InInCd, UINT32 InTime, UINT8 InShift);

	// add by zhangrui05
	void Send_RoleTeleport(UINT16 posX, UINT16 posY);
	void Send_QueryRoleLocation(const FString& roleName);
	void Send_Dice_bet(UINT16 type, UINT16 count);
	void Send_RecallTeamMember();
	void Send_ChangeRecallState(UBOOL enable);
	void Send_Lovers_Teleport();

	void Send_Activation(const FString& activation);
	
private:

#include "message_handler_Chat.h"

#include "message_handler_Combat.h"
#include "message_handler_Drop.h"
#include "message_handler_Doodad.h"
#include "message_handler_Item.h"
#include "message_handler_Soul.h"
#include "message_handler_Login.h"
#include "message_handler_Quest.h"
#include "message_handler_Stall.h"
#include "message_handler_Team.h"
#include "message_handler_Friend.h"
#include "message_handler_Scene.h"
#include "message_handler_NPC.h"
#include "message_handler_Trade.h"
#include "message_handler_Shop.h"
#include "message_handler_Mail.h"
#include "message_handler_Arena.h"
#include "message_handler_MasterStudent.h"

#include "message_handler_Achievement.h"
#include "message_handler_BattleTeam.h"
#include "message_handler_Guild.h"
#include "message_handler_EquipSlot.h"
#include "message_handler_Weather.h"
#include "message_handler_Heaven.h"
#include "message_handler_EquipDecompose.h"
#include "message_handler_Activity.h"
#include "message_handler_PlayerInfo.h"
//#include "message_handler_UndergroundTrade.h"
#include "message_handler_Ranklist.h"
#include "message_handler_Mall.h"
#include "message_handler_Auction.h"
#include "message_handler_TeamAuction.h"
#include "message_handler_KillRecord.h"
#include "message_handler_GoldStore.h"


#include "message_handler_WebPage.h"
#include "message_handler_EquipRemake.h"
#include "message_handler_Jigsaw.h"

#include "message_handler_DynamicPwd.h"

	M_DECL_PARSE_FUN(g2c_sync_reputation);
	M_DECL_PARSE_FUN(g2c_sync_pk_value);
	M_DECL_PARSE_FUN(g2c_sync_pk_result);
	M_DECL_PARSE_FUN(g2c_sync_grey_name);


	M_DECL_PARSE_FUN(g2c_sync_title);
	M_DECL_PARSE_FUN(g2c_sync_equip_title);
	M_DECL_PARSE_FUN(g2c_add_title);
	M_DECL_PARSE_FUN(g2c_del_title);
	M_DECL_PARSE_FUN(g2c_modify_reputation);
	
    M_DECL_PARSE_FUN(g2c_sync_game_data_begin);
    M_DECL_PARSE_FUN(g2c_sync_data_error);
    M_DECL_PARSE_FUN(g2c_exp_change);
	M_DECL_PARSE_FUN(g2c_exercise_info);
	M_DECL_PARSE_FUN(g2c_get_exercise_exp);
	M_DECL_PARSE_FUN(g2c_add_exercise_time);
	M_DECL_PARSE_FUN(g2c_extra_exp_change);
    M_DECL_PARSE_FUN(g2c_role_level_up);
    M_DECL_PARSE_FUN(g2c_sync_game_data_end);

//    M_DECL_PARSE_FUN(g2c_frame_signal);
	M_DECL_PARSE_FUN(g2c_gpk_dyn_code);
	M_DECL_PARSE_FUN(g2c_gpk_auth_data);
	M_DECL_PARSE_FUN(g2c_char_run);
	M_DECL_PARSE_FUN(g2c_char_walk);
    M_DECL_PARSE_FUN(g2c_char_passive_move);
	M_DECL_PARSE_FUN(g2c_char_stop);
    M_DECL_PARSE_FUN(g2c_object_rotate);
    M_DECL_PARSE_FUN(g2c_sync_container_sizes);
	//M_DECL_PARSE_FUN(g2c_refine_item);
	//M_DECL_PARSE_FUN(g2c_transfer_refine_item);
    M_DECL_PARSE_FUN(g2c_char_appear);
	M_DECL_PARSE_FUN(g2c_char_mine);
	M_DECL_PARSE_FUN(g2c_sync_npc_transform_state);
    M_DECL_PARSE_FUN(g2c_sync_char_list);
    M_DECL_PARSE_FUN(g2c_sync_role_data);
    M_DECL_PARSE_FUN(g2c_sync_attr_list);
    M_DECL_PARSE_FUN(g2c_sync_attr_change);
    M_DECL_PARSE_FUN(g2c_sync_role_avatar);
	M_DECL_PARSE_FUN(g2c_sync_hero_avatar);
	M_DECL_PARSE_FUN(g2c_object_disappear);
	M_DECL_PARSE_FUN(g2c_object_disappear_list);

    M_DECL_PARSE_FUN(g2c_sync_role_pos);
    M_DECL_PARSE_FUN(g2c_return_select_role);

	M_DECL_PARSE_FUN(m2c_sync_role_info);
	M_DECL_PARSE_FUN(m2c_queue_info);

	M_DECL_PARSE_FUN(g2c_sync_vip_state);
	M_DECL_PARSE_FUN(g2c_sync_vip_level);

	M_DECL_PARSE_FUN(m2c_free_mode);
//    M_DECL_PARSE_FUN(g2c_self_hifreq_sync);
//    M_DECL_PARSE_FUN(g2c_region_hifreq_sync);
	M_DECL_PARSE_FUN(g2c_sync_hp);
	M_DECL_PARSE_FUN(g2c_sync_mp);

    M_DECL_PARSE_FUN(g2c_error_notify);
	M_DECL_PARSE_FUN(m2c_error_notify);


    // 金钱同步
    M_DECL_PARSE_FUN(g2c_sync_money);
    M_DECL_PARSE_FUN(g2c_money_change);
	
	M_DECL_PARSE_FUN(t2c_ping_ack);
	M_DECL_PARSE_FUN(g2c_ping_ack);

	M_DECL_PARSE_FUN(g2c_clear_spell_cache);
	// 坐骑同步
	M_DECL_PARSE_FUN(g2c_sync_mount_list);
	M_DECL_PARSE_FUN(g2c_activate_mount);
	M_DECL_PARSE_FUN(g2c_char_on_mount);
	M_DECL_PARSE_FUN(g2c_char_off_mount);
	M_DECL_PARSE_FUN(g2c_char_wing);
	M_DECL_PARSE_FUN(g2c_wing_info);

	//hero
	M_DECL_PARSE_FUN(g2c_sync_hero_data);
	M_DECL_PARSE_FUN(g2c_hero_exp_change);
	M_DECL_PARSE_FUN(g2c_hero_level_up);
	M_DECL_PARSE_FUN(g2c_sync_hero_attr_list);
	M_DECL_PARSE_FUN(g2c_hero_attr_change);
	M_DECL_PARSE_FUN(g2c_hire_hero);
	M_DECL_PARSE_FUN(g2c_fire_hero);
	M_DECL_PARSE_FUN(g2c_summon_hero);
	M_DECL_PARSE_FUN(g2c_change_hero_ai);
	M_DECL_PARSE_FUN(g2c_recall_hero);


	M_DECL_PARSE_FUN(m2c_fcm_state);

	//M_DECL_PARSE_FUN(m2c_h5_url);
	//debug
	M_DECL_PARSE_FUN(g2c_draw_debug_line);


// pc / npc cache datas and functions
public:

	typedef std::map<INT, G2C_SYNC_ROLE_DATA*, M_UE3_STL_ALLOC_MAP(INT, G2C_SYNC_ROLE_DATA*) > INT2SyncRoleDataMap;
	typedef std::map<INT, G2C_SYNC_NPC_DATA*, M_UE3_STL_ALLOC_MAP(INT, G2C_SYNC_NPC_DATA*) > INT2SyncNPCDataMap;
	typedef std::map<INT, G2C_SYNC_HERO_DATA*, M_UE3_STL_ALLOC_MAP(INT, G2C_SYNC_HERO_DATA*) > INT2SyncHeroDataMap;
	//typedef std::map<INT, G2C_SYNC_NPC_EX_DATA*, M_UE3_STL_ALLOC_MAP(INT, G2C_SYNC_NPC_EX_DATA*) > INT2SyncPetDataMap;

	//INT2SyncRoleDataMap		m_RoleDataCache;
	CharCacheData<G2C_SYNC_ROLE_DATA> m_RoleDataCache;
	CharCacheData<G2C_SYNC_NPC_EX_DATA> m_NPCDataCache;
	//INT2SyncNPCDataMap		m_NPCDataCache;
	INT2SyncHeroDataMap		m_HeroDataCache;
	void SyncRoleData(G2C_SYNC_ROLE_DATA* NewData);
	void SyncNpcData(G2C_SYNC_NPC_EX_DATA* NewData);
	void SyncHeroData(G2C_SYNC_HERO_DATA* NewData,  G2C_SYNC_HERO_DATA* OldData);

	

	G2C_SYNC_NPC_EX_DATA* GetNPCSyncData(INT InId)
	{
		return m_NPCDataCache.GetCacheData(InId);
		//INT2SyncNPCDataMap::iterator pos = m_NPCDataCache.find(InId);
		//if (pos != m_NPCDataCache.end())
		//{
		//	return pos->second;
		//}
		//return NULL;
	}

	G2C_SYNC_ROLE_DATA* GetRoleSyncData(INT InId)
	{
		return m_RoleDataCache.GetCacheData(InId);
	}


	void TrySyncRoleData(INT InId);
	void ClearCacheData();
	void CleanMsgSendRestrictList();
	float GetPing()
	{
		return _fPing;
	}

	static INT GetClientEquipTypeByServer( INT ServerSlot );

	UINT32 GetClientOption(){return m_dwClientOption;}

private:
    void checkLog(FVector& checkPos, FVector& startPos, FVector& endPos);
    BOOL _Send(void* data, UINT32 size);

	bool _ProcessReturnLoginError(INT errorCode, INT Param1=0, INT Param2=0);

private:
	enum EMsgSendRestrictList
	{
		MsgRL_Login,
		MsgRL_Create,
		MsgRL_Delete,
		MsgRL_Select,
		MsgRL_Num
	};
	TArray<BOOL>						m_vecMsgSendRestrictList;
    TArray<FString>                     MsgSendRestrictList;
	bool	_disableSend;
    SimpleNet::CNet*                    m_pNet;
	ISocketSession*                     m_pSocketSessionGate;
    ISocketSession*                     m_pSocketSession;
	ISocketSession*                     m_pSocketSession2;
    CPacketAllocator*                   m_pPacketAllocator;
    static CMessageHandler              ms_Instance;

	FString								m_bsIP1;
	FString								m_bsIP2;
	FString								m_gtSessionUserData;

public:

	void SetDisableSend(bool on) { _disableSend = on; }

    static char                         ms_MsgBuffer[MAX_MESSAGE_SIZE];

    BOOL                                m_bConnectBS;
    UINT32                              m_dwServerIP;
    UINT16                              m_dwPort;
    enum ENetConnectionState
    {
        NCS_Closed = 0,
        NCS_ConnectingBS,
        NCS_Connecting,
        NCS_Connected,
        NCS_Disconnecting
    };

	UINT8  _isAdult;
	UINT8	m_byEncryptCode;
	UINT32	m_dwClientOption;

    ENetConnectionState                 _connectState;
    bool                                _bPreLogin;
    bool                                _lockPlayer;
    TCHAR                               _UserName[USER_NAME_LEN];
    TCHAR                               _PW[USER_NAME_LEN]; // should be removed before CB
    TCHAR                               _RoleName[ROLE_NAME_LEN];
    UINT32                              _RoleID;
	float								_fPing;

	UBOOL								bPendingToExit;
#if STATS
	UINT32								dwUploadSpeed;
	UINT32								dwDownloadSpeed;
#endif

//	void _OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize);

    typedef void (CMessageHandler::*MESSAGE_FUNC)(const char* buf, UINT32 size);
    MESSAGE_FUNC                        m_MessageFuncList[ExternalMessage::max_message_count];

	struct NET_PACKET
	{
		INT32			nServerFrame;
		PACKET_BUFFER*	pPacketBuffer;
		UINT32			dwSize;
	};

	std::list<NET_PACKET>				m_PacketCache;

#if M_ENABLE_NET_MSG_BUFF

	bool _fullSpeedProcNetMsg;
	struct NetMsgCacheBuff
	{
		UINT32 Size;
		PACKET_BUFFER* Buffer;
	};

	typedef std::deque<NetMsgCacheBuff> NetMsgBuff;
	NetMsgBuff _msgCache;

	FCriticalSection _msgCacheLock;

	void ClearNetMsgBuf();

	void AddToMsgBuf(PACKET_BUFFER* buf, UINT32 msgSize);
	void TickMsgBuff(FLOAT deltaTime);

	void CheckDuplicateChatContent( FString &Content )
	{
		FString ContentTest = Content.Replace(TEXT(" "), TEXT(""));
		if(Content == g_lastSendContent)
		{
			FString strScreen = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Chat.CanNotSendDuplicate"));
			//MMOUI::GetUIManager()->delegateAddScreenMessage(strScreen,MsgInfoType_Table,TEXT("MMOGame.Chat.NoPrivatePlayer"));
			MMOChatManager::getSingleton().AddSystemMessage(strScreen);
			return;
		}

		g_lastSendContent = Content;
		g_lastSendContent = g_lastSendContent.Replace(TEXT(" "), TEXT(""));
	}

#endif

#if !SHIPPING_PC_GAME
	public:
	FString m_MessageDebugString[ExternalMessage::max_message_count];
#endif
};


#endif  // _LS_MESSAGE_HANDLER_H_
