#ifndef __MMOFACADE_H__
#define __MMOFACADE_H__

#include "MMOSingleton.h"
#include "scene_def.h"
#include "role_def.h"
#include "char_def.h"
#include <bitset>
#include "MMOGameClasses.h"
#include "net/message_handler.h"
#include <vector>
#include <map>
#include <list>
#include "MMOUtil.h"
#include "MMOGameEvent.h"
#include "CommonGameEvents.h"
#include "net/base/process_error.h"
#include "DebugMMO.h"
#include "FSMState.h"

//#define DEBUG_SYNC_INFO
#define UI_OPTIMIZATION
#define BETA_7_7 1

class AMMOPlayerController;
class ANPC;
class AMMOHud;
class AMMOPlayer;
class AMMOActor;
class UResourceTemplateManager;
//struct FNPCTemplate;
//struct FItemTemlpate;
class AMMOVolume_Trigger;
class IIServerTrigger;
struct FSoundParticleEffect;
struct FHitSoundEffectGroup;
class AbilityProcessor;
class MMOBuffManager;
//class MMOPlayerControllerAI;
class FSceneView;
class UCanvas;
class UAI_Affix_Table;

FString NumberToString(UINT num, UINT transfer);
UINT StringToNumber(FString str, UINT transfer);
UINT IntPow(UINT x, UINT y);

typedef TMap<FName, class UMMOUIBase*> UIMaps;
extern UIMaps AllUIMaps;

struct StringMapVal
{
	FString stringVal;
	FString soundName;
	INT	msgInfoType;
};

typedef TMap<FString, StringMapVal> StringMap;

typedef std::map<EGameState, class UMMOGameState*, M_UE3_STL_ALLOC_MAP(EGameState, class UMMOGameState*) > GameStateMap;
extern GameStateMap g_gameStatesMap;

#define UIHudProcess(method)  \
	for(UIMaps::TIterator PairIt(AllUIMaps); PairIt; ++PairIt) \
	{ \
		if(PairIt.Value())PairIt.Value()->method; \
	}

struct ItemRandomAttr
{
	ECHAR_MODIFIER_INDEX formatType;
	FString attrType;
	INT32 value;
	INT32 power;			
	FString strText;
	ESCENE_WEATHER_TYPE weatherType;
	ESCENE_DAYTIME_TYPE	dayType;
};

#define MAX_ITEM_ATTR_COUNT (30)
#define MAX_ITEM_COUNT	(128)
#define MAX_DROP_COUNT	(128)
#define MAX_REFINE_LEVEL    (12)
#define MAX_PK_DROP_ITEM_COUNT	(16)
#define MAX_REPUTATION_COUNT (99)

#define PLAYER_COUNT_SCALE 40

static const INT s_ActorLvAllowGameSystem = 7;

enum WeightType
{
	WT_Bag,
	WT_Weap,
	WT_Armor,
};
enum LanguageType
{
	LT_CHN,
	LT_INT,
	LT_Num,
};

enum LockSortMode
{
	LSM_SortNone,
	LSM_SortBag,
	LSM_SortWarehouse,
	LSM_SortBagResource,
};

enum ItemSortType
{
	IST_Invalid = -1,
	IST_Bag = 1,
	IST_Warehouse = 2,
	IST_BagResource = 3,

};

enum CensoredWordType
{
	CensoredWord_All,
	CensoredWord_Chat,
	CensoredWord_ChatRecive,
};

struct ItemInfo
{
    UINT64 m_llGuid;
    INT32 m_nTemplateID;
    UINT8 m_byConSlot;
    UINT8 m_byItemSlot;
    INT32 m_nCurDur;
    INT32 m_nMaxDur;
    UINT8 m_byBindType;
	UINT8 m_Exclusive;

    //以下属性仅限装备
    //是否已鉴定
    UINT8 m_byIdentified;
    //随机属性
    UINT16 m_wRandomAttrID[MAX_RANDOM_ATTR_COUNT];
    //强化等级
    UINT16 m_wRefineLevel; //To Be Deleted...
    //签名
    UINT32 m_dwRoleID;
	UINT32 m_dwCostMoney;

    //以下属性仅限武器
    //幸运值
    INT8 m_byLuck;
	//武器升级
	UINT8 m_byLevelupPhyPoint;
	UINT8 m_byLevelupMagPoint;
	UINT8 m_byLevelupTaoPoint;
    //铸魂
    UINT16 m_wRune[MAX_RUNE_COUNT];

    //以下属性仅限防具
    //融灵
    UINT32 m_dwSocketCombination;
    INT32 m_nSoulID[MAX_SOCKET_COUNT];

    ItemInfo()
    {
        memset(this, 0, sizeof(ItemInfo));
    }

    UINT8 GetSocketType(UINT8 socketindex) const
    {
        switch (socketindex)
        {
        case 0: return (m_dwSocketCombination & 0x000000FF);
        case 1: return (m_dwSocketCombination & 0x0000FF00) >> 8;
        case 2: return (m_dwSocketCombination & 0x00FF0000) >> 16;
        default: return SOCKET_NONE;
        }
    }
	// 当前堆叠数量
	INT GetCurStackCount(const UItem* pkTemp) const
	{
		if(pkTemp&&pkTemp->ItemDuration == E_DURTYPE_STACK)
		{
			return m_nCurDur;
		}
		return 1;
	}
    template<typename T>
    void ReadBuf(T& t, UINT32& s, const char* pData)
    {
        memcpy(&t, pData+s, sizeof(T));
        s += sizeof(T);
    }

    static const UINT32 HAS_IDENTIFIED      = 0x1;
    static const UINT32 HAS_RANDOM_ATTR_0   = 0x2;
    static const UINT32 HAS_RANDOM_ATTR_1   = 0x4;
    static const UINT32 HAS_RANDOM_ATTR_2   = 0x8;
    static const UINT32 HAS_RANDOM_ATTR_3   = 0x10;
    static const UINT32 HAS_RUNE0           = 0x20;
    static const UINT32 HAS_RUNE1           = 0x40;
    static const UINT32 HAS_RUNE2           = 0x80;
    static const UINT32 HAS_RUNE3           = 0x100;
    static const UINT32 HAS_RUNE4           = 0x200;
    static const UINT32 HAS_RUNE5           = 0x400;
    static const UINT32 HAS_LUCK            = 0x800;
	static const UINT32 HAS_LEVELUP         = 0x1000;
	static const UINT32 HAS_SOCKETS         = 0x2000;
	static const UINT32 HAS_SOUL0           = 0x4000;
	static const UINT32 HAS_SOUL1           = 0x8000;
	static const UINT32 HAS_SOUL2           = 0x10000;
	static const UINT32 HAS_ROLEID          = 0x20000;
	static const UINT32 HAS_COST_MONEY      = 0x40000;

    UINT32 Load(const char* pData)
    {
		UINT8 itemver = 0;
        UINT32 bufsize = 0;
        UINT32 field_bits = 0;
        memset(this, 0, sizeof(ItemInfo));

		ReadBuf(itemver, bufsize, pData);
		if (itemver != 3)
		{
			warnf(NAME_Error, TEXT("客户端与服务器的道具数据结构版本不匹配 服务器版本号: %d, 客户端版本号: 3"), itemver);
		}

        ReadBuf(m_llGuid, bufsize, pData);
        ReadBuf(m_nTemplateID, bufsize, pData);
        ReadBuf(m_byConSlot, bufsize, pData);
        ReadBuf(m_byItemSlot, bufsize, pData);
        ReadBuf(m_nCurDur, bufsize, pData);
        ReadBuf(m_nMaxDur, bufsize, pData);
        ReadBuf(m_byBindType, bufsize, pData);

        ReadBuf(field_bits, bufsize, pData);

        if (field_bits & HAS_IDENTIFIED)
        {
            ReadBuf(m_byIdentified, bufsize, pData);
        }

        if (field_bits & HAS_RANDOM_ATTR_0)
        {
            ReadBuf(m_wRandomAttrID[0], bufsize, pData);
        }

        if (field_bits & HAS_RANDOM_ATTR_1)
        {
            ReadBuf(m_wRandomAttrID[1], bufsize, pData);
        }

        if (field_bits & HAS_RANDOM_ATTR_2)
        {
            ReadBuf(m_wRandomAttrID[2], bufsize, pData);
        }

        if (field_bits & HAS_RANDOM_ATTR_3)
        {
            ReadBuf(m_wRandomAttrID[3], bufsize, pData);
        }

        if (field_bits & HAS_RUNE0)
        {
            ReadBuf(m_wRune[0], bufsize, pData);
        }

        if (field_bits & HAS_RUNE1)
        {
            ReadBuf(m_wRune[1], bufsize, pData);
        }

        if (field_bits & HAS_RUNE2)
        {
            ReadBuf(m_wRune[2], bufsize, pData);
        }

        if (field_bits & HAS_RUNE3)
        {
            ReadBuf(m_wRune[3], bufsize, pData);
        }

        if (field_bits & HAS_RUNE4)
        {
            ReadBuf(m_wRune[4], bufsize, pData);
        }

        if (field_bits & HAS_RUNE5)
        {
            ReadBuf(m_wRune[5], bufsize, pData);
        }

        if (field_bits & HAS_LUCK)
        {
            ReadBuf(m_byLuck, bufsize, pData);
        }

		if (field_bits & HAS_LEVELUP)
		{
			UINT8 useless;
			ReadBuf(useless, bufsize, pData);
			ReadBuf(m_byLevelupPhyPoint, bufsize, pData);
			ReadBuf(m_byLevelupMagPoint, bufsize, pData);
			ReadBuf(m_byLevelupTaoPoint, bufsize, pData);
			ReadBuf(useless, bufsize, pData);
			ReadBuf(useless, bufsize, pData);
		}

        if (field_bits & HAS_SOCKETS)
        {
            ReadBuf(m_dwSocketCombination, bufsize, pData);
        }

        if (field_bits & HAS_SOUL0)
        {
            ReadBuf(m_nSoulID[0], bufsize, pData);
        }

        if (field_bits & HAS_SOUL1)
        {
            ReadBuf(m_nSoulID[1], bufsize, pData);
        }

        if (field_bits & HAS_SOUL2)
        {
            ReadBuf(m_nSoulID[2], bufsize, pData);
        }

        if (field_bits & HAS_ROLEID)
        {
            ReadBuf(m_dwRoleID, bufsize, pData);
        }

		if (field_bits & HAS_COST_MONEY)
		{
			ReadBuf(m_dwCostMoney, bufsize, pData);
		}

        return bufsize;
    }

