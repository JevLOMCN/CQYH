#ifndef _UITEXT_PARSER_H_
#define _UITEXT_PARSER_H_

#include "MMOSingleton.h"
#include "IDialogOptionAction.h"
#include "MMOUtil.h"
#include "UStallManager.h"

#define PARSE_MAX_PARAMETERS 4
struct ContextParameter
{
	bool IsInUse;
	FString ParameterString;
};

struct ParseContext
{
	ANPC* NPC;

	//now there are three parameters
	/*bool HasParameter0;
	FString Parameter0; // special usage;*/
	ContextParameter Parameters[PARSE_MAX_PARAMETERS];
	bool SkipAppendNPCDialogOption;

	ParseContext()
		: SkipAppendNPCDialogOption(false)
	{
		NPC = 0;
		//HasParameter0 = false;
		for(INT i = 0; i < PARSE_MAX_PARAMETERS; ++i)
		{
			Parameters[i].IsInUse = false;
		}
	}

	INT GetParamCount() const
	{
		for(INT i = 0; i < PARSE_MAX_PARAMETERS; ++i)
		{
			if(Parameters[i].IsInUse == false)
			{
				return i;
			}
		}

		return PARSE_MAX_PARAMETERS;
	}
};

struct FGuardSkipAppendNPCDialogOption
{
	ParseContext* Contex;
	bool SkipAppendNPCDialogOption;
	FGuardSkipAppendNPCDialogOption(ParseContext* ctx, bool inSkipAppendNPCDialogOption)
		: Contex(ctx)
	{
		SkipAppendNPCDialogOption = ctx->SkipAppendNPCDialogOption;
		ctx->SkipAppendNPCDialogOption = inSkipAppendNPCDialogOption;
	}

	~FGuardSkipAppendNPCDialogOption()
	{
		Contex->SkipAppendNPCDialogOption = SkipAppendNPCDialogOption;
	}
};

struct ItemInfo;
struct ServerShopItemData;

struct DialogContext
{
	TArray<FString> DialogText;
	INT			TextId;
	FString TempText;
	FString AnswerText;
	FString PlayerName;
	INT			QuestId;
	INT ClientActionAutoID;

	typedef std::shared_ptr<IDialogOptionAction> IDialogOptionActionPtr;
	typedef std::map<INT, IDialogOptionActionPtr, M_UE3_STL_ALLOC_MAP_GRT(INT, IDialogOptionActionPtr) > DialogOptionActionMap;
	DialogOptionActionMap OptionActionMap;

	DialogContext()
	{		
		DialogText.Empty();		
		TempText.Empty();
		AnswerText.Empty();
		PlayerName.Empty();
		TextId = -1;
		QuestId = -1;
		ClientActionAutoID = -1;
	}

	void Clear()
	{
		DialogText.Empty();
		TempText.Empty();
		AnswerText.Empty();
		PlayerName.Empty();
		OptionActionMap.clear();
	}

	// client's own option text uses negative values
	void AdvanceClientActionID()
	{
		--ClientActionAutoID;
	}

// 	void Add( DialogContext& diagCtx)
// 	{
// 		//DialogText += DC.DialogText;
// 		for ( INT i = 0 ; i < DC.DialogText.Num() ; i++ )
// 		{
// 			if( !DC.DialogText(i).IsEmpty() )
// 			{
// 				DialogText.AddItem(DC.DialogText(i));
// 			}
// 		}
// 
// 		if( TextId < 0)
// 			TextId = DC.TextId;
// 
// 		if( QuestId < 0)
// 			QuestId = DC.QuestId;
// 
// 		AnswerText += DC.AnswerText;
// 		
// 		OptionActionMap.insert(DC.OptionActionMap.begin(),
// 			DC.OptionActionMap.end());
// 	}

	void ExecuteOption(INT OptionID);
};
//------------------------------------------------------------------------
class ITextParser
{
public:

	static const FString StartTag;
	static const FString EndTag;

