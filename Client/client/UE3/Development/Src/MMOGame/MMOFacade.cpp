#include "stdafx.h"
#include "ForceTry.h"
#include "MMOGameClasses.h"
#include "MMOFacade.h"
#include "MMOStats.h"
#include "id_def.h"
#include "scene_def.h"
#include "role_def.h"
#include "UITextParser.h"
#include "UnTerrain.h"
#include "FSMIO.h" 

#include "DeviceInfoHelper.h"

#include "..\..\Common\define\version.h" // oscar@snda
#include "error_code_name.h"
#include "MMODataManager.h"

#include "MMOAchievementManager.h"
#include "MMOGuildManager.h"
#include "FFileManagerWindows.h"
#include "FFileManagerMPQ.h"
#include "ScaleformEngine.h"
#include "AbilityProcessor.h"
#include "MMOBuffManager.h"
#include "MMOBattleManager.h"
#include "MMOEquipManager.h"
#include "MailManager.h"
#include "DoodadManager.h"
#include "MMOGuideManager.h"
#include "MMOChatManager.h"
#include "MMOGoldStoreManager.h"
#include "MMOGuideOptionManager.h"

#include "EngineAudioDeviceClasses.h"
#include "FriendChatManager.h"
#include "ClientActionConditionCheck.h"
#include "ClientSceneVariablesParse.h"
#include "MMOQuestManager.h"
#include "CommonGameEvents.h"
#include "MMOMapDataManager.h"
#include "LinkManager.h"
#include "g2c_message.h"
#include "TeamManager.h"
#include "MMOMsgBoxManager.h"
#include "MMOAttributeAccountManager.h"

#include "UILayout.h"
#include "MMOUtil.h"
#include "MMONavigationManager.h"
#include "MMODropManager.h"
#include "ReplayManager.h"
#include "MMOSkillManager.h"
//#include "MMOScreenMsgMgr.h"
#include "ScreenMsgQueue.h"
#include "Shop.h"
#include "MMOWeatherManager.h"
#include "MMOPowerManager.h"
#include "MMOItemDataManager.h"
#include "MMOHeavenManager.h"
#include "EquipSlotManager.h"
#include "MMOFreeTipsDataManager.h"
#include "MMOFreeTipsUIManager.h"
#include "ActivityManager.h"
#include "MMOArenaManager.h"
#include "MMONpcFactory.h"
#include "MMOCoolDownManager.h"
#include "SplashScreen.h"
#include "MasterStudentManager.h"
#include "MallManager.h"
#include "ScreenMsgQueue.h"
#include "MMOAuctionManager.h"
#include "MMOHurtManager.h"
#include "MMOBossBloodManager.h"
#include "LoadMapTipsManager.h"
#include "MMOPlayerInfoManager.h"
#include "MMOVipManager.h"
#include "FriendManager.h"
#include "MMOShopManager.h"
#include "MMOSystemActivateManager.h"
#include "MMOHandbookManager.h"
#include "MMOLoginManager.h"

#include "tabfile.h"
#include "DataErrorCheck.h"
#include "MMOHelperManager.h"
#include "WebHtmlManager.h"
#include "MMOPerformance.h"	// zhaojun@snda
#include "MMOJourneyManager.h"
#include "MMODeathManager.h"
#include "Mir2Attack.h"
#include "MMOAssistManager.h"
#include "MMOClientVariableManager.h"
#include "MMODailySignInManager.h"
#include "MMOEquipRemakeMgr.h"
#include "MMOJigsawManager.h"
#include "MMOVipManager.h"
#include "UStallManager.h"

#ifdef _DEBUG
#include "TestCase.h"
#endif

#if WITH_GFx
//#include "GFxUIClasses.h"
#include "ScaleformEngine.h"
#include "GFx/GFx_Event.h"
#include "GFx/GFx_InteractiveObject.h" // oscar@snda
#endif //WITH_GFx

using namespace std;

extern UMMOGFXPlayer_RootBase* g_rootGFXPlayer;
extern UBOOL g_IsMoving;
ETickOrder g_tickOrder = ETO_BeforeViewportTick; //ETO_AfterRenderingTick;	//

void _LoadItemTemplates();
//void _LoadAffixTemplates();
void _LoadNPCTemplates();
void _LoadMissileTemplates();
void _LoadBuffTemplates();
void _LoadMMOAppName();
//void _LoadBattleTemplates();

// template<> Facade* Singleton<Facade>::ms_Singleton = 0;
FString MMOFacade::mMMOAppName;

bool g_hideAllNPC = false;
bool g_hideAllPC = false;

INT g_TotalFrameNum = 0;
UBOOL g_bDurabilityShowingUpdatePos = FALSE;
UBOOL g_bGMBuff = FALSE;
UIMaps AllUIMaps;

typedef TMap<FName, UMMOGFXPlayer*> MoviePlayerMaps;
MoviePlayerMaps AllMoviePlayers;

typedef TMap<FName, FName> PanelToMoviePlayerMaps;
PanelToMoviePlayerMaps PanelToMoviePlayers;

//typedef std::vector<class UMMOUIBase*, M_UE3_STL_ALLOC(class UMMOUIBase*) > PanelQueue;
//PanelQueue g_PanelQueue;

typedef std::list<FString> PanelList;
PanelList g_PanelList;

//typedef TMap<FName, UMMOUIBase*> UMMOUIBaseMaps;
//UMMOUIBaseMaps g_PanelQueueMap;

StringMap g_StringMap;

//------------------------------------------------------------------------
// Game States
//------------------------------------------------------------------------
GameStateMap g_gameStatesMap;
EGameState g_CurrentState = EGameState_None;
UMMOGameState* g_CurrentStateObj = NULL;
UMMOGameState* g_PreStateObj = NULL;

#define CombineClassLevel( a , b )  ( (a)*100 + (b) )

//--------------
// perf test
//--------------
#if !FINAL_RELEASE
#define IncPlayerCount	(++PlayerCount)
#define DecPlayerCount	(--PlayerCount)
#else
#define IncPlayerCount
#define DecPlayerCount
#endif


TArray<HANDLE> g_CursorList;
INT g_currentCursorIndex = 0;
bool g_lockMouseCursorForUI = false;

#define MAX_RUNE_COUNT_NEW 2

#define MoneyRatio 1000

void MMOFacade::SetMouseCursor( EMMOCursorType cursorIndex)
{
	if(GIsEditor || g_lockMouseCursorForUI)
	{
		return;
	}

	UBOOL bNeedForceUpdate = FALSE;
	if (g_currentCursorIndex != cursorIndex)
	{
		bNeedForceUpdate = TRUE;
	}
	g_currentCursorIndex = cursorIndex;
	if(g_currentCursorIndex >= g_CursorList.Num())
	{
		if(!GIsEditor)
		{
			debugf(NAME_Error, TEXT("[Facade::SetMouseCursor] Mouse cursor index not found:%d"), g_currentCursorIndex);
		}

		g_currentCursorIndex = 0;
	}

	if (bNeedForceUpdate)
	{
		AMMOPlayerController* Controller = MMOFacade::getSingleton().GetMainController();
		if (Controller)
		{
			ULocalPlayer* lp = Cast<ULocalPlayer>(Controller->Player);
			if (lp && lp->ViewportClient)
			{
				lp->ViewportClient->ForceUpdateMouseCursor(TRUE);
			}
		}
	}
}

void ClearMMOPlayerCtrl()
{
	MMOFacade::getSingleton().ClearMainController();	
}
//class aaa
//{
//public:
//	aaa()
//	{ 
//		MMO::Facade::CheckName(TEXT("e瞭望c踯躅棄靥：）筽人1g"));
//	}
//};
//
//static aaa a;

FString MMOFacade::CheckName( const FString& word )
{
	char sz[30] = "";
	strncpy( sz, TCHAR_TO_ANSI(*word)  , 30);

	FString res;
	UBOOL firstChar = TRUE;

	for ( int i = 0 ; i < 30 ; i++ )
	{
		if( sz[i] == 0)
			break;

		if( firstChar && sz[i] < 0x80 && sz[i] > 0 )
		{
			res.AppendChar( sz[i] );
			continue;
		}

		if( firstChar )
		{
			firstChar = FALSE;
			continue;
		}
		else
		{
			

			firstChar = TRUE;

			BYTE high = sz[i-1];
			BYTE low = sz[i];
			short p = low + high * 256;
			int value = p + 65535;

			FString s;

			if( value >= 45217 && value <= 45252 )
			{
				s = TEXT("a");
			}
			else if( value >= 45253 && value <= 45760)
			{
				s =TEXT("b") ;
			}
			else if( value >= 45761 && value <= 46317 )
			{
				s = TEXT("c");
			}
			else if( value >= 46318 && value <= 46825 )
			{
				s =TEXT("d");
			}
			else if( value >= 46826 && value <= 47009 )
			{
				s = TEXT("e");
			}
			else if( value >= 47010 && value <= 47296 )
			{
				s = TEXT("f");
			}
			else if( value >= 47297 && value <= 47613 )
			{
				s = TEXT("g");
			}
			else if( value >= 47614 && value <= 48118 )
			{
				s = TEXT("h");
			}
			else if( value >= 48119 && value <= 49061 )
			{
				s = TEXT("j");
			}
			else if( value >= 49062 && value <= 49323 )
			{
				s = TEXT("k");
			}
			else if( value >= 49324 && value <= 49895 )
			{
				s = TEXT("l");
			}
			else if( value >= 49896 && value <= 50370 )
			{
				s = TEXT("m");
			}
			else if( value >= 50371 && value <= 50613 )
			{
				s = TEXT("n");
			}
			else if( value >= 50614 && value <= 50621 )
			{
				s = TEXT("o");
			}
			else if( value >= 50622 && value <= 50905 )
			{
				s = TEXT("p");
			}
			else if( value >= 50906 && value <= 51386 )
			{
				s =TEXT( "q");
			}
			else if( value >= 51387 && value <= 51445 )
			{
				s =TEXT( "r");
			}
			else if( value >= 51446 && value <= 52217 )
			{
				s =TEXT( "s");
			}
			else if( value >= 52218 && value <= 52697 )
			{
				s = TEXT("t");
			}
			else if( value >= 52698 && value <= 52979 )
			{
				s = TEXT("w");
			}
			else if( value >= 52980 && value <= 53688 )
			{
				s = TEXT("x");
			}
			else if( value >= 53689 && value <= 54480 )
			{
				s = TEXT("y");
			}
			else if( value >= 54481 && value <= 62289 )
			{
				s = TEXT("z");
			}
			else
			{
				s.AppendChar( sz[i-1] );
			}
			//warnf(  TEXT("%s"), *s );
			res += s;
		}

	}

	return res;
}

void MMOFacade::ShowMouseCursor(UBOOL Show)
{
	if(Show)
	{
		while ( ::ShowCursor(TRUE) <0 );
	}
	else
	{
		while ( ::ShowCursor(FALSE) >= 0 );
	}
}

void MMOFacade::SetChainPay( UBOOL open )
{
	_chainPay = open;
	if( _chainPay )
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.OpenChainPay") ) , MsgInfoType_Table , TEXT("MMOGame.Bag.OpenChainPay") ) ;
	}
	else
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.CloseChainPay") ) , MsgInfoType_Table , TEXT("MMOGame.Bag.CloseChainPay") ) ;
	}
}

void MMOFacade::SetCanvas(UCanvas* cvs)
{
	if(cvs)
	{
		Canvas = cvs;
		SceneView = cvs->SceneView;
		ViewProjectionMatrix = cvs->SceneView->ViewProjectionMatrix;
		if(ScreenX!=cvs->SceneView->SizeX || ScreenY!=cvs->SceneView->SizeY)
		{
			g_bDurabilityShowingUpdatePos = TRUE;
		}
		ScreenX = cvs->SceneView->SizeX;
		ScreenY = cvs->SceneView->SizeY;

	}

}

FMatrix	MMOFacade::GetWorldView()
{
	return ViewProjectionMatrix;
}


void MMOFacade::UpdateScreenPixelFromWorld()
{
	if(MainController){
		ULocalPlayer* lp = Cast<ULocalPlayer>(MainController->Player);
		if(lp)
		{
			lp->GetProjectViewMatrix(&MatViewProjOfLocalPlayer);
		}
	}


	if(g_bDurabilityShowingUpdatePos)
	{
		g_bDurabilityShowingUpdatePos=FALSE;
		UMMOUIDurabilityShowing *durabilityShowingPanel = (UMMOUIDurabilityShowing*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_DurabilityShowing );
		if(durabilityShowingPanel && durabilityShowingPanel->IsVisible)
		{
			durabilityShowingPanel->UpdatePosition();
		}
	}
}

//void MMOFacade::ClearSceneView()
//{
//	delete ProjectView;	// notion: will cause the static view family context invalid.
//}

UBOOL MMOFacade::GetScreenPixelFromWorld(FVector WorldPos,FVector2D& OutPos)
{
	//最新方式，快速
	//if(ProjectView)
	{
		FPlane Result = MatViewProjOfLocalPlayer.TransformFVector4(FVector4(WorldPos,1));
		const FLOAT RHW = 1.0f / Result.W;
		FPlane V(Result.X * RHW,Result.Y * RHW,Result.Z * RHW,Result.W);
		
		OutPos   = FVector2D((1.0f + V.X) * 0.5f, 1.0f - (1.0f + V.Y) * 0.5f) * FVector2D(ScreenX,ScreenY);
		//if(g_IsMoving==FALSE)
		//{
		//	OutPos.X = appFloor(OutPos.X + 0.5f);
		//	OutPos.Y = appFloor(OutPos.Y + 0.5f);
		//}
		return TRUE;
	}
	//return FALSE;


	////次新方式，完整
	//if(MainController){
	//	ULocalPlayer* lp = Cast<ULocalPlayer>(MainController->Player);
	//	if(lp)
	//	{
	//		OutPos = lp->Project(WorldPos)*FVector2D(ScreenX,ScreenY);
	//		return TRUE;
	//	}
	//}
	//return FALSE;


	//老方式
	//if(!Canvas)
	//	return FALSE;

	//const FVector4 ScreenPoint = ViewProjectionMatrix.TransformFVector4(FVector4(WorldPos,1));
	//

	//if(ScreenPoint.W > 0.0f)
	//{
	//	FLOAT InvW = 1.0f / ScreenPoint.W;
	//	OutPos = FVector2D(
	//		0 + (0.5f + ScreenPoint.X * 0.5f * InvW) * ScreenX,
	//		0 + (0.5f - ScreenPoint.Y * 0.5f * InvW) * ScreenY
	//		);
	//	return TRUE;
	//}
	//else
	//{
	//	return FALSE;
	//}

	
}

FSceneView* MMOFacade::GetSceneView()
{
	return SceneView;
}

UCanvas* MMOFacade::GetCanvas()
{
	return Canvas;
}



FIntPoint MMOFacade::GetMouseCursorPosition()
{
	FIntPoint pt(-1,-1);
	
        // zhaojun@snda, add the if() to avoid crash
        // 1. in editor close the viewport; 2.call this func somewhere; 3. crash
	if (FGFxEngine::GetEngine()->GetRenderViewport())
	{
		FGFxEngine::GetEngine()->GetRenderViewport()->GetMousePos(pt);
	}
	
	return pt;
}

UBOOL MMOFacade::GetCaptureInput()
{
	return MMOFacade::getSingleton().GameCaptureInput;
}

void MMOFacade::UpdateCaptureInput()
{
#if 0 // oscar@snda
	/////////////////////////////////////////////////////////////////
	FIntPoint MousePos = GetMouseCursorPosition();
	FGFxMovie* pTopMostMovie = NULL;
	GFx::InteractiveObject* pTopMostEntity = FGFxEngine::GetEngine()->GetTopMostMovie( &pTopMostMovie, GFx::PointF(MousePos.X, MousePos.Y));
	MMOFacade::getSingleton().GameCaptureInput = FALSE;

	if(pTopMostMovie && pTopMostEntity){
		GFx::PointF MPInMovie;
		UGFxMoviePlayer* FocusedMoviePlayer = pTopMostMovie->pUMovie;
		pTopMostMovie->pView->MousePosToMovieSpace(MPInMovie, GFx::PointF(MousePos.X, MousePos.Y));

		if(FocusedMoviePlayer){
			MMOFacade::getSingleton().GameCaptureInput = ( FocusedMoviePlayer->bCaptureInput 
				|| FocusedMoviePlayer->bCaptureMouseInput 
				|| !FocusedMoviePlayer->eventSkipCaptureInputKey(TEXT(""), IE_Pressed, 
				pTopMostEntity != NULL ? UTF8_TO_TCHAR(pTopMostEntity->GetName().ToCStr()) : TEXT(""), 
				FVector2D(MPInMovie.x, MPInMovie.y)));
		}
	}
#endif
}

class UGFxObject* MMOFacade::GetMouseTopMostEntity()
{
	//FIntPoint MousePos = GetMouseCursorPosition();
	//FGFxMovie* pTopMostMovie = NULL;
	//GFx::InteractiveObject* pTopMostEntity = FGFxEngine::GetEngine()->GetTopMostMovie( &pTopMostMovie, GFx::PointF(MousePos.X, MousePos.Y), 0, TRUE, NULL, TRUE,NULL);
	
	if(g_rootGFXPlayer)
	{
		TopMostObject = NULL;
		g_rootGFXPlayer->eventCallGetMouseTopMostEntity();
		return TopMostObject;

		//UMMOGFXPlayer* TopMostMoviePlayer = Cast<UMMOGFXPlayer>(pTopMostMovie->pUMovie);
		//if(TopMostMoviePlayer){
		//	TopMostObject = NULL;
		//	TopMostMoviePlayer->eventCallGetMouseTopMostEntity();
		//	return TopMostObject;
		//}
	}
	return NULL;
}

//------------------------------------------------------------------------

class FInitFacadeAsyncTask : public FNonAbandonableTask
{
	friend class FAsyncTask<FInitFacadeAsyncTask>;

public:

	void DoWork()
	{
		MMOFacade::getSingleton().Init();
	}

	static const TCHAR *Name()
	{
		return TEXT("FInitFacadeAsyncTask");
	}
};

#ifdef _DEBUG
bool TestCensoredWord();
#endif

// zhaojun@snda

TArray<FString>	g_ArrayTempFont;

void UnRegisterMMOFontFile()
{
#if 0
	UBOOL b;

	UBOOL bFlushed = FALSE;

	for (int i=0; i < g_ArrayTempFont.Num(); ++i)
	{
		const FString& strTempFont = g_ArrayTempFont(i);

		b = ::RemoveFontResource(*strTempFont);

		if (b)
		{
			bFlushed = TRUE;
		}
	}

	if (bFlushed)
	{
		::SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0/*(LPARAM)(*strKey)*/);
	}
#endif
}

void RegisterMMOFontFile()
{
	g_ArrayTempFont.Empty();
	
	const int MAX_KEY_LENGTH = 255;
	const int MAX_VALUE_NAME = 16383;

#ifdef _WINDOWS_
	// register the font reg-key to avoid the font installation
	class FontValidEntry
	{
	public:
		FString m_strFontName;
		FString m_strFileName;
		bool bKeyExist;
		bool bValueValid;

	public:
		void Set(const FString& fontName, const FString& fileName)
		{
			m_strFontName = fontName;
			m_strFileName = fileName;

			bKeyExist = false;
			bValueValid = false;
		}
	};

	class FontValidList
	{
	public:
		void Init()
		{
			INT FontCount = 0;

			GConfig->GetInt(TEXT("Font"), TEXT("FontCount"), FontCount, GUIIni);

			if (FontCount > 0)
			{
				TCHAR workDir[MAX_PATH];

				GetCurrentDirectory(MAX_PATH, workDir);

				FString strFontFilePath;

				FString strRelativePath;

				GConfig->GetString(TEXT("Font"), TEXT("FontRelativePath"), strRelativePath, GUIIni);

				strFontFilePath = workDir;
				strFontFilePath += strRelativePath;
				SetFontPath(strFontFilePath);

				TArray<FString> arrayFontNames;
				TArray<FString> arrayFontFiles;
				GConfig->GetArray(TEXT("Font"), TEXT("FontName"), arrayFontNames, GUIIni);
				GConfig->GetArray(TEXT("Font"), TEXT("FontFile"), arrayFontFiles, GUIIni);

				check(arrayFontNames.Num() == FontCount);
				check(arrayFontFiles.Num() == FontCount);

				m_arrayFontEntry.AddZeroed(FontCount);

				for (INT f = 0; f < FontCount; ++f)
				{
					m_arrayFontEntry(f).Set(arrayFontNames(f), arrayFontFiles(f));
				}
			}
			//GConfig->GetBool();


			//FString strWorkDir(workDir);
			//strWorkDir += TEXT("\\..\\..\\MMOGame\\Flash\\Fonts");
			//SetFontPath(strWorkDir);

			//// move it to config
			//m_listFontEntry[0].Set(TEXT("AgeOfDawn (TrueType)"), TEXT("AgeOfDawn.ttf"));
			//m_listFontEntry[1].Set(TEXT("方正粗圆_GBK (TrueType)"), TEXT("方正粗圆_GBK.ttf"));
			//m_listFontEntry[2].Set(TEXT("AgeOfDawn (TrueType)"), TEXT("AgeOfDawn_1.ttf"));
			//m_listFontEntry[3].Set(TEXT("AgeOfDawn (TrueType)"), TEXT("AgeOfDawn_1.ttf"));
			//m_listFontEntry[4].Set(TEXT("AgeOfDawn (TrueType)"), TEXT("AgeOfDawn_1.ttf"));
		}
		void SetFontPath(const FString& strBasePath)
		{
			m_strBasePath = strBasePath;
		}

		void OnEntry(const TCHAR* pKey, const TCHAR* pValue)
		{
			for (int e = 0; e < m_arrayFontEntry.Num(); ++e)
			{
				FontValidEntry& entry = m_arrayFontEntry(e);

				if (entry.m_strFontName == pKey)
				{
					entry.bKeyExist = true;

					FFilename strPath(pValue);

					if (strPath.GetPath().Len() == 0 || strPath.FileExists())
					{
						entry.bValueValid = true;
					}
				}
			}
		}

		bool Flush(HKEY hKey)
		{
			// 2. if yes, validate it
			// 3. if no, find the font file and then register it
			bool bFlushed = false;

			//TCHAR  sWinDir  [MAX_PATH];  
			//::GetWindowsDirectory(sWinDir,MAX_PATH);  
			//FFilename  systemFontPath(sWinDir);  
			//systemFontPath  +=  TEXT("\\Fonts\\"); 

			FFilename srcPath(m_strBasePath);
			srcPath += TEXT("\\");

			for (int e = 0; e < m_arrayFontEntry.Num(); ++e)
			{
				FontValidEntry& entry = m_arrayFontEntry(e);

				if (!entry.bKeyExist || !entry.bValueValid)
				{

					FFilename srcFile(srcPath);
					srcFile += entry.m_strFileName;

					//FFilename fontName(entry.m_strFontName);

					if (srcFile.FileExists())
					{
						// write the registry
						// replace value

						//FFilename targetFile(systemFontPath);
						//targetFile += entry.m_strFileName;

						//CopyFile((*srcFile),(*targetFile),FALSE);  
						//DWORD h2 = GetLastError();


						//////////////////////////////////////////////////////////////////////////

						//FString sFOTFile = srcFile.GetBaseFilename(FALSE);
						//sFOTFile += TEXT(".FOT");  

						BOOL  Result;  
						//Result  =  ::CreateScalableFontResource(0,  (*sFOTFile),  (*(entry.m_strFileName)),  (*srcPath));
						//DWORD h = GetLastError();

						Result  =  ::AddFontResource((*srcFile));  

						if (Result > 0)
						{
							g_ArrayTempFont.AddItem(srcFile);
						}


						//////////////////////////////////////////////////////////////////////////
						//::RegSetValueEx(hKey, *(entry.m_strFontName), 0, REG_SZ, 
						//	(const BYTE*)(*entry.m_strFileName), 
						//	entry.m_strFileName.Len() * sizeof(TCHAR));

						//::DeleteFile(*targetFile);  

						bFlushed = true;
					}
				}
			}

			return bFlushed;
		}

	protected:
		FString	m_strBasePath;
		TArray<FontValidEntry> m_arrayFontEntry;
	};

	HKEY	hKey;
	LONG	lReturn;
	// 1. has the font been installed;
	// 1.1 open the main key
	FString strKey(TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"));
	
	lReturn = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, *strKey, 0, KEY_READ, &hKey);

	bool bFlushed = false;

	if (lReturn != ERROR_SUCCESS)
	{
		debugf(TEXT("can't open font key"));
	}
	else
	{
		// 1.2 enum the font-key
		//TCHAR szName[MAX_PATH];

		FontValidList vl;
		vl.Init();

		strKey += TEXT("\\");

		//TCHAR    achKey[MAX_KEY_LENGTH];		// buffer for subkey name
		//DWORD    cbName;						// size of name string 
		TCHAR    achClass[MAX_PATH] = TEXT(""); // buffer for class name 
		DWORD    cchClassName = MAX_PATH;		// size of class string 
		DWORD    cSubKeys=0;					// number of subkeys 
		DWORD    cbMaxSubKey;					// longest subkey size 
		DWORD    cchMaxClass;					// longest class string 
		DWORD    cValues;						// number of values for key 
		DWORD    cchMaxValue;					// longest value name 
		DWORD    cbMaxValueData;				// longest value data 
		DWORD    cbSecurityDescriptor;			// size of security descriptor 
		FILETIME ftLastWriteTime;				// last write time 

		DWORD i, retCode; 

		TCHAR  achValue[MAX_VALUE_NAME]; 
		TCHAR achData[MAX_VALUE_NAME];
		DWORD cchValue = MAX_VALUE_NAME; 


		// Get the class name and the value count. 

		retCode = ::RegQueryInfoKey(
			hKey,                    // key handle 
			achClass,                // buffer for class name 
			&cchClassName,           // size of class string 
			NULL,                    // reserved 
			&cSubKeys,               // number of subkeys 
			&cbMaxSubKey,            // longest subkey size 
			&cchMaxClass,            // longest class string 
			&cValues,                // number of values for this key 
			&cchMaxValue,            // longest value name 
			&cbMaxValueData,         // longest value data 
			&cbSecurityDescriptor,   // security descriptor 
			&ftLastWriteTime);       // last write time 

		// Enumerate the subkeys, until RegEnumKeyEx fails.

		//if (cSubKeys)
		//{
		//	for (i=0; i<cSubKeys; i++) 
		//	{ 
		//		cbName = MAX_KEY_LENGTH;
		//		retCode = ::RegEnumKeyEx(hKey, i,
		//			achKey, 
		//			&cbName, 
		//			NULL, 
		//			NULL, 
		//			NULL, 
		//			&ftLastWriteTime); 

		//		if (retCode == ERROR_SUCCESS) 
		//		{

		//		}
		//	}
		//} 

		// Enumerate the key values. 

		if (cValues) 
		{
			for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
			{ 
				cchValue = MAX_VALUE_NAME; 
				achValue[0] = '\0'; 
				//retCode = RegEnumKey(hKey, i, )
				retCode = ::RegEnumValue(hKey, i, 
					achValue, 
					&cchValue, 
					NULL, 
					NULL,
					(LPBYTE)(achData),
					&cchValue);

				if (retCode == ERROR_SUCCESS ) 
				{
					// check the entry
					vl.OnEntry(achValue, achData);
				} 
			}
		}

		// valid and flush the entry
		
		//::RegCloseKey(hKey);
		
		//lReturn = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, *strKey, 0, KEY_SET_VALUE, &hKey);

		bFlushed = vl.Flush(hKey);
	}

	::RegCloseKey(hKey);

	if (bFlushed)
	{
		::SendNotifyMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0/*(LPARAM)(*strKey)*/);
	}

#endif
}
// ~zhaojun@snda

void OnMMOPreEngineInit()	// zhaojun@snda
{
	SetMemPerformanceProfiler(&g_mmoTexturePerfProfiler);
}

extern UBOOL g_focusInTextInput;

void OnMMOInit()
{
	appSetSplashText(SplashTextType::StartupProgress, TEXT("初始化游戏数据..."));

	if (!GIsEditor)
		UMMOKeyManager::getSingleton()->InitKeyManager();

	warnf(NAME_Log, TEXT("Init RegisterMMOFontFile"));

#if !SHIPPING_PC_GAME
	RegisterMMOFontFile();
#endif

#ifdef _DEBUG
	TestCensoredWord();
#endif

#ifdef _DEBUG
	MMOUI::TrimNumber(TEXT("NPC8"));
	MMOUI::TrimNumber(TEXT("NPC10086"));
	FLOAT fTest = appAtof(TEXT("0.75"));
#endif

	//if(Facade::getSingletonPtr() == NULL)
	{
		//new Facade();
		MMOFacade::getSingleton().Init();	
		g_focusInTextInput = FALSE;
		M_ReportError();
		M_EnableErrorToConsole(true);

		//FAutoDeleteAsyncTask<FInitFacadeAsyncTask>* task = new FAutoDeleteAsyncTask<FInitFacadeAsyncTask>;
		//task->StartBackgroundTask();
	}

	if(LinkManager::getSingletonPtr() == NULL)
	{
		new LinkManager();
	}

#if M_ENABLE_RPMGR
	ReplayManager::getSingleton().Init();
#endif
}
//------------------------------------------------------------------------
void OnMMOExit()
{
#if M_ENABLE_RPMGR
	ReplayManager::getSingleton().ShutDown();
#endif

	//if(Facade::getSingletonPtr())
	{
		MMOFacade::getSingleton().Shutdown();
	}

#if !SHIPPING_PC_GAME
	UnRegisterMMOFontFile();
#endif
}
//------------------------------------------------------------------------
void _OnMMOTick(FLOAT DeltaTime)
{
	M_FORCE_TRY_BEGIN;

#if !M_USE_FORCE_TRY
	SCOPE_CYCLE_COUNTER(STAT_MMOTICK);
#endif

	MMOFacade::getSingleton().TickLogic(DeltaTime);

#if M_ENABLE_RPMGR
	ReplayManager::getSingleton().Tick();
#endif

	M_FORCE_TRY_END("Protected for OnMMOTick");
}

void TickGameLogic_BeforeGFXTick(float DeltaTime)
{
	if(g_tickOrder == ETO_BeforeGfxTick)
	{
		_OnMMOTick(DeltaTime);
		logTickOrderf(TEXT("ETO_BeforeGfxTick"));
	}
}

void TickGameLogic_AfterGFXTick(float DeltaTime)
{
	if(g_tickOrder == ETO_AfterGfxTick)
	{
		_OnMMOTick(DeltaTime);
		logTickOrderf(TEXT("ETO_AfterGfxTick"));
	}
}

void TickGameLogic_BeforeRenderingTick(float DeltaTime)
{
	if(g_tickOrder == ETO_BeforeRenderingTick)
	{
		_OnMMOTick(DeltaTime);
		logTickOrderf(TEXT("ETO_BeforeRenderingTick"));
	}
}

void TickGameLogic_AfterRenderingTick(float DeltaTime)
{
	if(g_tickOrder == ETO_AfterRenderingTick)
	{
		_OnMMOTick(DeltaTime);
		logTickOrderf(TEXT("ETO_AfterRenderingTick"));
	}
}

void TickGameLogic_AfterEngineTick(float DeltaTime)
{
	if(g_tickOrder == ETO_AfterEngineTick)
	{
		_OnMMOTick(DeltaTime);
		logTickOrderf(TEXT("ETO_AfterEngineTick"));
	}
}


void TickGameLogic_BeforeViewportTick(FLOAT DeltaTime)
{
	if(g_tickOrder == ETO_BeforeViewportTick)
	{
		_OnMMOTick(DeltaTime);
		logTickOrderf(TEXT("ETO_BeforeViewportTick"));
	}
}

void TickGameLogic_AfterViewportTick(FLOAT DeltaTime)
{
	if(g_tickOrder == ETO_AfterViewportTick)
	{
		_OnMMOTick(DeltaTime);
		logTickOrderf(TEXT("ETO_AfterViewportTick"));
	}
}

//------------------------------------------------------------------------
// if return true, the engine will not handle the message.
UBOOL MMOPreHandleOSDeferMessage(UINT Message, WPARAM w, LPARAM l)	// 引擎defer处理相关windows消息前调用，zhaojun@snda
{
	SCOPE_CYCLE_COUNTER(STAT_MMOPreHandleOSDeferMessage);

	if (!GIsEditor)
	{
		switch(Message)
		{
		case WM_CLOSE:
			//if( ViewportClient )
			{
				//GetPC().ConsoleCommand("exit"  );

				if(MMO::GetCurrentGameState() == EGameState_InGame)
				{
					//FScriptDelegate Fun(EC_EventParm);

					//UMMOUIBase *pUI = 0;
					//MMOUI::GetPanelByString("SystemSetting", &pUI);

					//if (pUI)
					//{
					//	Fun.Object = pUI->GFXPlayer;
					//	Fun.FunctionName = TEXT("LeaveGameWithoutConfirm");
					//}
					//else
					//{
					//	Fun.Object = NULL;
					//	Fun.FunctionName = TEXT("");
					//}

					//FScriptDelegate FunNone(EC_EventParm);
					//FunNone.Object = NULL;
					//FunNone.FunctionName = TEXT("");


					//MMOMsgBoxManager::getSingleton().AddMessageBoxEvent(
					//	MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MsgBox.text11") ),
					//	0, GameEvent_ExitMsg );
                    
					//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_ActivityQuit, TRUE, "");
					MMOUI::GetUIManager()->SendQuitGame();
				}
				else
				{
					MMOFacade::getSingleton().GetMainController()->ConsoleCommand(TEXT("exit"));
				}	
			}
			return TRUE;
//		case WM_QUIT:
//			{
//#if SHIPPING_PC_GAME
//				if(GIsGame)
//				{
//					WebHtmlManager::getSingleton().OpenGameQuitWeb();
//				}
//#endif
//			}
//			break;
		default:
			break;
		}
	}

	return FALSE;
}

//------------------------------------------------------------------------
void MMOPostHandleOSDeferMessage(UINT Message, WPARAM w, LPARAM l)	// 引擎defer处理相关windows消息后调用，zhaojun@snda
{
	// 暂时没有使用
}
//------------------------------------------------------------------------
//static UBOOL bSortPickedActors = FALSE;
static QSORT_RETURN CDECL ComparePickedResultWithPriority( const FCheckResult** AA, const FCheckResult** AB )
{
	typedef TScriptInterface<class IIPickableActor> IPickableActor;
	IPickableActor A, B;

	A = ScriptInterfaceCast<IIPickableActor>((*AA)->Actor);
	B = ScriptInterfaceCast<IIPickableActor>((*AB)->Actor);

	if(A && B)
	{
		if(A->GetPickPriority() < B->GetPickPriority())
		{
			return -1;
		}
		else if(A->GetPickPriority() == B->GetPickPriority())
		{
			if((*AA)->Time < (*AB)->Time)
				return -1;
		}
	}

	return 1;
}

static QSORT_RETURN CDECL ComparePickedResultWithoutPriority( const FCheckResult** AA, const FCheckResult** AB )
{
	typedef TScriptInterface<class IIPickablePoint> IPickablePoint;
	IPickablePoint A, B;

	A = ScriptInterfaceCast<IIPickablePoint>((*AA)->Actor);
	B = ScriptInterfaceCast<IIPickablePoint>((*AB)->Actor);

	if(A && B)
	{
		if((*AA)->Time < (*AB)->Time)
			return -1;
	}
	return 1;
}

float PickTimeClamp(float f, float l, float r)
{
    // zhaojun@snda 2015/7/23
	return (f*(r-l)+l);
}

FCheckResult* MMO::PickActor(FVector StartWorldPos,FVector EndWorldPos, AActor* SrcActor, 
	UBOOL TestAABBOnly/* = FALSE*/, QSORT_COMPARE fnSort/* = NULL*/, DWORD TraceFlags/* = TRACE_Actors | TRACE_ComplexCollision*/,
	const float SkeletalMin/* = 0.0f*/, const float SkeletalMax/* = 1.0f*/,
	const float CylinderMin/* = 0.0f*/, const float CylinderMax/* = 1.0f*/,
	const float AABBMin/* = 0.9999f*/, const float AABBMax/* = 1.0f*/)
{
	SCOPE_CYCLE_COUNTER(MMOSTAT_PickActor)

	//DWORD TraceFlags = TRACE_Actors | TRACE_ComplexCollision;// | TRACE_Blocking;

	// 	TraceFlags &= ~TRACE_SingleResult;
	// 	TraceFlags &= ~TRACE_StopAtAnyHit;

	FCheckResult* HitActors[64] = {0};
	INT HitNum = 0;

	FMemMark Mark(GMainThreadMemStack);

	FCheckResult* HitR = GWorld->MultiLineCheck( GMainThreadMemStack, EndWorldPos,
		StartWorldPos, FVector(0,0,0), TraceFlags, SrcActor, NULL, TestAABBOnly );

	Mark.Pop();

	if(HitR)
	{
		while(HitR && (HitNum < 64))
		{
			typedef TScriptInterface<class IIPickableActor> IPickableActor;
			IPickableActor A = ScriptInterfaceCast<IIPickableActor>(HitR->Actor);
			UBOOL IsPickable = TRUE;
			if(HitR->Actor)
			{
				IsPickable = HitR->Actor->IsPickable;
			}

			if(A && IsPickable)
			{
				HitActors[HitNum] = HitR;
				++HitNum;
			}
			HitR = HitR->GetNext();

		}

		for (INT h = 0; h < HitNum; ++h)
		{
			FCheckResult* pResult = HitActors[h];

			FLOAT CollisionRadius;
			FLOAT CollisionHeight;
			const FLOAT miniActorThreshold = 32.0f;

			if (pResult && pResult->Actor)
			{
				if (pResult->Time < 1.0f)
				{
					pResult->Actor->GetBoundingCylinder(CollisionRadius, CollisionHeight);
					UPrimitiveComponent* pPrimitiveComponent = pResult->Component;

					pResult->Time = PickTimeClamp(pResult->Time, CylinderMin, CylinderMax);

					if (CollisionRadius < miniActorThreshold && CollisionHeight < miniActorThreshold && (!pResult->Actor->IsA(AMMOPlayer::StaticClass())))
					{
						// small obj
						//return pResult;
					}
					else if (pPrimitiveComponent)
					{
						// big obj
						// override the cylinder with s
						if (pPrimitiveComponent->IsA(UCylinderComponent::StaticClass()))
						{
							// try to check the skeletal mesh
							if (pResult->Actor->IsA(APawn::StaticClass()))
							{
								APawn* pPawn = Cast<APawn>(pResult->Actor);
								USkeletalMeshComponent* pSkeletal = pPawn->OverriddenMesh != NULL ? pPawn->OverriddenMesh : pPawn->Mesh;

								if (pSkeletal != NULL)
								{
									FCheckResult CRTemp;
									CRTemp.Time = 1.0f;
									pSkeletal->LineCheck(CRTemp, EndWorldPos, StartWorldPos, FVector(0,0,0), TraceFlags);

									if (CRTemp.Time < 1.0f)
										pResult->Time = PickTimeClamp(CRTemp.Time, SkeletalMin, SkeletalMax);
								}
							}
						}

						//return pResult;
					}
				}
				else
				{
					// only hit the aabb, but the aabb can't be too big

					pResult->Actor->GetBoundingCylinder(CollisionRadius, CollisionHeight);

					if (CollisionRadius < miniActorThreshold && CollisionHeight < miniActorThreshold)
					{
						//return pResult;
						pResult->Time = PickTimeClamp(pResult->Time, AABBMin, AABBMax);
					}
				}
			}
		}

		//bSortPickedActors = bSort;
		//if(bSort)
		{ // sort by pick priority
			SCOPE_CYCLE_COUNTER(STAT_Col_Sort);
			appQsort( HitActors, HitNum, sizeof(FCheckResult*), 
				fnSort != NULL ? fnSort : (QSORT_COMPARE)ComparePickedResultWithPriority );
		}

		for (INT h = 0; h < HitNum; ++h)
		{
			FCheckResult* pResult = HitActors[h];

			if (pResult && pResult->Time < 1.0f)
			{
				return pResult;
			}
			else if (pResult && pResult->Time == 1.0f)//aabb
			{
				AMMOPlayer* pPlayer = Cast<AMMOPlayer>(pResult->Actor);
				if(pPlayer)
				{
					AMMOPlayer* pMainPlayer =MMOFacade::getSingleton().GetMainPlayer();
					{
						return pResult;
					}
					//if(pPlayer->isMoving)
					//{
					//	return pResult;
					//}
					//else if(pMainPlayer && pMainPlayer->isMoving)
					//{
					//	return pResult;
					//}
				}
			}
		}
	}

	return NULL;
}

FCheckResult* MMO::PickPoint(FVector StartWorldPos,FVector EndWorldPos, AActor* SrcActor, 
	UBOOL TestAABBOnly/* = FALSE*/, QSORT_COMPARE fnSort/* = NULL*/, DWORD TraceFlags/* = TRACE_AllComponents |TRACE_Actors*/)
{
	SCOPE_CYCLE_COUNTER(MMOSTAT_PickActor)

	//| TRACE_ComplexCollision;// | TRACE_Blocking;

	// 	TraceFlags &= ~TRACE_SingleResult;
	// 	TraceFlags &= ~TRACE_StopAtAnyHit;

	FCheckResult* HitActors[64] = {0};
	INT HitNum = 0;

	FMemMark Mark(GMainThreadMemStack);

	FCheckResult* HitR = GWorld->MultiLineCheck( GMainThreadMemStack, EndWorldPos,
		StartWorldPos, FVector(0,0,0), TraceFlags, SrcActor, NULL, TestAABBOnly );

	Mark.Pop();

	if(HitR)
	{
		while(HitR && (HitNum < 64))
		{
			typedef TScriptInterface<class IIPickableActor> IPickableActor;
			IPickableActor A = ScriptInterfaceCast<IIPickableActor>(HitR->Actor);
			AStaticMeshActor* sma = Cast<AStaticMeshActor>(HitR->Actor);
			UBOOL IsPickable = TRUE;
			if(HitR->Actor)
			{
				IsPickable = HitR->Actor->IsPickable;
			}

			if(sma && sma->bCanHidden)
			{
				//++HitNum;
			}
			else if(A && IsPickable)
			{
				HitActors[HitNum] = HitR;
				++HitNum;
			}
			HitR = HitR->GetNext();

		}
		//bSortPickedActors = bSort;
		//if(bSort)
		{ // sort by pick priority
			SCOPE_CYCLE_COUNTER(STAT_Col_Sort);
			appQsort( HitActors, HitNum, sizeof(FCheckResult*), 
				fnSort != NULL ? fnSort : (QSORT_COMPARE)ComparePickedResultWithPriority );
		}

//#if !SHIPPING_PC_GAME
//		FCheckResult* pResult = HitActors[0];
//		if (pResult)
//		{
//			debugf(TEXT("======MMO::PickPoint:%s"), (pResult->Component ? *pResult->Component->GetOwner()->GetFullName() : TEXT("Unknown!")));
//		}
//#endif

		return HitActors[0];
	}

	return 0;
}



//------------------------------------------------------------------------
// Facade
//------------------------------------------------------------------------
MMOFacade::MMOFacade(void)
	: MainController(0)
	, AbilityProcessorPtr(0)
	, MMOBuffManagerPtr(0)
	, _isRepairing(FALSE)
	, UIHudManager(0)
	, LastInGameScene(FALSE)
	, Canvas(0)
	, SceneView(0)
	, _sceneLevel(0)
	, _sceneDifficulty(0)
	, MatViewProjOfLocalPlayer(FMatrix::Identity)
	, _SortTimerCounter(0)
	, _LockSortMode(LSM_SortNone)
	, _SortTimer(0.0f)
	, PlayerInfoRequestTime(0.0f)
	, _UpdateSortTimer(false)
	//, _CurrentSceneID(0)
	, PlayerCount(0)
	, NPCCount(0)
	, _chainPay(FALSE)
	, _todayExpIsFull(FALSE)
	, GCTType(GC_MIR2)
	, TopMostObject(NULL)
	, BlockType(0)
	, BlockRadius(32)
	, LastSyncTickCount(0)
	, DelayPositionSetting(FALSE)
	, PetState(E_ps_Invalid)
	, IsReceivedGameData(FALSE)
	, bLevelsLoaded(FALSE)
{
	appMemzero(&_PlayerMoney, sizeof(G2C_SYNC_MONEY));
	appMemzero(ContainerSize, E_CONSLOT_MAX * sizeof(UINT8));

	MMOUI::RegisterEventHandler(this, GameEvent_RequestMainPlayerData);
	MMOUI::RegisterEventHandler(this, GameEvent_EnterGameSuccess);
	MMOUI::RegisterEventHandler(this, GameEVent_SceneLoadFinish);
	//MMOUI::RegisterEventHandler(this, GameEvent_ExitMsg);

// #if defined(_DEBUG) && 1
// 	FString strTest = TEXT("\t\t\t\nIndex	PackageFolder	ID	Name	Type	SubType	RequireLevel	RequireClass	RequireGender	MaxEquip	MaxOwn	Color	BindType	DurType	Dur	SpellID	SpellLevel	SelfCoolDown	GroupCoolDown	SubSubType	Price	UsedStatus	Drop	Trade	Storage	Script	DisplayIcon	Description	MaxStackNum	DropMesh	SoundMaterialType	SoundActionType	ClothNoiseType	ItemLevel	CanSell	DisplayLargeIcon	DisplayLargeIconFemale	LimitDuration	LimitCount	Log	MaxRefineLevel	WeaponPhyAtkLo	WeaponPhyAtkHi	WeaponMagAtkLo	WeaponMagAtkHi	AtkRange	PhyDef	MagDef	MaxHp	MaxMp	BlockRate	ParryRate	BlockValue	CritDamageModificationRate	pow	FoundationRandomAttribute	FixedRandomAttribute1	FixedRandomAttribute2	FixedRandomAttribute3	FixedRandomAttribute4	BasePow	Identified	WeaponRuneID1	WeaponRuneID2	WeaponRuneID3	WeaponRuneID4	WeaponRuneID5	WeaponRuneID6	EquipAppearenceTemplate	SocketCount\n0		0		0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0									0	0			0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	1	0	0	0	0	1	0	0	0	0	0	0	0		\n");
// 	TArray< TArray<FString> > testArrayString2D;
// 	SplitStringToArray2D( strTest, true, false, testArrayString2D );
// #endif
}
//------------------------------------------------------------------------
MMOFacade::~MMOFacade(void)
{
	MMOUI::UnRegisterEventHandler(this, GameEVent_SceneLoadFinish);
	MMOUI::UnRegisterEventHandler(this, GameEvent_EnterGameSuccess);
	MMOUI::UnRegisterEventHandler(this, GameEvent_RequestMainPlayerData);
	//MMOUI::UnRegisterEventHandler(this, GameEvent_ExitMsg);
}
//------------------------------------------------------------------------
extern void InitDisplayDetailOptionList();
extern bool g_enableGridLoad;

void DumpHardwareInfo()
{
	// dump hard device info
	C2G_CLIENT_HARDWARE_INFO msg;
	DeviceInfoHelper helper;
	helper.GetHardwareInfo( msg, TRUE);
}

void MMOFacade::Init()
{
	if (/*GIsEditor ||*/ GIsMMOBenchmarking)
	{
		return;
	}

	if(M_MMO_HD)
	{
		g_enableGridLoad = false;
	}
	
#if 0
	FString enumVal = GetErrorCodeEnumString(-1);
	enumVal = GetErrorCodeEnumString((INT)errMiscEnd);
#endif



#if USE_MALLOC_PROFILER
	debugf( TEXT("======== [Facade::Init] USE_MALLOC_PROFILER ========") );
#elif TRACK_MEM_USING_TAGS
	debugf( TEXT("======== [Facade::Init] TRACK_MEM_USING_TAGS ========") );
#else
	debugf( TEXT("======== [Facade::Init] ========") );
#endif
	SCOPE_LOG_TIME(___INIT__, TEXT("Facade::Init"), 0);

	M_SCOPE_MEM_PROF(__FACADE__, TEXT("Facade::Init"));
	
	MALLOC_PROFILER_GAME(GEngine->ExecMemFunctions(TEXT("MEM"));)

	InitDisplayDetailOptionList();

// 	FString cfgDir = appGameConfigDir();
// 	cfgDir = cfgDir.RightChop(1);
// 	BOOL nRetCode = g_InitReadDataScript(TCHAR_TO_UTF8(*cfgDir), READ_DATA_SCRIPT_NAME);
// 	if(!nRetCode)
// 	{
// 		warnf(NAME_Error, TEXT("加载读取配置文件LUA失败！"));		
// 	}

	warnf(NAME_Log, TEXT("Init UIManager"));
	if(MMOUI::GetUIManager())
	{
		appSetSplashText(SplashTextType::StartupProgress, TEXT("初始化UI管理器..."));
		MMOUI::GetUIManager()->InitUIManager();
	}
	
	warnf(NAME_Log, TEXT("Init LoadStringMap"));

	appSetSplashText(SplashTextType::StartupProgress, TEXT("加载本地化字符串表..."));
	//put it early
	LoadStringMap();

	appSetSplashText(SplashTextType::StartupProgress, TEXT("初始化好友聊天管理..."));
	warnf(NAME_Log, TEXT("Init FriendChatManager"));
	FriendChatManager::getSingleton().Init();

	//new Network();
	//Network::getSingleton().Init();
	appSetSplashText(SplashTextType::StartupProgress, TEXT("初始化网络协议模块..."));
	warnf(NAME_Log, TEXT("Init MessageHandler"));
	CMessageHandler::GetInstance().Init();

	// TEST CODE:
	//GWorld->GetWorldInfo()->IsPlayInEditor()
	//debugf( TEXT("[Facade::Init] IP:%s Port:%d"),
	//	*AMMOGameFacade::GetSingleton().LoginServerIP, AMMOGameFacade::GetSingleton().LoginServerPort);

	//Network::getSingleton().ConnectServer(
	//	*(MainController->LoginServerIP), MainController->LoginServerPort);

	appSetSplashText(SplashTextType::StartupProgress, TEXT("初始化游戏配置数据..."));
	//
	// Load data templates
	//
// 	warnf(NAME_Log, TEXT("_LoadAffixTemplates"));
// 	_LoadAffixTemplates();

	//LoadAbilityTemplates();
	warnf(NAME_Log, TEXT("_LoadMissileTemplates"));
	_LoadMissileTemplates();


	warnf(NAME_Log, TEXT("_LoadBuffTemplates"));
	_LoadBuffTemplates();

	warnf(NAME_Log, TEXT("ReadRandomAttribute"));
    ReadRandomAttribute();

	warnf(NAME_Log, TEXT("MMOSkillManager::getSingleton().Init()"));
	MMOSkillManager::getSingleton().Init();
	warnf(NAME_Log, TEXT("ReadWeaponRuneSkillData()"));
	//读入铭文配置(需要先先于_LoadItemTemplates)
	MMOEquipManager::getSingleton().ReadWeaponRuneSkillData();
	// -----------------------------------
	// oscar@snda ---- 保留这段的顺序不变[
	warnf(NAME_Log, TEXT("_LoadItemTemplates"));
	_LoadItemTemplates();

	// 这三个读取顺序有依赖，不能修改
	warnf(NAME_Log, TEXT("DoodadManager::getSingleton().Init()"));
 	DoodadManager::getSingleton().Init();	

	warnf(NAME_Log, TEXT("drop_group.txt"));
	MMODropGroupDataManager::getSingleton().InitFromFile(TEXT("drop_group.txt"), true);

#if !SHIPPING_PC_GAME
	warnf(NAME_Log, TEXT("doodad_drop_data.txt"));
	MMODoodadDropDataManager::getSingleton().InitFromFile(TEXT("doodad_drop_data.txt"), true);
#endif
	
	warnf(NAME_Log, TEXT("_LoadNPCTemplates"));
	MMOQuestData::getSingleton().ClearMonsterGroupData();
	_LoadNPCTemplates();

	warnf(NAME_Log, TEXT("MMOQuestManager::getSingleton().Init()"));
	MMOQuestManager::getSingleton().Init();

	// oscar@snda ---- 保留这段的顺序不变]
	// -----------------------------------

	warnf(NAME_Log, TEXT("TeamManager::getSingleton().Init()"));
	TeamManager::getSingleton().Init();	

	warnf(NAME_Log, TEXT("MMOBattleManager::getSingleton().Init()"));
	MMOBattleManager::getSingleton().Init();

	warnf(NAME_Log, TEXT("MMOArenaManager::getSingleton().Init()"));
	MMOArenaManager::getSingleton().Init();

	//warnf(NAME_Log, TEXT("MMOScreenMsgMgr::getSingleton().Init()"));
	//MMOScreenMsgMgr::getSingleton().Init();
	warnf(NAME_Log, TEXT("ScreenMessageQueue::getSingleton().Init()"));
	ScreenMessageQueue::getSingleton().Init();

	warnf(NAME_Log, TEXT("MMOEquipManager::getSingleton().Init()"));
	//获取物品强化信息(需要先有物品信息_LoadItemTemplates)
	MMOEquipManager::getSingleton().Initialize();

	warnf(NAME_Log, TEXT("MMOMapDataManager::getSingleton().Init()"));
	MMOMapDataManager::getSingleton().Initialize();

	warnf(NAME_Log, TEXT("MMOVipManager::getSingleton().Init()"));
	MMOVipManager::getSingleton().Initialize();

	warnf(NAME_Log, TEXT("MMODailySignInManager::getSingleton().Init()"));
	MMODailySignInManager::getSingleton().Initialize();

	warnf(NAME_Log, TEXT("MMOShopManager::getSingleton().Init()"));
	MMOShopManager::getSingleton().Init();

	warnf(NAME_Log, TEXT("MallManager::getSingleton().Init()"));
	MallManager::getSingleton().Init();

	////Test code
	//INT teleportNodeCnt;
	//for( INT i = 0; i < EWR_MAX; ++i )
	//{
	//	TArray<FTeleportNodeObj*> nodesListInRegion;
	//	teleportNodeCnt = MMOMapDataManager::getSingleton().GetTeleportNetNodesIn( (EWorldRegion)i, 0, TRUE, &nodesListInRegion );

	//	FTeleportNodeObj *pNode;
	//	INT lastSceneID = -1;
	//	INT itemObjIdx = 0;
	//	for ( INT i = 0; i < teleportNodeCnt; ++i ){
	//		pNode = nodesListInRegion(i);
	//		if( lastSceneID != pNode->InSceneID ){
	//			//第一级表项：地图名
	//			lastSceneID = pNode->InSceneID;

	//			FString mapName;
	//			mapName = UGlobalMapInfo::GetSingleton()->FindMapInfo( lastSceneID ).MapDisplayName;

	//			warnf(NAME_Log, TEXT("[TeleportNetNodes Debug] ListItem%d -MapName:%s" ), itemObjIdx, *mapName );
	//			++itemObjIdx;
	//		}
	//		warnf(NAME_Log, TEXT("[TeleportNetNodes Debug] ListItem%d  -NodeName:%s Scene:%d InRegion:%d" ), itemObjIdx, *pNode->DisplayName, pNode->InSceneID, pNode->Region );
	//		++itemObjIdx;
	//	}	
	//}
	////end Test code

	//InitNPCMapPositionInfo();
	//warnf(NAME_Log, TEXT("UGlobalMapInfo::getSingleton().LoadNPCInfoFromXMLFiles()"));
	//UGlobalMapInfo::GetSingleton()->LoadNPCInfoFromXMLFiles();


	warnf(NAME_Log, TEXT("IniMMOAppName"));
	IniMMOAppName();

	{
		SCOPE_LOG_TIME(___INIT__, TEXT("MMOFacade::Init::ReadLevelConfig"), 0);
		warnf(NAME_Log, TEXT("ReadLevelConfig"));
		ReadCensoredWordList (CensoredWord_All);
		ReadCensoredWordList (CensoredWord_Chat);
		ReadCensoredWordList (CensoredWord_ChatRecive);
		ReadPowerConfig();
		ReadAttributeConfig();
		ReadAttribute2Config();
		ReadForceRelative();
		ReadTitle();
		ReadTweenData();
		//ReadTestDropTrace();
		ReadMountData();
		ReadReputationTemp();
		ReadReputation();
		ReadHint();
		ReadPkLevel();
	}

	warnf(NAME_Log, TEXT("MMOFreeTipsUIManager::getSingleton().ReadFreeTipsConfig()"));
	MMOFreeTipsUIManager::getSingleton().ReadFreeTipsConfig();

	warnf(NAME_Log, TEXT("MMOAchievementManager::getSingleton().Init()"));
	MMOAchievementManager::getSingleton().Init();

	warnf(NAME_Log, TEXT("MMOGuildManager::getSingleton().Init()"));
	MMOGuildManager::getSingleton().Init();

	warnf(NAME_Log, TEXT("MMOGuideManager::getSingleton().Init()"));
	MMOGuideManager::getSingleton().Init();

	warnf(NAME_Log, TEXT("FriendChatManager::getSingleton().Init()"));
	FriendChatManager::getSingleton().Init();
		
	//属性表读取后初始化
	MMOAttributeAccountManager::getSingleton().Init();

	warnf(NAME_Log, TEXT("InitClientActionInformationNotify"));
    InitClientActionInformationNotify();
	
	warnf(NAME_Log, TEXT("InitClientSceneVariableActionList"));
	InitClientSceneVariableActionList();

	warnf(NAME_Log, TEXT("ShopManager::getSingleton().Init()"));
	ShopManager::getSingleton().Init();

	warnf(NAME_Log, TEXT("MMOWeatherManager::getSingleton().Init()"));
	MMOWeatherManager::getSingleton().Init();

	warnf(NAME_Log, TEXT("MMOHeavenManager::getSingleton().Init()"));
	MMOHeavenManager::getSingleton().Init();

	warnf(NAME_Log, TEXT("ActivityManager::getSingleton().Init()"));
	ActivityManager::getSingleton().Init();

	warnf(NAME_Log, TEXT("MailManager::getSingleton().Init()"));
	MailManager::getSingleton().Init();

	warnf(NAME_Log, TEXT("MMOAuctionManager::getSingleton().Init()"));
	MMOAuctionManager::getSingleton().Init();

	warnf(NAME_Log, TEXT("MMOHurtManager::getSingleton().Init()"));
	MMOHurtManager::getSingleton().Init();
	warnf(NAME_Log, TEXT("MMOBossBloodManager::getSingleton().Init()"));
	MMOBossBloodManager::getSingleton().Init();

	warnf(NAME_Log, TEXT("LoadMapTipsManager::getSingleton().Init()"));
	LoadMapTipsManager::getSingleton().Init();

	warnf(NAME_Log, TEXT("MMOGoldStoreManager::getSingleton().Init()"));
	MMOGoldStoreManager::getSingleton().Init();

	warnf(NAME_Log, TEXT("MMOGuideOptionManager::getSingleton().Init()"));
	MMOGuideOptionManager::getSingleton().Init();

	warnf(NAME_Log, TEXT("MasterStudentManager::getSingleton().Init()"));
	MasterStudentManager::getSingleton().Init();

	MMOHelperManager::getSingleton().Init();

	MMOSystemActivateManager::getSingleton().Init();

	MMOHandbookManager::getSingleton().Init();
	MMOLoginManager::getSingleton().Init();

	MMOJourneyManager::getSingleton().Init();

	MMODeathManager::getSingleton().Init();

	MMOAssistManager::getSingleton().Init();

	UStallManager::getSingleton().Init();

	MMOEquipRemakeManager::getSingleton().Init();

	MMOJigsawManager::getSingleton().Init();

	InGameStateFps = 0.0f;
	InGameCalTimes = 0;

	appSetSplashText(SplashTextType::StartupProgress, TEXT("初始化游戏完成！"));

	warnf(NAME_Log, TEXT("MMOFacade::Init End!"));
	MALLOC_PROFILER_GAME(GEngine->ExecMemFunctions(TEXT("MEM")););

	bFinger = FALSE;
	bWrist = FALSE;
	bFingerShowType = FALSE;

	LastTimeCheckPlayerShowOrNot = 0;
	playerSortType = IST_Invalid;
}
//------------------------------------------------------------------------
void MMOFacade::SetCurVolumeAndSaveAudioConfig()
{
	UMMOUIManager* uimgr = MMOUI::GetUIManager();
	if( uimgr == NULL ){
		return;
	}

	UAudioDevice* adioDv = GEngine->GetAudioDevice();
	if( adioDv == NULL ){
		// oscar@snda TODO：查为什么没有创建出来
		return;
	}
	adioDv->VolumeTotal_op		= uimgr->bSoundAll;
	adioDv->VolumeBackMusic_op	= uimgr->bSoundBack;
	adioDv->VolumeEnv_op		= uimgr->bSoundEnv;	
	adioDv->VolumeVoice_op		= uimgr->bSoundVoice;
	adioDv->VolumeSfx_op		= uimgr->bSoundSfx;
	GIsVolumeInBackground		= uimgr->bSoundFocusOut;
	adioDv->VolumeTotal			= (float)uimgr->nSoundAllVL		/ 100.0f;
	adioDv->VolumeBackMusic		= (float)uimgr->nSoundBackVL	/ 100.0f;
	adioDv->VolumeEnv			= (float)uimgr->nSoundEnvVL		/ 100.0f;
	adioDv->VolumeVoice			= (float)uimgr->nSoundVoiceVL	/ 100.0f;
	adioDv->VolumeSfx			= (float)uimgr->nSoundSfxVL		/ 100.0f;

	GConfig->SetBool( TEXT("XAudio2.XAudio2Device"), TEXT("VolumeTotal_op"), adioDv->VolumeTotal_op, GEngineIni );
	GConfig->SetBool( TEXT("XAudio2.XAudio2Device"), TEXT("VolumeBackMusic_op"), adioDv->VolumeBackMusic_op, GEngineIni );
	//GConfig->SetBool( TEXT("ALAudio.ALAudioDevice"), TEXT("VolumeEnv_op"), adioDv->VolumeEnv_op, GEngineIni );
	GConfig->SetBool( TEXT("XAudio2.XAudio2Device"), TEXT("VolumeVoice_op"), adioDv->VolumeVoice_op, GEngineIni );
	GConfig->SetBool( TEXT("XAudio2.XAudio2Device"), TEXT("VolumeSfx_op"), adioDv->VolumeSfx_op, GEngineIni );
	GConfig->SetBool( TEXT("XAudio2.XAudio2Device"), TEXT("VolumeFocus_op"), GIsVolumeInBackground, GEngineIni );

	GConfig->SetFloat( TEXT("XAudio2.XAudio2Device"), TEXT("VolumeTotal"), adioDv->VolumeTotal, GEngineIni );
	GConfig->SetFloat( TEXT("XAudio2.XAudio2Device"), TEXT("VolumeBackMusic"), adioDv->VolumeBackMusic, GEngineIni );
	//GConfig->SetFloat( TEXT("ALAudio.ALAudioDevice"), TEXT("VolumeEnv"), adioDv->VolumeEnv, GEngineIni );
	GConfig->SetFloat( TEXT("XAudio2.XAudio2Device"), TEXT("VolumeVoice"), adioDv->VolumeVoice, GEngineIni );
	GConfig->SetFloat( TEXT("XAudio2.XAudio2Device"), TEXT("VolumeSfx"), adioDv->VolumeSfx, GEngineIni );
	adioDv->SaveConfig();
}

extern BYTE g_loginServerGroup;
extern FString g_loginServerName;

void MMOFacade::IniMMOAppName()
{
#if _WIN64
	const FString PlatformBitsString( TEXT( "64" ) );
#else
	const FString PlatformBitsString( TEXT( "32" ) );
#endif

#ifdef _DEBUG
	const FString BuildConfig(TEXT("Debug"));
#else
	const FString BuildConfig(TEXT(""));
#endif

	const FString GameName = GConfig->GetStr(TEXT("URL"), TEXT("GameName"), GEngineIni);
	const FString RHIName = ShaderPlatformToText( GRHIShaderPlatform, TRUE, TRUE );
	FString GameVer = FString::Printf(TEXT("%s - %d"), MMO_GAME_VER_DISPLAY, BUILD_NUMBER ); // oscar@snda
#if SHIPPING_PC_GAME
	FString appName = FString::Printf( TEXT( "%s [%s] [%s] %s %s" ),
		*GameName, *GameVer, *g_loginServerName, (GSystemSettings.bUseVSync ? TEXT("VSYNC") : TEXT("")), (M_MMO_HD ? TEXT("HD") : TEXT("Lite"))); 
#else
	FString appName = FString::Printf( TEXT( "%s [%s] (%s)[%s] [%s] [%d] Group %s%s" ),
		*GameName, *GameVer, *RHIName, *BuildConfig, *g_loginServerName, g_loginServerGroup,
		(GSystemSettings.bUseVSync ? TEXT("VSYNC - ") : TEXT("")), (M_MMO_HD ? TEXT("HD") : TEXT("Lite"))) ; 
#endif

	mMMOAppName = appName ;

	//::SetWindowText(GGameWindow, *mMMOAppName );	
}

FString MMOFacade::GetMMOAppName()
{
	IniMMOAppName();
	return mMMOAppName;
}

void MMOFacade::SetDoorStates( std::bitset<64> tState )
{
	mDoorStates = tState;
}

void MMOFacade::UpdateDoors()
{
	//遍历场景内的Door
    TActorInstanceIterator< AMMOServerToggleDoorActor > it;
    while ( *it )
    {
        if( (*it) == NULL)
            continue;
        if((*it)->DoorId < 64 && (*it)->DoorId >= 0 )
        {
            if( mDoorStates.test( (*it)->DoorId ) )
                (*it)->SetState(ETAS_Closed);
            else
                (*it)->SetState(ETAS_Opened);
        }
        ++it;
    }
	/*
	for (INT i = 0; i < GWorld->Levels.Num(); ++i)
	{
	ULevel* level = GWorld->Levels(i);
	for (INT j = 0; j < level->Actors.Num(); ++j)
	{
	AActor* actor = level->Actors(j);
	if(actor)
	{				
	Iterator
	if(actor->IsA(AMMOServerToggleDoorActor::StaticClass()))
	{
	AMMOServerToggleDoorActor *pDoor = Cast<AMMOServerToggleDoorActor>(actor);
	if(pDoor->DoorId < 64 && pDoor->DoorId > 0 )
	{
	if( doorstate.test( pDoor->DoorId ) )
	pDoor->eventSetState(ETAS_Opened);
	else
	pDoor->eventSetState(ETAS_Closed);
	}
	}
	}
	}
	}*/

}

void _LoadMMOAppName()
{
// #if _WIN64
// 	const FString PlatformBitsString( TEXT( "64" ) );
// #else
// 	const FString PlatformBitsString( TEXT( "32" ) );
// #endif
	const FString GameName = GConfig->GetStr(TEXT("URL"), TEXT("GameName"), GEngineIni);
	const FString RHIName = ShaderPlatformToText( GRHIShaderPlatform, TRUE, TRUE );
	FString GameVer = FString::Printf(TEXT("%s - %d %s"), MMO_GAME_VER_DISPLAY, BUILD_NUMBER, (M_MMO_HD ? TEXT("HD") : TEXT("Lite")) ); // oscar@snda

	//const FString Facade::MMOAppName = FString::Printf( TEXT( "%s [%s] (%s-bit, %s)" ), *GameName, *ChangeListID, *PlatformBitsString, *RHIName ); 
	//Or 可以这样
	/*
	TCHAR nowAppName[256];
	::GetWindowText(GGameWindow,nowAppName,256);
	MMOAppName = (const TCHAR*)nowAppName;
	*/
}
AbilityProcessor* MMOFacade::GetAbilityProcessor()
{
	if (!AbilityProcessorPtr)
	{
		AbilityProcessorPtr = new AbilityProcessor();
	}
	return AbilityProcessorPtr;
}

MMOBuffManager* MMOFacade::GetMMOBuffManager()
{
	if (!MMOBuffManagerPtr)
	{
		MMOBuffManagerPtr = new MMOBuffManager();
	}
	return MMOBuffManagerPtr;
}

ReputationTemp*  MMOFacade::GetReputationTemp(INT index)
{ 
	if(index<_ReputationTempArray.Num()){
		return &_ReputationTempArray(index);
	}
	return NULL;
}

ReputationTemp*  MMOFacade::GetReputationTempBySceneID(INT SceneID)
{ 
	for(INT i=0;i<_ReputationTempArray.Num();i++)
	{
		if(_ReputationTempArray(i).SceneID ==  SceneID)
		{
			return &_ReputationTempArray(i);
		}
	}
	return NULL;
}

ReputationTemp*  MMOFacade::GetReputationTempByForceID(INT ForceID)
{ 
	for(INT i=0;i<_ReputationTempArray.Num();i++)
	{
		if(_ReputationTempArray(i).ForceID ==  ForceID)
		{
			return &_ReputationTempArray(i);
		}
	}
	return NULL;
}

INT	MMOFacade::GetCanAttackWithFreePkMode(INT ForceID)
{
	if(ForceID>=0&&ForceID<MAX_NPC_FORCE_ID)
	{
		return _CanAttackWithFreePkModeArray[ForceID];
	}
	return 0;
}

//MMOBattleManager* Facade::GetBattleManager()
//{
//	if (!BattleManager)
//	{
//		BattleManager = MMOBattleManager::getSingletonPtr();
//		//BattleManager = &MMOBattleManager::getSingleton();
//	}
//	return BattleManager;
//}

void MMOFacade::Shutdown()
{
	//if (GIsEditor)
	//{
	//	return;
	//}

	debugf( TEXT("======== [Facade::Shutdown] ========") );

	M_ReportError();

	MMOFacade::getSingleton().NotifyShutDown();

	CMessageHandler::GetInstance().UnInit();

	for(UILayoutList::iterator i = g_layoutList.begin(); i != g_layoutList.end(); ++i)
	{
		delete *i;
	}

	
	////
	//if ( BattleManager )
	//{
	//	delete BattleManager;
	//}
	//
	//if (MainControllerAI)
	//{
	//	delete MainControllerAI;
	//}

	ClearAllCharacter();

	//Network::getSingleton().ShutDown();


	ClearItems();
	ClearPlayerEquip();

	ClearSoldItems();

	UninitClientActionInformationNotify();

	UStallManager::getSingleton().Shutdown();
	MMOAssistManager::getSingleton().Shutdown();
	MMOJourneyManager::getSingleton().Shutdown();
	MMOHandbookManager::getSingleton().Shutdown();
	MMOSystemActivateManager::getSingleton().Shutdown();
	MMOHelperManager::getSingleton().Shutdown();
	MMOQuestManager::getSingleton().Shutdown();
	MMOGuildManager::getSingleton().Shutdown();
	MMOGuideManager::getSingleton().Shutdown();
	MMOAchievementManager::getSingleton().Shutdown();
	MailManager::getSingleton().Shutdown();

	// global map info
	UGlobalMapInfo::GetSingleton()->RemoveFromRoot();

	// Battle Template
	MMOBattleManager::getSingleton().Shutdown();
	// Arena Template
	MMOArenaManager::getSingleton().Shutdown();
	//equip
	MMOEquipManager::getSingleton().Shutdown();
	//skill
	MMOSkillManager::getSingleton().Shutdown();
	// Navigation
	MMONavigationManager::getSingleton().Shutdown();
	// vip
	MMOVipManager::getSingleton().Shutdown();
	//DailySignIn
	MMODailySignInManager::getSingleton().Shutdown();
	//
	MMOMapDataManager::getSingleton().Shutdown();
	//
	//MMOScreenMsgMgr::getSingleton().Shutdown();
	ScreenMessageQueue::getSingleton().Shutdown();

	MMOWeatherManager::getSingleton().Shutdown();
	MMOHeavenManager::getSingleton().Shutdown();
	ActivityManager::getSingleton().Shutdown();
	MallManager::getSingleton().Shutdown();
	MMOAuctionManager::getSingleton().Shutdown();
	LoadMapTipsManager::getSingleton().Shutdown();
	MasterStudentManager::getSingleton().Shutdown();
	WebHtmlManager::getSingleton().Shutdown();

	MMODeathManager::getSingleton().Shutdown();

	MMOEquipDataManager::getSingleton().Clear();
	MMOItemDataManager::getSingleton().Clear();

	// 与初始化反向
	MMODoodadDropDataManager::getSingleton().Clear();
	MMODropGroupDataManager::getSingleton().Clear();
	DoodadManager::getSingleton().Clear();
	MMOHeavenLevelDataManager::getSingleton().Clear();
	MMOHeavenGasItemDataManager::getSingleton().Clear();

	MMOEquipRemakeManager::getSingleton().Shutdown();

	MMOJigsawManager::getSingleton().Shutdown();
	FriendChatManager::getSingleton().Shutdown();

	if (AbilityProcessorPtr)
	{
		delete AbilityProcessorPtr;
	}

	if (MMOBuffManagerPtr)
	{
		delete MMOBuffManagerPtr;
	}

// 	g_UnInitReadDataScript();
	//delete this;
}

void MMOFacade::UpdateTimeStamp()
{
	if (GetMainController())
	{
		GetMainController()->SyncPosition();
	}
}

LONG64 MMOFacade::GetCurrentTimeStamp()
{
	//double st = GameStartTime;
	//double lt = LastUpdateTime;
	//debugf(L"LastUpdateTime=%f, GameStartTime=%f", lt, st);
	return LONG64(GCurrentTime * 1000) - GameStartTime;
	//return LastUpdateTime - GameStartTime;
}

LONG64 MMOFacade::GetGameStartTime()
{
	return GameStartTime;
}


LONG64 MMOFacade::GetCurrentTime()
{
	return LONG64(GCurrentTime * 1000);
}

LONG64 MMOFacade::GetLastUpdateTime()
{
	return LastUpdateTime;
}

void MMOFacade::SetLastUpdateTime(LONG64& t)
{
	LastUpdateTime = t;
}

void MMOFacade::SetGameStartTime()
{
	GameStartTime = LONG64(GCurrentTime * 1000);
	LastUpdateTime = GameStartTime;
}

FString MMOFacade::TimeDisplay(INT seconds,INT precision)
{
	FString res = TEXT("");
	if(seconds<0)
	{
		return res;
	}
	
	DWORD	level[6] = {60*60*24*365,60*60*24*30,60*60*24,60*60,60,1}; 
	FString name[6] = {TEXT("MMOGame.Time.Time1"),TEXT("MMOGame.Time.Time2"),TEXT("MMOGame.Time.Time3"),TEXT("MMOGame.Time.Time4"),TEXT("MMOGame.Time.Time5"),TEXT("MMOGame.Time.Time6")}; 
	DWORD	time[6] = {seconds/level[0],seconds%level[0]/level[1],seconds%level[1]/level[2],seconds%level[2]/level[3],seconds%level[3]/level[4],seconds%level[4]/level[5]}; 
	int i=0;
	for(;i<6;i++)
	{
		if(time[i]>0)break;
	}
	INT curPrecision = 0;
	for(;i<6;i++)
	{
		res += appItoa(time[i])+MMOUI::ParseLocalizedPropertyPath(name[i]);
		++curPrecision;
		if(curPrecision>=precision)break;
	}
	return res;
}

//------------------------------------------------------------------------
void TickEffectProcessor(FLOAT deltaTime)
{
	CTRL_TICK("MMOTickEffectProcessor");

	M_FORCE_TRY_BEGIN;
	EffectProcessor::GetSingleton().Tick(deltaTime);
	M_FORCE_TRY_END("Protected for TickEffectProcessor");
}

void MMOFacade::TickLogic( FLOAT deltaTime )
{
	CTRL_TICK("MMOFacade::TickLogic");

	g_TotalFrameNum++;

	if (/*GIsEditor ||*/ GIsMMOBenchmarking)
	{
		return;
	}

	if (!GIsEditor)
	{
		MMOPerformanceBalancer::getSingleton().Tick(deltaTime);	// zhaojun@snda

		TickPlayerInfoRequestTime( deltaTime );

		MMOFacade::getSingleton().UpdateScreenPixelFromWorld();

		if (TRUE)
		{
			SCOPE_CYCLE_COUNTER(STAT_MMONetProcess);
			CMessageHandler::GetInstance().Tick(deltaTime);
		}


		UpdateObjectsToSync(deltaTime);

		UpdatePlayerShowCount(deltaTime);
	}

	
	if (bLevelsLoaded == FALSE && MMO::GetCurrentGameState() == EGameState_InGame)
	{		
		if (GWorld->bLevelsNotReady == FALSE)
		{
			bLevelsLoaded = TRUE;
		}
	}

	if (TRUE)
	{
		SCOPE_CYCLE_COUNTER(STAT_AbilitySystemTime);

		gServerGridWorld.SetNetLatency(GetGameWorldPingDelayTime());	//added by zhaojun@snda

		if (AbilityProcessorPtr)
		{
			AbilityProcessorPtr->Tick(deltaTime);
		}
		else
		{
			AbilityProcessorPtr = GetAbilityProcessor();
		}

		if (MMOBuffManagerPtr)
		{
			MMOBuffManagerPtr->TickBuffData(deltaTime);
		}

		//MMOArenaManager::getSingleton().TickArena(deltaTime);
		MMOCoolDownManager::getSingleton().TickCD(deltaTime);
		FriendManager::getSingleton().TickTime(deltaTime);

		TickEffectProcessor(deltaTime);

		EffectProcessor::GetSingleton().ClearCurrentEmitterNumber();
	}

	if (GIsEditor)
	{
		return;
	}

#if M_ENABLE_ASYNC_PROC_CHAR
	CMessageHandler::GetInstance().TickCharAppear(deltaTime);
#endif

	MMOFacade::getSingleton().TickTitleTime(deltaTime);

	MMODropManager::getSingleton().UpdateDropHudPos(deltaTime);

	MMOAchievementManager::getSingleton().Update(deltaTime);

	MMOGuideManager::getSingleton().Update(deltaTime);

	MMOQuestManager::getSingleton().Update(deltaTime);

	MMOGuildManager::getSingleton().Update(deltaTime);

	MMOWeatherManager::getSingleton().Update(deltaTime);

	MMONavigationManager::getSingleton().Update(deltaTime);

	MMOHeavenManager::getSingleton().Update(deltaTime);

	ScreenMessageQueue::getSingleton().Update(deltaTime);

	MallManager::getSingleton().TickUpdateInfo();

	MMOHurtManager::getSingleton().Update(deltaTime);

	MMOHelperManager::getSingleton().Update(deltaTime);

	MMOHandbookManager::getSingleton().Update(deltaTime);

	MMOAssistManager::getSingleton().Update(deltaTime);

	UStallManager::getSingleton().Update(deltaTime);

	MMOGoldStoreManager::getSingleton().Update(deltaTime);

	MMOJigsawManager::getSingleton().Update(deltaTime);

	//MMOFacade::getSingleton().ClearSceneView();

	if(_UpdateSortTimer)
	{	
		_SortTimer += deltaTime;
		if(_SortTimer > 1.0f)
		{
			if(_SortTimerCounter <= 0)
			{
				_UpdateSortTimer = false;
				_LockSortMode = LSM_SortNone;
				MMOGameEvent_SortTimerEnd evt;
				MMOUI::PostGameEvent(&evt);
			}
			else
			{
				_SortTimer -= 1.0f;

				MMOGameEvent_SortTimer evt(_SortTimerCounter);
				MMOUI::PostGameEvent(&evt);

				--_SortTimerCounter;
			}
		}
	}

    DoodadManager::getSingleton().Tick();

	ScreenMessageQueue::getSingleton().Update(deltaTime);
}

// 上面两个创建函数将会被这个函数替换掉,统一玩家与npc的创建流程
AMMOActor* MMOFacade::CreateCharacter(INT id, INT CharacterType, const FVector& initLoc, const FRotator& initRot)
{
	if (CharacterType == otRole || CharacterType == otHero)
	{
		if( CharacterType == otRole )
		{
			IncPlayerCount;
		}
		return CreatePlayer(id, initLoc, initRot);
	}
	else if (CharacterType == otNpc)
	{
		++NPCCount;
		return CreateNPC(id, initLoc, initRot);
	}

	return NULL;
}

AMMOPlayer* MMOFacade::CreatePlayer(INT Id, const FVector& initLoc, const FRotator& initRot)
{
	AActor* actor = GetCharacter(Id);
	if (actor)
	{
		debugf(L"Actor:%d is already exist!", Id);
		return Cast<AMMOPlayer>(actor);
	}

	actor = GetMainController()->eventSpawnCharacter(initLoc, initRot);

	if (actor)
	{
		AMMOPlayer* NewPlayer = Cast<AMMOPlayer>(actor);
		if (NewPlayer)
		{
			NewPlayer->SetServerLocation(initLoc);// zhaojun@snda //NewPlayer->ServerLocation = initLoc;
			//NewPlayer->PickPriority = EPP_OtherPlayer;
			NewPlayer->SetActorType( EACT_OtherPlayer );
			NewPlayer->setPhysics(PHYS_None);
			NewPlayer->eventSpawnMoveController();
			NewPlayer->ServerID = Id;
			NewPlayer->SetMainController(MainController);
			NewPlayer->eventSetPlayerRBChannels();
			NewPlayer->SetInFighting( FALSE );
			NewPlayer->Mesh->LightingChannels.Unnamed_5 = TRUE;
			CharacterMap.insert(make_pair(Id, NewPlayer->GetIndex()));
			//
			MMOMapDataManager::getSingleton().AddOrUpdateMapDymcActorNear( *NewPlayer );
			//
			PlayerMap.insert( make_pair(Id, NewPlayer->GetIndex()) );
		}
		
		NewPlayer->SetVisibleByKismet(MainController->bShowTheOtherPlayersByKismet);
		NewPlayer->SetVisibleByHotKey(MainController->bShowTheOtherPlayersByHotKey);

		return NewPlayer;
	}
	else
	{
		debugf(L"Create Player Failed! %d", Id);
		return NULL;
	}
}

bool MMOFacade::IsSelf(AMMOPlayer* player) const
{
	check(GetMainPlayer() != NULL);
	return GetMainPlayer()->ServerID == player->ServerID;
}

bool MMOFacade::IsSelf(INT roleID) const
{
	check(GetMainPlayer() != NULL);
	return GetMainPlayer()->ServerID == roleID;
}

UBOOL MMOFacade::IsMonsterOwnedByMe( AMMOActor *pActor ) const
{
	if( pActor == NULL ){
		return FALSE;
	}

	//UBOOL isPet = ( pActor->GetActorType() == EACT_Pet );
	//if( isPet == FALSE ){
	//	return FALSE;
	//}

	AMMOActor *pMainActor = GetMainPlayer();
	if( pMainActor == NULL ){
		return FALSE;
	}

	return ( pActor->MasterName == pMainActor->ActorName );
}

// 创建NPC,如果已存在,会返回已存在的对象
ANPC* MMOFacade::CreateNPC(INT Id, const FVector& initLoc, const FRotator& initRot)
{
	ANPC* NewActor = Cast<ANPC>(GetCharacter(Id));
	if (NewActor)
	{
		debugf(L"Actor:%d is already exist!", Id);
		return NewActor;
	}

	AMMOGame* GameInfo = Cast<AMMOGame>(MainController->WorldInfo->Game);
	if (GameInfo == NULL)
	{
		return NULL;
	}

	//ANPC * npc = GameInfo->eventSpawnDefaultMonster(initLoc, initRot);

	ANPC* npc = Cast<ANPC>(GWorld->SpawnActor(ANPC::StaticClass(), NAME_None, initLoc, initRot, NULL, TRUE));
	if (npc)
	{
		npc->SetServerLocation(initLoc);//zhaojun@snda //npc->ServerLocation = initLoc;
		npc->setPhysics(PHYS_Walking);
		npc->ServerID = Id;
		//npc->ActorType = npc->Template->ActorType;//还没有Template
		npc->SetMainController(MainController);
		CharacterMap.insert(make_pair(Id, npc->GetIndex()));
		////
		//EActorClassType actorType = (EActorClassType)npc->GetActorType();
		////
		//if( actorType == EACT_Monster || actorType == EACT_Boss){
		//	MMOMapDataManager::getSingleton().AddOrUpdateMapDymcActor( Id );
		//}
		//
		//if( !npc->m_bIsMonster)
		//	FriendlyNpcMap.insert(make_pair(Id, npc->GetIndex()));
		//npc->eventStartMeshFade( true, 2.5f );
	}
	else
	{
		warnf(NAME_Warning, L"Create npc failed: %d", Id);
		return NULL;
	}

	//npc->SetHidden(g_hideAllNPC);
	npc->SetVisibleByKismet(MainController->bShowNpc);

	return npc;
}

AMMOEmitter* MMOFacade::CreateEmitter(INT Id, INT EmitterTemplateId)
{
	AMMOEmitter* emit = GetEmitter(Id);
	if (emit)
	{
		return emit;
	}

	UMissile* missile = FindMissileTemplate(EmitterTemplateId);
	if (missile == NULL)
	{
		M_AddDataError(TEXT("spell.txt"), FString::Printf(
			TEXT("无法找到子弹模板 ID(%d)"), EmitterTemplateId));
		return NULL;
	}

	//if (missile->ParticleSystemTemplate == NULL)
	//{
	//	return NULL;
	//}

	AActor* actor = GWorld->SpawnActor(AMMOEmitter::StaticClass(), NAME_None);
	if(actor == NULL)
	{
		warnf(NAME_Error, TEXT("Can not Spawn an AMMOEmitter by Id(%d)"), EmitterTemplateId);
		return NULL;
	}

	emit = Cast<AMMOEmitter>(actor);
	if (emit)
	{
		// create sound cue component
		if (missile->FlySoundCue && emit->Components.Num() > 0)
		{
			FSceneInterface* CS = NULL;
			for (INT i = 0; i < emit->Components.Num(); ++i)
			{
				if (emit->Components(i))
				{
					CS = emit->Components(i)->GetScene();
					break;
				}
			}
			if (CS)
			{
				UAudioComponent* AudioComponent = UAudioDevice::CreateComponent( missile->FlySoundCue, CS, emit, 0, TRUE);
				if( AudioComponent )
				{
					AudioComponent->Location = actor->Location;
					AudioComponent->bUseOwnerLocation = TRUE;
					AudioComponent->bStopWhenOwnerDestroyed = TRUE;
					//AudioComponent->VolumeMultiplier		= 1.0;
					//AudioComponent->PitchMultiplier			= PitchMultiplier;
					AudioComponent->bAllowSpatialization	= TRUE;
					AudioComponent->bIsUISound				= FALSE;
					AudioComponent->bAutoDestroy			= 1;
					AudioComponent->SubtitlePriority		= 10000;
					AudioComponent->Play();
				}
			}

		}

		emit->MissileData = missile;
		emit->Speed = 128.0*1000.0/missile->Speed;
		emit->ServerID = Id;
		emit->SetDrawScale(missile->Scale);
		emit->DelayToKill = missile->DelayTime;
		missile->eventSetDataToProjectile(emit);
		CharacterMap.insert(make_pair(Id, emit->GetIndex()));

		//debugf(L"Add Missle id=%d", Id);
	}
	return emit;
}


//void MMOFacade::UpdatePlayerShowCount(FLOAT delta)
//{
//	DOUBLE NowTime = appSeconds();
//	if (NowTime - LastTimeCheckPlayerShowOrNot > 2.0)
//	{
//		LastTimeCheckPlayerShowOrNot = NowTime;
//	}
//	else
//	{
//		return;
//	}
//
//
//	struct ShowNode
//	{
//		FLOAT Distance;
//		UBOOL bVisible;
//		AMMOPlayer* Player;
//
//		ShowNode(){ Player = NULL;}
//	};
//
//	struct CompareNode
//	{
//		bool operator()(const ShowNode& A, const ShowNode& B)
//		{
//			if (A.Distance < B.Distance)
//			{
//				return true;
//			}
//			return false;
//		}
//	};
//
//	std::list<ShowNode> PlayerList;
//
//	AMMOPlayer* MainPlayer = GetMainPlayer();
//	if (MainPlayer == NULL)
//	{
//		return;
//	}
//
//	AMMOPlayerController* MainController = GetMainController();
//	if (MainController == NULL)
//	{
//		return;
//	}
//
//	for (STLInt2IntMap::iterator i = PlayerMap.begin(); i != PlayerMap.end(); ++i)
//	{
//		AMMOPlayer* player = Cast<AMMOPlayer>(GetCharacter(i->first));
//		if (!player)
//		{
//			warnf(TEXT("%d is in PlayerMap, but not in CharacterMap"), i->first);
//			continue;
//		}
//
//		if (player == GetMainPlayer())
//		{
//			continue;
//		}
//
//		ShowNode NewNode;
//		NewNode.Distance = (player->Location - MainPlayer->Location).SizeSquared();
//		NewNode.Player = player;
//
//
//		if (player->bSendRequestToServerToGetFullInfo)
//		{
//			if (player->bVisibleByCharacterCountLimit)
//			{
//				NewNode.bVisible = TRUE;
//				if (player->bRelevant)
//				{
//					continue;
//				}
//			}
//			else
//			{
//				NewNode.bVisible = FALSE;
//
//				if (player->bRelevant)
//				{
//					player->SetVisibleByCharacterCountLimit(TRUE);
//					continue;
//				}
//			}
//		}
//		else
//		{
//			NewNode.bVisible = FALSE;
//
//			if (player->bRelevant)
//			{
//				CMessageHandler::GetInstance().TrySyncRoleData(player->ServerID);
//				player->bSendRequestToServerToGetFullInfo = TRUE;
//				continue;
//			}
//		}
//
//		PlayerList.push_back(NewNode);
//	}
//
//	PlayerList.sort(CompareNode());
//
//
//	INT PlayerLimitCount = GSystemSettings.MaxShowPlayerCount;
//
//
//	for (std::list<ShowNode>::iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
//	{
//		if (PlayerLimitCount > 0)
//		{
//			if (i->bVisible == FALSE)
//			{
//				if (i->Player->bSendRequestToServerToGetFullInfo)
//				{
//					if (i->Player->bVisibleByCharacterCountLimit == FALSE)
//					{
//						i->Player->SetVisibleByCharacterCountLimit(TRUE);
//					}
//				}
//				else
//				{
//					CMessageHandler::GetInstance().TrySyncRoleData(i->Player->ServerID);
//					i->Player->bSendRequestToServerToGetFullInfo = TRUE;
//				}
//			}
//
//			PlayerLimitCount--;
//		}
//		else
//		{
//			if (i->bVisible)
//			{
//				i->Player->SetVisibleByCharacterCountLimit(FALSE);
//			}
//		}
//
//	}
//}



void MMOFacade::UpdatePlayerShowCount(FLOAT delta)
{
	AMMOPlayer* MainPlayer = GetMainPlayer();
	if (MainPlayer == NULL)
	{
		return;
	}

	FLOAT deltaTime = 0.8;
	if(MainPlayer->IsInSafeZone)
	{
		deltaTime = 0.4;
	}
	
	DOUBLE NowTime = appSeconds();
	if (NowTime - LastTimeCheckPlayerShowOrNot > deltaTime)
	{
		LastTimeCheckPlayerShowOrNot = NowTime;
	}
	else
	{
		return;
	}


	AMMOPlayerController* MainController = GetMainController();
	if (MainController == NULL)
	{
		return;
	}


	struct ShowNodeEx
	{
		FLOAT Distance;
		UBOOL bVisible;
		UBOOL bPet;
		AMMOActor* Actor;

		ShowNodeEx(){ Actor = NULL;}
	};

	struct CompareNodeEx
	{
		bool operator()(const ShowNodeEx& A, const ShowNodeEx& B)
		{
			if (A.Distance < B.Distance)
			{
				return true;
			}
			return false;
		}
	};

	static const INT MAPMAX = 29;
	INT mapNum[MAPMAX][MAPMAX];
	ZeroMemory(mapNum,sizeof(mapNum));
	std::list<ShowNodeEx> ActorList;

	for (STLInt2IntMap::iterator i = CharacterMap.begin(); i != CharacterMap.end(); ++i)
	{
		//AMMOActor* mmoActor = Cast<AMMOActor>(GetCharacter(i->first));
		AMMOActor* mmoActor = Cast<AMMOActor>(UObject::GetIndexedObject(i->second));
		if (!mmoActor)
		{
			continue;
		}

		//核实roledata----------------
		AMMOPlayer* mmoPlayer = Cast<AMMOPlayer>(mmoActor);
		if(mmoPlayer)
		{
			if (mmoPlayer->bSendRequestToServerToGetFullInfo==FALSE)
			{
				CMessageHandler::GetInstance().TrySyncRoleData(mmoPlayer->ServerID);
				mmoPlayer->bSendRequestToServerToGetFullInfo = TRUE;
			}
		}
		//核实roledata----------------

		EActorClassType actorType = (EActorClassType)mmoActor->GetActorType();
		if(actorType!=EACT_OtherPlayer && actorType!=EACT_Pet)
		{
			continue;
		}

		if(mmoActor->bActorDataReset==FALSE)//|| mmoActor->fActorDataResetDelay>0.0f)
		{
			mmoActor->bVisibleByCharacterCountLimit = FALSE;
			continue;
		}

		if (mmoActor == MainPlayer)
		{
			continue;
		}

		//AMMOPlayer* mmoPlayer = Cast<AMMOPlayer>(mmoActor);
		if (mmoPlayer && mmoPlayer->bRelevant)
		{
			if (mmoPlayer->bSendRequestToServerToGetFullInfo)
			{
				mmoPlayer->SetVisibleByCharacterCountLimit(TRUE);
			}
			continue;
		}

		FLOAT fNum=0.0f;
		//if(mmoActor->bIsActorInSafeZone)
		{
			INT Center = (MAPMAX+1)/2;
			INT X = mmoActor->GridX-MainPlayer->GridX+Center;
			INT Y = mmoActor->GridY-MainPlayer->GridY+Center;
			if(X>=0&&X<MAPMAX&&Y>=0&&Y<MAPMAX)
			{
				fNum=FLOAT(mapNum[X][Y]);
				mapNum[X][Y]++;
			}
			else
			{
				mmoActor->SetVisibleByCharacterCountLimit(FALSE);
				continue;
			}


		}

		ShowNodeEx NewNode;
		NewNode.bPet = (actorType==EACT_Pet);
		if(NewNode.bPet)
		{
			NewNode.Distance = (mmoActor->Location - MainPlayer->Location).Size2D()*2.0f + fNum*1920.0f;
		}
		else
		{
			NewNode.Distance = (mmoActor->Location - MainPlayer->Location).Size2D() + fNum*1920.0f;
		}
		NewNode.Actor = mmoActor;

		ActorList.push_back(NewNode);
	}
	ActorList.sort(CompareNodeEx());

	INT ActorLimitCount = GSystemSettings.MaxShowPlayerCount;
	if(!MainPlayer->IsInSafeZone)
	{
		ActorLimitCount = GSystemSettings.GetMaxShowPlayerCount();
	}

	for (std::list<ShowNodeEx>::iterator i = ActorList.begin(); i != ActorList.end(); ++i)
	{
		if (ActorLimitCount > 0)
		{
			i->Actor->SetVisibleByCharacterCountLimit(TRUE);
			ActorLimitCount--;
		}
		else
		{
			i->Actor->SetVisibleByCharacterCountLimit(FALSE);
		}
	}
}



//ADoodadActor* Facade::CreateDoodad(INT Id, const FVector& initLoc, const FRotator& initRot)
//{
//	UDoodad* doodad = FindDoodadTemplate(Id);
//	if (doodad == NULL)
//	{
//		warnf( NAME_Error, TEXT("Can not find Doodad Template by ID(%d)"), Id);
//		return NULL;
//	}
//
//	AActor* actor = GWorld->SpawnActor(ADoodadActor::StaticClass(), NAME_None, initLoc, initRot);
//	if(actor == NULL)
//	{
//		warnf( NAME_Error, TEXT("Can not Spawn an ADoodadActor by Id(%d)"), Id);
//		return NULL;
//	}
//
//	ADoodadActor* ADoodad = Cast<ADoodadActor>(actor);
//	if (ADoodad)
//	{
//		ADoodad->SetHidden(false);
//		ADoodad->SetCollision( true, false ,true );
//		ADoodad->setPhysics( PHYS_None);
//
//		if(doodad->DoodadAvatar)
//		{
//			ADoodad->Mesh->SetSkeletalMesh(doodad->DoodadAvatar->SkeletalMesh);
//			ADoodad->SetCollisionSize(doodad->DoodadAvatar->CollisionRadius,doodad->DoodadAvatar->CollisionHeight);
//			ADoodad->Mesh->BlockZeroExtent=true;
//			ADoodad->bBlockActors=true;
//			ADoodad->bCollideActors=true;
//			ADoodad->bCollideWorld=false;
//			ADoodad->BlockRigidBody=false;
//			ADoodad->SetCollisionType(COLLIDE_BlockAll);
//		}
//	}
//	return ADoodad;
//}

// 获取对象. NULL表示不存在
AMMOActor* MMOFacade::GetCharacter(INT Id)
{
	return Cast<AMMOActor>(GetActor(Id));
}

AActor* MMOFacade::GetActor(INT Id)
{
	AActor* RetActor = NULL;
	STLInt2IntMap::iterator pos = CharacterMap.find(Id);
	if (pos != CharacterMap.end())
	{
		RetActor = Cast<AActor>(UObject::GetIndexedObject(pos->second));
		if (RetActor && RetActor->IsA(AMMOActor::StaticClass()))
		{
			AMMOActor* mmoActor = (AMMOActor*)(RetActor);
			if (mmoActor->ServerID != Id)
			{
				RetActor = NULL;
				// data dirty
				//CharacterMap.erase(pos);
				//
				//MMOMapDataManager::getSingleton().RemoveMapDymcActorNear( Id );
			}
		}
		else if (RetActor && RetActor->IsA(AMMOEmitter::StaticClass()))
		{
			AMMOEmitter* mmoEmitter = (AMMOEmitter*)RetActor;
			if (mmoEmitter->ServerID != Id)
			{
				RetActor = NULL;
				//CharacterMap.erase(pos);
			}
		}
		else
		{
			RetActor = NULL;
			//CharacterMap.erase(pos);
		}
	}

	if (RetActor && RetActor->IsPendingKill())
	{
		RetActor = NULL;
	}

	return RetActor;
}

ANPC* MMOFacade::GetFrendlyNpc( INT hashindex)
{
	ANPC* FrendlyNpc = NULL;

	if (FriendlyNpcMapPos != FriendlyNpcMap.end())
	{
		FrendlyNpc = Cast<ANPC>(UObject::GetIndexedObject( FriendlyNpcMapPos->second ));
		FriendlyNpcMapPos++;
	}

	return FrendlyNpc;
}


AMMOEmitter* MMOFacade::GetEmitter(INT Id)
{
	return Cast<AMMOEmitter>(GetActor(Id));
}

void MMOFacade::DestroyObject(INT Id, UBOOL bRemoveFromWorld, INT disappearType)
{
	AMMOPlayerController* playerController = GetMainController(); 
//#if !SHIPPING_PC_GAME
//	warnf(NAME_Warning, TEXT("------------------------Facade::DestroyObject:%d"), Id);
//#endif

	if( playerController == NULL)
	{
		warnf( NAME_Error,  TEXT("[Facade::DestroyObject] AMMOPlayerController failed !") );
		return;
	}

	INT32 objType = GetTypeFromID( Id );
	//Id4位用来标记Obj类型

	switch(objType)
	{
	case otDoodad:
		{
			class AMMODoodadActor* doodadActor = DoodadManager::getSingleton().FindDoodadActor(Id);
			if(doodadActor)
			{
				if(!doodadActor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
				{
					doodadActor->SetServerLocation(FVector(-1,-1,-1));	// zhaojun@snda 2015-3-3, remove from grid world
				}

				FSMIO::getSingleton().Target.ClearActor(doodadActor);
				//MMOTargetManager::getSingleton().ClearActor(doodadActor);
				DoodadManager::getSingleton().DestroyDoodadActor(Id, bRemoveFromWorld, disappearType);
			}
		}
		break;

	case otDrop:
		{
			class AMMODropActor* dropActor = MMODropManager::getSingleton().FindDropActor(Id);
			if(dropActor)
			{
				FSMIO::getSingleton().Target.ClearActor(dropActor);
				//MMOTargetManager::getSingleton().ClearActor(dropActor);
				MMODropManager::getSingleton().DestroyDropActor(Id, bRemoveFromWorld);
			}
			else
			{
				warnf(NAME_Error, TEXT("[Facade::DestroyObject] faild to find drop actor:%d"), Id);
			}
		}
		break;

	default://@todo 其它类型	
		{
			AActor* actor = GetActor(Id);
			if (actor && actor != MainController->Pawn)
			{
				FSMIO::getSingleton().Target.ClearActor(actor);
				//MMOTargetManager::getSingleton().ClearActor(actor);

				if(actor->IsA(AMMOActor::StaticClass()))
				{
					MMOGameEvent_ActorDestroyed evt(Cast<AMMOActor>(actor));
					MMOUI::PostGameEvent(&evt);
				}

				if(actor->IsA(ANPC::StaticClass()))
				{
					MMOGameEvent_NPCDestroyed evt(Cast<ANPC>(actor));
					MMOUI::PostGameEvent(&evt);
				}

				if (actor->IsA(AMMOPlayer::StaticClass()))
				{
					UStallManager::getSingleton().DestroyStallActor(Cast<AMMOPlayer>(actor));
				}

				ANPC* npc = Cast<ANPC>( actor );
				if( npc && !npc->IsMonster())
				{
					FriendlyNpcMap.erase(Id);
				}

				if (bRemoveFromWorld)
				{
					if(actor->IsA(AMMOActor::StaticClass()))
					{
						// zhaojun@snda	2015-3-3
						AMMOActor* pAMMOActor = Cast<AMMOActor>(actor);
						check(pAMMOActor);
						pAMMOActor->SetServerLocation(FVector(-1,-1,-1));	//remove from grid world
						// ~~~~zhaojun@snda	2015-3-3
					}

					if( actor->IsA(AMMOEmitter::StaticClass()))
					{
						AMMOEmitter* emit = (AMMOEmitter*)actor;
						emit->Speed = 0.f;
						if(emit->MyPointLightComponent)
						{
							emit->MyPointLightComponent->Brightness = 0.f;
						}

						emit->Duration = emit->DelayToKill + emit->CurrentTime;
						actor->SetHidden(TRUE);
#if SHOW_MIssile_LOG
						debugf(TEXT("hide Missile %d time=%f"), emit->ServerID, emit->Duration);

#endif
						//debugf(L" server delete projectile");
						return;
					}
					else
					{
						if(!actor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
						{
							GWorld->DestroyActor(actor);
						}

						if( objType == otRole )
						{
							DecPlayerCount;
						}
						else if(objType == otNpc)
						{
							--NPCCount;
						}
					}
				}

				//删除buff信息
				MMOFacade::getSingleton().GetMMOBuffManager()->DeleteBuffByCharacterID(Id);
				CharacterMap.erase(Id);
				PlayerMap.erase(Id);
				RaiseCharacterRemoveEvent();
				//
				MMOMapDataManager::getSingleton().RemoveMapDymcActorNear( Id );
			}
		}
		break;
	}
}

void MMOFacade::RemoveFromCharacterMap(INT Id)
{
	CharacterMap.erase(Id);
	FriendlyNpcMap.erase(Id);
	PlayerMap.erase(Id);
	RaiseCharacterRemoveEvent();
}

void MMOFacade::RaiseCharacterRemoveEvent()
{
	MMOGameEvent_Any evt( GameEvent_CharactorRemoved);
	MMOUI::PostGameEvent(&evt);
}

void MMOFacade::AddToCharacterMap(AMMOActor* actor, INT id)
{
	CharacterMap.insert(std::make_pair(id, actor->GetIndex()));
}


//------------------------注意此请求人物非服务器必回的请求，不要以接受这个请求回包作为解除逻辑阻塞的条件------------------------------------------
PlayerBaseInfo* MMOFacade::RequestPlayerBaseInfo( INT roleID , BOOL forceRequest )
{
	INT CharType = GetTypeFromID(roleID);
	if (CharType != otRole)
	{
		warnf(NAME_Error, TEXT("RequestPlayerBaseInfo is called, but the roleId's owner is not a player, roleID:%d %X"), roleID, roleID);
		return NULL;
	}

	if (roleID <= 0)
	{
		// 0的话服务器会汇报
		warnf(NAME_Error, TEXT("Invalide role id RequestPlayerBaseInfo:%d"), roleID);
		return NULL;
	}

	PlayerBaseInfo* info = GetPlayerBaseInfo( roleID );
	if( info )
	{
		//forceRequest：不管是否有本地信息，都请求，为了获得最新的数据（PlayerBaseInfo有些数据偶尔会变，有些数据不会变）
		if( forceRequest )
		{
			_PlayerBaseInfoRequestBuffMap.Set( roleID , info->_Crc );
		}

		return info;
	}
	else
	{
		_PlayerBaseInfoRequestBuffMap.Set( roleID , 0 );
		return NULL;
	}
	return NULL;
}

//------------------------------------------------------------------------
PlayerBaseInfo *MMOFacade::GetPlayerBaseInfo( INT roleID )
{
	PlayerBaseInfo *info = _PlayerBaseInfoMap.Find( roleID );
		
	return info;
}

INT MMOFacade::GetPlayerBaseInfoByName(FString name)
{
	INT id = 0;

	PlayerBaseInfoMap::TIterator itor (_PlayerBaseInfoMap);
	for( ; itor ; ++itor )
	{
		PlayerBaseInfo info = itor.Value();
		if (info._RoleName == name)
		{
			id = info._RoleID;
			break;
		}
	}
	return id;
}

//
PlayerBaseInfo *MMOFacade::AddPlayerBaseInfo( M2C_SYNC_ROLE_INFO &data )
{	 
	PlayerBaseInfo& info = _PlayerBaseInfoMap.FindOrAdd( data.dwRoleID );
	
	info._RoleID = data.dwRoleID;
	info._RoleName = UTF8_TO_TCHAR(data.szRoleName);
	info._Sex = data.bySex;
	info._RoleClass = data.byClass;
	info._Level = 0;
	info._VIPType = data.byVIPLevel;
	info._GuildName = UTF8_TO_TCHAR(data.szGuildName);

	// modified by zhangrui05
	//info._Crc = appMemCrc( &data , sizeof(M2C_SYNC_ROLE_INFO) );
	INT crc = crc32(0, (const Bytef*)(&data), sizeof(M2C_SYNC_ROLE_INFO));

	MMOGameEvent_PlayerBaseInfo evt( info._RoleID , &info );
	MMOUI::PostGameEvent(&evt);

	return &info;
}

//
void MMOFacade::RemovePlayerBaseInfo( INT roleID )
{
	_PlayerBaseInfoMap.Remove(roleID);
}

#define SendRequestInterval 1.0f
void MMOFacade::TickPlayerInfoRequestTime( FLOAT deltaTime )
{
	CTRL_TICK("MMOFacade::TickPlayerInfoRequestTime");

	if( _PlayerBaseInfoRequestBuffMap.Num() > 0 )
	{
		PlayerInfoRequestTime += deltaTime;
		if( PlayerInfoRequestTime >= SendRequestInterval )
		{
			PlayerInfoRequestTime = 0.0f;
			PlayerBaseInfoRequestInfoMap::TIterator itor(_PlayerBaseInfoRequestBuffMap);

			//这个256是等服务器的宏
			INT countLimit = (MAX_QUERY_ROLE_INFO_PER_SEC / 4) * 3;//256/2;
			for( ; itor ; ++itor )
			{

				CMessageHandler::GetInstance().Send_QueryPlayerInfo( itor.Key() , itor.Value() );
				//_PlayerBaseInfoRequestBuffMap.Remove( itor.Key() );
				itor.RemoveCurrent();

				countLimit--;
				if( countLimit <= 0 )
				{
					break;
				}
			}
		}
	}
}

//------------------------------------------------------------------------
void MMOFacade::SetCurrentPlayerController(class AMMOPlayerController* ctrl)
{
#ifdef _DEBUG
	if(ctrl == NULL)
	{
		warnf(TEXT("[Facade::SetCurrentPlayerController] player controller is NULL"));
		appDebugBreak();
	}
#endif
	MainController = ctrl;
}
////------------------------------------------------------------------------
//MMOPlayerControllerAI* Facade::GetMainControllerAI()
//{
//	if (!MainControllerAI)
//	{
//		MainControllerAI = new MMOPlayerControllerAI();
//	}
//	return MainControllerAI;
//}

AMMOPlayerController* MMOFacade::GetMainController()
{
	return MainController;
}

AMMOActor* MMOFacade::GetMainActor() const
{
	if (MainController)
	{
		return Cast<AMMOActor>(MainController->Pawn);
	}
	return NULL;
}
//------------------------------------------------------------------------------------
AMMOHud* MMOFacade::GetMyHud()
{
	if( MainController && MainController->myHUD )
	{
		return Cast<AMMOHud>(MainController->myHUD);
	}
	return NULL;
}


AMMOPlayer* MMOFacade::GetMainHero() const
{
	INT characterId = MMOFacade::getSingleton().GetMainHeroId();
	return Cast<AMMOPlayer>(MMOFacade::getSingleton().GetCharacter(characterId));
}

INT			MMOFacade::GetMainHeroId()
{
	if (MainController)
	{
		AMMOPlayer* Player = Cast<AMMOPlayer>(MainController->Pawn);
		if(Player)
		{
			return Player->ServerHeroID;
		}
	}
	return 0;
}

void		MMOFacade::SetMainHeroId(INT HeroId)
{
	if (MainController)
	{
		AMMOPlayer* Player = Cast<AMMOPlayer>(MainController->Pawn);
		if(Player)
		{
			Player->ServerHeroID = HeroId;
		}
		}
	}


UBOOL MMOFacade::GetHeroSummon()
{
	if( MMOFacade::getSingleton().GetMainPlayer() )
		return MMOFacade::getSingleton().GetMainPlayer()->HeroSummon;
	else
		return FALSE;
}

void MMOFacade::SetHeroSummon(UBOOL bSummon)
{
	if( MMOFacade::getSingleton().GetMainPlayer() )
		MMOFacade::getSingleton().GetMainPlayer()->HeroSummon = bSummon;
}

//------------------------------------------------------------------------------------
AMMOPlayer* MMOFacade::GetMainPlayer() const
{
	if (MainController)
	{
		return Cast<AMMOPlayer>(MainController->Pawn);
	}
	return NULL;
}
//---------------------------------------------------------------------------------------
FLOAT MMOFacade::GetMainActorCollisionHeight()
{
	if (MainController && MainController->Pawn && MainController->Pawn->CylinderComponent)
	{
		return MainController->Pawn->CylinderComponent->CollisionHeight;
	}
	return 0;
}



void MMOFacade::AddMainCharacter(AMMOPlayer* MainCharacter)
{
	if (MainCharacter)
	{
		STLInt2IntMap::iterator pos = CharacterMap.find(MainCharacter->ServerID);
		if (pos == CharacterMap.end())
		{
			CharacterMap.insert(make_pair(MainCharacter->ServerID, MainCharacter->GetIndex()));
		}
	}
	else
	{
		AMMOPlayerController* controller = GetMainController();
		if (!controller)
		{
			return;
		}
		AMMOActor* MainPawn = Cast<AMMOActor>(controller->Pawn);
		if(MainPawn)
		{
			STLInt2IntMap::iterator pos = CharacterMap.find(MainPawn->ServerID);
			if (pos == CharacterMap.end())
			{
				CharacterMap.insert(make_pair(MainPawn->ServerID, MainPawn->GetIndex()));
			}
		}
	}
}

//------------------------------------------------------------------------
std::vector<INT> ToDeleteList;
void MMOFacade::ClearAllCharacter()
{
	FriendlyNpcMap.clear();
	PlayerMap.clear();

	INT MainPlayerId = 0;
	AMMOPlayer* MainActor = GetMainPlayer();
	if (MainActor)
	{
		MainPlayerId = MainActor->ServerID;
	}

	ToDeleteList.clear();
	for (STLInt2IntMap::iterator pos = CharacterMap.begin(); pos != CharacterMap.end(); ++pos)
	{
		if (MainPlayerId != pos->first)
		{
			ToDeleteList.push_back(pos->first);
		}
	}

	for (UINT32 i = 0; i < ToDeleteList.size(); ++i)
	{
		M_FORCE_TRY_BEGIN;
		DestroyObject(ToDeleteList[i], TRUE, sodtLeaveScene);
		M_FORCE_TRY_END("Protected for ClearAllCharacter.DestroyObject");
	}

	ToDeleteList.clear();

	CharacterMap.clear();

	RaiseCharacterRemoveEvent();

	if (MainActor)
	{
		AddMainCharacter(MainActor);
	}
}
//------------------------------------------------------------------------
void MMOFacade::RegisterTrigger(TScriptInterface<IIServerTrigger> trigger )
{
	TriggerObjectMap::iterator i = _triggerObjectMap.find(trigger->GetTriggerID());
	if(i != _triggerObjectMap.end())
	{
		warnf(NAME_Warning, TEXT("[Facade::RegisterTrigger] Trigger replaced, ID[%d]"), trigger->GetTriggerID());
		i->second = trigger;
		return;
	}

	_triggerObjectMap.insert( std::make_pair(trigger->GetTriggerID(), trigger) );
}
//------------------------------------------------------------------------
TScriptInterface<IIServerTrigger> MMOFacade::FindTrigger( INT serverID ) const
{
	TriggerObjectMap::const_iterator it = _triggerObjectMap.find(serverID);
	if(it != _triggerObjectMap.end())
	{
		return it->second;
	}

	checkMsg(false, "[Facade::FindTrigger] trigger id not found");
	return TScriptInterface<IIServerTrigger>();
}
//------------------------------------------------------------------------

//extern UBOOL g_froceFullSpeedLoad;

void MMOFacade::Teleport( INT sceneID, const FVector& initPos, INT yaw )
{
	//g_froceFullSpeedLoad = TRUE;

	_initTeleportPos = initPos;

	//记录当前地图
	MMOMapDataManager::getSingleton().SetCurMap( sceneID );

	//if (GetMainController()->Pawn && GetMainController()->Pawn->CylinderComponent)
	//{
	//	_initTeleportPos.Z += GetMainController()->Pawn->CylinderComponent->CollisionHeight;
	//}

	AMMOPlayer* player = GetMainPlayer();
	if(player)
	{
		player->PlayTeleportLeaveEffect(player->GetFootLocation(), player->Rotation);
	}

	ClearAllCharacter();
	ClearAllTriggerVolume();
	_triggerObjectMap.clear();

	FString mapName = MMOMapDataManager::getSingleton().GetCurMap().MapName;
	GetMainController()->eventTeleportToScene(mapName, _initTeleportPos);

	if(GetMainController()->Pawn)
	{
		FRotator NewRotation = GetMainController()->Pawn->Rotation;
		NewRotation.Yaw = yaw;
		GetMainController()->Pawn->SetDesiredRotation(NewRotation);
	}

	SET_WARN_COLOR(COLOR_PURPLE);
	warnf( TEXT("==== [Facade::Teleport] [%s] (%.2f,%.2f,%.2f) Yaw[%d]"), 
		*mapName, _initTeleportPos.X, _initTeleportPos.Y, _initTeleportPos.Z, yaw);
	CLEAR_WARN_COLOR();
}
//------------------------------------------------------------------------
//地图点击移动
UBOOL MMOFacade::PlayerMoveToScenePos( FExportMap &toMoveSceneInfo, FVector world3DPos )
{
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	FExportMap curMapInfo = pMapDataMgr->GetCurMap();
	if( toMoveSceneInfo.ServerID != curMapInfo.ServerID ){
		FString errorName = TEXT("MMOErrorInfo.Navigation.NotSupportAccross");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errorName ), MsgInfoType_Table, errorName );
		return FALSE;
	}
	//cannot walk across dynamic map
	//if( curWorldMapInfo.ServerID != curMapInfo.ServerID 
	//	&& ( ( curWorldMapInfo.ServerMapType != E_est_Static || curWorldMapInfo.ServerMapType != E_est_Line )
	//		 || ( curMapInfo.ServerMapType != E_est_Static || curMapInfo.ServerMapType != E_est_Line ) ) )
	//{
	//	return;
	//}

	////副本与副本间不能寻路
	//UBOOL isTargetMapDynamicScene = ( curWorldMapInfo.ServerMapType != E_est_Static && curWorldMapInfo.ServerMapType != E_est_Line );
	//UBOOL isCurrentMapDynamicScene = ( curMapInfo.ServerMapType != E_est_Static && curMapInfo.ServerMapType != E_est_Line );
	//UBOOL isSameScene = ( curMapInfo.ServerID == curWorldMapInfo.ServerID );
	//if( ( isCurrentMapDynamicScene || isTargetMapDynamicScene ) && !isSameScene  ){
	//	return;
	//}

	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if( ctrl ){
		//FCheckResult *hitWorld;
		//FVector startPos( world3DPos.X, world3DPos.Y, 60000 );
		//world3DPos.Z = 0;

		////UBOOL isHit = GWorld->SingleLineCheck( hitWorld, ctrl->Pawn, world3DPos, startPos, TRACE_AllBlocking );
		//hitWorld = MMO::PickPoint(startPos, world3DPos, ctrl->Pawn);
		//if( hitWorld ){
		//	world3DPos.Z = hitWorld->Location.Z + 64;//因为格子取整
		//}

		INT64 llSrcX = 0;
		INT64 llSrcY = 0;
		INT64 llSrcZ = 0;
		ZONE_DATA* pDestZone = NULL;
		GRID_DATA* pDestGrid = NULL;
		FVector startPos( world3DPos.X, world3DPos.Y, ctrl->Location.Z + 64*10 );

		gServerGridWorld.ConvertPosClient2Server( startPos, llSrcX, llSrcY, llSrcZ );
		gServerGridWorld.GetZoneAndGrid( llSrcX, llSrcY, llSrcZ, pDestZone, pDestGrid );
		if( pDestGrid != NULL ){
			world3DPos.Z = pDestGrid->uHighLayer * GRID_LAYER_HEIGHT/POINT_PER_CM + 64;
		}
	}

	UBOOL isSuccess = MMONavigationManager::getSingleton().Navigate( toMoveSceneInfo.ServerID, world3DPos, FALSE );
	//
	if( isSuccess ){
		UINT interval = pMapDataMgr->GetCurSceneWorldMapPathInterval();
		TArray<FVector> pathPos = pMapDataMgr->GetCurMainPlayerPath( interval, TRUE );

		//if( pathPos.Num() <= 0 ){
		//	pMapDataMgr->InvalidCurPath();
		//	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOErrorInfo.Navigation.NoPath") ) ,MsgInfoType_Table,TEXT("MMOErrorInfo.Navigation.NoPath") );	
		//	return;
		//}
		pMapDataMgr->SetMapPath( pathPos );
	}

	return isSuccess;
}
//------------------------------------------------------------------------
void MMOFacade::CorrectTeleportPos()
{
	if( _initTeleportPos.Z <= 0 )
	{
		if( MainController )
			_initTeleportPos = TraceGroundPosition( MainController->Pawn, _initTeleportPos );
	}
}
//------------------------------------------------------------------------
FVector MMOFacade::TraceGroundPosition(APawn* pawn, const FVector& startPos)
{
	if (GSeamlessTravelHandler.IsInTransition())
	{
		debugf(L"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5");
		return startPos;
	}
	if( pawn == NULL )
		return startPos;

	try
	{
		FCheckResult Hit(1.f);
		FVector GravDir = FVector(0,0,-1);
		FVector Down = GravDir * (100000 + MAXSTEPHEIGHTFUDGE);
		FVector Up = GravDir * (100000 + MAXSTEPHEIGHTFUDGE);

		FVector ColLocation = pawn->CollisionComponent ? pawn->CollisionComponent->Bounds.Origin : pawn->Location;

		ColLocation.X = startPos.X;
		ColLocation.Y = startPos.Y;

		// note: MoveActor multiplies extent by FVector(1.001f,1.001f,1.0f), we do smaller checks here so that if we get popped onto a surface due to floating point error, we will not stick there
		UBOOL r = GWorld->SingleLineCheck( Hit, pawn, ColLocation + Down, ColLocation - Up, TRACE_AllBlocking, pawn->GetCylinderExtent() );
		//FloorDist = Hit.Time * (MaxStepHeight + MAXSTEPHEIGHTFUDGE);
		if( r == 1 )
		{
			warnf( NAME_Warning, TEXT("No Trace Result at (%.2f, %.2f, %.2f)"), startPos.X, startPos.Y, startPos.Z );
			return startPos;
		}
		else
		{
			warnf( NAME_Warning, TEXT("Trace[%d] (%.2f, %.2f, %.2f)"), r, Hit.Location.X, Hit.Location.Y, Hit.Location.Z );
			return Hit.Location;
		}
	}
	catch(...)
	{
		return startPos;
	}
}
//------------------------------------------------------------------------------------
extern FServerGridWorld gServerGridWorld;
UBOOL MMOFacade::CheckPlayerBlock(FVector &lastPos,FVector &nowPos, AMMOActor* pBaseActor)
{

	INT		BlockType	= MMOFacade::getSingleton().BlockType;
	FLOAT	BlockRadius = MMOFacade::getSingleton().BlockRadius;

	//所有的阻挡物进行遍历筛选
	FVector StartPos = lastPos+FVector(0.0,0.0,64.0);
	FVector BuffPos  = nowPos-lastPos;
	BuffPos.Normalize();
	FVector EndPos   = StartPos+BuffPos*128.0;

	DWORD TraceFlags = TRACE_World | TRACE_Visible | TRACE_Pawns | TRACE_Others;
	FVector TraceExtend = FVector(0, 0, 0);
	FMemMark Mark(GMainThreadMemStack);
	FCheckResult* Hit = GWorld->MultiLineCheck( GMainThreadMemStack, EndPos, StartPos, TraceExtend, TraceFlags, pBaseActor);
	while(Hit)
	{
		AMMOEmitter* HitActor = Cast<AMMOEmitter>(Hit->Actor);
		if(HitActor)
		{
			if(HitActor->CollisionType==COLLIDE_BlockAll)
			{
				//warnf(NAME_Log, TEXT("--------------------------OLLIDE_BlockAll"));
				return TRUE;
			}
		}
		//warnf(NAME_Log, TEXT("--------------------------NULL"));
		Hit = Hit->GetNext();
	}

	//安全区内无阻挡
	if(MMOFacade::getSingleton().GetMainPlayer()->IsInSafeZone==TRUE)
	{
		return FALSE;
	}

	//ZONE_DATA *pOldZone = NULL;	
	//GRID_DATA *pOldGrid = NULL;	
	//gServerGridWorld.GetZoneAndGrid(lastPos, pOldZone, pOldGrid);
	//if(pOldGrid && pOldGrid->bRoleBlock)
	//{

	//}




	//暂时屏蔽人物的阻挡
	//if(BlockType==0)
	//{
	//	//player遍历
	//	STLInt2IntMap* PlayerMap = MMOFacade::getSingleton().GetPlayerMap();
	//	STLInt2IntMap::iterator PlayerMapItor ;
	//	for ( PlayerMapItor= PlayerMap->begin();  PlayerMapItor !=PlayerMap->end(); PlayerMapItor++ )
	//	{
	//		AMMOPlayer* pl = Cast<AMMOPlayer>(MMOFacade::getSingleton().GetCharacter(PlayerMapItor->first) );
	//		if(pl){

	//			FLOAT dis  = (nowPos-pl->Location).Size();
	//			if(dis<=BlockRadius*4.0f)
	//			{
	//				FLOAT disLast  = (lastPos-pl->Location).Size();
	//				if(disLast>dis)
	//				{
	//					FVector dirA = nowPos-lastPos;
	//					dirA.Normalize();
	//					FVector C = lastPos-pl->Location;
	//					FLOAT	modeC2 = C.SizeSquared();
	//					FLOAT	modeA = dirA|C;
	//					FLOAT	modeB = sqrt(modeC2-modeA*modeA);

	//					if(modeB<=BlockRadius*2.0f)
	//					{
	//						return TRUE;
	//					}
	//				}
	//			}

	//		}
	//	}
	//}
	//else if(BlockType==1){
	//	//npc遍历
	//	STLInt2IntMap* NpcMap = MMOFacade::getSingleton().GetCharacterMap();
	//	STLInt2IntMap::iterator NpcMapItor ;
	//	for ( NpcMapItor= NpcMap->begin();  NpcMapItor !=NpcMap->end(); NpcMapItor++ )
	//	{
	//		ANPC* npc = Cast<ANPC>(MMOFacade::getSingleton().GetCharacter(NpcMapItor->first) );
	//		if(npc){
	//			FLOAT dis  = (nowPos-npc->Location).Size();
	//			if(dis<=BlockRadius*3.0f)
	//			{
	//				FLOAT disLast  = (lastPos-npc->Location).Size();
	//				if(disLast>dis)
	//				{
	//					FVector dirA = nowPos-lastPos;
	//					dirA.Normalize();
	//					FVector C = lastPos-npc->Location;
	//					FLOAT	modeC2 = C.SizeSquared();
	//					FLOAT	modeA = dirA|C;
	//					FLOAT	modeB = sqrt(modeC2-modeA*modeA);

	//					if(modeB<=BlockRadius*2.0f)
	//					{
	//						return TRUE;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	return FALSE;
}

//------------------------------------------------------------------------------------
// void Facade::AddShopTemplate(INT id, class UShop* shop)
// {
// 	ShopTemplateMap::const_iterator it = _shopTemplateMap.find(id);
// 	if(it != _shopTemplateMap.end())
// 	{
// 		warnf(NAME_Error, TEXT("[Facade::AddShopTemplate] Shop template already exist:%d"),	id);
// 	}
// 	else
// 	{
// 		_shopTemplateMap.insert(std::make_pair(id, shop));
// 	}
// }
// //------------------------------------------------------------------------------------
// void Facade::ClearShopTemplates()
// {
// 	_shopTemplateMap.clear();
// }
//------------------------------------------------------------------------------------
// FShopData*  Facade::FindShopTemplate(INT templateID)
// {
// 	return Shop
// // 	ShopTemplateMap::const_iterator pos = _shopTemplateMap.find(templateID);
// // 	if (pos != _shopTemplateMap.end())
// // 	{
// // 		return pos->second;
// // 	}
// // 	else
// // 	{
// // 		FString dataPath = _FindShopTemplateObjPath(templateID);
// // 		if(!dataPath.IsEmpty())
// // 		{
// // 			UShop* datTmp =
// // 				Cast<UShop>(UObject::StaticLoadObject(UShop::StaticClass(), NULL, *dataPath, NULL, LOAD_NoWarn | LOAD_Quiet, NULL));
// // 			if(datTmp)
// // 			{
// // 				datTmp->AddToRoot();
// // 				AddShopTemplate(templateID, datTmp);
// // 				return datTmp;
// // 			}
// // 			else
// // 			{
// // 				warnf(NAME_Error, TEXT("[Facade::FindShopTemplate] Shop template[%d][%s] faild to load!"), templateID, *dataPath);
// // 				return NULL;
// // 			}
// // 		}
// // 	}
// // 
// // 	return NULL;
// }
//------------------------------------------------------------------------
// FShopItemTemplate MMO::Facade::FindShopItem( INT ShopTemplateID, INT ItemIdx )
// {
// 	FShopItemTemplate itm(EC_EventParm);
// 
// 	UShop* shopTemp = FindShopTemplate(ShopTemplateID);
// 	if (shopTemp)
// 	{
// 		if(ItemIdx < shopTemp->ItemList.Num())
// 		{
// 			itm = shopTemp->ItemList(ItemIdx);
// 		}
// 	}
// 
// 	return itm;
// }
//------------------------------------------------------------------------
class UNPCTemplate* MMOFacade::FindNPCTemplate(INT TemplateID)
{
	NPCTemplateMap::iterator it = _npcTemplateMap.find(TemplateID);
	if(it != _npcTemplateMap.end())
	{
		return it->second;
	}

	const NPCTemplateDataIndex* dat = NPCTemplateDataIndexManager::getSingleton().FindData(TemplateID);
	if(!dat)
	{
		return NULL;
	}

	FString dataPath = dat->ObjectPath;
	if(!dataPath.IsEmpty())
	{
		UNPCTemplate* datTmp =
			Cast<UNPCTemplate>(UObject::StaticLoadObject(UNPCTemplate::StaticClass(), NULL, *dataPath, NULL, LOAD_NoWarn | LOAD_Quiet, NULL));
		if(datTmp)
		{
			datTmp->DisplayName = dat->DisplayName;
#if !SHIPPING_PC_GAME
			warnf(NAME_Warning, TEXT("Loading npc:%s"), *dataPath);
#endif
			datTmp->AddToRoot();
			AddNPCTemplate(TemplateID, datTmp);
			return datTmp;
		}
		else
		{
			warnf(NAME_Error, TEXT("[Facade::FindNPCTemplate] NPC template[%d][%s] faild to load!"), TemplateID, *dataPath);
			return NULL;
		}
	}

	warnf(TEXT("[Facade::FindNPCTemplate] NPC template not found:%d"), TemplateID);

	return NULL;
}

//
void	MMOFacade::ResetTitleData()
{
	TitleDataMap::iterator it = _TitleDataMap.begin(); 
	for(; it != _TitleDataMap.end(); it++)
	{
		it->second.ValidTime = 0;
	}	
}

//
TitleData* MMOFacade::GetTitleData(INT ID)
{
	TitleDataMap::iterator it = _TitleDataMap.find(ID); 
	if(it != _TitleDataMap.end())
	{
		return &(it->second);
	}
	return NULL;
}

//
TweenData* MMOFacade::GetTweenData(FString stringID)
{
	TweenDataMap::iterator it = _TweenDataMap.find(stringID); 
	if(it != _TweenDataMap.end())
	{
		return &(it->second);
	}
	return NULL;
}

MountData* MMOFacade::GetMountData(INT ID)
{
	MountDataMap::iterator it = _MountDataMap.find(ID); 
	if(it != _MountDataMap.end())
	{
		return &(it->second);
	}
	return NULL;
}


ShowPanelAdvancedData* MMOFacade::GetShowPanelAdvancedData(const FString& mapName)
{
	ShowPanelAdvancedDataMap::iterator it = _ShowPanelAdvancedDataMap.find(mapName); 
	if(it != _ShowPanelAdvancedDataMap.end())
	{
		return &(it->second);
	}
	return NULL;
}

void	MMOFacade::DelShowPanelAdvancedData(const FString& mapName)
{
	_ShowPanelAdvancedDataMap.erase(mapName);
}

void	MMOFacade::SetShowPanelAdvancedData(const FString& mapName, ShowPanelAdvancedData& AdvancedData)
{
	_ShowPanelAdvancedDataMap.insert(std::make_pair(mapName, AdvancedData));
}


INT	MMOFacade::GetTitleDataArrayCount()
{
	return _TitleDataArray.Num();
}

TitleData* MMOFacade::GetTitleDataArray(INT Index)
{
	if(Index>=0 && Index<_TitleDataArray.Num())
	{
		return &(_TitleDataArray(Index));
	}
	else
	{
		return NULL;
	}
}



//------------------------------------------------------------------------
void MMOFacade::AddNPCTemplate(INT k, class UNPCTemplate* npc)
{
	NPCTemplateMap::const_iterator i = _npcTemplateMap.find(k);
	if(i != _npcTemplateMap.end())
	{
		warnf(NAME_Error, TEXT("[Facade::AddNPCTemplate] NPC template already exist:%d[%s]->[%s]"),
			k, *npc->NameID, *i->second->NameID);
	}
	else
	{
		if(!npc->AvatarTemplate)
		{
			warnf(NAME_Error, TEXT("[Facade::AddNPCTemplate] !!!!!!!!!!NPC AvatarData is NULL:ID[%d]\'%s\'"), npc->TemplateID, *npc->GetDisplayName());
		}

		_npcTemplateMap.insert(std::make_pair(k, npc));
	}
}
//------------------------------------------------------------------------
void MMOFacade::ClearNPCTemplates()
{
	NPCTemplateMap::iterator i = _npcTemplateMap.begin();
	for (; i != _npcTemplateMap.end(); ++i)
	{
		i->second->RemoveFromRoot();
	}

	_npcTemplateMap.clear();
}
//====================================================================================
void MMOFacade::AddMissileTemplate(INT Id, UMissile* data)
{
	MissileTemplateMap::iterator pos = MissileTemplates.find(Id);
	if (pos == MissileTemplates.end())
	{
		MissileTemplates.insert(make_pair(Id, data));
	}
	else
	{
		warnf(NAME_Error, TEXT("there are two same MissilteTemplates, Id=%d"), Id);
	}
}
//------------------------------------------------------------------------
UMissile* MMOFacade::FindMissileTemplate(INT Id)
{
	MissileTemplateMap::iterator pos = MissileTemplates.find(Id);
	if (pos != MissileTemplates.end())
	{
		return pos->second;
	}
	else
	{
		MissileTemplateDataIndex* RetDatat = MissileTemplateDataIndexManager::getSingleton().FindAndModifyData(Id);
		UMissile* Ret = NULL;
		if (RetDatat)
		{
			FString PathName = RetDatat->ObjectPath;
			Ret = Cast<UMissile>(UObject::StaticLoadObject(UMissile::StaticClass(), NULL, *PathName, NULL, LOAD_None, NULL));
		}
		if (!Ret)
		{
			warnf(TEXT("Can not found missile template by id=%d"), Id);
		}
		else
		{
			Ret->AddToRoot();
			MissileTemplates.insert(make_pair(Id, Ret));
		}
		return Ret;
	}
}
//------------------------------------------------------------------------
void MMOFacade::ClearAllMissileTemplate()
{
	MissileTemplates.clear();
}



//------------------------------------------------------------------------
void MMOFacade::RegisterTriggerVolume( AMMOVolume_Trigger* vol )
{
	TriggerVolumeMap::iterator i = _triggerVolumeMap.find(vol->ServerUniqueID);
	if(i != _triggerVolumeMap.end())
	{
		warnf(NAME_Warning, TEXT("[Facade::RegisterTriggerVolume] replaced, ID[%d]"), vol->ServerUniqueID);
		i->second = vol;
		return;
	}

	_triggerVolumeMap.insert( std::make_pair(vol->ServerUniqueID, vol) );
}
//------------------------------------------------------------------------
void MMOFacade::ClearAllTriggerVolume()
{
	_triggerVolumeMap.clear();
}
//------------------------------------------------------------------------
AMMOVolume_Trigger* MMOFacade::FindTriggerVolume( INT areaID ) const
{
	TriggerVolumeMap::const_iterator i = _triggerVolumeMap.find(areaID);
	if(i != _triggerVolumeMap.end())
	{
		return i->second;
	}

	warnf(NAME_Warning, TEXT("[Facade::GetTriggerVolume] Area not found, ID[%d]"), areaID);

	return 0;
}


void MMOFacade::AddSoundParticle(INT key, FSoundParticleEffect* node)
{
	if (node != NULL)
	{
		SoundParticleMap.insert(make_pair(key, node));
	}
}

void MMOFacade::ClearSoundParticle()
{
	SoundParticleMap.clear();
}

FSoundParticleEffect* MMOFacade::GetSoundParticle(INT key)
{
	Int2SoundParticleMap::iterator pos = SoundParticleMap.find(key);
	if (pos != SoundParticleMap.end())
	{
		return pos->second;
	}
	return NULL;
}
//------------------------------------------------------------------------
void MMOFacade::AddHitSoundGroup( INT key, USoundCue* node )
{
	if(node != NULL)
	{
		HitSoundGrpMap.insert( std::make_pair(key, node) );
	}
}

//------------------------------------------------------------------------
void MMOFacade::ClearHitSoundGroup()
{
	HitSoundGrpMap.clear();
}

//------------------------------------------------------------------------
INT MMOFacade::GetSceneLevel()
{
	return _sceneLevel;
}
//------------------------------------------------------------------------
void MMOFacade::SetSceneLevel( INT sceneLevel )
{
	_sceneLevel = sceneLevel;
}

void MMOFacade::SetSceneDifficulty( INT sceneDifficulty )
{
	_sceneDifficulty = sceneDifficulty;
}


//------------------------------------------------------------------------
//UAI_Affix_Table* MMO::Facade::FindAffixTable( INT monsterLevel , INT monsterQuantity )
//{
//
//	FString newName = FString::Printf(TEXT("%d_%d_%d"),SceneLevel,monsterQuantity,monsterLevel);
//	std::string nameString = (TCHAR_TO_ANSI(*newName));
//	AffixTemplateMap::const_iterator pos = _affixTemplateMap.find( nameString );
//		
//
//	if (pos != _affixTemplateMap.end())
//	{
//		return pos->second;		
//	}
//
//	return NULL;
//}
//
//BOOL MMOFacade::InitAffix()
//{
//	TArray<TArray<FString>> ArrayString2D;
//	if( !ReadConfigString( ArrayString2D, TEXT("affix.txt"), true) )
//	{
//		warnf(NAME_Error, TEXT("can't open file %s"), TEXT("affix.txt") );
//		return FALSE;
//	}
//
//	_mapAffixs.clear();
//
//	UINT rowCnt = ArrayString2D.Num();
//	for( UINT i=0; i < rowCnt; ++i )
//	{
//		TArray<FString>& ArrayString = ArrayString2D(i);
//
//		if ( 0 == i )//检查标题行
//		{
//			continue;
//		}
//
//		if( ArrayString.Num() < 4 )
//		{
//			continue;
//		}
//
//		//UINT len = ArrayString.Num();
//		//if( len < 12 )
//		//{
//		//	warnf(NAME_Error, TEXT("======!!!!======= [read affix.txt ERROR]AT line %d"), i );
//		//	continue;
//		//}
//
//		//
//		AffixClientInfo affix;
//		affix.ID =appAtoi( tabFile->GetData( 1) );
//		if ( affix.ID > 255 || affix.ID < 0 )
//		{
//			warnf(NAME_Error, TEXT("======!!!!======= [invalid affix ID %d AT line %d"), affix.ID, i );
//			continue;
//		}
//
//		affix.Type	= appAtoi( *ArrayString(3) );
//		affix.Name = ArrayString(2) ;
//
//		_mapAffixs.insert( std::make_pair( affix.ID, affix ) );
//	}	
//
//	return TRUE;
//}
////------------------------------------------------------------------------
//AffixClientInfo* MMOFacade::FindAffix( INT ID )
//{
//	MapAffixID2Data::iterator it = _mapAffixs.find( ID );
//	if ( it == _mapAffixs.end() )
//	{
//		return NULL;
//	}
//
//	return &(it->second);
//}

FString MMOFacade::GetPlayerIcon( INT classId , INT sex )
{
	// valid player face icons: class0..class6 (needs the 1.05 MMOUIFaceIcon package for 5/6); npc (7) / none (8) have none
	if( classId < EPCT_PlayerClassA || classId > EPCT_PlayerClassH )
	{
		return TEXT("");
	}

	FString path = TEXT("img://MMOUIFaceIcon.Player.class");
	path += appItoa( classId );
	if( sex == EGT_Male )
	{
		path += TEXT("man");
	}
	else
	{
		path += TEXT("fe");
	}
	return path;
}
////------------------------------------------------------------------------
//void MMO::Facade::AddAffixTable( const FString& name , UAI_Affix_Table* node )
//{
//	INT lenth = name.Len();
//	INT levHigh = 0;
//	INT levLow = 0;
//
//	INT levHighPos = name.InStr( TEXT("_") , TRUE , FALSE ) + 1 ;	
//	FString levHighString = name.Mid( levHighPos );
//	levHigh = appAtoi( *levHighString );
//	
//	INT levLowPos = name.InStr( TEXT("_l") , TRUE , FALSE , levHighPos ) + 2 ;
//	FString levLowString = name.Mid( levLowPos );
//	levLow = appAtoi( *name.Mid( levLowPos ) );
//
//	INT monsterQuanPos = name.InStr( TEXT("_q") , TRUE , FALSE , levLowPos ) + 2 ;
//	INT monsterQuan = appAtoi( *name.Mid( monsterQuanPos ) );
//
//	INT sceneLevPos = name.InStr( TEXT("_s") , TRUE , FALSE , monsterQuanPos ) + 2 ;
//	INT sceneLev = appAtoi( *name.Mid( sceneLevPos ) );
//
//	for ( INT k = levLow ; k <= levHigh ; k++ )
//	{
//		FString newName = FString::Printf(TEXT("%d_%d_%d"),sceneLev,monsterQuan,k);
//		std::string nameString = (TCHAR_TO_ANSI(*newName));
//		AffixTemplateMap::const_iterator i = _affixTemplateMap.find( nameString );
//		if(i != _affixTemplateMap.end())
//		{
//			warnf( NAME_Error, TEXT("[Facade::AddAffixTable] AffixTable already exist:%d_%d_%d") , sceneLev , monsterQuan , k );
//		}
//		else
//		{			
//			_affixTemplateMap.insert(std::make_pair( nameString , node ));
//
//		}
//	}
//
//}

//------------------------------------------------------------------------
void MMOFacade::AddItemTemplate( INT k ,UItem* node)
{
	ItemTemplateMap::const_iterator i = _itemTemplateMap.find(k);
	if(i != _itemTemplateMap.end())
	{
		M_AddDataError(TEXT("item.txt"), FString::Printf(TEXT("道具重复:%d[%s]->[%s]"),
			k, *node->GetDisplayName(), *i->second->GetDisplayName()));
	}
	else
	{
		INT sz = _itemTemplateMap.size();
		_itemTemplateMap.insert(std::make_pair(k, node));
		//debugIDf(TEXT("TemplateData"), TEXT("=====[Facade::AddItemTemplate] Add item to ItemTemplateMap ID[%d] Name[%s]"), k, *node->GetDisplayName() );

		if(node->ItemType == TEXT("ITEMTYPE_INVALID"))
		{
			M_AddDataError(TEXT("item.txt"), FString::Printf(TEXT("item.txt"), TEXT("无效的道具类型:ID[%d] Name[%s]"), k, *node->GetDisplayName()));
		}
	} 
	MMOAuctionManager::getSingleton().AddAuctionItem(node);
}

//------------------------------------------------------------------------
void MMOFacade::ClearItemTemplates()
{
	ItemTemplateMap::iterator i = _itemTemplateMap.begin();
	for (; i != _itemTemplateMap.end(); ++i)
	{
		i->second->RemoveFromRoot();
	}

	_itemTemplateMap.clear();	
}

//------------------------------------------------------------------------

class UItem* MMOFacade::FindItemTemplate(UINT16 ContainerID, INT SlotID)
{
	ItemInfo* item = FindItem(ContainerID, SlotID);
	if(item)
	{
		return FindItemTemplate(item->m_nTemplateID);
	}

	return 0;
}

UItem* MMOFacade::FindItemTemplate(INT TemplateID)
{
	ItemTemplateMap::iterator it = _itemTemplateMap.find(TemplateID);
	if(it != _itemTemplateMap.end())
		return it->second;

	return NULL;
}
//=======================================================================
FString MMOFacade::GetItemIconNamePersonalized( UItem *pItem, const ItemInfo *pItemInfo /*= NULL*/, INT iconState /*= 0*/ )
{
	FString iconName;
	if( pItem == NULL ){
		return iconName;
	}

	AMMOActor* mainPlayer = GetMainActor();
	UEquip *pEquip = Cast<UEquip>(pItem);
	if(pEquip == NULL){
		//不是装备
		iconName = pItem->GetIconName( pItemInfo != NULL ? pItemInfo->m_nCurDur : 0 );
	}
	else{
		//是装备
		INT actorSex = ( mainPlayer != NULL ? mainPlayer->GetClassId() : 0 );
		actorSex = iconState != 0 ? iconState : actorSex;
		iconName = pItem->GetIconName( actorSex );
	}

	return iconName;
}
//=======================================================================
//------------------------------------------------------------------------
#ifndef _DEBUG
inline
#endif
	UINT32 HashItemKey(UINT16 ContainerTypeID, UINT16 SlotIdx)
{
	UINT32 k = ((UINT32)ContainerTypeID << 16);
	k |= SlotIdx;
	return k;
}
//------------------------------------------------------------------------
//size_t Facade::GetItemSize(ITEM_INFO* info)
//{
//	size_t sz = sizeof(ITEM_INFO);
//	if(info->byIsEquip)
//	{
//		sz += sizeof(EQUIP_INFO);
//		EQUIP_INFO* eqinfo = info->stEquipInfo;
//		//sz += eqinfo->byRefineCount * sizeof(UINT8);
//		//sz += eqinfo->byAttrCount * sizeof(ITEM_ATTR);
//	}
//
//	return sz;
//}
//------------------------------------------------------------------------
void MMOFacade::UpdateItemInfo(ItemInfo* src, FClientItemInfo* dest)
{
	appMemset(dest, 0, sizeof(ItemInfo)); // 不要把客户端的属性清除了!

	dest->m_nTemplateID = src->m_nTemplateID;
	dest->m_byConSlot = src->m_byConSlot;
	dest->m_byItemSlot = src->m_byItemSlot;
    if( dest->IsOnSale )
    {
        //如果上架了
        dest->OriDur = src->m_nCurDur;
	}
	dest->m_nCurDur = src->m_nCurDur;
	dest->m_nMaxDur = src->m_nMaxDur;
	dest->m_byIdentified = src->m_byIdentified;
	dest->m_byBindType = src->m_byBindType;
	dest->m_dwRoleID = src->m_dwRoleID;
	//if(src->byIsEquip)
	//{
	//	EQUIP_INFO* eqinfo = src->stEquipInfo;
		//dest->m_byPrefixLevel = eqinfo->byPrefixLevel;
		dest->m_byLuck = src->m_byLuck;
        dest->m_wRefineLevel = src->m_wRefineLevel;
		//dest->m_byAttrCount = eqinfo->byAttrCount; 

		dest->m_dwSocketCombination = src->m_dwSocketCombination;
		CopyMemory(&dest->m_nSoulID,src->m_nSoulID,sizeof(INT32) * MAX_SOCKET_COUNT);
        CopyMemory(&dest->m_wRandomAttrID,src->m_wRandomAttrID,sizeof(UINT16) * MAX_RANDOM_ATTR_COUNT);
		//铸魂
		CopyMemory(&dest->m_wRune,src->m_wRune,sizeof(UINT16) * MAX_RUNE_COUNT);
		//武器升级
		//dest->m_byLevel = src->m_byLevel;
		//dest->m_byLevelupPoint = src->m_byLevelupPoint;
		dest->m_byLevelupPhyPoint = src->m_byLevelupPhyPoint;
		dest->m_byLevelupMagPoint = src->m_byLevelupMagPoint;
		dest->m_byLevelupTaoPoint = src->m_byLevelupTaoPoint;

		//char* charPtr = ((char*)src->stEquipInfo) + sizeof(EQUIP_INFO);
		//if(dest->m_wRefineLevel > 0)
		//{
		//	UINT8* dataPtr = (UINT8*)charPtr;
		//	for (INT i = 0; i < dest->m_wRefineLevel; ++i)
		//	{
		//		dest->m_RefineTypes[i] = *(dataPtr + i);
		//	}

		//	charPtr += dest->m_wRefineLevel * sizeof(UINT8);
		//}

		//UItem *ItemClass = MMOFacade::getSingleton().FindItemTemplate( tEquip->ItemId );
		//if(ItemClass != NULL)
		//{
		//	dest->m_byAttrCount = ItemClass->AttributeList
		//}
		
		//ItemClass->
		/*
		if(dest->m_byAttrCount > 0)
		{
			ITEM_ATTR* attrPtr = (ITEM_ATTR*)charPtr;
			for (INT i = 0; i < dest->m_byAttrCount; ++i)
			{
				dest->m_attrs[i].shAttrType = (attrPtr + i)->shAttrType;
				dest->m_attrs[i].shParam = (attrPtr + i)->shParam;
				dest->m_attrs[i].byAttrColor = (attrPtr + i)->byAttrColor;
			}
		}
		*/
	//}

	// 成就系统
	MMOGameEvent_Achievement evtAch(adItemLuck, MMOEquipManager::getSingleton().CountEquipLuckTotal(), 0, 0);
	MMOUI::PostGameEvent(&evtAch);
}
//------------------------------------------------------------------------
void MMOFacade::UpdateItemDurability( INT conIdx, INT slotIdx, INT curDur )
{
	FClientItemInfo* equippedItem = FindItem( conIdx, slotIdx );
	if( equippedItem == NULL ){
		return;
	}
	UINT32 lastDur = equippedItem->m_nCurDur;
	equippedItem->m_nCurDur = curDur;

	switch( conIdx )
	{
	case E_CONSLOT_EQUIPPED:
		{//equip durability changed
			MMOGameEvent_Item itemEvt( GameEvent_EquipDurChanged, CONSLOT_EQUIPPED, slotIdx );
			MMOUI::PostGameEvent( &itemEvt );
		}
		break;

	case E_CONSLOT_DEPOSIT:
		{
			//只用经验瓶用，这里就不判物品类型了
			INT changedValue = equippedItem->m_nCurDur - lastDur;
			FString strHint = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.ExpContainer.FullfillHint") ), changedValue );
			MMOChatManager::getSingleton().AddSystemMessage( strHint );

			if( equippedItem->m_nCurDur >= equippedItem->m_nMaxDur ){
				FString errStrKey = TEXT("MMOGame.ExpContainer.IsFullHint");
				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errStrKey ), MsgInfoType_Table, errStrKey );

				//储值满了自动卸载
				UMMOUIBag *pBagPanel = (UMMOUIBag*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_Bag );
				if( pBagPanel != NULL ){
					pBagPanel->AutoUnloadCurExpContainer();
				}
			}

			MMOGameEvent_OneInt expContainerChg( GameEvent_ExpContainerChanged, changedValue );
			MMOUI::PostGameEvent( &expContainerChg );
		}
		break;

	default:
		break;
	}
	
}
//------------------------------------------------------------------------
FClientItemInfo* MMOFacade::AddItem( ItemInfo* iteminfo , UBOOL showNew ,UBOOL notify)
{
	if(iteminfo->m_byConSlot < E_CONSLOT_MAX )
	{
		ItemsMap& container = _Containers[iteminfo->m_byConSlot];
		ItemsMap::iterator i = container.find(iteminfo->m_byItemSlot);
		if(i == container.end())
		{
			FClientItemInfo* newInfo = new FClientItemInfo;
			//新获得物品标记 
			if( showNew && MMOUI::GetUIManager()->GetCurrentGameState() == EGameState_InGame &&  iteminfo->m_byConSlot < E_CONSLOT_COUNT )
			{
				//newInfo->IsNew = TRUE;
				newInfo->NeedHintMode |= ItemSlotHint_New;				
				//MMOUI::GetUIManager()->delegateHintNewItemLabel(iteminfo->m_byConSlot);
			}
			
			UpdateItemInfo(iteminfo, newInfo);
			container.insert(std::make_pair(iteminfo->m_byItemSlot, newInfo));

			debugf(TEXT("[Facade::AddItem] Add Item:[%d]-[%d]"), newInfo->m_byConSlot, newInfo->m_byItemSlot);

			if(notify)	
				NotifyItemChange(newInfo->m_byConSlot, newInfo->m_byItemSlot);

			UItem* item = FindItemTemplate(iteminfo->m_nTemplateID);
			if (item)
			{
				//add by JimZhang 2015.4.10 目前只有--------------------------------------
				Mir2Attack::getSingleton().UpdateItemSubType(newInfo, item);
			}
			return newInfo;
		}

		warnf(NAME_Error, TEXT("[Facade::AddItem] Item already exist:[%d]-[%d]"), iteminfo->m_byConSlot, iteminfo->m_byItemSlot);
		return i->second;
	}

	 return 0;
}

FClientItemInfo* MMOFacade::UpdateItem(ItemInfo* iteminfo)
{
	if(iteminfo->m_byConSlot < E_CONSLOT_MAX)
	{
		ItemsMap& container = _Containers[iteminfo->m_byConSlot];
		ItemsMap::iterator i = container.find(iteminfo->m_byItemSlot);
		if(i != container.end())
		{
			UpdateItemInfo(iteminfo, i->second);		
			NotifyItemChange(iteminfo->m_byConSlot, iteminfo->m_byItemSlot);
			return i->second;
		}
		else
		{
			return AddItem(iteminfo);
		}
	}

	warnf(NAME_Error, TEXT("[Facade::UpdateItem] Container exceed max count:[%d]"), iteminfo->m_byConSlot);

	return 0;
}

FClientItemInfo* MMOFacade::UpdateItemByContainerId(ItemInfo* iteminfo,UINT16 ContainerTypeID)
{
	if (ContainerTypeID < E_CONSLOT_MAX)
	{
		ItemsMap& container = _Containers[ContainerTypeID];
		ItemsMap::iterator i = container.find(iteminfo->m_byItemSlot);
		if(i != container.end())
		{
			UpdateItemInfo(iteminfo, i->second);
			//NotifyBagSlotChange(iteminfo->m_byConSlot, iteminfo->m_byItemSlot);
			return i->second;
		}
		else
		{
			return AddItem(iteminfo,FALSE,FALSE);
		}
	}
	return 0;
}

void MMOFacade::DeleteItem(UINT16 ContainerTypeID, UINT16 SlotIdx,UBOOL bNotify)
{
	if(ContainerTypeID < E_CONSLOT_MAX)
	{
		ItemsMap& container = _Containers[ContainerTypeID];
		ItemsMap::iterator i = container.find(SlotIdx);
		if(i != container.end())
		{
			
			delete i->second;
			container.erase(i);
			if (bNotify)
			{
				NotifyItemDelete(ContainerTypeID, SlotIdx);
			}
			
			return;
		}
	}

	warnf(NAME_Error, TEXT("[Facade::DeleteItem] Item not found:[%d]-[%d]"), ContainerTypeID, SlotIdx);
}

void MMOFacade::SwapItem(G2C_ITEM_SWAP& SwapInfo)
{
	if(SwapInfo.bySrcConSlot < E_CONSLOT_MAX
		&& SwapInfo.byDstConSlot < E_CONSLOT_MAX)
	{
		ItemsMap& containerFrom = _Containers[SwapInfo.bySrcConSlot];
		ItemsMap& containerTo = _Containers[SwapInfo.byDstConSlot];

		ItemsMap::iterator iFrom = containerFrom.find(SwapInfo.bySrcItemSlot);
		ItemsMap::iterator iTo = containerTo.find(SwapInfo.byDstItemSlot);
		
		if(iFrom != containerFrom.end() && iTo != containerTo.end())
		{
			// 交换
			std::swap(iFrom->second, iTo->second);
			
			iFrom->second->m_byConSlot = SwapInfo.bySrcConSlot;
			iFrom->second->m_byItemSlot = SwapInfo.bySrcItemSlot;

			iTo->second->m_byConSlot = SwapInfo.byDstConSlot;
			iTo->second->m_byItemSlot = SwapInfo.byDstItemSlot;

			NotifyItemChange(SwapInfo.bySrcConSlot, SwapInfo.bySrcItemSlot);
			NotifyItemChange(SwapInfo.byDstConSlot, SwapInfo.byDstItemSlot);
		}
		else if(iFrom != containerFrom.end() && iTo == containerTo.end())
		{
			// 移动（脱下）
			iTo = containerTo.insert(std::make_pair(SwapInfo.byDstItemSlot, iFrom->second)).first;
			
			iTo->second->m_byConSlot = SwapInfo.byDstConSlot;
			iTo->second->m_byItemSlot = SwapInfo.byDstItemSlot;

			containerFrom.erase(iFrom);

			// 原来的地方就删除了
			NotifyItemDelete(SwapInfo.bySrcConSlot, SwapInfo.bySrcItemSlot);

			NotifyItemChange(SwapInfo.byDstConSlot, SwapInfo.byDstItemSlot);
		}
		else // 不能从“空”换到“有” 或 “空”换“空”
		{
			warnf(NAME_Error, TEXT("[Facade::SwapItem] Source item not found:[%d]-[%d] -> [%d]-[%d]"),
				SwapInfo.bySrcConSlot, SwapInfo.bySrcItemSlot, SwapInfo.byDstConSlot, SwapInfo.byDstItemSlot);
		}
	}
	else
	{
		warnf(NAME_Error, TEXT("[Facade::SwapItem] Container exceed max count:[%d]-[%d] -> [%d]-[%d]"),
			SwapInfo.bySrcConSlot, SwapInfo.bySrcItemSlot, SwapInfo.byDstConSlot, SwapInfo.byDstItemSlot);
	}
}

void MMOFacade::ClearItems()
{
	for (INT i = 0; i < E_CONSLOT_MAX; ++i)
	{
		ClearItemsByContainerId(i);
	}
}

void MMOFacade::ClearItemsByContainerId(UINT16 ContainerTypeID)
{
	ItemsMap& container = _Containers[ContainerTypeID];
	ItemsMap::iterator it = container.begin();
	for (; it != container.end(); ++it)
	{
		delete it->second;
	}

	container.clear();
}
void MMOFacade::ClearPlayerEquip()
{
	AMMOPlayer* pMainPlayer =MMOFacade::getSingleton().GetMainPlayer();
	if(pMainPlayer && pMainPlayer->ItemManager)
	{
		for(INT i = 0; i < E_NES_TOTAL; ++i )
		{
			if(i!=E_NES_HEAD)
			{
				if(  pMainPlayer->ItemManager->EquippedItemList_Normal(i) != NULL )
				{
					pMainPlayer->ItemManager->EquippedItemList_Normal(i)->OnTakeOff( pMainPlayer );
					pMainPlayer->ItemManager->EquippedItemList_Normal(i) = NULL;
				}
			}
			//pMainPlayer->ItemManager->Reset();
		}
	}
}

//=================================================================================================================================
void MMOFacade::LockItem( INT ContainerID, INT SlotID, INT isToLockIn )
{
	FClientItemInfo *pItemInfo = FindItem( ContainerID, SlotID );

	if( pItemInfo != NULL ){
		pItemInfo->IsDragIntoOtherUI = isToLockIn;
	}

	//
	switch( ContainerID )
	{
	case E_CONSLOT_DEFAULT:
		{
			UMMOUIBag *pBagPanel = (UMMOUIBag*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Bag );
			if( pBagPanel != NULL ){
				pBagPanel->LockItem( ContainerID, SlotID, isToLockIn );
			}
		}
		break;
	case E_CONSLOT_RESOURCE:
		{
			UMMOUIBagResource *pBagResourcePanel = (UMMOUIBagResource*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_BagResource );
			if( pBagResourcePanel != NULL ){
				pBagResourcePanel->LockItem( ContainerID, SlotID, isToLockIn );
			}
		}
		break;
	case E_CONSLOT_EQUIPPED:
		{
			UMMOUICharacter *pCharactorPanel = (UMMOUICharacter*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Character );
			if( pCharactorPanel != NULL ){
				pCharactorPanel->LockItem( ContainerID, SlotID, isToLockIn );
			}
		}
		break;
	}
}
//=================================================================================================================================
//void MMOFacade::HintItem( INT ContainerID, INT SlotID, EItemSlotHint hintType )
//{
	//
//}
//=================================================================================================================================
void MMOFacade::GrayItem( ESlotGrayMark opPanelMaskEnum, INT containerID, INT slotID, UBOOL isToGray )
{
	FClientItemInfo *pItemInfo = FindItem( containerID, slotID );

	if( pItemInfo == NULL ){
		return;
	}

	//op data
	UINT32 maskEnum = (UINT32)opPanelMaskEnum;
	if( isToGray == TRUE ){
		pItemInfo->IsGrayMarkedInUI |= maskEnum;
	}
	else{
		pItemInfo->IsGrayMarkedInUI &= ~maskEnum;
	}

	//Op in UI
	switch( containerID )
	{
	case E_CONSLOT_DEFAULT:
		{
			UMMOUIBag *pBagPanel = (UMMOUIBag*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Bag );
			if( pBagPanel != NULL ){
				pBagPanel->GrayItem( containerID, slotID, pItemInfo );
			}
		}
		break;
	case E_CONSLOT_RESOURCE:
		{
			UMMOUIBagResource *pBagResourcePanel = (UMMOUIBagResource*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_BagResource );
			if( pBagResourcePanel != NULL ){
				pBagResourcePanel->GrayItem( containerID, slotID, pItemInfo );
			}
		}
		break;
	case E_CONSLOT_BANK:
		{
			UMMOUIWarehouse *pWarehousePanel = (UMMOUIWarehouse*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Warehouse );
			if( pWarehousePanel != NULL ){
				pWarehousePanel->GrayItem( containerID, slotID, pItemInfo );
			}
		}
		break;
	}
}

//=================================================================================================================================
FClientItemInfo* MMOFacade::FindItem(UINT16 ContainerTypeID, UINT16 SlotIdx) const
{
	if(ContainerTypeID < E_CONSLOT_MAX)
	{
		const ItemsMap& container = _Containers[ContainerTypeID];
		ItemsMap::const_iterator i = container.find(SlotIdx);
		if(i != container.end())
		{
			return i->second;
		}
	}
	else if( ContainerTypeID < E_CONSLOT_MAX + ESPType_Invalid )
	{
		FClientItemInfo* itemInfo = NULL;

		INT slotType = ContainerTypeID -= E_CONSLOT_MAX;
		switch( slotType )
		{
		case ESPType_ViewProfile:
			{
				PlayerDetailInfo* detailInfo = MMOPlayerInfoManager::getSingleton().FindCurPlayerDetailInfo();
				if(detailInfo && SlotIdx < E_NES_TOTAL && SlotIdx >= 0)
				{
					itemInfo = &detailInfo->EquipList[SlotIdx];
				}
			}
			break;
		case ESPType_ShopSell:
			{
				itemInfo = (FClientItemInfo*)FindSoldItem(SlotIdx);
			}
			break;
		case ESPType_AuctionItem:
			{
				itemInfo = (FClientItemInfo*)MMOAuctionManager::getSingleton().GetAuctionItemInfo(SlotIdx);
			}
			break;
		case ESPType_TeamAuctionList:
			{
				TArray<MMOTeamAuctionItem>& auctionItemList = TeamManager::getSingleton().GetTeamAuctionItemList();
				if( auctionItemList.Num() > SlotIdx )
				{
					itemInfo = (FClientItemInfo*)&auctionItemList(SlotIdx).Item;
				}	
			}
			break;
		case ESPType_TeamAuctionPanel:
			{
				MMOTeamAuctionItem* teamAuctionItem = TeamManager::getSingleton().GetCurAuctionItem();
				if(teamAuctionItem)
				{
					itemInfo = (FClientItemInfo*)&teamAuctionItem->Item;
				}
			}
			break;
		//case ESPType_GuildWarehouse:
		//	{
		//		itemInfo = MMOGuildData::getSingleton().GetWarehouseItem( 0, SlotIdx );
		//	}
		//	break;
		}

		return itemInfo;
	}
	//warnf(NAME_Error, TEXT("[Facade::FindItem] Item not found:[%d]-[%d]"), ContainerTypeID, SlotIdx);
	return 0;
}

FClientItemInfo* MMOFacade::FindFirstItemPos(UINT itemID , INT* outContainerTypeID , INT* outSlotIdx) const
{
	for ( INT i = 0 ; i < E_CONSLOT_MAX ; i++ )
	{
		const ItemsMap& container = _Containers[i];
		ItemsMap::const_iterator itor = container.begin();
		for ( ; itor != container.end() ; itor++ )
		{
			if( itor->second->m_nTemplateID == itemID )
			{
				*outContainerTypeID = i;
				*outSlotIdx = itor->first;
				return itor->second;
			}
		}
	}

	return NULL;
}

FClientItemInfo* MMOFacade::FindFirstItemPosInContainer( UINT itemID, INT containerTypeID, INT& outSlotIdx ) const
{
	outSlotIdx = -1;
	if( containerTypeID < 0 || E_CONSLOT_MAX <= containerTypeID ){
		return NULL;
	}
	
	const ItemsMap& itemContainer = GetContainer( (ECONTAINER_SLOT)containerTypeID );
	ItemsMap::const_iterator itor = itemContainer.begin();
	for ( ; itor != itemContainer.end() ; itor++ )
	{
		if( itor->second->m_nTemplateID == itemID ){
			outSlotIdx = itor->first;
			return itor->second;
		}
	}

	return NULL;
}

bool MMOFacade::IsItemExist(UINT16 ContainerTypeID, UINT16 SlotIdx) const
{
	if(ContainerTypeID < E_CONSLOT_MAX)
	{
		const ItemsMap& container = _Containers[ContainerTypeID];
		ItemsMap::const_iterator i = container.find(SlotIdx);
		if(i != container.end())
		{
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------------------
INT MMOFacade::GetItemCount(INT itemID)
{
	INT iResult = 0;
	AMMOPlayer *pMainPlayer = GetMainPlayer();
	UINT uiBagCap = pMainPlayer->ItemManager->CurCapacity( E_CONSLOT_DEFAULT );
	ItemInfo* pItemInfo = NULL;

	for( UINT j = 0; j < uiBagCap; ++j)
	{
		pItemInfo = FindItem( E_CONSLOT_DEFAULT, j );
		if( pItemInfo != NULL && pItemInfo->m_nTemplateID == itemID )
		{
			UItem* itemTemp = FindItemTemplate( itemID );
			if( itemTemp )
			{
				continue;
			}

			iResult += pItemInfo->GetCurStackCount( itemTemp );
		}
	}

	return iResult;
}
//-----------------------------------------------------------------------------------------
void MMOFacade::NotifyItemChange( UINT16 ContainerTypeID, UINT16 SlotIdx )
{
	UIHudProcess( ItemChange( ContainerTypeID, SlotIdx ) );
}

FString g_PetNameColor = TEXT("#0000FF");

void MMOFacade::UpdatePetLevelColor(AMMOActor* actor)
{
	if(actor && actor->ActorAttributes)
	{
		if(actor->ActorAttributes->PetLevel==0){
			actor->ActorAttributes->ColorEx1=TEXT("#FFFFFF");
		}
		else if(actor->ActorAttributes->PetLevel==1){
			actor->ActorAttributes->ColorEx1=TEXT("#00FFFF");
		}
		else if(actor->ActorAttributes->PetLevel==2){
			actor->ActorAttributes->ColorEx1=TEXT("#8CD6EF");
		}
		else if(actor->ActorAttributes->PetLevel==3){
			actor->ActorAttributes->ColorEx1=TEXT("#39B5EF");
		}
		else if(actor->ActorAttributes->PetLevel==4){
			actor->ActorAttributes->ColorEx1=TEXT("#7788FF");
		}
		else if(actor->ActorAttributes->PetLevel==5){
			actor->ActorAttributes->ColorEx1=TEXT("#007BDE");
		}
		else if(actor->ActorAttributes->PetLevel==6){
			actor->ActorAttributes->ColorEx1=TEXT("#104ADE");
		}
		else if(actor->ActorAttributes->PetLevel>=7){
			actor->ActorAttributes->ColorEx1 = g_PetNameColor;
		}
		else {
			actor->ActorAttributes->ColorEx1=TEXT("");
		}		
	}
}

void MMOFacade::ClearMainPlayer()
{
	AMMOPlayer* MainActor = GetMainPlayer();
	if(MainActor && MainActor->ItemManager){
		MainActor->ItemManager->Reset();
	}
}

void MMOFacade::ClearSoldItems()
{
	for (INT i = 0; i < _SoldItemList.Num(); ++i)
	{
		delete _SoldItemList(i);
	}

	_SoldItemList.Empty();
}

void MMOFacade::AddSoldItem(ItemInfo* info)
{	
	FClientItemInfo* newInfo = new FClientItemInfo;	
	UpdateItemInfo(info, newInfo);
	MMOPowerManager::getSingleton().UpdateEquipPower(newInfo);
	_SoldItemList.AddItem(newInfo);

	debugf(TEXT("[Facade::UpdateOrAddSoldItem] [%d]"), _SoldItemList.Num() - 1);

	// 目前是一次性全通知过来
	//NotifySoldItemChange(_SoldItemList.Num() - 1);
}

struct ItemInfo* MMOFacade::FindSoldItem(INT SlotID) const
{
	if(SlotID < _SoldItemList.Num())
	{
		return _SoldItemList(SlotID);
	}

	warnf(NAME_Error, TEXT("[Facade::FindSoldItem] Sold Item not found index:[%d]"), SlotID);

	return 0;
}

INT MMOFacade::GetSoldItemCount() const
{
	return _SoldItemList.Num();
}

void MMOFacade::NotifySoldItemChange()
{
	UIHudProcess( SoldItemChange() );
}

INT MMOFacade::GetUsedSlotCountInBag(UINT16 containerID) const
{
	const ItemsMap& container = _Containers[containerID];
	ItemsMap::const_iterator it = container.begin();
	//for (; it != container.end(); ++it)
	//{
	//	const FClientItemInfo* itemInfo = it->second;
	//	
	//}
	return container.size();
}

UBOOL MMOFacade::GetItemDurInBag( UINT itemID, UINT itemSubType, INT32 RequireDur )
{
	const ItemsMap& container = _Containers[E_CONSLOT_DEFAULT];
	if (container.size() <= 0) return FALSE;

	ItemsMap::const_iterator it = container.begin();
	for (; it != container.end(); ++it)
	{
		const FClientItemInfo* itemInfo = it->second;
		if(itemInfo->ItemSubTypeNum==itemSubType || itemInfo->m_nTemplateID==itemID)
		{
			if(itemInfo->m_nCurDur>=RequireDur)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

INT MMOFacade::GetItemCountInBag(UINT itemID)
{
	INT ret = 0;
	
	for (INT i = E_CONSLOT_EQUIPPED; i < E_CONSLOT_BANK; ++i)
	{
		ret += GetItemCountInBag(itemID, i);
	}

	return ret;
}

INT MMOFacade::GetItemCountInBag( UINT itemID, UINT16 containerID )
{
	INT ret = 0;
	const ItemsMap& container = _Containers[containerID];

	if (container.size() <= 0)
		return ret;

	ItemsMap::const_iterator it = container.begin();
	for (; it != container.end(); ++it)
	{
		const FClientItemInfo* itemInfo = it->second;
		if(itemInfo->m_nTemplateID == itemID)
		{
			UItem* itemTemp = FindItemTemplate(itemID);
			if(!itemTemp)
			{
				++ret;
				continue;
			}

			/*switch(itemTemp->ItemDuration)
			{
			case E_DURTYPE_STACK:
				{
					ret += itemInfo->m_nCurDur;
				}
				break;

			default:
				{
					++ret;
				}
			}*/
			ret += itemInfo->GetCurStackCount(itemTemp);
		}
	}

	return ret;
}

INT MMOFacade::GetEmptyItemSlot(ECONTAINER_SLOT ContainerID) const
{
	UINT16 sz = GetContainerSize(ContainerID);
	TArray<UBOOL> TestUsed;
	TestUsed.AddZeroed(sz);

	const ItemsMap& container = _Containers[ContainerID];

	ItemsMap::const_iterator it = container.begin();
	for (; it != container.end(); ++it)
	{
		if(it->first < sz)
		{
			TestUsed(it->first) = TRUE;			
		}
	}

	// 找到第一个没占用的
	for (UINT16 i = 0; i < sz; ++i)
	{
		if(TestUsed(i) == FALSE)
		{
			return i;
		}
	}

	return -1;	
}

INT MMOFacade::GetCanStackSlot( ECONTAINER_SLOT ContainerID , INT itemId , INT stackCount ) const
{
	const ItemsMap& container = _Containers[ContainerID];

	ItemsMap::const_iterator it = container.begin();
	for (; it != container.end(); ++it)
	{
		FClientItemInfo* item = it->second;
		if( item && item->m_nTemplateID == itemId && item->m_nCurDur + stackCount <= item->m_nMaxDur )
		{
			return item->m_byItemSlot;
		}
	}
	return -1;
}

UBOOL MMOFacade::IsItemPack(UINT itemPack)
{
	UBOOL res = FALSE;
	INT count = 0;

	for (INT i = E_CONSLOT_EQUIPPED; i < E_CONSLOT_BANK; ++i)
	{
		count = GetItemCountInBag(itemPack,i);
	}
	if (count > 0)
		res = TRUE;

	return res;
}

//------------------------------------------------------------------------
USoundCue* MMOFacade::GetHitSound( INT key )  const
{
	HitSoundGroupMap::const_iterator i = HitSoundGrpMap.find(key);
	if(i != HitSoundGrpMap.end())
	{
		return i->second;
	}

	return 0;
}
//--------------------------------------------hemu@snda

void MMOFacade::StopVoiceSound(const FString& SoundName, BOOL Immediately, FLOAT FadeOutDuration, FLOAT FadeVolumeLevel)
{
	if (!GEngine)
	{
		return;
	}

	UAudioDevice* audioDevice = GEngine->GetAudioDevice();
	if (!audioDevice)
	{
		return;
	}

	for (INT i = 0; i < audioDevice->AudioComponents.Num(); ++ i)
	{
		UAudioComponent* sound = audioDevice->AudioComponents(i);
		if (sound && 
			sound->IsPlaying() && 
			sound->SoundCue &&
			(sound->SoundCue->SoundClass == FName(VOLUME_CONTROL_VOICE) || sound->SoundCue->SoundClass == FName(VOLUME_CONTROL_VOICE_SPEECH)))
		{
			if (sound->SoundCue->GetPathName() == SoundName)
			{
				if (Immediately)
				{
					sound->Cleanup();
				}
				else
				{
					sound->FadeOut(FadeOutDuration, FadeVolumeLevel);
				}
				break;
			}
		}
	}
}

void MMOFacade::StopAllVoiceSound(BOOL Immediately, FLOAT FadeOutDuration, FLOAT FadeVolumeLevel)
{
	if (!GEngine)
	{
		return;
	}

	UAudioDevice* audioDevice = GEngine->GetAudioDevice();
	if (!audioDevice)
	{
		return;
	}

	for (INT i = 0; i < audioDevice->AudioComponents.Num(); ++ i)
	{
		UAudioComponent* sound = audioDevice->AudioComponents(i);
		if (sound &&
			sound->IsPlaying() &&
			sound->SoundCue &&
			(sound->SoundCue->SoundClass == FName(VOLUME_CONTROL_VOICE) || sound->SoundCue->SoundClass == FName(VOLUME_CONTROL_VOICE_SPEECH)))
		{
			if (Immediately)
			{
				sound->Cleanup();
			}
			else
			{
				sound->FadeOut(FadeOutDuration, FadeVolumeLevel);
			}
		}
	}
}

void MMOFacade::AddAnimMix(INT key, FAnimationNode* node)
{
	if (node != NULL)
	{
		AnimMixGrpMap.insert(make_pair(key, node));
	}
}

void MMOFacade::ClearAnimMix()
{
	AnimMixGrpMap.clear();
}

FAnimationNode* MMOFacade::GetAnimMix(INT key)
{
	AnimMixGruopMap::iterator pos = AnimMixGrpMap.find(key);
	if (pos != AnimMixGrpMap.end())
	{
		return pos->second;
	}
	return NULL;
}
//------------------------------------------------------------------------
extern GRID_DATA* g_pBufGrid;
void    MMOFacade::SetBlockCheck(FVector CheckPos)
{
	if(g_pBufGrid)
	{
		m_BlockPosition = CheckPos;
		m_BlockCheck = TRUE;
		m_BlockCheckRes = 0;
		m_pBufGrid = g_pBufGrid;
	}
}

void    MMOFacade::RunBlockCheck(AMMOActor* pBlockActor)
{
	if(m_BlockCheck && pBlockActor)
	{
		if (pBlockActor->CurBlockType>0 || pBlockActor->GetActorType() == EACT_Boss)
		{
			FLOAT Scope = (pBlockActor->CurBlockType>1)?40000.0:4096.0;//考虑到3*3的阻挡物
			FVector def = pBlockActor->ServerLocation - m_BlockPosition;
			FLOAT Distance2 = def.SizeSquared2D();
			if( Distance2 > Scope )return; // 64*64
			if( def.Z*def.Z > 40000.0 )return;  //    16384.0 )return;  //deltaZ > 128;
			m_BlockCheckRes++;
		}
	}
}

void    MMOFacade::DoBlockCheck()
{
	if(m_BlockCheck)
	{
		if(m_BlockCheckRes==0)//发现是没有阻挡的npc和player,则进一步加以判定
		{
			gServerGridWorld.RemoveAllActor(m_pBufGrid);
			
		}
		m_BlockCheck = FALSE;
		m_pBufGrid = NULL;
		m_BlockCheckRes = 0;
	}
}

//------------------------------------------------------------------------
void MMOFacade::AddBuffTemplate(INT Id, INT Level, class UBuff* BuffTemplate)
{
	if (!BuffTemplate)
	{
		return;
	}
	INT key = Id<<16;
	key += Level;

	Int2BuffMap::iterator pos = BuffTemplateMap.find(key);
	if (pos == BuffTemplateMap.end())
	{
		BuffTemplateMap.insert(make_pair(key, BuffTemplate));
	}
	else
	{
		warnf(NAME_Error, TEXT("Facade : Two BuffTemplates are the same! Id=%d, Level=%d"), Id, Level);
	}

}

void MMOFacade::ClearBuffTemplates()
{
	BuffTemplateMap.empty();
}

class UBuff* MMOFacade::FindBuffTemplate(INT Id, INT Level)
{
	Int2BuffMap::iterator pos = BuffTemplateMap.find(Id);
	if (pos != BuffTemplateMap.end())
	{
		return pos->second;
	}
	else
	{
		BuffTemplateDataIndex* RetDatat = BuffTemplateDataIndexManager::getSingleton().FindAndModifyData(Id);
		UBuff* Ret = NULL;
		if (RetDatat)
		{
			FString PathName = RetDatat->ObjectPath;
			Ret = Cast<UBuff>(UObject::StaticLoadObject(UBuff::StaticClass(), NULL, *PathName, NULL, LOAD_None, NULL));
		}
		if (!Ret)
		{
			warnf(TEXT("Can not found buff template by id=%d"), Id);
		}
		else
		{
			Ret->AddToRoot();
			BuffTemplateMap.insert(make_pair(Id, Ret));
		}
		return Ret;
	}
	
	//INT key = Id<<16;
	//key += Level;
	//Int2BuffMap::iterator pos = BuffTemplateMap.find(key);
	//if (pos != BuffTemplateMap.end())
	//{
	//	return pos->second;
	//}
	//else
	//{
	//	return NULL;
	//}
}

void MMO::FormatTempleteDataLoadingDir( TArray<FString>& dirArray ) 
{
	if(!GUseSeekFreeLoading)
	{
		for (INT i = 0; i < dirArray.Num(); ++i)
		{
			dirArray(i) = FString(TEXT("..\\..\\MMOGame\\Content\\")) + dirArray(i);
		}
	}
	else
	{
		for (INT i = 0; i < dirArray.Num(); ++i)
		{
			dirArray(i) = FString(TEXT("..\\..\\MMOGame\\CookedPC\\")) + dirArray(i);
		}
	}
}

FString MMO::FormatTempleteDataLoadingDir( const FString& srcDirName )
{
	if(!GUseSeekFreeLoading)
	{
		return FString(TEXT("..\\..\\MMOGame\\Content\\")) + srcDirName;
	}
	else
	{
		return FString(TEXT("..\\..\\MMOGame\\CookedPC\\")) + srcDirName;
	}
}


FString NumberToString(UINT num, UINT transfer)
{
	FString ret;
	ret.Empty();
	if(num == 0)
	{
		ret = TEXT("0");
		return ret;
	}

	TArray<TCHAR, FStringAllocator> StringChars;
	StringChars.Empty();
	while(num != 0)
	{
		UINT ch = num % transfer;
		if(ch >= 10)
		{
			TCHAR t = ch - 10 + TCHAR('A');
			StringChars.AddItem(t);
		}
		else
		{
			TCHAR t = ch + 48;
			StringChars.AddItem(t);
		}

		num = num / transfer;
	}

	while(StringChars.Num() > 0)
	{
		ret.AppendChar(StringChars.Pop());
	}
	return ret;
}

UINT IntPow(UINT x, UINT y)
{
	UINT ret = 1;
	for(UINT i = 0; i < y; ++i)
	{
		ret *= x;
	}
	return ret;
}

UINT StringToNumber(FString str, UINT transfer)
{
	UINT ret = 0;
	const TArray<TCHAR, FStringAllocator>& CharArray = str.GetCharArray();
	INT length = CharArray.Num() - 1;
	for(INT i = 0; i < CharArray.Num() - 1; ++i)
	{
		TCHAR t = CharArray(i);
		if(t >= TCHAR('A'))
		{
			ret += (t - TCHAR('A') + 10) * IntPow(transfer,length - 1);
		}
		else
		{
			ret += (t - 48) * IntPow(transfer,length - 1);
		}

		--length;
	}
	return ret;
}

//------------------------------------------------------------------------

void MMOFacade::EmptyMountList()
{
	if(GetMainPlayer())
	{
		GetMainPlayer()->ActorAttributes->MountList.Empty();
	}	
}

void MMOFacade::AddMountList(INT MountID)
{
	if(GetMainPlayer())
	{
		GetMainPlayer()->ActorAttributes->MountList.Push(MountID);

		// 成就
		MMOGameEvent_Achievement evtAch1(adSet, MountID, 0, 0);
		MMOUI::PostGameEvent(&evtAch1);

		MMOGameEvent_Achievement evtAch2(adSetCount, GetMainPlayer()->ActorAttributes->MountList.Num(), 0, 0);
		MMOUI::PostGameEvent(&evtAch2);
	}	
}

void MMOFacade::RemoveMountList(INT MountID)
{
	if(GetMainPlayer() )
	{
		UActorAttribute* p = GetMainPlayer()->ActorAttributes;
		if(p)
		{
			if(MountID<=0)
			{
				p->MountList.Empty();
				return;
			}

			for(int i=0; i < p->MountList.Num(); ++i )
			{
				if(p->MountList(i) == MountID)
				{
					p->MountList.Remove(i);
					return;
				}
			}
		}
	}	
}

void MMOFacade::EquipMount(UINT CharID,INT MountID)
{
	AMMOActor* pActor = GetCharacter(CharID);
	if(pActor)
	{
		if(MountID<=0)
		{
			pActor->UnEquipMount();
		}
		else
		{
			pActor->EquipMount(MountID);
		}
	}

}



//------------------------------------------------------------------------
void MMOFacade::SetMyCurrentHP( INT CurrentHP )
{
	if(!GetMainPlayer() || !GetMainPlayer()->ActorAttributes)
	{
		return;
	}
	if(GetMainPlayer()->ActorAttributes->GetCurHp() != CurrentHP)
	{
		GetMainPlayer()->ActorAttributes->HpPercent = CurrentHP*100/ GetMainPlayer()->ActorAttributes->GetMaxHp();

		GetMainPlayer()->ActorAttributes->SetHP(CurrentHP);// = CurrentHP;
		NotifyMyHPChange();

		if(GetMainPlayer()->ActorAttributes->GetCurHp()<10)
		{
			FSMIO::getSingleton().Ctrl.SetAlwaysWalkBase(TRUE);
		}
		else
		{
			FSMIO::getSingleton().Ctrl.SetAlwaysWalkBase(FALSE);
		}
	}
}
//------------------------------------------------------------------------
void MMOFacade::SetMyCurrentMaxHP( INT MaxHP )
{
	if(!GetMainPlayer() || !GetMainPlayer()->ActorAttributes)
	{
		return;
	}


	GetMainPlayer()->ActorAttributes->MaxHP = MaxHP;
	//NotifyMyHPChange();

}
//------------------------------------------------------------------------
void MMOFacade::SetMyCurrentMP( INT CurrentMP )
{
	if(!GetMainPlayer() || !GetMainPlayer()->ActorAttributes)
	{
		return;
	}
	//被默认初始化为0
	//if( GetMainPlayer()->ActorAttributes->MP != CurrentMP )
	//{
	GetMainPlayer()->ActorAttributes->MP = CurrentMP;
	NotifyMyMPChange();
	//}
}
//------------------------------------------------------------------------
void MMOFacade::SetMyMaxMP( INT MaxMP )
{
	if(!GetMainPlayer() || !GetMainPlayer()->ActorAttributes)
	{
		return;
	}

	GetMainPlayer()->ActorAttributes->SetMaxMP(MaxMP);
	//NotifyMyMPChange();
}
//------------------------------------------------------------------------
void MMOFacade::SetMyLevel(INT level)
{
	if(!GetMainPlayer() || !GetMainPlayer()->ActorAttributes)
	{
		return;
	}
	INT lastLevel = GetMainPlayer()->GetActorLevel();
	GetMainPlayer()->SetActorLevel( level );
	NotifyPlayerLevelChange( lastLevel , level );
	MMOPowerManager::getSingleton().UpdatePartPower( PowerPart_CharacterBasePow );
	MMOHeavenManager::getSingleton().NotifyLevelChange(level);

	MMOQuestManager::getSingleton().NotifyPlayerLevelChange(level);

	MasterStudentManager::getSingleton().NotifyLevelChange(level);
	
	// 引导
	MMOGameEvent_Guide evtGuide(EGUET_LevelChange, level);
	MMOUI::PostGameEvent(&evtGuide);

	MMOSystemActivateManager::getSingleton().NotifyPlayerLevelChange(level);

	//境界power
	MMOPowerManager::getSingleton().UpdatePartPower(PowerPart_HeavenPow);

	// 成就
	MMOAchievementManager::getSingleton().NotifyPlayerLevelChange(lastLevel, level);
	MMOJourneyManager::getSingleton().NotifyPlayerLevelChange(lastLevel, level);

	//
	if( level >= MMODailySignInManager::SIGN_IN_LV_REQUIRE ){
		MMODailySignInManager::getSingleton().CheckHintForSignIn();
	}

	//
	MMOVipManager::getSingleton().NotifyPlayerLevelChange(level);
}
//
INT MMOFacade::GetMyLevel()
{
	if(!GetMainPlayer() || !GetMainPlayer()->ActorAttributes)
	{
		return 0;
	}
	return GetMainPlayer()->ActorAttributes->LevelAttributes.Level;
}

void MMOFacade::SetMyPkValue(INT PkValue)
{
	if(!GetMainPlayer() || !GetMainPlayer()->ActorAttributes)
	{
		return;
	}
	GetMainPlayer()->ActorAttributes->SetPkValue(PkValue);
	NotifyPlayerPkValueChange();
}

INT   MMOFacade::GetPkValueLevel(INT PkValue)
{
	INT PkLevel=0;
	for(int i=0;i<vecPkLevel.Num();i++)
	{
		if(PkValue<vecPkLevel(i))
		{
			PkLevel = i;
			break;
		}
		else
		{
			PkLevel = i+1;
		}
	}
	return PkLevel;
}

UBOOL MMOFacade::IsSamePkLevel(INT PkValue1,INT PkValue2)
{
	INT PkLevel1=GetPkValueLevel(PkValue1);
	INT PkLevel2=GetPkValueLevel(PkValue2);

	if(PkLevel1==PkLevel2)return TRUE;
	return FALSE;
}

INT MMOFacade::GetMyPkValue()
{
	if(!GetMainPlayer() || !GetMainPlayer()->ActorAttributes)
	{
		return 0;
	}
	return GetMainPlayer()->ActorAttributes->GetPkValue();
}

void MMOFacade::SetMyPkMode(INT PkMode)
{
	if(!GetMainPlayer() || !GetMainPlayer()->ActorAttributes)
	{
		return;
	}
	GetMainPlayer()->ActorAttributes->PkMode=PkMode;
	NotifyPlayerPkModeChange();

	if(FSM::Target.SelectMMOActor)
	{
		AMMOPlayerController* pCtrl = MMOFacade::getSingleton().GetMainController();
		if(pCtrl)
		{
// 			if(pCtrl->MyDecalManager)
// 			{
// 				pCtrl->MyDecalManager->eventSelectActor(NULL);
// 			}
			pCtrl->SetSelectTarget(FSM::Target.SelectMMOActor);
		}
	}
}
INT MMOFacade::GetMyPkMode()
{
	if(!GetMainPlayer() || !GetMainPlayer()->ActorAttributes)
	{
		return 0;
	}
	return GetMainPlayer()->ActorAttributes->PkMode;
}

void MMOFacade::RefleshAllHints()
{
	UMMOUISceneHint* pPanel = Cast<UMMOUISceneHint>(MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_SceneHint));
	if (pPanel)
	{
		pPanel->RefleshAllHints();
	}
}

void MMOFacade::RefleshAllNPCHints()
{
	UMMOUISceneHint* pPanel = Cast<UMMOUISceneHint>(MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_SceneHint));
	if (pPanel)
	{
		pPanel->RefleshAllNPCHints();
	}
}


//------------------------------------------------------------------------
void MMOFacade::NotifyPlayerTitleChange()
{
	//MMOUI::GetUIManager()->delegateUpdateHintText(GetMainPlayer());
	UActorAttribute* attri = GetMainPlayer()->ActorAttributes;
	UIHudProcess( PlayerTitleChange( attri->TitleID ));
}
//------------------------------------------------------------------------
void MMOFacade::NotifyOtherPlayerTitleChange(AMMOPlayer* player , INT TitleID)
{
	//MMOUI::GetUIManager()->delegateUpdateHintText(player);
	UIHudProcess( OtherPlayerTitleChange(player, TitleID ));
}
//------------------------------------------------------------------------
void MMOFacade::NotifyPlayerPkModeChange()
{
	UActorAttribute* attri = GetMainPlayer()->ActorAttributes;
	UIHudProcess( PlayerPkModeChange( attri->PkMode ));
}
//------------------------------------------------------------------------
void MMOFacade::NotifyOtherPlayerPkModeChange(AMMOPlayer* player, INT PkMode)
{
	//老崩在这里，并没操作
	//UIHudProcess( OtherPlayerPkModeChange( player,PkMode ));
}
//------------------------------------------------------------------------
void MMOFacade::NotifyPlayerPkValueChange()
{
	//MMOUI::GetUIManager()->delegateUpdateHintText(GetMainPlayer());
	UIHudProcess( PlayerPkValueChange(GetMainPlayer()->ActorAttributes->PkValue));

	// 成就
	MMOGameEvent_Achievement evtAch(adPkValue, GetMainPlayer()->ActorAttributes->PkValue, 0, 0);
	MMOUI::PostGameEvent(&evtAch);
}
//------------------------------------------------------------------------
void MMOFacade::NotifyOtherPlayerPkValueChange(AMMOPlayer* player, INT PkValue)
{
	//MMOUI::GetUIManager()->delegateUpdateHintText(player);
	UIHudProcess( OtherPlayerPkValueChange(player,PkValue));
}
//------------------------------------------------------------------------
void MMOFacade::NotifyPlayerLevelChange( INT lastLevel , INT newLevel )
{
	//UActorAttribute* attri = GetMainPlayer()->ActorAttributes;
	UIHudProcess( PlayerLevelChange( lastLevel , newLevel ));
}
//------------------------------------------------------------------------
void MMOFacade::NotifyOptionChange()
{
	UIHudProcess( PlayerLevelChange( 0 , GetMainPlayer()->ActorAttributes->LevelAttributes.Level ));
}
//------------------------------------------------------------------------
void MMOFacade::NotifyPlayerBaseInfoChange()
{
	
	UIHudProcess( PlayerBaseInfoChange() );
}
//------------------------------------------------------------------------
void MMOFacade::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	switch(evt->GetEventType())
	{
	case GameEvent_RequestMainPlayerData:
		{
			NotifyMyHPChange();
			NotifyMyMPChange();
		}
		break;
	case GameEvent_EnterGameSuccess:
		{
			AMMOPlayer* mainPlayer = GetMainPlayer();
			if( mainPlayer != NULL ){
				mainPlayer->SetInFighting( FALSE );
			}
		}
		break;
	case GameEVent_SceneLoadFinish:
		{
			GameVerAnnounceProcess();
		}
		break;
	//case GameEvent_ExitMsg:
	//	{
	//		MMOGameEvent_MsgboxCallBack* cb = (MMOGameEvent_MsgboxCallBack*)(evt);
	//		if(cb)
	//		{
	//			if( cb->Agree )
	//			{
	//				AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	//				if(ctrl)
	//				{
	//					ctrl->ConsoleCommand( TEXT("exit") );
	//				}

	//			}

	//		}
	//	}
	//	break;
	}

}

void MMOFacade::NotifyMyHPChange()
{

	UActorAttribute* attri = GetMainPlayer()->ActorAttributes;

	MMOGameEvent_MainPlayerDataChange evt(MMOGameEvent_MainPlayerDataChange::EMSET_HP,
		GetMainPlayer(), attri->HP, attri->GetMaxHp());

	MMOUI::PostGameEvent(&evt);

	//UIHudProcess( PlayerHPChange(attri->HP, attri->GetMaxHp() ) );

}
//------------------------------------------------------------------------
void MMOFacade::NotifyMyMPChange()
{

	UActorAttribute* attri = GetMainPlayer()->ActorAttributes;

	MMOGameEvent_MainPlayerDataChange evt(MMOGameEvent_MainPlayerDataChange::EMSET_MP,
		GetMainPlayer(), attri->GetCurMp(), attri->GetMaxMp());

	MMOUI::PostGameEvent(&evt);

// 	for(UIMaps::TIterator PairIt(AllUIMaps); PairIt; ++PairIt)
// 	{ 
// 		if(PairIt.Value())
// 		{
// 			PairIt.Value()->PlayerMPChange(attri->MP, attri->GetMaxMp()); 
// 		}
// 	}
	//UIHudProcess(  PlayerMPChange(attri->MP, attri->GetMaxMp()) );

}
//------------------------------------------------------------------------
void MMOFacade::NotifyShutDown()
{
	UIHudProcess( OnShutDown());
}

INT MMOFacade::GetMyCurrentHP() const
{
	if(!GetMainPlayer() || !GetMainPlayer()->ActorAttributes)
	{
		return 0;
	}

	return GetMainPlayer()->ActorAttributes->GetCurHp();
}

INT MMOFacade::GetMyMaxHP() const
{
	if(!GetMainPlayer() || !GetMainPlayer()->ActorAttributes)
	{
		return 0;
	}

	return GetMainPlayer()->ActorAttributes->MaxHP;
}

INT MMOFacade::GetMyCurrentMP() const
{
	if(!GetMainPlayer() || !GetMainPlayer()->ActorAttributes)
	{
		return 0;
	}

	return GetMainPlayer()->ActorAttributes->MP;
}

INT MMOFacade::GetMyMaxMP() const
{
	if(!GetMainPlayer() || !GetMainPlayer()->ActorAttributes)
	{
		return 0;
	}

	return GetMainPlayer()->ActorAttributes->GetMaxMp();
}

UBOOL MMOFacade::CheckEnoughMoney( EMONEY_TYPE moneyType , INT needMoney )
{
	INT money =	GetPlayerMoney(moneyType);

	if (moneyType == E_MONEYTYPE_MORAL )
	{
		if(needMoney > 0)
		{
			needMoney = needMoney /  MMOFacade::getSingleton().GetMoneyRatio();
		}
	}
	if( money < needMoney )
	{
		if( moneyType == E_MONEYTYPE_01 )
		{
			UBOOL selected = MMOFacade::getSingleton().GetChainPay();
			if(!selected)
			{
				//MMOMsgBoxManager::getSingleton().AddOpenChainPayMessageBox();
				//MMOMsgBoxManager::getSingleton().AddModalMessageBoxEvent( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.AskOpenChainPay")) , 0 , GameEvent_OpenChainPay );
			}
			else
			{
				//绑金不足可以尝试让服务器判断连带交易
				return TRUE;
			}		
		}

		FString str = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Shop.MoneyNotEnough") ) , *UITextParser::HTMLGetCurrencyName((MONEY_TYPE)moneyType) );
		MMOUI::GetUIManager()->delegateAddScreenMessage( str , MsgInfoType_Table , TEXT("MMOGame.Shop.MoneyNotEnough") );
		
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


UBOOL MMOFacade::IsMainPlayerTrading()
{
	AMMOPlayer *mainPlayer = GetMainPlayer();
	if( mainPlayer == NULL ){
		return FALSE;
	}

	UBOOL bResult = mainPlayer->IsInTrading();
	if( bResult == TRUE ){
		return TRUE;
	}

	//UMMOUITradeP2P *pTradeP2PUi = (UMMOUITradeP2P*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_TradeP2P );
	//if( pTradeP2PUi == NULL ){
	//	return bResult;
	//}

	//if( pTradeP2PUi->MyTradeState == etsAsking || pTradeP2PUi->MyTradeState == etsBeingAsked ){
	//	bResult = TRUE;
	//}

	return FALSE;
}


INT MMOFacade::GetDeltaMaxMP(INT nClassID, INT ConstParam, INT catStrength, INT catAgility, INT catIntellect, INT catStamina, INT catSpirit, INT catValue, INT catCombatParam) const
{
	for(INT i=0;i<vecAttribute2.Num();i++)
	{
		if(vecAttribute2(i).ClassID == nClassID)
		{
			if(vecAttribute2(i).AttrName == E_cat_MaxHp)
			{
				INT MaxBaseHp = (
				vecAttribute2(i).ConstParam*ConstParam +
				vecAttribute2(i).catStrength*catStrength +
				vecAttribute2(i).catAgility*catAgility +
				vecAttribute2(i).catIntellect*catIntellect +
				vecAttribute2(i).catStamina*catStamina +
				vecAttribute2(i).catSpirit*catSpirit +
				vecAttribute2(i).catValue*catValue +
				vecAttribute2(i).catCombatParam*catCombatParam )/10000 ;

				return MaxBaseHp;
			}
		}
	}

	return -1;
}
//------------------------------------------------------------------------
// void _LoadAffixTemplates()
// {
// 	SCOPE_LOG_TIME(___INIT__, TEXT("_LoadAffixTemplates"), 0);
// 
// 	//TArray<FString> DirArray;
// 	//GConfig->GetArray( TEXT("AffixTemplates"), TEXT("TemplateDir"), DirArray, GGameIni );
// 	//MMO::FormatTempleteDataLoadingDir(DirArray);
// 
// 	//for ( INT k = 0 ; k < DirArray.Num() ; ++k )
// 	//{
// 	//	const FString& cfgDir = DirArray(k);
// 	//	TArray<FString> pkgs;
// 	//	appFindFilesInDirectory(pkgs, *cfgDir, TRUE, FALSE);
// 	//	for (INT i = 0; i < pkgs.Num(); ++i)
// 	//	{
// 	//		UPackage* Package = UObject::LoadPackage( NULL, *pkgs(i), LOAD_Verify );
// 	//		if( !Package )
// 	//		{
// 	//			warnf( NAME_Error, TEXT("Unable to load %s"), *pkgs(i) );
// 	//			continue;
// 	//		}
// 
// 	//		if(!Package->IsFullyLoaded())
// 	//		{
// 	//			Package->FullyLoad();
// 	//		}
// 
// 	//		for (INT j = 0; j < Package->GetNetObjectCount(0); ++j)
// 	//		{
// 	//			UAI_Affix_Table* Affix = Cast<UAI_Affix_Table>(Package->GetNetObjectAtIndex(j));
// 	//			if(Affix)
// 	//			{
// 	//				Affix->AddToRoot();					
// 	//				MMOFacade::getSingleton().AddAffixTable ( Affix->TableName , Affix );
// 	//			}
// 	//		}
// 	//	}
// 	//}
// 
// 	MMOFacade::getSingleton().InitAffix();	
// 
// }

//------------------------------------------------------------------------
void _LoadItemTemplates()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("_LoadItemTemplates"), 0);

	MMOItemDataManager::getSingleton().InitFromFile(TEXT("item.txt"), true);
	MMOEquipDataManager::getSingleton().InitFromFile(TEXT("item_armor.txt"), true);
}

//UI表现上对技能的归类
INT MMOFacade::GetESUITypeByAbility( UMMOAbility *pAbility )
{
	return MMOSkillManager::getSingleton().GetESUITypeByAbility( pAbility );
}
//------------------------------------------------------------------------
void _LoadNPCTemplates()
{
	static bool g_loadNPCData = false;
	if(g_loadNPCData)
	{
		return;
	}

	g_loadNPCData = true;

	NPCTemplateDataIndexManager::getSingleton().InitFromFile(TEXT("npc.txt"), true);


	SCOPE_LOG_TIME(___INIT__, TEXT("_LoadNPCTemplates"), 0);


#if 0

TArray<FString> DirArray;
	GConfig->GetArray( TEXT("NPCTemplates"), TEXT("TemplateDir"), DirArray, GGameIni );

	MMO::FormatTempleteDataLoadingDir(DirArray);

	for (INT k = 0; k < DirArray.Num(); ++k)
	{
		const FString& cfgDir = DirArray(k);

		debugIDf(TEXT("TemplateData"), TEXT("===== Loading NPC template data form %s"), *cfgDir);

		TArray<FString> pkgs;
		appFindFilesInDirectory(pkgs, *cfgDir, TRUE, FALSE);
		for (INT i = 0; i < pkgs.Num(); ++i)
		{
			UPackage* Package = UObject::LoadPackage( NULL, *pkgs(i), LOAD_Verify );
			if( !Package )
			{
				warnf( NAME_Error, TEXT("Unable to load %s"), *pkgs(i) );
				continue;;
			}

			if(!Package->IsFullyLoaded())
			{
				Package->FullyLoad();
			}

			for (INT j = 0; j < Package->GetNetObjectCount(0); ++j)
			{
				UNPCTemplate* item = Cast<UNPCTemplate>(Package->GetNetObjectAtIndex(j));
				if(item)
				{
					item->AddToRoot();
					MMOFacade::getSingleton().AddNPCTemplate(item->TemplateID, item);
				}
			}
		}
	}
#endif
}
//------------------------------------------------------------------------
void _LoadMissileTemplates()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("_LoadMissileTemplates"), 0);

	MissileTemplateDataIndexManager::getSingleton().InitFromFile(TEXT("missile.txt"));

	//TArray<FString> DirArray;
	//GConfig->GetArray( TEXT("MissileTemplates"), TEXT("TemplateDir"), DirArray, GGameIni );

	//MMO::FormatTempleteDataLoadingDir(DirArray);

	//for (INT k = 0; k < DirArray.Num(); ++k)
	//{
	//	const FString& cfgDir = DirArray(k);

	//	debugIDf(TEXT("TemplateData"), TEXT("===== Loading Missile template data form %s"), *cfgDir);

	//	TArray<FString> pkgs;
	//	appFindFilesInDirectory(pkgs, *cfgDir, TRUE, FALSE);
	//	for (INT i = 0; i < pkgs.Num(); ++i)
	//	{
	//		UPackage* Package = UObject::LoadPackage( NULL, *pkgs(i), LOAD_Verify );
	//		if( !Package )
	//		{
	//			warnf( NAME_Error, TEXT("Unable to load %s"), *pkgs(i) );
	//			continue;;
	//		}

	//		if(!Package->IsFullyLoaded())
	//		{
	//			Package->FullyLoad();
	//		}

	//		for (INT j = 0; j < Package->GetNetObjectCount(0); ++j)
	//		{
	//			UMissile* item = Cast<UMissile>(Package->GetNetObjectAtIndex(j));
	//			if(item)
	//			{
	//				item->AddToRoot();
	//				MMOFacade::getSingleton().AddMissileTemplate(item->Id, item);
	//			}
	//		}
	//	}
	//}
}
//------------------------------------------------------------------------
void _LoadBuffTemplates()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("_LoadBuffTemplates"), 0);

	BuffTemplateDataIndexManager::getSingleton().InitFromFile(TEXT("aura.txt"));

	/*
	TArray<FString> DirArray;
	GConfig->GetArray( TEXT("BuffTemplates"), TEXT("TemplateDir"), DirArray, GGameIni );

	MMO::FormatTempleteDataLoadingDir(DirArray);

	for (INT k = 0; k < DirArray.Num(); ++k)
	{
		const FString& cfgDir = DirArray(k);

		debugIDf(TEXT("TemplateData"), TEXT("===== Loading Buff template data form %s"), *cfgDir);

		TArray<FString> pkgs;
		appFindFilesInDirectory(pkgs, *cfgDir, TRUE, FALSE);
		for (INT i = 0; i < pkgs.Num(); ++i)
		{
			UPackage* Package = UObject::LoadPackage( NULL, *pkgs(i), LOAD_Verify );
			if( !Package )
			{
				warnf( NAME_Error, TEXT("Unable to load %s"), *pkgs(i) );
				continue;;
			}

			if(!Package->IsFullyLoaded())
			{
				Package->FullyLoad();
			}

			for (INT j = 0; j < Package->GetNetObjectCount(0); ++j)
			{
				UBuff* item = Cast<UBuff>(Package->GetNetObjectAtIndex(j));
				if(item)
				{
					item->AddToRoot();
					MMOFacade::getSingleton().AddBuffTemplate(item->TemplateID, 0, item);
				}
			}
		}
	}
	*/
}



//void MMO::Facade::NotifyUpdateFriendItemValue(INT GroupID,INT pos,const FString& PlayerName,INT Lv,const FString& Carrer,const FString& Location,const FString& Sign,INT RoleId,INT Online)
//{
//	UIHudProcess( SetFriendItemValue( GroupID, pos, PlayerName, Lv, Carrer, Location, Sign, RoleId , Online ) );
//}

//-------------------------------------------------------------------------------------
void MMOFacade::PauseBackSound( UBOOL bPause )
{
	UAudioDevice* ad = GEngine->GetAudioDevice();
	if(ad==FALSE)return;

	UMMOUIManager* uimgr = MMOUI::GetUIManager();
	if(uimgr==FALSE)return;

	if(bPause)
	{
		ad->VolumeBackMusic_op = FALSE;
	}
	else
	{
		ad->VolumeBackMusic_op = uimgr->bSoundBack;
	}
}

//-------------------------------------------------------------------------------------
void MMOFacade::NotifyBagSlotChange( INT BagId,INT SlotId )
{
	UIHudProcess( BagSlotChange( BagId, SlotId ) );
}


void MMOFacade::NotifyItemDelete( INT ContainerID, INT SlotID )
{
	UIHudProcess( ItemDelete( ContainerID, SlotID ) );
}
//-------------------------------------------------------------------------------------
void  MMOFacade::SetPlayerAttribute(INT AtrrId, INT Value)
{
	if(!GetMainPlayer())
	{
		return ;
	}
	GetMainPlayer()->SetActorAttributeValue(AtrrId,Value);
	NotifyPlayerAttributeChange(   AtrrId,  Value );
}

//-------------------------------------------------------------------------------------
void  MMOFacade::NotifyEnterCompact( UINT actorID )
{
	AMMOActor* actor = GetCharacter(actorID);

	if (actor)
	{
		actor->SwitchFightIdle(TRUE);
	}

	ANPC* npc = Cast<ANPC>(actor);
	if(npc && npc->Template && npc->Template->AvatarTemplate)
	{
		npc->PlayRandomSound(npc->Template->AvatarTemplate->Sound_EnterBattle);
	}


	AMMOPlayer* player = Cast<AMMOPlayer>( actor );

	if( player == NULL ){
		return;
	}

	UBOOL isMainPlayer = player->IsMainPlayer();
	player->SetInFighting( TRUE );

	if( isMainPlayer ){
		MMOUI::GetUIManager()->delegateShowEnterBattle( TRUE );

	}

	////
	//if( player->IsInTrading() ){
	//	AMMOPlayerController *pCtrlMe = GetMainController();
	//	if( ( pCtrlMe->TradeTargetID == player->ServerID || isMainPlayer ) ){
	//		//
	//		CMessageHandler::GetInstance().Send_TradeCancel();
	//	}
	//}
}

void MMOFacade::NotifyLeaveCompact( UINT actorID )
{
	AMMOActor* actor = GetCharacter(actorID);

	if (actor)
	{
		actor->SwitchFightIdle(FALSE);
	}

	AMMOPlayer* player = Cast<AMMOPlayer>( actor );

	if( player == NULL ){
		return;
	}

	UBOOL isMainPlayer = player->IsMainPlayer();
	player->SetInFighting( FALSE );
	if( isMainPlayer ){
		MMOUI::GetUIManager()->delegateShowEnterBattle( FALSE );
	}
}

//-------------------------------------------------------------------------------------
INT   MMOFacade::GetPlayerAttribute(INT AtrrId)
{
	if(!GetMainPlayer())
	{
		return 0;
	}
	return GetMainPlayer()->GetActorAttributeValue(AtrrId);
}
//-------------------------------------------------------------------------------------
void MMOFacade::NotifyPlayerAttributeChange(  INT AtrrId, INT Value )
{
	UIHudProcess( PlayerAttributeChange( AtrrId, Value ) );
}
//-------------------------------------------------------------------------------------
void  MMOFacade::SetPlayerExp( INT CurrentExp,INT MaxExp ,INT todayExp , INT todayMaxExp,INT ChangeExp )
{
	if(!GetMainPlayer() || !GetMainPlayer()->ActorAttributes)
	{
		return;
	}

	// 移到外面
// 	INT expChange = CurrentExp - GetMainPlayer()->ActorAttributes->LevelAttributes.CurrentExp;
// 
// 	if(expChange > 0) // 以后要是有扣经验值的再加新提示
// 	{
// 		FString str = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.SysExpChange"));
// 		MMOChatManager::getSingleton().AddSystemMessage(FString::Printf(*str, expChange));
// 	}
	if( todayExp >= todayMaxExp )
	{
		_todayExpIsFull = TRUE;
	}
	else
	{
		_todayExpIsFull = FALSE;
	}

	GetMainPlayer()->ActorAttributes->LevelAttributes.CurrentExp = CurrentExp;
	GetMainPlayer()->ActorAttributes->LevelAttributes.MaxExp = MaxExp;
	NotifyPlayerExpChange( CurrentExp , MaxExp,ChangeExp );
}
//-------------------------------------------------------------------------------------
INT  MMOFacade::GetPlayerCurExp()
{
	if(!GetMainPlayer() || !GetMainPlayer()->ActorAttributes)
	{
		return 0;
	}
	return GetMainPlayer()->ActorAttributes->LevelAttributes.CurrentExp;
}
//-------------------------------------------------------------------------------------
INT  MMOFacade::GetPlayerMaxExp()
{
	if(!GetMainPlayer() || !GetMainPlayer()->ActorAttributes)
	{
		return 0;
	}
	return GetMainPlayer()->ActorAttributes->LevelAttributes.MaxExp;
}
//-------------------------------------------------------------------------------------
void  MMOFacade::NotifyPlayerExpChange(  INT CurrentExp,INT MaxExp,INT ChangeExp )
{
	UIHudProcess( PlayerExpChange( CurrentExp, MaxExp, ChangeExp ) );

	// 成就
	MMOGameEvent_Achievement evtAch(adExperience, CurrentExp, 0, 0);
	MMOUI::PostGameEvent(&evtAch);

	MMOGameEvent_MasterStudent evtMaster(EMasterStudentType::EMST_PlayerExpChange);
	MMOUI::PostGameEvent(&evtMaster);
}
//-------------------------------------------------------------------------------------

INT32 MMOFacade::GetInt32(INT64 nInValue)
{
	if(nInValue>2147483647)
	{
		return 2147483647;
	}
	return (INT32)nInValue;
}

UBOOL g_bSyncPlayerMoney=FALSE;
void MMOFacade::SetPlayerMoney( G2C_SYNC_MONEY PlayerMoney )
{
	g_bSyncPlayerMoney = TRUE;
	for (INT i = E_MONEYTYPE_01; i < E_MONEYTYPE_COUNT; ++i)
	{
		SetPlayerMoney((EMONEY_TYPE)i, PlayerMoney.allMoneys[i]);

		////新增代币的周末获取量和周末最大获取量
		//_PlayerMoney.anWeeklyMoney[i]		= PlayerMoney.anWeeklyMoney[i];
		//_PlayerMoney.anWeeklyMoneyLimit[i]	= PlayerMoney.anWeeklyMoneyLimit[i];

		_PlayerMoney.allMoneys[i]		= PlayerMoney.allMoneys[i];
		_PlayerMoney.allMoneysLimit[i]	= PlayerMoney.allMoneysLimit[i];
		
		_PlayerMoney.anMoneys[i]=GetInt32(_PlayerMoney.allMoneys[i]);		
		_PlayerMoney.anMoneysLimit[i]=GetInt32(_PlayerMoney.allMoneysLimit[i]);

		_PlayerMoney.anDailyEarned[i]		= PlayerMoney.anDailyEarned[i];
		_PlayerMoney.anDailyEarnedLimit[i]	= PlayerMoney.anDailyEarnedLimit[i];
		_PlayerMoney.anDailyUsed[i]		= PlayerMoney.anDailyUsed[i];
		_PlayerMoney.anDailyUsedLimit[i]	= PlayerMoney.anDailyUsedLimit[i];
		_PlayerMoney.anWeeklyEarned[i]		= PlayerMoney.anWeeklyEarned[i];
		_PlayerMoney.anWeeklyEarnedLimit[i]	= PlayerMoney.anWeeklyEarnedLimit[i];
		_PlayerMoney.anWeeklyUsed[i]		= PlayerMoney.anWeeklyUsed[i];
		_PlayerMoney.anWeeklyUsedLimit[i]	= PlayerMoney.anWeeklyUsedLimit[i];

	}
	g_bSyncPlayerMoney = FALSE;
}


void MMOFacade::SetPlayerMoney( G2C_MONEY_CHANGE PlayerMoneyChange )
{
	EMONEY_TYPE MoneyType = (EMONEY_TYPE)PlayerMoneyChange.nMoneyType;

	if( MoneyType == E_MONEYTYPE_01 || MoneyType == E_MONEYTYPE_02 )
	{
		if( PlayerMoneyChange.llMoney >= MAX_MONEY/10*9 )
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.MoneyToMore") ) ,MsgInfoType_Table,TEXT("MMOGame.Bag.MoneyToMore"));
		}
	}

	if(MoneyType < E_MONEYTYPE_COUNT)
	{
		
		INT64 oldMoney = _PlayerMoney.allMoneys[MoneyType];
		_PlayerMoney.allMoneys[MoneyType]	= PlayerMoneyChange.llMoney;
		_PlayerMoney.anMoneys[MoneyType]=GetInt32(_PlayerMoney.allMoneys[MoneyType]);	
		INT64 Money	= _PlayerMoney.allMoneys[MoneyType];

		_PlayerMoney.anDailyEarned[MoneyType]		= PlayerMoneyChange.nDailyEarned;
		_PlayerMoney.anWeeklyUsed[MoneyType]		= PlayerMoneyChange.nDailyUsed;

		_PlayerMoney.anWeeklyEarned[MoneyType]		= PlayerMoneyChange.nWeeklyEarned;
		_PlayerMoney.anWeeklyUsed[MoneyType]		= PlayerMoneyChange.nWeeklyUsed;

		if(oldMoney!=Money)
		{
			NotifyPlayerMoneyChange(MoneyType, Money , oldMoney );
		}
	}
	else
	{
		debugf(NAME_Error, TEXT("Facade::SetPlayerMoney money type is large than MONEYTYPE_COUNT!"));
	}
}

void MMOFacade::SetPlayerMoney( EMONEY_TYPE MoneyType, INT64 Money )
{
	if( MoneyType == E_MONEYTYPE_01 || MoneyType == E_MONEYTYPE_02 )
	{
		if( Money >= MAX_MONEY/10*9 )
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.MoneyToMore") ),MsgInfoType_Table,TEXT("MMOGame.Bag.MoneyToMore") );
		}
	}

	if(MoneyType < E_MONEYTYPE_COUNT)
	{
		INT32 oldMoney = _PlayerMoney.anMoneys[MoneyType];
		_PlayerMoney.allMoneys[MoneyType] = Money;
		_PlayerMoney.anMoneys[MoneyType]=GetInt32(_PlayerMoney.allMoneys[MoneyType]);	
		
		NotifyPlayerMoneyChange(MoneyType, _PlayerMoney.anMoneys[MoneyType] , oldMoney );
	}
	else
	{
		debugf(NAME_Error, TEXT("Facade::SetPlayerMoney money type is large than MONEYTYPE_COUNT!"));
	}
}

INT32 MMOFacade::GetPlayerMoney_Weekly( EMONEY_TYPE MoneyType )
{
	//return _PlayerMoney.anWeeklyEarned[MoneyType];

	if(MoneyType < E_MONEYTYPE_COUNT)
	{
		switch(MoneyType)
		{
		case E_MONEYTYPE_MASTER_EXP:
		case E_MONEYTYPE_MORAL:
			{
				INT32 nMoney = _PlayerMoney.anWeeklyEarned[MoneyType];

				if(nMoney > 0)
				{
					nMoney = nMoney / MoneyRatio;
				}
				return nMoney;
			}
			break;

		default:
			return _PlayerMoney.anWeeklyEarned[MoneyType];
			break;
		}
	}
	return 0;
}

INT32 MMOFacade::GetPlayerMoney_WeeklyLimit( EMONEY_TYPE MoneyType )
{
	if(MoneyType < E_MONEYTYPE_COUNT)
	{
		switch(MoneyType)
		{
		case E_MONEYTYPE_MASTER_EXP:
		case E_MONEYTYPE_MORAL:
			{
				INT32 nMoney = _PlayerMoney.anWeeklyEarnedLimit[MoneyType];

				if(nMoney > 0)
				{
					nMoney = nMoney / MoneyRatio;
				}
				return nMoney;
			}
			break;

		default:
			return _PlayerMoney.anWeeklyEarnedLimit[MoneyType];
			break;
		}

		
	}
	return 0;
}

INT32 MMOFacade::GetPlayerMoney( EMONEY_TYPE MoneyType )
{
	if(MoneyType < E_MONEYTYPE_COUNT)
	{
		switch(MoneyType)
		{
			//case E_MONEYTYPE_MASTER_EXP:
			case E_MONEYTYPE_MORAL:
				{
					INT32 nMoney = _PlayerMoney.anMoneys[MoneyType];
					
					if(nMoney > 0)
					{
						nMoney = nMoney / MoneyRatio;
					}
					return nMoney;
				}
				break;

			default:
				return _PlayerMoney.anMoneys[MoneyType];
				break;
		}
		
	}

	debugf(NAME_Error, TEXT("Facade::GetPlayerMoney money type is large than MONEYTYPE_COUNT!"));

	return 0;
}


INT64 MMOFacade::GetPlayerMoney64( EMONEY_TYPE MoneyType )
{
	if(MoneyType < E_MONEYTYPE_COUNT)
	{
		//switch(MoneyType)
		//{
		//	//case E_MONEYTYPE_MASTER_EXP:
		//case E_MONEYTYPE_MORAL:
		//	{
		//		INT64 nMoney = _PlayerMoney.allMoneys[MoneyType];

		//		if(nMoney > 0)
		//		{
		//			nMoney = nMoney / MoneyRatio;
		//		}
		//		return nMoney;
		//	}
		//	break;

		//default:
		//	return _PlayerMoney.allMoneys[MoneyType];
		//	break;
		//}
		return _PlayerMoney.allMoneys[MoneyType];
	}
	return 0;
}
//------------------------------------------------------------------------------------------------
void MMOFacade::RequestUpdateMoney()
{
	for (BYTE i = 0; i < MONEYTYPE_COUNT; ++i)
	{
		MMOGameEvent_UpdateMoney evtUpdateMoney((MONEY_TYPE)i, _PlayerMoney.anMoneys[i], _PlayerMoney.anMoneys[i]);
		MMOUI::PostGameEvent(&evtUpdateMoney);
	}
}
//-------------------------------------------------------------------------------------
void   MMOFacade::NotifyPlayerMoneyChange( EMONEY_TYPE MoneyType, INT64 Money , INT64 OldMoney )
{
	UIHudProcess( PlayerMoneyChange( MoneyType , GetInt32(Money-OldMoney) ) );
	MMOHeavenManager::getSingleton().NotifyMoneyChange(MoneyType,Money,OldMoney);

	if (MoneyType == E_MONEYTYPE_MASTER_EXP)
	{
		MasterStudentManager::getSingleton().NotifyMasterValueChange(GetInt32(Money), GetInt32(OldMoney));
	}

	MMOGameEvent_UpdateMoney evtUpdateMoney((MONEY_TYPE)MoneyType, GetInt32(OldMoney), GetInt32(Money));
	MMOUI::PostGameEvent(&evtUpdateMoney);

	// 成就
	MMOGameEvent_Achievement evtAch(adTotalMoneyType, MoneyType, Money, 0);
	MMOUI::PostGameEvent(&evtAch);

	MMOGuildManager::getSingleton().NotifyPlayerMoneyChange(MoneyType, Money-OldMoney);

	//金币交易行
	MMOGoldStoreManager::getSingleton().NotifyUpdatePlayerMoney();
}
//-------------------------------------------------------------------------------------
void   MMOFacade::NotifyNPCHpChange( ANPC* npc , INT hp )
{
	UIHudProcess( NPCHpChange( npc,hp ) );

	MMOQuestManager::getSingleton().UpdateNpcHp(npc, hp);
}
//-------------------------------------------------------------------------------------
void MMOFacade::SetNPCHP( ANPC* npc , INT hp )
{
	if(hp<0)hp=0;

	if( !npc || !npc->ActorAttributes )
		return;

	if( npc->ActorAttributes->HP != hp )
	{
		npc->ActorAttributes->SetHP(hp);// = hp;
		NotifyNPCHpChange( npc,hp );
	}
}

void MMOFacade::SetNPCHP( ANPC* npc , INT hp, INT maxHp )
{
	if(hp<0)hp=0;
	if( !npc || !npc->ActorAttributes )
		return;

	if( npc->ActorAttributes->HP != hp || npc->ActorAttributes->MaxHP != maxHp)
	{
		npc->ActorAttributes->MaxHP = maxHp;
		npc->ActorAttributes->SetHP(hp);// = hp;
		NotifyNPCHpChange( npc,hp );
	}
}
//-------------------------------------------------------------------------------------
void MMOFacade::NotifyUpdateTeamContent()
{
	UIHudProcess( UpdateTeamContent( ) );
}
//-------------------------------------------------------------------------------------
//void MMO::Facade::NotifyUpdateAllTeamMate()
//{
//	UIHudProcess( UpdateAllTeamMate() );
//}
//-------------------------------------------------------------------------------------
//void MMO::Facade::NotifyUpdateTeamMateNum( INT Num )
//{
//	UIHudProcess( UpdateTeamMateNum(Num) );
//}
//
//void  MMO::Facade::NotifyAddedAttentionBy(INT RoleId , FString RoleName )
//{
//	UIHudProcess( AddedAttentionBy( RoleId,RoleName ) );
//}
//-------------------------------------------------------------------------------------
void MMOFacade::NotifyFriendChatMsg( INT RoleId , FString Content )
{
	UIHudProcess( FriendChatMsg(RoleId,Content) );
}
//-------------------------------------------------------------------------------------
void MMOFacade::NotifyEnterScene()
{
	bLevelsLoaded = FALSE;
	NotifyChangeScene();

	if( LastInGameScene )
	{		
		return;
	}
	
	UIHudProcess( EnterScene() );
	LastInGameScene = TRUE;
}

//-------------------------------------------------------------------------------------
void MMOFacade::NotifyChangeScene()
{
	bLevelsLoaded = FALSE;
	UIHudProcess( ChangeScene() );
}
//-------------------------------------------------------------------------------------
void MMOFacade::NotifyEnterLogin()
{
	UIHudProcess( EnterLogin() ); 
	//if(MMOFacade::getSingleton().GetMainControllerAI())
	//{
	//	MMOFacade::getSingleton().GetMainControllerAI()->StartCmdAbility(NULL,0,FALSE,FALSE);
	//}
	//LastInGameScene = FALSE;
//	MMOTargetManager::getSingleton().ClearDesireTarget();
	FSMIO::getSingleton().State.ClearAllState();
	MMOFacade::getSingleton().GetMMOBuffManager()->ClearAllBuff();
	LastInGameScene = FALSE;
}
//-------------------------------------------------------------------------------------
void MMOFacade::NotifyEnterGame()
{
	//MMOUI::StartAllPanels( );
	//LastInGameScene = TRUE;
}
//-------------------------------------------------------------------------------------
void MMOFacade::NotifyHUDLoaded()
{
	for( MoviePlayerMaps::TIterator PairIt(AllMoviePlayers); PairIt; ++PairIt)
	{
		if( PairIt.Value() )
		{
			PairIt.Value()->OnHUDLoaded(  );
		}
	}
}
//-------------------------------------------------------------------------------------
void MMOFacade::NotifyAddNewNpcData( ANPC* Npc )
{
	//
	EActorClassType actorType = (EActorClassType)Npc->GetActorType();
	//
	//if( actorType == EACT_Monster || actorType == EACT_Boss || actorType == EACT_NPC){
	if( actorType == EACT_Monster || actorType == EACT_Boss || actorType == EACT_Pet ){
		MMOMapDataManager::getSingleton().AddOrUpdateMapDymcActorNear( *Npc );
	}

	UIHudProcess( AddNewNpcData(Npc) );

	MMOGameEvent_NPCAppear evt(Npc);
	MMOUI::PostGameEvent(&evt);
}
//-------------------------------------------------------------------------------------
void MMOFacade::NotifyUnReadFriendChatMsgChange()
{
	UIHudProcess( UnReadFriendChatMsgChange() );
}
//-------------------------------------------------------------------------------------
void MMOFacade::SetPlayerHP( AMMOPlayer* player , INT hp )
{
	if(hp<0)hp=0;
	if( !player || !player->ActorAttributes )
		return;
	player->ActorAttributes->SetHP(hp);// = hp;
	NotifyOtherPlayerHpChange(player,hp);
	if( GetMainPlayer() && TeamManager::getSingleton().HasTeam() )
	{		
		SetTeamMemberHP(player->ServerID,hp,player->ActorAttributes->MaxHP );
	}
}

void MMOFacade::SetPlayerHP ( AMMOPlayer* player , INT hp , INT maxHp)
{
	if(hp<0)hp=0;
	if( !player || !player->ActorAttributes )
		return;
	player->ActorAttributes->SetHP(hp);// = hp;
	player->ActorAttributes->MaxHP = maxHp;
	NotifyOtherPlayerHpChange(player,hp);
	if( GetMainPlayer() && TeamManager::getSingleton().HasTeam() )
	{		
		SetTeamMemberHP(player->ServerID,hp,player->ActorAttributes->MaxHP );
	}
}
//-------------------------------------------------------------------------------------
void MMOFacade::SetPlayerMP( AMMOPlayer* player , INT mp )
{
	if( !player || !player->ActorAttributes )
		return;
	player->ActorAttributes->MP = mp;
	NotifyOtherPlayerMpChange(player,mp);

	if( GetMainPlayer() && TeamManager::getSingleton().HasTeam() )
		SetTeamMemberMP(player->ServerID,mp,player->ActorAttributes->GetMaxMp() );
}
//-------------------------------------------------------------------------------------
void MMOFacade::NotifyOtherPlayerHpChange( AMMOPlayer* player , INT hp )
{
	if( !player || !player->ActorAttributes )
		return;
	//UIHudProcess( OtherPlayerHpChange( player,hp ) );
	for(UIMaps::TIterator PairIt(AllUIMaps); PairIt; ++PairIt) 
	{
		if(PairIt.Value())
			PairIt.Value()->OtherPlayerHpChange( player,hp ); 
	}
}
//-------------------------------------------------------------------------------------
void MMOFacade::NotifyOtherPlayerMpChange( AMMOPlayer* player , INT mp )
{
	if( !player || !player->ActorAttributes )
		return;
	UIHudProcess( OtherPlayerMpChange( player,mp ) );
}
//队伍成员面板更新------------------------------------------------------------------------------
//void MMO::Facade::NotifyUpdateMyTeam()
//{
//	UIHudProcess( UpdateMyTeam(  ) );
//}
//-------------------------------------------------------------------------------------
void MMOFacade::SetTeamMemberHP(  INT MemberServeId ,  INT HP ,INT MAXHP,INT hasPos  )
{
	if(!GetMainPlayer())
		return;

	
	
	if( hasPos < 0 )
	{
		hasPos = TeamManager::getSingleton().GetMemberPosByMemberId(MemberServeId);
	}

	if( hasPos >= 0 && TeamManager::getSingleton().GetTeamMemberCount() > hasPos )
	{
		MMOTeamMember* mem = TeamManager::getSingleton().GetTeamMember(hasPos);

		if(mem)
		{
			if( HP == 0 || MAXHP == 0)
			{
				mem->HPPecent =0.1;
				mem->HP = 0;
			}
			else
			{
				mem->HPPecent =  (float)HP / MAXHP;
				mem->HP = HP;
			}
			NotifyUpdateTeamMate( mem->UIPos , hasPos );
		}

	}

}
//-------------------------------------------------------------------------------------
void MMOFacade::SetTeamMemberMP(  INT MemberServeId ,  INT MP , INT MAXMP ,INT hasPos)
{
	if(!GetMainPlayer())
		return;

	if( hasPos < 0 )
	{
		hasPos = TeamManager::getSingleton().GetMemberPosByMemberId(MemberServeId);
	}
	if( hasPos >= 0 && TeamManager::getSingleton().GetTeamMemberCount() > hasPos )
	{
		MMOTeamMember* mem = TeamManager::getSingleton().GetTeamMember(hasPos);
		if(mem)
		{
			if( MP == 0 || MAXMP == 0)
				mem->MPPecent =0.1;
			else
				mem->MPPecent = (float)MP / MAXMP;

			if( mem->UIPos >= 0 )
				NotifyUpdateTeamMate( mem->UIPos , hasPos );
		}

	}
	
}
//----------------------------------------------------------------------
void MMOFacade::SetTeamMemberLv( INT MemberServeId , INT Lv , INT hasPos /* = -1 */ )
{
	if(!GetMainPlayer())
		return;

	if( hasPos < 0 )
	{
		hasPos = TeamManager::getSingleton().GetMemberPosByMemberId(MemberServeId);
	}
	if( hasPos >= 0 && TeamManager::getSingleton().GetTeamMemberCount() > hasPos )
	{
		MMOTeamMember* tmem = TeamManager::getSingleton().GetTeamMember(hasPos);
		if(tmem)
		{
			tmem->Lv = Lv;
			if( tmem->UIPos >= 0 )
				NotifyUpdateTeamMate( tmem->UIPos , hasPos );
		}
		
		
	}


	
}
//----------------------------------------------------------------------
void MMOFacade::SetTeamMemberOnline( INT MemberServeId , BOOL Online , INT hasPos /* = -1 */ )
{
	if(!GetMainPlayer())
		return;


	if( hasPos < 0 )
	{
		hasPos = TeamManager::getSingleton().GetMemberPosByMemberId(MemberServeId);
	}
	if( hasPos >= 0 && TeamManager::getSingleton().GetTeamMemberCount() > hasPos )
	{
		MMOTeamMember* mem = TeamManager::getSingleton().GetTeamMember(hasPos);
		if(mem)
		{
			mem->Online =Online;
			if( mem->UIPos >= 0 )
				NotifyUpdateTeamMate( mem->UIPos , hasPos );
		}
		
	}
}

//通知更新队友-------------------------------------------------------------------------------------
void MMOFacade::NotifyUpdateTeamMate( INT UIPos , INT Pos )
{
	UIHudProcess( UpdateTeamMate(  UIPos , Pos ) );
}


//add by jimzhang ---------------------------------------------------------------------------
UBOOL MMOFacade::IsPlayerAttackable(const AMMOPlayer * Player1, const AMMOPlayer* Player2)
{
	//if(MMOMapDataManager::getSingleton().GetCurMap().bPK == FALSE)return FALSE;
	if(Player1==NULL)return FALSE;
	if(Player2==NULL)return FALSE;
	if(Player1->ActorAttributes==NULL)return FALSE;
	if(Player2->ActorAttributes==NULL)return FALSE;
	//if(Player1->ActorAttributes->LevelAttributes.Level<=10)return FALSE;
	//if(Player2->ActorAttributes->LevelAttributes.Level<=10)return FALSE;


	//敌对势力，直接可攻击
	INT nForceRelative = ForceRelative[Player1->ActorAttributes->Faction][Player2->ActorAttributes->Faction];
	if(nForceRelative<0)
	{
		return TRUE;
	}

	//然后再判定不同场合
	switch(Player1->ActorAttributes->PkMode)
	{
	case rpmPeace: 
		{
			return FALSE;
		}
		break;
	case rpmGuild: 
		{
			if( Player1->GuildName.Len()>0 && Player1->GuildName==Player2->GuildName )
			{
				return FALSE;
			}
			if (Player1->IsMainPlayer() && GR_FRIEND == MMOGuildData::getSingleton().GetGuildRelation(Player2->GuildName) )
			{
				return FALSE;
			}
		}
		break;
	case rpmTeam: 
		{
			if( Player1->TeamID>0 && Player1->TeamID==Player2->TeamID)
			{
				return FALSE;
			}
		}
		break;
	case rpmSlayer: 
		{

		}
		break;
	case rpmJustice: 
		{
			if(GetPkValueLevel(Player2->ActorAttributes->GetPkValue())<2 )
			{
				return FALSE;
			}
		}
		break;
	}

	return TRUE;
}

//add by jimzhang ---------------------------------------------------------------------------
UBOOL MMOFacade::IsAttackable(const AMMOActor * Actor1, const AMMOActor* Actor2)
{
	if( Actor1 == NULL || Actor2 == NULL )return FALSE;
	if( Actor1 == Actor2) return FALSE;

	const AMMOPlayer* p1 = ConstCast<AMMOPlayer>(Actor1);
	const AMMOPlayer* p2 = ConstCast<AMMOPlayer>(Actor2);
	const ANPC* n1 = ConstCast<ANPC>(Actor1);
	const ANPC* n2 = ConstCast<ANPC>(Actor2);

	const AMMODoodadActor* doodadActor1 = ConstCast<AMMODoodadActor>(Actor1);
	const AMMODoodadActor* doodadActor2 = ConstCast<AMMODoodadActor>(Actor2);
	
	if(doodadActor1)
	{
		if(doodadActor1->DoodadTemplate &&
			(doodadActor1->DoodadTemplate->DoodadType == E_dt_Destrucible))
		{
			return TRUE;
		}
	}

	if(doodadActor2)
	{
		if(doodadActor2->DoodadTemplate &&
			(doodadActor2->DoodadTemplate->DoodadType == E_dt_Destrucible))
		{
			return TRUE;
		}
	}

	if(p1&&p2)//玩家之间
	{
		return IsPlayerAttackable(p1,p2);
	}
	else if(p1&&n2)//玩家与npc
	{
		if(n2->dwHostID>0&&n2->GetActorType()==EACT_Pet)
		{
			AMMOActor* HostActor = MMOFacade::getSingleton().GetCharacter(n2->dwHostID);
			if(HostActor){
				return IsAttackable(p1,HostActor);
			}
			else
			{
				return FALSE;
			}
		}

		if(p1->ActorAttributes->PkMode==rpmSlayer&&n2->IsMonster())
		{
			return TRUE;
		}

		if(p1->ActorAttributes->PkMode==rpmSlayer)
		{
			INT nCanAttackWithFreePkMode = GetCanAttackWithFreePkMode(n2->ForceID);
			if(nCanAttackWithFreePkMode)
			{
				return TRUE;
			}
		}

		return ( MMOFacade::getSingleton().GetPlayerReputation(p1, n2->ForceID)<0 );
	}
	else if(p2&&n1)//玩家与npc
	{
		if(n1->dwHostID>0)
		{
			AMMOActor* HostActor = MMOFacade::getSingleton().GetCharacter(n1->dwHostID);
			AMMOPlayer* HostPlayer = Cast<AMMOPlayer>(HostActor);
			if(HostPlayer){
				return IsAttackable(HostActor,p2);
			}
			//else
			//{
			//	return FALSE;
			//}
		}

		if(p2->ActorAttributes->PkMode==rpmSlayer&&n1->IsMonster())
		{
			return TRUE;
		}
		if(p2->ActorAttributes->PkMode==rpmSlayer)
		{
			INT nCanAttackWithFreePkMode = GetCanAttackWithFreePkMode(n1->ForceID);
			if(nCanAttackWithFreePkMode)
			{
				return TRUE;
			}
		}
		return ( MMOFacade::getSingleton().GetPlayerReputation(p2, n1->ForceID)<0 );
	}

	return FALSE;
}

UBOOL MMOFacade::IsAttackable( const AMMOActor * target )
{
	return IsAttackable(GetMainPlayer(), target);
}

UBOOL MMOFacade::IsTradeable( const AMMOPlayer* player1, const AMMOPlayer* player2 )
{
	return FALSE;

}
//======================================================================================================================================================
UMMOAbility* MMOFacade::FindMyUsableSkill( INT mainSkillID )
{
	UMMOAbility* myLearnedMainSkill = MMOSkillManager::getSingleton().FindMyLearnedMainSkill( mainSkillID );
	if( myLearnedMainSkill == NULL ){
		return NULL;
	}

	////铭文技能
	INT uniqueSkillID = 0;
	INT runeSkillIdx = 0;
	UBOOL isSkillActive = FALSE;
	UMMOAbility* equpRuneSkill = MMOEquipManager::getSingleton().GetCurWeaponRuneSkillOfMainSkillID( mainSkillID, isSkillActive );
	if( isSkillActive == TRUE && equpRuneSkill != NULL ){
		//INT runeSkillMainSkillID = equpRuneSkill->GetTemplateID();
		//runeSkillIdx = equpRuneSkill->GetRuneSlot();
		//uniqueSkillID = MMOSkillManager::CombinateAbilityID( mainSkillID, equpRuneSkill->SpellCommonAttribute.Level, runeSkillIdx );
		return equpRuneSkill;
	}

	return myLearnedMainSkill;
}
//======================================================================================================================================================
UBOOL MMOFacade::IsAbilityCanUse( UMMOAbility *toUseAbility )
{
	if( toUseAbility == NULL ){
		return FALSE;
	}

	AMMOPlayer *mainPlayer = GetMainPlayer();
	if( mainPlayer == NULL ){
		return FALSE;
	}

	UBOOL isAbilityActive = TRUE;
	//
	BuffDataMap* buffMap = GetMMOBuffManager()->GetBuffMap( mainPlayer->ServerID );
	if( buffMap != NULL ){
		const FSpell_CCLimitBitsType &lmtSta = toUseAbility->SpellCommonAttribute.CasterCCLimitBits;
		const FSpell_LimitBitsType &actSta = toUseAbility->SpellCommonAttribute.CasterLimitBits;

		for( STLInt2BuffDataMap::iterator buffIter = buffMap->Buffs.begin(); buffIter != buffMap->Buffs.end(); ++buffIter )
		{
			if( buffIter->second == NULL || buffIter->second->Buff == NULL ){
				continue;
			}
			UBuff *buffTmpt = buffIter->second->Buff;
			switch( buffTmpt->Type )
			{
			case E_auet_Type01: 
				{
					isAbilityActive = !( lmtSta.bCCLimitBitsType5 || lmtSta.bCCLimitBitsType6 );
				}
				break;
			case E_auet_Type02: isAbilityActive = !( lmtSta.bCCLimitBitsType3 ); break;
			case E_auet_Type03:
				{
					isAbilityActive = !( lmtSta.bCCLimitBitsType0 || lmtSta.bCCLimitBitsType6 );
			}
				break;
			case E_auet_Type04: isAbilityActive = !( lmtSta.bCCLimitBitsType2 ); break;
			case E_auet_Type05: isAbilityActive = !( lmtSta.bCCLimitBitsType7 ); break;
			case E_auet_Type06: isAbilityActive = !( lmtSta.bCCLimitBitsType1 ); break;
			case E_auet_Type17: isAbilityActive = !( lmtSta.bCCLimitBitsType4 ); break;
			}

			if( isAbilityActive == FALSE ){
				break;
			}
		}
	}
	//else{
	//	const FSpell_LimitBitsType &actSta = toUseAbility->SpellCommonAttribute.CasterLimitBits;
	//	if( actSta.bLimitBitsType2 || actSta.bLimitBitsType3 ){
	//		//3反击状态和4隐身状态
	//		isAbilityActive = FALSE;
	//	}
	//	else{
	//		isAbilityActive = TRUE;
	//	}
	//}

	return isAbilityActive;
}

//获取某个ForceID的声望值
INT   MMOFacade::GetPlayerReputation(const AMMOPlayer* player, INT ForceID)
{
	if(player && player->ActorAttributes)
	{
		return player->ActorAttributes->GetReputation(ForceID);
	}
	return 0;
}

//获取某个ForceID的是否激活值
UBOOL   MMOFacade::GetPlayerActiveState(const AMMOPlayer* player, INT ForceID)
{
	if(player && player->ActorAttributes)
	{
		return player->ActorAttributes->GetActiveState(ForceID);
	}
	return false;
}


void MMOFacade::SetPlayerReputation(AMMOPlayer* player, INT Index,INT ForceID, INT newReputation, INT lastReputation)
{
	if(player && player->ActorAttributes)
	{
		player->ActorAttributes->SetReputation(Index,ForceID, newReputation);
		UIHudProcess( UpdatePlayerReputation(player,ForceID,newReputation,lastReputation) );
	}
}
//设置某个ForceID的是否领取
void MMOFacade::SetPlayerActiveState(AMMOPlayer* player, INT Index,INT ForceID, BOOL ActiveState)
{
	if(player && player->ActorAttributes)
	{
		player->ActorAttributes->SetActiveState(Index,ForceID, ActiveState);
	}
}

void MMOFacade::SetPlayerAwardState(AMMOPlayer* player, INT Index,INT ForceID, BOOL AwardState)
{
	if(player && player->ActorAttributes)
	{
		player->ActorAttributes->SetAwardState(Index,ForceID, AwardState);

		// 强者之路
		MMOGameEvent_Journey evtJourney(MMOGameEvent_Journey::EJET_GetAwards, ForceID);
		MMOUI::PostGameEvent(&evtJourney);
	}
}


UBOOL  MMOFacade::GetPlayerAwardState(const AMMOPlayer* player, INT ForceID)
{
	if(player && player->ActorAttributes)
	{
		return player->ActorAttributes->GetAwardState(ForceID);
	}
	return FALSE;
}

UBOOL  MMOFacade::GetPlayerAwardDisplay(const AMMOPlayer* player, INT ForceID)
{
	UBOOL AwardDisplay = FALSE;
	if(player && player->ActorAttributes)
	{
		UBOOL AwardState	= player->ActorAttributes->GetAwardState(ForceID);
		INT	  Reputation	= MMOFacade::getSingleton().GetPlayerReputation(player, ForceID);

		ReputationTemp*  pReputationTemp = MMOFacade::getSingleton().GetReputationTempByForceID(ForceID);

		if(AwardState==false && pReputationTemp && Reputation>=pReputationTemp->MaxValue )
		{
			AwardDisplay = TRUE;
		}
		else
		{
			AwardDisplay  = FALSE;
		}
	}
	return AwardDisplay;
}


UBOOL MMOFacade::LoadStringMap()
{
	g_StringMap.Empty();

	FString lang = appGetLanguageExt();

	CTabFile* tabFile = ReadTabFile(TEXT("StringMap.txt"));
	if(!tabFile)
	{
		return FALSE;
	}

	int idx = tabFile->GetKeyColIdx(*lang);
	if(idx == -1)
	{
		warnf(NAME_Error, TEXT("读取StringMap.txt配置文件时未找到对应的语言列：%s"), *lang);
		return FALSE;
	}
	int iSoundIndex = tabFile->GetKeyColIdx(TEXT("soundName"));
	int msgTypeIdx = tabFile->GetKeyColIdx(TEXT("msgType"));
	for (int i = 1; i < tabFile->GetRowCount(); ++i)
	{
		StringMapVal val;
		val.stringVal = tabFile->GetData(idx, i);
		if(iSoundIndex > -1)
			val.soundName = tabFile->GetData(iSoundIndex, i);
		if(msgTypeIdx > -1 )
		{
			val.msgInfoType = appAtoi( tabFile->GetData( msgTypeIdx , i ) );
		}
		g_StringMap.Set( tabFile->GetData(0, i) , val );
	}

	delete tabFile;
#if 0
	TArray<TArray<FString>> ArrayString2D;
	if(!ReadConfigString( ArrayString2D, TEXT("StringMap.txt") , TRUE ))
	{
		return FALSE;
	}	

	FString lang = appGetLanguageExt();
	INT type = MMOUI::GetStringType(lang);
	if( type < 0 )
		return FALSE;

	for( INT i = 0 ; i < ArrayString2D.Num() ; i++ )
	{
		TArray<FString>& line = ArrayString2D(i);
		if( line.Num() <= type + 1 )
			continue;
		StringMapVal val;
		val.stringVal = line(type+1);
		val.soundName = line(3);
		g_StringMap.Set( line(0) , val );
	}
#endif
	return TRUE;
}

UBOOL MMOFacade::ReadPowerConfig()
{
	//TArray<TArray<FString>> ArrayString2D;

//#if 0 // TABFILE
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("Power.txt"), TRUE ) )
	//if(!ReadConfigString( ArrayString2D, TEXT("level.txt")))
//#endif
	{
		return FALSE;
	}

	//TArray<FString>& ArrayStringTitle = ArrayString2D(0);
	INT lvIndex = tabFile->GetKeyColIdx(TEXT("Level"));		//ArrayStringTitle.FindItemIndex( TEXT("Level") );
	INT poIndex = tabFile->GetKeyColIdx(TEXT("BasePow"));		//ArrayStringTitle.FindItemIndex( TEXT("Pow") );
	INT spiriPoIndex = tabFile->GetKeyColIdx(TEXT("SpiritPower"));	
	//INT expIndex = tabFile->GetKeyColIdx(TEXT("MaxExp"));	//ArrayStringTitle.FindItemIndex(TEXT("MaxExp"));
	
	if( lvIndex < 0 || poIndex < 0 || spiriPoIndex < 0)
	{
		delete tabFile;
		return FALSE;
	}

	for ( INT k = 0 ; k < tabFile->GetRowCount() ; k++ )
	{

		LevelPowerMap.Set( appAtoi( tabFile->GetData(lvIndex,k) ) , appAtoi( tabFile->GetData(poIndex,k)  ) );
		//LevelExpMap.Set( appAtoi( tabFile->GetData(lvIndex,k) ) , appAtoi( tabFile->GetData(expIndex,k)  ) );
		LevelSpiritPowerMap.Set( appAtoi( tabFile->GetData(lvIndex,k) ) , appAtoi( tabFile->GetData(spiriPoIndex,k)  ) );
		//BaseAttributeValueMap.AddItem(AttributeId);
	}

	
	delete tabFile;
	return TRUE;
}


INT MMOFacade::GetLevelPow( INT lv )
{
	INT* val = LevelPowerMap.Find( lv );
	if(val)
	{
		return *val;
	}
	else
	{
		return 0;
	}
}

INT MMOFacade::GetSpiritPow( INT lv )
{
	INT* val = LevelSpiritPowerMap.Find( lv );
	if(val)
	{
		return *val;
	}
	else
	{
		return 0;
	}
}

INT MMOFacade::GetLevelExp( INT lv)
{
	INT* exp = LevelExpMap.Find(lv);
	if (exp)
	{
		return *exp;
	}
	else
	{
		return 0;
	}
}


UBOOL MMOFacade::ReadAttributeConfig()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("Facade::ReadAttributeConfig"), 0);

	//TArray<TArray<FString>> ArrayString2D;
//#if 0 // TABFILE
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("attributes1.txt"), TRUE ) )
	//if(!ReadConfigString( ArrayString2D, TEXT("attributes1.txt") , TRUE ))
//#endif
	{
		return FALSE;
	}

	//记录标题的映射
	BaseAttributeValueMap.Empty();
	//TArray<FString>& ArrayStringTitle = ArrayString2D(0);
	for ( INT k=2; k<tabFile->GetColCount(); k++ )
	{
		
		INT AttributeId = FindClientEnumValue(tabFile->GetData(k,0),TEXT("ECHAR_ATTR_TYPE")) ;
		//INT AttributeId = FindClientEnumValue(ArrayStringTitle(k),TEXT("ECHAR_ATTR_TYPE")) ;
		
		BaseAttributeValueMap.AddItem(AttributeId);
	}

	
		
	//vecAttribute2.Add(ArrayString2D.Num()-1);
	//标题已经处理
	for(INT i=0+1;i<tabFile->GetRowCount();i++)
	{
		INT len = tabFile->GetColCount();

		INT level = appAtoi(tabFile->GetData( 1,i));
		INT classID = appAtoi(tabFile->GetData( 0,i));

		STLIntVector Value; 
		for ( INT k = 2 ; k < len ; k++ )
		{
			INT AttributeValue = appAtoi(tabFile->GetData( k,i));

			Value.push_back(AttributeValue);
		}
		//查找操作较多，所以我把两个key值合并起来，会快些
		INT key = CombineClassLevel(level,classID);
		INT keyMax = INT(BaseAttributeValue.size());
		//if(key<keyMax)
		//{
			BaseAttributeValue[key] = Value;
		//}
	}
	delete tabFile;
	return TRUE;
}

Attribute2 MMOFacade::GetAttribute2ConfigStructData( INT nClass,INT AttributeType )
{
	Attribute2 res;
	// JO*(WJOSIJERg
	if( ( nClass>=clsPlayerBegin && nClass<clsPlayerEnd ) &&
		( AttributeType>= catBeginExAttr && AttributeType< catTotal ) ) 
	{
		for ( INT i = 0 ; i < vecAttribute2.Num() ; i++ )
		{
			if( vecAttribute2(i).ClassID == nClass && vecAttribute2(i).AttrName == AttributeType )
			{
				return vecAttribute2(i);
				
			}

		}
	}
	// JO*(WJOSIJERg end
	
	return res;
}

INT	  MMOFacade::GetAttribute2ConfigData( INT nLevel,INT nClass,INT AttributeType )
{
	// JO*(WJOSIJERg
	if( ( nLevel>0 && nLevel<=MAX_ROLE_LEVEL) &&
		( nClass>=clsPlayerBegin && nClass<clsPlayerEnd ) &&
		( AttributeType>= catBeginExAttr && AttributeType< catTotal ) )
	{
		for ( INT i = 0 ; i < vecAttribute2.Num() ; i++ )
		{
			if( vecAttribute2(i).ClassID == nClass && vecAttribute2(i).AttrName == AttributeType )
			{
				INT res = 0; 

				if( vecAttribute2(i).catStrength != 0 )
				{
					//res += GetAttributeConfigData( nLevel , nClass , catStrength ) * vecAttribute2(i).catStrength;
				}
				if( vecAttribute2(i).catAgility != 0 )
				{
					res += GetAttributeConfigData( nLevel , nClass , catAgility ) * vecAttribute2(i).catAgility;
				}
				if( vecAttribute2(i).catIntellect != 0 )
				{
					//res += GetAttributeConfigData( nLevel , nClass , catIntellect ) * vecAttribute2(i).catIntellect;
				}
				if( vecAttribute2(i).catStamina != 0 )
				{
					//res += GetAttributeConfigData( nLevel , nClass , catStamina ) * vecAttribute2(i).catStamina;
				}
				if( vecAttribute2(i).catSpirit != 0 )
				{
					//res += GetAttributeConfigData( nLevel , nClass , catSpirit ) * vecAttribute2(i).catSpirit;
				}
				//if( vecAttribute2(i).catValue != 0 )
				//{
				//	res += GetAttributeConfigData( nLevel , nClass , catValue ) * vecAttribute2(i).catValue;
				//}
				//if( vecAttribute2(i).catCombatParam != 0 )
				//{
				//	res += GetAttributeConfigData( nLevel , nClass , catCombatParam ) * vecAttribute2(i).catCombatParam;
				//}
				if( vecAttribute2(i).ConstParam != 0 )
				{
					res += vecAttribute2(i).ConstParam;
				}


				/*if(AttributeType>=catPhyAtkHi && AttributeType<=catMagAtkLo)
				{
					return res/1000000;
				}
				else{
					return res/10000;
				}*/
				return res/10000;
			}

		}
	}
	// JO*(WJOSIJERg end
	return 0;
}

INT   MMOFacade::GetAttributeConfigData(INT nLevel,INT nClass,INT AttributeType)
{

	if( ( nLevel>0 && nLevel<=MAX_ROLE_LEVEL) &&
	( nClass>=clsPlayerBegin && nClass<clsPlayerEnd ) ) 
	{
		INT key = CombineClassLevel(nLevel,nClass);
		Int2IntVectorMap::iterator itor = BaseAttributeValue.find(key);
		if( itor != BaseAttributeValue.end() )
		{
			STLIntVector& attributeList = itor->second;
			INT index = -1;
			for ( INT i = 0 ; i < BaseAttributeValueMap.Num() ; i++ )
			{
				if( BaseAttributeValueMap(i) == AttributeType )
				{
					index = i;
					break;
				}
			}
			if( index >= 0 )
			{
				return attributeList[index];
			}
		}
	}
	return 0;
}


FString MMOFacade::ClientEnum2ServerEnum(const FString& ClientEnum, INT EscapeNum)
{
	TCHAR buf[128];
	INT   size = (INT)ClientEnum.Len();
	const TCHAR* pClientEnum = (TCHAR*)*ClientEnum;
	INT   _Num = 0;
	buf[0] = '\0';

	for(INT i=1,j=0;i<size+1;i++)
	{
		if( *(pClientEnum+i)!='_' || _Num>=EscapeNum )
		{
			buf[j++] = *(pClientEnum+i);
		}
		else
		{
			_Num++;
		}
	}
	return FString(buf);
}


FString MMOFacade::FindClientEnumString( const FString& ServerEnum, const TCHAR* SerEnumType, INT EscapeNum ) 
{
	//一张静态字符对应表，仅此函数使用
	static 	TMap<FString,FString> CSEnumMap;
	const FString* pClientEnum = CSEnumMap.Find(ServerEnum);

	if(pClientEnum==NULL)
	{
		UEnum* Enum = FindObject<UEnum>(UServerEnum::StaticClass(), SerEnumType, TRUE);
		if(Enum){
			for(INT i=0;i<Enum->NumEnums();i++)
			{
				FString ClientEnumStr = FString(Enum->GetEnum(i).ToString());
				FString ServerEnumStr = ClientEnum2ServerEnum(ClientEnumStr, EscapeNum);
				CSEnumMap.Set(ServerEnumStr,ClientEnumStr);
			}
		}
		pClientEnum = CSEnumMap.Find(ServerEnum);
	}

	if(pClientEnum)
	{
		return *pClientEnum;
	}
	else
	{
		warnf(NAME_Log, TEXT("$$$$$$$$$$$$$$ 没有找到枚举:%s "), *ServerEnum );
		return FString(TEXT(""));
	}
}

INT MMOFacade::FindClientEnumValue( const FString& ServerEnum, const TCHAR* SerEnumType, INT EscapeNum )
{
	static TMap<FString,INT> MapBuff;
	const INT* pFinder = MapBuff.Find(ServerEnum);
	if(pFinder==NULL){
		INT Res = -1;
		UEnum* Enum = FindObject<UEnum>(UServerEnum::StaticClass(), SerEnumType, TRUE);
		if(Enum){
			FString buf = FindClientEnumString(ServerEnum, SerEnumType, EscapeNum);
			if(!buf.IsEmpty())
			{
				Res = Enum->FindEnumIndex(FName(*buf));
				MapBuff.Set(ServerEnum,Res);
			}
			else
			{
				warnf(NAME_Log, TEXT("############# 没有找到枚举:%s buf = %s"), *ServerEnum,*buf );
			}
		}
		return Res;
	}
	else{
		return *pFinder;
	}
}

FString MMOFacade::FindServerEnumString( INT ServerEnumID, const TCHAR* SerEnumType )
{
	UEnum* Enum = FindObject<UEnum>(UServerEnum::StaticClass(), SerEnumType, TRUE);
	if(Enum){
		return Enum->GetEnum(ServerEnumID).GetNameString();
	}
	return TEXT("");
}

FString MMOFacade::FindClientAttrName(const FString& ServerEnum, const TCHAR* SerEnumType)
{
	static TMap<FString,FString> CSEnumMap;
	const FString* pClientEnum = CSEnumMap.Find(ServerEnum);
	if(pClientEnum==NULL)
	{
		UEnum* Enum = FindObject<UEnum>(UServerEnum::StaticClass(), SerEnumType, TRUE);
		if(Enum)
		{
			for(INT i=0;i<Enum->NumEnums();i++)
			{
				FString ClientEnumStr = FString(Enum->GetEnum(i).ToString());
				FString ServerEnumStr = ClientEnum2ServerEnum(ClientEnumStr);
				CSEnumMap.Set(ServerEnumStr,ClientEnumStr);
			}
		}
		pClientEnum = CSEnumMap.Find(ServerEnum);
	}
	if(NULL != pClientEnum)
		return *pClientEnum;
	return ServerEnum;
}

INT MMOFacade::ClientStrToEnum(const FString& ClientEnum, const TCHAR* ClientEnumType)
{
	INT Res = -1;
	UEnum* Enum = FindObject<UEnum>(UTypeDefine::StaticClass(), ClientEnumType, TRUE);
	if(Enum)
	{
		Res = Enum->FindEnumIndex(FName(*ClientEnum));		
	}
	return Res;
}

UBOOL MMOFacade::ReadAttribute2Config()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("Facade::ReadAttribute2Config"), 0);

	//TArray<TArray<FString>> ArrayString2D;
//#if 0 // TABFILE
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("attributes2.txt"), TRUE ) )
//	if(!ReadConfigString( ArrayString2D, TEXT("attributes2.txt")))
//#endif
	{
		return FALSE;
	}

	//剔除标题行
	//static UEnum* Enum = FindObject<UEnum>(UServerEnum::StaticClass(), TEXT("ECHAR_ATTR_TYPE"), TRUE);
	vecAttribute2.Add(tabFile->GetRowCount()-1);
	for(INT i=0+2;i<tabFile->GetRowCount();i++)
	{
		
		//UINT len			= ArrayString.Num();

		vecAttribute2(i-1).ClassID  = appAtoi(tabFile->GetData( 0,i));
		vecAttribute2(i-1).AttrName =  FindClientEnumValue(tabFile->GetData(1,i),TEXT("ECHAR_ATTR_TYPE")) ;// Enum->FindEnumIndex(FName(*buf));

		vecAttribute2(i-1).ConstParam = appAtoi(tabFile->GetData( 2,i));
		vecAttribute2(i-1).catValue = appAtoi(tabFile->GetData( 3,i));
		vecAttribute2(i-1).catCombatParam = appAtoi(tabFile->GetData( 4,i));

	}
	delete tabFile;
	return TRUE;
}

UBOOL MMOFacade::ReadCensoredWordList(CensoredWordType type)
{
	SCOPE_LOG_TIME(___INIT__, TEXT("Facade::ReadCensoredWordList"), 0);

	//TArray<TArray<FString>> ArrayString2D;
	CTabFile* tabFile = NULL;
	FString fileName = "";
	switch(type)
	{
	case CensoredWord_All:
		fileName = TEXT("CensoredWordList.txt");
		break;
	case CensoredWord_Chat:
		fileName = TEXT("CensoredWordList_Chat.txt");
		break;
	case CensoredWord_ChatRecive:
		fileName = TEXT("CensoredWordList_ChatRecive.txt");
		break;
	}

	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, *fileName, TRUE ) )
	{
		return FALSE;
	}

	MMOUI::ClearCensoredWord(type);

	int num = tabFile->GetRowCount();
	for(int i=0;i<num;i++)
	{
		FString buf = tabFile->GetData(0,i);
		MMOUI::AddCensoredWord(buf,type);
	}

	MMOUI::BuildSortList(type);

	delete tabFile;
	return TRUE;
}

UBOOL MMOFacade::ReadForceRelative()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("Facade::ReadForceRelative"), 0);

	for(int i=0;i<MAX_NPC_FORCE_ID;i++)
	{
		for(int j=0;j<MAX_NPC_FORCE_ID;j++)
		{
			ForceRelative[i][j] = 0;
		}
	}


//#if 0 // TABFILE
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("role_relation.txt"), TRUE ) )
	//	if(!ReadConfigString( ArrayString2D, TEXT("role_relation.txt")))
//#endif
	{
		return FALSE;
	}

	//剔除标题行
	
	vecAttribute2.Add(tabFile->GetRowCount()-1);
	for(INT i=0+2;i<tabFile->GetRowCount();i++)
	{
		INT len			= tabFile->GetColCount();
		if(i-2 >= MAX_NPC_FORCE_ID)
		{
			M_AddDataError(TEXT("role_relation.txt"), FString::Printf(
				TEXT("声望表行数超出MAX_NPC_FORCE_ID") ));
			break;
		}

		for(INT j=1;j<len;j++){
			if(j-1 >= MAX_NPC_FORCE_ID)
			{
				M_AddDataError(TEXT("role_relation.txt"), FString::Printf(
					TEXT("声望表列数超出MAX_NPC_FORCE_ID")));
				break;
			}
			ForceRelative[i-2][j-1] = appAtoi(tabFile->GetData(j,i));
		}
	}
	delete tabFile;
	return TRUE;

}

//Bruce@snda
TArray<FString> UseMMOSplitString(const std::wstring& str, const std::wstring& delims, unsigned int maxSplits, bool preserveDelims)
{
	return MMO::SplitString(str, delims, maxSplits, preserveDelims);
}
//Bruce@snda

TArray<FString> MMO::SplitString( const std::wstring& str, const std::wstring& delims, unsigned int maxSplits, bool preserveDelims)
{
	TArray<FString> ret;
	// Pre-allocate some space for performance
	//ret.reserve(maxSplits ? maxSplits+1 : 10);    // 10 is guessed capacity for most case

	unsigned int numSplits = 0;

	// Use STL methods 
	size_t start, pos;
	start = 0;
	do 
	{
		pos = str.find_first_of(delims, start);
		if (pos == start)
		{
			// Do nothing
			start = pos + 1;
		}
		else if (pos == std::wstring::npos || (maxSplits && numSplits == maxSplits))
		{
			// Copy the rest of the string
			//ret.push_back( str.substr(start) );
			ret.AddItem(FString(str.substr(start).c_str()));
			break;
		}
		else
		{
			// Copy up to delimiter
			//ret.push_back( str.substr(start, pos - start) );
			ret.AddItem( FString(str.substr(start, pos - start).c_str()) );

			if(preserveDelims)
			{
				// Sometimes there could be more than one delimiter in a row.
				// Loop until we don't find any more delims
				size_t delimStart = pos, delimPos;
				delimPos = str.find_first_not_of(delims, delimStart);

				if (delimPos == std::wstring::npos)
				{
					// Copy the rest of the string
					//ret.push_back( str.substr(delimStart) );
					ret.AddItem( FString(str.substr(delimStart).c_str() ));
				}
				else
				{
					//ret.push_back( str.substr(delimStart, delimPos - delimStart) );
					ret.AddItem( FString(str.substr(delimStart, delimPos - delimStart).c_str()) );
				}
			}

			start = pos + 1;
		}
		// parse up to next real data
		start = str.find_first_not_of(delims, start);
		++numSplits;

	} while (pos != std::wstring::npos);



	return ret;
}

//add by JimZhang 读取二维配置表,返回二维FString
UBOOL MMOFacade::ReadConfigString( class CTabFile*& outTabFile, const TCHAR* ConfigName, bool needEmpty/* = false */, bool delQuotate/* = false */ )
{
	M_BeginLoadData(ConfigName);

	FString filePath;

#if SHIPPING_PC_GAME
	filePath = appGameConfigDir() + ConfigName;
#else

	FString baseDir = appBaseDir();
	filePath = baseDir + TEXT("..\\..\\..\\..\\NewServer\\bin\\commonconfig\\setting\\") + ConfigName;

	// 如果服务器配置目录没有就从客户端配置目录读
	if(GFileManager->FileSize(*filePath) <= 0)
	{
		filePath = appGameConfigDir() + ConfigName;
		filePath = GFileManager->ConvertToAbsolutePath(*filePath);

		SET_WARN_COLOR(COLOR_GREEN);
		warnf(NAME_Log, TEXT("############# Loading config from client config dir:%s"), *filePath);
		CLEAR_WARN_COLOR();
	}
 	else
 	{
 		SET_WARN_COLOR(COLOR_GREEN);
 		warnf(NAME_Log, TEXT("############# Loading config:%s"), *filePath );
 		CLEAR_WARN_COLOR();
 	}
#endif
 	
	outTabFile = NULL;

	CTabFile* tabfile = new CTabFile();
	if(!tabfile->Init(*filePath))
	{
		warnf(NAME_Error, TEXT("读取文件错误，请检查文件是否正确:%s"), *filePath);
		appMsgf(AMT_OK, TEXT("读取文件错误，请检查文件是否正确:%s"), *filePath);
		delete tabfile;
		return FALSE;
	}

	if(tabfile->GetColCount() == 0 || tabfile->GetRowCount() == 0)
	{
		appMsgf(AMT_OK, TEXT("%s 不能正确读取到数据，请检查文件格式必须以UNICODE文本存储！"), *filePath);
		delete tabfile;
		return FALSE;
	}

	outTabFile = tabfile;

// 	ArrayString2D.Empty();
// 	ArrayString2D.AddZeroed(tabfile.GetRowCount());
// 
// 	for (INT i = 0; i < tabfile.GetRowCount(); ++i)
// 	{
// 		TArray<FString>& tmpArr = ArrayString2D(i);
// 		tmpArr.AddZeroed(tabfile.GetColCount());
// 		for (INT j = 0; j < tabfile.GetColCount(); ++j)
// 		{
// 			tmpArr(j) = FString(tabfile.GetData(j, i));
// 		}
// 	}

	return TRUE;
}

class CTabFile* MMOFacade::ReadTabFile( const TCHAR* ConfigName )
{
	M_BeginLoadData(ConfigName);

	FString fileName = appGameConfigDir() + ConfigName;
	CTabFile* tabfileObj = new CTabFile();

	CTabFile& tabfile = *tabfileObj;

	if(!tabfileObj->Init(*fileName))
	{
		warnf(NAME_Error, TEXT("读取文件错误，请检查文件是否正确:%s"), ConfigName);
		appMsgf(AMT_OK, TEXT("读取文件错误，请检查文件是否正确:%s"), ConfigName);
		delete tabfileObj;
		return NULL;
	}

	if(tabfileObj->GetColCount() == 0 || tabfileObj->GetRowCount() == 0)
	{
		appMsgf(AMT_OK, TEXT("%s 不能正确读取到数据，请检查文件格式必须以UNICODE文本存储！"), *fileName);
		delete tabfileObj;
		return NULL;
	}

#if !SHIPPING_PC_GAME // oscar@snda 测试稳定了关掉以下代码
	FString testFile = fileName + TEXT(".txt");
	FILE* testFP = _tfopen(*testFile, TEXT("wb+"));
	UNICHAR BOM = UNICODE_BOM;
	fwrite(&BOM, sizeof(UNICHAR), 1, testFP);

	for (INT r = 0; r < tabfile.GetRowCount(); ++r)
	{
		for (INT c = 0; c < tabfile.GetColCount(); ++c)
		{
			FString str(tabfile.GetData(c, r));
			fwrite(*str, str.Len() * 2, 1, testFP);

			static const TCHAR tab = TEXT('\t');
			fwrite(&tab, sizeof(TCHAR), 1, testFP);
		}

		static const TCHAR lineEnd = TEXT('\r');
		fwrite(&lineEnd, sizeof(TCHAR), 1, testFP);
	}
	fclose(testFP);
#endif
 
	return tabfileObj;
}

//读取index
INT MMOFacade::GetColumnIndex(TArray<FString>& ArrayString, FString KeyName)
{
	for(INT i=0;i<ArrayString.Num();i++)
	{
		if(KeyName==ArrayString(i))
		{
			return i;
		}
	}
	warnf( NAME_Error, TEXT("read KeyName error:%s"), *KeyName);
	return 0;
}

void  MMOFacade::ReadHintNode(HintConfig& NpcHint, TArray<FString>& ArrayNodeName )
{
	NpcHint.NpcTypeName		= ArrayNodeName(0);
	NpcHint.NameVisible		= appAtoi(*(ArrayNodeName(1)));	
	NpcHint.StatusVisible	= appAtoi(*(ArrayNodeName(2)));	
	NpcHint.IconVisible		= appAtoi(*(ArrayNodeName(3)));	
	NpcHint.HPVisible		= appAtoi(*(ArrayNodeName(4)));	
}

void  MMOFacade::ReadHintNode(HintConfig& NpcHint,  FString NpcTypeName, INT NameVisible, INT StatusVisible, INT IconVisible, INT HPVisible )
{
	NpcHint.NpcTypeName		= NpcTypeName;
	NpcHint.NameVisible		= NameVisible;	
	NpcHint.StatusVisible	= StatusVisible;	
	NpcHint.IconVisible		= IconVisible;	
	NpcHint.HPVisible		= HPVisible;	
}

HintConfig*  MMOFacade::GetHintInfo(EActorClassType ActorType,ENPC_QUALITY quality )
{
	if(ActorType==EACT_Monster)return &NpcQualityHintInfo[quality];
	else	return &NpcHintInfo[ActorType];
}

//读取头顶文字配置列表
UBOOL MMOFacade::ReadHint()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("Facade::ReadHint"), 0);

	//TArray<TArray<FString>> ArrayString2D;
//#if 0 // TABFILE
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("hint.txt"), TRUE ) )
	//	if(!ReadConfigString( ArrayString2D, TEXT("hint.txt")))
//#endif
	{
		return FALSE;
	}

	INT num =  tabFile->GetRowCount()-1;//  ArrayString2D.Num()-1;

	ZeroMemory(NpcQualityHintInfo,sizeof(NpcQualityHintInfo));
	ZeroMemory(NpcHintInfo,sizeof(NpcHintInfo));

	if(num>=13){//13代表以下需读取13行ReadHintNode
		INT i=1;
		ReadHintNode(NpcQualityHintInfo[E_nq_Normal1], tabFile->GetData(0,i), appAtoi(tabFile->GetData(1,i)), appAtoi(tabFile->GetData(2,i)), appAtoi(tabFile->GetData(3,i)), appAtoi(tabFile->GetData(4,i)) );
		i++;
		ReadHintNode(NpcQualityHintInfo[E_nq_Normal2], tabFile->GetData(0,i), appAtoi(tabFile->GetData(1,i)), appAtoi(tabFile->GetData(2,i)), appAtoi(tabFile->GetData(3,i)), appAtoi(tabFile->GetData(4,i)) );
		i++;
		ReadHintNode(NpcQualityHintInfo[E_nq_Normal3], tabFile->GetData(0,i), appAtoi(tabFile->GetData(1,i)), appAtoi(tabFile->GetData(2,i)), appAtoi(tabFile->GetData(3,i)), appAtoi(tabFile->GetData(4,i)) );
		i++;
		ReadHintNode(NpcQualityHintInfo[E_nq_Normal4], tabFile->GetData(0,i), appAtoi(tabFile->GetData(1,i)), appAtoi(tabFile->GetData(2,i)), appAtoi(tabFile->GetData(3,i)), appAtoi(tabFile->GetData(4,i)) );
		i++;
		ReadHintNode(NpcQualityHintInfo[E_nq_Normal5], tabFile->GetData(0,i), appAtoi(tabFile->GetData(1,i)), appAtoi(tabFile->GetData(2,i)), appAtoi(tabFile->GetData(3,i)), appAtoi(tabFile->GetData(4,i)) );
		i++;
		ReadHintNode(NpcQualityHintInfo[E_nq_Elite],   tabFile->GetData(0,i), appAtoi(tabFile->GetData(1,i)), appAtoi(tabFile->GetData(2,i)), appAtoi(tabFile->GetData(3,i)), appAtoi(tabFile->GetData(4,i)) );
		i++;
		ReadHintNode(NpcQualityHintInfo[E_nq_Rare],	   tabFile->GetData(0,i), appAtoi(tabFile->GetData(1,i)), appAtoi(tabFile->GetData(2,i)), appAtoi(tabFile->GetData(3,i)), appAtoi(tabFile->GetData(4,i)) );
		i++;
		ReadHintNode(NpcQualityHintInfo[E_nq_Boss],    tabFile->GetData(0,i), appAtoi(tabFile->GetData(1,i)), appAtoi(tabFile->GetData(2,i)), appAtoi(tabFile->GetData(3,i)), appAtoi(tabFile->GetData(4,i)) );
		i++;
		ReadHintNode(NpcHintInfo[EACT_NPC],			   tabFile->GetData(0,i), appAtoi(tabFile->GetData(1,i)), appAtoi(tabFile->GetData(2,i)), appAtoi(tabFile->GetData(3,i)), appAtoi(tabFile->GetData(4,i)) );
		i++;
		ReadHintNode(NpcHintInfo[EACT_Function_NPC],   tabFile->GetData(0,i), appAtoi(tabFile->GetData(1,i)), appAtoi(tabFile->GetData(2,i)), appAtoi(tabFile->GetData(3,i)), appAtoi(tabFile->GetData(4,i)) );
		i++;
		ReadHintNode(NpcHintInfo[EACT_Toy],			   tabFile->GetData(0,i), appAtoi(tabFile->GetData(1,i)), appAtoi(tabFile->GetData(2,i)), appAtoi(tabFile->GetData(3,i)), appAtoi(tabFile->GetData(4,i)) );
		i++;
		ReadHintNode(NpcHintInfo[EACT_Pet],			   tabFile->GetData(0,i), appAtoi(tabFile->GetData(1,i)), appAtoi(tabFile->GetData(2,i)), appAtoi(tabFile->GetData(3,i)), appAtoi(tabFile->GetData(4,i)) );
		i++;
		ReadHintNode(NpcHintInfo[EACT_Boss],		   tabFile->GetData(0,i), appAtoi(tabFile->GetData(1,i)), appAtoi(tabFile->GetData(2,i)), appAtoi(tabFile->GetData(3,i)), appAtoi(tabFile->GetData(4,i)) );
	}
	delete tabFile;
	return TRUE;
}


//读取掉落轨迹表
extern FLOAT g_Trajectory[5][192][2];
UBOOL MMOFacade::ReadTestDropTrace()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("Facade::ReadTestDropTrace"), 0);
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("TestDropTrace.txt"), TRUE ) )
	{
		return FALSE;
	}

	for (int i = 1; i < tabFile->GetRowCount(); ++i)
	{
		if(i-1<5){
			for(INT j=0;j<192;j++){
				if(j<192){
					g_Trajectory[i-1][j][0]		= appAtof(tabFile->GetData( j*2,i));	
					g_Trajectory[i-1][j][1]		= appAtof(tabFile->GetData( j*2+1,i));	
				}
			}
		}
	}
	return TRUE;
}

//读取Tween列表
UBOOL MMOFacade::ReadTweenData()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("Facade::ReadTweenData"), 0);

	//TArray<TArray<FString>> ArrayString2D;
//#if 0 // TABFILE
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("TweenData.txt"), TRUE ) )
		//	if(!ReadConfigString( ArrayString2D, TEXT("TweenData.txt")))
//#endif
	{
		return FALSE;
	}

	_TweenDataMap.clear();
	//剔除标题行
	for (int i = 1; i < tabFile->GetRowCount(); ++i)
	{

		TweenData T;

		FString KeyName = tabFile->GetData( 0,i);
		T.sx			=appAtof(tabFile->GetData( 1,i));	
		T.sy			=appAtof(tabFile->GetData( 2,i));	
		T.sz			=appAtof(tabFile->GetData( 3,i));

		T.vx			=appAtof(tabFile->GetData( 4,i));	
		T.vy			=appAtof(tabFile->GetData( 5,i));	
		T.vz			=appAtof(tabFile->GetData( 6,i));	
		
		T.ax			=appAtof(tabFile->GetData( 7,i));	
		T.ay			=appAtof(tabFile->GetData( 8,i));	
		T.az			=appAtof(tabFile->GetData( 9,i));	

		T.sxRange		=appAtof(tabFile->GetData( 10,i));	
		T.syRange		=appAtof(tabFile->GetData( 11,i));	
		T.szRange		=appAtof(tabFile->GetData( 12,i));

		T.vxRange		=appAtof(tabFile->GetData( 13,i));	
		T.vyRange		=appAtof(tabFile->GetData( 14,i));	
		T.vzRange		=appAtof(tabFile->GetData( 15,i));	

		T.axRange		=appAtof(tabFile->GetData( 16,i));	
		T.ayRange		=appAtof(tabFile->GetData( 17,i));	
		T.azRange		=appAtof(tabFile->GetData( 18,i));	

		T.scaleStart	=appAtof(tabFile->GetData( 19,i));	
		T.scaleBomb		=appAtof(tabFile->GetData( 20,i));	
		T.scaleEnd		=appAtof(tabFile->GetData( 21,i));	

		T.scaleStartRange	=appAtof(tabFile->GetData( 22,i));	
		T.scaleBombRange	=appAtof(tabFile->GetData( 23,i));	
		T.scaleEndRange		=appAtof(tabFile->GetData( 24,i));

		T.durTime		=appAtof(tabFile->GetData( 25,i));	
		T.bombTime		=appAtof(tabFile->GetData( 26,i));	
		T.dumping		=appAtof(tabFile->GetData( 27,i));	

		T.alphaStart	=appAtof(tabFile->GetData( 28,i));	
		T.alphaBomb	=appAtof(tabFile->GetData( 29,i));	
		T.alphaEnd		=appAtof(tabFile->GetData( 30,i));	
		T.alphaPow		=appAtof(tabFile->GetData( 31,i));	
		T.isVisible		= appAtoi(tabFile->GetData( 32,i));

		_TweenDataMap.insert(std::make_pair(KeyName, T));

	}
	delete tabFile;
	return TRUE;
}


//读取声望表
UBOOL MMOFacade::ReadReputation()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("Facade::ReadReputation"), 0);

	CTabFile* tabFile = NULL;
	if(!ReadConfigString( tabFile, TEXT("Reputation.txt"), TRUE ) )
	{
		return FALSE;
	}

	INT _ForceID				= tabFile->GetKeyColIdx( TEXT("ForceID"));
	INT _CanAttackWithFreePkMode= tabFile->GetKeyColIdx( TEXT("CanAttackWithFreePkMode"));

	INT _WarriorAwardItemID[MAX_REPUTATION_AWARD_COUNT];
	INT _WarriorAwardItemCount[MAX_REPUTATION_AWARD_COUNT];
	for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
	{
		FString headName = FString::Printf(TEXT("WarriorAwardItemID%d"), i + 1);
		_WarriorAwardItemID[i] = tabFile->GetKeyColIdx(*headName);
		headName = FString::Printf(TEXT("WarriorAwardItemCount%d"), i + 1);
		_WarriorAwardItemCount[i] = tabFile->GetKeyColIdx(*headName);
	}
	
	INT _MageAwardItemID[MAX_REPUTATION_AWARD_COUNT];
	INT _MageAwardItemCount[MAX_REPUTATION_AWARD_COUNT];
	for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
	{
		FString headName = FString::Printf(TEXT("MageAwardItemID%d"), i + 1);
		_MageAwardItemID[i] = tabFile->GetKeyColIdx(*headName);
		headName = FString::Printf(TEXT("MageAwardItemCount%d"), i + 1);
		_MageAwardItemCount[i] = tabFile->GetKeyColIdx(*headName);
	}

	INT _RogueAwardItemID[MAX_REPUTATION_AWARD_COUNT];
	INT _RogueAwardItemCount[MAX_REPUTATION_AWARD_COUNT];
	for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
	{
		FString headName = FString::Printf(TEXT("RogueAwardItemID%d"), i + 1);
		_RogueAwardItemID[i] = tabFile->GetKeyColIdx(*headName);
		headName = FString::Printf(TEXT("RogueAwardItemCount%d"), i + 1);
		_RogueAwardItemCount[i] = tabFile->GetKeyColIdx(*headName);
	}

	INT _ArcherAwardItemID[MAX_REPUTATION_AWARD_COUNT];
	INT _ArcherAwardItemCount[MAX_REPUTATION_AWARD_COUNT];
	for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
	{
		FString headName = FString::Printf(TEXT("ArcherAwardItemID%d"), i + 1);
		_ArcherAwardItemID[i] = tabFile->GetKeyColIdx(*headName);
		headName = FString::Printf(TEXT("ArcherAwardItemCount%d"), i + 1);
		_ArcherAwardItemCount[i] = tabFile->GetKeyColIdx(*headName);
	}

	INT _TaoistAwardItemID[MAX_REPUTATION_AWARD_COUNT];
	INT _TaoistAwardItemCount[MAX_REPUTATION_AWARD_COUNT];
	for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
	{
		FString headName = FString::Printf(TEXT("TaoistAwardItemID%d"), i + 1);
		_TaoistAwardItemID[i] = tabFile->GetKeyColIdx(*headName);
		headName = FString::Printf(TEXT("TaoistAwardItemCount%d"), i + 1);
		_TaoistAwardItemCount[i] = tabFile->GetKeyColIdx(*headName);
	}

	//任务 #14866
	INT _InitValue				= tabFile->GetKeyColIdx( TEXT("InitValue"));
	INT _MaxValue				= tabFile->GetKeyColIdx( TEXT("MaxValue"));

	ReputationTemp buf[MAX_NPC_FORCE_ID];

	for (int i = 2; i < tabFile->GetRowCount(); ++i)
	{
		INT ForceID				=	appAtoi(tabFile->GetData( _ForceID,i));
		if(ForceID>=0&&ForceID<MAX_NPC_FORCE_ID){
			buf[ForceID].ForceID	= 	ForceID;
			buf[ForceID]._CanAttackWithFreePkMode   =	appAtoi(tabFile->GetData( _CanAttackWithFreePkMode,i));
			_CanAttackWithFreePkModeArray[ForceID]		=	buf[ForceID]._CanAttackWithFreePkMode;

			for (int j = 0; j < MAX_REPUTATION_AWARD_COUNT; ++ j)
			{
				buf[ForceID]._WarriorAwardItemID[j]		= 	appAtoi(tabFile->GetData(_WarriorAwardItemID[j],i));
				buf[ForceID]._WarriorAwardItemCount[j]	= 	appAtoi(tabFile->GetData(_WarriorAwardItemCount[j],i));

				buf[ForceID]._MageAwardItemID[j]		= 	appAtoi(tabFile->GetData(_MageAwardItemID[j],i));
				buf[ForceID]._MageAwardItemCount[j]		= 	appAtoi(tabFile->GetData(_MageAwardItemCount[j],i));

				buf[ForceID]._RogueAwardItemID[j]		= 	appAtoi(tabFile->GetData(_RogueAwardItemID[j],i));
				buf[ForceID]._RogueAwardItemCount[j]	= 	appAtoi(tabFile->GetData(_RogueAwardItemCount[j],i));

				buf[ForceID]._ArcherAwardItemID[j]		= 	appAtoi(tabFile->GetData(_ArcherAwardItemID[j],i));
				buf[ForceID]._ArcherAwardItemCount[j]	= 	appAtoi(tabFile->GetData(_ArcherAwardItemCount[j],i));

				buf[ForceID]._TaoistAwardItemID[j]		= 	appAtoi(tabFile->GetData(_TaoistAwardItemID[j],i));
				buf[ForceID]._TaoistAwardItemCount[j]	= 	appAtoi(tabFile->GetData(_TaoistAwardItemCount[j],i));
			}

			//任务 #14866
			buf[ForceID].InitValue					=   appAtoi(tabFile->GetData( _InitValue,i));
			buf[ForceID].MaxValue					=   appAtoi(tabFile->GetData( _MaxValue,i));
		}
	}

	for(INT i=0;i<_ReputationTempArray.Num();i++)
	{
		INT ForceID	= _ReputationTempArray(i).ForceID;
		if(ForceID>=0&&ForceID<MAX_NPC_FORCE_ID)
		{

			_ReputationTempArray(i)._CanAttackWithFreePkMode	=	buf[ForceID]._CanAttackWithFreePkMode;

			for (int j = 0; j < MAX_REPUTATION_AWARD_COUNT; ++ j)
			{
				_ReputationTempArray(i)._WarriorAwardItemID[j]		=	buf[ForceID]._WarriorAwardItemID[j];
				_ReputationTempArray(i)._WarriorAwardItemCount[j]	=	buf[ForceID]._WarriorAwardItemCount[j];

				_ReputationTempArray(i)._MageAwardItemID[j]			=	buf[ForceID]._MageAwardItemID[j];
				_ReputationTempArray(i)._MageAwardItemCount[j]		=	buf[ForceID]._MageAwardItemCount[j];

				_ReputationTempArray(i)._RogueAwardItemID[j]		=	buf[ForceID]._RogueAwardItemID[j];
				_ReputationTempArray(i)._RogueAwardItemCount[j]		=	buf[ForceID]._RogueAwardItemCount[j];

				_ReputationTempArray(i)._ArcherAwardItemID[j]		=	buf[ForceID]._ArcherAwardItemID[j];
				_ReputationTempArray(i)._ArcherAwardItemCount[j]	=	buf[ForceID]._ArcherAwardItemCount[j];

				_ReputationTempArray(i)._TaoistAwardItemID[j]		=	buf[ForceID]._TaoistAwardItemID[j];
				_ReputationTempArray(i)._TaoistAwardItemCount[j]	=	buf[ForceID]._TaoistAwardItemCount[j];
			}

			//任务 #14866
			_ReputationTempArray(i).InitValue					=	buf[ForceID].InitValue;
			_ReputationTempArray(i).MaxValue					=	buf[ForceID].MaxValue;
		}
	}


	return TRUE;
}


//读取声望表
UBOOL MMOFacade::ReadReputationTemp()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("Facade::ReadReputationTemp"), 0);

	//TArray<TArray<FString>> ArrayString2D;
//#if 0 // TABFILE
	CTabFile* tabFile = NULL;
	if(!ReadConfigString( tabFile, TEXT("ReputationTemp.txt"), TRUE ) )
	//	if(!ReadConfigString( ArrayString2D, TEXT("ReputationTemp.txt")))
//#endif
	{
		return FALSE;
	}
	

	INT _Index					= tabFile->GetKeyColIdx( TEXT("Index"));
	INT _ForceID				= tabFile->GetKeyColIdx( TEXT("ForceID"));
	INT _ListLevel				= tabFile->GetKeyColIdx( TEXT("ListLevel"));
	INT _ForceName				= tabFile->GetKeyColIdx( TEXT("ForceName"));
	INT _ForceVisible			= tabFile->GetKeyColIdx( TEXT("ForceVisible"));
	//INT _InitValue				= tabFile->GetKeyColIdx( TEXT("InitValue"));
	//INT _MaxValue				= tabFile->GetKeyColIdx( TEXT("MaxValue"));
	INT _SceneID				= tabFile->GetKeyColIdx( TEXT("SceneID"));
	INT _ForceTips				= tabFile->GetKeyColIdx( TEXT("ForceTips"));
	INT _ForceInfo				= tabFile->GetKeyColIdx( TEXT("ForceInfo"));

	//INT _ForceAward1				= tabFile->GetKeyColIdx( TEXT("ForceAward1"));
	//INT _ForceAward2				= tabFile->GetKeyColIdx( TEXT("ForceAward2"));
	//INT _ForceAward3				= tabFile->GetKeyColIdx( TEXT("ForceAward3"));
	//INT _ForceAward4				= tabFile->GetKeyColIdx( TEXT("ForceAward4"));

	ZeroMemory(_CanAttackWithFreePkModeArray,sizeof(_CanAttackWithFreePkModeArray));
	_ReputationTempArray.Empty();
	//剔除标题行
	for (int i = 1; i < tabFile->GetRowCount(); ++i)
	{
		ReputationTemp T;

		T.Index					=appAtoi(tabFile->GetData( _Index,i));
		T.ForceID				=appAtoi(tabFile->GetData( _ForceID,i));
		T.ListLevel				=appAtoi(tabFile->GetData( _ListLevel,i));
		T.ForceName				=tabFile->GetData( _ForceName,i);
		T.ForceVisible			=appAtoi(tabFile->GetData( _ForceVisible,i));
		
		//任务 #14866
		//T.InitValue				=appAtoi(tabFile->GetData( _InitValue,i));
		//T.MaxValue				=appAtoi(tabFile->GetData( _MaxValue,i));
		
		T.SceneID				=appAtoi(tabFile->GetData( _SceneID,i)); 
		T.ForceTips				=tabFile->GetData( _ForceTips,i);
		T.ForceInfo				=tabFile->GetData( _ForceInfo,i);
		//T.ForceAward[0]			=tabFile->GetData( _ForceAward1,i); 
		//T.ForceAward[1]			=tabFile->GetData( _ForceAward2,i); 
		//T.ForceAward[2]			=tabFile->GetData( _ForceAward3,i); 
		//T.ForceAward[3]			=tabFile->GetData( _ForceAward4,i); 

		_ReputationTempArray.Push(T);
	}
	delete tabFile;
	return TRUE;
}
//读取mount列表
UBOOL MMOFacade::ReadMountData()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("Facade::ReadMountData"), 0);

	//TArray<TArray<FString>> ArrayString2D;
//#if 0 // TABFILE
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("mount.txt"), TRUE ) )
// 		if(!ReadConfigString( ArrayString2D, TEXT("mount.txt"), TRUE ))
//#endif
	{
		return FALSE;
	}

	INT _ID						= tabFile->GetKeyColIdx( TEXT("ID"));
	INT _Name					= tabFile->GetKeyColIdx( TEXT("Name"));
	INT _Quality				= tabFile->GetKeyColIdx( TEXT("Quality"));
	INT _LevelLimit				= tabFile->GetKeyColIdx( TEXT("LevelLimit"));
	INT _SpeedModificationRate	= tabFile->GetKeyColIdx( TEXT("SpeedModificationRate"));
	INT _HitUnmountRate			= tabFile->GetKeyColIdx( TEXT("HitUnmountRate"));
	INT _ItemID1				= tabFile->GetKeyColIdx( TEXT("ItemID1"));
	INT _ItemCount1				= tabFile->GetKeyColIdx( TEXT("ItemCount1"));
	INT _ItemID2				= tabFile->GetKeyColIdx( TEXT("ItemID2"));
	INT _ItemCount2				= tabFile->GetKeyColIdx( TEXT("ItemCount2"));
	INT _ItemID3				= tabFile->GetKeyColIdx( TEXT("ItemID3"));
	INT _ItemCount3				= tabFile->GetKeyColIdx( TEXT("ItemCount3"));
	INT _IconName				= tabFile->GetKeyColIdx( TEXT("Icon"));
	INT _Script					= tabFile->GetKeyColIdx( TEXT("Script"));

	_MountDataMap.clear();
	//剔除标题行
	for (int i = 2; i < tabFile->GetRowCount(); ++i)
	{
		MountData T;

		T.ID					=appAtoi(tabFile->GetData( _ID,i));	

		T.Name					=tabFile->GetData( _Name,i);
		T.Quality				=appAtoi(tabFile->GetData( _Quality,i));
		T.LevelLimit			=appAtoi(tabFile->GetData( _LevelLimit,i));
		T.SpeedModificationRate	=appAtoi(tabFile->GetData( _SpeedModificationRate,i));
		T.HitUnmountRate		=appAtoi(tabFile->GetData( _HitUnmountRate,i));
		T.ItemID1				=appAtoi(tabFile->GetData( _ItemID1,i));	
		T.ItemCount1			=appAtoi(tabFile->GetData( _ItemCount1,i));	
		T.ItemID2				=appAtoi(tabFile->GetData( _ItemID2,i));	
		T.ItemCount2			=appAtoi(tabFile->GetData( _ItemCount2,i));	
		T.ItemID3				=appAtoi(tabFile->GetData( _ItemID3,i));	
		T.ItemCount3			=appAtoi(tabFile->GetData( _ItemCount3,i));	

		T.IconName				=tabFile->GetData( _IconName,i);
		T.Script				=tabFile->GetData( _Script,i);
		T.Have					=0;

		_MountDataMap.insert(std::make_pair(T.ID, T));

	}


	//TArray<TArray<FString>> ArrayString2D_Client;
	//if(!ReadConfigString( ArrayString2D_Client, TEXT("mountScript.txt")))
	//{
	//	return FALSE;
	//}
	////剔除标题行
	//for(INT i=0+1;i<ArrayString2D_Client.Num();i++)
	//{
	//	TArray<FString>& ArrayString = ArrayString2D_Client(i);
	//	INT ID = appAtoi(tabFile->GetData( 0));	
	//	MountData* pMount = GetMountData(ID);
	//	if(pMount)
	//	{
	//		pMount->Name					=ArrayString(1);
	//		pMount->IconName				=ArrayString(2);
	//		pMount->Script				=ArrayString(3); 
	//	}
	//}
	delete tabFile;
	return TRUE;
}

UBOOL MMOFacade::RefreshMountData()
{
	if(GetMainPlayer() )
	{
		UActorAttribute* p = GetMainPlayer()->ActorAttributes;
		if(p)
		{
			//先清除所有的得到标志
			for(MountDataMap::iterator iter=_MountDataMap.begin();iter!=_MountDataMap.end();iter++ )
			{
				iter->second.Have = FALSE;
			}	

			//置上已得标志
			for(int i=0; i < p->MountList.Num(); ++i )
			{
				MountData* pMountData =  MMOFacade::getSingleton().GetMountData(p->MountList(i));
				if(pMountData)
				{
					pMountData->Have = TRUE;
				}
			}
		}
		MMOUI::GetUIManager()->delegateRefreshMount();
	}	
	return TRUE;
}

//读取称号列表
UBOOL MMOFacade::ReadTitle()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("Facade::ReadTitle"), 0);

	//TArray<TArray<FString>> ArrayString2D;
//#if 0 // TABFILE
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("title.txt"), TRUE ) )
		//if(!ReadConfigString( ArrayString2D, TEXT("title.txt")))
//#endif
	{
		return FALSE;
	}

	INT _ID						= tabFile->GetKeyColIdx( TEXT("ID"));
	INT _ValidTime				= tabFile->GetKeyColIdx( TEXT("ValidTime"));
	INT _pow					= tabFile->GetKeyColIdx( TEXT("pow"));
	INT _AttrID1				= tabFile->GetKeyColIdx( TEXT("AttrID1"));
	INT _Tip					= tabFile->GetKeyColIdx( TEXT("Tip"));
	INT _AttrID2				= tabFile->GetKeyColIdx( TEXT("AttrID2"));
	INT _AttrID3				= tabFile->GetKeyColIdx( TEXT("AttrID3"));
	INT _AttrID4				= tabFile->GetKeyColIdx( TEXT("AttrID4"));
	INT _AttrID5				= tabFile->GetKeyColIdx( TEXT("AttrID5"));

	INT _ConditionType1				= tabFile->GetKeyColIdx( TEXT("ConditionType1"));
	INT _ConditionValue1			= tabFile->GetKeyColIdx( TEXT("ConditionValue1"));
	INT _ConditionType2				= tabFile->GetKeyColIdx( TEXT("ConditionType2"));
	INT _ConditionValue2			= tabFile->GetKeyColIdx( TEXT("ConditionValue2"));


	INT _TitleName			= tabFile->GetKeyColIdx( TEXT("TitleName"));
	INT _TitleInfo			= tabFile->GetKeyColIdx( TEXT("TitleInfo"));
	INT _TitleLevel			= tabFile->GetKeyColIdx( TEXT("TitleLevel"));

	INT _TitleStyle 		= tabFile->GetKeyColIdx( TEXT("TitleStyle"));
	INT _TitleType			= tabFile->GetKeyColIdx( TEXT("TitleType"));

	INT _HoverShow			= tabFile->GetKeyColIdx( TEXT("HoverShow"));

	_TitleDataMap.clear();
	//剔除标题行
	for (int i = 2; i < tabFile->GetRowCount(); ++i)
	{
		TitleData T;

		T.IsNew					=FALSE;
		T.ID					=appAtoi(tabFile->GetData( _ID,i));	
		T.ValidTime				=appAtoi(tabFile->GetData( _ValidTime,i));

		T.Pow					=appAtoi(tabFile->GetData( _pow,i)); 
		T.AttrID[0]				=appAtoi(tabFile->GetData( _AttrID1,i)); 
		T.AttrID[1]				=appAtoi(tabFile->GetData( _AttrID2,i)); 
		T.AttrID[2]				=appAtoi(tabFile->GetData( _AttrID3,i)); 
		T.AttrID[3]				=appAtoi(tabFile->GetData( _AttrID4,i)); 
		T.AttrID[4]				=appAtoi(tabFile->GetData( _AttrID5,i)); 

		T.ConditionType1		=tabFile->GetData( _ConditionType1,i);
		T.ConditionValue1		=appAtoi(tabFile->GetData( _ConditionValue1,i));
		T.ConditionType2		=tabFile->GetData( _ConditionType2,i);
		T.ConditionValue2		=appAtoi(tabFile->GetData( _ConditionValue2,i));

		T.TitleName				=tabFile->GetData( _TitleName,i);
		T.TitleInfo				=tabFile->GetData( _TitleInfo,i);
		T.TitleLevel			=appAtoi(tabFile->GetData( _TitleLevel,i));
		T.TitleStyle			=appAtoi(tabFile->GetData( _TitleStyle,i));
		T.TitleType				=appAtoi(tabFile->GetData( _TitleType,i));
		T.HoverShow				=appAtoi(tabFile->GetData( _HoverShow,i));

		T.ValidTime = 0;
		// 		T.Star = i%5+1;//星数策划忘配，先写来测试
		_TitleDataMap.insert(std::make_pair(T.ID, T));

	}
	delete tabFile;
	return TRUE;
}

//读取pk值分级别列表
UBOOL MMOFacade::ReadPkLevel()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("Facade::ReadPkLevel"), 0);

	//TArray<TArray<FString>> ArrayString2D;
//#if 0 // TABFILE
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("pklist.txt"), TRUE ) )
// 		if(!ReadConfigString( ArrayString2D, TEXT("pklist.txt")))
//#endif
	{
		return FALSE;
	}

	vecPkLevel.Init( tabFile->GetRowCount()-1 );
	//剔除标题行
	for(INT i=0+1;i<tabFile->GetRowCount();i++)
	{
		//TArray<FString>& ArrayString = ArrayString2D(i);
		vecPkLevel(i-1) = appAtoi(tabFile->GetData( 0,i));	
	}
	delete tabFile;
	return TRUE;
}

static QSORT_RETURN CDECL CompareTitle( const TitleData* AA, const TitleData* AB )
{
	if(AA&&AB)
	{
		if(AA->TitleLevel>AB->TitleLevel)return -1;
	}
	return 1;
}

void MMOFacade::TickTitleTime(FLOAT deltaTime)
{
	CTRL_TICK("MMOFacade::TickTitleTime");

	static FLOAT timeProcess = 0.0;
	timeProcess += deltaTime;
	UBOOL bRefresh = FALSE;
	if(timeProcess>1.0)
	{
		timeProcess -= 1.0;
		for(TitleDataMap::iterator i=_TitleDataMap.begin();i!=_TitleDataMap.end();i++ )
		{
			if( i->second.ValidTime>0 ){
				bRefresh = TRUE;
				i->second.ValidTime--;
			}
		}	
	}
	if(bRefresh)
	{
		MMOUI::GetUIManager()->delegateUpdateTitleData(FALSE);
	}
}

UBOOL MMOFacade::UpdateTitleArray()
{
	_TitleDataArray.Empty();
	for(TitleDataMap::iterator i=_TitleDataMap.begin();i!=_TitleDataMap.end();i++ )
	{
		_TitleDataArray.Push(i->second);
	}
	appQsort(&_TitleDataArray(0),_TitleDataArray.Num(),sizeof(TitleData), (QSORT_COMPARE)CompareTitle );

	return TRUE;
}

//获取一个技能的下一级
UMMOAbility *MMOFacade::GetAbilityByNextLevel( UMMOAbility *ability )
{
	return MMOSkillManager::getSingleton().GetAbilityByNextLevel( ability );
}

//获取一个技能的下一级(带符文)
UMMOAbility *MMOFacade::GetAbilityWithRuneByNextLevel( UMMOAbility *ability )
{
	return MMOSkillManager::getSingleton().GetAbilityWithRuneByNextLevel( ability );
}

//取得指定id的随机属性
const ItemRandomAttr* MMOFacade::GetRandomAttributById( int id )
{
	Int2ItemAttrMap::iterator iter =RandomAttributesMap.find(id);
	if (iter != RandomAttributesMap.end())
		return &(iter->second);
	return NULL;
}

//
FString MMOFacade::GetRandomAttributeFormatedValue(const ItemRandomAttr* pItemRandomAttr)
{
	FString strValue = TEXT("");
	LOG_PROCESS_ERROR(pItemRandomAttr);
	switch(pItemRandomAttr->formatType)
	{
	case E_cmi_A:
		{
			//if (pItemRandomAttr->attrType == TEXT("E_cat_PhyAtkHi")
			//	|| pItemRandomAttr->attrType == TEXT("E_cat_PhyAtkLo")
			//	|| pItemRandomAttr->attrType == TEXT("E_cat_MagAtkHi")
			//	|| pItemRandomAttr->attrType == TEXT("E_cat_MagAtkLo"))
			//{

			//	strValue = appItoa(pItemRandomAttr->value/10000);
			//}
			//else
			//{
				strValue = appItoa(pItemRandomAttr->value);
			//}
		}
	break;
	case E_cmi_B:
		{
			strValue = FString::Printf(TEXT("%5.2f%%"), pItemRandomAttr->value/100.f);
		}
		break;
	default:
		{
			strValue = appItoa(pItemRandomAttr->value);
		}
		break;
	}
	return strValue;
Exit0:
	return TEXT("");
}

//
INT MMOFacade::IsSameClassRandomAttributeBetter( const ItemRandomAttr* pAttr, const ItemRandomAttr* pCompareToAttr )
{
	if( pAttr == NULL || pCompareToAttr == NULL ){
		return FALSE;
	}

	//
	if( pAttr->formatType != pCompareToAttr->formatType ){
		return FALSE;
	}

	INT isBetter = 0;
	switch(pAttr->formatType)
	{
	case E_cmi_A:
	case E_cmi_B:
		{
			isBetter = INT( pAttr->value - pCompareToAttr->value );
			break;
		}
	default:
		{
			isBetter = INT( pAttr->value - pCompareToAttr->value );
			break;
		}
	}
	return isBetter;
}

//读取随机属性
UBOOL MMOFacade::ReadRandomAttribute()
{
    //TArray<TArray<FString>> ArrayString2D;
//#if 0 // TABFILE
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("attribute.txt"), TRUE ) )
    //剔除标题行
    //if(!ReadConfigString( ArrayString2D, TEXT("attribute.txt"), true, true))
    {
        return FALSE;
    }
//#endif
    
	RandomAttributesMap.clear();

	UINT size = tabFile->GetRowCount(); //ArrayString2D.Num();
	FString strEnum("E_");
	FString strBuff;
	ItemRandomAttr attr;
	ZeroMemory(&attr, sizeof(ItemRandomAttr));

    for(UINT i = M_SKIP_ROWS; i < size; ++i )
    {
        //TArray<FString>& ArrayString = ArrayString2D(i);
		UINT len = tabFile->GetColCount(); // ArrayString.Num();
			
		//这个配置表项填的特殊 先处理下
		//attr.formatType = (ECHAR_MODIFIER_INDEX)FindClientEnumValue(ClientEnum2ServerEnum(strEnum+ArrayString(1)), TEXT("ECHAR_MODIFIER_INDEX"));
		attr.formatType = (ECHAR_MODIFIER_INDEX)FindClientEnumValue(ClientEnum2ServerEnum(strEnum+tabFile->GetData(TEXT("Key"),i)), TEXT("ECHAR_MODIFIER_INDEX"));
		if(attr.formatType == E_cmi_AllNpcSpecies )
		{
			attr.attrType = TEXT("");
		}
		else if(attr.formatType == E_cmi_A ||attr.formatType == E_cmi_B)
		{
			attr.attrType =  FindClientEnumString(tabFile->GetData(TEXT("Param1"),i),TEXT("ECHAR_ATTR_TYPE"),2 );
		}
		else
		{
			attr.attrType =   tabFile->GetData(TEXT("Param1"),i);//ArrayString(2);
		}
		//attr.attrType =   ArrayString(2).Len()<=1 ? TEXT("") : FindClientEnumString(ArrayString(2),TEXT("ECHAR_ATTR_TYPE"),2 );	//FindClientAttrName(ArrayString(2), TEXT("ECHAR_ATTR_TYPE"));
		attr.value = appAtoi(tabFile->GetData(TEXT("Param2"),i)); 
		attr.power = appAtoi(tabFile->GetData(TEXT("Pow"),i));
		attr.strText = tabFile->GetData(TEXT("Tip"),i);
		strBuff = tabFile->GetData(TEXT("WeatherCond"),i);

		attr.weatherType = strBuff.Len()<=1 ? E_swt_Invalid : (ESCENE_WEATHER_TYPE)FindClientEnumValue(strBuff, TEXT("ESCENE_WEATHER_TYPE"));

		strBuff = tabFile->GetData(TEXT("DaytimeCond"), i);
		attr.dayType = strBuff.Len()<=1? E_sdt_Total : (ESCENE_DAYTIME_TYPE)FindClientEnumValue(strBuff, TEXT("ESCENE_DAYTIME_TYPE"));

		UINT AttributeID = appAtoi(tabFile->GetData( TEXT("AttributeID"), i));
		RandomAttributesMap.insert(std::make_pair(AttributeID, attr));

    }
	delete tabFile;
    return TRUE;
}

const EnhanceData* MMOFacade::GetEnhanceConsumption( UINT curEnhanceLv )
{
	return MMOEquipManager::getSingleton().GetEnhanceConsumption( curEnhanceLv );
}

//
const EnhanceResult* MMOFacade::GetEnhanceResult( const FString & equipSubType, const FString &equipSubSubType, UINT enhanceLv )
{
	return MMOEquipManager::getSingleton().GetEnhanceResult( equipSubType, equipSubSubType, enhanceLv );
}

//
UINT32 MMOFacade::GetEnhanceResultKey( const FString & equipSubType, const FString &equipSubSubType )
{
	return MMOEquipManager::getSingleton().GetEnhanceResultKey( equipSubType, equipSubSubType );
}

//=========================================================================================================
INT MMOFacade::GetEquipNpcPrice( UItem *pItem, const ItemInfo *pItemInfo )
{
	if( pItem == NULL ){
		return 0;
	}
	FLOAT npcSellPrice = pItem->SellPrice;
	if( pItemInfo == NULL ){
		return npcSellPrice;
	}
	INT defualtItemDur = pItem->DefaultDuration;
	UEquip *pEquip = Cast<UEquip>( pItem );
	if( pEquip != NULL ){
		defualtItemDur *= EQUIP_DUR_MULTIPLIER;
	}

	MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();
	switch( pItem->ItemDuration )
	{
	case DURTYPE_STACK:
		{
			npcSellPrice *= pItemInfo->m_nCurDur;
		}
		break;
	case DURTYPE_USE:
		{
			npcSellPrice = npcSellPrice *pItemInfo->m_nCurDur / defualtItemDur;
		}
		break;
	case DURTYPE_EQUIP:
		{
			//随机属性
			for( INT i = 0; i < MAX_RANDOM_ATTR_COUNT; ++i ){
				if( pItemInfo->m_wRandomAttrID[i] != 0 ){
					const ItemRandomAttr* propAttr = GetRandomAttributById( pItemInfo->m_wRandomAttrID[i] );
					if( propAttr == NULL ){
						continue;
					}

					npcSellPrice += propAttr->power * 100;
				}
			}

			//融灵属性
			for( INT i = 0; i < MAX_SOCKET_COUNT; ++i ){
				if( pItemInfo->m_nSoulID[i] != 0 ){
					const SoulInfo *pSoulInfo = mmoEquipMgr.FindSoulInfoByItemTemplateID( pItemInfo->m_nSoulID[i] );
					if( pSoulInfo == NULL ){
						continue;
					}

					const ItemRandomAttr* propAttr = GetRandomAttributById( pSoulInfo->_AttrID );
					if( propAttr == NULL ){
						continue;
					}

					npcSellPrice += propAttr->power * 100;
				}
				else if( pItemInfo->GetSocketType(i) != SOCKET_NONE ){
					npcSellPrice += EMPTY_SOCKET_SELL_PRICE;
				}
			}

			//铸魂属性
			for( INT i = 0; i < MAX_RUNE_COUNT; ++i ){
				if( pItemInfo->m_wRune[i] != 0 ){
					npcSellPrice += RUNE_SELL_PRICE;
				}
			}

			//幸运、诅咒
			npcSellPrice += pItemInfo->m_byLuck;

			//升级属性
			npcSellPrice += pItemInfo->m_byLevelupPhyPoint * 100;
			npcSellPrice += pItemInfo->m_byLevelupMagPoint * 100;
			npcSellPrice += pItemInfo->m_byLevelupTaoPoint * 100;

			//耐久度
			//npcSellPrice = npcSellPrice * 9 *  pItemInfo->m_nCurDur / 10 / defualtItemDur + npcSellPrice / 10;
			
			FLOAT dur = (FLOAT)pItemInfo->m_nCurDur / (FLOAT)defualtItemDur;
			npcSellPrice = (npcSellPrice * 0.9) * dur + npcSellPrice * 0.1;
		}
		break;
	}

	if( npcSellPrice <= 0 ){
		npcSellPrice = 1;
	}

	return npcSellPrice;
}
//=============================================================================================================================

FVector2D MMOFacade::GetScreenSize()
{
	FVector2D sz(1280.0f, 720.0f);	

	if(GEngine->GetAViewport())
	{
		sz.X = GEngine->GetAViewport()->GetSizeX();
		sz.Y = GEngine->GetAViewport()->GetSizeY();
	}

	return sz;
}

void MMOFacade::StartSortTimer(LockSortMode sortMode)
{
	_SortTimer = 0.0f;
	_SortTimerCounter = 10;
	_UpdateSortTimer = true;
	_LockSortMode = sortMode;
	MMOGameEvent_SortTimerBegin evt;
	MMOUI::PostGameEvent(&evt);
}
//------------------------------------------------------------------------
void MMOFacade::AddDamgeEffectPair(BYTE ResultType,BYTE WeaponHitType,BYTE DirectionType,BYTE ElementaryType, struct FDamageEffectPair* InData)
{
	UINT Key = ResultType<<24;
	Key += WeaponHitType<<16;
	Key += DirectionType<<8;
	Key += ElementaryType;
	DamageTemplateData.insert(make_pair(Key, InData));
}

void MMOFacade::ClearDamageTemplateData()
{
	DamageTemplateData.clear();
}

FDamageEffectPair* MMOFacade::GetDamgeTemplateData(BYTE ResultType,BYTE WeaponHitType,BYTE DirectionType,BYTE ElementaryType)
{
	UINT Key = ResultType<<24;
	Key += WeaponHitType<<16;
	Key += DirectionType<<8;
	Key += ElementaryType;
	Int2DamageEffectPairMap::iterator pos = DamageTemplateData.find(Key);
	if (pos == DamageTemplateData.end())
	{
		return NULL;
	}
	else
	{
		return pos->second;
	}
}

UBOOL MMO::IsForAllPlayerClass(FSpell_ClassType classType)
{
	return classType.bClassType1 == 1 
		&& classType.bClassType2 == 1
		&& classType.bClassType3 == 1
		&& classType.bClassType4 == 1
		&& classType.bClassType6 == 1; // 道士
}


UBOOL MMO::CanBeUsedByPlayerClass(EPlayerClass playerClass, FSpell_ClassType classType)
{
	if(MMO::IsForAllPlayerClass(classType))
		return TRUE;

	switch(playerClass)
	{
	case EPCT_PlayerClassA:
		return classType.bClassType1;

	case EPCT_PlayerClassB:
		return classType.bClassType2;

	case EPCT_PlayerClassC:
		return classType.bClassType3;

	case EPCT_PlayerClassD:
		return classType.bClassType4;

	case EPCT_PlayerClassE:
		return classType.bClassType6;

	case EPCT_PlayerClassG:
		return classType.bClassType7;

	case EPCT_PlayerClassH:
		return classType.bClassType8;

	case EPCT_PlayerClassF:
		return classType.bClassType5;
	};

	return FALSE;
}

// Spell masks (spell.txt Class column): bit = class id (0..4 = Warrior..Taoist), bit5 = monster/npc,
// bit6 = Lancer, bit7 = Sorcerer. Differs from the item convention above only for Taoist/npc.
UBOOL MMO::CanSpellBeUsedByPlayerClass(EPlayerClass playerClass, FSpell_ClassType classType)
{
	switch(playerClass)
	{
	case EPCT_PlayerClassA: return classType.bClassType1;
	case EPCT_PlayerClassB: return classType.bClassType2;
	case EPCT_PlayerClassC: return classType.bClassType3;
	case EPCT_PlayerClassD: return classType.bClassType4;
	case EPCT_PlayerClassE: return classType.bClassType5;
	case EPCT_PlayerClassG: return classType.bClassType7;
	case EPCT_PlayerClassH: return classType.bClassType8;
	case EPCT_PlayerClassF: return classType.bClassType6;
	}
	return FALSE;
}

UBOOL MMO::CanBeUsedByPlayerGender(EGenderType playerGender, BYTE genderReq)
{
	return (genderReq == EGenderType::EGT_None) || (genderReq == playerGender);
}

UBOOL MMO::IsClassTypeMatch( const FSpell_ClassType& newReqTyp, const FSpell_ClassType& useReqTyp )
{
	return( useReqTyp.bClassType1 && !newReqTyp.bClassType1 )
		|| ( useReqTyp.bClassType2 && !newReqTyp.bClassType2 )
		||( useReqTyp.bClassType3 && !newReqTyp.bClassType3 )
		|| ( useReqTyp.bClassType4 && !newReqTyp.bClassType4 )
		|| ( useReqTyp.bClassType5 && !newReqTyp.bClassType5 )
		;
}


FVector MMO::GetGfxOffsetVector(FVector Pos,float CavWidth,float CavHeight)
{
	if(CavWidth>0.0f&&CavHeight>0.0f)
	{
		return FVector( Pos.X*1280.0f/CavWidth, Pos.Y*720.0f/CavHeight,0.0f);
	}
	else
	{
		return Pos;
	}
}

//
void MMOFacade::SetFBSummary( const G2C_DUNGEON_FINISH_DATA &data )
{
	//
	_FBSummary.m_uiClearanceTimePt = data.byTimeScore;
	_FBSummary.m_uiClearanceTime = data.nTotalTime;
	//
	_FBSummary.m_uiKillSkillPt = data.byAttackScore;
	_FBSummary.m_uiKilled = data.nKillNpcCount;
	_FBSummary.m_uiDestroyed = data.nDestroyDoodadCount;
	_FBSummary.m_uiStrategy = 0;
	//
	_FBSummary.m_uiAccomplishPt = data.byGoalScore;
	//
	_FBSummary.m_uiGradedPt = data.byTotalScore;
	_FBSummary.m_uiGradedExp = data.nNormalExp;
	//
	_FBSummary.m_uiVIPExp = data.nVipExp;
	_FBSummary.m_uiGuildExp = data.nGuildExp;
	_FBSummary.m_uiActivityExp = data.nActivityExp;
	_FBSummary.m_uiCouplesExp = data.nTeamExp;
	_FBSummary.m_uiFirstTimeExp = data.nFirstPassExp;
	_FBSummary.m_uiSumUpExp = data.nTotalExp;
}

//
void MMOFacade::ResetFBSummary()
{ 
	memset( &_FBSummary, 0, sizeof(_FBSummary) );
}

//
FBMemberAwardData* MMOFacade::SetFBOtherScore(const G2C_DUNGEON_FINISH_SCORE &data)
{
	FBMemberAwardData* pFbAward = NULL;
	//
	UINT uCnt = data.byCount;
	for( UINT i = 0; i < uCnt; ++i ){
		pFbAward = MMOFacade::getSingleton().GetFBAwardByRoleID( data.stScore[i].dwRoleID );
		if( pFbAward == NULL ){
			continue;
		}
		pFbAward->m_uiFBGradedPt = data.stScore[i].dwTotalScore;
	}
	return pFbAward;
}

FBMemberAwardData* MMOFacade::SetFBAward( const G2C_DUNGEON_REWARD &data )
{
	FBMemberAwardData* pFbAward = GetFBAwardByRoleID( data.dwRoleID );
	if( pFbAward == NULL ){
		return NULL;
	}
	//
	if( pFbAward->m_uiFBAwardCount >= MAX_FB_AWARDS_CNT ){
		warnf( NAME_Error, TEXT("RoleID[%d] SetFBAward To the MAX count"), data.dwRoleID );
		return NULL;
	}

	//
	UINT uIdx = pFbAward->m_uiFBAwardCount;
	if( data.byRewardType <= 100 ){//这是钱
		pFbAward->m_Award[uIdx].uiItemCnt = data.dwRewardCount;
		
	}
	else{//这是装备
		pFbAward->m_Award[uIdx].uiItemCnt = data.dwRewardCount;
	}
	pFbAward->m_Award[uIdx].isValid = TRUE;
	pFbAward->m_Award[uIdx].uiItemID = data.dwRewardID;
	//
	SET_WARN_COLOR(COLOR_CYAN);
	warnf( NAME_Log, TEXT("RoleID[%d] get FBAward(%d) ItemID[%d] Count[%d]"), data.dwRoleID, pFbAward->m_uiFBAwardCount, pFbAward->m_Award[uIdx].uiItemID, pFbAward->m_Award[uIdx].uiItemCnt );
	//
	pFbAward->m_uiFBAwardCount++;

	return pFbAward;
}

//
void MMOFacade::ResetFBAward()
{
	INT iSize = sizeof(_FBMemberAward)/sizeof(FBMemberAwardData);
	for( INT i = 0; i < iSize; ++i){
		_FBMemberAward[i].m_uiMemberID = 0;
		_FBMemberAward[i].m_uiFBGradedPt = 0;
		_FBMemberAward[i].m_uiFBAwardCount = 0;
		for( UINT j = 0; j < MAX_FB_AWARDS_CNT; ++j){
			_FBMemberAward[i].m_Award[j].uiItemID = 0;
			_FBMemberAward[i].m_Award[j].uiItemCnt = 0;
			_FBMemberAward[i].m_Award[j].isValid = FALSE;
		}
	}
	//先把第一项占了
	AMMOPlayer* pl = GetMainPlayer();
	_FBMemberAward[0].m_uiMemberID = pl->ServerID;
}

//第一项为自己，即iIdx为0时
FBMemberAwardData* MMOFacade::GetFBAward( INT iIdx )
{
	INT iSize = sizeof(_FBMemberAward)/sizeof(FBMemberAwardData);
	if( iIdx < 0 || iIdx >= iSize ){
		return NULL;
	}
	return &_FBMemberAward[iIdx];
}

FBMemberAwardData* MMOFacade::GetFBAwardByRoleID( UINT uiIdx )
{
	INT iSize = sizeof(_FBMemberAward)/sizeof(FBMemberAwardData);
	for( INT i = 0; i < iSize; ++i){
		if( _FBMemberAward[i].m_uiMemberID == uiIdx ){
			return &_FBMemberAward[i];
		}
	}
	//if not found, add it(first one is myself, it can't be add by this)
	for( INT i = 1; i < iSize; ++i){
		if( _FBMemberAward[i].m_uiMemberID == 0 ){
			_FBMemberAward[i].m_uiMemberID = uiIdx;
			return &_FBMemberAward[i];
		}
	}
	//error
	return NULL;
}

//------------------------------------------------------------------------

void MMOFacade::SetContainerSize(UINT8 inContainerSize[CONSLOT_COUNT])
{
    //将本地的Container也进行一次初始化 默认为0 
    appMemset( ContainerSize, 0, sizeof(UINT8) * E_CONSLOT_MAX );
    appMemcpy( ContainerSize, inContainerSize, sizeof(UINT8) * CONSLOT_COUNT );
}

void MMOFacade::SetContainerSize(ECONTAINER_SLOT ContainerID, UINT16 ConSize)
{
	if(ContainerID < E_CONSLOT_MAX)
	{
		ContainerSize[ContainerID] = ConSize;
	}
}

UINT16 MMOFacade::GetContainerSize(ECONTAINER_SLOT ContainerID) const
{
	if(ContainerID < E_CONSLOT_MAX)
	{
		return ContainerSize[ContainerID];
	}

	return 0;
}

ItemsMap& MMOFacade::GetContainer(ECONTAINER_SLOT containerID)
{
	check(containerID < E_CONSLOT_MAX);
	return _Containers[containerID];
}

const ItemsMap& MMOFacade::GetContainer(ECONTAINER_SLOT containerID) const
{
	check(containerID < E_CONSLOT_MAX);
	return _Containers[containerID];
}

UBOOL MMOFacade::IsBagFull()
{
	UINT emptySlotCount = 0;
	AMMOPlayer* mainPlayer = GetMainPlayer();
	UINT bagCap = mainPlayer->ItemManager->CurCapacity(E_CONSLOT_DEFAULT);

	for (INT i = bagCap - 1; i >= 0; --i)
	{
		ItemInfo* pItemInfo = FindItem(E_CONSLOT_DEFAULT, i);
		if(pItemInfo == NULL)
		{
			++emptySlotCount;
			return FALSE;
		}
	}

	return (emptySlotCount <= 0);
}

INT MMOFacade::GetBagEmptySlotCount()
{
	UINT count = 0;
	AMMOPlayer* mainPlayer = GetMainPlayer();
	UINT bagCap = mainPlayer->ItemManager->CurCapacity(E_CONSLOT_DEFAULT);

	for (UINT i = 0; i < bagCap; ++ i)
	{
		ItemInfo* itemInfo = FindItem(E_CONSLOT_DEFAULT, i);
		if (itemInfo)
		{
			UItem* item = FindItemTemplate(itemInfo->m_nTemplateID);
			if (item)
			{
				count ++;
			}
		}
	}

	return count >= bagCap ? 0 : (bagCap - count);
}

//=======================================================================
//时间
void MMOFacade::SetEnterSceneServerTime( UINT32 serverTime )
{ 
	EnterSceneServerUtcTime = serverTime;

	//
	SYSTEMTIME curTime;
	::GetLocalTime(&curTime);
	UINT32 curTimeSec = ConvertSystemTimeToTmTSecond( curTime );

	//
	__time32_t localServerTime = serverTime;
	_localtime32(&localServerTime);

	ClientToServerSecDiff = (INT32)(localServerTime - curTimeSec);
	
	SET_WARN_COLOR(COLOR_CYAN);
	warnf(TEXT("SetEnterSceneServerTime  curTimeSec:%d, serverTime-localServerTime:%d-%d, ClientToServerSecDiff:%d"), curTimeSec, serverTime, localServerTime, ClientToServerSecDiff );
	CLEAR_WARN_COLOR();
}

//
_SYSTEMTIME MMOFacade::ConvertUtcSecondToSystemTime( UINT32 tmtTime )
{
	__time32_t tempCvtTime = tmtTime;
	struct tm* tmTime = _localtime32(&tempCvtTime);

	_SYSTEMTIME outTime;
	outTime.wYear = 1900 + tmTime->tm_year;
	outTime.wMonth = tmTime->tm_mon + 1;
	outTime.wDayOfWeek = tmTime->tm_wday;
	outTime.wDay = tmTime->tm_mday;
	outTime.wHour = tmTime->tm_hour;
	outTime.wMinute = tmTime->tm_min;
	outTime.wSecond = tmTime->tm_sec;
	
	return outTime;
}

UINT32 MMOFacade::ConvertSystemTimeToTmTSecond( _SYSTEMTIME systemTime )
{
	struct tm tempTmTime;
	tempTmTime.tm_year = systemTime.wYear - 1900;
	tempTmTime.tm_mon = systemTime.wMonth - 1;
	tempTmTime.tm_wday = systemTime.wDayOfWeek;
	tempTmTime.tm_mday = systemTime.wDay;
	tempTmTime.tm_hour = systemTime.wHour;
	tempTmTime.tm_min = systemTime.wMinute;
	tempTmTime.tm_sec = systemTime.wSecond;

	__time32_t tempCvtTime = _mktime32( &tempTmTime );
	return (UINT32)tempCvtTime;
}
//
UINT32 MMOFacade::GetRealClientUTCTime()
{
	//当前本地时间
	SYSTEMTIME curTime;
	::GetLocalTime(&curTime);
	UINT32 curTimeSec = ConvertSystemTimeToTmTSecond( curTime );

	//本地与服务器的时间差
	INT32 secDiff = GetClientToSceneSecondDiff();
	UINT32 newTime = ( ( secDiff > 0 ) ? ( curTimeSec + (UINT32)secDiff ) : ( (UINT32)( curTimeSec - (UINT32)( -secDiff) ) ) );

	return newTime;
}

SYSTEMTIME MMOFacade::GetRealClientTime()
{
	UINT32 newTime = GetRealClientUTCTime();
	//
	SYSTEMTIME realCurTime = ConvertUtcSecondToSystemTime( newTime );
	return realCurTime;
}
/*
正常游戏态的帧率 
*/
void MMOFacade::AddNewGameStateFPSData(INT nFps)
{
	InGameCalTimes++;
	if(InGameCalTimes <= 0)InGameCalTimes = 1;
	InGameStateFps += (nFps - InGameStateFps)/((FLOAT)InGameCalTimes);
}
void MMOFacade::ResetGameSateFps()
{
	//只重置一半次数 避免次数过大
	InGameCalTimes &= 0x000000FF;
	InGameCalTimes = (InGameCalTimes>>1);
}
FLOAT MMOFacade::GetGameSateFps()
{
	return InGameStateFps;
}
//=======================================================================
//------------------------------------------------------------------------
// MMOUI
//------------------------------------------------------------------------
void MMOUI::RegisterUIPanel(class UMMOUIBase* UI, FName FrindlyName)
{
	if(UI)
	{
		UI->FriendlyName = FrindlyName;
	}

	AllUIMaps.Set(FrindlyName, UI);
}
//------------------------------------------------------------------------
void MMOUI::RegisterMoviePlayer(class UMMOGFXPlayer* UI, FName FrindlyName)
{
	AllMoviePlayers.Set(FrindlyName, UI);

	if(UI)
	{
		UI->FriendlyName = FrindlyName;
	}
}
//------------------------------------------------------------------------
class UMMOUIBase* MMOUI::FindUIPanel(FName FrindlyName)
{
	UMMOUIBase** ptr = AllUIMaps.Find(FrindlyName);
	if(ptr)
	{
		return *ptr;
	}

	return 0;
}
//------------------------------------------------------------------------
class UMMOUIBase* MMOUI::FindUIPanelEx( FName panelName )
{
	UMMOUIBase* panel = FindUIPanel( panelName );
	if( panel == NULL )
	{
		FName PlayerName = MMOUI::FindMoviePlayerName( panelName );
 
 		if(!g_rootGFXPlayer->LoadViewByName(panelName))
 		{
 			warnf(NAME_Warning, TEXT("MMOUI::FindUIPanelEx 没有从新的加载方式中找到UI:%s"), *panelName.ToString());
		//UMMOGFXPlayer* Player = MMOUI::FindMoviePlayer( PlayerName );

		AMMOHud* HUD = MMOFacade::getSingleton().GetMyHud();
 			if( HUD )
 			{
			FName PlayerClassName = FName( *FString::Printf( TEXT("%s%s"), TEXT("MMOGame.MMOGFXPlayer_"), *( PlayerName.GetNameString() ) ) );
			MMOUI::LoadMoviePlayers( HUD, PlayerClassName );

			panel = FindUIPanel( panelName );
		}
	}
 		else
 		{
 			panel = FindUIPanel( panelName );
 		}
	}

	return panel;
}
//------------------------------------------------------------------------
UBOOL MMOUI::GetPanelByString( FString inName, UMMOUIBase **InPtr )
{
	*InPtr = MMOUI::FindUIPanel ( FName( *inName ) );
	if( *InPtr )
		return TRUE;
	else
		return FALSE;
}
//------------------------------------------------------------------------
class UMMOGFXPlayer* MMOUI::FindMoviePlayer(FName PanelName)
{
	UMMOGFXPlayer** ptr = AllMoviePlayers.Find(PanelName);
	if(ptr)
	{
		return *ptr;
	}

	return 0;
}


void MMOUI::RemoveMoviePlayer( class UMMOGFXPlayer* player )
{
	for( MoviePlayerMaps::TIterator PairIt(AllMoviePlayers); PairIt; ++PairIt)
	{
		if( PairIt.Value() == player )
		{
			AllMoviePlayers.Remove(PairIt.Key());
			return;
		}
	}
}

void MMOUI::SetPanelToMoviePlayer(FName PanelName, FName MoverPlayerName)
{
	if(PanelToMoviePlayers.HasKey(PanelName))
	{
		warnf(NAME_Error, TEXT("MMOUI::SetPanelToMoviePlayer 有键值冲突，应该是换新UI加载后的问题，请修正代码: %s -> %s"),
			*MMOUI::FindMoviePlayerName(PanelName).ToString(), *MoverPlayerName.ToString());
	}

	PanelToMoviePlayers.Set(PanelName, MoverPlayerName);
}

FName MMOUI::FindMoviePlayerName(FName PanelName)
{
	FName* ptr = PanelToMoviePlayers.Find(PanelName);
	if(ptr)
	{
		return *ptr;
	}
	return FName(TEXT(""));
}


//-------------------------------------------------------------------------
void MMOUI::LoadMoviePlayers(class AMMOHud* HUD,FName PlayerClassName)
{
	if(HUD)
	{
		FString PlayerClassString = PlayerClassName.GetNameString();
		UClass* MovePlayerClass = (UClass*)UObject::StaticFindObject(UClass::StaticClass(),
			ANY_PACKAGE, *PlayerClassString, TRUE);
		if(MovePlayerClass)
		{
			M_SCOPE_MEM_PROF(__MEMPROF__, FString::Printf(TEXT("MMOUI::LoadAllMoviePlayers-%s"), *PlayerClassString));
			debugf(NAME_DevGFxUI, TEXT("[MMO::LoadMoviePlayers] CreateMovie player class:%s Begin"), *PlayerClassString);
			MALLOC_PROFILER_GAME(GEngine->ExecMemFunctions(TEXT("MEM"));)

			UMMOGFXPlayer* MoviePlayer = HUD->eventCreateMoviePlayer(MovePlayerClass);
			if(MoviePlayer)
			{
				if(!MoviePlayer->MovieInfo)
				{
					MoviePlayer->MovieInfo = LoadObject<USwfMovie> (
						NULL, *MoviePlayer->MovieInfoStr, NULL, LOAD_None, NULL );
				}
				
					MoviePlayer->eventInit(GEngine->GamePlayers(0));
				//MoviePlayer->eventShowAllUI(debugUI);
				//MoviePlayer->Init();

				if(!MoviePlayer->RegisterEndCalled)
				{
					MoviePlayer->Init();
					MoviePlayer->PostInit();
					warnf(NAME_Warning, TEXT("MMOUI::LoadMoviePlayers 兼容加载旧的C++ MoviePlayer，但采用新的FLASH UPK资源：%s"), *MovePlayerClass->GetName());
				}
			}


			MALLOC_PROFILER_GAME(GEngine->ExecMemFunctions(TEXT("MEM"));)
				debugf(NAME_DevGFxUI, TEXT("[MMO::LoadMoviePlayers] CreateMovie player class:%s End"), *PlayerClassString);
		}
		else
		{
			warnf(NAME_Warning, TEXT("[MMO::LoadAllMoviePlayers] Movie player class not found:%s"), *PlayerClassString);
		}

	}
}

void DeleteMoviePlayer(AMMOHud* hud, UMMOGFXPlayer* player)
{
	for (INT i = 0; i < hud->AllMoviePlayers.Num(); ++i)
	{
		if(hud->AllMoviePlayers(i) == player)
		{
			hud->AllMoviePlayers.Remove(i);
			break;
		}
	}

	MMOUI::RemoveMoviePlayer(player);
}

void MMOUI::CloseMoviePlayersByName(FName PlayerName)
{
	FString PlayerString = PlayerName.GetNameString();
	UMMOGFXPlayer* pMMOGfxPlayer = MMOUI::FindMoviePlayer(*PlayerString);
	CloseMoviePlayers(pMMOGfxPlayer);
}

void MMOUI::CloseMoviePlayers(class UMMOGFXPlayer* Player)
{
	AMMOHud* HUD = MMOFacade::getSingleton().GetMyHud();
	if(HUD)
	{
		UMMOGFXPlayer* pMMOGfxPlayer = Player;
		if(pMMOGfxPlayer){
			//pMMOGfxPlayer->UnInit(); // 统一走OnClose调用 
			pMMOGfxPlayer->Close();
			DeleteMoviePlayer(HUD, pMMOGfxPlayer);
			//HUD->eventDeleteMoviePlayer(pMMOGfxPlayer);
		}
	}
}

//------------------------------------------------------------------------

extern TArray<UMMOGFXPlayer_RootBase*> g_rootGFXPlayerList;

void MMOUI::LoadAllMoviePlayers(class AMMOHud* HUD, class ULocalPlayer* LocPlay)
{
	UBOOL debugUI = FALSE;
	GConfig->GetBool(TEXT("Debug"), TEXT("DebugLoadUI"), debugUI, GUIIni);
		
	TArray<FString> CustomLoadList;
	
	if(GIsEditor || GIsMMOBenchmarking)
	{
		GConfig->GetArray(TEXT("MMOMoviePlayers_Benchmark"), TEXT("MoviePlayer"), CustomLoadList, GUIIni);
	}
	else if(debugUI)
	{
		GConfig->GetArray(TEXT("Debug"), TEXT("MoviePlayer"), CustomLoadList, GUIIni);
	}
	else
	{
		GConfig->GetArray(TEXT("MMOMoviePlayers"), TEXT("MoviePlayer"), CustomLoadList, GUIIni);
	}

	for (INT i = 0; i < CustomLoadList.Num(); ++i)
	{
		UClass* MovePlayerClass = (UClass*)UObject::StaticFindObject(UClass::StaticClass(),
			ANY_PACKAGE, *CustomLoadList(i), TRUE);
		if(MovePlayerClass)
		{
			M_SCOPE_MEM_PROF(__MEMPROF__, FString::Printf(TEXT("MMOUI::LoadAllMoviePlayers-%s"), *CustomLoadList(i)));
			debugf(TEXT("[MMO::LoadAllMoviePlayers] CreateMovie player class:%s Begin"), *CustomLoadList(i));

			MALLOC_PROFILER_GAME(GEngine->ExecMemFunctions(TEXT("MEM"));)

			UMMOGFXPlayer* MoviePlayer = HUD->eventCreateMoviePlayer(MovePlayerClass);
			if(MoviePlayer)
			{
				if(!MoviePlayer->MovieInfo)
				{
					MoviePlayer->MovieInfo = LoadObject<USwfMovie> (
						NULL, *MoviePlayer->MovieInfoStr, NULL, LOAD_None, NULL );
				}

				MoviePlayer->bAutoPlay = TRUE;
				MoviePlayer->eventInit(GEngine->GamePlayers(0));

				UMMOGFXPlayer_RootBase* rootGFXPlayer = Cast<UMMOGFXPlayer_RootBase>(MoviePlayer);
				if(rootGFXPlayer)
				{
					// 新的MoviePlayer不走RegisterEnd回调进行初始化
					g_rootGFXPlayerList.AddItem(rootGFXPlayer);
					MoviePlayer->Init();
					MoviePlayer->PostInit();

					rootGFXPlayer->PreLoadPanels();
				}
				else if(!MoviePlayer->RegisterEndCalled)
				{
					MoviePlayer->Init();
					MoviePlayer->PostInit();
					warnf(NAME_Warning, TEXT("兼容加载旧的C++ MoviePlayer，但采用新的FLASH UPK资源：%s"), *MovePlayerClass->GetName());
				}

				//Debug时，直接显示 非Debug状态下关闭
				MoviePlayer->ShowAllUI(/*FALSE*/debugUI );
				//MoviePlayer->Init();

			}

			MALLOC_PROFILER_GAME(GEngine->ExecMemFunctions(TEXT("MEM"));)
			debugf(TEXT("[MMO::LoadAllMoviePlayers] CreateMovie player class:%s End"), *CustomLoadList(i));
			//Cast<UMMOGFXPlayer>(UObject::StaticConstructObject( MovePlayerClass ));
			//MoviePlayer->eventInit(LocPlay);
		}
		else
		{
			warnf(NAME_Warning, TEXT("[MMO::LoadAllMoviePlayers] Movie player class not found:%s"), *CustomLoadList(i));
		}
	}	
}
//------------------------------------------------------------------------
void MMOUI::ShowAllPanels(UBOOL show, UBOOL bRecover)
{

	for(UIMaps::TIterator PairIt(AllUIMaps); PairIt; )
	{
		FName name = PairIt.Key();
		UMMOUIBase* ui = PairIt.Value();
		if(ui){
			
			if(show){
				//if(ui->LastVisibleRecover){
					MMOUI::GetUIManager()->ShowPanelEx(name,ui->LastVisible,"");
				//}
			}
			else
			{
				UBOOL bVisible = ui->IsVisible;
				MMOUI::GetUIManager()->ShowPanelEx(name,show,"");
				ui->LastVisible = bVisible;
			}
		}

		++PairIt;
	}

	if(!show)
	{
		WebHtmlManager::getSingleton().CloseWebPage();
	}


	//if(bRecover){
	//	if(show==FALSE){
	//		for(UIMaps::TIterator PairIt(AllUIMaps); PairIt; ++PairIt)
	//		{
	//			UMMOUIBase* ui = PairIt.Value();
	//			if(ui){
	//				ui->LastVisible = ui->IsVisible && ui->IsDynamicLoad;
	//				ui->SetVisible(show);
	//			}
	//		}
	//	}
	//	else
	//	{
	//		for(UIMaps::TIterator PairIt(AllUIMaps); PairIt; ++PairIt)
	//		{
	//			UMMOUIBase* ui = PairIt.Value();
	//			if(ui){
	//				ui->SetVisible(ui->LastVisible);
	//			}
	//		}
	//	}
	//}
	//else{
	//	for(UIMaps::TIterator PairIt(AllUIMaps); PairIt; )
	//	{
	//		if(PairIt.Value()){
	//			FName name = PairIt.Key();
	//			++PairIt;
	//			MMOUI::GetUIManager()->ShowPanelEx(name,show, "");
	//		}
	//		else
	//		{
	//			++PairIt;
	//		}
	//	}
	//}
}
//------------------------------------------------------------------------
void MMOUI::GetEquippedSlotByEquipType(INT EquipType, INT& tConSlot, INT& tItemSlot)
{
	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();

	switch(EquipType)
	{
	case EET_Normal_MainHand:
		tItemSlot = E_NES_MAIN_HAND;
		break;
	//case EET_Normal_OffHand:
	//	tItemSlot = E_NES_OFF_HAND;
	//	break;
	case EET_Normal_Head:
		tItemSlot = E_NES_HEAD;
		break;
	case EET_Normal_Shoulder:
		tItemSlot = E_NES_SHOULDER;
		break;
	case EET_Normal_Body:
		tItemSlot = E_NES_BODY;
		break;
	//case EET_Normal_Leg:
	//	tItemSlot = E_NES_LEG;
	//	break;
	case EET_Normal_Arm:
		tItemSlot = E_NES_ARM;
		break;
	case EET_Normal_Waist:
		tItemSlot = E_NES_WAIST;
		break;
	case EET_Normal_Foot:
		tItemSlot = E_NES_FOOT;
		break;
	case EET_Normal_Back:
		tItemSlot = E_NES_BACK;
		break;
	//case EET_Normal_Avatar:
	//	tItemSlot = E_NES_AVATAR;
	//	break;
	case EET_Normal_Neck:
		tItemSlot = E_NES_NECK;
		break;
	case EET_Normal_Finger:
		if( pl && pl->ItemManager )
		{
			if( pl->ItemManager->EquippedItemList_Normal( E_NES_FINGER_L )!= NULL &&  
				pl->ItemManager->EquippedItemList_Normal( E_NES_FINGER_R )== NULL)
			{
				tItemSlot = E_NES_FINGER_R;
			}
			else if(pl->ItemManager->EquippedItemList_Normal( E_NES_FINGER_L )== NULL &&  
				pl->ItemManager->EquippedItemList_Normal( E_NES_FINGER_R )!= NULL)
			{
				tItemSlot = E_NES_FINGER_L;
			}
			else
			{
				tItemSlot = E_NES_FINGER_L;

				if (MMOFacade::getSingleton().GetFingerPosShowType())
				{
					if (MMOFacade::getSingleton().GetFingerPos())
					{
						tItemSlot = E_NES_FINGER_R;
						MMOFacade::getSingleton().SetFingerPos(FALSE);
					}
					else
					{
						tItemSlot = E_NES_FINGER_L;
						MMOFacade::getSingleton().SetFingerPos(TRUE);
					}
				}
			}
		}
		else
		{
			tItemSlot = E_NES_FINGER_L;
		}
		break;
	case EET_Normal_Wrist:
		if( pl && pl->ItemManager )
		{
			if( pl->ItemManager->EquippedItemList_Normal( E_NES_WRIST_L )!= NULL &&  
				pl->ItemManager->EquippedItemList_Normal( E_NES_WRIST_R )== NULL)
			{
				tItemSlot = E_NES_WRIST_R;
			}
			else if(pl->ItemManager->EquippedItemList_Normal( E_NES_WRIST_L )== NULL &&  
				pl->ItemManager->EquippedItemList_Normal( E_NES_WRIST_R )!= NULL)
			{
				tItemSlot = E_NES_WRIST_L;
			}
			else
			{
				tItemSlot = E_NES_WRIST_L;

				if (MMOFacade::getSingleton().GetFingerPosShowType())
				{
					if (MMOFacade::getSingleton().GetWristPos())
					{
						tItemSlot = E_NES_WRIST_R;
						MMOFacade::getSingleton().SetWristPos(FALSE);
					}
					else
					{
						tItemSlot = E_NES_WRIST_L;
						MMOFacade::getSingleton().SetWristPos(TRUE);
					}
				}
				
			}
		}
		else
		{
			tItemSlot = E_NES_WRIST_L;
		}
		break;
	/*case EET_Avatar_Head:
		tItemSlot = EAES_Head;
		break;
	case EET_Avatar_Shoulder:
		tItemSlot = EAES_Shoulder;
		break;
	case EET_Avatar_Body:
		tItemSlot = EAES_Body;
		break;
	case EET_Avatar_Leg:
		tItemSlot = EAES_Leg;
		break;
	case EET_Avatar_Arm:
		tItemSlot = EAES_Arm;
		break;
	case EET_Avatar_Foot:
		tItemSlot = EAES_Foot;
		break;
	case EET_Avatar_Hair:
		tItemSlot = EAES_Hair;
		break;
	case EET_Avatar_Back:
		tItemSlot = EAES_Back;
		break;*/
	default:
		warnf(NAME_Error, TEXT("Wrong equip type: %d"), EquipType);
		break;
	}

	//tConSlot = E_CONSLOT_EQUIPPED;

	//if( EquipType <= EET_Normal_Count )
	
	// 这里为什么要这样去取Slot？
		//if( MMOUI::GetTopmostSlotContainer())
		//{
			//tConSlot = MMOUI::GetTopmostSlotContainer()->GetSlotType();
		//}
		//else{
			tConSlot = E_CONSLOT_EQUIPPED;
		//}
}
//------------------------------------------------------------------------
class UMMOUIManager* MMOUI::GetUIManager()
{
	return Cast<UMMOUIManager>(UMMOUIManager::StaticClass()->GetDefaultObject());
}

void MMOUI::CloseAllPanels( UBOOL _IsCleanClose = FALSE )
{
	for( MoviePlayerMaps::TIterator PairIt(AllMoviePlayers); PairIt; ++PairIt)
	{
		if( PairIt.Value() )
		{
			PairIt.Value()->ShowAllUI( false );
			PairIt.Value()->Close( _IsCleanClose );
		}
	}
}

void MMOUI::StartAllPanels( )
{
	for( MoviePlayerMaps::TIterator PairIt(AllMoviePlayers); PairIt; ++PairIt)
	{
		if( PairIt.Value() )
		{
			//PairIt.Value()->Start( );
			//PairIt.Value()->Advance(0);
			//PairIt.Value()->SetAllChildren2DefaultVisible( );
		}
	}
}
//------------------------------------------------------------------------
// TODO:
class PatternUILayoutPolicy
{
public:
	PatternUILayoutPolicy();
	virtual void Layout()
	{
	}
};

void MMOUI::ShowPatternPanel(class UMMOUIBase* RefPanel, class UMMOUIBase* PatternPanel)
{
	RefPanel->SetVisible(TRUE);
	PatternPanel->SetVisible(TRUE);
}

bool g_lastShow = true;

void MMOUI::ShowInGameUI(bool show)
{
	if(g_lastShow == show)
	{
		return;
	}

	g_lastShow = show;

	ShowAllPanels(show,TRUE);
	if(show)
	{
		MMODropManager::getSingleton().SetShowDropHud(show);
		MMOUI::GetUIManager()->ShowHintSceneWidget();//delegateShowHintSceneWidget();
		MMOUI::GetUIManager()->ShowHurtPanel(MMOUI::GetUIManager()->DrawHurt);
		if(  MMOFacade::getSingleton().GetMainPlayer() )
		{
			if(  MMOFacade::getSingleton().GetMainPlayer()->eventIsDead() )
			{
				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Death, TRUE, "");
			}
		}
	}



//
//
//	typedef std::map<UMMOUIBase*, UBOOL, M_UE3_STL_ALLOC_MAP(UMMOUIBase*, UBOOL) > GameUIVisiblitityMap;
//	static GameUIVisiblitityMap lastUIVisMap;
//	if(!show)
//	{
//		lastUIVisMap.clear();
//	}
//
//	MMODropManager::getSingleton().SetShowDropHud(show);
//
//	debugf(TEXT("************** MMOUI::ShowInGameUI:%d"), (INT)show);
//
//#if 1
//	MMOUI::GetUIManager()->ShowAllPanels(FALSE);
//	if(show)
//	{
//		MMOUI::GetUIManager()->ShowPanelEx(TEXT("Buff"), TRUE, "");
//		MMOUI::GetUIManager()->ShowPanelEx(TEXT("Minimap"), TRUE, "");
//		MMOUI::GetUIManager()->ShowPanelEx(TEXT("Chat"), TRUE, "");
//		MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_HudInfo_MyInfo , TRUE, "");
//		MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_HudInfo_TeamHint , TRUE, "");
//		MMOUI::GetUIManager()->ShowPanelEx(TEXT("MsgInfo"), TRUE, "");
//		MMOUI::GetUIManager()->ShowPanelEx(TEXT("SceneHint"), TRUE, "");
//		MMOUI::GetUIManager()->ShowPanelEx(TEXT("FriendChatLevel"), TRUE, "");
//		MMOUI::GetUIManager()->ShowPanelEx(TEXT("MenuBar"), TRUE, "");
//		MMOUI::GetUIManager()->ShowPanelEx(TEXT("TeamMate"), TRUE, "");
//		MMOUI::GetUIManager()->ShowPanelEx(TEXT("DamageLevel"), TRUE, "");
//		//MMOUI::GetUIManager()->ShowPanel(TEXT("BattleState"), TRUE, "");	//暂时注释掉，陈凯要求的
//		MMOUI::GetUIManager()->ShowPanelEx(TEXT("HintButtonBar"), TRUE, "");	
//		MMOUI::GetUIManager()->ShowPanelEx(TEXT("MsgButtonBar"), TRUE, "");	
//		MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_Friend_FriendChatHint , TRUE, "");
//		MMOUI::GetUIManager()->delegateShowHintSceneWidget();
//
//		//显示伤害判断ui
//		MMOUI::GetUIManager()->ShowHurtPanel(MMOUI::GetUIManager()->DrawHurt);
//
//		//skillbar最后show，因为最后show的会设置focusplayer，响应键盘
//		MMOUI::GetUIManager()->ShowPanelEx(TEXT("Skillbar"), TRUE, "");
//		MMOUI::GetUIManager()->ShowPanelEx(TEXT("DropItemTips"), TRUE, "");
//		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_QuestTrace, TRUE, "");
//		if(  MMOFacade::getSingleton().GetMainPlayer() )
//		{
//			if(  MMOFacade::getSingleton().GetMainPlayer()->eventIsDead() )
//			{
//				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Death, TRUE, "");
//			}
//		}
//
//
//	}
//#else
//	for(UIMaps::TIterator PairIt(AllUIMaps); PairIt; ++PairIt)
//	{
//		UMMOUIBase* ui = PairIt.Value();
//// 		GameUIVisiblitityMap::iterator it = lastUIVisMap.find(ui);
//// 		if(it != lastUIVisMap.end())
//// 		{
//// 			ui->Show(it->second);
//// 		}
//// 		else
//// 		{
//// 			lastUIVisMap.insert(std::make_pair(ui, ui->IsVisible));
//// 			ui->Show(show ? TRUE : FALSE);
//// 		}
//
//		if(ui){
//			GameUIVisiblitityMap::iterator it = lastUIVisMap.find(ui);
//			if(it != lastUIVisMap.end())
//			{
//				if(ui->ShowInMatinee)
//				{
//					debugf(TEXT("	************** matineeUI %s %d"), *(*ui).StaticClass()->GetName(), (INT)!show);
//					// matinee ui
//					ui->Show(!show);
//				}
//				else
//				{
//					debugf(TEXT("	************** restore  %s %d->%d"), *(*ui).StaticClass()->GetName(), ui->IsVisible, (INT)it->second);
//					// in game ui
//					ui->Show(it->second);
//				}
//			}
//			else
//			{
//				lastUIVisMap.insert(std::make_pair(ui, ui->IsVisible));
//				if(ui->ShowInMatinee)
//				{
//					debugf(TEXT("	************** matineeUI %s %d"), *(*ui).StaticClass()->GetName(), (INT)!show);
//					ui->Show(!show);
//				}
//				else
//				{
//					debugf(TEXT("	************** %s %d->%d"), *(*ui).StaticClass()->GetName(), ui->IsVisible, (INT)show);
//				ui->Show(show);
//			}
//		}
//	}	
//}
//#endif
}

INT MMOUI::GetStringType( const FString& language )
{
	INT langType = -1;
	if( language == TEXT("CHN") )
	{
		langType = LT_CHN;
	}
	else if( language == TEXT("INT") )
	{
		langType = LT_INT;
	}
	return langType;
}
//------------------------------------------------------------------------
FString MMOUI::ParseLocalizedPropertyPath(const FString& PathName)
{
	TArray<FString> Pieces;
	//PathName.ParseIntoArray(&Pieces, TEXT("."), false);
	//FString* value = g_StringMap.Find(PathName);
	StringMapVal* value = g_StringMap.Find(PathName);
	if( !value )
	{
#if !SHIPPING_PC_GAME
		return PathName + TEXT("?");
#else
		return TEXT("");
#endif
	}
	return value->stringVal;
}

FString MMOUI::ParseLocalizedPropertyPath(const FString& PathName, UBOOL& Res)
{
	TArray<FString> Pieces;
	//FString* value = g_StringMap.Find(PathName);
	StringMapVal* value = g_StringMap.Find(PathName);
	if( !value )
	{
		Res = FALSE;
#if !SHIPPING_PC_GAME
		return PathName + TEXT("?");
#else
		return TEXT("");
#endif
	}
	Res = TRUE;
	return value->stringVal;
}

FString MMOUI::FindSoundNameByLocalString(const FString& PathName)
{
	StringMapVal* value = g_StringMap.Find(PathName);
	if( !value )
	{
		return TEXT("");
	}
	return value->soundName;
}

INT MMOUI::FindMsgTypeByLocalString(const FString& PathName)
{
	StringMapVal* value = g_StringMap.Find(PathName);
	if( !value )
	{
		return MsgInfoType_Normal;
	}
	return value->msgInfoType;
}
//焦点系列处理 add by JimZhang@snda.com-------------------------------------------------------------------------------------------------------
void MMOUI::OnFocusGained(class UMMOUIBase* Panel)
{
	//PanelQueue::iterator i = std::find(g_PanelQueue.begin(), g_PanelQueue.end(), Panel);
	//if(i != g_PanelQueue.end())
	//{
	//	g_PanelQueue.erase(i);
	//	g_PanelQueueMap.Set(Panel->FriendlyName, NULL);
	//}

	//if(Panel && Panel->GFXPlayer && Panel->GFXPlayer->bAllowFocus && Panel->CanMoveTop )
	//{
	//	// 只设置能接收焦点的MOVIE
	//	g_PanelQueue.push_back(Panel);
	//	g_PanelQueueMap.Set(Panel->FriendlyName, Panel);
	//}
}

void MMOUI::OnFocusLost(class UMMOUIBase* Panel)
{
	//不用做什么，顺序符合自然顺序
}

//void MMOUI::OnFocusClose(class UMMOUIBase* Panel)
//{
//	if(Panel==NULL)return;
//
//	PanelList::reverse_iterator iter = g_PanelList.rbegin();
//	for (; iter != g_PanelList.rend(); ++iter)
//	{
//		if(Panel->FriendlyName.GetNameString() == *iter)
//		{
//			g_PanelList.erase( (++iter).base() );
//			break;
//		}
//	}
//
//	SetOptimizationFocus(Panel);
//}

void MMOUI::SetOptimizationFocus(class UMMOUIBase* Panel)
{
	//if(!g_PanelQueue.empty())
	//{
	//	// 只设置能接收焦点的MOVIE
	//	INT num = g_PanelQueue.size();
	//	for(INT j=num-1;j>=0;j--)
	//	{
	//		if(g_PanelQueue[j]->GFXPlayer // fix for crash
	//			&&  g_PanelQueue[j]->GFXPlayer->bAllowFocus
	//			&& g_PanelQueue[j]->GFXPlayer->pMovie )
	//		{
	//			FGFxEngine::GetEngine()->SetFocusMovie( g_PanelQueue[j]->GFXPlayer->pMovie );
	//			return;
	//		}
	//	}
	//}

// 	if(!g_PanelList.empty())
// 	{
// 		PanelList panelList = g_PanelList;
// 		// 只设置能接收焦点的MOVIE
// 		INT num = g_PanelList.size();
// 		PanelList::reverse_iterator iter = panelList.rbegin(); // .find(num-1);
// 		for(;iter!=panelList.rend(); ++iter)
// 		{
// 			UMMOUIBase* Panel = MMOUI::GetUIManager()->FindUIPanel(FName(**iter));
// 			if(	Panel && 
// 				Panel->GFXPlayer &&
// 				Panel->GFXPlayer->bAllowFocus &&
// 				Panel->GFXPlayer->pMovie)
// 			{
// 				//FGFxEngine::GetEngine()->SetFocusMovie( Panel->GFXPlayer->pMovie );
// 				return;
// 			}
// 		}
// 	}

	//FGFxEngine::GetEngine()->SetFocusMovie();
}



void MMOUI::MoveToTop(class UMMOUIBase* Panel)
{
	//return; // oscar@snda test

	if(Panel == NULL)
	{
		return;
	}

	if(Panel->GFXPlayer && Panel->GFXPlayer->bAllowFocus && Panel->CanMoveTop && Panel->PanelAllowFocus)
	{
		// TODOUI: 检查第2个参数是否要传
		// 只设置能接收焦点的MOVIE
		//FGFxEngine::GetEngine()->SetFocusMovie(Panel->GFXPlayer->pMovie/*, Panel->FriendlyName*/);
	}

	PanelList::iterator iter = g_PanelList.begin();
	for (; iter != g_PanelList.end(); ++iter)
	{
		if(Panel->FriendlyName.GetNameString() == *iter)
		{
			//g_PanelList.erase(	(++iter).base()	);
			g_PanelList.erase(	iter );
			break;
		}
	}
	if(Panel && Panel->GFXPlayer && Panel->GFXPlayer->bAllowFocus && Panel->CanMoveTop && Panel->PanelAllowFocus)
	{
		// 只设置能接收焦点的MOVIE
		if(Panel->FriendlyName == NAME_None)
		{

		}
		else
		{
			g_PanelList.push_back(Panel->FriendlyName.GetNameString());
		}
	}

	//PanelList::reverse_iterator iter = g_PanelList.rbegin();
	//for (; iter != g_PanelList.rend(); ++iter)
	//{
	//	if(Panel->FriendlyName.GetNameString() == *iter)
	//	{
	//		g_PanelList.erase( (++iter).base() );
	//		break;
	//	}
	//}
	//else if( !Panel->GFXPlayer )
	//{
	//	FGFxEngine::GetEngine()->SetFocusMovie();
	//}
}

void MMOUI::RemoveTop(class UMMOUIBase* Panel)
{
	if(Panel==NULL)
	{
		return;
	}

	PanelList::iterator iter = g_PanelList.begin();
	for (; iter != g_PanelList.end(); ++iter)
	{
		if(Panel->FriendlyName.GetNameString() == *iter)
		{
			//g_PanelList.erase( (++iter).base() );
			g_PanelList.erase( iter );
			break;
		}
	}

	SetOptimizationFocus(Panel);


	//SetOptimizationFocus(Panel);

	//PanelQueue::iterator i = std::find(g_PanelQueue.begin(), g_PanelQueue.end(), Panel);
	//if(i != g_PanelQueue.end())
	//{
	//	g_PanelQueue.erase(i);
	//}

	//if(!g_PanelQueue.empty())
	//{
	//	// 只设置能接收焦点的MOVIE
	//	//FGFxEngine::GetEngine()->SetFocusMovie(
	//	//	g_PanelQueue.back()->GFXPlayer->bAllowFocus ? g_PanelQueue.back()->GFXPlayer->pMovie : NULL);

	//	INT num = g_PanelQueue.size();
	//	for(INT j=num-1;j>=0;j--)
	//	{
	//		if(g_PanelQueue[j]->GFXPlayer // fix for crash
	//			&&  g_PanelQueue[j]->GFXPlayer->bAllowFocus
	//			&& g_PanelQueue[j]->GFXPlayer->pMovie )
	//		{
	//			FGFxEngine::GetEngine()->SetFocusMovie( g_PanelQueue[j]->GFXPlayer->pMovie );
	//			return;
	//		}
	//	}
	//}
	//FGFxEngine::GetEngine()->SetFocusMovie();
}

class UMMOUIBase* MMOUI::GetTopmostPanel()
{
	//if(!g_PanelQueue.empty())
	//{
	//	return g_PanelQueue.back();
	//}
	if(!g_PanelList.empty())
	{
// 		PanelList::reverse_iterator iter = g_PanelList.rbegin();
// 		if(iter!=g_PanelList.rend())
// 		{
// 			UMMOUIBase* Panel = MMOUI::GetUIManager()->FindUIPanel(FName(**iter));
// 			return Panel;
// 		}

		UMMOUIBase* Panel = MMOUI::GetUIManager()->FindUIPanel(FName(*g_PanelList.back()));
	}
	return NULL;
}

class UMMOUIBaseSlotContainer* MMOUI::GetTopmostSlotContainer()
{
	if(!g_PanelList.empty())
	{
		PanelList panelList = g_PanelList;
		PanelList::reverse_iterator iter = panelList.rbegin();
		for(;iter!=panelList.rend(); ++iter )
		{
			UMMOUIBase* Panel = MMOUI::GetUIManager()->FindUIPanel(FName(**iter));
			if(	Panel )
			{
				UMMOUIBaseSlotContainer* pSlotContainer = Cast<UMMOUIBaseSlotContainer>(Panel);
				if(pSlotContainer)
				{
					return pSlotContainer;
				}
			}
		}
	}


	//INT num = g_PanelQueue.size();
	//for(INT j=num-1;j>=0;j--)
	//{
	//	UMMOUIBaseSlotContainer* pSlotContainer = Cast<UMMOUIBaseSlotContainer>(g_PanelQueue[j]);
	//	if(pSlotContainer)
	//	{
	//		return pSlotContainer;
	//	}
	//}
	return NULL;
}

//焦点系列处理 add by JimZhang@snda.com-------------------------------------------------------------------------------------------------------

class UMMOUIBase* MMOUI::GetFocusPanel()
{
	FGFxMovie* pFocusMovie = FGFxEngine::GetEngine()->GetFocusMovie(0);

	for(UIMaps::TIterator PairIt(AllUIMaps); PairIt; ++PairIt)
	{
		UMMOUIBase* pPanel = PairIt.Value();

		if(pPanel && pPanel->GFXPlayer&& pPanel->GFXPlayer->pMovie == pFocusMovie)
		{
			return pPanel;
		}
	}
	return 0;
}

//------------------------------------------------------------------------
// Game States
//------------------------------------------------------------------------
void MMO::ProShowLoading(UBOOL bShow)
{
	UMMOGameState* loadingSt = FindGameStateObject(EGameState_Loading);
	if(loadingSt)
	{
		if(bShow)
		{
			loadingSt->OnEnter();
		}
		else
		{
			loadingSt->OnLeave();
		}
	}
}


void MMO::PreActiveGameState(EGameState gs)
{
	UMMOGameState* newSt = FindGameStateObject(gs);
	g_PreStateObj = newSt;
	if(g_PreStateObj)
	{
		g_PreStateObj->OnPreEnter();
	}
}


void MMO::ActiveGameState(EGameState gs)
{
#ifdef _DEBUG
	static UEnum* est = FindObject<UEnum>(ANY_PACKAGE, TEXT("EGameState"), TRUE);
#endif

	g_PreStateObj = NULL;
	if(gs == g_CurrentState )
	{
#ifdef _DEBUG
		SET_WARN_COLOR(COLOR_CYAN);
		warnf(NAME_Log, TEXT("==================[ActiveGameState] same state:%s"),
			*est->GetEnum(g_CurrentState).ToString());
		CLEAR_WARN_COLOR();
#endif
		if(g_CurrentStateObj != NULL)
		{
			g_CurrentStateObj-> OnUpdate();
		}
		return;
	}

#ifdef _DEBUG
	SET_WARN_COLOR(COLOR_CYAN);
	warnf(NAME_Log, TEXT("==================[ActiveGameState] %s -> %s"),
		*est->GetEnum(g_CurrentState).ToString(),
		*est->GetEnum(gs).ToString());
	CLEAR_WARN_COLOR();
#endif

	
	UMMOGameState* newSt = FindGameStateObject(gs);

	if(g_CurrentStateObj != NULL)
	{
		g_CurrentStateObj->OnLeave();
	}

	g_CurrentState = gs;
	g_CurrentStateObj = newSt;

	if(g_CurrentStateObj != NULL)
	{
		g_CurrentStateObj->OnEnter();
	}
}

EGameState MMO::GetCurrentGameState()
{	
	return g_CurrentState;
}

void MMO::AddGameState(class UMMOGameState* st)
{
	g_gameStatesMap[(EGameState)st->GetID()] = st;
	st->AddToRoot(); // 不要让GC销毁这类对象
}

class UMMOGameState* MMO::FindGameStateObject(EGameState gs)
{
	GameStateMap::iterator i = g_gameStatesMap.find(gs);
	if(i != g_gameStatesMap.end())
	{
		return i->second;
	}

	return 0;
}
//------------------------------------------------------------------------
// Game events
//------------------------------------------------------------------------

typedef std::vector<INativeGameEventHandler*, M_UE3_STL_ALLOC(INativeGameEventHandler*) > NativeGameEventHandlerList;
typedef std::map<EGameEvent,  NativeGameEventHandlerList, M_UE3_STL_ALLOC_MAP(EGameEvent, NativeGameEventHandlerList) > NativeGameEventHandlerMap;
NativeGameEventHandlerMap g_nativeGameEventHandlerMap;

void MMOUI::RegisterEventHandler(class INativeGameEventHandler* handler, EGameEvent requiredEventType)
{
	check(handler);

	NativeGameEventHandlerMap::iterator i = g_nativeGameEventHandlerMap.find(requiredEventType);
	if(i != g_nativeGameEventHandlerMap.end())
	{
		i->second.push_back(handler);
	}
	else
	{
		NativeGameEventHandlerList lst;
		lst.push_back(handler);
		g_nativeGameEventHandlerMap.insert(std::make_pair(requiredEventType, lst));
	}
}


void MMOUI::UnRegisterEventHandler(class INativeGameEventHandler* handler, EGameEvent requiredEventType)
{
	check(handler);

	NativeGameEventHandlerMap::iterator i = g_nativeGameEventHandlerMap.find(requiredEventType);
	if(i != g_nativeGameEventHandlerMap.end())
	{
		 NativeGameEventHandlerList::iterator it = std::find(i->second.begin(), i->second.end(), handler);
		 if(it != i->second.end())
		 {
			 i->second.erase(it);
		 }
	}
}

void _PostGameEvent(struct MMOGameEvent* evt)
{
	NativeGameEventHandlerMap::iterator i2 = g_nativeGameEventHandlerMap.find(evt->GetEventType());
	if(i2 != g_nativeGameEventHandlerMap.end())
	{
		for ( size_t j = 0; j < i2->second.size(); ++j )
		{
			INativeGameEventHandler* hdl = i2->second[j];
			if (hdl)
			{
				hdl->OnNativeGameEvent(evt);
			}
		}
	}
}

void MMOUI::PostGameEvent(struct MMOGameEvent* evt)
{
	M_FORCE_TRY_BEGIN;
	_PostGameEvent(evt);
	M_FORCE_TRY_END("Protected MMOUI::PostGameEvent");
}

FString MMOUI::TrimNumber( const FString& srcStr )
{
	if(srcStr.Len() <= 0)
	{
		return TEXT("");
	}


	INT idx = 0;
	for (INT i = srcStr.Len() - 1; i > 0; --i)
	{
		if(!appIsDigit(srcStr[i]))
		{
			break;
		}

		++idx;
	}

	FString tmpStr = srcStr.Left(srcStr.Len() - idx);
	return tmpStr;
}

#if WITH_EDITOR

FString MMOUI::GetPropertyDisplayName( UProperty* objectProp )
{
	FString FinalDisplayName = objectProp->GetMetaData(TEXT("DisplayName"));
	if(FinalDisplayName.Len() > 0)
	{
		FString LocalizedName = LocalizeProperty( *(objectProp->GetFullGroupName(TRUE)), *FinalDisplayName );	
		if (LocalizedName.Len() > 0)
		{
			FinalDisplayName = LocalizedName;
		}
		else
		{
			FString LocalizedName = LocalizeProperty( *(objectProp->GetFullGroupName(TRUE)), *(objectProp->GetName()) );	
			if (LocalizedName.Len() > 0)
			{
				FinalDisplayName = LocalizedName;
			}
		}
	}

	return FinalDisplayName;
}

#endif

FString MMOUI::ConvertServerEnumToClientEnum( const FString& serverEnum, UEnum* clientEnumCls )
{
	FString clientEnum;
	INT upperStart = -1;
	INT underLine = -1;
	INT lowerEnd = -1;

	for (INT i = 0; i < serverEnum.Len(); ++i)
	{
		if(appIsLower(serverEnum[i]))
		{
			lowerEnd = i;
		}
		else if(appIsUpper(serverEnum[i]))
		{
			if(upperStart == -1)
			{
				upperStart = i;
				break;
			}
		}
		else
		{
			warnf(NAME_Error, TEXT("Unknown server enum naming format:%s"), *serverEnum);
		}
	}

	underLine = serverEnum.InStr(TEXT("_"));

	if(upperStart != -1)
	{
		if(underLine != -1)
		{
			clientEnum = FString(TEXT("E_")) + serverEnum;
		}
		else if(lowerEnd != -1)
		{
			FString enumPrefix = serverEnum.Mid(0, lowerEnd + 1);
			FString enumVal = serverEnum.Right(serverEnum.Len() - lowerEnd - 1);
			clientEnum = FString(TEXT("E_")) + enumPrefix + TEXT("_") + enumVal;
		}
		else
		{
			warnf(NAME_Error, TEXT("Unknown server enum naming format, without '_' :%s"), *serverEnum);
		}
	}
	else if(lowerEnd != -1)
	{
		FString enumPrefix = serverEnum.Mid(0, lowerEnd + 1);
		FString enumVal = serverEnum.Right(serverEnum.Len() - lowerEnd - 1);
		clientEnum = FString(TEXT("E_")) + enumPrefix + TEXT("_") + enumVal;
	}
	else
	{
		warnf(NAME_Error, TEXT("Unknown server enum naming format, nor lower or upper begin:%s"), *serverEnum);
	}

	return clientEnum;
}

//直接导出地址上连续的账户角色信息到一个指定DataTypeName类型的二进制文件
void MMOFacade::ExportSimpleAccountData( void *pData , size_t lDataLength , FString DataTypeName , UBOOL bingUsename )
{	
	if(  !pData || lDataLength <= 0 )
		return;
	//CreateFArchive
	FArchive* pArchive = NULL;
	if( BeginExportAccountData( &pArchive , DataTypeName , bingUsename ) )
	{
		pArchive->ByteOrderSerialize( pData, lDataLength );
	}
	EndExportAccountData( pArchive );
}

FString MMOFacade::GetUserAccountStoreDir()
{
	return appGameLogDir();//TEXT("..\\..\\MMOGame\\Account\\");
}

//开始导出账户角色信息到一个指定DataTypeName类型的二进制文件
UBOOL MMOFacade::BeginExportAccountData( FArchive** pArchive , FString DataTypeName , UBOOL bingUsename )
{
	AMMOPlayerController* ctrl = GetMainController();
	AMMOPlayer* player = GetMainPlayer();
	if( NULL == ctrl || NULL == player )
		return FALSE;

	// GEditorIni在非编辑器环境有可能不会被初始化, 因此这里暂时将Path写死
	FString pathName = GetUserAccountStoreDir();
	FString fileName;
	if(bingUsename)
		fileName = pathName + player->UserId + TEXT("\\") + player->ActorName + TEXT("_") + DataTypeName + TEXT(".cfg");
	else
		fileName = pathName + player->UserId + TEXT("\\") + DataTypeName + TEXT(".cfg");

	// 转换成绝对路径，add by zhangrui05  
	FString filePath = GFileManager->ConvertAbsolutePathToUserPath(*GFileManager->ConvertToAbsolutePath(*fileName));

	//FCheckoutResult sccResult = FSourceControl::TryCheckOut(fileName);
	*pArchive = GFileManager->CreateFileWriter(*filePath);
	if( *pArchive )
	{
		return TRUE;
	}
	else
	{
		warnf( NAME_Error, TEXT("ExportAccountData, failed to create file:%s"), *filePath);
		return FALSE;
	}
}

//需要与BeginExpo rtAccountData成对使用
void MMOFacade::EndExportAccountData( FArchive* pArchive )
{
	if( pArchive && !pArchive->IsError() )
	{
		pArchive->Flush();
		(FArchiveFileWriterWindows *)pArchive->Close();
	}
	else
	{
		warnf( TEXT("Failed to EndExportAccountData "));
	}

	delete pArchive;
}

//读取账户角色信息中DataTypeName的二进制数据
UBOOL MMOFacade::BeginLoadAccountBinary( FArchive** pArchive,FString DataTypeName , UBOOL bingUsename )
{
	AMMOPlayerController* ctrl = GetMainController();
	AMMOPlayer* player = GetMainPlayer();
	if( NULL == ctrl || NULL == player )
		return FALSE;

	FArchive* tArchive = NULL;
	//开辟内存区块
	//FBufferArchive    memAr;

	// GEditorIni在非编辑器环境有可能不会被初始化, 因此这里暂时将Path写死
	FString pathName = GetUserAccountStoreDir();//TEXT("..\\..\\MMOGame\\Account\\");
	FString fileName;
	if(bingUsename)
		fileName = pathName + player->UserId + TEXT("\\") + player->ActorName + TEXT("_") + DataTypeName + TEXT(".cfg");
	else
		fileName = pathName + player->UserId + TEXT("\\") + DataTypeName + TEXT(".cfg");

	// 转换成绝对路径，add by zhangrui05  
	FString filePath = GFileManager->ConvertAbsolutePathToUserPath(*GFileManager->ConvertToAbsolutePath(*fileName));

	// 读取
	tArchive = GFileManager->CreateFileReader(*filePath);
	if( tArchive )
	{
		//先申请足够的空间，防止后面ReAllocate地址会发生改变
		if( tArchive->IsError() )
		{
			return FALSE;
		}
		if( tArchive->TotalSize() <= 0 )
		{
			return FALSE;
		}
		*pArchive = tArchive;
		return TRUE;
	}
	else
	{
		warnf( TEXT("LoadAccountData dose not existed file:%s"), *filePath);
		return FALSE;
	}
}

//需要与BeginLoadAccountBinary成对使用
void MMOFacade::EndLoadAccountBinary( FArchive* pArchive )
{
	if( pArchive && !pArchive->IsError() )
	{
		pArchive->Flush();
		( (FArchiveFileReaderWindows *)pArchive )->Close();
	}
	else
	{
		warnf( TEXT("Failed to EndLoadAccountBinary "));
	}

	delete pArchive;
}

void MMOFacade::ClearUISound()
{
	_UISoundMap.clear();
}

void MMOFacade::AddUISound(USoundCue* sound)
{
	if (sound)
	{
		_UISoundMap.insert(std::make_pair(sound->GetPureName().GetNameString(), sound));
	}
}

USoundCue* MMOFacade::GetUISound(const FString& name) const
{
	UISoundMap::const_iterator iter = _UISoundMap.find(name);
	if (iter != _UISoundMap.end())
	{
		return iter->second;
	}
	return NULL;
}

void MMOFacade::PlayUISound(const FString& cueName)
{
	AMMOActor* mainActor = GetMainActor();
	if (mainActor)
	{
		USoundCue* sound = GetUISound(cueName);
		if (sound)
		{
			mainActor->PlaySound(sound);
		}
	}
}

void MMO::ActiveKismetPlayBik(const FString& bikName, const bool bPlayBGM )
{
	if(GFullScreenMovie){
		GFullScreenMovie->GameThreadPlayMovie(MM_PlayOnceFromStream, *bikName);
		GFullScreenMovie->PlayBGM( bPlayBGM );
		GFullScreenMovie->GameThreadWaitForMovie();
		GFullScreenMovie->GameThreadStopMovie();	
	}
}

void MMO::ActiveKismetRemoteEvent(const FName& eventName)
{
	// look for all remote events in the entire sequence tree	
	USequence* rootSeq = GWorld->GetGameSequence();
	if(!rootSeq)
	{
		warnf(NAME_Warning, TEXT("[MMO::ActiveKismetRemoteEvent] root sequence is null!"));
		return;
	}

	SET_WARN_COLOR(COLOR_CYAN);
	warnf(NAME_Log, TEXT("------------ ActiveKismetRemoteEvent:%s"), *eventName.ToString());
	CLEAR_WARN_COLOR();

	//rootSeq->GetRootSequence(TRUE);

	TArray<USeqEvent_RemoteEvent*> remoteEvents;

	rootSeq->FindSeqObjectsByClass(USeqEvent_RemoteEvent::StaticClass(),
		(TArray<USequenceObject*>&)remoteEvents);

	for (INT Idx = 0; Idx < remoteEvents.Num(); Idx++)
	{
		USeqEvent_RemoteEvent* remoteEvent = remoteEvents(Idx);
		if (remoteEvent != NULL && remoteEvent->EventName == eventName )
		{
			if ( remoteEvent->bEnabled)
			{
				// check activation for the event
				remoteEvent->ForceActivateOutput(0);
				// CheckActivate(Originator,Instigator,FALSE,NULL);
			}
		}
	}
}

void MMO::ActiveKismetEvent_ServerVarChange( INT varType, INT varIndex, INT varReferID, INT varValue )
{
	// look for all remote events in the entire sequence tree	
	USequence* rootSeq = GWorld->GetGameSequence();
	if(!rootSeq)
	{
		warnf(NAME_Warning, TEXT("[MMO::ActiveKismetEvent_ServerVarChange] root sequence is null!"));
		return;
	}

	//rootSeq->GetRootSequence(TRUE);

	TArray<USeqEvent_Server2Client_VarChange*> varEvents;

	rootSeq->FindSeqObjectsByClass(USeqEvent_Server2Client_VarChange::StaticClass(),
		(TArray<USequenceObject*>&)varEvents);

	for (INT Idx = 0; Idx < varEvents.Num(); Idx++)
	{
		USeqEvent_Server2Client_VarChange* varEvent = varEvents(Idx);
		if (varEvent != NULL && varEvent->bEnabled)
		{
// 			varEvent->VarType = varType;
// 			varEvent->VarIndex = varIndex;
// 			varEvent->VarReferID = varReferID;
// 			varEvent->VarValue = varValue;
// 			// check activation for the event
// 			varEvent->PopulateLinkedVariableValues();

			varEvent->PublishLinkedVariableValues();
			if(varEvent->VarType == varType
				&& varEvent->VarIndex == varIndex
				&& varEvent->VarReferID == varReferID
				&& varEvent->VarValue == varValue)
			{
				varEvent->ForceActivateOutput(0);
			}
		}
	}
}


SHORT MMO::GetAsyncKeyStateEx(int vKey)
{
	if(GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport && GEngine->GameViewport->Viewport->HasFocus())
	{
		return GetAsyncKeyState(vKey);
	}
	else
	{
		return 0;
	}
}


bool g_needUpdateServerVars = false;
typedef TMap<INT, G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA> ServerVarContainer;
ServerVarContainer g_serverVars;

void MMO::SetServerSceneVars( const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA* vars, INT count )
{
	g_needUpdateServerVars = true;
	g_serverVars.Empty();

	INT mapID = MMOMapDataManager::getSingleton().GetCurMap().ServerID;

	for (INT i = 0; i < count; ++i)
	{
		G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA serverData = g_serverVars.FindOrAdd( vars[i].wVarIndex );
		serverData.wVarIndex = vars[i].wVarIndex;
		serverData.nValue = vars[i].nValue;
		CheckClientSceneVariableAction( mapID, vars[i] );
	}

	////服务器默认数据为0的不下发//算了，暂时保持现状，有问题再说
	//for (INT i = 0; i < MAX_SCENE_VARIABLE_COUNT; ++i)
	//{
	//	G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA *pServerData = g_serverVars.Find( i );
	//	if( pServerData != NULL ){
	//		continue;
	//	}

	//	G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA serverData = g_serverVars.FindOrAdd( i );
	//	serverData.wVarIndex = i;
	//	serverData.nValue = 0;
	//	CheckClientSceneVariableAction( mapID, serverData );
	//}

	// TODO: move to game init
	//MMO::UpdateAllServerSceneVars();
}

void MMO::UpdateAllServerSceneVars()
{
	if(!g_needUpdateServerVars)
	{
		return;
	}

	g_needUpdateServerVars = false;

	INT mapID = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	for (ServerVarContainer::TIterator iter(g_serverVars); iter; ++iter)
	{
		G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA& var = iter.Value();
		MMO::TActiveKismetEvent_ServerVarChange<USeqEvent_Server2Client_VarChange_Scene>(var.wVarIndex,	0, var.nValue, false);
		//
		CheckClientSceneVariableAction( mapID, var );
	}
}

bool MMO::IsRequiredVarOK(INT varID, INT desiredVarValue)
{
	return true;
}


INT MMOFacade::GetCurrentBagWgt()
{
	INT nWeight = 0;
	const ItemsMap& container = _Containers[E_CONSLOT_DEFAULT];

	if (container.size() <= 0)
		return nWeight;

	ItemsMap::const_iterator it = container.begin();
	for (; it != container.end(); ++it)
	{
		const FClientItemInfo* itemInfo = it->second;
		if(itemInfo&&itemInfo->m_nTemplateID>0)
		{
			UItem* itemTemp = FindItemTemplate(itemInfo->m_nTemplateID);
			if(itemTemp)
			{
				INT nStack = (itemTemp->ItemDuration==E_DURTYPE_STACK)?itemInfo->m_nCurDur:1;
				nWeight+= itemTemp->Weight*nStack;
			}
		}
	}
	return nWeight;
	//INT wgt = 0;

	//AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	//if (pl && pl->ItemManager)
	//{
	//	for(int i = 0; i < pl->ItemManager->BagItemList.Num(); i++)
	//	{
	//		UItem* item = pl->ItemManager->BagItemList(i);
	//		if (!item)
	//		{
	//			continue;
	//		}

	//		INT nStack = (item->ItemDuration==E_DURTYPE_STACK)?
	//		if(item->ItemDuration == E_DURTYPE_STACK)
	//		{
	//			return m_nCurDur;
	//		}

	//		item->sta
	//		wgt += item->Weight ;
	//	}
	//}
	//return wgt;
}
INT MMOFacade::GetCurrentWeaponWgt()
{
	INT wgt = 0;
	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	for (INT i = 0; i < pl->ItemManager->EquippedItemList_Normal.Num(); ++ i)
	{
		UItem* item = pl->ItemManager->EquippedItemList_Normal(i);
		if (!item)
		{
			continue;
		}
		if (item->ItemType == TEXT("ITEMTYPE_EQUIP") && item->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_WEAPON"))
		{
			wgt += item->Weight;
		}
	}
	return wgt;
}
INT MMOFacade::GetCurrentArmorWgt()
{
	INT wgt = 0;
	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	for (INT i = 0; i < pl->ItemManager->EquippedItemList_Normal.Num(); ++ i)
	{
		UItem* item = pl->ItemManager->EquippedItemList_Normal(i);
		if (!item)
		{
			continue;
		}
		if (item->ItemType == TEXT("ITEMTYPE_EQUIP") && item->ItemSubType != TEXT("ITEMSUBTYPE_EQUIP_WEAPON"))
		{
			wgt += item->Weight;
		}
	}
	return wgt;
}

BOOL MMOFacade::CheckWeight(INT itemWeight,INT lastItemWeight,WeightType type)
{
	BOOL res = TRUE;
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	FString strScreen;

	if( !player || !player->ActorAttributes )
	{
		return res;
	}

	switch(type)
	{
	case WT_Bag:
		{
			if (itemWeight + GetCurrentBagWgt() > player->ActorAttributes->GetAttribute(catBagWeightLmt))
			{
				res = FALSE;
				strScreen = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOWeightError.Bag"));
			}
		}
		break;
	case WT_Armor:
		{
			if (itemWeight + ( GetCurrentArmorWgt()- lastItemWeight) > player->ActorAttributes->GetAttribute(catArmorWeightLmt))
			{
				res = FALSE;
				strScreen = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOWeightError.Armor"));
			}
		}
		break;
	case WT_Weap:
		{
			if (itemWeight + ( GetCurrentWeaponWgt() - lastItemWeight) > player->ActorAttributes->GetAttribute(catWeaponWeightLmt))
			{
				res = FALSE;
				strScreen = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOWeightError.Weap"));
			}
		}
		break;
	}

	if (res == FALSE)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(strScreen,MsgInfoType_Warning,strScreen);
	}
	return res;
}

//======================================================================================================
void MMOFacade::GameVerAnnounceProcess()
{
	INT lastUpdateVer = 0;
	GConfig->GetInt( TEXT("GamePatch"), TEXT("LastUpdateVer"), lastUpdateVer, GGameIni );

	//INT majorVer = ( lastUpdateVer >> 24 );
	//INT minorVer = ( ( lastUpdateVer >> 16 ) & ( ( 1 << 8 ) - 1 ) );
	//INT buildVer = ( lastUpdateVer & ( ( 1 << 16 ) - 1 ) );

	//if( MAJOR_VERSION != majorVer || MINOR_VERSION != minorVer || BUILD_NUMBER != buildVer ){
	if( BUILD_NUMBER != lastUpdateVer ){
		//INT curUpdateVer = ( MAJOR_VERSION << 24 );
		//curUpdateVer |= ( ( MINOR_VERSION & ( ( 1 << 8 ) - 1 ) ) << 16 );
		//curUpdateVer |= ( BUILD_NUMBER & ( ( 1 << 16 ) - 1 ) );
		INT curUpdateVer = BUILD_NUMBER;
		GConfig->SetInt( TEXT("GamePatch"), TEXT("LastUpdateVer"), curUpdateVer, GGameIni );
		GConfig->Flush( FALSE, GGameIni );

		GConfig->GetInt( TEXT("GamePatch"), TEXT("LastUpdateVer"), lastUpdateVer, GGameIni );
		SET_WARN_COLOR(COLOR_PURPLE);
		warnf( NAME_Log, TEXT("=================update game last version: %d( %d, %d, %d )"), lastUpdateVer, MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER );
		CLEAR_WARN_COLOR();

		FString textIdx = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GameAnnouncement.TextIdx") );
		INT iTextIdx = appAtoi( *textIdx );
		if( iTextIdx > 0 ){
			MMOMsgButtonManager_Parms parms(EC_EventParm);
			parms.Parm1 = iTextIdx;
			parms.Parm2 = BUILD_NUMBER;
			MMOMsgButtonManager::getSingleton().AddNewButton( FScriptDelegate( MMOUI::GetUIManager(), TEXT("OpenGameAnnouncement") ), parms, E_MsgBtnShow_GameUpdateInfo );
		}
	}
}//======================================================================================================

FString MMOUI::FormatToGFXImage( UTexture2D* tex )
{
	if(tex)
	{
		return FString(TEXT("img://")) + tex->GetPathName();
	}

	return TEXT("img://");
}

static QSORT_RETURN CDECL CompareRole( const FRoleInfo* AA, const FRoleInfo* AB )
{
	if(AA&&AB)
	{
		if(AA->nSaveTime > AB->nSaveTime)return -1;
	}
	return 1;
}


void MMOFacade::AddOjbectToSync(INT InId)
{
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(InId);
	if(actor)
	{
		actor->bActorDataReset=FALSE;
	}
	ObjectsToSync.push_back(InId);
}

void MMOFacade::RemoveOjbectToSync(INT InId)
{
	ObjectsToSync.remove(InId);
}

void MMOFacade::RemoveOjbectToSync()
{
	ObjectsToSync.clear();
}

UBOOL MMOFacade::ForceSyncObject(INT InId, UBOOL bClearSyncList)
{
	if (bClearSyncList)
	{
		ObjectsToSync.remove(InId);
	}

	AMMOActor* ToLoadResObject = GetCharacter(InId);

	if (ToLoadResObject == NULL)
	{
		return TRUE;
	}

	if (ToLoadResObject->IsA(ANPC::StaticClass()))
	{
		G2C_SYNC_NPC_EX_DATA* npcData = CMessageHandler::GetInstance().GetNPCSyncData(ToLoadResObject->ServerID);
		if (npcData == NULL)
		{
			// 这里查不到NPC信息，说明m_NPCDataCache的数据已经被新数据循环写掉，需要再查一下
			CMessageHandler::GetInstance().SendQueryCharData(ToLoadResObject->ServerID, 0);
			return FALSE;
		}

		CMessageHandler::GetInstance().SyncNpcData(npcData);
	}
	else if (ToLoadResObject->IsA(AMMOPlayer::StaticClass()))
	{
		G2C_SYNC_ROLE_DATA* playerData = CMessageHandler::GetInstance().GetRoleSyncData(ToLoadResObject->ServerID);
		if (playerData == NULL)
		{
			// 这里查不到角色信息，说明m_RoleDataCache的数据已经被新数据循环写掉，需要再查一下
			CMessageHandler::GetInstance().SendQueryCharData(ToLoadResObject->ServerID, 0);
			return FALSE;
		}

		CMessageHandler::GetInstance().SyncRoleData(playerData);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

void MMOFacade::UpdateObjectsToSync(FLOAT Delta)
{
	EndObjectsOrder();
	return;





	//if (ObjectsToSync.size() == 0)
	//{
	//	return;
	//}


	////DWORD ct = GetTickCount();
	////if (ct - LastSyncTickCount < 100)
	////{
	////	return;
	////}
	////LastSyncTickCount = ct;

	//if(LastSyncTickCount++%3!=0)
	//{
	//	return;
	//}

	//INT ToSyncId = ObjectsToSync.front();
	//ObjectsToSync.pop_front();

	//ForceSyncObject(ToSyncId, FALSE);

	////AMMOActor* ToLoadResObject = GetCharacter(ObjectsToSync.front());

	////ObjectsToSync.pop_front();

	////if (ToLoadResObject == NULL)
	////{
	////	return;
	////}

	////if (ToLoadResObject->IsA(ANPC::StaticClass()))
	////{
	////	G2C_SYNC_NPC_DATA* npcData = CMessageHandler::GetInstance().GetNPCSyncData(ToLoadResObject->ServerID);
	////	if (npcData == NULL)
	////	{
	////		return;
	////	}

	////	CMessageHandler::GetInstance().SyncNpcData(npcData);
	////}
	////else if (ToLoadResObject->IsA(AMMOPlayer::StaticClass()))
	////{
	////	G2C_SYNC_ROLE_DATA* playerData = CMessageHandler::GetInstance().GetRoleSyncData(ToLoadResObject->ServerID);
	////	if (playerData == NULL)
	////	{
	////		return;
	////	}

	////	CMessageHandler::GetInstance().SyncRoleData(playerData);
	////}
}

void MMOFacade::StartObjectsOrder()
{
	AMMOPlayer* pMainPlayer = GetMainPlayer();
	if(pMainPlayer){
		MainPlayerLocation = GetMainPlayer()->Location;
	}
	for(INT i=0;i<NearbyNum;i++)
	{
		NearbyDis[i]=40000000.0;
		NearbyId[i]=0;
	}
}

void MMOFacade::OperateObjectsOrder(AMMOActor* pAMMOActor)
{
	if(pAMMOActor && pAMMOActor->bActorDataReset==FALSE && pAMMOActor->IsMainPlayer()==FALSE)
	{
		FLOAT Dis = (pAMMOActor->Location - MainPlayerLocation).Size2D();
		NearbyDis[NearbyNum-1] =  Dis;
		NearbyId[NearbyNum-1] =  pAMMOActor->ServerID;

		for(INT i=NearbyNum-1;i>0;i--)
		{
			if(NearbyDis[i]<NearbyDis[i-1])
			{
				FLOAT Buf1 = NearbyDis[i-1];
				INT   Buf2 = NearbyId[i-1];
				NearbyDis[i-1] = NearbyDis[i];
				NearbyId[i-1] = NearbyId[i];
				NearbyDis[i] = Buf1;
				NearbyId[i] = Buf2;
			}
		}
	}
}

void MMOFacade::EndObjectsOrder()
{
	INT SyncNum=10;

	for(int i=0;i<NearbyNum-1;i++)
	{
		if(NearbyId[i]==0)
		{
			break;
		}

		UBOOL bRes = ForceSyncObject(NearbyId[i], FALSE);
		if(bRes)
		{
			SyncNum--;
			if(SyncNum<=0)
			{
				break;
			}
		}

		NearbyDis[i]=40000000.0;
		NearbyId[i]=0;
	}
}

void MMOFacade::GetTimeString(UINT32 time, FString& timeString)
{
	UINT32 day = time / 86400;
	UINT32 hour = (time % 86400) / 3600;
	UINT32 min = (time % 86400 % 3600) / 60;
	UINT32 sec = time % 86400 % 3600 % 60;

	timeString = TEXT("");
	if (day > 0)
	{
		timeString = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.TimeString.Day")), day);
	} 
	else
	{
		if (hour > 0)
		{
			timeString = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.TimeString.Hour")), hour);
		}
		else
		{
			if (min > 0)
			{
				timeString = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.TimeString.Min")), min);
			} 
			else
			{
				timeString = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.TimeString.LessSec"));
			}
		}
	}
}

void MMOFacade::GetDurationTimeString(UINT32 startTime, FString& timeString)
{
	__time32_t lastTime = startTime;
	_localtime32(&lastTime);

	__time32_t localTime;
	_time32(&localTime);
	_localtime32(&localTime);

	INT time = localTime - lastTime;
	if (time < 0)
	{
		time = 0;
	}

	GetTimeString(time, timeString);
}

void MMOFacade::GetCountdownTimeString(UINT32 endTime, FString& timeString)
{
	__time32_t destTime = endTime;
	_localtime32(&destTime);

	__time32_t localTime;
	_time32(&localTime);
	_localtime32(&localTime);

	INT time = destTime - localTime;
	if (time < 0)
	{
		time = 0;
	}

	GetTimeString(time, timeString);
}

void MMOFacade::SendOpenPackage(INT itemId)
{
	INT itemCnt = GetItemCountInBag( itemId);
	UItem* pItem = FindItemTemplate(itemId);

	if (pItem  && itemCnt <= 1)
	{
		if (MMOFacade::getSingleton().IsItemPack(pItem->ItemPack))
		{
			MMOGameEvent_BagOpenPack evt(pItem->ItemPack);
			MMOUI::PostGameEvent(&evt);
		}
		else if (MMOFacade::getSingleton().IsItemPack(pItem->ItemPack2))
		{
			MMOGameEvent_BagOpenPack evt(pItem->ItemPack2);
			MMOUI::PostGameEvent(&evt);
		}
	}
}

BOOL MMOFacade::GetPlayerSort()
{
	BOOL res = FALSE;
	if (playerSortType != IST_Invalid)
		res = TRUE;
	return res;
}

FString FClientItemInfo::ToReadableString()
{
	INT i = 0;
	FString ret;
	ret.Empty();

	//#define ADDELESTRING(n) ret += (appItoa(n) + TEXT(","));
#define ADDELESTRING(n) ret += (NumberToString(n,14) + TEXT(","));
	ADDELESTRING(m_nTemplateID);
	ADDELESTRING(m_byConSlot);
	ADDELESTRING(m_byItemSlot);
	ADDELESTRING(m_nCurDur);
	ADDELESTRING(m_nMaxDur);
	ADDELESTRING(m_byBindType);
	ADDELESTRING(Pow);

	UItem* item = MMOFacade::getSingleton().FindItemTemplate(m_nTemplateID);

	if(item && item->ItemType == TEXT("ITEMTYPE_EQUIP"))
	{
		//以下属性仅限装备
		ADDELESTRING(m_byIdentified);
		for(i = 0; i < MAX_RANDOM_ATTR_COUNT; ++i)
		{
			ADDELESTRING(m_wRandomAttrID[i]);
		}
		//ADDELESTRING(m_wRefineLevel);
		ADDELESTRING(m_dwRoleID);

		//以下属性仅限武器
		if (item->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_WEAPON"))
		{
			ADDELESTRING(m_byLuck);
			for(i = 0; i < MAX_RUNE_COUNT_NEW; ++i)
			{
				ADDELESTRING(m_wRune[i]);
			}

			ADDELESTRING(m_byLevelupPhyPoint);
			ADDELESTRING(m_byLevelupMagPoint);
			ADDELESTRING(m_byLevelupTaoPoint);
		}
		else
		{
			//以下属性仅限防具
			ADDELESTRING(m_dwSocketCombination);
			for(i = 0; i < MAX_SOCKET_COUNT; ++i)
			{
				ADDELESTRING(m_nSoulID[i]);
			}
		}

	}


#undef ADDELESTRING
	//去掉最后一个逗号
	ret = ret.Mid(0,ret.Len() - 1);
	return ret;
}

void FClientItemInfo::FromGFXStringArray(UGFxObject* params, INT startIndex)
{
	INT i = 0;
	//#define PARSEELESTRING() (appAtoi(params->GetElementString(startIndex++)));
#define PARSEELESTRING() (StringToNumber(params->GetElementString(startIndex++),14));
	m_nTemplateID = (INT32)PARSEELESTRING();
	m_byConSlot = (UINT8)PARSEELESTRING();
	m_byItemSlot = (UINT8)PARSEELESTRING();
	m_nCurDur = (INT32)PARSEELESTRING();
	m_nMaxDur = (INT32)PARSEELESTRING();
	m_byBindType = (UINT8)PARSEELESTRING();
	Pow = (INT)PARSEELESTRING();

	UItem* item = MMOFacade::getSingleton().FindItemTemplate(m_nTemplateID);
	if(item && item->ItemType == TEXT("ITEMTYPE_EQUIP"))
	{
		m_byIdentified = (UINT8)PARSEELESTRING();
		for(i = 0; i < MAX_RANDOM_ATTR_COUNT; ++i)
		{
			m_wRandomAttrID[i] = (UINT16)PARSEELESTRING();
		}
		m_dwRoleID = (UINT32)PARSEELESTRING();

		//以下属性仅限武器
		if (item->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_WEAPON"))
		{
			m_byLuck = (INT8)PARSEELESTRING();
			for(i = 0; i < MAX_RUNE_COUNT_NEW; ++i)
			{
				m_wRune[i] = (UINT16)PARSEELESTRING();
			}
			m_byLevelupPhyPoint = (UINT8)PARSEELESTRING();
			m_byLevelupMagPoint = (UINT8)PARSEELESTRING();
			m_byLevelupTaoPoint = (UINT8)PARSEELESTRING();
		}
		else
		{
			m_dwSocketCombination = (UINT32)PARSEELESTRING();
			for(i = 0; i < MAX_SOCKET_COUNT; ++i)
			{
				m_nSoulID[i] = (INT32)PARSEELESTRING();
			}
		}		
	}


#undef PARSEELESTRING
}

INT MMOFacade::GetMoneyRatio()
{
	return MoneyRatio;
}

FString GetErrorCodeEnumString(INT idx)
{
	if(idx < 0 || idx >= (INT)errMiscEnd)
	{
		appMsgf(AMT_OK, TEXT("发现无法识别的错误号：%d"), idx);
		return TEXT("");
	}

	FString enumVal(g_szErrorCodeNameList[idx]);
	return enumVal;
}

FString GetPlayerRoleName()
{
	if(MMOFacade::getSingleton().GetMainPlayer())
	{
		return MMOFacade::getSingleton().GetMainPlayer()->ActorName;
	}

	return TEXT("");
}

INT	GetPlayerRoleID()
{
	if(MMOFacade::getSingleton().GetMainPlayer())
	{
		return MMOFacade::getSingleton().GetMainPlayer()->ServerID;
	}

	return 0;
}