	INT GetPotential(INT ClassID) const
	{
		INT PotentialWidget[SOCKET_COLORFUL+1] = {0,100,100,50,50,100,200};
		INT Potential=0;
		for(INT i=0;i<3;i++){
			SOCKET_TYPE tp = (SOCKET_TYPE)GetSocketType(i);
			if(tp==SOCKET_RED)			Potential+=100;
			else if(tp==SOCKET_YELLOW)	Potential+=100;
			else if(tp==SOCKET_BLUE)	Potential+=50;
			else if(tp==SOCKET_GREEN)	Potential+=50;
			else if(tp==SOCKET_PURPLE)	Potential+=100;
			else if(tp==SOCKET_COLORFUL)Potential+=200;
			else						Potential+=0;
		}
		return Potential;
	}
};

struct PlayerBaseInfo
{
	UINT32 _RoleID;
	FString _RoleName;
	UINT8 _RoleClass;
	UINT8 _Sex;
	UINT32 _Level;
	UINT32 _VIPType;
	FString	_GuildName;
	INT _Crc;
};


struct Player_Money
{
	INT64            allMoneys[MONEYTYPE_COUNT];
	INT64            allMoneysLimit[MONEYTYPE_COUNT];

	//32位映射------------------------------------------------
	INT32            anMoneys[MONEYTYPE_COUNT];
	INT32            anMoneysLimit[MONEYTYPE_COUNT];

	INT32            anDailyEarned[MONEYTYPE_COUNT];
	INT32            anDailyEarnedLimit[MONEYTYPE_COUNT];

	INT32            anDailyUsed[MONEYTYPE_COUNT];
	INT32            anDailyUsedLimit[MONEYTYPE_COUNT];

	INT32            anWeeklyEarned[MONEYTYPE_COUNT];
	INT32            anWeeklyEarnedLimit[MONEYTYPE_COUNT];

	INT32            anWeeklyUsed[MONEYTYPE_COUNT];
	INT32            anWeeklyUsedLimit[MONEYTYPE_COUNT];

	UINT8            byUseMoneytype01Only;
};

//
enum ESlotGrayMark
{
	ESGMark_Bag					= 1,//0x1,
	ESGMark_WeaponSkill			= 1<<1,//0x2,
	ESGMark_SoulEmbed			= 1<<2,//0x4,
	ESGMark_SoulUnEmbed			= 1<<3,//0x8,
	ESGMark_SoulLevelUp			= 1<<4,//0x10,
	ESGMark_EquipSlotEnhance	= 1<<5,//0x20,
	ESGMark_EquipLevelUp		= 1<<6,//0x40,
	ESGMark_BackMakeUp			= 1<<7,//0x80,
	ESGMark_BackUpgrade			= 1<<8,//0x100,
	ESGMark_NpcShop             = 1<<9,//0x200,
	ESGMark_WeaponSkillRplc		= 1<<10//0x400,
};

enum EItemSlotHint
{
	ItemSlotHint_Normal		= 0x0,
	ItemSlotHint_New		= 0x1,
	ItemSlotHint_PowerEquip	= 0x2,
};

struct FClientItemInfo : public ItemInfo
{
    //拆分出售后 记录原始数据
    int  OriDur;
    //出售数量
    int  OnSaleCount;
	//是否被拖入其它界面(灵魂融合，灵魂升级)
	int IsDragIntoOtherUI;
	//是否被某界面标记
	UINT32 IsGrayMarkedInUI;
	//战斗力值
	INT Pow;
	//是否新获得
	//bool IsNew;
	//是否已经上架
	bool IsOnSale;
	//抽取熟练度中
	bool IsOnWeaponExp;
	//是否闪烁提示
    INT NeedHintMode;
	//子类型
	INT ItemSubTypeNum;

	//拍卖行id
	INT32 AuctionId;

    FClientItemInfo()
		: OriDur(0)
		, OnSaleCount(0)
        , IsDragIntoOtherUI(-1)
		, IsGrayMarkedInUI(0)
		, Pow(0)
		//, IsNew(false)
		, IsOnSale(false)
		, IsOnWeaponExp(false)
		, NeedHintMode(ItemSlotHint_Normal)
		, ItemSubTypeNum(0)
		, AuctionId(0)
	{
	}

    //拆分
    void MinusSaleDur( int inCount )
    {
        SetSaleDur( m_nCurDur - inCount );
    }
    //设置背包剩余叠加量
    void SetSaleDur( int inDur )
    {
        IsOnSale  = true;
        OriDur = m_nCurDur;
        m_nCurDur = inDur;
        OnSaleCount = OriDur - inDur;
    }
    //重置
    void ResetSaleDur()
    {
        IsOnSale  = false;
        m_nCurDur = OriDur;
        OnSaleCount = 0;
    }
	//解除Lock部分数量
	void RecoverSaleDur(int inCount)
	{
		m_nCurDur += inCount;

		if(m_nCurDur >= OriDur)
		{
			//恢复原状
			ResetSaleDur();
		}
		else
		{
			OnSaleCount = OriDur - m_nCurDur;
			IsOnSale = true;
		}
		
	}

	//当前堆叠数量
	INT GetCurStackCount(const UItem* pkTemp) const
	{
		if(pkTemp&&pkTemp->ItemDuration == E_DURTYPE_STACK)
		{
			if(IsOnSale)
			{
				return OriDur;
			}
			else
			{
				return m_nCurDur;
			}
		}
		return 1;
	}
	//简单粗暴，先把所有数据都搞进去
	FString ToReadableString();
//	{
//		INT i = 0;
//		FString ret;
//		ret.Empty();
//
//		//#define ADDELESTRING(n) ret += (appItoa(n) + TEXT(","));
//#define ADDELESTRING(n) ret += (NumberToString(n,20) + TEXT(","));
//		ADDELESTRING(m_nTemplateID);
//		ADDELESTRING(m_byConSlot);
//		ADDELESTRING(m_byItemSlot);
//		ADDELESTRING(m_nCurDur);
//		ADDELESTRING(m_nMaxDur);
//		ADDELESTRING(m_byBindType);
//		ADDELESTRING(Pow);
//
//		UItem* item = MMOFacade::getSingleton().FindItemTemplate(m_nTemplateID);
//
//		if(item && item->ItemType == TEXT("ITEMTYPE_EQUIP"))
//		{
//			//以下属性仅限装备
//			ADDELESTRING(m_byIdentified);
//			for(i = 0; i < MAX_RANDOM_ATTR_COUNT; ++i)
//			{
//				ADDELESTRING(m_wRandomAttrID[i]);
//			}
//			//ADDELESTRING(m_wRefineLevel);
//			ADDELESTRING(m_dwRoleID);
//
//			//以下属性仅限武器
//			if (item->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_WEAPON"))
//			{
//				ADDELESTRING(m_byLuck);
//				for(i = 0; i < MAX_RUNE_COUNT; ++i)
//				{
//					ADDELESTRING(m_wRune[i]);
//				}
//
//				ADDELESTRING(m_byLevelupPhyPoint);
//				ADDELESTRING(m_byLevelupMagPoint);
//				ADDELESTRING(m_byLevelupTaoPoint);
//			}
//			else
//			{
//				//以下属性仅限防具
//				ADDELESTRING(m_dwSocketCombination);
//				for(i = 0; i < MAX_SOCKET_COUNT; ++i)
//				{
//					ADDELESTRING(m_nSoulID[i]);
//				}
//			}
//			
//		}
//		
//
//#undef ADDELESTRING
//		//去掉最后一个逗号
//		ret = ret.Mid(0,ret.Len() - 1);
//		return ret;
//	}
		
	void FromGFXStringArray(UGFxObject* params, INT startIndex);
//	{
//		INT i = 0;
//		//#define PARSEELESTRING() (appAtoi(params->GetElementString(startIndex++)));
//#define PARSEELESTRING() (StringToNumber(params->GetElementString(startIndex++),20));
//		m_nTemplateID = (INT32)PARSEELESTRING();
//		m_byConSlot = (UINT8)PARSEELESTRING();
//		m_byItemSlot = (UINT8)PARSEELESTRING();
//		m_nCurDur = (INT32)PARSEELESTRING();
//		m_nMaxDur = (INT32)PARSEELESTRING();
//		m_byBindType = (UINT8)PARSEELESTRING();
//		Pow = (INT)PARSEELESTRING();
//
//		UItem* item = MMOFacade::getSingleton().FindItemTemplate(m_nTemplateID);
//		if(item && item->ItemType == TEXT("ITEMTYPE_EQUIP"))
//		{
//			m_byIdentified = (UINT8)PARSEELESTRING();
//			for(i = 0; i < MAX_RANDOM_ATTR_COUNT; ++i)
//			{
//				m_wRandomAttrID[i] = (UINT16)PARSEELESTRING();
//			}
//			m_dwRoleID = (UINT32)PARSEELESTRING();
//			
//			//以下属性仅限武器
//			if (item->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_WEAPON"))
//			{
//				m_byLuck = (INT8)PARSEELESTRING();
//				for(i = 0; i < MAX_RUNE_COUNT; ++i)
//				{
//					m_wRune[i] = (UINT16)PARSEELESTRING();
//				}
//				m_byLevelupPhyPoint = (UINT8)PARSEELESTRING();
//				m_byLevelupMagPoint = (UINT8)PARSEELESTRING();
//				m_byLevelupTaoPoint = (UINT8)PARSEELESTRING();
//			}
//			else
//			{
//				m_dwSocketCombination = (UINT32)PARSEELESTRING();
//				for(i = 0; i < MAX_SOCKET_COUNT; ++i)
//				{
//					m_nSoulID[i] = (INT32)PARSEELESTRING();
//				}
//			}		
//		}
//		
//		
//#undef PARSEELESTRING
//	}
};


