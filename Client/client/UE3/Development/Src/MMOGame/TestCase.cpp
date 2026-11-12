#include "stdafx.h"
#include "TestCase.h"

#ifdef _DEBUG

#include "TipsFormatter.h"
#include "UITextParser.h"
#include "MMOFacade.h"
#include "WebHtmlManager.h"

INT ProcTestCaseExcept( LPEXCEPTION_POINTERS ExceptionInfo, ITestCase* testCase)
{
	SET_WARN_COLOR_AND_BACKGROUND(COLOR_RED, COLOR_YELLOW);
	warnf(TEXT("[TestCase] %s failed, unknown exception!"), *testCase->GetName());
	CLEAR_WARN_COLOR();

	appDebugBreak();

	return EXCEPTION_EXECUTE_HANDLER;
}

void TestCaseLoader( ITestCase* testCase )
{
	//__try
	{
		try
		{

			SET_WARN_COLOR(COLOR_GREEN);
			warnf(TEXT("[TestCase] %s start -------"), *testCase->GetName());
			CLEAR_WARN_COLOR();

			if(!testCase->Run())
			{
				SET_WARN_COLOR_AND_BACKGROUND(COLOR_RED, COLOR_YELLOW);
				warnf(TEXT("[TestCase]%s failed!"), *testCase->GetName());
				CLEAR_WARN_COLOR();
			}
			else
			{
				SET_WARN_COLOR(COLOR_GREEN);
				warnf(TEXT("[TestCase]%s succeeded!"), *testCase->GetName());
				CLEAR_WARN_COLOR();
			}
		}	
		catch (const std::exception& ex)
		{
			SET_WARN_COLOR_AND_BACKGROUND(COLOR_RED, COLOR_YELLOW);
			warnf(TEXT("[TestCase]%s failed, exception:%s"), *testCase->GetName(), ANSI_TO_TCHAR(ex.what()));
			CLEAR_WARN_COLOR();
		}
		catch(...)
		{
			SET_WARN_COLOR_AND_BACKGROUND(COLOR_RED, COLOR_YELLOW);
			warnf(TEXT("[TestCase]%s failed, unknown exception!"), *testCase->GetName());
			CLEAR_WARN_COLOR();

			appDebugBreak();
		}
	}
	//__except(ProcTestCaseExcept(GetExceptionInformation(), testCase))
	{	
	}
}

const FString& AbstractTestCase::GetName() const
{
	return _name;
}

AbstractTestCase::AbstractTestCase()
{

}

AbstractTestCase::~AbstractTestCase()
{

}

void AbstractTestCase::SetName( const FString& name )
{
	_name = name;
}

//typedef TMap<FString, FString> StringMap;
extern StringMap g_StringMap;

class TestCase5814_TipsCrash : public AbstractTestCase
{
public:

	virtual bool Run()
	{
		// HACK an unsafe object first for testing
		BYTE buf[4096] = {0};
		UItem* item = (UItem*)buf;
		UGFxObject* testObj = (UGFxObject*)(buf);//MMOUI::GetUIManager()->GetCommonUIComponentMoviePlayer()->CreateArray();
		TipsFormatter_Item::FormatItemTip( MMOUI::GetUIManager()->GetCommonUIComponentMoviePlayer(), testObj, item, 0, 0, 0, false);
		return true;
	}
};


class TestCase_FormatTipPara0 : public AbstractTestCase
{
public:

	virtual bool Run()
	{
		// keep the old one, to avoid bugs in the unlikely case in DEBUG
		StringMapVal oldVal;
		StringMapVal* value = g_StringMap.Find(TEXT("8888"));
		if( value )
		{
			oldVal = *value;
		}

		StringMapVal tempVal;
		tempVal.stringVal = TEXT("You got `~");
		g_StringMap.Set(TEXT("8888"), tempVal);

		FString testSrc = TEXT("<#P0:age of dawn><#P1:some chaos><#T:8888>");
		ParseContext ctx;
		DialogContext dtx;
		FString outStr;
		UITextParser::getSingleton().Parse(ctx, dtx, testSrc, outStr);

		if(oldVal.stringVal.Len() > 0)
		{
			g_StringMap.Set(TEXT("8888"), oldVal);
		}
		
		if(dtx.TempText == TEXT("You got age of dawn"))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

class TestCase_FormatTipPara1 : public AbstractTestCase
{
public:

	virtual bool Run()
	{
		// keep the old one, to avoid bugs in the unlikely case in DEBUG
		StringMapVal oldVal;
		StringMapVal* value = g_StringMap.Find(TEXT("8888"));
		if( value )
		{
			oldVal = *value;
		}

		StringMapVal tempVal;
		tempVal.stringVal = TEXT("You got `~ P1:`~");
		g_StringMap.Set(TEXT("8888"), tempVal);

		FString testSrc = TEXT("<#P0:age of dawn><#P1:[Some chaos]><#T:8888>");
		ParseContext ctx;
		DialogContext dtx;
		FString outStr;
		UITextParser::getSingleton().Parse(ctx, dtx, testSrc, outStr);

		if(oldVal.stringVal.Len() > 0)
		{
			g_StringMap.Set(TEXT("8888"), oldVal);
		}

		FString expectedResult(TEXT("You got age of dawn P1:[Some chaos]"));

		if(dtx.TempText == expectedResult)
		{
			return true;
		}
		else
		{
			warnf(NAME_Error, TEXT("\tError Result:%s \n\tExpected:%s"), *dtx.TempText, *expectedResult);
			return false;
		}
	}
};

class TestCase_FormatTipParaRecu : public AbstractTestCase
{
public:

	virtual bool Run()
	{
		FString testSrc = TEXT("<#P0:<#I:%d>><#T:MMOGame.Mail.Buy.Success>");
		ParseContext ctx;
		DialogContext dtx;
		FString outStr;
		UITextParser::getSingleton().Parse(ctx, dtx, testSrc, outStr);
				
		if(dtx.TempText != TEXT(""))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

class TestCase_TextPasse_SO : public AbstractTestCase
{
public:

	virtual bool Run()
	{		
		FString testSrc = TEXT("<#SO:1>Server option text<#SO/>");
		ParseContext ctx;
		DialogContext dtx;
		FString outStr;
		UITextParser::getSingleton().Parse(ctx, dtx, testSrc, outStr);

		if(dtx.TempText == TEXT("") && dtx.OptionActionMap.size() == 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

class TestCase_TextPasse_NPC : public AbstractTestCase
{
public:

	virtual bool Run()
	{		
		FString testSrc = TEXT("<#N:-888>");
		ParseContext ctx;
		DialogContext dtx;
		FString outStr;
		UITextParser::getSingleton().Parse(ctx, dtx, testSrc, outStr);

		if(dtx.TempText == TEXT(""))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

class TestCase_TextPasse_UI : public AbstractTestCase
{
public:

	virtual bool Run()
	{
		FString testSrc = TEXT("<#D>Diag text<#D/><#UI:Shop>Test Open Shop<#UI/>");
		ParseContext ctx;
		DialogContext dtx;
		FString outStr;
		UITextParser::getSingleton().Parse(ctx, dtx, testSrc, outStr);

		if(dtx.DialogText.Num() && dtx.DialogText(0) == TEXT("Diag text") && dtx.OptionActionMap.size() == 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};



class TestCase_TextPasse_UI_DialogOption : public AbstractTestCase
{
public:

	virtual bool Run()
	{
		FString testSrc = TEXT("<#D>Diag text<#D/>");

		BYTE* buf1 = new BYTE[sizeof(ANPC)];
		BYTE* buf2 = new BYTE[sizeof(UNPCTemplate)];

		appMemzero(buf1, sizeof(ANPC));
		appMemzero(buf2, sizeof(UNPCTemplate));

		ANPC* npc = (ANPC*)buf1;
		UNPCTemplate* npcTem = (UNPCTemplate*)buf2;
		npcTem->EnableDiag_Auction.Enable = true;
		npcTem->EnableDiag_Auction.ClientDiagText = TEXT("<#UI:Auction>Open Auction<#UI/>");

		npcTem->EnableDiag_GemEmbed.Enable = true;
		npcTem->EnableDiag_GemEmbed.ClientDiagText = TEXT("<#UI:SoulEmbed>Open SoulEmbed<#UI/>");

		npc->Template = npcTem;
		ParseContext ctx;
		DialogContext dtx;
		ctx.NPC = npc;

		FString outStr;
		UITextParser::getSingleton().Parse(ctx, dtx, testSrc, outStr);

		delete[] buf1;
		delete[] buf2;
		if(dtx.OptionActionMap.size() == 2)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

class TestCase_TextPasse_ServerOptionUTF8 : public AbstractTestCase
{
public:

	virtual bool Run()
	{
		const FString srcServerOption = TEXT("这是中文的UTF8");
		std::string utfStr = TCHAR_TO_UTF8(*srcServerOption);
		FString serverOption = ANSI_TO_TCHAR(utfStr.c_str());

		FString testSrc = FString::Printf(TEXT("<#SO:1>%s<#SO/>"), *serverOption);
		ParseContext ctx;
		DialogContext dtx;
		FString outStr;
		UITextParser::getSingleton().Parse(ctx, dtx, testSrc, outStr);

		if(dtx.TempText == TEXT("") && dtx.OptionActionMap.size() == 1)
		{
			DialogContext::IDialogOptionActionPtr dlgPtr = dtx.OptionActionMap.begin()->second;
			if(dlgPtr->GetDisplayText() == srcServerOption)
			{
				return false;
			}
			return true;
		}
		else
		{
			return false;
		}
	}
};


class TestCase_ConvertServerEnumToClientEnum : public AbstractTestCase
{
public:

	virtual bool Run()
	{
		bool result = true;
		FString serverEnums[] =
		{
			TEXT("catMountMoveSpeed"),
			TEXT("CMS_PASSIVE_MOVE"),
		};

		FString clientEnums[] =
		{
			TEXT("E_cat_MountMoveSpeed"),
			TEXT("E_CMS_PASSIVE_MOVE"),
		};

		for (INT i = 0; i < sizeof(serverEnums)/sizeof(FString); ++i)
		{
			FString serverEnum = MMOUI::ConvertServerEnumToClientEnum(serverEnums[i], NULL);
			if(clientEnums[i] != serverEnum)
			{
				result = false;
				warnf(NAME_Error, TEXT("%s -> %s != %s"), *serverEnums[i], *serverEnum, *clientEnums[i]);
			}
		}

		return result;
	}
};

//Bruce@snda 
class TestCase_EncodeDecode : public AbstractTestCase
{
public:
	
	virtual bool Run()
	{
		char buf[256];
		char buf2[256];
 		for (INT i = 0; i < 256; ++i)
		{
			buf[i] = i;
			buf2[i] = i;
		}
		FArchiveEncode::Encode((BYTE*)buf, 256);
		FArchiveEncode::Decode((BYTE*)buf, 256);
		INT ns = appMemcmp(buf, buf2, 256);

		char usr[] = "skdfkaskdfk**kkTestCase5814_TipsCrashTestCase5814_TipsCrashTestCa盛大se5814_TipsCrashTestCase5814_TipsCrash盛大ase5814_TipsCrashTestCase5814_TipsCra盛大 sh814_TipsCrashTestC ";
		char usr2[] = "skdfkaskdfk**kkTestCase5814_TipsCrashTestCase5814_TipsCrashTestCa盛大se5814_TipsCrashTestCase5814_TipsCrash盛大ase5814_TipsCrashTestCase5814_TipsCra盛大 sh814_TipsCrashTestC ";
		int sst = sizeof(usr) / sizeof(usr[0]);
		FArchiveEncode::Encode((BYTE*)usr, sst);
		FArchiveEncode::Decode((BYTE*)usr, sst);
		INT n = appMemcmp(usr, usr2, sst);
		return n == 0;
	}
};
//Bruce@snda 

class TestCase_SpellClassType : public AbstractTestCase
{
public:

	virtual bool Run()
	{
		FSpell_ClassType spellClsType(EC_EventParm);
		INT tmpVal1 = 0;
		spellClsType.bClassType1 = 1;
		memcpy(&tmpVal1, &spellClsType, sizeof(FSpell_ClassType));

		INT tmpVal2 = 0;
		spellClsType.bClassType1 = 0;
		spellClsType.bClassType2 = 1;
		memcpy(&tmpVal2, &spellClsType, sizeof(FSpell_ClassType));

		INT tmpVal3 = 0;
		spellClsType.bClassType1 = 0;
		spellClsType.bClassType2 = 0;
		spellClsType.bClassType3 = 1;
		memcpy(&tmpVal3, &spellClsType, sizeof(FSpell_ClassType));

		INT tmpVal4 = 0;
		spellClsType.bClassType1 = 0;
		spellClsType.bClassType2 = 0;
		spellClsType.bClassType3 = 0;
		spellClsType.bClassType4 = 1;
		memcpy(&tmpVal4, &spellClsType, sizeof(FSpell_ClassType));

		INT tmpVal5 = 0;
		spellClsType.bClassType1 = 0;
		spellClsType.bClassType2 = 0;
		spellClsType.bClassType3 = 0;
		spellClsType.bClassType4 = 0;
		spellClsType.bClassType5 = 1;
		memcpy(&tmpVal5, &spellClsType, sizeof(FSpell_ClassType));

		INT tmpVal6 = 0;
		spellClsType.bClassType1 = 0;
		spellClsType.bClassType2 = 0;
		spellClsType.bClassType3 = 0;
		spellClsType.bClassType4 = 0;
		spellClsType.bClassType5 = 0;
		spellClsType.bClassType6 = 1;
		memcpy(&tmpVal6, &spellClsType, sizeof(FSpell_ClassType));
		warnf(NAME_Log, TEXT("ClassType:\n%s:%d\n%s:%d\n%s:%d\n%s:%d\n%s:%d\n%s:%d\n"),
			*MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.MMOAbility:Spell_ClassType.ClassType1")), tmpVal1,
			*MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.MMOAbility:Spell_ClassType.ClassType2")), tmpVal2,
			*MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.MMOAbility:Spell_ClassType.ClassType3")), tmpVal3,
			*MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.MMOAbility:Spell_ClassType.ClassType4")), tmpVal4,
			*MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.MMOAbility:Spell_ClassType.ClassType5")), tmpVal5,
			*MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.MMOAbility:Spell_ClassType.ClassType6")), tmpVal6
			);
		

		return true;
	}
};

class TestCase_URLEncoding : public AbstractTestCase
{
public:

	virtual bool Run()
	{
		FString srcServerOption = TEXT("☆调查●");
		char buf[1024] = {0};
		char bufd[1024] = {0};
		FTCHARToANSI ansiStr(*srcServerOption);
		if(WebHtmlManager::UrlEncode((ANSICHAR*)ansiStr, buf, 1023, FALSE))
		{
			//FString str = ANSI_TO_TCHAR(buf);
			WebHtmlManager::UrlDecode(buf, bufd, 1023);
			printf(("URL: %s -> %s"), (char*)buf, (char*)bufd);
		}
		std::string utfStr = TCHAR_TO_UTF8(*srcServerOption);
		std::string encodStr = WebHtmlManager::QURL_UrlEncode(utfStr);
		const char* encodStrBuf = encodStr.c_str();
		if(appStricmp(buf, encodStrBuf) == 0)
		{
			return true;
		}

		return false;
	}
};


class TestCase_CharacterNameCensoredWord : public AbstractTestCase
{
public:

	virtual bool Run()
	{
		bool testOK = true;
		for (int i = 0; i <= 32; ++i)
		{
			char c = (char)i;
			TCHAR wc = *ANSI_TO_TCHAR(&c);

			if(MMOUI::HasSpecialSymbol(FString(&wc)) != true)
			{
				warnf(NAME_Error, TEXT("Failed in band list:%d - %d"), c, wc);
				testOK = false;
			}
			else
			{
				warnf(NAME_Log, TEXT("OK:%d - %d"), c, wc);
			}
		}

		char c = (char)127;
		TCHAR wc = *ANSI_TO_TCHAR(&c);
		if(MMOUI::HasSpecialSymbol(FString(&wc)) != true)
		{
			warnf(NAME_Error, TEXT("Failed in band list:%d - %d"), c, wc);
			testOK = false;
		}
		else
		{
			warnf(NAME_Log, TEXT("OK:%d - %d"), c, wc);
		}		

		char test[4] = {'w', 'f', 0, 'c'};
		FString wsr(ANSI_TO_TCHAR(&c));
		if(MMOUI::HasSpecialSymbol(wsr) != true)
		{
			warnf(NAME_Error, TEXT("Failed in band list:%s"), *wsr);
			testOK = false;
		}
		else
		{
			warnf(NAME_Log, TEXT("OK:%s"), *wsr);
		}

		return testOK;
	}
};

class TestCase_ObjectDestroy : public AbstractTestCase
{
public:

	virtual bool Run()
	{
		G2C_DOODAD_APPEAR buf;
		buf.wMsg = g2c_doodad_appear;
		buf.AppearData.dwDoodadID = MakeObjectID(otDoodad, 414);
		buf.AppearData.byRemainOpenCount = 2;
		buf.AppearData.nTemplateID = 1;
		buf.AppearData.wCurrentX = 1;
		buf.AppearData.wCurrentY = 1;
		buf.AppearData.wCurrentZ = 1;
		buf.AppearData.wFaceDirection = 30;

		CMessageHandler::GetInstance().DebugParseMsg((const char*)&buf, sizeof(G2C_DOODAD_APPEAR));
		//CMessageHandler::GetInstance().Parse_g2c_doodad_appear((const char*)&buf, sizeof(G2C_DOODAD_APPEAR));

		buf.AppearData.dwDoodadID = MakeObjectID(otDoodad, 413);
		//CMessageHandler::GetInstance().Parse_g2c_doodad_appear((const char*)&buf, sizeof(G2C_DOODAD_APPEAR));
		CMessageHandler::GetInstance().DebugParseMsg((const char*)&buf, sizeof(G2C_DOODAD_APPEAR));

		UINT32 objId = MakeObjectID(otDoodad, 414);
		MMOFacade::getSingleton().DestroyObject(objId, TRUE, sodtLeaveScene);
		MMOFacade::getSingleton().DestroyObject(objId, TRUE, sodtLeaveScene);
		MMOFacade::getSingleton().DestroyObject(objId, FALSE, sodtLeaveScene);
		MMOFacade::getSingleton().DestroyObject(objId, FALSE, sodtLeaveScene);

		objId = MakeObjectID(otDoodad, 413);
		MMOFacade::getSingleton().DestroyObject(objId, TRUE, sodtOutOfRange);
		MMOFacade::getSingleton().DestroyObject(objId, TRUE, sodtOutOfRange);
		MMOFacade::getSingleton().DestroyObject(objId, FALSE, sodtOutOfRange);
		MMOFacade::getSingleton().DestroyObject(objId, FALSE, sodtOutOfRange);
		return true;
	}
};

void RunTestCase()
{
	M_TEST_CASE(TestCase_ObjectDestroy);
	M_TEST_CASE(TestCase_CharacterNameCensoredWord);
	M_TEST_CASE(TestCase_URLEncoding);
	//M_TEST_CASE(TestCase5814_TipsCrash);
	M_TEST_CASE(TestCase_FormatTipPara0);
	M_TEST_CASE(TestCase_FormatTipPara1);
	M_TEST_CASE(TestCase_FormatTipParaRecu);
	M_TEST_CASE(TestCase_TextPasse_SO);
	M_TEST_CASE(TestCase_TextPasse_NPC);
	M_TEST_CASE(TestCase_TextPasse_UI);
	M_TEST_CASE(TestCase_TextPasse_UI_DialogOption);
	M_TEST_CASE(TestCase_TextPasse_ServerOptionUTF8);
	M_TEST_CASE(TestCase_ConvertServerEnumToClientEnum);
	M_TEST_CASE(TestCase_EncodeDecode);	
	M_TEST_CASE(TestCase_SpellClassType);
}


#endif