	virtual FString GetKeyword() const = 0;
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen ) = 0;
	virtual FString ParseContent(const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen);

	static INT ParseID(const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen);
	static void ParseNPC( const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen, INT& id, INT& mapId);
	static void ParseNavPoint( const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen, FString& pointName, INT& mapId);
	static void ParseDoodad( const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen, FString& pointName, INT& doodadId, INT& mapId);
	static FString ParseStringID(const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen,FString* pOutStr=NULL);
	static void ParseCloseTag(const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen, const FString& keyWord);
	static void AddParseParameter(ParseContext& ctx, const FString& params);

	static FString ParseStringAsNewWord(const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen,FString* pOutStr=NULL);
};

//------------------------------------------------------------------------
class UITextParser : public Singleton<UITextParser>
{
public:

	UITextParser(void);
	virtual ~UITextParser(void);

	void Parse(ParseContext& ctx, DialogContext& diagCtx, const FString& srcStr ,const FString& matchString= TEXT("") );

	// add by zhangrui05@snda.com
	void ParseContent(ParseContext& ctx, DialogContext& diagCtx, const FString& srcStr ,const FString& matchString= TEXT("") );

    //BaseFunc
	static FString HTMLNewLine();
	static FString HTMLBeginLink(const FString& linkParameters);
	static FString HTMLEndLink();
	static FString HTMLBeginFont(const FString& fntClr, INT fntSz = 13);
	static FString HTMLEndFont();
	static FString HTMLGetItemColor(EItemColor i);
    static INT     HTMLGetItemRealColor( EItemColor i );
	static FString HTMLBeginParagraph(const FString& alignMode = TEXT("left"));
	static FString HTMLEndParagraph();

	//get tips color
	static FString GetTipsColor(const TCHAR* key = NULL);
	static FString GetNormalColor( const TCHAR* key = NULL);

    //LogicBaseFunc
    static int     GetEnumIdByTypeName( FString name );
    static FString HTMLGetCurrencyColor(MONEY_TYPE i);
    static FString HTMLGetCurrencyName(MONEY_TYPE i);
    static FString HTMLFormatCurrency(MONEY_TYPE i, int cnt);
	static FString HTMLMoneyIconString( INT moneyType , INT64 moneyValue, FString moneyColor=TEXT("") );
	static FString CvtNumToChnNum( UINT64 uiNum );
	static FString GetMoneyIconImgAndName( FString &moneyName,INT moneyType );
    static FString HTMLFormatDummyLongText(const FString& originText);
    static FString HTMLFormatPlayerName(const FString& playerName);
	static FString HTMLFormatMyName(const FString& playerName);
	static FString HTMLFormatGuildName(const FString& playerName);
    static FString HTMLFormatQuestName(const UQuest* quest, bool withColor = true, bool withPrefix = true, bool withType = true);
	static FString HTMLFormatDialogOptionName(const FString& optionCommand, const FString& optionName);