/*
//hemu@snda
//TActorInstanceIterator 迭代出指定类型 actor instance in GWorld
//相对TObjectIterator提高效率 
//Example：
TActorInstanceIterator< APlayerStart > tempIter;
while ( *tempIter )
{
APlayerStart* plStart = (*tempIter);
++tempIter;
}
*/
template< class T > 
class TActorInstanceIterator
{
public:
    TActorInstanceIterator()
        :Class( T::StaticClass() ), LevelIndex( 0 ),ActorIndex( 0 ),Actor( 0 )
    {
        ++*this;
    }

    void operator++()
    {
        while ( GWorld && LevelIndex < GWorld->Levels.Num() )
        {
            ULevel* level = GWorld->Levels( LevelIndex );
            while ( level && ActorIndex < level->Actors.Num() )
            {
                AActor* actortemp = level->Actors( ActorIndex );
                if( actortemp && actortemp->IsA( Class ) )
                {
                    Actor = actortemp;
                    ++ActorIndex;
                    return;
                }
                ++ActorIndex;
            }
            ActorIndex = 0;
            ++LevelIndex;
        }
        Actor = NULL;
    }

    T* operator*() const
    {
        return GetObject();
    }

    T* operator->() const
    {
        return GetObject();
    }

    bool operator !() const 
    { 
        return !Actor; 
    }

protected:

    T* GetObject() const 
    { 
        return (T*)Actor; 
    }

    UClass* Class;
    AActor* Actor;
    INT LevelIndex;
    INT ActorIndex;
};


//class MMOBattleManager;
struct EnhanceData;
struct EnhanceResult;

typedef std::map<INT, class UNPCTemplate*, std::less<INT>, STLAllocator< std::pair<INT, class UNPCTemplate*>, UE3STLAllocatorPolicy> > NPCTemplateMap;
typedef std::map<INT, class UItem*, std::less<INT>, STLAllocator< std::pair<INT, class UItem*>, UE3STLAllocatorPolicy> > ItemTemplateMap;

class PlayerProxy;


//typedef std::map< std::string , class UAI_Affix_Table*> AffixTemplateMap;
struct AffixClientInfo
{
	INT ID;
	INT Type;
	FString Name;
};


//副本评分
struct FBSummaryData
{
	//通关时间
	UINT m_uiClearanceTimePt;//评分
	UINT m_uiClearanceTime;//秒
	//击杀技巧
	UINT m_uiKillSkillPt;//评分
	UINT m_uiKilled;
	UINT m_uiDestroyed;
	UINT m_uiStrategy;
	//成就
	UINT m_uiAccomplishPt;//评分
	//INT m_iAccompLishID[8];
	//综合评分
	UINT m_uiGradedPt;
	UINT m_uiGradedExp;
	//额外加成
	UINT m_uiVIPExp;
	UINT m_uiGuildExp;
	UINT m_uiActivityExp;
	UINT m_uiFirstTimeExp;
	UINT m_uiCouplesExp;
	//总加成
	UINT m_uiSumUpExp;
	UINT GetSumUpExp(){ 
		return m_uiSumUpExp;
		//return ( m_uiGradedExp + m_uiVIPExp + m_uiGuildExp + m_uiFirstTimeExp + m_uiCouplesExp );
	}
};
//副本奖励
struct FBAward
{
	UINT uiItemID;
	UINT uiItemCnt;
	UBOOL isValid;
};

static const UINT MAX_FB_AWARDS_CNT = 3;
struct FBMemberAwardData
{
	UINT m_uiMemberID;//0表示此项空
	//var string MemberName;
	UINT m_uiFBGradedPt;
	UINT m_uiFBAwardCount;
	FBAward m_Award[MAX_FB_AWARDS_CNT];
};

//npc头顶显示配置表
struct HintConfig
{
	FString NpcTypeName;
	INT NpcType;
	INT NpcColor;
	INT NameVisible;
	INT StatusVisible;
	INT IconVisible;
	INT HPVisible;
};

//title表
struct TitleData 
{
	//表值，顺序
	INT ID;
	INT ValidTime;
	INT Pow;
	INT AttrID[5];
	//FString AttrType1;
	//INT AttrValue1;
	//FString AttrType2;
	//INT AttrValue2;
	//FString AttrType3;
	//INT AttrValue3;
	//FString AttrType4;
	//INT AttrValue4;
	//FString AttrType5;
	//INT AttrValue5;
	FString ConditionType1;
	INT ConditionValue1;
	FString ConditionType2;
	INT ConditionValue2;
	FString TitleName;
	FString TitleInfo;
	INT		TitleLevel;
	INT		TitleStyle;
	INT		TitleType;
	BOOL	IsNew;
	INT		HoverShow;

};

//tween data物理运动效果设置数据
struct TweenData
{
	//单位厘米,秒
	FLOAT sx;
	FLOAT sy;
	FLOAT sz;				

	FLOAT vx;		
	FLOAT vy;				
	FLOAT vz;

	FLOAT ax;		
	FLOAT ay;				
	FLOAT az;				

	FLOAT sxRange;		
	FLOAT syRange;				
	FLOAT szRange;				

	FLOAT vxRange;		
	FLOAT vyRange;				
	FLOAT vzRange;				

	FLOAT axRange;		
	FLOAT ayRange;				
	FLOAT azRange;				

	FLOAT scaleStart;
	FLOAT scaleBomb;
	FLOAT scaleEnd;

	FLOAT scaleStartRange;
	FLOAT scaleBombRange;
	FLOAT scaleEndRange;

	FLOAT durTime;		
	FLOAT bombTime;
	FLOAT dumping;		

	FLOAT alphaStart;		
	FLOAT alphaBomb;		
	FLOAT alphaEnd;		
	FLOAT alphaPow;	
	INT	  isVisible;

};

//mount结构
struct MountData
{
	INT ID;
	FString Name;
	INT Quality;
	INT LevelLimit;
	INT SpeedModificationRate;
	INT HitUnmountRate;
	INT ItemID1;
	INT ItemCount1;
	INT ItemID2;
	INT ItemCount2;
	INT ItemID3;
	INT ItemCount3;

	FString IconName;
	FString Script;
	INT Have;
};

//wings.txt (1.05)
struct WingData
{
	INT ID;
	FString AvatarName;	//object path of the wing AvatarData, e.g. Wing_Avt_Data.AvatarData_Wing_01_Lv1
};

//显示结构
struct ShowPanelAdvancedData
{
	FString PanelName;
	FString	TabName;
	INT		Prarm1;
	FString Prarm2;

	ShowPanelAdvancedData()
	{
		PanelName = TEXT("");
		TabName	  = TEXT("");
		Prarm1	  = 0;
		Prarm2	  = TEXT("");
	}
	
};

//声望结构
struct ReputationTemp
{
	INT Index;
	INT ForceID;
	INT ListLevel;
	FString ForceName;
	INT ForceVisible;
	INT SceneID;
	FString ForceTips;
	FString ForceInfo;
	//FString ForceAward[4];
	INT InitValue;
	INT MaxValue;
	INT _CanAttackWithFreePkMode;

	INT _WarriorAwardItemID[MAX_REPUTATION_AWARD_COUNT];
	INT _WarriorAwardItemCount[MAX_REPUTATION_AWARD_COUNT];

	INT _MageAwardItemID[MAX_REPUTATION_AWARD_COUNT];
	INT _MageAwardItemCount[MAX_REPUTATION_AWARD_COUNT];

	INT _RogueAwardItemID[MAX_REPUTATION_AWARD_COUNT];
	INT _RogueAwardItemCount[MAX_REPUTATION_AWARD_COUNT];

	INT _ArcherAwardItemID[MAX_REPUTATION_AWARD_COUNT];
	INT _ArcherAwardItemCount[MAX_REPUTATION_AWARD_COUNT];

	INT _TaoistAwardItemID[MAX_REPUTATION_AWARD_COUNT];
	INT _TaoistAwardItemCount[MAX_REPUTATION_AWARD_COUNT];
};


//二级属性列表
struct Attribute2
{
	INT ClassID;
	INT AttrName;
	INT ConstParam;

	INT catStrength;
	INT	catAgility;
	INT	catIntellect;
	INT	catStamina;
	INT	catSpirit;
	INT	catValue;
	INT	catCombatParam;

	Attribute2()
	{
		appMemzero( this , sizeof(Attribute2) );
	}

	//INT Strength;
	//INT Agility;
	//INT Intellect;
	//INT Stamina;
	//INT Spirit;
};


// typedef std::map<INT, struct AffixClientInfo, std::less<INT>, STLAllocator< std::pair<INT, struct AffixClientInfo>, UE3STLAllocatorPolicy> > MapAffixID2Data;
typedef std::map<UINT, class UMMOAbility*, std::less<UINT>, STLAllocator< std::pair<INT, class UMMOAbility*>, UE3STLAllocatorPolicy> > AbilityTemplateMap;
typedef std::map<UINT, class UShop*, std::less<UINT>, STLAllocator< std::pair<INT, class UShop*>, UE3STLAllocatorPolicy> > ShopTemplateMap;
typedef std::map<UINT32, FClientItemInfo*, std::less<UINT32>, STLAllocator< std::pair<INT, FClientItemInfo*>, UE3STLAllocatorPolicy> > ItemsMap;
typedef std::map<INT, TitleData, std::less<INT>, STLAllocator< std::pair<INT, struct TitleData>, UE3STLAllocatorPolicy> > TitleDataMap;
typedef std::map<FString, struct TweenData, ComparFString, STLAllocator< std::pair<FString, struct TweenData>, UE3STLAllocatorPolicy> > TweenDataMap;
typedef std::map<INT, struct MountData, std::less<INT>, STLAllocator< std::pair<INT, struct MountData>, UE3STLAllocatorPolicy> > MountDataMap;
typedef std::map<INT, struct WingData, std::less<INT>, STLAllocator< std::pair<INT, struct WingData>, UE3STLAllocatorPolicy> > WingDataMap;

