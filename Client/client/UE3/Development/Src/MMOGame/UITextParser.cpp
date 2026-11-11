#include "stdafx.h"
#include "UITextParser.h"
#include "MMOGameUIClasses.h"
#include "DialogOptionAction.h"
#include "TranslatorTags.h"
#include "AbilityProcessor.h"
#include "LinkManager.h"
#include "MMOQuestManager.h"
#include "MMOMapDataManager.h"
#include "MMOSingleton.h"
#include "MMOFacade.h"
#include "MMOEquipManager.h"
#include "MMOChatManager.h"
#include "MMOWeatherManager.h"
#include "MMOSkillManager.h"
#include "EquipSlotManager.h"
#include "MMOGuildData.h"
#include "Shop.h"
#include "MMOAttributeAccountManager.h"
#include "MMOPowerManager.h"

#define WeaponTypeHelperMacro(s, n, i, count, singleLimit ) \
    if(types##s->bWeaponType##s##n)	\
	{ret += MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.EWeaponType." TEXT(#i)) ) + TEXT(" "); ++count; singleLimit = n; }
//{ret += Localize(TEXT("EWeaponType"), TEXT(#i), TEXT("Properties")) + TEXT(" "); ++count; singleLimit = n; }

#define WeaponTypeHelperMacro2(s, n, i, count, singleLimit )	WeaponTypeHelperMacro(s, n, EWT_Weapon_##i, count, singleLimit )

template<> UITextParser* Singleton<UITextParser>::ms_Singleton = 0;

UITextParser GUITextParser;
FString UITextParser::RightAlignTabs = TEXT("\t\t\t\t\t");
FString UITextParser::DummyText = TEXT(" ");
const FString ITextParser::StartTag = TEXT("<#");
const FString ITextParser::EndTag = TEXT("/>");


//------------------------------------------------------------------------
void DialogContext::ExecuteOption(INT OptionID)
{
    DialogOptionActionMap::iterator i = OptionActionMap.find(OptionID);
    if(i != OptionActionMap.end())
    {
        i->second->Execute();
    }
}

//------------------------------------------------------------------------
INT QueryItemInfoAttribute(INT16 attributeType, UItem* item, ItemInfo *itemInfo, INT &outIntValue, FLOAT &outFloatValue )
{
	UEquip* equip = (UEquip*)item;
	if( equip == NULL ){
		return -1;
	}

	outIntValue = 0;
	outFloatValue = 0;
	INT returnValueType = 1;
	switch(attributeType)
	{
	case catPhyAtk:
		{
			INT phyAtkHi = equip->FixAttrbutes.WeaponPhyAtkLo;
			outIntValue = phyAtkHi;
			//if( itemInfo != NULL && itemInfo->m_byLevelupPhyPoint > 0 ){
			//	outIntValue += itemInfo->m_byLevelupPhyPoint;
			//}
		}
		break;
	case catMaxPhyAtk:
		outIntValue = equip->FixAttrbutes.WeaponPhyAtkHi;
		if( itemInfo != NULL && itemInfo->m_byLevelupPhyPoint > 0 ){
			outIntValue += itemInfo->m_byLevelupPhyPoint;
		}
		break;
	case catMagAtk:
		outIntValue = equip->FixAttrbutes.WeaponMagAtkLo;
		//if( itemInfo != NULL && itemInfo->m_byLevelupMagPoint > 0 ){
		//	outIntValue += itemInfo->m_byLevelupMagPoint;
		//}
		break;
	case catMaxMagAtk:
		{
			INT magAtkHi = equip->FixAttrbutes.WeaponMagAtkHi;
			outIntValue = magAtkHi;
			if( itemInfo != NULL && itemInfo->m_byLevelupMagPoint > 0 ){
				outIntValue += itemInfo->m_byLevelupMagPoint;
			}
		}
		break;
	case catTaoAtk:
		outIntValue = equip->FixAttrbutes.WeaponTaoAtkLo;
		//if( itemInfo != NULL && itemInfo->m_byLevelupTaoPoint > 0 ){
		//	outIntValue += itemInfo->m_byLevelupTaoPoint;
		//}
		break;
	case catMaxTaoAtk:
		outIntValue = equip->FixAttrbutes.WeaponTaoAtkHi;
		if( itemInfo != NULL && itemInfo->m_byLevelupTaoPoint > 0 ){
			outIntValue += itemInfo->m_byLevelupTaoPoint;
		}
		break;
	case catPhyDef:
		outIntValue = equip->FixAttrbutes.PhyDefLo;
		break;
	case catMaxPhyDef:
		outIntValue = equip->FixAttrbutes.PhyDefHi;
		break;

	case catMagDef:
		outIntValue = equip->FixAttrbutes.MagDefLo;
		break;
	case catMaxMagDef:
		outIntValue = equip->FixAttrbutes.MagDefHi;
		break;
	case catMaxHp:
		outIntValue = equip->FixAttrbutes.MaxHp;
		break;
	case catMaxMp:
		outIntValue = equip->FixAttrbutes.MaxMp;
		break;
	case catAccurate:
		outIntValue = equip->FixAttrbutes.Accurate;
		break;
	case catAgility:
		outIntValue = equip->FixAttrbutes.Agility;
		break;
	case catAtkSpeed:
		outIntValue = equip->FixAttrbutes.AtkSpeed;
		break;
	case catMagDodge:
		outIntValue = equip->FixAttrbutes.MagDodge / 100;
		break;
	}

	return returnValueType;
}

//------------------------------------------------------------------------
UITextParser::UITextParser(void)
{
    // 	SplitCharList.AddItem(TEXT(' '));
    // 	SplitCharList.AddItem(TEXT('\t'));
    // 	SplitCharList.AddItem(TEXT('<'));
    // 	SplitCharList.AddItem(TEXT('>'));
    // 	SplitCharList.AddItem(TEXT(':'));

#define REG_UITXT_PARSER(cls) {cls* obj = new cls; TextParserMap[obj->GetKeyword()] = obj; }

    REG_UITXT_PARSER(UITextParser_DefaultDialog);
    REG_UITXT_PARSER(UITextParser_NPC);
    REG_UITXT_PARSER(UITextParser_Item);
	REG_UITXT_PARSER(UITextParser_ItemLink);
	REG_UITXT_PARSER(UITextParser_FromStringMap);
	REG_UITXT_PARSER(UITextParser_DialogOptionServer);
	REG_UITXT_PARSER(UITextParser_Paramater0);
	REG_UITXT_PARSER(UITextParser_Paramater1);
	REG_UITXT_PARSER(UITextParser_Paramater2);
	REG_UITXT_PARSER(UITextParser_Paramater3);
	REG_UITXT_PARSER(UITextParser_CommonParameter);
	REG_UITXT_PARSER(UITextParser_EquipSlot);
	REG_UITXT_PARSER(UITextParser_EquipSlotLevel);
	REG_UITXT_PARSER(UITextParser_WeaponRuneID);
    REG_UITXT_PARSER(UITextParser_DialogText);
    REG_UITXT_PARSER(UITextParser_DialogOption);
    //REG_UITXT_PARSER(UITextParser_Player);
    REG_UITXT_PARSER(UITextParser_Quest);
    REG_UITXT_PARSER(UITextParser_OpenUI);
    REG_UITXT_PARSER(UITextParser_SelfName);
	REG_UITXT_PARSER(UITextParser_PlayerName);
	REG_UITXT_PARSER(UITextParser_GuildName);
	REG_UITXT_PARSER(UITextParser_NavPointName);
	REG_UITXT_PARSER(UITextParser_Instruction);
    REG_UITXT_PARSER(UITextParser_URL);
    REG_UITXT_PARSER(UITextParser_QURL);
	REG_UITXT_PARSER(UITextParser_DoodadName);
	REG_UITXT_PARSER(UITextParser_QShowPage);
	REG_UITXT_PARSER(UITextParser_NpcMove);
	REG_UITXT_PARSER(UITextParser_NavPointNameMove);
	REG_UITXT_PARSER(UITextParser_DoodadNameMove);
	REG_UITXT_PARSER(UITextParser_NpcArrow);

#ifdef _DEBUG
    FString txt = TEXT("<D>你好啊，<P:NAME>，强壮的小哥~ </D> \
                       <O:1>需要我的特殊服务么？奖励：<I:100> </O> \
                       <M:1> \
                       <QD:1>");

    ParseContext ctx;
    //DialogContext outTxt = Parse(ctx,  DCtxt,TEXT("O"));

#endif
} 

FString UITextParser::GetNormalColor( const TCHAR* key )
{
	FString value = TEXT("");
	if (NULL != key)
	{
		GConfig->GetString(TEXT("NormalColor"), key, value, GUIColorIni);
		return value;
	}
	return value;

}

FString UITextParser::GetTipsColor(const TCHAR* key)
{
	FString value = TEXT("");
	if (NULL != key)
	{
		GConfig->GetString(TEXT("TipsColor"), key, value, GUIColorIni);
		return value;
	}
	return value;
}

//------------------------------------------------------------------------
UITextParser::~UITextParser(void)
{
    ParserMap::iterator i = TextParserMap.begin();
    for (; i != TextParserMap.end(); ++i)
    {
        delete i->second;
    }
}

//------------------------------------------------------------------------
void UITextParser::Parse( ParseContext& ctx, DialogContext& OutText, const FString& srcStr,const FString& matchString )
{
    INT i = 0;
    INT tokenStart = 0;
    bool foundTokenStart = false;
    TCHAR* c = (TCHAR*)*srcStr;

    // OutText.Clear(); // oscar@snda 这个在递归解析时会清除之前的结果，不需要

    //TArray<FString> tokens;
    //TTranslator::EscapeAndTokenize(&tokens, srcStr);

    while (i < srcStr.Len())
    {		
        if(!appStrnicmp(c, *ITextParser::StartTag, ITextParser::StartTag.Len()))
        {
            foundTokenStart = true;

            i += ITextParser::StartTag.Len();
            c += ITextParser::StartTag.Len();

            tokenStart = i;
        }
        else if(foundTokenStart && ((*c == TEXT(':') || (*c == TEXT('>')))))
        {

            FString token = srcStr.Mid(tokenStart, i - tokenStart);
            token = token.ToUpper();
            ITextParser* parser = GetParser(token);
            if(parser /*&& (matchString == TEXT("") ||  matchString == token || token == TEXT("DFT") )*/) //加了matchString，只截取此间文字
            {
                //if( )
                parser->Parse(ctx,  OutText, c, i, srcStr.Len());
                //else
                //{
                //	++i;                
                //	++c;
                //	continue;
                //}
            }
            else
            {
                // 没有解析器的情况要继续走
                ITextParser::ParseCloseTag(ctx, c, i, srcStr.Len(), token);
            }

            foundTokenStart = false;
        }
        else if(!foundTokenStart && matchString == TEXT(""))
        {
            OutText.TempText.AppendChar(*c);
            ++i;
            ++c;
        }
        else
        {
            ++i;
            ++c;
        }
    }

#if 1
	if(!ctx.SkipAppendNPCDialogOption)
	{
		FString oldTempText = OutText.TempText;
		switch(ctx.GetParamCount())
		{
		case 1:
			OutText.TempText = FString::Printf(LocalizeSecure(OutText.TempText, *ctx.Parameters[0].ParameterString));
			break;

		case 2:
			OutText.TempText = FString::Printf(LocalizeSecure(OutText.TempText, *ctx.Parameters[0].ParameterString, *ctx.Parameters[1].ParameterString));
			break;

		case 3:
			OutText.TempText = FString::Printf(LocalizeSecure(OutText.TempText, *ctx.Parameters[0].ParameterString, *ctx.Parameters[1].ParameterString, *ctx.Parameters[2].ParameterString));
			break;
		case 4:
			OutText.TempText = FString::Printf(LocalizeSecure(OutText.TempText, *ctx.Parameters[0].ParameterString, *ctx.Parameters[1].ParameterString, *ctx.Parameters[2].ParameterString, *ctx.Parameters[3].ParameterString));
			break;
		}

		// modified by zhangrui05
		// fix bug #24000
		// LocalizeSecure解析的时候，如果原文没有参数，强行追加一个参数，返回原文; 强行追加2个(或2个以上)，返回TEXT("")
		// 所以原来服务器默认带一个参数，不会出问题，带两个，就有问题了，这里缓存一下原文
		if (OutText.TempText == TEXT("") && oldTempText != TEXT(""))
		{
			OutText.TempText = oldTempText;
		}
	}
#else
	for(INT i = 0; i < PARSE_MAX_PARAMETERS; ++i)
	{
		if(ctx.Parameters[i].IsInUse)
		{
			OutText.TempText = FString::Printf(LocalizeSecure(OutText.TempText, *ctx.Parameters[i].ParameterString));
		}
	}
#endif

	// 插入NPC对话选项的补充:
	if(!ctx.SkipAppendNPCDialogOption && ctx.NPC && ctx.NPC->Template)
	{
		FString token = TEXT("UI");
		ITextParser* parser = GetParser(token);

		if(parser)
		{
			TArray<FDialogOptionConfig*> ProcList;
			ProcList.AddItem(&ctx.NPC->Template->EnableDiag_PlayerBank);
			ProcList.AddItem(&ctx.NPC->Template->EnableDiag_GuildBank);
			ProcList.AddItem(&ctx.NPC->Template->EnableDiag_NPCShop);
			ProcList.AddItem(&ctx.NPC->Template->EnableDiag_RewardQuest);
			ProcList.AddItem(&ctx.NPC->Template->EnableDiag_CreateGuild);
			ProcList.AddItem(&ctx.NPC->Template->EnableDiag_GuildSalary);
			ProcList.AddItem(&ctx.NPC->Template->EnableDiag_Auction);
			ProcList.AddItem(&ctx.NPC->Template->EnableDiag_WeaponRune);
			ProcList.AddItem(&ctx.NPC->Template->EnableDiag_GemEmbed);
			ProcList.AddItem(&ctx.NPC->Template->EnableDiag_LevelUpCurWeapon);
			ProcList.AddItem(&ctx.NPC->Template->EnableDiag_GetBackCurLvUpWeapon);
			ProcList.AddItem(&ctx.NPC->Template->EnableDiag_SpecialRepair);
			ProcList.AddItem(&ctx.NPC->Template->EnableDiag_Gamble);
			ProcList.AddItem(&ctx.NPC->Template->EnableDiag_Exercise);
			ProcList.AddItem(&ctx.NPC->Template->EnableDiag_GambleBook);
			ProcList.AddItem(&ctx.NPC->Template->EnableDiag_ArenaReward);
			ProcList.AddItem(&ctx.NPC->Template->EnableDiag_GamblePotion);

			FString removeBegin = TEXT("<#UI");
			for (INT m = 0; m < ProcList.Num(); ++m)
			{
				FDialogOptionConfig* cfg = ProcList(m);
				if(cfg->Enable && (cfg->ClientDiagText.Len() > removeBegin.Len()))
				{
					TCHAR* diagOptStr = (TCHAR*)*cfg->ClientDiagText + removeBegin.Len();

					INT diagOptStrIdx = 0;

					parser->Parse(ctx,  OutText, diagOptStr, diagOptStrIdx, cfg->ClientDiagText.Len() - removeBegin.Len());
				}
			}
		}
	}

}

// add by zhangrui05@snda.com
void UITextParser::ParseContent(ParseContext& ctx, DialogContext& OutText, const FString& srcStr,const FString& matchString)
{
	INT i = 0;
	INT tokenStart = 0;
	bool foundTokenStart = false;
	TCHAR* c = (TCHAR*)*srcStr;

	OutText.Clear();

	while (i < srcStr.Len())
	{		
		if(!appStrnicmp(c, *ITextParser::StartTag, ITextParser::StartTag.Len()))
		{
			foundTokenStart = true;

			i += ITextParser::StartTag.Len();
			c += ITextParser::StartTag.Len();

			tokenStart = i;
		}
		else if(foundTokenStart && ((*c == TEXT(':') || (*c == TEXT('>')))))
		{

			FString token = srcStr.Mid(tokenStart, i - tokenStart);
			token = token.ToUpper();
			ITextParser* parser = GetParser(token);
			if(parser)
			{
				if (token == TEXT("N"))
				{
					OutText.TempText += parser->ParseContent(ctx, c, i, srcStr.Len());
				}
				else
				{
					parser->ParseStringID(ctx, c, i, srcStr.Len());
					OutText.TempText += parser->ParseContent(ctx, c, i, srcStr.Len());
				}
			}
			else
			{
				// 没有解析器的情况要继续走
				ITextParser::ParseCloseTag(ctx, c, i, srcStr.Len(), token);
			}

			foundTokenStart = false;
		}
		else if(!foundTokenStart && matchString == TEXT(""))
		{
			OutText.TempText.AppendChar(*c);
			++i;
			++c;
		}
		else
		{
			++i;
			++c;
		}
	}
}

//------------------------------------------------------------------------
ITextParser* UITextParser::GetParser( const FString& keyword )
{
    ParserMap::iterator i = TextParserMap.find(keyword);
    if(i != TextParserMap.end())
    {
        return i->second;
    }

    return 0;
}


//BaseFunc Begin----------------------------------------------------------------------------------------------------------
FString UITextParser::HTMLBeginFont( const FString& fntClr, INT fntSz /*= 12*/ )
{
    // size=\'%d\'
    //return FString::Printf(TEXT("<font color=\'%s\' size=\'%d\'>"), *fntClr, fntSz);
	return FString::Printf(TEXT("<font color=\'%s\' >"), *fntClr);

}

//------------------------------------------------------------------------
FString UITextParser::HTMLEndFont()
{
    return TEXT("</font>");
}

//------------------------------------------------------------------------
FString UITextParser::HTMLBeginParagraph(const FString& alignMode)
{
    return FString::Printf(TEXT("<p align=\'%s\'>"), *alignMode);
}

//------------------------------------------------------------------------
FString UITextParser::HTMLEndParagraph()
{
    return TEXT("</p>");
}

//------------------------------------------------------------------------
FString UITextParser::HTMLNewLine()
{
    return TEXT("</p><br></p>");
}

//------------------------------------------------------------------------
FString UITextParser::HTMLGetItemColor( EItemColor i )
{
	TArray<FString> colorArr;
	GConfig->GetArray(TEXT("EItemColor"), TEXT("ItemColorHTML"), colorArr, GUIColorIni);
	if((BYTE)i < colorArr.Num())
	{
		return colorArr((BYTE)i);
	}

    return GetTipsColor(TEXT("AlertColor"));
}

//------------------------------------------------------------------------
INT UITextParser::HTMLGetItemRealColor( EItemColor i )
{
    INT realColor = 0;
    TArray<FString> colorArr;
    GConfig->GetArray(TEXT("EItemColor"), TEXT("ItemColorHTML"), colorArr, GUIColorIni);
    if((BYTE)i < colorArr.Num())
    {
        FString colorStr = colorArr((BYTE)i).Replace( TEXT("#"), TEXT("") );
        realColor =  _tcstol( *colorStr, NULL, 16 );
    }

    return realColor;
}

//BaseFunc End----------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------
int UITextParser::GetEnumIdByTypeName( FString name )
{
    UEnum* ItemTypeEnum = FindObject<UEnum>( UServerEnum::StaticClass(), TEXT("EITEM_TYPE"), TRUE );
    FString enumPrefix = ( TEXT( "E_" )  );

    if( ItemTypeEnum )
    {
        return ItemTypeEnum->FindEnumIndex( FName( *( enumPrefix  + name ) ) );
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------
FString UITextParser::HTMLGetCurrencyColor( MONEY_TYPE i )
{
    FString cfg;
    if(GConfig->GetString(TEXT("EMoneyTypeColor"),
        *FString::Printf(TEXT("EMoneyTypeColor_%d"), (BYTE)i), cfg, GUIColorIni))
    {
        return cfg;
    }

    return  GetTipsColor(TEXT("AlertColor"));
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatCurrency( MONEY_TYPE i, int cnt )
{
    FString tips = TEXT("");

    tips += HTMLBeginFont(HTMLGetCurrencyColor(i));

    if(cnt > 0)
    {
        tips += FString::Printf(TEXT("%d"), cnt);
    }
    else
    {
        tips += MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Shop.NotForSale"));
    }

    tips += HTMLEndFont();
    return tips;
}

FString UITextParser::ConvertPrice(INT64 money)
{
	FString res;

	if( money/100000000 > 0 ){
		res += appItoa( money/100000000 ) + TEXT("亿");
		res += appItoa( (money%100000000)/10000 ) + TEXT("万");
	}
	else{
		char cvtMnyNum[64];
		_i64toa( money, cvtMnyNum, 10 );

		res += cvtMnyNum;
		res += TEXT("万");
	}

	return res;
}

FString UITextParser::HTMLMoneyIconString( INT moneyType , INT64 inMoney, FString moneyColor )
{
#define MONEY_WAN_HTML_STRING (TEXT("<font color='#fbd671'>万</font>"))
#define MONEY_YI_HTML_STRING (TEXT("<font color='#fbd671'>亿</font>"))

#ifdef _WINDOWS_
#define  ISC_INT64_FORMAT TEXT("%I64d")
#else
#define  ISC_INT64_FORMAT TEXT("%lld")
#endif
	FString res = TEXT("");
	FString imgPath;

	BOOL  bNeedShow_LS = FALSE;//true需要分单位
	switch( moneyType )
	{
	case MONEYTYPE_01:
		{
			imgPath = TEXT("<IMG  SRC='img://MMO_UIIcon.MoneyIcon.MoneyCash_");
			bNeedShow_LS = TRUE;
		}
		break;
	case MONEYTYPE_02:
		{
			imgPath = TEXT("<IMG  SRC='img://MMO_UIIcon.MoneyIcon.MoneyCash_");
			bNeedShow_LS = TRUE;
		}
		break;
	case MONEYTYPE_03:
		{
			imgPath = TEXT("<IMG  SRC='img://MMO_UIIcon.MoneyIcon.ArenaMoney'/>");
		}
		break;
	case MONEYTYPE_04:
		imgPath = TEXT("<IMG  SRC='img://MMO_UIIcon.MoneyIcon.MoneyBullion'/>");
		break;
	case MONEYTYPE_ARENA:
		imgPath = TEXT("<IMG  SRC='img://MMO_UIIcon.MoneyIcon.ArenaPoint'/>");
		break;
	case MONEYTYPE_BF:
		imgPath = TEXT("<IMG  SRC='img://MMO_UIIcon.MoneyIcon.BattleHonor'/>");
		break;
	case MONEYTYPE_MORAL:
		{
			imgPath = TEXT("<IMG  SRC='img://MMO_UIIcon.MoneyIcon.Faith'/>");
			if (inMoney > 0)
			{
				inMoney = inMoney /  MMOFacade::getSingleton().GetMoneyRatio();
			}
			
		}
		
		break;
	case MONEYTYPE_GUILD:
		imgPath = TEXT("<IMG  SRC='img://MMO_UIIcon.MoneyIcon.GuildHonor'/>");
		break;
	case MONEYTYPE_SOUL_PTS:
		imgPath = TEXT("<IMG  SRC='img://MMO_UIIcon.MoneyIcon.Soul'/>");
		break;
	case MONEYTYPE_ACHIEVEMENTPTS:
		imgPath = TEXT("<IMG  SRC='img://MMO_UIIcon.MoneyIcon.Achievement'/>");
		break;

	// 下面三个都没有资源, 临时用家族贡献的格式吧
	case MONEYTYPE_MASTER_EXP:
		imgPath = TEXT("<IMG  SRC='img://MMO_UIIcon.MoneyIcon.GuildHonor'/>");
		break;
	case MONEYTYPE_SPELL_PTS:
		imgPath = TEXT("<IMG  SRC='img://MMO_UIIcon.MoneyIcon.GuildHonor'/>");
		break;

	default:
		return TEXT("");
	}

	UBOOL isBigMoney = FALSE;
	INT64 moneyValue = 0;
	if( inMoney >= 0x7fffffff ){
		isBigMoney = TRUE;
		moneyValue = ( inMoney % 10000 );
		inMoney -= moneyValue;
	}
	else{
		moneyValue = inMoney;
	}

	if( moneyColor.IsEmpty() )
	{
		if(bNeedShow_LS)
		{
			if( isBigMoney || moneyValue >= 10000 )
			{
				if( !isBigMoney ){
					if( moneyValue/100000000 > 0 ){
						res += FString::Printf(ISC_INT64_FORMAT,moneyValue / 100000000) + MONEY_YI_HTML_STRING;

						if ((moneyValue%100000000)/10000 > 0){
							res += FString::Printf(ISC_INT64_FORMAT,(moneyValue % 100000000) / 10000) + MONEY_WAN_HTML_STRING;
						}
					}
					else{
						res += FString::Printf(ISC_INT64_FORMAT,moneyValue / 10000) + MONEY_WAN_HTML_STRING;
					}
				}
				else{
					//char cvtMnyNum[64];
					//_i64toa( inMoney, cvtMnyNum, 10 );

					//res += cvtMnyNum;
					//res += MONEY_WAN_HTML_STRING;
					FString price =  ConvertPrice(inMoney);
					res += price;
				}
			}
			INT64 oddments = moneyValue%10000;
			if( oddments != 0 || inMoney == 0 )
			{
				res += FString::Printf(ISC_INT64_FORMAT, oddments );
			}

			res += imgPath + TEXT("S'/>");
		}
		else
			res += appItoa( moneyValue ) + imgPath;
	}
	else
	{
		res += UITextParser::HTMLBeginFont( moneyColor );

		if(bNeedShow_LS)
		{
			if( moneyValue >= 10000 )
			{
				if( !isBigMoney ){
					if( moneyValue/100000000 > 0 ){
						res += FString::Printf(ISC_INT64_FORMAT,moneyValue / 100000000) + MONEY_YI_HTML_STRING;

						if ((moneyValue%100000000)/10000 > 0){
							res += FString::Printf(ISC_INT64_FORMAT,(moneyValue % 100000000) / 10000) + MONEY_WAN_HTML_STRING;
						}
					}
					else{
						res += FString::Printf(ISC_INT64_FORMAT,moneyValue / 10000) + MONEY_WAN_HTML_STRING;
					}
				}
				else{
					char cvtMnyNum[64];
					_i64toa( inMoney, cvtMnyNum, 10 );

					res += cvtMnyNum;
					res += MONEY_WAN_HTML_STRING;
				}
			}
			INT64 oddments = moneyValue%10000;
			if( oddments != 0 || inMoney == 0 )
			{
				res += FString::Printf(ISC_INT64_FORMAT, oddments );
			}

			res += imgPath + TEXT("S'/>");
		}
		else
			res += FString::Printf(ISC_INT64_FORMAT, moneyValue ) + imgPath;
		res += UITextParser::HTMLEndFont();
	}

	return res;
}

//-------------------------------------------------------------------------------------
FString UITextParser::GetMoneyIconImgAndName( FString &moneyName,INT moneyType )
{
	FString imgPath;
	switch( moneyType )
	{
	case E_MONEYTYPE_01: imgPath = TEXT("MMO_ItemIcon_Supplies.img_BindMoney"); break;
	case E_MONEYTYPE_02: imgPath = TEXT("MMO_ItemIcon_Supplies.img_Money"); break;
	case E_MONEYTYPE_03: imgPath = TEXT(""); break;
	case E_MONEYTYPE_04: imgPath = TEXT(""); break;
	case E_MONEYTYPE_FATIGUE: imgPath = TEXT(""); break;
	case E_MONEYTYPE_SOUL_PTS: imgPath = TEXT(""); break;
	case E_MONEYTYPE_MASTER_EXP: imgPath = TEXT(""); break;
	default:
		return imgPath;
	}
	//name
	//FString moneyNameLookUp = TEXT("Properties.EMONEY_TYPE.EMoneyTypeName_");
	//moneyNameLookUp += appItoa(moneyType);
	//moneyName = MMOUI::ParseLocalizedPropertyPath(moneyNameLookUp);
	moneyName = UITextParser::HTMLGetCurrencyName((MONEY_TYPE)moneyType);

	//
	return imgPath;
}

//------------------------------------------------------------------------
FString UITextParser::CvtNumToChnNum( UINT64 uiNum )
{
	FString toChnNumStrLookUp = TEXT("MMOGame.Common.Num");
	FString chnNumStr;
	UBOOL isBigMoney = FALSE;
	INT moneyValue = 0;
	if( uiNum >= 0x7fffffff ){
		isBigMoney = TRUE;
		//TODO:
	}
	else{
		INT curScore = (INT)uiNum;
		INT curFactor = 10;
		for( INT i = 1; i < 3; ++i ){//只支持到千
			INT curNum = curScore / curFactor;
			if( curNum > 0 ){
				chnNumStr += ( ( curFactor != 10 ) ? MMOUI::ParseLocalizedPropertyPath( toChnNumStrLookUp + appItoa( curNum ) ) : TEXT("") );
				chnNumStr += MMOUI::ParseLocalizedPropertyPath( toChnNumStrLookUp + appItoa( curFactor ) );
			}
			else{
				INT leftNum = curScore % 10;
				chnNumStr +=  ( ( leftNum != 0 ) ? MMOUI::ParseLocalizedPropertyPath( toChnNumStrLookUp + appItoa( leftNum ) ) : TEXT("") );
				break;
			}
			curFactor *= 10;
		}
	}

	return chnNumStr;
}
//------------------------------------------------------------------------
FString UITextParser::HTMLGetCurrencyName( MONEY_TYPE i )
{
	FString enumVal = TEXT("");
    static UEnum* Enum = FindObject<UEnum>(UServerEnum::StaticClass(), TEXT("EMONEY_TYPE"), TRUE);
	if (Enum)
	{
		enumVal = Enum->GetEnum(i).ToString();
	}
	return MMOUI::ParseLocalizedPropertyPath(FString(TEXT("Properties.EMONEY_TYPE.")) + enumVal);
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemNameWithNoColor( UItem* item )
{	
    FString tips=TEXT("");

    tips += HTMLBeginFont(GetTipsColor(TEXT("NormalContentColor")));
    tips += item->GetDisplayName();	
    tips += HTMLEndFont();
    if(item->ItemType == "ITEMTYPE_SPELLBOOK")
    {
        if(item->GetToUseAbility())
        {
            const INT AbilityID = item->GetToUseAbility()->SpellCommonAttribute.Id;
			const INT ablilityRuneID = item->GetToUseAbility()->GetRuneSlot();
			if(MMOFacade::getSingleton().GetMainActor()->GetAbilityById(AbilityID, ablilityRuneID))
            {
                tips += FString::Printf(TEXT("(%s)"), *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.SpellBook.Learned")) );
            }
            else
            {
                tips +=  FString::Printf(TEXT("(%s)"), *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.SpellBook.Notyet")) );
            }
        }
    }
    return tips;
}


//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemName( UItem* item )
{	
    FString tips=TEXT("");
	if( item == NULL )
	{
		return tips;
	}
    tips += item->GetDisplayName();	

    return tips;
}

//
FString UITextParser::HTMLFormatItemGottenType( UItem* item )
{
	FString tips = TEXT("");
	if( item == NULL )
	{
		return tips;
	}

	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainPlayer == NULL )
	{
		return tips;
	}

	if( item->ItemType == TEXT("ITEMTYPE_SPELLBOOK") )
	{
		UMMOAbility *itemAbility = item->GetToUseAbility();
		if( itemAbility == NULL )
		{
			return tips;
		}
		 EPlayerClass playerClass = (EPlayerClass)mainPlayer->GetClassId();

		UBOOL isPlayerUsable = MMO::CanBeUsedByPlayerClass( playerClass, item->RequireClass );
		if( !isPlayerUsable ){
		//if( !itemAbility->ClientCommonAttribute.ShowOnUI ){
			return tips;
		}
		
		//isPlayerUsable = MMOSkillManager::IsSkillUsableForPlayer( itemAbility, playerClass );
		//if( !isPlayerUsable ){
		//	return tips;
		//}

		const INT itemAbilityID = itemAbility->SpellCommonAttribute.Id;
		if( MMOSkillManager::getSingleton().FindMyLearnedMainSkill( itemAbilityID ) )
		{
			tips += MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.SpellBook.Learned"));
		}
		else
		{
			tips += MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.SpellBook.Notyet"));
		}
	}

	return tips;
}
//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemType( UItem* item )
{
    FString tips=TEXT("");

    if(item->ItemType.IsEmpty())
    {
        tips += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Debug.ItemTypeCfgError"));
    }
    else
    {
		tips += HTMLBeginFont( GetTipsColor(TEXT("EquipTypeColor")) ) ;
		if (item->ItemType != TEXT("ITEMTYPE_EQUIP"))
		{
			tips += MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.ItemTypes.") ) + item->ItemSubType );
		}
		else
		{
			//装备
			if (item->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_WEAPON"))
			{
				tips += MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.ItemSubSubTypes.") ) + item->ItemSubSubType );
			}
			else
			{
				tips += MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.ItemSubTypes.") ) + item->ItemSubType );
			}
		}

        tips += HTMLEndFont();
    }

    return tips;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemPrefixLevel( UItem* item )
{
    FString tips = TEXT("");

    tips += HTMLBeginFont(HTMLGetItemColor(EIC_Color8));
    if( item->byPrefixLevel )
    {
        tips += FString::Printf(TEXT("<p > %s</p>"), *MMOUI::ParseLocalizedPropertyPath(FString(TEXT("Properties.PrefixLevel.")) + FString::Printf(TEXT("PrefixLevel%d"), item->byPrefixLevel)));
    }

    tips += HTMLEndFont();

    return tips;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemAttributes( UEquip* equip, ItemInfo* equipInfo )
{
    FString tips;

	INT iValue1;
	INT iValue2;
	FLOAT fValue1;
	FLOAT fValue2;
	if( equip ) // 如果是模板属性
	{
		tips += HTMLBeginFont( GetTipsColor(TEXT("MainAttrColor")) );
		//物理攻击
		QueryItemInfoAttribute( catPhyAtk, equip, equipInfo, iValue1, fValue1 );
		QueryItemInfoAttribute( catMaxPhyAtk, equip, equipInfo, iValue2, fValue2 );
		if(iValue1 || iValue2)
		{
			tips += FString::Printf( TEXT("<p>%s %d - %d</p>"), 
				*MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.WeaponPhyAtk")), iValue1 , iValue2 );

		}
		//魔法攻击
		QueryItemInfoAttribute( catMagAtk, equip, equipInfo, iValue1, fValue1 );
		QueryItemInfoAttribute( catMaxMagAtk, equip, equipInfo, iValue2, fValue2 );
		if(iValue1 || iValue2)
		{
			tips += FString::Printf( TEXT("<p>%s %d - %d</p>"),
				*MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.WeaponMagAtk")), iValue1 , iValue2 );
		}
		//道术攻击
		QueryItemInfoAttribute( catTaoAtk, equip, equipInfo, iValue1, fValue1 );
		QueryItemInfoAttribute( catMaxTaoAtk, equip, equipInfo, iValue2, fValue2 );
		if(iValue1 || iValue2)
		{
			tips += FString::Printf( TEXT("<p>%s %d - %d</p>"),
				*MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.WeaponTaoAtk")), iValue1 , iValue2 );

		}
		//物理防御
		QueryItemInfoAttribute( catPhyDef, equip, equipInfo, iValue1, fValue1 );
		QueryItemInfoAttribute( catMaxPhyDef, equip, equipInfo, iValue2, fValue2 );
		if(iValue1 || iValue2)
		{
			tips += FString::Printf( TEXT("<p>%s %d - %d</p>"),
				*MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.PhyDef")), iValue1 , iValue2 );
		}
		//魔法防御
		QueryItemInfoAttribute( catMagDef, equip, equipInfo, iValue1, fValue1 );
		QueryItemInfoAttribute( catMaxMagDef, equip, equipInfo, iValue2, fValue2 );
		if(iValue1 || iValue2)
		{
			tips += FString::Printf( TEXT("<p>%s %d - %d</p>"),
				*MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.MagDef")), iValue1 , iValue2 );
		}
		//生命值
		QueryItemInfoAttribute( catMaxHp, equip, equipInfo, iValue1, fValue1 );
		if(iValue1)
		{
			tips += FString::Printf( TEXT("<p>%s %d</p>"), *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.MaxHp")), iValue1 );
		}
		//魔法值
		QueryItemInfoAttribute( catMaxMp, equip, equipInfo, iValue1, fValue1 );
		if(iValue1)
		{
			tips += FString::Printf( TEXT("<p>%s %d</p>"), *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.MaxMp")), iValue1 );
		}
		//准确度
		QueryItemInfoAttribute( catAccurate, equip, equipInfo, iValue1, fValue1 );
		if(iValue1)
		{
			tips += FString::Printf( TEXT("<p>%s %d</p>"), *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.Accurate")), iValue1 );
		}
		//敏捷度
		QueryItemInfoAttribute( catAgility, equip, equipInfo, iValue1, fValue1 );
		if(iValue1)
		{
			tips += FString::Printf( TEXT("<p>%s %d</p>"), *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.Agility")), iValue1 );
		}
		//攻击速度
		QueryItemInfoAttribute( catAtkSpeed, equip, equipInfo, iValue1, fValue1 );
		if(iValue1)
		{
			QueryItemInfoAttribute( catAtkSpeed, equip, equipInfo, iValue1, fValue1 );
			tips += FString::Printf( TEXT("<p>%s %d</p>"), *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.AtkSpeed")), iValue1 );
		}
		//魔法躲避
		QueryItemInfoAttribute( catMagDodge, equip, equipInfo, iValue1, fValue1 );
		if(iValue1)
		{
			tips += FString::Printf( TEXT("<p>%s %d%%</p>"), *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.MagDodge")), iValue1 );
		}
		tips += HTMLEndFont();
	}

    if( equip &&equip->HasAdditiveAttributes() )// 如果是模板道具并且有附加属性，显示为“随机附魔”
    {
        tips += HTMLBeginFont( GetTipsColor(TEXT("NormalContentColor")) ); // TODO: move to config
        FString randomAttrClr;
        GConfig->GetString(TEXT("Tips"), TEXT("RandomAttributeColor"), randomAttrClr, GGameIni);
        tips += HTMLBeginParagraph() + HTMLBeginFont(randomAttrClr) + 
            MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.ItemTips.RandomMag")) + HTMLEndFont() + HTMLEndParagraph();
        tips += HTMLEndFont();
    }

	return tips;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemRandomAttribute( UEquip* equip, ItemInfo* itemInfo )
{
    FString tips = TEXT("");

    AMMOPlayer *pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
    MMOEquipManager *equipMgr = &MMOEquipManager::getSingleton();

    if( !equip || !itemInfo || !pMainPlayer || !equipMgr )
    {
        return tips;
    }

    bool hasRandomAttr = false;
    for(INT i = 0; i < MAX_RANDOM_ATTR_COUNT; ++i )
    {
        if( itemInfo->m_wRandomAttrID[i] > 0 )
        {
            hasRandomAttr = true;
            break;
        }
    }
    if( !hasRandomAttr )
    {
        return tips;
    }

    for(INT i = 0; i < MAX_RANDOM_ATTR_COUNT; ++i )
    {
        const ItemRandomAttr* attr = MMOFacade::getSingleton().GetRandomAttributById( itemInfo->m_wRandomAttrID[i] );
        if( NULL != attr )
        {
            tips += HTMLBeginParagraph() + HTMLBeginFont( GetTipsColor(TEXT("RandAttrColor")) ) 
                + attr->strText  + HTMLEndFont() + HTMLEndParagraph();
        }
    }

    return tips;
}

//------------------------------------------------------------------------
void UITextParser::HTMLFormatItemBaseAndRandomAttributes( UEquip* equip, ItemInfo* itemInfo, FString &mixParaText, FString &randParaText )
{
	AMMOPlayer *pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	MMOEquipManager &equipMgr = MMOEquipManager::getSingleton();

	mixParaText.Empty();
	randParaText.Empty();
	INT *pIValue;
	TMap< FString, INT > equipRandAttr;
	if( itemInfo != NULL ){
		for(INT i = 0; i < MAX_RANDOM_ATTR_COUNT; ++i )
		{
			const ItemRandomAttr* attr = MMOFacade::getSingleton().GetRandomAttributById( itemInfo->m_wRandomAttrID[i] );
			if( attr != NULL ){
				pIValue = equipRandAttr.Find( attr->attrType );
				if( pIValue != NULL ){
					equipRandAttr.FindOrAdd( attr->attrType ) += attr->value;
				}
				else{
					equipRandAttr.FindOrAdd( attr->attrType ) = attr->value;
				}
			}
		}
	}

	const FString propFomat = TEXT("<p><font color='%s'>%s %s</font></p>");
	const FString digitFomat2Num = TEXT("%d - %d");
	const FString digitFomat1Pct = TEXT("%d%%");

	enum enumDigitColor
	{
		digitWorse,
		digitNormal,
		digitBetter,
		digitColorCnt
	};
	FString digitColr[digitColorCnt];
	digitColr[digitWorse] = GetTipsColor(TEXT("MainAttrColor"));
	digitColr[digitNormal] = GetTipsColor(TEXT("MainAttrColor"));
	digitColr[digitBetter] = GetTipsColor(TEXT("RandAttrColor"));
#define GET_ITEM_PROP_DIGIT_COLOR_MARC( clorRet, compVal ) \
	( (clorRet) = ( (clorRet) == digitNormal ? \
					( ( (compVal) > 0 ) ? \
						digitBetter : \
						( (compVal) < 0 ? digitWorse : digitNormal ) ) : \
					(clorRet) ) )

	INT randAddValue;
	INT isPropBetter = digitNormal;
	INT iValue1;
	INT iValue2;
	FLOAT fValue1;
	FLOAT fValue2;
	FString valueStr;

    if( equip ) // 如果是模板属性
    {
        mixParaText += HTMLBeginFont( GetTipsColor(TEXT("MainAttrColor")) );
		//物理攻击
		QueryItemInfoAttribute( catPhyAtk, equip, itemInfo, iValue1, fValue1 );
		QueryItemInfoAttribute( catMaxPhyAtk, equip, itemInfo, iValue2, fValue2 );

		isPropBetter = digitNormal;
		if( equipRandAttr.RemoveAndCopyValue( TEXT("E_cat_PhyAtk"), randAddValue ) ){
			iValue1 += randAddValue;
			GET_ITEM_PROP_DIGIT_COLOR_MARC( isPropBetter, randAddValue );
		}

		if( equipRandAttr.RemoveAndCopyValue( TEXT("E_cat_MaxPhyAtk"), randAddValue ) ){
			iValue2 += randAddValue;
			GET_ITEM_PROP_DIGIT_COLOR_MARC( isPropBetter, randAddValue );
		}

		if(iValue1 || iValue2)
		{
			valueStr = FString::Printf( *digitFomat2Num, iValue1, iValue2 );
			mixParaText += FString::Printf( *propFomat, *digitColr[isPropBetter], *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.WeaponPhyAtk")), *valueStr );
        }
		//魔法攻击
		QueryItemInfoAttribute( catMagAtk, equip, itemInfo, iValue1, fValue1 );
		QueryItemInfoAttribute( catMaxMagAtk, equip, itemInfo, iValue2, fValue2 );

		isPropBetter = digitNormal;
		if( equipRandAttr.RemoveAndCopyValue( TEXT("E_cat_MagAtk"), randAddValue ) ){
			iValue1 += randAddValue;
			GET_ITEM_PROP_DIGIT_COLOR_MARC( isPropBetter, randAddValue );
		}

		if( equipRandAttr.RemoveAndCopyValue( TEXT("E_cat_MaxMagAtk"), randAddValue ) ){
			iValue2 += randAddValue;
			GET_ITEM_PROP_DIGIT_COLOR_MARC( isPropBetter, randAddValue );
		}

		if(iValue1 || iValue2)
		{
			valueStr = FString::Printf( *digitFomat2Num, iValue1, iValue2 );
			mixParaText += FString::Printf( *propFomat, *digitColr[isPropBetter], *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.WeaponMagAtk")), *valueStr );
        }
		//道术攻击
		QueryItemInfoAttribute( catTaoAtk, equip, itemInfo, iValue1, fValue1 );
		QueryItemInfoAttribute( catMaxTaoAtk, equip, itemInfo, iValue2, fValue2 );

		isPropBetter = digitNormal;
		if( equipRandAttr.RemoveAndCopyValue( TEXT("E_cat_TaoAtk"), randAddValue ) ){
			iValue1 += randAddValue;
			GET_ITEM_PROP_DIGIT_COLOR_MARC( isPropBetter, randAddValue );
		}

		if( equipRandAttr.RemoveAndCopyValue( TEXT("E_cat_MaxTaoAtk"), randAddValue ) ){
			iValue2 += randAddValue;
			GET_ITEM_PROP_DIGIT_COLOR_MARC( isPropBetter, randAddValue );
		}

		if(iValue1 || iValue2)
		{
			valueStr = FString::Printf( *digitFomat2Num, iValue1, iValue2 );
			mixParaText += FString::Printf( *propFomat, *digitColr[isPropBetter], *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.WeaponTaoAtk")), *valueStr );
		}
		//物理防御
		QueryItemInfoAttribute( catPhyDef, equip, itemInfo, iValue1, fValue1 );
		QueryItemInfoAttribute( catMaxPhyDef, equip, itemInfo, iValue2, fValue2 );

		isPropBetter = digitNormal;
		if( equipRandAttr.RemoveAndCopyValue( TEXT("E_cat_PhyDef"), randAddValue ) ){
			iValue1 += randAddValue;
			GET_ITEM_PROP_DIGIT_COLOR_MARC( isPropBetter, randAddValue );
		}

		if( equipRandAttr.RemoveAndCopyValue( TEXT("E_cat_MaxPhyDef"), randAddValue ) ){
			iValue2 += randAddValue;
			GET_ITEM_PROP_DIGIT_COLOR_MARC( isPropBetter, randAddValue );
		}

		if(iValue1 || iValue2)
		{
			valueStr = FString::Printf( *digitFomat2Num, iValue1, iValue2 );
			mixParaText += FString::Printf( *propFomat, *digitColr[isPropBetter], *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.PhyDef")), *valueStr );
        }
		//魔法防御
		QueryItemInfoAttribute( catMagDef, equip, itemInfo, iValue1, fValue1 );
		QueryItemInfoAttribute( catMaxMagDef, equip, itemInfo, iValue2, fValue2 );

		isPropBetter = digitNormal;
		if( equipRandAttr.RemoveAndCopyValue( TEXT("E_cat_MagDef"), randAddValue ) ){
			iValue1 += randAddValue;
			GET_ITEM_PROP_DIGIT_COLOR_MARC( isPropBetter, randAddValue );
		}

		if( equipRandAttr.RemoveAndCopyValue( TEXT("E_cat_MaxMagDef"), randAddValue ) ){
			iValue2 += randAddValue;
			GET_ITEM_PROP_DIGIT_COLOR_MARC( isPropBetter, randAddValue );
		}

		if(iValue1 || iValue2)
		{
			valueStr = FString::Printf( *digitFomat2Num, iValue1, iValue2 );
			mixParaText += FString::Printf( *propFomat, *digitColr[isPropBetter], *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.MagDef")), *valueStr );
        }
		//生命值
		QueryItemInfoAttribute( catMaxHp, equip, itemInfo, iValue1, fValue1 );

		isPropBetter = digitNormal;
		if( equipRandAttr.RemoveAndCopyValue( TEXT("E_cat_MaxHp"), randAddValue ) ){
			iValue1 += randAddValue;
			GET_ITEM_PROP_DIGIT_COLOR_MARC( isPropBetter, randAddValue );
		}

		if(iValue1)
		{
			valueStr = appItoa( iValue1 );
			mixParaText += FString::Printf( *propFomat, *digitColr[isPropBetter], *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.MaxHp")), *valueStr );
        }
		//魔法值
		QueryItemInfoAttribute( catMaxMp, equip, itemInfo, iValue1, fValue1 );

		isPropBetter = digitNormal;
		if( equipRandAttr.RemoveAndCopyValue( TEXT("E_cat_MaxMp"), randAddValue ) ){
			iValue1 += randAddValue;
			GET_ITEM_PROP_DIGIT_COLOR_MARC( isPropBetter, randAddValue );
		}

		if(iValue1)
		{
			valueStr = appItoa( iValue1 );
			mixParaText += FString::Printf( *propFomat, *digitColr[isPropBetter], *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.MaxMp")), *valueStr );
		}
		//准确度
		QueryItemInfoAttribute( catAccurate, equip, itemInfo, iValue1, fValue1 );

		isPropBetter = digitNormal;
		if( equipRandAttr.RemoveAndCopyValue( TEXT("E_cat_Accurate"), randAddValue ) ){
			iValue1 += randAddValue;
			GET_ITEM_PROP_DIGIT_COLOR_MARC( isPropBetter, randAddValue );
		}

		if(iValue1)
		{
			valueStr = appItoa( iValue1 );
			mixParaText += FString::Printf( *propFomat, *digitColr[isPropBetter], *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.Accurate")), *valueStr );
		}
		//敏捷度
		QueryItemInfoAttribute( catAgility, equip, itemInfo, iValue1, fValue1 );

		isPropBetter = digitNormal;
		if( equipRandAttr.RemoveAndCopyValue( TEXT("E_cat_Agility"), randAddValue ) ){
			iValue1 += randAddValue;
			GET_ITEM_PROP_DIGIT_COLOR_MARC( isPropBetter, randAddValue );
		}

		if(iValue1)
		{
			valueStr = appItoa( iValue1 );
			mixParaText += FString::Printf( *propFomat, *digitColr[isPropBetter], *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.Agility")), *valueStr );
		}
		//攻击速度
		QueryItemInfoAttribute( catAtkSpeed, equip, itemInfo, iValue1, fValue1 );

		isPropBetter = digitNormal;
		if( equipRandAttr.RemoveAndCopyValue( TEXT("E_cat_AtkSpeed"), randAddValue ) ){
			iValue1 += randAddValue;
			GET_ITEM_PROP_DIGIT_COLOR_MARC( isPropBetter, randAddValue );
		}

		if(iValue1)
		{
			valueStr = appItoa( iValue1 );
			mixParaText += FString::Printf( *propFomat, *digitColr[isPropBetter], *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.AtkSpeed")), *valueStr );
		}
		//魔法躲避
		QueryItemInfoAttribute( catMagDodge, equip, itemInfo, iValue1, fValue1 );

		isPropBetter = digitNormal;
		if( equipRandAttr.RemoveAndCopyValue( TEXT("E_cat_MagDodge"), randAddValue ) ){
			iValue1 += (INT)( randAddValue / 100 );
			GET_ITEM_PROP_DIGIT_COLOR_MARC( isPropBetter, randAddValue );
		}

		if(iValue1)
		{
			valueStr = FString::Printf( *digitFomat1Pct, iValue1 );
			mixParaText += FString::Printf( *propFomat, *digitColr[isPropBetter], *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Equip:FixAttr.MagDodge")), *valueStr );
		}

		//
		if( itemInfo != NULL && equipRandAttr.Num() > 0 ){
			for(INT i = 0; i < MAX_RANDOM_ATTR_COUNT; ++i )
			{
				if( itemInfo->m_wRandomAttrID[i] == 0 ){
					continue;
				}

				const ItemRandomAttr* attr = MMOFacade::getSingleton().GetRandomAttributById( itemInfo->m_wRandomAttrID[i] );
				if( attr == NULL ){
					continue;
				}

				if( equipRandAttr.Find( attr->attrType ) != NULL ){
					mixParaText += HTMLBeginParagraph();
					mixParaText += HTMLBeginFont( digitColr[digitBetter] );
					mixParaText += attr->strText;
					mixParaText += HTMLEndFont();
					mixParaText += HTMLEndParagraph();
				}
			}
		}
        mixParaText += HTMLEndFont();
    }

	return;
}
//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemWeightAttribute( UItem* item )
{
	FString tipsContent = TEXT("");
	if( item == NULL ){
		return tipsContent;
	}

	tipsContent += HTMLBeginParagraph();
	tipsContent += HTMLBeginFont( GetTipsColor( TEXT("MainAttrColor") ) );

	tipsContent += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Tips.ItemWeight") ), item->Weight );

	tipsContent += HTMLEndFont();
	tipsContent += HTMLEndParagraph();

	return tipsContent;
}
//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemLevelUpPoints( ItemInfo* itemInfo )
{
	FString tipsContent = TEXT("");
	if( itemInfo == NULL ){
		return tipsContent;
	}

	tipsContent += HTMLBeginParagraph();
	tipsContent += HTMLBeginFont( GetTipsColor( TEXT("MainAttrColor") ) );

	INT totalPoint = itemInfo->m_byLevelupPhyPoint + itemInfo->m_byLevelupMagPoint + itemInfo->m_byLevelupTaoPoint;
	tipsContent += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Tips.ItemLevelUpPoint") ), totalPoint, 7 );

	tipsContent += HTMLEndFont();
	tipsContent += HTMLEndParagraph();

	return tipsContent;
}
//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemPurityAttribute( UItem* item )
{
	FString tipsContent = TEXT("");
	if( item == NULL ){
		return tipsContent;
	}
	if( item->ItemSubType != TEXT("ITEMSUBTYPE_MATERIAL_WEAPON_LEVELUP") && item->ItemSubType != TEXT("ITEMSUBTYPE_MATERIAL_MISC") ){
		return tipsContent;
	}

	tipsContent += HTMLBeginParagraph();
	tipsContent += HTMLBeginFont( GetTipsColor( TEXT("MainAttrColor") ) );

	tipsContent += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Tips.ItemPurity") ), item->DefaultDuration );

	tipsContent += HTMLEndFont();
	tipsContent += HTMLEndParagraph();

	return tipsContent;
}
//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemLuckyAttribute( UEquip* equip, ItemInfo* itemInfo )
{
    FString tips = TEXT("");

    if( itemInfo && itemInfo->m_byLuck > 0 )
    {
        tips += HTMLBeginParagraph() + HTMLBeginFont(GetTipsColor(TEXT("LuckColor")))
            + MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EnhanceGeneral.LuckyPoint") )
            + appItoa( itemInfo->m_byLuck )
            + HTMLEndFont()+ HTMLEndParagraph();
    }
    if( itemInfo && itemInfo->m_byLuck < 0 )
    {
        tips += HTMLBeginParagraph() + HTMLBeginFont(GetTipsColor(TEXT("LuckColor")))
            + MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EnhanceGeneral.GuiltyPoint") )
            + HTMLEndFont()/*+ HTMLEndParagraph()*/;
        tips += /*HTMLBeginParagraph() +*/ HTMLBeginFont(GetTipsColor(TEXT("LuckColor")))
            + appItoa( -itemInfo->m_byLuck )
            + HTMLEndFont()+ HTMLEndParagraph();
    }
    return tips;
}

FString UITextParser::HTMLFormatItemEnhanceAttribute( UEquip* equip, ItemInfo* itemInfo )
{
	FString tips = TEXT("");
	//LOG_PROCESS_ERROR(equip && itemInfo);
	if( itemInfo == NULL ){
		return tips;
	}

	if(E_CONSLOT_EQUIPPED == itemInfo->m_byConSlot )
	{
		AMMOPlayer *pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if( pMainPlayer == NULL )
		{
			return TEXT("");
		}
		INT playerClass = pMainPlayer->GetClassId();

		//
		INT slotID = itemInfo->m_byItemSlot;
		EquipSlotProperties *pCurEnhanceSlotLv = EquipSlotManager::getSingleton().GetCurMySlotEnhanceState(slotID);
		if( pCurEnhanceSlotLv == NULL )
		{
			return TEXT("");
		}

		//
		INT curSlotEnhanceLv = pCurEnhanceSlotLv->CurEnhanceLevel;
		if (curSlotEnhanceLv > 0)
		{
			INT ehcMajorLevel = 0;
			INT ehcMinorLevel = 0;
			INT ehcMinorMinorLv = 0;
			EquipSlotManager::getSingleton().CvtEnhanceLevel( curSlotEnhanceLv, ehcMajorLevel, ehcMinorLevel, ehcMinorMinorLv );

			tips += HTMLBeginParagraph() + HTMLBeginFont( GetTipsColor(TEXT("TitleColor")) ) + MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EnhanceGeneral.EnhcGenera3"))
				+ TEXT(" ") + HTMLEndFont();

			TArray<FString> EnhenceLvColor;
			GConfig->GetArray( TEXT("EEnhenceLvColor"), TEXT("EnhenceLvColorHTML"), EnhenceLvColor, GUIColorIni); 

			FString params = TEXT("");
			FString Level = TEXT("");
			if(ehcMinorMinorLv > 0){
				//Level = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.EnhenceLevel")), ehcMinorMinorLv);
				Level = FString::Printf(TEXT("+%d"), ehcMinorMinorLv);
			}
			
			if(curSlotEnhanceLv == MAX_ENHANCE_LEVEL)
			{
				params =  MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipSlotEnhance.MaxLevel"));
			}
			else
			{
				params = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(FString::Printf(TEXT("MMOGame.EquipSlotEnhance.Level_%d"), ehcMinorLevel)));
			}
			
			
			tips+=HTMLBeginFont(EnhenceLvColor(ehcMajorLevel)) + params + Level 
				+ HTMLEndFont() + HTMLEndParagraph();

			tips += HTMLBeginFont( GetTipsColor(TEXT("EnhanceValueColor")) );

			//
			const EquipSlotEnhancePropCfgData *pCurSelSlotProp = EquipSlotManager::getSingleton().FindEnhancePropCfg( playerClass, slotID, curSlotEnhanceLv );
			if(pCurSelSlotProp == NULL)
			{
				return TEXT("");
			}
			for(int i = 0; i < ENHANCE_EFFECT_COUNT; i++)
			{
				const ItemRandomAttr* propAttr = MMOFacade::getSingleton().GetRandomAttributById(pCurSelSlotProp->EffectProperties[i]);
				if( propAttr != NULL )
				{
					tips += propAttr->strText;
					tips += HTMLNewLine( );
				}
			}

			tips += HTMLEndFont();
		}
		
	}

	return tips;
}

BOOL UITextParser::HTMLFormatItemSoulAttribute(UItem* item, ItemInfo* itemInfo, UGFxObject* soulName, UGFxObject* soulIconType, UGFxObject* soulIconName)
{
	BOOL ShowSoul = FALSE;
	if( item == NULL || itemInfo == NULL ){
		return ShowSoul;
	}
	if( soulName == NULL && soulIconType == NULL && soulIconName == NULL ){
		return ShowSoul;
	}

	if (item->ItemType != TEXT("ITEMTYPE_EQUIP")
		|| item->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_WEAPON"))
	{
		return ShowSoul;
	}

	for(INT i = 0; i < MAX_SOCKET_COUNT; i++)
	{
		UINT8 SocketType = itemInfo->GetSocketType(i);
		if (0 == itemInfo->m_nSoulID[i] && SOCKET_NONE == SocketType)
		{
			continue;
		}

		if(!ShowSoul)
			ShowSoul = TRUE;
		soulIconType->SetElementString(i, FString(TEXT("soul")) + appItoa((INT)SocketType));
		const SoulInfo *pSoulInfo = MMOEquipManager::getSingleton().FindSoulInfoByItemTemplateID( itemInfo->m_nSoulID[i] );
		if(NULL != pSoulInfo)
		{
			const ItemRandomAttr* pRandomAttr = MMOFacade::getSingleton().GetRandomAttributById(pSoulInfo->_AttrID);
			if (NULL != pRandomAttr)
			{
				FString strText = UITextParser::HTMLBeginParagraph() + UITextParser::HTMLBeginFont( UITextParser::GetTipsColor(TEXT("SkillNameColor")) ) ;
				strText += pRandomAttr->strText;
				strText += UITextParser::HTMLEndFont() + UITextParser::HTMLEndParagraph();
				soulName->SetElementString(i, strText);
			}
			else{
				soulName->SetElementString(i, TEXT(""));
			}

			UItem* pSoulItem = MMOFacade::getSingleton().FindItemTemplate(pSoulInfo->_ItemID);
			if(NULL != pSoulItem){
				soulIconName->SetElementString(i, MMOFacade::getSingleton().GetItemIconNamePersonalized( pSoulItem, itemInfo ));
			}
			else{
				soulIconName->SetElementString(i, TEXT(""));
			}
		}
		else
		{
			FString strText = HTMLBeginFont(GetTipsColor(TEXT("DisableColor"))) + MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.Tips.Soul")) + appItoa((INT)SocketType) )
				+ HTMLEndFont();
			soulName->SetElementString(i, strText);
			soulIconName->SetElementString(i, TEXT(""));
		}
	}

	return ShowSoul;
}

BOOL UITextParser::HTMLFormatItemWeaponSkillAttribute(UEquip* item, ItemInfo* itemInfo, UGFxObject* skillNameArray, UGFxObject* skillDescArray, UGFxObject* skillIconNameArray)
{
	BOOL showSkill = FALSE;
	LOG_PROCESS_ERROR(item);

	if( item == NULL || item->ItemSubType != TEXT("ITEMSUBTYPE_EQUIP_WEAPON") )
	{
		return showSkill;
	}

	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();
	for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i )
	{
		UBOOL isSkillSlotActive = FALSE;
		UMMOAbility* weaponSkill = NULL;
		if( itemInfo != NULL ){
			weaponSkill = mmoEquipMgr.GetWeaponRuneSkillBySlotIdx( i, itemInfo, isSkillSlotActive );
		}

		if( weaponSkill == NULL ){
			continue;
		}
		
		showSkill = TRUE;

		//Name
		FString skillName = TEXT("");
		skillName += UITextParser::HTMLBeginParagraph() 
					 + UITextParser::HTMLBeginFont( isSkillSlotActive ? UITextParser::GetTipsColor(TEXT("SkillNameColor")):UITextParser::GetTipsColor(TEXT("DisableColor")));

		//
		UMMOAbility *pMainSkill = mmoSkillMgr.GetMainAbilityFor( weaponSkill );
		if( pMainSkill != NULL ){
			skillName += pMainSkill->eventGetAbilityName();
		}
		skillName += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Tips.WeaponSkillName") ), *( weaponSkill->eventGetAbilityName() ) );
		//skillName += weaponSkill->ClientCommonAttribute.AbilityName;
		skillName += UITextParser::HTMLEndFont() + UITextParser::HTMLEndParagraph();
		skillNameArray->SetElementString(i, skillName);

		//Icon
		FString skillIconPath = weaponSkill->GetAbilityIconName();
		if( !isSkillSlotActive ){
			 skillIconPath += TEXT("_g");
		}
		skillIconNameArray->SetElementString(i, skillIconPath);

		//Desc
		UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( weaponSkill->SpellCommonAttribute.Id, weaponSkill->SpellCommonAttribute.Level, weaponSkill->SpellCommonAttribute.RuneSlotID );
		const SpellVarData* spellDat = SpellVarManager::getSingleton().FindData( uniqueSkillID );
		if( spellDat ){
			skillDescArray->SetElementString( i, spellDat->RuneDescription );
		}
	}

Exit0:
	return showSkill;
}
//------------------------------------------------------------------------
FString UITextParser::HTMLFormatEquipSuitAttribute( UEquip* item )
{
	FString tips = TEXT("");
	FString tipsSuitTitle = TEXT("");
	FString suitNameStr = TEXT("");
	FString tipsSuitInfo = TEXT("");

	MMOEquipManager *equipMgr = &MMOEquipManager::getSingleton();
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	LOG_PROCESS_ERROR(equipMgr && item);

	//套装属性
	EquipSuit* pSuit =  equipMgr->FindEquipSuitByPartOfSuit( item->ItemId );
	LOG_PROCESS_ERROR(pSuit);

	//Suit parts
	UINT partsCnt = pSuit->_PartsCnt;
	UINT totalSuitPartHave = 0;
	for( UINT i = 0; i < partsCnt; ++i )
	{
		EquipSuitPart &suitPat = pSuit->_Parts[i];
		ItemInfo* itemInfo = mmoFacade.FindItem( E_CONSLOT_EQUIPPED, suitPat._EquipSlot );

		bool isHaveSuit = ( itemInfo != NULL && ( itemInfo->m_nTemplateID == suitPat._ItemID ) );
		if( isHaveSuit )
		{
			++totalSuitPartHave;
		}
		else{
			switch( suitPat._EquipSlot )
			{
			case E_NES_FINGER_L: itemInfo = mmoFacade.FindItem( E_CONSLOT_EQUIPPED, E_NES_FINGER_R ); break;
			case E_NES_FINGER_R: itemInfo = mmoFacade.FindItem( E_CONSLOT_EQUIPPED, E_NES_FINGER_L ); break;
			case E_NES_WRIST_L: itemInfo = mmoFacade.FindItem( E_CONSLOT_EQUIPPED, E_NES_WRIST_R ); break;
			case E_NES_WRIST_R: itemInfo = mmoFacade.FindItem( E_CONSLOT_EQUIPPED, E_NES_WRIST_L ); break;
			}
			isHaveSuit = ( itemInfo != NULL && ( itemInfo->m_nTemplateID == suitPat._ItemID ) );
			if( isHaveSuit )
			{
				++totalSuitPartHave;
			}
		}

		tipsSuitInfo += UITextParser::HTMLBeginParagraph() + UITextParser::HTMLBeginFont( isHaveSuit ? UITextParser::GetTipsColor(TEXT("SuitHaveColor")) : UITextParser::GetTipsColor(TEXT("SuitNotHaveColor")))
			+ suitPat.GetDisplayName()
			+ UITextParser::HTMLEndFont() + UITextParser::HTMLEndParagraph();
	}

	//Suit name
	suitNameStr = FString::Printf( TEXT("%s( %d/%d )"), *pSuit->_SuitName, totalSuitPartHave, pSuit->_PartsCnt );
	tipsSuitTitle = UITextParser::HTMLBeginParagraph() + UITextParser::HTMLBeginFont( UITextParser::GetTipsColor(TEXT("SuitColor")) ) 
		+ suitNameStr + UITextParser::HTMLEndFont() + UITextParser::HTMLEndParagraph();

	//
	tips = tipsSuitTitle + tipsSuitInfo;

	UINT propCnt = pSuit->_PropsCnt;
	for( UINT i = 0; i < propCnt; ++i )
	{
		bool isConditionOK = ( pSuit->_Props[i]._Condition <= totalSuitPartHave );
		const ItemRandomAttr* pAttr = mmoFacade.GetRandomAttributById( pSuit->_Props[i]._PropID );
		if( pAttr == NULL )
			continue;

		tips += UITextParser::HTMLBeginParagraph() + UITextParser::HTMLBeginFont( isConditionOK ? UITextParser::GetTipsColor(TEXT("SuitHaveColor")) : UITextParser::GetTipsColor(TEXT("SuitNotHaveColor")) );
		tips += appItoa( pSuit->_Props[i]._Condition ) + MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TipsSuit.Unit"));
		tips += pAttr->strText;
		tips += UITextParser::HTMLEndFont() + UITextParser::HTMLEndParagraph();
	}

Exit0:
	return tips;
}

FString UITextParser::HTMLFormatEquipSignAttribute(ItemInfo* itemInfo)
{
	FString tips = TEXT("");
	
	if (itemInfo && itemInfo->m_dwRoleID != 0)
	{
		PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(itemInfo->m_dwRoleID);
		if (NULL != pPlayerBaseInfo)
		{
			tips = HTMLBeginParagraph() + HTMLBeginFont(GetTipsColor(TEXT("SignColor"))) 
				+ MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.ItemSign.Sign")) + pPlayerBaseInfo->_RoleName + HTMLEndFont()+ HTMLEndParagraph();  
		}

	}
	return tips;
}

FString UITextParser::HTMLFormatEquipWeatherAttribute(UEquip* item)
{
	FString tips = TEXT("");
	if (NULL != item && item->FixAttrbutes.WeatherAttrID > 0)
	{
		BOOL bActivate = FALSE;
		const ItemRandomAttr* pRandomAttr = MMOFacade::getSingleton().GetRandomAttributById(item->FixAttrbutes.WeatherAttrID);
		LOG_PROCESS_ERROR(pRandomAttr);
		INT curHour = MMOWeatherManager::getSingleton().GetCurTime();
		ESCENE_WEATHER_TYPE curWeatherType =  (ESCENE_WEATHER_TYPE)MMOWeatherManager::getSingleton().GetCurWeatherType();
		switch(pRandomAttr->dayType)
		{
		case E_sdt_Day:
			{
				if ((8 <= curHour && curHour < 20) 
					&& ((pRandomAttr->weatherType == curWeatherType) || (pRandomAttr->weatherType == -1)))
					bActivate = TRUE;
				else
					bActivate = FALSE;
			}
			break;
		case E_sdt_Night:
			{
				if (((20 <= curHour && curHour <= 24) ||(0 <= curHour && curHour < 8))
					&& ((pRandomAttr->weatherType == curWeatherType) || (pRandomAttr->weatherType == -1)))
				{
					bActivate = TRUE;
				}
				else
				{
					bActivate = FALSE;
				}
			}
			break;
		default:
			{
				if (pRandomAttr->weatherType == curWeatherType)
					bActivate = TRUE;
				else
					bActivate = FALSE;
			}
			break;
		}

		tips += HTMLBeginParagraph() + HTMLBeginFont( GetTipsColor(TEXT("TitleColor")) ) + MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.WeatherAttr.Title"))
			+ HTMLEndFont() + HTMLEndParagraph();
		tips += HTMLBeginParagraph() + HTMLBeginFont( bActivate ? GetTipsColor(TEXT("WeatherColor")) : GetTipsColor(TEXT("WeatherNotColor")))
			+ pRandomAttr->strText + HTMLEndFont() + HTMLEndParagraph();
	}

Exit0:
	return tips;
}

FString UITextParser::HTMLFormatItemDecompose(UItem* item)
{
	FString tips=TEXT("");
	LOG_PROCESS_ERROR(item);

	if( !item->CanDecompose && ( item->ItemType == TEXT("ITEMTYPE_SPELLBOOK") || item->ItemType == TEXT("ITEMTYPE_EQUIP") ) )
	{
		tips += HTMLBeginFont( GetTipsColor(TEXT("NormalContentColor")) );
		tips +=MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.ForbidDecompose"));
		tips +=HTMLEndFont();
	}

Exit0:
	return tips;
}

FString UITextParser::HTMLFormatEquipRepairState( UEquip* equip, ItemInfo* itemInfo )
{
	FString tips = TEXT("");
	LOG_PROCESS_ERROR( equip );
	LOG_PROCESS_ERROR( itemInfo );

	if( itemInfo->m_nMaxDur <= 0 || !equip->CanRepair ){
		tips = UITextParser::HTMLBeginFont( UITextParser::GetTipsColor( TEXT("AlertColor") ) );
		tips += MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.Item.RepairCannot") );
		tips += UITextParser::HTMLEndFont();
	}

Exit0:
	return tips;
}
//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemMaterial( UItem* item )
{
    FString tips=TEXT("");

    if( !item )
    {
        return tips;
    }

    if( item->ItemSubType.IsEmpty() )
    {
        tips += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Debug.ItemSubTypeCfgError"));
    }
    else
    {
        if( item->ItemSubSubType != TEXT("None") )
        {
            tips +=  MMOUI::ParseLocalizedPropertyPath( FString(TEXT("MMOGame.ItemSubTypes.")) + item->ItemSubSubType );
        }
    }

    return tips;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemBindType( UItem* item , ItemInfo* itemInfo )
{
    FString tips=TEXT("");

	INT bindType = BINDTYPE_NONE;
	if( itemInfo != NULL ){
		bindType = itemInfo->m_byBindType;
	}
	else if( item != NULL ){
		bindType = item->ItemBindType;
	}

	switch( bindType )
	{
	case E_BINDTYPE_BINDED:
		{
			tips += HTMLBeginFont( GetTipsColor(TEXT("AlertColor")) );
			tips +=MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.BindType.Binded"));
			tips +=HTMLEndFont();
			break;
		}
	case E_BINDTYPE_BOU:
		tips = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.BindType.onUse"));
		break;
	case E_BINDTYPE_BOE:
		tips = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.BindType.onEquip"));
		break;
	case E_BINDTYPE_BOP:
		tips = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.BindType.onPurchase"));
		break;
	case E_BINDTYPE_BOT:
		tips = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.BindType.onTrade"));
		break;
	}

    return tips;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemPower(ItemInfo* pItemInfo)
{
	FClientItemInfo* pClientItemInfo = (FClientItemInfo*)pItemInfo;
	if( pClientItemInfo == NULL ){
		return TEXT("");
	}

	if( pClientItemInfo->Pow <= 0 ){
		MMOPowerManager::getSingleton().UpdateEquipPower( pClientItemInfo );
	}

	if( pClientItemInfo->Pow >= 0 ){
		return FString::Printf(TEXT("%d"), pClientItemInfo->Pow);
	}

	return TEXT("");
}
//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemPower(UEquip* pEquip)
{
	if( pEquip != NULL ){
		INT basePower = pEquip->FixAttrbutes.BasePow;
		if(basePower)
			return FString::Printf(TEXT("%d"), basePower);
	}

	return TEXT("");
}
//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemGender(UItem* item)
{
	FString tips = TEXT("");
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	LOG_PROCESS_ERROR(item && mainPlayer);
	if( item->RequireGender == EGT_None ){
		return tips;
	}

	UActorAttribute* pActorAttributes = mainPlayer->ActorAttributes;
	LOG_PROCESS_ERROR(pActorAttributes);
	EGenderType gender = (EGenderType)pActorAttributes->GenderType;
	tips += HTMLBeginParagraph();
	if( gender == item->RequireGender ){
		tips += HTMLBeginFont( GetTipsColor(TEXT("NormalContentColor")) );
	}
	else{
		tips += HTMLBeginFont( GetTipsColor(TEXT("AlertColor")) );
	}

	if(EGT_Male == item->RequireGender)
	{
		tips += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.GenderMale"));		
	}
	else if (EGT_Female == item->RequireGender)
	{
		tips += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.GenderFeMale"));
	}
	tips += HTMLEndFont() + HTMLEndParagraph();
Exit0:
	return tips;
}

FString UITextParser::HTMLFormatItemPrice( UItem* item, ItemInfo *itemInfo )
{
	FString tips = TEXT("");
	LOG_PROCESS_ERROR(item);

	INT bindType = BINDTYPE_NONE;
	if( itemInfo != NULL ){
		bindType = itemInfo->m_byBindType;
	}
	else if( item != NULL ){
		bindType = item->ItemBindType;
	}

	if(item->CanSell && bindType == E_BINDTYPE_NONE)
	{
		tips += HTMLBeginFont(GetTipsColor(TEXT("FootColor")));
		tips += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.Price"));
		tips += HTMLEndFont();
		INT price = MMOFacade::getSingleton().GetEquipNpcPrice(item, itemInfo);
		tips += UITextParser::HTMLMoneyIconString(MONEYTYPE_02, price);
	}
Exit0:
	return tips;
}

FString UITextParser::HTMLFormatStallItemPrice(const StallItemInfo& stallItemInfo)
{
	FString tips = TEXT("");
	if(stallItemInfo.Price)
	{
		tips += HTMLBeginFont(GetTipsColor(TEXT("FootColor")));
		tips += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.StallPrice"));
		tips += HTMLEndFont();
		tips += UITextParser::HTMLMoneyIconString(MONEYTYPE_02, stallItemInfo.Price);
	}

	return tips;
}

FString UITextParser::HTMLFormatItemRequireReputation( const ServerShopItemData* shopItem )
{
	FString res;
	if(shopItem)
	{
		UMMOUICharacter* panel = Cast<UMMOUICharacter>(MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_Character));
		if ( panel && shopItem->RequireForceID >= 0 )
		{
			AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
			INT curReputation = MMOFacade::getSingleton().GetPlayerReputation( player , shopItem->RequireForceID );

			res += HTMLBeginFont( curReputation >= shopItem->Reputation ? GetTipsColor(TEXT("NormalContentColor")):GetTipsColor(TEXT("AlertColor")));
			
			FString reputationName = panel->GetReputationForceName( shopItem->RequireForceID );
			FString reputationLevel = panel->GetReputationLevelName( shopItem->RequireForceID,shopItem->Reputation );
			res += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Tips.ReputationText") ) , *reputationName , *reputationLevel )
				+ HTMLEndFont();
			
		}
	}
	return res;
}

FString UITextParser::HTMLFormatItemRequireGuild( const ServerShopItemData* shopItem )
{
	FString res;
	if(shopItem)
	{
		if( MMOGuildData::getSingleton().GetGuildBaseInfo().Id > 0 && shopItem->BuildingLevel > 0 )
		{
			INT level = MMOGuildData::getSingleton().GetGuildBuildingMaxLevel( shopItem->RequireGuildBuildingID );
			GuildBuildingData buildingData;
			MMOGuildData::getSingleton().GetGuildBuildingData( shopItem->RequireGuildBuildingID , 1 , buildingData);

			res += HTMLBeginFont( level >= shopItem->BuildingLevel ? GetTipsColor(TEXT("NormalContentColor")):GetTipsColor(TEXT("AlertColor")));
						
			res += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Tips.GuildText") ) , *buildingData.Name , shopItem->BuildingLevel )
				+ HTMLEndFont();
		}
		
	}
	return res;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatBaseAttribute( UItem* item, ItemInfo* itemInfo )
{
    FString tips = HTMLBeginFont(HTMLGetItemColor(EIC_Color2)) + HTMLFormatItemType(item) + HTMLNewLine();

    if(item->ItemDuration == E_DURTYPE_USE)
    {
        tips += FString::Printf(TEXT("<p>%s</p>"), *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Item.UseCount")));
    }
    else if(item->ItemDuration == E_DURTYPE_EQUIP)
    {
        if(item->MaxStackNum > EQUIP_DUR_MULTIPLIER)
        {
            tips += FString::Printf(TEXT("<p>%s</p>"), *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Item.DurationType")) );
        }
        else
        {
            tips += FString::Printf(TEXT("<p>%s</p>"), *MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.Item.DurationType")) );
        }
    }

    tips += HTMLEndFont();
    return tips;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemRequireProfession( UItem* item )
{
    FString tips = TEXT("");
    if(MMO::IsForAllPlayerClass(item->RequireClass) == FALSE)
    {
        const FString NullStr = TEXT("");
        const FString SpaceStr = TEXT(" ");
        EPlayerClass playerClass = (EPlayerClass)MMOFacade::getSingleton().GetMainPlayer()->GetClassId();
		FString classText = (MMO::CanBeUsedByPlayerClass(EPCT_PlayerClassA, item->RequireClass) == TRUE ? MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.School.SchoolName0")) + SpaceStr : NullStr) +
            (MMO::CanBeUsedByPlayerClass(EPCT_PlayerClassB, item->RequireClass) == TRUE ? MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.School.SchoolName1")) + SpaceStr : NullStr) +
            (MMO::CanBeUsedByPlayerClass(EPCT_PlayerClassC, item->RequireClass) == TRUE ? MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.School.SchoolName2")) + SpaceStr : NullStr) +
            (MMO::CanBeUsedByPlayerClass(EPCT_PlayerClassD, item->RequireClass) == TRUE ? MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.School.SchoolName3")) + SpaceStr : NullStr) +
			(MMO::CanBeUsedByPlayerClass(EPCT_PlayerClassE, item->RequireClass) == TRUE ? MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.School.SchoolName4")) + SpaceStr : NullStr) +
			(MMO::CanBeUsedByPlayerClass(EPCT_PlayerClassG, item->RequireClass) == TRUE ? MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.EPlayerClass.E_PlayerClassG")) + SpaceStr : NullStr) +
			(MMO::CanBeUsedByPlayerClass(EPCT_PlayerClassH, item->RequireClass) == TRUE ? MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.EPlayerClass.E_PlayerClassH")) + SpaceStr : NullStr)
            ;

        FString text = FString::Printf(TEXT("<p>%s</p>"), *classText);

        if(MMO::CanBeUsedByPlayerClass(playerClass, item->RequireClass) == FALSE)
        {
            tips += HTMLBeginFont(HTMLGetItemColor(EIC_Color7)) + text + HTMLEndFont();
        }
        else
        {
            tips += HTMLBeginFont(HTMLGetItemColor(EIC_Color1)) + text + HTMLEndFont();
        }
    }

    return tips;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemRequireLevel( UItem* item )
{
    FString tips = TEXT("");

    if(item->RequireLevel > 0)
    {
		tips+= MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.RequireLvTitle"));
        if(MMOFacade::getSingleton().GetMyLevel() < item->RequireLevel)
        {
            tips += HTMLBeginFont(GetTipsColor(TEXT("AlertColor")));
			tips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.RequireLv")), item->RequireLevel);
			tips += HTMLEndFont();
        }
        else
        {
			tips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.RequireLv")), item->RequireLevel);
        }
    }

    return tips;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemRequireAll( UItem* item )
{
	FString tips = TEXT("");
	//等级需求
	if(item->RequireLevel > 0)
	{
		tips+= MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.RequireLvTitle"));
		if(MMOFacade::getSingleton().GetMyLevel() < item->RequireLevel)
		{
			tips += HTMLBeginFont(GetTipsColor(TEXT("AlertColor")));
			tips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.RequireLv")), item->RequireLevel);
			tips += HTMLEndFont();
		}
		else
		{
			tips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.RequireLv")), item->RequireLevel);
		}
	}
	UEquip *pEquip = Cast<UEquip>( item );
	if( pEquip != NULL ){
		//物理攻击需求
		if(pEquip->RequireMaxPhyAtk > 0)
		{
			tips += ( tips.Len() > 0 ? HTMLNewLine() : TEXT("") );
			tips += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.RequireMaxPhyAtkTitle"));
			INT curPhyAtk = MMOAttributeAccountManager::getSingleton().GetOwnerAttribute( catMaxPhyAtk, AttrListenerType_Mine );
			if(curPhyAtk < pEquip->RequireMaxPhyAtk)
			{
				tips += HTMLBeginFont(GetTipsColor(TEXT("AlertColor")));
				tips += appItoa(pEquip->RequireMaxPhyAtk);
				tips += HTMLEndFont();
			}
			else
			{
				tips += appItoa(pEquip->RequireMaxPhyAtk);
			}
		}
		//魔法力需求
		if(pEquip->RequireMaxMagAtk > 0)
		{
			tips += ( tips.Len() > 0 ? HTMLNewLine() : TEXT("") );
			tips += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.RequireMaxMagAtkTitle"));
			INT curMagAtk = MMOAttributeAccountManager::getSingleton().GetOwnerAttribute( catMaxMagAtk, AttrListenerType_Mine );
			if(curMagAtk < pEquip->RequireMaxMagAtk)
			{
				tips += HTMLBeginFont(GetTipsColor(TEXT("AlertColor")));
				tips += appItoa(pEquip->RequireMaxMagAtk);
				tips += HTMLEndFont();
			}
			else
			{
				tips += appItoa(pEquip->RequireMaxMagAtk);
			}
		}
		//道术需求
		if(pEquip->RequireMaxTaoAtk > 0)
		{
			tips += ( tips.Len() > 0 ? HTMLNewLine() : TEXT("") );
			tips += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.RequireMaxTaoAtkTitle"));
			INT curTaoAtk = MMOAttributeAccountManager::getSingleton().GetOwnerAttribute( catMaxTaoAtk, AttrListenerType_Mine );
			if(curTaoAtk < pEquip->RequireMaxTaoAtk)
			{
				tips += HTMLBeginFont(GetTipsColor(TEXT("AlertColor")));
				tips += appItoa(pEquip->RequireMaxTaoAtk);
				tips += HTMLEndFont();
			}
			else
			{
				tips += appItoa(pEquip->RequireMaxTaoAtk);
			}
		}
	}
	return tips;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemLevel( UItem* item )
{
	FString tips = TEXT("");

	if(item->ItemLevel > 0)
	{
		tips += HTMLBeginParagraph() + HTMLBeginFont( GetTipsColor(TEXT("ItemLevColor")) ) ;
		tips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.ItemLv")), item->ItemLevel);
		tips += HTMLEndFont() + HTMLEndParagraph();
	}

	return tips;
}


//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemDuration( UItem* item, ItemInfo* itemInfo )
{
    FString tips = TEXT(""); 

	switch( item->ItemDuration )
	{
	case E_DURTYPE_USE:
		{
			tips += HTMLBeginFont(GetTipsColor(TEXT("FootColor"))) + MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.UseTimes")) + HTMLEndFont();
			tips += FString::Printf(TEXT("<p >%d</p>"), itemInfo ? itemInfo->m_nCurDur : item->DefaultDuration);
		}
		break;
	case E_DURTYPE_EQUIP:
		{
			tips += HTMLBeginFont(GetTipsColor(TEXT("FootColor"))) + MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.Duration")) + HTMLEndFont();
			if(itemInfo)
			{
				tips += HTMLBeginParagraph();
				INT curDur = (INT)((itemInfo->m_nCurDur + EQUIP_DUR_MULTIPLIER -1) / EQUIP_DUR_MULTIPLIER);
				INT curMaxDur = (INT)((itemInfo->m_nMaxDur + EQUIP_DUR_MULTIPLIER -1) / EQUIP_DUR_MULTIPLIER);
				INT origMaxDur = item->DefaultDuration;

				UBOOL needColor = FALSE;
				if( origMaxDur == 0 || curDur == 0 ){
					tips += UITextParser::HTMLBeginFont( UITextParser::GetTipsColor( TEXT("AlertColor") ) );
					needColor = TRUE;
				}
				else{
					if( ( curMaxDur <= (INT)(0.2 * origMaxDur) ) || ( curDur <= (INT)(0.2 * curMaxDur) ) ){
						tips += UITextParser::HTMLBeginFont( UITextParser::GetTipsColor( TEXT("AlarmPink") ) );
						needColor = TRUE;
					}
				}

				tips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.DurationValue")), curDur, curMaxDur );

				if( needColor ){
					tips += UITextParser::HTMLEndFont();
				}
				tips += HTMLEndParagraph();
			}
			else// 如果是模板道具
			{
				tips += FString::Printf(TEXT("<p >%d/%d</p>"), item->DefaultDuration, item->DefaultDuration) ;
			}
		}
		break;
	case E_DURTYPE_DEPOSIT:
	case E_DURTYPE_RECOVER:
		{
			if( itemInfo )
			{
				tips += HTMLBeginFont(GetTipsColor(TEXT("FootColor"))) + MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Recoverslot.SlotTips")) + HTMLEndFont();

				const INT per = 10000;
				FString wanText = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Tips.UnitWan") );
				FString curNum;
				if( itemInfo->m_nCurDur >= per ){
					curNum = appItoa( INT( itemInfo->m_nCurDur / per ) );
					curNum += wanText;
					INT decimal = itemInfo->m_nCurDur % per;
					if( decimal > 0 ){
						curNum += appItoa( decimal );
					}
				}
				else{
					curNum = appItoa( itemInfo->m_nCurDur );
				}
				INT curWan = itemInfo->m_nCurDur / per;
				tips += FString::Printf(TEXT("<p >%s/%d%s</p>"), *curNum, (item->DefaultDuration/per), *wanText );
			}
			else
			{
				tips = TEXT("");
			}
		}
		break;
	case E_DURTYPE_NONE:
	default:
		{
			tips = TEXT("");
		}
		break;
	}

    return tips;
}

FString UITextParser::HTMLFormatItemForbidSell(UItem* item , ItemInfo* itemInfo )
{
	FString tips = TEXT("");
	LOG_PROCESS_ERROR(item);

	INT bindType = BINDTYPE_NONE;
	if( itemInfo != NULL ){
		bindType = itemInfo->m_byBindType;
	}
	else if( item != NULL ){
		bindType = item->ItemBindType;
	}

	if(bindType != E_BINDTYPE_NONE || !item->CanSell  )
	{
		tips += HTMLBeginFont(GetTipsColor(TEXT("AlertColor"))) + MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.ItemAttr.ForbidSell"))
			+ HTMLEndFont();
	}
	
Exit0:
	return tips;
}

//FString UITextParser::HTMLFormatItemForbidTrade(UItem* item)
//{
//	FString tips = TEXT("");
//	LOG_PROCESS_ERROR(item);
//	if (!item->Trade)
//	{
//		tips += HTMLBeginFont(GetTipsColor(TEXT("AlertColor"))) + MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.ItemAttr.ForbidTrade"))
//			+ HTMLEndFont();
//	}
//
//Exit0:
//	return tips;
//}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatItemDescription( UItem* item )
{
	 FString tips;
    // description
	if( item->Description.IsEmpty() ){
		return tips;
	}

	tips = HTMLBeginFont( GetTipsColor(TEXT("DescribeColor")) ); // TODO: move to config
    tips += FString::Printf(TEXT("%s"), *item->Description) + HTMLNewLine();
    return tips;
}

//------------------------------------------------------------------------
INT UITextParser::HTMLFormatCostText( BYTE costType, INT costValue, FString& outCostActionText, FString& outCostNameText )
{
    FString costName;
    switch(costType)
    {
    case ES_COST_Type1:
        costName = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.ESpell_CostType.SPELL_COST_Type1"));//TEXT("怒气");
        break;

    case ES_COST_Type2:
        costName = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.ESpell_CostType.SPELL_COST_Type2"));//TEXT("灵能");
        break;

    case ES_COST_Type3:
        costName = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.ESpell_CostType.SPELL_COST_Type3"));//TEXT("魔力");
        break;

    case ES_COST_Type4:
        //if(costValue >= 0)
            costName = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.ESpell_CostType.SPELL_COST_Type4"));//TEXT("阳力");
        //else
        //    costName = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.ESpell_CostType.SPELL_COST_Type5"));//TEXT("阴力");
        break;
    };
    outCostNameText = costName;

    FString action;
    INT k = 1;
    //if(costValue >= 0 || costType == ES_COST_Type4)
    //{
    //    action = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Skill.Cost"));

    //    if(costType == ES_COST_Type4 && costValue < 0)
    //        k = -1;
    //}
	if( costValue >= 0 )
	{
		action = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Skill.Cost"));
	}
    else
    {
        action = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Skill.Generate"));
        k = -1;
    }
    outCostActionText = action;

    return k;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatWeaponTypesText(const FSpell_WeaponTypeL* typesL, const FSpell_WeaponTypeR* typesR)
{
    int count = 0;
    int singleLimt = EWT_Weapon_0;

	FString ret = FormatSpellWeaponLimitText( typesL, typesR, count, singleLimt );

    int wType = EWT_Weapon_0;
    AMMOPlayer *player = MMOFacade::getSingleton().GetMainPlayer();
    if( player )
        wType = EWeaponType( player->GetWeaponType() );

    //规则改变，只要勾选大于2种武器，就默认显示通用 
    if( count > 1 )
    {
        ret = HTMLBeginFont( GetTipsColor(TEXT("NormalContentColor")) )+MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Skill.General")) + HTMLEndFont();
    }
    else if( wType == singleLimt )
    {
        ret = HTMLBeginFont( GetTipsColor(TEXT("NormalContentColor")) ) + ret + HTMLEndFont();
    }
    else
    {
        //不适合
        ret = HTMLBeginFont( GetTipsColor(TEXT("AlertColor")) )+ ret + HTMLEndFont();
    }

    return ret;
}

//------------------------------------------------------------------------
FString UITextParser::FormatSpellWeaponLimitText(const FSpell_WeaponTypeL* typesL, const FSpell_WeaponTypeR* typesR, INT &canUseWeaponCnt, int &singleLimt)
{
    FString ret;
    singleLimt = EWT_Weapon_0;
	canUseWeaponCnt = 0;

    if(typesR)
    {
        WeaponTypeHelperMacro2(R, 0, 0, canUseWeaponCnt, singleLimt );
        WeaponTypeHelperMacro2(R, 1, 1, canUseWeaponCnt, singleLimt );
        WeaponTypeHelperMacro2(R, 2, 2, canUseWeaponCnt, singleLimt );
        WeaponTypeHelperMacro2(R, 3, 3, canUseWeaponCnt, singleLimt );
        WeaponTypeHelperMacro2(R, 4, 4, canUseWeaponCnt, singleLimt );
        WeaponTypeHelperMacro2(R, 5, 5, canUseWeaponCnt, singleLimt );
        WeaponTypeHelperMacro2(R, 6, 6, canUseWeaponCnt, singleLimt );
        WeaponTypeHelperMacro2(R, 7, 7, canUseWeaponCnt, singleLimt );
        WeaponTypeHelperMacro2(R, 8, 8, canUseWeaponCnt, singleLimt );
        WeaponTypeHelperMacro2(R, 9, 9, canUseWeaponCnt, singleLimt );
        WeaponTypeHelperMacro2(R, 10, 10, canUseWeaponCnt, singleLimt );
        WeaponTypeHelperMacro2(R, 11, 11, canUseWeaponCnt, singleLimt );
        WeaponTypeHelperMacro2(R, 12, 12, canUseWeaponCnt, singleLimt );
        WeaponTypeHelperMacro2(R, 13, 13, canUseWeaponCnt, singleLimt );
        WeaponTypeHelperMacro2(R, 14, 14, canUseWeaponCnt, singleLimt );
    }

    if(typesL)
    {
        WeaponTypeHelperMacro2(L, 1, 15, canUseWeaponCnt, singleLimt );
        WeaponTypeHelperMacro2(L, 2, 16, canUseWeaponCnt, singleLimt );
        WeaponTypeHelperMacro2(L, 3, 17, canUseWeaponCnt, singleLimt );
    }

	return ret;
}

//------------------------------------------------------------------------
FString UITextParser::FormatSpellPlayerClassLimitText(const FSpell_ClassType &spellClassLimit)
{
	FString ret;

	//战士
	if( spellClassLimit.bClassType1 )
	{
		ret = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.MMOAbility:Spell_ClassType.ClassType1"));
		return ret;
	}

	//法师
	if( spellClassLimit.bClassType2 )
	{
		ret = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.MMOAbility:Spell_ClassType.ClassType2"));
		return ret;
	}

	//刺客
	if( spellClassLimit.bClassType3 )
	{
		ret = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.MMOAbility:Spell_ClassType.ClassType3"));
		return ret;
	}

	//弓箭手
	if( spellClassLimit.bClassType4 )
	{
		ret = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.MMOAbility:Spell_ClassType.ClassType4"));
		return ret;
	}

	//NPC
	if( spellClassLimit.bClassType5 )
	{
		ret = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.MMOAbility:Spell_ClassType.ClassType5"));
		return ret;
	}

	//道士
	if( spellClassLimit.bClassType6 )
	{
		ret = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.MMOAbility:Spell_ClassType.ClassType6"));
		return ret;
	}

	// 1.05 Lancer / Sorcerer
	if( spellClassLimit.bClassType7 )
	{
		ret = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.MMOAbility:Spell_ClassType.ClassType7"));
		return ret;
	}
	if( spellClassLimit.bClassType8 )
	{
		ret = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.MMOAbility:Spell_ClassType.ClassType8"));
		return ret;
	}

	return ret;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatRuneItemTips(INT ActiveID, INT RuneSlot )
{
    FString tips;
    AMMOPlayer* MainPlayer = MMOFacade::getSingleton().GetMainPlayer();
    if( !MainPlayer )
        return "";
    UMMOAbility* tMainActive = MainPlayer->GetMainAbilityById( ActiveID );
    UMMOAbility* tRune=NULL;
    //显示当前技能所有的符文 
    if( RuneSlot >= 0 && tMainActive && RuneSlot < tMainActive->SubRuneList.Num() )
        tRune = tMainActive->SubRuneList( RuneSlot );
    if( !tRune )
        return "";

    tips += HTMLBeginFont( GetTipsColor(TEXT("NormalContentColor")) );
    tips += TEXT("《");
    tips += tRune->eventGetAbilityName();
    tips += TEXT("》");
    tips += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Skill.RuneDebrisNeed"));
    tips += HTMLNewLine();

    int ItemId;
    int ItemCount;
    //显示符文碎片需求
    for ( int i=0; i < tRune->vecRuneDebris.Num();++i )
    {
        ItemId = tRune->vecRuneDebris(i).ItemId;
        ItemCount = tRune->vecRuneDebris(i).ItemCount;
        if( ItemCount >= 0 )
        {
            UItem* item = MMOFacade::getSingleton().FindItemTemplate( ItemId );
            if( item )
            {
                tips += HTMLBeginFont(GetTipsColor(TEXT("NormalContentColor"))) + item->GetDisplayName() + FString(TEXT(" X "))+ appItoa( ItemCount )+ HTMLEndFont();
                tips += HTMLNewLine();
            }
        }
    }

    return tips;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatBuffTips( UBuff* pBuff ) //  BuffData* buffData )
{
	//if( !buffData || !buffData->Buff )
	//{
	//	return TEXT("");
	//}
	if( !pBuff )
	{
		return TEXT("");
	}

    FString tips = HTMLBeginFont( GetTipsColor(TEXT("TitleColor")) );
    tips += pBuff->BuffName.ToString() + HTMLNewLine() + HTMLNewLine() + HTMLEndFont();
    tips += HTMLBeginFont( GetTipsColor(TEXT("NormalContentColor")) );
    tips += pBuff->BuffDescription + HTMLNewLine() + HTMLNewLine() ;
    tips += HTMLEndFont();
    return tips;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatObject( UGFxObject* object )
{
	if(!object)
	{
		return TEXT("");
	}

    FString tips = HTMLBeginFont( GetTipsColor(TEXT("TitleColor")) );
    tips += object->GetString(TEXT("title")) + HTMLNewLine() + HTMLNewLine() + HTMLEndFont();

	FString text = object->GetString(TEXT("text"));
	if(!text.IsEmpty())
	{
		tips += HTMLBeginFont( GetTipsColor(TEXT("NormalContentColor")) );
		tips += text + HTMLNewLine() + HTMLNewLine() ;
		tips += HTMLEndFont();
	}
    return tips;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatSkillTipCost( UMMOAbility* skill )
{
    FString tips;// = HTMLBeginFont( GetTipsColor(TEXT("TitleColor")) ); 
    FString costName;
    FString costAction;
    INT k = 1;
    tips = HTMLBeginParagraph();
    //Initial 

    if( skill->SpellCommonAttribute.InitialCostValue != 0 )
    {
        k = HTMLFormatCostText(skill->SpellCommonAttribute.CostType, skill->SpellCommonAttribute.InitialCostValue, costAction, costName);
        tips += FString::Printf( TEXT( "%s%s："), *costAction, *costName);
        //tips += HTMLBeginFont( GetTipsColor(TEXT("NormalContentColor")) ) + FString::Printf(TEXT("%d"), skill->SpellCommonAttribute.InitialCostValue * k) + HTMLEndFont();
		tips += appItoa( skill->SpellCommonAttribute.InitialCostValue * k );
    }

    if( skill->SpellCommonAttribute.InitialCostValuePct != 0 )
    {
        k = HTMLFormatCostText(skill->SpellCommonAttribute.CostType, skill->SpellCommonAttribute.InitialCostValuePct, costAction, costName);
        tips += FString::Printf( TEXT("%s%s："), *costAction, *costName);
        //tips += HTMLBeginFont( GetTipsColor(TEXT("NormalContentColor")) ) + FString::Printf(TEXT( "%d"), skill->SpellCommonAttribute.InitialCostValuePct * k ) + HTMLEndFont();
		tips += appItoa( skill->SpellCommonAttribute.InitialCostValuePct * k );
    }
    //Modifier
    if( skill->SpellCommonAttribute.ModifierCostValue != 0 )
    {
        k = HTMLFormatCostText(skill->SpellCommonAttribute.CostType, skill->SpellCommonAttribute.ModifierCostValue, costAction, costName);
        tips += FString::Printf( TEXT("%s%s："), *costAction, *costName);
        //tips += HTMLBeginFont( GetTipsColor(TEXT("NormalContentColor")) ) + FString::Printf(TEXT( "%d"), skill->SpellCommonAttribute.ModifierCostValue * k ) + HTMLEndFont();
		tips += appItoa( skill->SpellCommonAttribute.ModifierCostValue * k );
    }
    if( skill->SpellCommonAttribute.ModifierCostValuePct != 0 )
    {
        k = HTMLFormatCostText(skill->SpellCommonAttribute.CostType, skill->SpellCommonAttribute.ModifierCostValuePct, costAction, costName);
        tips += FString::Printf( TEXT("%s%s："), *costAction, *costName);
        //tips += HTMLBeginFont( GetTipsColor(TEXT("NormalContentColor")) ) + FString::Printf(TEXT( "%d%%"), skill->SpellCommonAttribute.ModifierCostValuePct * k ) + HTMLEndFont();
		tips += FString::Printf(TEXT( "%d%%"), skill->SpellCommonAttribute.ModifierCostValuePct * k );
    }
    //Durative
    if( skill->SpellCommonAttribute.DurativeCostValue != 0 )
    {
        k = HTMLFormatCostText(skill->SpellCommonAttribute.CostType, skill->SpellCommonAttribute.DurativeCostValue, costAction, costName);
        tips += FString::Printf( TEXT("%s%s%s："), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Skill.Durative")), *costAction, *costName);
        int costPerSecond = int( float( skill->SpellCommonAttribute.DurativeCostValue ) / skill->SpellCommonAttribute.Interval );
        //tips += HTMLBeginFont( GetTipsColor(TEXT("NormalContentColor")) ) + FString::Printf( TEXT("%d"), costPerSecond ) + HTMLEndFont();
		tips += appItoa( costPerSecond );
    }
    if( skill->SpellCommonAttribute.DurativeCostValuePct != 0 )
    {
        k = HTMLFormatCostText(skill->SpellCommonAttribute.CostType, skill->SpellCommonAttribute.DurativeCostValuePct, costAction, costName);
        tips += FString::Printf( TEXT( "%s%s%s："), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Skill.Durative")), *costAction, *costName );
        //tips += HTMLBeginFont( GetTipsColor(TEXT("NormalContentColor")) ) + FString::Printf( TEXT( "%d"), skill->SpellCommonAttribute.DurativeCostValuePct * k) + HTMLEndFont();
		tips += appItoa( skill->SpellCommonAttribute.DurativeCostValuePct * k );
    }
    tips += HTMLEndParagraph();
    //tips += HTMLEndFont();

    return tips;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatSkillTipsLimit( UMMOAbility* skill )
{
    FString tips = TEXT(""); 			
    tips += HTMLBeginParagraph( );
    if(skill->SpellCommonAttribute.WeaponTypeR.bWeaponTypeR0) // 勾选空手
    {
        tips += HTMLBeginFont( GetTipsColor(TEXT("NormalContentColor")) ) + MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Skill.General"));
    }
    else
    {
        if(skill->SpellCommonAttribute.WeaponTypeR.bWeaponTypeR1 || skill->SpellCommonAttribute.WeaponTypeR.bWeaponTypeR2)
        {
            if(skill->SpellCommonAttribute.WeaponTypeL.bWeaponTypeL0) //左手为空手
            {
                tips += HTMLFormatWeaponTypesText(&skill->SpellCommonAttribute.WeaponTypeL, &skill->SpellCommonAttribute.WeaponTypeR);
            }

            else
            {
                tips += HTMLFormatWeaponTypesText(&skill->SpellCommonAttribute.WeaponTypeL, 0);
            }
        }
        else
        {  
            tips += HTMLFormatWeaponTypesText(&skill->SpellCommonAttribute.WeaponTypeL, &skill->SpellCommonAttribute.WeaponTypeR);
        }

    }
    tips += HTMLEndFont();
    tips += HTMLEndParagraph();

    return tips;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatSkillTipsUnlock( UMMOAbility* skill )
{
    AMMOPlayer *player = MMOFacade::getSingleton().GetMainPlayer();
	if( player == NULL ){
		return TEXT("");
	}

    //解锁后 就不再显示
    int requireLv = skill->SpellCommonAttribute.RequireLevel < 1 ? 1 : skill->SpellCommonAttribute.RequireLevel ;
    if( player->ActorAttributes->LevelAttributes.Level >= requireLv ){
		return TEXT("");
	} 

	//检查技能职业需求
	const FSpell_ClassType &skillReqTyp = skill->SpellCommonAttribute.Class;
	UBOOL isFitForMyClass = MMO::CanSpellBeUsedByPlayerClass( EPlayerClass(player->GetClassId()), skillReqTyp );
	if( isFitForMyClass != TRUE ){
		return TEXT("");
	}

	//
	FString tips;
	tips += FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Skill.UnlockLv")), requireLv );
    return tips;
}

FString UITextParser::HTMLFormatSkillTipsSkillLv( UMMOAbility* skill , UMMOAbility* originSkill )
{
	if( !skill )
	{
		return TEXT("");
	}

	UBOOL same = TRUE;

	if(originSkill)
	{
		same = skill->SpellCommonAttribute.Level == originSkill->SpellCommonAttribute.Level;
	}

	FString cLv = TEXT("");
	if( !same )
	{
		cLv += HTMLBeginFont( GetNormalColor(TEXT("Increase") ) );
	}
	//MMOGame.SkillTips.Lev
	cLv += FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.SkillLev")) , skill->SpellCommonAttribute.Level );
	if( !same )
	{
		cLv += HTMLEndFont( );
	}

	return cLv;
}

FString UITextParser::HTMLFormatSkillTipsRuneLv(UMMOAbility* skill , UMMOAbility* originSkill )
{
	if( !skill )
	{
		return TEXT("");
	}

	UBOOL same = TRUE;

	if(originSkill)
	{
		same = skill->SpellCommonAttribute.Level == originSkill->SpellCommonAttribute.Level;
	}

	FString cLv = TEXT("");
	if( !same )
	{
		cLv += HTMLBeginFont( GetNormalColor(TEXT("Increase") ) );
	}
	//MMOGame.SkillTips.Lev
	cLv += FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.RuneLev")) , skill->SpellCommonAttribute.Level );
	if( !same )
	{
		cLv += HTMLEndFont( );
	}

	return cLv;
}

FString UITextParser::HTMLFormatSkillTipsCombineLv( UMMOAbility* skill, UMMOAbility* mainSkill, UMMOAbility* originSkill )
{
	if( !skill )
	{
		return TEXT("");
	}

	UBOOL same = TRUE;
	INT lv = -1;
	
	if( originSkill )
	{
		same = ( skill->SpellCommonAttribute.Level == originSkill->SpellCommonAttribute.Level );
		lv = skill->SpellCommonAttribute.Level;
	}
	else
	{
		lv = skill->SpellCommonAttribute.Level;
	}

	//UBOOL isRuneSkill = ( skill->GetRuneSlot() > 0 );
	//if( originSkill )
	//{
	//	if( isRuneSkill )
	//	{
	//		//符文技能
	//		if( mainSkill != NULL )
	//		{
	//			INT curLv = Min<INT>( skill->SpellCommonAttribute.Level , mainSkill->SpellCommonAttribute.Level );
	//			INT oriLv = Min<INT>( originSkill->SpellCommonAttribute.Level , mainSkill->SpellCommonAttribute.Level );

	//			same = ( curLv == oriLv );
	//			lv = curLv;
	//		}
	//		else
	//		{
	//			lv = 0;
	//		}
	//	}
	//	else
	//	{
	//		//不是符文技能
	//		same = ( skill->SpellCommonAttribute.Level == originSkill->SpellCommonAttribute.Level );
	//		lv = skill->SpellCommonAttribute.Level;
	//	}
	//}
	//else
	//{
	//	if( mainSkill != NULL )
	//	{
	//		lv = Min<INT>( skill->SpellCommonAttribute.Level , mainSkill->SpellCommonAttribute.Level );
	//	}
	//	else
	//	{
	//		lv = 0;
	//	}
	//}
	
	FString cLv = TEXT("");
	if( !same )
	{
		cLv += HTMLBeginFont( GetNormalColor(TEXT("Increase") ) );
	}

	if( skill )
	{
		cLv += FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.ValidLev")) , lv );
	}
	else
	{
		cLv += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Tips.SkillLevNoLevel") );
	}

	if( !same )
	{
		cLv += HTMLEndFont( );
	}
	return cLv;
}

FString UITextParser::HTMLFormatSkillTipsDesc( UMMOAbility* skill , UMMOAbility* originSkill)
{
	if( !skill )
	{
		return TEXT("");
	}
	if( originSkill )
	{
		if( skill->GetAbilityDetailDescribe() == originSkill->GetAbilityDetailDescribe() )
		{
			return skill->GetAbilityDetailDescribe();
		}
		else
		{
			FString tips = TEXT("");
			int itemSize = Min<INT>( skill->SkillDetailDescribeArray.Num() , originSkill->SkillDetailDescribeArray.Num() );
			for ( int i = 0 ; i < itemSize ; i++ )
			{
				if( skill->SkillDetailDescribeArray(i) == originSkill->SkillDetailDescribeArray(i) )
				{
					tips += skill->SkillDetailDescribeArray(i);
					tips += TEXT("<br>");
				}
				else
				{
					tips += HTMLBeginFont( GetNormalColor(TEXT("Increase") ) );
					tips += skill->SkillDetailDescribeArray(i);
					tips += HTMLEndFont();
					tips += TEXT("<br>");
				}
			}
			
			return tips;
		}
	}

	return skill->GetAbilityDetailDescribe();	
}

FString UITextParser::HTMLFormatSkillTipsChangeSpecility( FString skillSp , FString originSkillSp )
{
	if( skillSp == originSkillSp )
	{
		return skillSp;
	}
	else
	{
		FString tips = HTMLBeginFont( GetNormalColor(TEXT("Increase") ) );
		tips += skillSp;
		tips += HTMLEndFont();
		return tips;
	}

	return skillSp;	
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatSkillTipsDamage( UMMOAbility* skill )
{
    FString tips = TEXT(""); 

    if( !skill )
    {
        return tips;
    }
    
    if( 0 == skill->SpellCommonAttribute.EffectPct )
    {
        //Buff类技能不会产生伤害 不作伤害显示 
         return tips;
    }

    //skill <P>第一段攻击</P> <P>(125%武器伤害值 + 50) 物理伤害 </P>";
    //如果是多段攻击 
    if( skill->SpellCommonAttribute.NextSpell > 0  )
    {
        int step = 1;
        UMMOAbility* cur = skill;
        do 
        {
            tips += HTMLBeginParagraph( );
            tips += FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Skill.SkillDamage")), step );
            tips += HTMLEndFont();

            if(  cur->SpellCommonAttribute.AttackType == E_spat_Mag )
            {
                tips += HTMLBeginParagraph( );
                tips += FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Skill.MagDamage")), cur->SpellCommonAttribute.EffectPct / 100, cur->SpellCommonAttribute.EffectValue );
                tips += HTMLEndFont();
            }
            else
            {
                tips += HTMLBeginParagraph( );
                tips += FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Skill.PhyDamage")), cur->SpellCommonAttribute.EffectPct / 100 , cur->SpellCommonAttribute.EffectValue );
                tips += HTMLEndFont();
            }
            UINT Key = MMOSkillManager::CombinateAbilityID( cur->SpellCommonAttribute.NextSpell, cur->SpellCommonAttribute.Level , cur->GetRuneSlot());
			//这里我小改下，如果nextspell配成自己，会死循环，我加个判断 by york
			UMMOAbility* next = MMOSkillManager::getSingleton().FindAbilityTemplate( Key );
			if( next == cur )
			{
				cur = NULL;
			}
			else
			{
				cur = next;
			}
            ++step;
        } while ( NULL != cur );

    }
    else
    {
        tips += HTMLBeginParagraph( );
        tips += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Skill.SkillDamageSingle"));
        tips += HTMLEndFont();

        if(  skill->SpellCommonAttribute.AttackType == E_spat_Mag )
        {
            tips += HTMLBeginParagraph( );
            tips += FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Skill.MagDamage")), skill->SpellCommonAttribute.EffectPct / 100, skill->SpellCommonAttribute.EffectValue );
            tips += HTMLEndFont();
        }
        else
        {
            tips += HTMLBeginParagraph( );
            tips += FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Skill.PhyDamage")), skill->SpellCommonAttribute.EffectPct / 100, skill->SpellCommonAttribute.EffectValue );
            tips += HTMLEndFont();
        }
    }
    return tips;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatSkillTipsNextNeedMaxProgress( UMMOAbility* skill )
{
    FString tips = TEXT(""); 	
	tips += HTMLBeginParagraph( );
	//INT SpellPoints = MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_SPELL_PTS);
	INT SpellPoints = 0;
	SkillDynamicData* skillDynData = MMOSkillManager::getSingleton().FindSkillExtraData( skill );
	if( skillDynData != NULL ){
		SpellPoints = skillDynData->_CurExp;
	}
	tips += HTMLBeginFont(SpellPoints >= skill->LvUpRequirePoints ? TEXT(""):GetTipsColor(TEXT("SkillLvUpNeed")));
	tips += FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Skill.SkillLvUpNeedMaxProgress")), skill->LvUpRequirePoints  );
	tips += HTMLEndFont();
	tips += HTMLEndParagraph();
    return tips;
}

//-------------------------------------------------------------------------
FString UITextParser::HTMLFormatSkillTipsNextNeedLevel( UMMOAbility* skill )
{
	FString tips = TEXT(""); 	
	tips += HTMLBeginParagraph( );
	INT level = MMOFacade::getSingleton().GetMyLevel();
	tips += HTMLBeginFont( level >= skill->LvUpRequireRoleLevel ? TEXT(""):GetTipsColor(TEXT("SkillLvUpNeed")));
	tips += FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Skill.SkillLvUpNeedLevel")), skill->LvUpRequireRoleLevel  );
	tips += HTMLEndFont();
	tips += HTMLEndParagraph();
	return tips;
}

//----------------------------------消息中的技能着色------------------------------------
FString UITextParser::HTMLFormatMessageSkillColor( UMMOAbility* skill )
{
	if(!skill)
	{
		return TEXT("");
	}

	FString res;
	res = FString::Printf( TEXT("<FONT COLOR='%s'>&lt;%s&gt;</FONT>") , *MMOChatManager::getSingleton().ChatSkillColorString , *skill->eventGetAbilityName() );
	return res;
}

//----------------------------------消息中的称号着色------------------------------------
FString UITextParser::HTMLFormatMessageTitleColor( struct TitleData* title )
{
	if( !title )
	{
		return TEXT("");
	}

	FString res;
	res = FString::Printf( TEXT("<FONT COLOR='%s'>&lt;%s&gt;</FONT>") , *UITextParser::HTMLGetItemColor( (EItemColor)title->TitleLevel ) , *title->TitleName );
	
	return res;
}

FString UITextParser::HTMLFormatItemColorString( UItem* item )
{
	if( !item )
	{
		return TEXT("");
	}
	FString str;
	str = UITextParser::HTMLBeginFont( UITextParser::HTMLGetItemColor( (EItemColor)item->ItemColor ) )
		+ item->GetDisplayName()
		+ UITextParser::HTMLEndFont();
	return str;
}

//------------------------------------------------------------------------
FString UITextParser::HTMLFormatDummyLongText(const FString& originText)
{
    return originText;
    //if(originText.Len() < 40)
    //	return originText;

    FString dummyLongText = TEXT("");

    const INT tempCount = originText.Len();

    for(INT i = 0; i < tempCount; ++i)
        dummyLongText += DummyText;

    return dummyLongText;
}



FString UITextParser::HTMLFormatMyName( const FString& myName )
{
	FString clr;
	GConfig->GetString(TEXT("TextColor"), TEXT("SelfPlayerName"), clr, GUIColorIni);

	FString str = HTMLBeginFont(clr);
	str += myName;
	str += HTMLEndFont();
	return str;
}

FString UITextParser::HTMLFormatPlayerName( const FString& playerName )
{
    FString clr;
    GConfig->GetString(TEXT("TextColor"), TEXT("PlayerName"), clr, GUIColorIni);

    FString str = HTMLBeginFont(clr);
    str += playerName;
    str += HTMLEndFont();
    return str;
}

FString UITextParser::HTMLFormatGuildName( const FString& guildName )
{
	FString clr;
	GConfig->GetString(TEXT("TextColor"), TEXT("GuildName"), clr, GUIColorIni);

	FString str = HTMLBeginFont(clr);
	str += guildName;
	str += HTMLEndFont();
	return str;
}

FString UITextParser::HTMLBeginLink( const FString& linkParameters )
{
    FString str = FString::Printf(TEXT("<a href=\'%s\'>"));	
    return str;
}

FString UITextParser::HTMLEndLink()
{
    return TEXT("</a>");
}

FString UITextParser::HTMLFormatQuestName( const UQuest* quest, bool withColor, bool withPrefix,  bool withType )
{
    static UEnum* questEnum = FindObject<UEnum>(UServerEnum::StaticClass(), TEXT("EQUEST_TYPE"), TRUE);
    FString enumVal = questEnum->GetEnum(quest->QuestType).ToString();

    FString questCaption;

    if(withPrefix)
    {
		if (withType)
		{
			questCaption += MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.QuestTitleDisplay_QuestType.")) + enumVal);
		}
    }

    questCaption += quest->QuestName;

    if(withColor)
    {
        FString QuestLvLess5Color, QuestLvNormalColor, QuestLvGreat5Color, clr;
        GConfig->GetString(TEXT("TextColor"), TEXT("QuestLvLess5Color"), QuestLvLess5Color, GUIColorIni);
        GConfig->GetString(TEXT("TextColor"), TEXT("QuestLvNormalColor"), QuestLvNormalColor, GUIColorIni);
        GConfig->GetString(TEXT("TextColor"), TEXT("QuestLvGreat5Color"), QuestLvGreat5Color, GUIColorIni);

        INT playerLevel = MMOFacade::getSingleton().GetMainPlayer()->ActorAttributes->LevelAttributes.Level;
        INT lowLevelDiffCfg = 0;
        GConfig->GetInt(TEXT("Quest"), TEXT("LowLevelDiff"), lowLevelDiffCfg, GGameIni);

        INT lvDiff = quest->QuestLevel - playerLevel;

        if(lvDiff <= -lowLevelDiffCfg)
        {
            clr = QuestLvLess5Color;
        }
        else if(lvDiff >= lowLevelDiffCfg)
        {
            clr = QuestLvGreat5Color;
        }
        else
        {
            clr = QuestLvNormalColor;
        }

        questCaption = HTMLBeginFont(clr) + questCaption + HTMLEndFont();
    }

	// 如果是环任务
	if (quest->CircleID > 0)
	{
		CircleQuestInfo info;
		if (MMOQuestData::getSingleton().GetCircleQuestInfo(quest->CircleID, info))
		{
			// 环序号从1开始
			questCaption += FString(TEXT("(第")) + appItoa(info.CircleIndex + 1) + FString(TEXT("环)"));
		}
	}

    return questCaption;
}

FString UITextParser::HTMLFormatDialogOptionName(const FString& optionCommand, const FString& optionName)
{
	FString dlgOption = TEXT("");
	if (!optionName.IsEmpty())
	{
		dlgOption += FString(TEXT("<#UI:")) + optionCommand + FString(TEXT(">")) + optionName + FString(TEXT("<#UI/>"));
	}
	return dlgOption;
}
INT UITextParser::SplitStringTagWithUTF8MaxLength(const FString& strSrc,FString& strSplit,INT nLength)
{
	if(strSrc.IsEmpty() || nLength <= 1)
		return -1;

	INT nPosMax = UTF8Length(strSrc,nLength-1);
	if((nPosMax +1)== strSrc.GetCharArray().Num())
	{
		strSplit = strSrc;
		return nPosMax;
	}

	//超过长度的做截断位置矫正
	INT nModifyPos = UTF8LengthMaxPosModify(strSrc,nPosMax);
	if(nModifyPos >= 0 && nModifyPos < strSrc.GetCharArray().Num())
		strSplit = strSrc.Left(nModifyPos + 1);
	return nModifyPos;
}

/*
nMaxLen != 0时  返回的是达到限制长度时的字符位置
nMaxLen == 0时  返回的是字符串转为UTF8后的长度
*/
INT UITextParser::UTF8Length(const FString& strSrc,INT nMaxLen)
{
	const TArray<TCHAR, FStringAllocator>& tCharArr = strSrc.GetCharArray();
	INT LengthW = tCharArr.Num();
	INT nPos = 0;
	INT nLength = 0;
	while (nPos < LengthW)
	{
		DWORD cp = (DWORD) (tCharArr(nPos++));
		if (cp == 0) break;

		if (cp > 0x10FFFF)   // No Unicode codepoints above 10FFFFh, (for now!)
			cp = TEXT('?');
		else if ((cp == 0xFFFE) || (cp == 0xFFFF))  // illegal values.
			cp = TEXT('?');
		else
		{
			// There are seven "UTF-16 surrogates" that are illegal in UTF-8.
			switch (cp)
			{
			case 0xD800:
			case 0xDB7F:
			case 0xDB80:
			case 0xDBFF:
			case 0xDC00:
			case 0xDF80:
			case 0xDFFF:
				cp = TEXT('?');
			}
		}
		// computer cp length
		INT nCharLen(4);
		if (cp < 0x80)
			nCharLen = 1;
		else if (cp < 0x800)
			nCharLen = 2;
		else if (cp < 0x10000)
			nCharLen = 3;
		nLength += nCharLen;

		if(nMaxLen > 0&& nLength > nMaxLen)
		{
			nPos--;
			nLength -= nCharLen;
			break;
		}
	}	
	if(nMaxLen > 0)
		return nPos - 1;
	else
		return nLength;
}
INT UITextParser::UTF8LengthMaxPosModify(const FString& strSrc,INT nMaxPos)
{
	INT nAdjustPos = 0;
	INT nPos = INDEX_NONE,nLastTagPos = INDEX_NONE;
	FString sEndTagL  = TEXT("") , sEndTagR  = TEXT(""), sHeadTag = TEXT("");
	while(true)
	{
		INT nUseTagPos = strSrc.InStr(ITextParser::StartTag,FALSE,FALSE,nLastTagPos);
		INT nHtmlTagPos = strSrc.InStr(TEXT("<"),FALSE,FALSE,nLastTagPos);

		if(nHtmlTagPos != INDEX_NONE || nUseTagPos != INDEX_NONE)
		{  
			nPos = nLastTagPos;
			if(nHtmlTagPos >= nUseTagPos && nUseTagPos != INDEX_NONE)
			{
				nLastTagPos  = nUseTagPos;
				sHeadTag	 = ITextParser::StartTag;  
				sEndTagL     = ITextParser::StartTag;	
				sEndTagR     = ITextParser::EndTag;
			}
			else if(nHtmlTagPos != INDEX_NONE )
			{
				nLastTagPos = nHtmlTagPos;
				sHeadTag    = TEXT("<");				
				sEndTagL	= TEXT("</");				
				sEndTagR	= TEXT(">");
			}
			nAdjustPos = nLastTagPos - 1;

			if(nPos <= nMaxPos && nLastTagPos >= nMaxPos)
			{	
				if(nLastTagPos == nMaxPos)
					nAdjustPos = nMaxPos - 1; //在非tag内的截断
				else
					nAdjustPos = nMaxPos;
				break;
			}
		}
		else
		{
			nAdjustPos = nMaxPos; //没有tag直接最大截断
			break;
		}
		FString tNode = TEXT("");
		const TArray<TCHAR, FStringAllocator>& tCharArr = strSrc.GetCharArray();
		for (INT n = nLastTagPos + sHeadTag.Len(); n < tCharArr.Num(); ++n)
		{
			if(tCharArr(n) == TEXT(':') || tCharArr(n) == TEXT('>')||tCharArr(n) == TEXT(' '))
			{
				tNode = strSrc.Mid(nLastTagPos + sHeadTag.Len(),n-nLastTagPos - sHeadTag.Len());
				break;
			}
		}
		FString findStr = sEndTagL + tNode + sEndTagR;
		nPos = strSrc.InStr(findStr,FALSE,FALSE,nLastTagPos);
		if(nPos != INDEX_NONE)
			nPos += (findStr.Len());
		else
		{
			nPos = strSrc.InStr(TEXT(">"),FALSE,FALSE,nLastTagPos);
			if(nPos != INDEX_NONE)
				nPos += 1;
			else
			{	
				break;//tag不匹配 截断
			}
		}		

		if(nMaxPos >nLastTagPos && nMaxPos < nPos)
		{	
			if( nMaxPos == nPos - 1)
				nAdjustPos = nPos -1;
			//tag内结束
			break;
		}
		else
			nAdjustPos = nPos -1;//没结束 继续
		nLastTagPos = nPos;
	}
	return nAdjustPos;
}
//ITextITextParserParser Begin---------------------------------------------------------------------------------------
INT ITextParser::ParseID( const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
    FString IDStr, Content;
    while (srcIdx < totalLen)
    {
        if(appIsDigit(*srcTxt))
        {
            IDStr.AppendChar(*srcTxt);

            ++srcTxt;
            ++srcIdx;
        }
        else if(*srcTxt == TEXT('>'))
        {
            INT id = appAtoi(*IDStr);
            ++srcTxt;
            ++srcIdx;
            return id;
        }
        else
        {
            ++srcTxt;
            ++srcIdx;
        }
    }

    return -1;
}

void ITextParser::AddParseParameter(ParseContext& ctx, const FString& params)
{
	for(INT i = 0; i < PARSE_MAX_PARAMETERS; ++i)
	{
		if(!ctx.Parameters[i].IsInUse)
		{
			ctx.Parameters[i].IsInUse = true;
			ctx.Parameters[i].ParameterString = params;
			return;
		}
	}
}

// add by zhangrui05@snda.com
void ITextParser::ParseNPC( const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen, INT& id, INT& mapId)
{
    FString IDStr, Content;
	UBOOL HasMapId = FALSE;
    while (srcIdx < totalLen)
    {
        if(appIsDigit(*srcTxt))
        {
            IDStr.AppendChar(*srcTxt);

            ++srcTxt;
            ++srcIdx;
        }
        else if(*srcTxt == TEXT(','))
        {
            id = appAtoi(*IDStr);
            ++srcTxt;
            ++srcIdx;
			IDStr = TEXT("");
			HasMapId = TRUE;
		}
		else if(*srcTxt == TEXT('>'))
		{
			if (HasMapId)
			{
				mapId = appAtoi(*IDStr);
			} 
			else
			{
				id = appAtoi(*IDStr);
			}
			++srcTxt;
			++srcIdx;
			return;
		}
        else
        {
            ++srcTxt;
            ++srcIdx;
        }
    }
}

// add by zhangrui05@snda.com
void ITextParser::ParseNavPoint( const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen, FString& pointName, INT& mapId)
{
	FString IDStr, Content;
	UBOOL HasMapId = FALSE;
	while (srcIdx < totalLen)
	{
		if(appIsDigit(*srcTxt))
		{
			IDStr.AppendChar(*srcTxt);

			++srcTxt;
			++srcIdx;
		}
		else if(*srcTxt == TEXT(','))
		{
			pointName = IDStr;
			++srcTxt;
			++srcIdx;
			IDStr = TEXT("");
			HasMapId = TRUE;
		}
		else if(*srcTxt == TEXT('>'))
		{
			if (HasMapId)
			{
				mapId = appAtoi(*IDStr);
			} 
			else
			{
				pointName = IDStr;
			}
			++srcTxt;
			++srcIdx;
			return;
		}
		else if(*srcTxt == TEXT(':'))
		{
			IDStr = TEXT("");
			++srcTxt;
			++srcIdx;
		}
		else
		{
			IDStr.AppendChar(*srcTxt);

			++srcTxt;
			++srcIdx;
		}
	}
}

// add by zhangrui05@snda.com
void ITextParser::ParseDoodad(const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen, FString& pointName, INT& doodadId, INT& mapId)
{
	FString IDStr, Content;
	UBOOL HasDoodadId = FALSE;
	UBOOL HasMapId = FALSE;
	while (srcIdx < totalLen)
	{
		if(appIsDigit(*srcTxt))
		{
			IDStr.AppendChar(*srcTxt);

			++srcTxt;
			++srcIdx;
		}
		else if(*srcTxt == TEXT(','))
		{
			if (!HasDoodadId)
			{
				pointName = IDStr;
				HasDoodadId = TRUE;
			} 
			else
			{
				doodadId = appAtoi(*IDStr);
				HasMapId = TRUE;
			}

			++srcTxt;
			++srcIdx;
			IDStr = TEXT("");
		}
		else if(*srcTxt == TEXT('>'))
		{
			if (HasMapId)
			{
				mapId = appAtoi(*IDStr);
			} 
			else
			{
				doodadId = appAtoi(*IDStr);
			}
			++srcTxt;
			++srcIdx;
			return;
		}
		else if(*srcTxt == TEXT(':'))
		{
			IDStr = TEXT("");
			++srcTxt;
			++srcIdx;
		}
		else
		{
			IDStr.AppendChar(*srcTxt);

			++srcTxt;
			++srcIdx;
		}
	}
}

//------------------------------------------------------------------------
FString ITextParser::ParseStringID( const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen, FString* pOutStr )
{
    FString IDStr;
    FString CurStr;

    // skip :
    ++srcTxt;
    ++srcIdx;

    while (srcIdx < totalLen)
    {
        if(*srcTxt == TEXT('>'))
        {			
            if(IDStr.Len()>0)
            {
                if(pOutStr)*pOutStr = CurStr;
            }
            else{
                IDStr = CurStr;
            }
            ++srcTxt;
            ++srcIdx;
            break;
        }
        else if(*srcTxt == TEXT(':'))
        {
            IDStr = CurStr;
            CurStr = TEXT("");
            ++srcTxt;
            ++srcIdx;
        }
        else
        {
            CurStr.AppendChar(*srcTxt);

            ++srcTxt;
            ++srcIdx;
        }
    }

    return IDStr;
}

FString ITextParser::ParseStringAsNewWord(const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen,FString* pOutStr)
{
	FString IDStr;
	FString CurStr;

	// skip :
	++srcTxt;
	++srcIdx;

	while (srcIdx < totalLen)
	{
		if(*srcTxt == TEXT('>'))
		{			
			if(IDStr.Len()>0)
			{
				if(pOutStr)*pOutStr = CurStr;
			}
			else{
				IDStr = CurStr;
			}
			++srcTxt;
			++srcIdx;
			break;
		}
		//else if(*srcTxt == TEXT(':'))
		//{
		//	IDStr = CurStr;
		//	CurStr = TEXT("");
		//	++srcTxt;
		//	++srcIdx;
		//}
		else
		{
			CurStr.AppendChar(*srcTxt);

			++srcTxt;
			++srcIdx;
		}
	}

	return IDStr;
}

//------------------------------------------------------------------------
FString ITextParser::ParseContent( const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
#if 1
	FString content = srcTxt;
	FString keywd = TEXT("<#");
	keywd = keywd + GetKeyword() + TEXT("/>") ;
	INT keyLen = keywd.Len();
	INT lenth = content.InStr(keywd);
	content = content.Left(lenth);
	srcTxt += lenth + keyLen;
	srcIdx += lenth + keyLen;
	return content;
#else
	FString Content;
    while (srcIdx < totalLen)
    {
        if(*srcTxt != TEXT('<'))
        {
            Content.AppendChar(*srcTxt);
            ++srcTxt;
            ++srcIdx;
        }
        else
        {
            break;
        }
    }

    return Content;
#endif
}

//------------------------------------------------------------------------
void ITextParser::ParseCloseTag( const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen, const FString& keyWord )
{
#if 1
	FString content = srcTxt;

	if(content.Len() > 0)
	{
		if(content[0] == TEXT('>')) // 开始的TAG没有结束
		{
			++srcTxt;
			++srcIdx;
			return;
		}
	}

	FString keywd = TEXT("<#");
	keywd = keywd + keyWord + TEXT("/>");
	INT keyLen = keywd.Len();
	INT lenth = content.InStr(keywd);

	if(lenth == INDEX_NONE) // 没有找到说明是单个标签
	{
		lenth = content.InStr(TEXT(">"));
		keyLen = 1;
	}
	content = content.Left(lenth);
	srcTxt += lenth + keyLen;
	srcIdx += lenth + keyLen;
#else

    while (srcIdx < totalLen)
    {
        if(*srcTxt == TEXT('>'))
        {
            ++srcTxt;
            ++srcIdx;
            return;
        }

        ++srcTxt;
        ++srcIdx;
    }
#endif
}
//ITextParser End--------------------------------------------------------------------------------------


//------------------------------------------------------------------------
void UITextParser_DefaultDialog::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
    ParseCloseTag(ctx, srcTxt, srcIdx, totalLen, GetKeyword());
    if(ctx.NPC)
    {
		FGuardSkipAppendNPCDialogOption gardCtx(&ctx, true);
        UITextParser::getSingleton().Parse(ctx,  diagCtx, ctx.NPC->DefaultDialogText);
    }
}

//------------------------------------------------------------------------
void UITextParser_NPC::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
    INT id = 0;
	INT mapId = 0;
	ParseNPC(ctx, srcTxt, srcIdx, totalLen, id, mapId);
    //ParseCloseTag(ctx, srcTxt, srcIdx, totalLen);

    const UNPCTemplate* npcTmp = 0;

    //if(Facade::getSingletonPtr())
    {
        npcTmp = MMOFacade::getSingleton().FindNPCTemplate(id);
    }

    if(npcTmp)
    {
        //INT Num = diagCtx.DialogText
        FString clr;
        GConfig->GetString(TEXT("TextColor"), TEXT("NavNPCName"), clr, GUIColorIni);

        FString str = UITextParser::HTMLBeginFont(clr);

        FString linkStr = LinkManager::HtmlLink_NPCNav(npcTmp->GetDisplayName(), npcTmp->GetTemplateID(), mapId, FALSE);
        //str += UITextParser::HTMLBeginLink(linkPar);
        str += linkStr;
        //str += UITextParser::HTMLEndLink();

        str += UITextParser::HTMLEndFont();

        diagCtx.TempText += str;
    }
}

FString UITextParser_NPC::ParseContent( const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	FString Content;
	
	INT id = 0;
	INT mapId = -1;
	ParseNPC(ctx, srcTxt, srcIdx, totalLen, id, mapId);
	
	const UNPCTemplate* npcTmp = MMOFacade::getSingleton().FindNPCTemplate(id);
	if (npcTmp)
	{
		Content = npcTmp->GetDisplayName();
	}

	return Content;
}

void UITextParser_NpcMove::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	INT id = 0;
	INT mapId = 0;
	ParseNPC(ctx, srcTxt, srcIdx, totalLen, id, mapId);
	//ParseCloseTag(ctx, srcTxt, srcIdx, totalLen);

	const UNPCTemplate* npcTmp = 0;

	//if(Facade::getSingletonPtr())
	{
		npcTmp = MMOFacade::getSingleton().FindNPCTemplate(id);
	}

	if(npcTmp)
	{
		//INT Num = diagCtx.DialogText
		FString clr;
		GConfig->GetString(TEXT("TextColor"), TEXT("NavNPCName"), clr, GUIColorIni);

		FString str = UITextParser::HTMLBeginFont(clr);

		FString linkStr = LinkManager::HtmlLink_NPCNav(npcTmp->GetDisplayName(), npcTmp->GetTemplateID(), mapId, TRUE);
		//str += UITextParser::HTMLBeginLink(linkPar);
		str += linkStr;
		//str += UITextParser::HTMLEndLink();

		str += UITextParser::HTMLEndFont();

		diagCtx.TempText += str;
	}
}

FString UITextParser_NpcMove::ParseContent( const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	FString Content;

	INT id = 0;
	INT mapId = -1;
	ParseNPC(ctx, srcTxt, srcIdx, totalLen, id, mapId);

	const UNPCTemplate* npcTmp = MMOFacade::getSingleton().FindNPCTemplate(id);
	if (npcTmp)
	{
		Content = npcTmp->GetDisplayName();
	}

	return Content;
}

void UITextParser_NpcArrow::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	INT id = 0;
	INT mapId = 0;
	ParseNPC(ctx, srcTxt, srcIdx, totalLen, id, mapId);
	//ParseCloseTag(ctx, srcTxt, srcIdx, totalLen);

	const UNPCTemplate* npcTmp = 0;

	//if(Facade::getSingletonPtr())
	{
		npcTmp = MMOFacade::getSingleton().FindNPCTemplate(id);
	}

	if(npcTmp)
	{
		//INT Num = diagCtx.DialogText
		FString clr;
		GConfig->GetString(TEXT("TextColor"), TEXT("NavNPCName"), clr, GUIColorIni);

		FString str = UITextParser::HTMLBeginFont(clr);

		FString linkStr = LinkManager::HtmlLink_NPCNav(npcTmp->GetDisplayName(), npcTmp->GetTemplateID(), mapId, FALSE);
		//str += UITextParser::HTMLBeginLink(linkPar);
		str += linkStr;
		//str += UITextParser::HTMLEndLink();

		str += UITextParser::HTMLEndFont();

		diagCtx.TempText += str;
	}
}

FString UITextParser_NpcArrow::ParseContent( const ParseContext& ctx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	FString Content;

	INT id = 0;
	INT mapId = -1;
	ParseNPC(ctx, srcTxt, srcIdx, totalLen, id, mapId);

	const UNPCTemplate* npcTmp = MMOFacade::getSingleton().FindNPCTemplate(id);
	if (npcTmp)
	{
		Content = npcTmp->GetDisplayName();
	}

	return Content;
}
//------------------------------------------------------------------------
void UITextParser_Item::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
    INT id = ParseID(ctx, srcTxt, srcIdx, totalLen);
    //ParseCloseTag(ctx, srcTxt, srcIdx, totalLen);
    UItem* item = 0;
    //if(Facade::getSingletonPtr())
    {
        item = MMOFacade::getSingleton().FindItemTemplate(id);
    }

    if(item)
    {
        diagCtx.TempText += LinkManager::HtmlLink_Item(item);
    }
}

//------------------------------------------------------------------------
void UITextParser_ItemLink::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	INT id = ParseID(ctx, srcTxt, srcIdx, totalLen);
	//ParseCloseTag(ctx, srcTxt, srcIdx, totalLen);
	UItem* item = 0;
	//if(Facade::getSingletonPtr())
	{
		item = MMOFacade::getSingleton().FindItemTemplate(id);
	}

	if(item)
	{
		diagCtx.TempText += LinkManager::HtmlLink_Item(item);
	}
}

//------------------------------------------------------------------------
void UITextParser_Quest::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
    INT id = ParseID(ctx, srcTxt, srcIdx, totalLen);
    //ParseCloseTag(ctx, srcTxt, srcIdx, totalLen);

    diagCtx.QuestId = id;
    const UQuest* questTemplate = MMOQuestData::getSingleton().GetQuestData(id);
    if(questTemplate)
    {
        diagCtx.TempText += questTemplate->QuestName;
    }
    else
    {

    }

    //return DC/*FString::Printf(TEXT("[Invalid Item:%d]"), id)*/;
}

//------------------------------------------------------------------------
//void UITextParser_Player::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
//{
//	//INT id = ParseID(ctx, srcTxt, srcIdx, totalLen);
//	ParseCloseTag(ctx, srcTxt, srcIdx, totalLen);
//	
//	if(Facade::getSingletonPtr() &&MMOFacade::getSingleton().GetMainPlayer() )
//	{
//		diagCtx.DialogText(0) = MMOFacade::getSingleton().GetMainPlayer()->RoleName;
//	}
//}

//------------------------------------------------------------------------
void UITextParser_DialogOption::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
    INT id = ParseID(ctx, srcTxt, srcIdx, totalLen);
    //ParseCloseTag(ctx, srcTxt, srcIdx, totalLen); //

    //我改了改
// 	FString Content = srcTxt;
// 	FString Keyword = TEXT("<#");
// 	Keyword = Keyword +GetKeyword() ;
// 	INT lenth = Content.InStr(Keyword);
// 	Content = Content.Left(lenth);
// 	//ParseCloseTag(ctx, srcTxt, srcIdx, totalLen);

	FString Content = ParseContent(ctx, srcTxt, srcIdx, totalLen);
    
	FGuardSkipAppendNPCDialogOption gardCtx(&ctx, true);
	UITextParser::getSingleton().Parse(ctx, diagCtx, Content);

    diagCtx.TextId = id;
}

//------------------------------------------------------------------------
void UITextParser_DialogText::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
    ParseCloseTag(ctx, srcTxt, srcIdx, totalLen, GetKeyword()); //去掉>

    //FString Content = srcTxt;
    //FString Keyword = TEXT("</");
    //Keyword = Keyword +GetKeyword() ;
    //INT lenth = Content.InStr(Keyword);
    //Content = Content.Left(lenth);

	FString Content = ParseContent(ctx, srcTxt,	 srcIdx, totalLen);
    diagCtx.DialogText.AddItem(Content);
	diagCtx.TempText += Content;

    // 	INT DTNum = diagCtx.DialogText.Num();
    // 	if( DTNum == 0 )
    // 	{
    // 		diagCtx.DialogText.AddItem();
    // 		
    // 	}

    //diagCtx.DialogText(diagCtx.DialogText.Num() - 1) =
    //UITextParser::getSingleton().Parse(ctx,  diagCtx, Content);
}

//------------------------------------------------------------------------
void UITextParser_OpenUI::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
    FString UIParameters = TEXT("");
    FString UICommand = ParseStringID(ctx, srcTxt, srcIdx, totalLen, &UIParameters);
    //ParseCloseTag(ctx, srcTxt, srcIdx, totalLen);

    FString content = ParseContent(ctx, srcTxt, srcIdx, totalLen);
	//ParseCloseTag(ctx, srcTxt, srcIdx, totalLen, GetKeyword());

    IDialogOptionAction* action = 0;
    DialogOptionActionMap::iterator i = g_dialogOptionActMap.find(*UICommand);
    if(i == g_dialogOptionActMap.end())
    {
        action = new DialogOptionAction_UICommand_OpenPanel(FName(*UICommand));
    }
    else
    {
        action = (*i->second)();
    }

    diagCtx.AdvanceClientActionID();

    action->Parse(content, &ctx, &UIParameters);

    diagCtx.OptionActionMap.insert(std::make_pair(diagCtx.ClientActionAutoID, action));
}

// add by zhangrui@snda
void UITextParser_Instruction::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	FString UIParameters = TEXT("");
	FString UICommand = ParseStringID(ctx, srcTxt, srcIdx, totalLen, &UIParameters);
	//ParseCloseTag(ctx, srcTxt, srcIdx, totalLen);

	FString content = ParseContent(ctx, srcTxt, srcIdx, totalLen);
	//ParseCloseTag(ctx, srcTxt, srcIdx, totalLen, GetKeyword());

	DialogOptionAction_UICommand_Instruction* action = new DialogOptionAction_UICommand_Instruction();
	action->SetInstructionKey(UICommand);

	diagCtx.AdvanceClientActionID();

	action->Parse(content, &ctx, &UIParameters);

	diagCtx.OptionActionMap.insert(std::make_pair(diagCtx.ClientActionAutoID, action));
}

void UITextParser_SelfName::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
    ParseCloseTag(ctx, srcTxt, srcIdx, totalLen, GetKeyword());
    if(MMOFacade::getSingleton().GetMainPlayer())
    {
        diagCtx.TempText += 
            UITextParser::HTMLFormatMyName(
            MMOFacade::getSingleton().GetMainPlayer()->ActorName);
    }
}



void UITextParser_PlayerName::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	FString id = ParseStringID(ctx, srcTxt, srcIdx, totalLen);
	ParseCloseTag(ctx, srcTxt, srcIdx, totalLen, GetKeyword());

	diagCtx.TempText += UITextParser::HTMLFormatPlayerName(	id );
	
}

void UITextParser_GuildName::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	FString id = ParseStringID(ctx, srcTxt, srcIdx, totalLen);
	//ParseCloseTag(ctx, srcTxt, srcIdx, totalLen, GetKeyword());

	diagCtx.TempText += UITextParser::HTMLFormatGuildName( id );
	
}

void UITextParser_Font::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
    FString colorStr = ParseStringID(ctx, srcTxt, srcIdx, totalLen);
    //ParseCloseTag(ctx, srcTxt, srcIdx, totalLen);

    FString content = ParseContent(ctx, srcTxt, srcIdx, totalLen);
    //ParseCloseTag(ctx, srcTxt, srcIdx, totalLen, GetKeyword());

    FString str = UITextParser::HTMLBeginFont(colorStr);
    str += content;
    str += UITextParser::HTMLEndFont();

    diagCtx.TempText += str;
}

void UITextParser_NavPointName::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	FString serverPointName = TEXT("");
	INT mapId = 0;
	ParseNavPoint(ctx, srcTxt, srcIdx, totalLen, serverPointName, mapId);

    FString content = ParseContent(ctx, srcTxt, srcIdx, totalLen);

    // 	FVector toPos = FVector::ZeroVector;
    // 
    // 	if(!serverPt)
    // 	{
    // // 		content += UITextParser::HTMLBeginFont(TEXT("#FF0000"));
    // // 		content += FString(TEXT("Nav point not found:")) + serverPointName;
    // // 		content += UITextParser::HTMLEndFont();
    // 	}
    // 	else
    // 	{
    // 		toPos = serverPt->Location;
    // 		//linkPar = TEXT("Nav:");
    // 		//linkPar += serverPt->Location.ToString();
    // 	}

    //ParseCloseTag(ctx, srcTxt, srcIdx, totalLen, GetKeyword());

    FString clr;
    GConfig->GetString(TEXT("TextColor"), TEXT("NavCoord"), clr, GUIColorIni);

    FString str = UITextParser::HTMLBeginFont(clr);
    FString linkStr = LinkManager::HtmlLink_NavPointName(content, serverPointName, mapId, FALSE);
    //str += UITextParser::HTMLBeginLink(linkPar);
    str += linkStr;
    //str += UITextParser::HTMLEndLink();

    str += UITextParser::HTMLEndFont();

    diagCtx.TempText += str;
}

void UITextParser_NavPointNameMove::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	FString serverPointName = TEXT("");
	INT mapId = 0;
	ParseNavPoint(ctx, srcTxt, srcIdx, totalLen, serverPointName, mapId);

	FString content = ParseContent(ctx, srcTxt, srcIdx, totalLen);

	// 	FVector toPos = FVector::ZeroVector;
	// 
	// 	if(!serverPt)
	// 	{
	// // 		content += UITextParser::HTMLBeginFont(TEXT("#FF0000"));
	// // 		content += FString(TEXT("Nav point not found:")) + serverPointName;
	// // 		content += UITextParser::HTMLEndFont();
	// 	}
	// 	else
	// 	{
	// 		toPos = serverPt->Location;
	// 		//linkPar = TEXT("Nav:");
	// 		//linkPar += serverPt->Location.ToString();
	// 	}

	//ParseCloseTag(ctx, srcTxt, srcIdx, totalLen, GetKeyword());

	FString clr;
	GConfig->GetString(TEXT("TextColor"), TEXT("NavCoord"), clr, GUIColorIni);

	FString str = UITextParser::HTMLBeginFont(clr);
	FString linkStr = LinkManager::HtmlLink_NavPointName(content, serverPointName, mapId, TRUE);
	//str += UITextParser::HTMLBeginLink(linkPar);
	str += linkStr;
	//str += UITextParser::HTMLEndLink();

	str += UITextParser::HTMLEndFont();

	diagCtx.TempText += str;
}

void UITextParser_DoodadName::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	FString navPointName = TEXT("");
	INT doodadId = 0;
	INT mapId = 0;
	ParseDoodad(ctx, srcTxt, srcIdx, totalLen, navPointName, doodadId, mapId);

	FString content = ParseContent(ctx, srcTxt, srcIdx, totalLen);

	FString clr;
	GConfig->GetString(TEXT("TextColor"), TEXT("NavCoord"), clr, GUIColorIni);
	FString str = UITextParser::HTMLBeginFont(clr);

	FString linkStr = LinkManager::HtmlLink_DoodadNav(content, doodadId, navPointName, mapId, FALSE);
	str += linkStr;
	str += UITextParser::HTMLEndFont();

	diagCtx.TempText += str;
}

void UITextParser_DoodadNameMove::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	FString navPointName = TEXT("");
	INT doodadId = 0;
	INT mapId = 0;
	ParseDoodad(ctx, srcTxt, srcIdx, totalLen, navPointName, doodadId, mapId);

	FString content = ParseContent(ctx, srcTxt, srcIdx, totalLen);

	FString clr;
	GConfig->GetString(TEXT("TextColor"), TEXT("NavCoord"), clr, GUIColorIni);
	FString str = UITextParser::HTMLBeginFont(clr);

	FString linkStr = LinkManager::HtmlLink_DoodadNav(content, doodadId, navPointName, mapId, TRUE);
	str += linkStr;
	str += UITextParser::HTMLEndFont();

	diagCtx.TempText += str;
}

void UITextParser_FromStringMap::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	FString id = ParseStringID(ctx, srcTxt, srcIdx, totalLen);
	
	UBOOL r = FALSE;
	FString str = MMOUI::ParseLocalizedPropertyPath(id, r);
	if(r)
	{
		FGuardSkipAppendNPCDialogOption gardCtx(&ctx, true);
		UITextParser::getSingleton().Parse(ctx, diagCtx, str);
	}
	else
	{
		warnf(NAME_Error, TEXT("UITextParser_FromStringMap failed to find:%s"), *id);
	}
}

void UITextParser_DialogOptionServer::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	INT id = ParseID(ctx, srcTxt, srcIdx, totalLen);
	//ParseCloseTag(ctx, srcTxt, srcIdx, totalLen); //

	//我改了改
// 	FString Content = srcTxt;
// 	FString Keyword = TEXT("<#");
// 	Keyword = Keyword +GetKeyword() ;
// 	INT lenth = Content.InStr(Keyword);
// 	Content = Content.Left(lenth);
// 	//ParseCloseTag(ctx, srcTxt, srcIdx, totalLen);
	
	FString Content = ParseContent(ctx, srcTxt, srcIdx, totalLen);

	DialogOptionAction_SendServerOption* action = new DialogOptionAction_SendServerOption(id);

	action->Parse(Content, &ctx);
	//UITextParser::getSingleton().Parse(ctx, diagCtx, Content);
	//diagCtx.TextId = id;

	diagCtx.AdvanceClientActionID();
	diagCtx.OptionActionMap.insert(std::make_pair(diagCtx.ClientActionAutoID, action));
}

void UITextParser_Paramater0::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	FString value;
	FString id = ParseStringID( ctx, srcTxt, srcIdx, totalLen , &value );
	
	// 递归的格式：<#P0:<#I:%d>><#T:MMOGame.Mail.Buy.Success>

	if(id.Len() > 0)
	{
		if( id.Left(1) == TEXT("<") )
		{
			ParseCloseTag(ctx, srcTxt, srcIdx, totalLen, GetKeyword()); //去掉>

			id.AppendChar(TEXT(':'));
			id += value;
			id.AppendChar(TEXT('>'));
			ParseContext ctx2;
			DialogContext dtx2;	
			ctx2.SkipAppendNPCDialogOption = true;
			UITextParser::getSingleton().Parse(ctx2, dtx2, id);
			id = dtx2.TempText;
		}


		AddParseParameter(ctx, id);
	}
}

void UITextParser_Paramater1::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	FString value;
	FString id = ParseStringID(ctx, srcTxt, srcIdx, totalLen, &value );
	if(id.Len() > 0)
	{
		if( id.Left(1) == TEXT("<") )
		{
			ParseCloseTag(ctx, srcTxt, srcIdx, totalLen, GetKeyword()); //去掉>

			id.AppendChar(TEXT(':'));
			id += value;
			id.AppendChar(TEXT('>'));
			ParseContext ctx2;
			DialogContext dtx2;	
			ctx2.SkipAppendNPCDialogOption = true;
			UITextParser::getSingleton().Parse(ctx2, dtx2, id);
			id = dtx2.TempText;
		}
		AddParseParameter(ctx, id);

	}
}

void UITextParser_Paramater2::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	FString value;
	FString id = ParseStringID(ctx, srcTxt, srcIdx, totalLen, &value);
	if(id.Len() > 0)
	{
		if( id.Left(1) == TEXT("<") )
		{
			ParseCloseTag(ctx, srcTxt, srcIdx, totalLen, GetKeyword()); //去掉>

			id.AppendChar(TEXT(':'));
			id += value;
			id.AppendChar(TEXT('>'));
			ParseContext ctx2;
			DialogContext dtx2;	
			ctx2.SkipAppendNPCDialogOption = true;
			UITextParser::getSingleton().Parse(ctx2, dtx2, id);
			id = dtx2.TempText;
		}
		AddParseParameter(ctx, id);

	}
}

void UITextParser_Paramater3::Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen)
{
	FString value;
	FString id = ParseStringID(ctx, srcTxt, srcIdx, totalLen, &value);
	if (id.Len() > 0)
	{
		if (id.Left(1) == TEXT("<"))
		{
			ParseCloseTag(ctx, srcTxt, srcIdx, totalLen, GetKeyword()); //去掉>

			id.AppendChar(TEXT(':'));
			id += value;
			id.AppendChar(TEXT('>'));
			ParseContext ctx2;
			DialogContext dtx2;
			ctx2.SkipAppendNPCDialogOption = true;
			UITextParser::getSingleton().Parse(ctx2, dtx2, id);
			id = dtx2.TempText;
		}
		AddParseParameter(ctx, id);

	}
}

void UITextParser_CommonParameter::Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	FString id = ParseStringID(ctx, srcTxt, srcIdx, totalLen);
	if(id.Len() > 0)
	{
		AddParseParameter(ctx, id);
	}
}



void UITextParser_EquipSlot::Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	INT id = ParseID(ctx, srcTxt, srcIdx, totalLen);
	if( id >= 0 && id < EQUIP_SLOT_COUNT)
	{
		AddParseParameter(ctx, EquipSlotManager::getSingleton().GetEquipSlotNameById(id));
	}
}

void UITextParser_EquipSlotLevel::Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	INT lv = ParseID(ctx, srcTxt, srcIdx, totalLen);
	if(lv >= 0)
	{
		FString params;
		if(lv == MAX_ENHANCE_LEVEL)
		{
			FString ColorStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipSlotEnhance.Quality_5"));
			FString NameStr  = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipSlotEnhance.MaxLevel"));
			params = FString::Printf(TEXT("%s%s"),*ColorStr,*NameStr);
		}
		else
		{
			INT ehcMajorLevel = 0;
			INT ehcMinorLevel = 0;
			INT ehcMinorMinorLv = 0;
			EquipSlotManager::getSingleton().CvtEnhanceLevel( lv, ehcMajorLevel, ehcMinorLevel, ehcMinorMinorLv );
			FString ColorStr = MMOUI::ParseLocalizedPropertyPath(FString::Printf(TEXT("MMOGame.EquipSlotEnhance.Quality_%d"),ehcMajorLevel));
			FString NameStr  = MMOUI::ParseLocalizedPropertyPath(FString::Printf(TEXT("MMOGame.EquipSlotEnhance.Level_%d"),ehcMinorLevel));
			if(ehcMinorMinorLv > 0)
			{
				params = FString::Printf(TEXT("%s%s%d"),*ColorStr,*NameStr,ehcMinorMinorLv);
			}
			else
			{
				params = FString::Printf(TEXT("%s%s"),*ColorStr,*NameStr);
			}
			
		}
		AddParseParameter(ctx, params);
	}
}

void UITextParser_WeaponRuneID::Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	INT weaponRuneID = ParseID(ctx, srcTxt, srcIdx, totalLen);

	const WeaponSkillCfg *pWeaponSkillCfgData = MMOEquipManager::getSingleton().GetWeaponSkillCfg( weaponRuneID );
	if( pWeaponSkillCfgData == NULL ){
		return;
	}

	INT uniqueSkillID = MMOSkillManager::CombinateAbilityID( pWeaponSkillCfgData->_SkillID, 0, pWeaponSkillCfgData->_SkillRuneID );
	UMMOAbility *pWeaponSkill = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
	if( pWeaponSkill == NULL ){
		return;
	}
	
	FString skillName;
	UMMOAbility *pMainSkill = MMOSkillManager::getSingleton().GetMainAbilityFor( pWeaponSkill );
	if( pMainSkill != NULL ){
		skillName += pMainSkill->eventGetAbilityName();
	}
	skillName += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Announce.WeaponSkillName") ), *( pWeaponSkill->eventGetAbilityName() ) );

	AddParseParameter(ctx, skillName);
}

void UITextParser_URL::Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
    FString url_addr;
    ++srcTxt, ++srcIdx;
    bool find_end = false;
    while (!find_end && srcIdx < totalLen)
    {
        if (*srcTxt != TEXT('>'))
            url_addr += *srcTxt;
        else
            find_end = true;
        ++srcTxt, ++srcIdx;
    }
    FString content = ParseContent(ctx, srcTxt, srcIdx, totalLen);

	FString linkStr = LinkManager::HtmlLink_WebPath(content,0,url_addr);
	diagCtx.TempText += linkStr;

   /* DialogOptionAction_OpenURL* action = new DialogOptionAction_OpenURL;
    diagCtx.AdvanceClientActionID();
    action->Parse(content, &ctx, &url_addr);
    diagCtx.OptionActionMap.insert(std::make_pair(diagCtx.ClientActionAutoID, action));*/
}

void UITextParser_QURL::Parse(ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
    INT qurl_type = ParseID(ctx, srcTxt, srcIdx, totalLen);
    FString content = ParseContent(ctx, srcTxt, srcIdx, totalLen);
    FString tnil;

	 FString linkStr = LinkManager::HtmlLink_WebPath(content,qurl_type,TEXT(""));
	 diagCtx.TempText += linkStr;
    /*DialogOptionAction_OpenQURL* action = new DialogOptionAction_OpenQURL(qurl_type);
    diagCtx.AdvanceClientActionID();
    action->Parse(content, &ctx, &tnil);
    diagCtx.OptionActionMap.insert(std::make_pair(diagCtx.ClientActionAutoID, action));*/
}

void UITextParser_QShowPage::Parse( ParseContext& ctx, DialogContext& diagCtx, TCHAR*& srcTxt, INT& srcIdx, INT totalLen )
{
	ParseCloseTag(ctx, srcTxt, srcIdx, totalLen, GetKeyword()); //去掉>
	FString Content = ParseContent(ctx, srcTxt,	 srcIdx, totalLen);
	diagCtx.DialogText.AddItem(Content);
	diagCtx.TempText += Content;
}