    //ItemBase
    static FString HTMLFormatItemNameWithNoColor( UItem* item );
	static FString HTMLFormatItemName( UItem* item );
	static FString HTMLFormatItemType( UItem* item );
    //static FString HTMLFormatItemNamePrefix( UItem* item, ItemInfo* itemInfo );
	static FString HTMLFormatItemPrefixLevel( UItem* item );
	static FString HTMLFormatItemHeader( UItem* item, ItemInfo* itemInfo = 0 );
	static FString HTMLFormatBaseAttribute( UItem* item, ItemInfo* = 0 );
	static FString HTMLFormatItemDescription( UItem* item );
	static FString HTMLFormatItemGottenType( UItem* item );
	static FString HTMLFormatItemAttributes( UEquip* item, ItemInfo* equipInfo = NULL );
    static FString HTMLFormatItemRequireLevel( UItem* item );
	static FString HTMLFormatItemRequireAll( UItem* item );
	static FString HTMLFormatItemLevel( UItem* item );
    static FString HTMLFormatItemRequireProfession( UItem* item );
    static FString HTMLFormatItemDuration( UItem* item , ItemInfo* itemInfo );
	static FString HTMLFormatItemForbidSell(UItem* item , ItemInfo* itemInfo );
	//static FString HTMLFormatItemForbidTrade(UItem* item);
    static FString HTMLFormatItemMaterial( UItem* item );
    static FString HTMLFormatItemBindType( UItem* item , ItemInfo* itemInfo );
	static FString HTMLFormatItemPower(ItemInfo* pItemInfo);
	static FString HTMLFormatItemPower(UEquip* pEquip);
	static FString HTMLFormatItemGender(UItem* item);
	static FString HTMLFormatItemPrice(UItem* item, ItemInfo *itemInfo);
	static FString HTMLFormatStallItemPrice(const StallItemInfo& stallItemInfo);
	static FString HTMLFormatItemRequireReputation( const ServerShopItemData* shopItem );
	static FString HTMLFormatItemRequireGuild( const ServerShopItemData* shopItem );
	static FString HTMLFormatItemWeightAttribute( UItem* item );
	static FString HTMLFormatItemLevelUpPoints( ItemInfo* itemInfo );
	static FString HTMLFormatItemPurityAttribute( UItem* item );
    //Equip dynamic adjustment
    static FString HTMLFormatItemRandomAttribute( UEquip* item, ItemInfo* itemInfo = NULL );
	static void HTMLFormatItemBaseAndRandomAttributes( UEquip* equip, ItemInfo* itemInfo, FString &mixParaText, FString &randParaText );
    static FString HTMLFormatItemLuckyAttribute( UEquip* item, ItemInfo* itemInfo = NULL );
    static FString HTMLFormatItemEnhanceAttribute( UEquip* item, ItemInfo* itemInfo );
	static FString HTMLFormatEquipRepairState( UEquip* equip, ItemInfo* itemInfo = NULL );
	static BOOL HTMLFormatItemWeaponSkillAttribute(UEquip* item, ItemInfo* itemInfo, UGFxObject* skillName, UGFxObject* skillDesc, UGFxObject* skillIconName);
	static BOOL HTMLFormatItemSoulAttribute(UItem* item, ItemInfo* itemInfo,
		UGFxObject* soulName, UGFxObject* soulIconType, UGFxObject* soulIconName);
	static FString HTMLFormatEquipSuitAttribute( UEquip* item );
	static FString HTMLFormatEquipSignAttribute( ItemInfo* itemInfo = NULL);
	static FString HTMLFormatEquipWeatherAttribute(UEquip* item);
	static FString HTMLFormatItemDecompose(UItem* item);

    //Skill
	static INT	   HTMLFormatCostText(BYTE costType, INT costValue, FString& outCostActionText, FString& outCostNameText);
	static FString HTMLFormatWeaponTypesText(const FSpell_WeaponTypeL* typesL, const FSpell_WeaponTypeR* typesR);
	static FString FormatSpellWeaponLimitText(const FSpell_WeaponTypeL* typesL, const FSpell_WeaponTypeR* typesR, INT &canUseWeaponCnt, int &singleLimt);
	static FString HTMLFormatSkillTipCost( UMMOAbility* skill );
    static FString HTMLFormatSkillTipsUnlock( UMMOAbility* skill );
	static FString HTMLFormatSkillTipsDesc( UMMOAbility* skill , UMMOAbility* originSkill);
	static FString HTMLFormatSkillTipsChangeSpecility( FString skillSp , FString originSkillSp );
	static FString HTMLFormatSkillTipsCombineLv( UMMOAbility* skill, UMMOAbility* mainSkill, UMMOAbility* originSkill );
	static FString HTMLFormatSkillTipsSkillLv( UMMOAbility* skill , UMMOAbility* originSkill = NULL);
	static FString HTMLFormatSkillTipsRuneLv( UMMOAbility* skill , UMMOAbility* originSkill = NULL);
    static FString HTMLFormatSkillTipsDamage( UMMOAbility* skill );
    static FString HTMLFormatSkillTipsNextNeedMaxProgress( UMMOAbility* skill );
	static FString HTMLFormatSkillTipsNextNeedLevel( UMMOAbility* skill );
    static FString HTMLFormatSkillTipsLimit( UMMOAbility* skill );
	static FString FormatSpellPlayerClassLimitText(const FSpell_ClassType &spellClassLimit);
    //Buf
	static FString HTMLFormatBuffTips( UBuff* pBuff  );
	static FString HTMLFormatObject( UGFxObject* object );
    //Rune
    static FString HTMLFormatRuneItemTips(INT ActiveID, INT RuneSlot );