typedef std::map<FString, struct ShowPanelAdvancedData, ComparFString, STLAllocator< std::pair<FString, struct ShowPanelAdvancedData>, UE3STLAllocatorPolicy> > ShowPanelAdvancedDataMap;



struct MMO
{
static FCheckResult* PickActor(FVector StartWorldPos,FVector EndWorldPos, AActor* SrcActor, 
	UBOOL TestAABBOnly = FALSE, QSORT_COMPARE fnSort = NULL, DWORD TraceFlags = TRACE_Actors | TRACE_ComplexCollision, 
	const float SkeletalMin = 0.0f, const float SkeletalMax = 0.5f,
	const float CylinderMin = 0.5f, const float CylinderMax = 0.9f,
	const float AABBMin = 0.9f, const float AABBMax = 1.0f);

static FCheckResult* PickPoint(FVector StartWorldPos,FVector EndWorldPos, AActor* SrcActor, 
	UBOOL TestAABBOnly = FALSE, QSORT_COMPARE fnSort = NULL, DWORD TraceFlags = TRACE_Actors | TRACE_AllComponents);


static void FormatTempleteDataLoadingDir( TArray<FString>& dirArray );
static FString FormatTempleteDataLoadingDir( const FString& srcDirName );

static UBOOL IsForAllPlayerClass(FSpell_ClassType classType);
static UBOOL CanBeUsedByPlayerClass(EPlayerClass playerClass, FSpell_ClassType classType);
static UBOOL CanSpellBeUsedByPlayerClass(EPlayerClass playerClass, FSpell_ClassType classType); // spell.txt mask convention
static UBOOL CanBeUsedByPlayerGender(EGenderType playerGender, BYTE genderReq);
static FVector GetGfxOffsetVector(FVector Pos,float CavWidth,float CavHeight);
static UBOOL IsClassTypeMatch(const FSpell_ClassType& newReqTyp, const FSpell_ClassType& useReqTyp);

static void ActiveGameState(EGameState gs);
static void PreActiveGameState(EGameState gs);
static void ProShowLoading(UBOOL bShow);
static EGameState GetCurrentGameState();
static void AddGameState(class UMMOGameState* st);
static class UMMOGameState* FindGameStateObject(EGameState gs);

static void ActiveKismetPlayBik(const FString& bikName, const bool bPlayBGM );
static void ActiveKismetRemoteEvent(const FName& eventName);
static void ActiveKismetEvent_ServerVarChange(INT varType, INT varIndex, INT varReferID, INT varValue);

static SHORT GetAsyncKeyStateEx(int vKey);

template<typename T>
static TArray<T*> FindObjectInKisemetByType()
{
	USequence* rootSeq = GWorld->GetGameSequence();
	TArray<T*> objList;
	if(!rootSeq)
	{
		warnf(NAME_Warning, TEXT("[FindObjectInKisemetByType] root sequence is null!"));
		return objList;
	}

	rootSeq->FindSeqObjectsByClass(T::StaticClass(), (TArray<USequenceObject*>&)objList);

	return objList;
}

template<typename T>
static void TActiveKismetEvent_ServerVarChange( INT varIndex, INT varReferID, INT varValue, bool compareRefID = true )
{
	// look for all remote events in the entire sequence tree	
	USequence* rootSeq = GWorld->GetGameSequence();
	if(!rootSeq)
	{
		warnf(NAME_Warning, TEXT("[MMO::TActiveKismetEvent_ServerVarChange] root sequence is null!"));
		return;
	}

	//rootSeq = rootSeq->GetRootSequence(TRUE);

	TArray<T*> varEvents;

	rootSeq->FindSeqObjectsByClass(T::StaticClass(), (TArray<USequenceObject*>&)varEvents);

	for (INT Idx = 0; Idx < varEvents.Num(); Idx++)
	{
		T* varEvent = varEvents(Idx);
		if (varEvent != NULL && varEvent->bEnabled)
		{
			//varEvent->VarIndex = varIndex;
			//varEvent->VarReferID = varReferID;
			//varEvent->VarValue = varValue;
			// check activation for the event
			//varEvent->PopulateLinkedVariableValues();
			
			varEvent->PublishLinkedVariableValues();

			debugf(TEXT("-----SceneVarChange server == kismet %s varIndex:%d == %d varReferID:%d == %d varValue:%d == %d"),
				*varEvent->GetFullName(),
				varIndex, varEvent->VarIndex,
				varReferID, varEvent->VarReferID, 
				varValue, varEvent->VarValue);

			if(compareRefID)
			{
				if(varEvent->VarIndex == varIndex
					&& varEvent->VarReferID == varReferID
					&& varEvent->VarValue == varValue)
				{
					varEvent->ForceActivateOutput(0);
				}
			}
			else if(varEvent->VarIndex == varIndex				
				&& varEvent->VarValue == varValue)
			{
				varEvent->ForceActivateOutput(0);
			}
		}
	}
}

static bool IsRequiredVarOK(INT varID, INT desiredVarValue);
static void SetServerSceneVars(const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA* vars, INT count);
static void UpdateAllServerSceneVars();
static TArray<FString> SplitString( const std::wstring& str, const std::wstring& delims = TEXT("\t\n "), unsigned int maxSplits = 0, bool preserveDelims = false);

};

extern TArray<FString> UseMMOSplitString(const std::wstring& str, const std::wstring& delims, unsigned int maxSplits, bool preserveDelims);  //Bruce@snda

class MMOFacade : public SingletonMeyers<MMOFacade>, public INativeGameEventHandler
{
public:

	MMOFacade(void);
	virtual ~MMOFacade(void);

	void Init();

	static const INT MainCharacterID;

	void Shutdown();
	void TickLogic(FLOAT deltaTime);

	void SetCurrentPlayerController(class AMMOPlayerController* ctrl);

	//unsigned int GetTimeTickCount() const;

	static void SetMouseCursor(EMMOCursorType cursorIndex);
	static void ShowMouseCursor(UBOOL Show);

	static FString CheckName( const FString& word );

	static FIntPoint GetMouseCursorPosition();

	static UBOOL GetCaptureInput();
	static void UpdateCaptureInput();
	class UGFxObject* GetMouseTopMostEntity();

	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);

	BOOL GetFingerPos(){return bFinger;}
	void SetFingerPos(BOOL v){bFinger = v;}

	BOOL GetWristPos(){return bWrist;}
	void SetWristPos(BOOL v){bWrist = v;}

	BOOL GetFingerPosShowType(){return bFingerShowType;}
	void SetFingerPosShowType(BOOL v){bFingerShowType = v;}

	INT GetMoneyRatio();

public:
	//hemu 账户角色信息 IO相关--------------------------------------------Begin

	//直接导出地址上连续的账户角色信息到一个指定DataTypeName类型的二进制文件
	void ExportSimpleAccountData( void *pData , size_t lDataLength , FString DataTypeName , UBOOL bingUsename = TRUE );

	static FString GetUserAccountStoreDir();
	//开始导出账户角色信息到一个指定DataTypeName类型的二进制文件
	UBOOL BeginExportAccountData( FArchive** pArchive , FString DataTypeName , UBOOL bingUsename = TRUE /*和用户名有关*/ );

	//需要与BeginExportAccountData成对使用
	void EndExportAccountData( FArchive* pArchive );

	//读取账户角色信息中DataTypeName的二进制数据
	UBOOL BeginLoadAccountBinary( FArchive** pArchive,FString DataTypeName , UBOOL bingUsename = TRUE );

	//需要与BeginLoadAccountBinary成对使用
	void EndLoadAccountBinary( FArchive* pArchive );

	//hemu 账户角色信息 IO相关--------------------------------------------End

	// 创建玩家
	AMMOPlayer* CreatePlayer(INT Id, const FVector& initLoc, const FRotator& initRot);

	bool IsSelf(AMMOPlayer* player) const;
	bool IsSelf(INT roleID) const;
	UBOOL IsMonsterOwnedByMe( AMMOActor *pActor ) const;

	// 创建NPC,如果已存在,会返回已存在的对象
	ANPC* CreateNPC(INT Id, const FVector& initLoc, const FRotator& initRot);


	// 上面两个创建函数将会被这个函数替换掉,统一玩家与npc的创建流程
	AMMOActor* CreateCharacter(INT Id, INT CharacterType, const FVector& initLoc, const FRotator& initRot);

	AMMOEmitter* CreateEmitter(INT Id, INT EmitterTemplateId);

	void UpdatePlayerShowCount(FLOAT Delta);

	void AddOjbectToSync(INT InId);
	void RemoveOjbectToSync(INT InId);
	void RemoveOjbectToSync();
	UBOOL ForceSyncObject(INT InId, UBOOL bClearSyncList);
	void UpdateObjectsToSync(FLOAT Delta);
	void StartObjectsOrder();
	void OperateObjectsOrder(AMMOActor* pAMMOActor);
	void EndObjectsOrder();

	void GetTimeString(UINT32 time, FString& timeString);
	void GetDurationTimeString(UINT32 startTime, FString& timeString);
	void GetCountdownTimeString(UINT32 endTime, FString& timeString);

	// 获取主玩家
	void AddMainCharacter(AMMOPlayer* MainCharacter);
	//MMOPlayerControllerAI* GetMainControllerAI();
	AMMOPlayerController* GetMainController();
	AMMOActor* GetMainActor() const;
	AMMOHud* GetMyHud();
	AMMOPlayer* GetMainPlayer() const;
	AMMOPlayer* GetMainHero() const;
	INT			GetMainHeroId();
	void		SetMainHeroId(INT HeroId);

	UBOOL		GetHeroSummon();
	void		SetHeroSummon(UBOOL bSummon);

	FLOAT GetMainActorCollisionHeight();

	// 获取对象. NULL表示不存在
	AMMOActor* GetCharacter(INT Id);

	AActor* GetActor(INT Id);
	ANPC* GetFrendlyNpc( INT hashindex);
	AMMOEmitter* GetEmitter(INT Id);

	void DestroyObject(INT Id, UBOOL bRemoveFromWorld, INT disappearType);

	void RemoveFromCharacterMap(INT Id);
	void AddToCharacterMap(AMMOActor* actor, INT id);

	void ClearAllCharacter();

	// 添加/清除/获取 音效/粒子对资源. 用于打击,脚步表现
	void AddSoundParticle(INT key, FSoundParticleEffect* node);
	void ClearSoundParticle();
	FSoundParticleEffect* GetSoundParticle(INT key);

	//词缀
	//void				AddAffixTable( const FString& Name , UAI_Affix_Table* node );
	//UAI_Affix_Table*	FindAffixTable( INT monsterLevel , INT monsterQuantity );
// 	BOOL InitAffix();
// 	struct AffixClientInfo* FindAffix( INT ID );

	FString GetPlayerIcon( INT classId , INT sex );

	void				SetSceneLevel( INT SceneLevel );
	INT					GetSceneLevel( );

	void SetSceneDifficulty(INT sceneDifficulty);
	INT GetSceneDifficulty() const { return _sceneDifficulty; }

	// 添加/清除/获取 动画混合表
	void							AddAnimMix(INT key, FAnimationNode* node);
	void							ClearAnimMix();
	FAnimationNode*	GetAnimMix(INT key);

	void AddHitSoundGroup(INT key, USoundCue* node);
	void ClearHitSoundGroup();
	USoundCue* GetHitSound(INT key) const;

	void StopVoiceSound(const FString& SoundName, BOOL Immediately, FLOAT FadeOutDuration, FLOAT FadeVolumeLevel);
	void StopAllVoiceSound(BOOL Immediately, FLOAT FadeOutDuration, FLOAT FadeVolumeLevel);

	void AddItemTemplate( INT key, class UItem* node);
	void ClearItemTemplates ();
	class UItem* FindItemTemplate(INT TemplateID);
	class UItem* FindItemTemplate(UINT16 ContainerID, INT SlotID);
	// 	const class UItem* FindItemTemplate(INT TemplateID) const;
	const ItemTemplateMap& GetItemTemplateMap() const { return _itemTemplateMap; }

	//UpdatePetLevel
	void UpdatePetLevelColor(AMMOActor* actor);
	// 清除角色身上相关
	void ClearMainPlayer();

	// 回购相关功能
	void ClearSoldItems();
	void AddSoldItem(ItemInfo* info);
	INT GetSoldItemCount() const;
	struct ItemInfo* FindSoldItem(INT SlotID) const;
	void NotifySoldItemChange();


	//void InitEnhanceData( UINT itemLvCnt, UINT enhanceLvCnt );
	//void AddEnhanceData( UINT enhanceLv, EnhanceData* data);
	UINT32 GetEnhanceResultKey( const FString & equipSubType, const FString &equipSubSubType );
	const EnhanceData* GetEnhanceConsumption( UINT curEnhanceLv );
	const EnhanceResult* GetEnhanceResult( const FString & equipSubType, const FString &equipSubSubType, UINT enhanceLv );

	void UpdateItemDurability( INT conIdx, INT slotIdx, INT curDur );
	//static size_t GetItemSize(ItemInfo* info);
	static void UpdateItemInfo(ItemInfo* src, struct FClientItemInfo* dest);
	FClientItemInfo* AddItem( ItemInfo* iteminfo , UBOOL showNew = TRUE , UBOOL notify = TRUE);
	FClientItemInfo* UpdateItem(ItemInfo* iteminfo);
	FClientItemInfo* UpdateItemByContainerId(ItemInfo* iteminfo,UINT16 ContainerTypeID);

	void DeleteItem(UINT16 ContainerTypeID, UINT16 SlotIdx,UBOOL bNotify = TRUE);
	void SwapItem(G2C_ITEM_SWAP& SwapInfo);
	void ClearItems();
	void ClearItemsByContainerId(UINT16 ContainerTypeID);
	void ClearPlayerEquip();
	void LockItem( INT ContainerID, INT SlotID, INT isToLockIn );
	void GrayItem( ESlotGrayMark opPanelMaskEnum, INT containerID, INT slotID, UBOOL isToGray );
	//void HintItem( INT ContainerID, INT SlotID, EItemSlotHint hintType );
	FClientItemInfo* FindItem(UINT16 ContainerTypeID, UINT16 SlotIdx) const;
	FString GetItemIconNamePersonalized( UItem *pItem, const ItemInfo *pItemInfo = NULL, INT iconState = 0 );
	bool IsItemExist(UINT16 ContainerTypeID, UINT16 SlotIdx) const;
	INT GetItemCount(INT itemID);
	UBOOL IsBagFull();
	INT GetBagEmptySlotCount();
	void NotifyItemChange(UINT16 ContainerTypeID, UINT16 SlotIdx);
	INT GetItemCountInBag(UINT itemID);
	INT GetItemCountInBag(UINT itemID, UINT16 containerID);
	UBOOL GetItemDurInBag( UINT itemID, UINT itemSubType, INT32 RequireDur );
	INT GetUsedSlotCountInBag(UINT16 containerID) const;
	INT GetEmptyItemSlot(ECONTAINER_SLOT ContainerID) const;
	INT GetCanStackSlot( ECONTAINER_SLOT ContainerID , INT itemId , INT stackCount ) const;
	UBOOL IsItemPack(UINT itemPack);
	FClientItemInfo* FindFirstItemPos( UINT itemID , INT* outContainerTypeID , INT* outSlotIdx) const;
	FClientItemInfo* FindFirstItemPosInContainer( UINT itemID, INT containerTypeID, INT& outSlotIdx ) const;

	//NPC回收物品价格
	INT GetEquipNpcPrice( UItem *pItem, const ItemInfo *pItemInfo );

	void AddNPCTemplate(INT k, class UNPCTemplate* npc);
	void ClearNPCTemplates();
	class UNPCTemplate* FindNPCTemplate(INT TemplateID);

// 	void AddShopTemplate(INT id, class UShop* shop);
// 	void ClearShopTemplates();
// 	class UShop* FindShopTemplate(INT TemplateID);
// 	FShopItemTemplate FindShopItem(INT ShopTemplateID, INT ItemIdx);
	


	void AddMissileTemplate(INT Id, UMissile* data);
	UMissile* FindMissileTemplate(INT Id);
	void ClearAllMissileTemplate();

	// UI Sound
	void ClearUISound();
	void AddUISound(USoundCue* sound);
	USoundCue* GetUISound(const FString& name) const;
	void PlayUISound(const FString& cueName);
	const ItemRandomAttr* GetRandomAttributById( int id );
	FString GetRandomAttributeFormatedValue(const ItemRandomAttr* pItemRandomAttr);
	INT IsSameClassRandomAttributeBetter( const ItemRandomAttr* pAttr, const ItemRandomAttr* pCompareToAttr );

public:
	GAME_CONTROL_TYPE  GCTType;
	INT				BlockType;
	FLOAT			BlockRadius;
	INT				PetState;

	class UGFxObject* TopMostObject;

	BOOL bFinger;
	BOOL bWrist;
	BOOL bFingerShowType;
	UBOOL IsReceivedGameData;
	UBOOL bLevelsLoaded;
private:
	
	TArray<Attribute2> vecAttribute2;
	//基础属性读取值
	typedef std::map< INT,STLIntVector, M_UE3_STL_ALLOC_MAP(INT, STLIntVector) > Int2IntVectorMap;
	Int2IntVectorMap BaseAttributeValue;
	//attributeId 对应 下标值
	TArray<INT>  BaseAttributeValueMap;
	//level power
	TMap< INT , INT > LevelPowerMap;
	//level exp
	TMap< INT , INT > LevelExpMap;

	//level spiritPower
	TMap< INT , INT > LevelSpiritPowerMap;

	typedef std::map<INT, struct ItemRandomAttr, M_UE3_STL_ALLOC_MAP(INT, struct ItemAttr) > Int2ItemAttrMap;
	Int2ItemAttrMap RandomAttributesMap;

	TArray<INT> vecPkLevel;
	HintConfig NpcHintInfo[EACT_MAX];
	HintConfig NpcQualityHintInfo[E_nq_MAX];

	DOUBLE LastTimeCheckPlayerShowOrNot;

	FVector m_BlockPosition;
	UBOOL   m_BlockCheck;
	INT		m_BlockCheckRes;
	GRID_DATA* m_pBufGrid;


public:
	void    SetBlockCheck(FVector CheckPos);
	UBOOL   GetBlockCheck(){return m_BlockCheck;};
	void    RunBlockCheck(AMMOActor* pBlockActor);
	void    DoBlockCheck();
public:
	UBOOL GameCaptureInput;
	INT		GetESUITypeByAbility( UMMOAbility *pAbility );
// 	void	LoadAbilityTemplates();

	// buff
	void AddBuffTemplate(INT Id, INT Level, class UBuff* BuffTemplate);
	void ClearBuffTemplates();
	class UBuff* FindBuffTemplate(INT Id, INT Levle);


	struct FDamageEffectPair* GetDamgeTemplateData(BYTE ResultType,BYTE WeaponHitType,BYTE DirectionType,BYTE ElementaryType);
	void AddDamgeEffectPair(BYTE ResultType,BYTE WeaponHitType,BYTE DirectionType,BYTE ElementaryType, struct FDamageEffectPair* InData);
	void ClearDamageTemplateData();

public:
	void Teleport( INT sceneID, const FVector& initPos, INT yaw);
	void CorrectTeleportPos();		// correct tele pos after scene loaded
	const FVector& GetInitTeleportPos() const { return _initTeleportPos; }

public:

	void RegisterTriggerVolume(AMMOVolume_Trigger* vol);
	void ClearAllTriggerVolume();
	AMMOVolume_Trigger* FindTriggerVolume(INT areaID) const;

	FVector TraceGroundPosition(APawn* pawn, const FVector& startPos);

	UBOOL CheckPlayerBlock(FVector &lastPos,FVector &nowPos, AMMOActor* pBaseActor);

	void RegisterTrigger(TScriptInterface<IIServerTrigger> trigger);
	TScriptInterface<IIServerTrigger> FindTrigger(INT serverID) const;