	//Message
	static FString HTMLFormatMessageSkillColor( UMMOAbility* skill );
	static FString HTMLFormatMessageTitleColor( struct TitleData* title );
	//apply color by item quality
	static FString HTMLFormatItemColorString( UItem* item );
	INT			   SplitStringTagWithUTF8MaxLength(const FString& strSrc,FString& strSplit,INT nLength);
	INT			   UTF8Length(const FString& strSrc,INT nMaxLen = 0);
	INT			   UTF8LengthMaxPosModify(const FString& strSrc,INT nMaxPos);

	//price conversion
	static FString ConvertPrice(INT64 money);
	static FString RightAlignTabs;
	static FString DummyText;
protected:

	typedef std::map<FString, ITextParser*, ComparFString, STLAllocator<std::pair<FString, ITextParser*>, UE3STLAllocatorPolicy> > ParserMap;
	ParserMap TextParserMap;
	ITextParser* GetParser(const FString& keyword);
};

//------------------------------------------------------------------------
class UITextParser_DefaultDialog : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("DFT"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//------------------------------------------------------------------------
class UITextParser_NPC : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("N"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
	virtual FString ParseContent(const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen);
};

class UITextParser_NpcMove : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("NM"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
	virtual FString ParseContent(const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen);
};

class UITextParser_NpcArrow : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("NA"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
	virtual FString ParseContent(const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen);
};

//------------------------------------------------------------------------
class UITextParser_Item : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("I"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};
//
class UITextParser_ItemLink : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("U"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//------------------------------------------------------------------------
class UITextParser_FromStringMap : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("T"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};
//------------------------------------------------------------------------
class UITextParser_Player : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("P"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//------------------------------------------------------------------------
class UITextParser_DialogText : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("D"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//------------------------------------------------------------------------
class UITextParser_Quest : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("Q"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

class UITextParser_DialogOption : public UITextParser_DialogText
{
public:
	virtual FString GetKeyword() const { return TEXT("O"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

class UITextParser_DialogOptionServer : public UITextParser_DialogText
{
public:
	virtual FString GetKeyword() const { return TEXT("SO"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

class UITextParser_OpenUI : public UITextParser_DialogText
{
public:
	virtual FString GetKeyword() const { return TEXT("UI"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//------------------------------------------------------------------------
class UITextParser_SelfName : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("SelfName"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//------------------------------------------------------------------------
class UITextParser_PlayerName : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("PN"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//------------------------------------------------------------------------
class UITextParser_GuildName : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("GN"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//------------------------------------------------------------------------
class UITextParser_Paramater0 : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("P0"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//now UITextParser_CommonParameter is enough, but the server side has not been changed and still uses p0,p1,p2, so add it for now
class UITextParser_Paramater1 : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("P1"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//------------------------------------------------------------------------
class UITextParser_Paramater2 : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("P2"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//------------------------------------------------------------------------
class UITextParser_Paramater3 : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("P3"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen);
};

//------------------------------------------------------------------------
class UITextParser_CommonParameter : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("PC"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//------------------------------------------------------------------------
class UITextParser_EquipSlot : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("E"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//------------------------------------------------------------------------
class UITextParser_EquipSlotLevel : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("EL"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//------------------------------------------------------------------------
class UITextParser_WeaponRuneID : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("R"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//------------------------------------------------------------------------
class UITextParser_Font : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("F"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//------------------------------------------------------------------------
class UITextParser_NavPointName : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("Nav"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

class UITextParser_NavPointNameMove : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("NavM"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//------------------------------------------------------------------------
class UITextParser_DoodadName : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("Dod"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

class UITextParser_DoodadNameMove : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("DodM"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};
//------------------------------------------------------------------------
class UITextParser_URL : public ITextParser
{
public:
    virtual FString GetKeyword() const { return TEXT("URL"); }
    virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

//------------------------------------------------------------------------
// special handling of questionnaire options
class UITextParser_QURL : public ITextParser
{
public:
    virtual FString GetKeyword() const { return TEXT("QURL"); }
    virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

// add by zhangrui@snda
class UITextParser_Instruction : public ITextParser
{
public:
    virtual FString GetKeyword() const { return TEXT("INS"); }
    virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};

class UITextParser_QShowPage : public ITextParser
{
public:
	virtual FString GetKeyword() const { return TEXT("Page"); }
	virtual void Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen );
};
#endif