public:
	LONG64 GetCurrentTimeStamp();
	void UpdateTimeStamp();
	LONG64 GetCurrentTime();
	LONG64 GetLastUpdateTime();
	LONG64 GetGameStartTime();
	void SetGameStartTime();
	void SetLastUpdateTime(LONG64& t);
	FString TimeDisplay(INT seconds,INT precision = 6);

	inline void AddServerFrameCount()				{ ServerFrameCount++; }
	inline INT GetServerFrameCount()			{ return ServerFrameCount; }

	inline INT GetStartServerFrameCount()			{ return StartServerFrameCount; }
	inline void SetStartServerFrameCount(INT C)	
	{
		ServerFrameCount = StartServerFrameCount = C; 
		StartClientTime = (LONG64)appSeconds();
	}

	inline INT GetClientFrameCount()			{ return StartServerFrameCount + (GCurrentTime - StartClientTime) * GAME_FPS; }

	inline UINT32 GetGameWorldPingDelayTime()			{ return GameWorldPingDelayTime;}
	inline void	  SetGameWorldPingDelayTime(UINT32 nPing){GameWorldPingDelayTime = nPing; } 

	inline UINT32 GetGatePingDelayTime()			{ return GatePingDelayTime;}
	inline void	  SetGatePingDelayTime(UINT32 nPing){GatePingDelayTime = nPing; } 
	

	AbilityProcessor* GetAbilityProcessor();
	MMOBuffManager*   GetMMOBuffManager();
	STLInt2IntMap* GetFriendlyNpcMap(){  return &FriendlyNpcMap;}
	STLInt2IntMap* GetPlayerMap(){  return &PlayerMap;}
	STLInt2IntMap* GetCharacterMap(){  return &CharacterMap;}
	MountDataMap*  GetMountDataMap(){ return &_MountDataMap;}
	ReputationTemp*  GetReputationTemp(INT index);
	ReputationTemp*  GetReputationTempBySceneID(INT SceneID);
	ReputationTemp*  GetReputationTempByForceID(INT ForceID);
	INT				 GetCanAttackWithFreePkMode(INT ForceID);


	ShowPanelAdvancedData* GetShowPanelAdvancedData(const FString& mapName);
	void				   DelShowPanelAdvancedData(const FString& mapName);
	void				   SetShowPanelAdvancedData(const FString& mapName, ShowPanelAdvancedData& AdvancedData);	

	int							GetFriendlyNpcNum(){ return FriendlyNpcMap.size(); }
	void							StartFriendlyNpcFlash(){ FriendlyNpcMapPos = FriendlyNpcMap.begin(); }

	void EmptyMountList();
	void AddMountList(INT MountID);
	void RemoveMountList(INT MountID);
	void EquipMount(UINT CharID, INT MountID);
	void EquipWing(UINT CharID, INT WingID);
	void RaiseCharacterRemoveEvent();

	PlayerBaseInfo* RequestPlayerBaseInfo( INT roleID , BOOL forceRequest = FALSE );
	PlayerBaseInfo *GetPlayerBaseInfo( INT roleID );
	PlayerBaseInfo *AddPlayerBaseInfo( M2C_SYNC_ROLE_INFO &data );
	void RemovePlayerBaseInfo( INT roleID );
	INT GetPlayerBaseInfoByName(FString name);

	void SetMyCurrentHP(INT CurrentHP);
	INT GetMyCurrentHP() const;

	void SetMyCurrentMaxHP(INT CurrentHP);
	INT GetMyMaxHP() const;

	//检查玩家钱币是否足够
	UBOOL CheckEnoughMoney( EMONEY_TYPE moneyType , INT moneyValue );

	void NotifyMyHPChange();


	void SetMyCurrentMP(INT CurrentMP);
	INT GetMyCurrentMP() const;

	void SetMyMaxMP(INT CurrentMP);
	INT GetMyMaxMP() const;

	INT GetDeltaMaxMP(INT nClassID, INT ConstParam, INT catStrength, INT catAgility, INT catIntellect, INT catStamina, INT catSpirit, INT catValue, INT catCombatParam) const;

	void NotifyMyMPChange();
	void NotifyShutDown();


	void SetMyLevel(INT level);
	INT GetMyLevel();

	void SetMyPkValue(INT PkValue);
	INT GetMyPkValue();

	void SetMyPkMode(INT PkMode);
	INT GetMyPkMode();

	void RefleshAllHints();
	void RefleshAllNPCHints();

	UBOOL IsSamePkLevel(INT PkValue1,INT PkValue2);
	INT   GetPkValueLevel(INT PkValue);

	INT GetCurrentBagWgt();
	INT GetCurrentWeaponWgt();
	INT GetCurrentArmorWgt();

	void SetCurVolumeAndSaveAudioConfig();
	BOOL CheckWeight(INT itemWeight,INT lastItemWeight= 0,WeightType type = WT_Bag);
	void NotifyOptionChange();
	void NotifyPlayerLevelChange( INT lastLevel , INT newLevel );
	void NotifyPlayerBaseInfoChange();
	void NotifyPlayerPkModeChange();
	void NotifyOtherPlayerPkModeChange(AMMOPlayer* player, INT PkMode);
	void NotifyPlayerPkValueChange();
	void NotifyOtherPlayerPkValueChange(AMMOPlayer* player, INT PkValue);
	void NotifyPlayerTitleChange();
	void NotifyOtherPlayerTitleChange(AMMOPlayer* player , INT TitleID);

	void NotifyEnterCompact( UINT actorID );
	void NotifyLeaveCompact( UINT actorID );

	void NotifyBagSlotChange( INT BagId,INT SlotId );
	void NotifyItemDelete( INT ContainerID, INT SlotID );

	void PauseBackSound( UBOOL bPause );

	void SetPlayerAttribute(INT AtrrId, INT Value);
	INT GetPlayerAttribute(INT AtrrId);
	void NotifyPlayerAttributeChange(  INT AtrrId, INT Value );

	UBOOL TodayExpIsFull(){ return _todayExpIsFull; }
	void SetPlayerExp( INT CurrentExp,INT MaxExp ,INT todayExp , INT todayMaxExp, INT ChangeExp=0 );
	INT GetPlayerCurExp();
	INT GetPlayerMaxExp();
	void NotifyPlayerExpChange(  INT CurrentExp,INT MaxExp,INT ChangeExp );

	void SetPlayerMoney(G2C_SYNC_MONEY PlayerMoney);
	void SetPlayerMoney(G2C_MONEY_CHANGE PlayerMoneyChange);
	void SetPlayerMoney(EMONEY_TYPE MoneyType, INT64 Money);
	INT32 GetPlayerMoney(EMONEY_TYPE MoneyType);
	INT64 GetPlayerMoney64( EMONEY_TYPE MoneyType );
	INT32 GetPlayerMoney_Weekly( EMONEY_TYPE MoneyType );
	INT32 GetPlayerMoney_WeeklyLimit( EMONEY_TYPE MoneyType );

	INT32 GetInt32(INT64 nInValue);

	void RequestUpdateMoney();
	void NotifyPlayerMoneyChange( EMONEY_TYPE MoneyType, INT64  Money , INT64 OldMoney );

	UBOOL IsMainPlayerTrading();

	void SetMonsterHP( ANPC* npc , INT hp );
	void SetNPCHP ( ANPC* npc , INT hp );
	void SetNPCHP( ANPC* npc , INT hp, INT maxHp );
	void NotifyNPCHpChange( ANPC* npc , INT hp );

	void SetPlayerHP ( AMMOPlayer* player , INT hp );
	void SetPlayerHP ( AMMOPlayer* player , INT hp , INT maxHp);
	void SetPlayerMP ( AMMOPlayer* player , INT mp );
	void NotifyOtherPlayerHpChange( AMMOPlayer* player , INT hp );
	void NotifyOtherPlayerMpChange( AMMOPlayer* player , INT mp );

	//void NotifyUpdateMyTeam();
	void NotifyUpdateTeamMate( INT UIPos , INT Pos );
	//void NotifyChatMessage( INT ChannelId , INT TypeId , FString MsgContent,FString SenderName );
	void SetTeamMemberHP(  INT MemberServeId ,  INT HP ,INT MAXHP , INT hasPos = -1 /*已知队伍位置就不查找了*/);
	void SetTeamMemberMP(  INT MemberServeId ,  INT MP , INT MAXMP , INT hasPos = -1 /*已知队伍位置就不查找了*/ );
	void SetTeamMemberLv(  INT MemberServeId ,  INT Lv , INT hasPos = -1 /*已知队伍位置就不查找了*/  );
	void SetTeamMemberOnline(  INT MemberServeId ,  BOOL Online , INT hasPos = -1 /*已知队伍位置就不查找了*/ );

	//void NotifyUpdateMyFriend();
	//void NotifyUpdateFriendItemValue(INT GroupID,INT pos,const FString& PlayerName,INT Lv,const FString& Carrer,const FString& Location,const FString& Sign,INT RoleId,INT Online);
	//void NotifyUpdateFriendGroup(INT GroupId);
	//void NotifyAddedAttentionBy(INT RoleId , FString RoleName );
	void NotifyFriendChatMsg( INT RoleId , FString Content );
	void NotifyUnReadFriendChatMsgChange();

	void NotifyAddNewNpcData( ANPC* Npc );
	void NotifyUpdateTeamContent( );
	//void NotifyUpdateAllTeamMate();
	//void NotifyUpdateTeamMateNum( INT Num );

	void NotifyEnterScene();
	void NotifyChangeScene();
	void NotifyEnterLogin();
	void NotifyEnterGame();
	void NotifyHUDLoaded();



	//枚举字符转换
	FString ClientEnum2ServerEnum(const FString& ClientEnum, INT EscapeNum = 2);
	//通过服务器枚举字符获得客户端对应枚举字符
	FString FindClientEnumString( const FString& ServerEnum, const TCHAR* SerEnumType, INT EscapeNum ) ;
	//通过服务器枚举字符获得客户端对应枚举值
	INT		FindClientEnumValue( const FString& ServerEnum, const TCHAR* SerEnumType ,INT EscapeNum = 2 );
	//通过枚举值获取枚举字符串，
	FString FindServerEnumString( INT ServerEnumID, const TCHAR* SerEnumType );
	//通过服务器枚举字符获得客户端对应的枚举字符
	FString FindClientAttrName(const FString& ServerEnum, const TCHAR* SerEnumType);
	//客户端自己使用的字符串转枚举值
	INT		ClientStrToEnum( const FString& ClientEnum, const TCHAR* ClientEnumType);

	//读取称号列表
	UBOOL ReadTitle();
	//读取头顶文字配置列表
	UBOOL ReadHint();
	//读取Tween列表
	UBOOL ReadTweenData();
	//读取掉落轨迹表
	UBOOL ReadTestDropTrace();

	//读取mount列表
	UBOOL ReadMountData();
	UBOOL ReadWingData();
	UBOOL RefreshMountData();
	UBOOL ReadReputation();
	UBOOL ReadReputationTemp();

	void  ReadHintNode(HintConfig& NpcHint, TArray<FString>& ArrayNodeName );
	void  ReadHintNode(HintConfig& NpcHint, FString NpcTypeName, INT NameVisible, INT StatusVisible, INT IconVisible, INT HPVisible );
	HintConfig*  GetHintInfo(EActorClassType ActorType,ENPC_QUALITY quality );

	//读取pk值分级别列表
	UBOOL ReadPkLevel();
	//读取势力声望表
	UBOOL ReadForceRelative();
	//读取屏蔽字列表
	UBOOL ReadCensoredWordList(CensoredWordType type = CensoredWord_All);
	//获取一个技能的下一级
	UMMOAbility* GetAbilityByNextLevel( UMMOAbility *ability );
	UMMOAbility* GetAbilityWithRuneByNextLevel( UMMOAbility *ability );
	//读取随机属性
	UBOOL ReadRandomAttribute();

	UBOOL ReadAttribute2Config();

	UBOOL ReadAttributeConfig();

	//power.txts
	UBOOL ReadPowerConfig();

	UBOOL LoadStringMap();
	//获得5个基础属性
	INT   GetAttributeConfigData(INT nLevel,INT nClass,INT AttributeType);
	//获得2级属性的基础值
	INT	  GetAttribute2ConfigData( INT nLevel,INT nClass,INT AttributeType );
	Attribute2	  GetAttribute2ConfigStructData( INT nClass,INT AttributeType );
	//
	INT GetLevelPow( INT lv );
	INT GetLevelExp( INT lv);
	INT GetSpiritPow( INT lv );

	//获取某个ID的title信息
	void	   ResetTitleData();
	TitleData* GetTitleData(INT ID);
	TitleData* GetTitleDataArray(INT Index);
	INT		   GetTitleDataArrayCount();
	UBOOL	   UpdateTitleArray();
	MountData* GetMountData(INT ID);
	FString GetWingAvatarName(INT ID);
	//获取某个tween信息
	TweenData* GetTweenData(FString stringID);
	//获取某个ForceID的声望值
	INT   GetPlayerReputation(const AMMOPlayer* player, INT ForceID);
	void  SetPlayerReputation(AMMOPlayer* player, INT Index,INT ForceID, INT newReputation, INT lastReputation);
	//获取某个ForceID的激活值
	UBOOL  GetPlayerActiveState(const AMMOPlayer* player, INT ForceID);
	void  SetPlayerActiveState(AMMOPlayer* player, INT Index,INT ForceID, BOOL ActiveState);
	UBOOL  GetPlayerAwardState(const AMMOPlayer* player, INT ForceID);
	void  SetPlayerAwardState(AMMOPlayer* player, INT Index,INT ForceID, BOOL AwardState);
	UBOOL  GetPlayerAwardDisplay(const AMMOPlayer* player, INT ForceID);


	////获取两个ForceID的敌对关系
	

	UBOOL IsPlayerAttackable(const AMMOPlayer * Player1, const AMMOPlayer* Player2);
	UBOOL IsAttackable( const AMMOActor * target );
	UBOOL IsAttackable( const AMMOActor * Actor1, const AMMOActor* Actor2 );
	UBOOL IsTradeable( const AMMOPlayer* player1, const AMMOPlayer* player2 );
	UBOOL IsAbilityCanUse( UMMOAbility *toUseAbility );

	//可用的铭文技能
	UMMOAbility* FindMyUsableSkill( INT mainSkillID );

	//副本相关
	FBSummaryData* GetFBSummary(){ 
		return &_FBSummary; 
	}
	//

	//chain pay 连带支付
	void SetChainPay( UBOOL open );
	UBOOL GetChainPay(){ return _chainPay; }

	void SetFBSummary( const G2C_DUNGEON_FINISH_DATA &data );
	void ResetFBSummary();
	FBMemberAwardData* SetFBOtherScore(const G2C_DUNGEON_FINISH_SCORE &data);
	FBMemberAwardData* SetFBAward( const G2C_DUNGEON_REWARD &data );
	void ResetFBAward();
	//第一项为自己，即iIdx为0时
	FBMemberAwardData* GetFBAward( INT iIdx );
	FBMemberAwardData* GetFBAwardByRoleID( UINT uiIdx );

	//
	UBOOL PlayerMoveToScenePos( FExportMap &toMoveSceneInfo, FVector world3DPos );

	//读取二维config，返回二维FString
	static UBOOL ReadConfigString( class CTabFile*& outTabFile, const TCHAR* ConfigName, bool needEmpty = false , bool delQuotate = false);
	static class CTabFile* ReadTabFile( const TCHAR* ConfigName);
	static INT   GetColumnIndex(TArray<FString>& ArrayString, FString KeyName);

	void SetIsRepairing( UBOOL repairing ){ _isRepairing = repairing; }
	UBOOL GetIsRepairing(){ return _isRepairing; }

	void SetIsSpecialRepairing( UBOOL repairing ){ _isSpecialRepairing = repairing; }
	UBOOL GetIsSpecialRepairing(){ return _isSpecialRepairing; }

	//自动解包
	void SendOpenPackage(INT itemId);
private:
	void TickTitleTime(FLOAT deltaTime);

	UBOOL _isRepairing;
	UBOOL _isSpecialRepairing;

	UBOOL _chainPay;

	//读取单段文字
	//UBOOL ReadStrNode(TCHAR* pData,INT &nStart,TCHAR* OutStr,UBOOL* bReturn);
	//读取正行 确保当前行被读完
	//UBOOL ReadLine(TCHAR* pData,INT &nStart );


	//记录当前的游戏名字
	static  void IniMMOAppName();
	//当前程序名字 
	static  FString mMMOAppName;

public:
	static FString GetMMOAppName();
	void SetDoorStates( std::bitset<64> tState );
	void UpdateDoors();

	void SetContainerSize(UINT8 inContainerSize[CONSLOT_COUNT]);
	void SetContainerSize(ECONTAINER_SLOT ContainerID, UINT16 ConSize);
	UINT16 GetContainerSize(ECONTAINER_SLOT ContainerID) const;

	ItemsMap& GetContainer(ECONTAINER_SLOT containerID);
	const ItemsMap& GetContainer(ECONTAINER_SLOT containerID) const;

	void SetPlayerInitPosition(const FVector& pos) { _initTeleportPos = pos; }

	//时间
	INT32 GetClientToSceneSecondDiff(){ return ClientToServerSecDiff; }
	void SetEnterSceneServerTime( UINT32 serverTime );
	UINT32 GetEnterSceneServerTime(){ return EnterSceneServerUtcTime; }
	SYSTEMTIME GetRealClientTime();
	UINT32 GetRealClientUTCTime();
	_SYSTEMTIME ConvertUtcSecondToSystemTime( UINT32 tmtTime );
	UINT32 ConvertSystemTimeToTmTSecond( _SYSTEMTIME systemTime );

	void ClearMainController()
	{
		MainController = 0;
	}
	/*
	正常游戏态的帧率 
	*/
	void   AddNewGameStateFPSData(INT nFps);
	FLOAT  GetGameSateFps();
	void   ResetGameSateFps();

	//
	void GameVerAnnounceProcess();

	BOOL GetPlayerSort();
	void SetPlayerSortType(ItemSortType v){playerSortType = v;}
	ItemSortType GetPlayerSortType(){return playerSortType;}

protected:
	std::bitset<64> mDoorStates;
	AbilityProcessor* AbilityProcessorPtr;
	MMOBuffManager* MMOBuffManagerPtr;
	//MMOBattleManager *BattleManager;
	//MMOPlayerControllerAI* MainControllerAI;
	AMMOPlayerController* MainController;

	// 所有服务器统一编号的对象,包含玩家,NPC,实体子弹
	STLInt2IntMap FriendlyNpcMap;// 系统NPC常需要遍历，在任务
	STLInt2IntMap::iterator FriendlyNpcMapPos;
	STLInt2IntMap CharacterMap;
	STLInt2IntMap PlayerMap;// 

	std::list<INT, M_UE3_STL_ALLOC(INT)> ObjectsToSync;
	DWORD LastSyncTickCount;
	FVector MainPlayerLocation;
	static const INT     NearbyNum = 8;
	FLOAT   NearbyDis[8];
	INT		NearbyId[8];

	typedef TMap< INT, PlayerBaseInfo > PlayerBaseInfoMap;
	PlayerBaseInfoMap _PlayerBaseInfoMap;
	typedef TMap< INT, INT > PlayerBaseInfoRequestInfoMap;
	PlayerBaseInfoRequestInfoMap _PlayerBaseInfoRequestBuffMap;
	FLOAT PlayerInfoRequestTime;
	void TickPlayerInfoRequestTime( FLOAT deltaTime );

	UBOOL _todayExpIsFull;

	//纯Client的Contain也需要加上
	//用于传入的实际Item数据,如摊位中一个其他玩家的强化武器
	UINT8 ContainerSize[E_CONSLOT_MAX]; 
	ItemsMap _Containers[E_CONSLOT_MAX];

	//AffixTemplateMap _affixTemplateMap;
	//MapAffixID2Data _mapAffixs;

	NPCTemplateMap _npcTemplateMap;

	friend class MMODropManager;
	ItemTemplateMap _itemTemplateMap;
	TArray<ItemInfo*> _SoldItemList;
	//ShopTemplateMap _shopTemplateMap;
	TitleDataMap	_TitleDataMap;
	TArray<TitleData> _TitleDataArray;

	TweenDataMap	_TweenDataMap;
	MountDataMap	_MountDataMap;
	WingDataMap		_WingDataMap;

	ShowPanelAdvancedDataMap _ShowPanelAdvancedDataMap;

	TArray<ReputationTemp> _ReputationTempArray;
	INT	_CanAttackWithFreePkModeArray[MAX_NPC_FORCE_ID];

	// 传送过地图之后的初始位置
	FVector _initTeleportPos;
	// 副本相关
	FBSummaryData _FBSummary;
	//typedef std::vector<FBMemberAwardData> FBMemberAwardTemplateVec;
	//FBMemberAwardTemplateVec _FBMemberAward;
	FBMemberAwardData _FBMemberAward[5];
	// end
	typedef std::map<INT, AMMOVolume_Trigger*, M_UE3_STL_ALLOC_MAP(INT, AMMOVolume_Trigger*) > TriggerVolumeMap;
	TriggerVolumeMap _triggerVolumeMap;

	typedef std::map< INT, TScriptInterface<IIServerTrigger>, M_UE3_STL_ALLOC_MAP(UINT32, TScriptInterface<IIServerTrigger>) > TriggerObjectMap;
	TriggerObjectMap _triggerObjectMap;

	// sound map key: 00 type maker target
	typedef std::map<INT, FSoundParticleEffect*, M_UE3_STL_ALLOC_MAP(INT, FSoundParticleEffect*) > Int2SoundParticleMap;
	Int2SoundParticleMap SoundParticleMap;

	typedef std::map<INT, USoundCue*, M_UE3_STL_ALLOC_MAP(INT, USoundCue*) > HitSoundGroupMap;
	HitSoundGroupMap HitSoundGrpMap;

	typedef std::map<INT, FAnimationNode*, M_UE3_STL_ALLOC_MAP(INT, FAnimationNode*) > AnimMixGruopMap;
	AnimMixGruopMap AnimMixGrpMap;

	typedef std::map<INT, class UMissile*, M_UE3_STL_ALLOC_MAP(INT, class UMissile*) > MissileTemplateMap;
	MissileTemplateMap MissileTemplates;

	typedef std::map<INT, class UBuff*, M_UE3_STL_ALLOC_MAP(INT, class UBuff*) > Int2BuffMap;
	Int2BuffMap BuffTemplateMap;

	typedef std::map<UINT, struct FDamageEffectPair*, M_UE3_STL_ALLOC_MAP(UINT, struct FDamageEffectPair*) > Int2DamageEffectPairMap;
	Int2DamageEffectPairMap DamageTemplateData;

	INT	_sceneLevel;
	INT _sceneDifficulty;

	// 时间相关
	LONG64 GameStartTime;
	LONG64 LastUpdateTime;
	INT ServerFrameCount;
	INT StartServerFrameCount;
	LONG64 StartClientTime;
	
	UINT32 GameWorldPingDelayTime;
	UINT32 GatePingDelayTime;

	/*
	 InGameStateFps: 统计一个正常游戏中的帧率( loading时  最小化  不在前台时不计算在内)
	 InGameCalTimes: 游戏中求均值得次数
	*/
	FLOAT  InGameStateFps;
	INT    InGameCalTimes;

	UINT32 EnterSceneServerUtcTime;
	INT32 ClientToServerSecDiff;
	
	//加载相关
	UBOOL	LastInGameScene;
	// teleports

	class UMMOUIGFXPlayer* UIHudManager;

	INT ForceRelative[MAX_NPC_FORCE_ID][MAX_NPC_FORCE_ID];
	INT AttributeList[MAX_ROLE_LEVEL*clsPlayerEnd][catMaxMp+1];

	Player_Money _PlayerMoney;

	// UI Sound
	typedef std::map<FString, USoundCue*, ComparFString, STLAllocator< std::pair<FString, USoundCue*>, UE3STLAllocatorPolicy> > UISoundMap;
	UISoundMap _UISoundMap;

	//背包排序
	ItemSortType playerSortType;
	
protected:

	FMatrix		MatViewProjOfLocalPlayer;

	FSceneView* SceneView;
	UCanvas*	Canvas;
	FMatrix		ViewProjectionMatrix;
	INT			ScreenX;
	INT			ScreenY;

	//---- perf test ----
	INT			PlayerCount;
	INT			NPCCount;
public:
	INT			GetPlayerCount() const
	{
		return PlayerCount;
	}

	INT GetNPCCount() const
	{
		return NPCCount;
	}
	//---- perf test ----

public:

	// 	void SetRepairState( UBOOL bRepairing );
	// 	UBOOL GetRepairState();
	// 	void ChangeRepairState( );

	void SetCanvas( UCanvas* cvs );
	FSceneView* GetSceneView();
	UCanvas* GetCanvas();
	FMatrix	GetWorldView();
	void  UpdateScreenPixelFromWorld();
	UBOOL GetScreenPixelFromWorld(FVector WorldPos,FVector2D& OutPos);
	//void ClearSceneView(); // commentted by zhaojun@snda

	static FVector2D GetScreenSize();

protected:

	FLOAT _SortTimer;
	INT _SortTimerCounter;
	bool _UpdateSortTimer;
	LockSortMode _LockSortMode;

public:

	void StartSortTimer( LockSortMode sortMode );
	LockSortMode GetLockSortMode(){ return _LockSortMode; }

	// indicate that the player position setting is delaied after asnyc loading
	UBOOL	DelayPositionSetting;
};

class MMOUI
{
public:

static FString FormatToGFXImage(UTexture2D* tex);

//add by JimZhang@snda
static void LoadMoviePlayers(class AMMOHud* HUD,FName PlayerClassString);
static void CloseMoviePlayersByName(FName PlayerName);
static void CloseMoviePlayers(class UMMOGFXPlayer* Player);

static void LoadAllMoviePlayers(class AMMOHud* HUD, class ULocalPlayer* LocPlay);
static void RegisterUIPanel(class UMMOUIBase* UI, FName FrindlyName);
static void RegisterMoviePlayer(class UMMOGFXPlayer* UI, FName FrindlyName);
static class UMMOUIBase* FindUIPanel(FName FrindlyName);
static class UMMOUIBase* FindUIPanelEx(FName panelName);
static UBOOL GetPanelByString( FString inName, UMMOUIBase **InPtr );
static class UMMOGFXPlayer* FindMoviePlayer(FName PanelName);
static void RemoveMoviePlayer(class UMMOGFXPlayer* player);

static void SetPanelToMoviePlayer(FName PanelName, FName MoverPlayerName);
static FName FindMoviePlayerName(FName PanelName);

static void ShowAllPanels(UBOOL show, UBOOL bRecover=FALSE);
static void GetEquippedSlotByEquipType(INT EquipType, INT& tConSlot, INT& tItemSlot);
static class UMMOUIManager* GetUIManager();
static void CloseAllPanels( UBOOL _IsCleanClose );
static void StartAllPanels( );
static void ShowPatternPanel(class UMMOUIBase* RefPanel, class UMMOUIBase* PatternPanel);

static void ShowInGameUI(bool show);

static void OnFocusGained(class UMMOUIBase* Panel);
static void OnFocusLost(class UMMOUIBase* Panel);
//static void OnFocusClose(class UMMOUIBase* Panel);
static void SetOptimizationFocus(class UMMOUIBase* Panel);

static void MoveToTop(class UMMOUIBase* Panel);
static void RemoveTop(class UMMOUIBase* Panel);
static class UMMOUIBase* GetTopmostPanel();
static class UMMOUIBase* GetFocusPanel();
static class UMMOUIBaseSlotContainer* GetTopmostSlotContainer();


static FString ParseLocalizedPropertyPath(const FString& PathName);
static FString ParseLocalizedPropertyPath(const FString& PathName, UBOOL& Res);
static FString FindSoundNameByLocalString(const FString& PathName);
static INT FindMsgTypeByLocalString(const FString& PathName);
static INT GetStringType( const FString& language );

static FString TrimNumber(const FString& srcStr);

static void ParseChatMsg(FString& srcStr, FString& srcOri, CensoredWordType type);
static INT ReplaceCensoredWord(FString& srcStr,CensoredWordType type = CensoredWord_All);
static bool HasCensoredWord(const FString& srcStr);
static void ClearCensoredWord(CensoredWordType type = CensoredWord_All);
static void AddCensoredWord(FString& srcStr,CensoredWordType type = CensoredWord_All);
static void BuildSortList(CensoredWordType type = CensoredWord_All);

// add by zhangrui05
static bool HasSpecialSymbol(const FString& srcStr);
static void FormatPlayerSendChatMessage(FString& srcStr);
static void FixItemColor(FString& srcStr);
UBOOL ReadAttribute2Config();

// game events
static void RegisterEventHandler(class INativeGameEventHandler* handler, EGameEvent requiredEventType);
static void PostGameEvent(struct MMOGameEvent* evt);

static void UnRegisterEventHandler(class INativeGameEventHandler* handler, EGameEvent requiredEventType);

#if WITH_EDITOR
static FString GetPropertyDisplayName(UProperty* prop);
#endif

static FString ConvertServerEnumToClientEnum(const FString& serverEnum, UEnum* clientEnumCls = NULL);
}; // namespace MMOUI

FString GetErrorCodeEnumString(INT idx);

FString GetPlayerRoleName();
INT	GetPlayerRoleID();


#endif
