/*================================================================================
	SplashScreen.cpp: Splash screen for game/editor startup
	Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
================================================================================*/

#include "EnginePrivate.h"

#include "SplashScreen.h"

//#include "MinWindows.h"
//#include <Windows.h>
//#include <GdiPlus.h> // oscar@snda
//#pragma comment(lib, "gdiplus.lib") // oscar@snda

#include "..\..\Common\define\version.h" // oscar@snda

#if !CONSOLE && !PLATFORM_MACOSX

// From LaunchEngineLoop.cpp
extern INT GGameIcon;
extern INT GEditorIcon;

SIZE GBmpSize = {0, 0}; // oscar@snda

/**
 * Splash screen functions and static globals
 */

static HANDLE GSplashScreenThread = NULL;
//static HBITMAP GSplashScreenBitmap = NULL;
static HWND GSplashScreenWnd = NULL; 
static TCHAR* GSplashScreenFileName;
static FString GSplashScreenText[ SplashTextType::NumTextTypes ];
static RECT GSplashScreenTextRects[ SplashTextType::NumTextTypes ];
static HFONT GSplashScreenSmallTextFontHandle = NULL;
static HFONT GSplashScreenNormalTextFontHandle = NULL;
static FCriticalSection GSplashScreenSynchronizationObject;

//static HRGN GSplashRegn = 0; // oscar@snda

static Gdiplus::Image* g_image = 0; // oscar@snda
static BLENDFUNCTION g_blend; // oscar@snda
static ULONG_PTR gdiplusToken; // oscar@snda


/**
 * Window's proc for splash screen
 */
LRESULT CALLBACK SplashScreenWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
    PAINTSTRUCT ps;

	switch( message )
	{
		// oscar@snda
	case WM_ERASEBKGND:
		return TRUE;

		// oscar@snda
		case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);

				// Draw splash bitmap				
				//if(GIsEditor) // oscar@snda
					//DrawState(hdc, DSS_NORMAL, NULL, (LPARAM)GSplashScreenBitmap, 0, 0, 0, 0, 0, DST_BITMAP);

				{
					// Take a critical section since another thread may be trying to set the splash text
					FScopeLock ScopeLock( &GSplashScreenSynchronizationObject );

					// Draw splash text
					for( INT CurTypeIndex = 0; CurTypeIndex < SplashTextType::NumTextTypes; ++CurTypeIndex )
					{
						const FString& SplashText = GSplashScreenText[ CurTypeIndex ];
						const RECT& TextRect = GSplashScreenTextRects[ CurTypeIndex ];

						if( SplashText.Len() > 0 )
						{
							if( CurTypeIndex == SplashTextType::StartupProgress ||
								CurTypeIndex == SplashTextType::VersionInfo1 )
							{
								SelectObject( hdc, GSplashScreenNormalTextFontHandle );
							}
							else
							{
								SelectObject( hdc, GSplashScreenSmallTextFontHandle );
							}
							SetBkColor( hdc, 0x00000000 );
							SetBkMode( hdc, TRANSPARENT );

							RECT ClientRect;
							GetClientRect( hWnd, &ClientRect );

							// Draw background text passes
							const INT NumBGPasses = 8;
							for( INT CurBGPass = 0; CurBGPass < NumBGPasses; ++CurBGPass )
							{
								INT BGXOffset, BGYOffset;
								switch( CurBGPass )
								{
									default:
									case 0:	BGXOffset = -1; BGYOffset =  0; break;
									case 2:	BGXOffset = -1; BGYOffset = -1; break;
									case 3:	BGXOffset =  0; BGYOffset = -1; break;
									case 4:	BGXOffset =  1; BGYOffset = -1; break;
									case 5:	BGXOffset =  1; BGYOffset =  0; break;
									case 6:	BGXOffset =  1; BGYOffset =  1; break;
									case 7:	BGXOffset =  0; BGYOffset =  1; break;
									case 8:	BGXOffset = -1; BGYOffset =  1; break;
								}

								SetTextColor( hdc, 0x00000000 );
								TextOut(
									hdc,
									TextRect.left + BGXOffset,
									TextRect.top + BGYOffset,
									*SplashText,
									SplashText.Len() );
							}

							// Draw foreground text pass
							if( CurTypeIndex == SplashTextType::StartupProgress )
							{
								SetTextColor( hdc, RGB( 180, 180, 180 ) );
							}
							else if( CurTypeIndex == SplashTextType::VersionInfo1 )
							{
								SetTextColor( hdc, RGB( 240, 240, 240 ) );
							}
							else
							{
								SetTextColor( hdc, RGB( 160, 160, 160 ) );
							}
							TextOut(
								hdc,
								TextRect.left,
								TextRect.top,
								*SplashText,
								SplashText.Len() );
						}
					}
				}

				EndPaint(hWnd, &ps);
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

// oscar@snda
#if 0
HRGN CU_SetupRegion(HDC pDC
	, HBITMAP cBitmap, COLORREF TransColor
	, UINT uTilesX = 1, UINT uOffsetX = 0
	, UINT uTilesY = 1, UINT uOffsetY = 0
	)
{
	HDC memDC = ::CreateCompatibleDC(pDC);

	HBITMAP pOldMemBmp = (HBITMAP)::SelectObject(memDC, cBitmap);
	HRGN rgn = ::CreateRectRgn(0,0,0,0);

	BITMAP bit;   
	GetObject(cBitmap, sizeof(bit), &bit);

	UINT uWidth		= bit.bmWidth / uTilesX;
	UINT uHeight	= bit.bmHeight / uTilesY;

	int iOffsetX = uOffsetX * uWidth;
	int iOffsetY = uOffsetY * uHeight;
	for(UINT y = 0; y < uHeight; y++)
	{
		HRGN rgnTemp = 0;

		int iX = 0;
		UINT iPos = iOffsetX;
		do
		{
			//Skip transparent color to find the next non-transparent point.
			while(iPos < iOffsetX + uWidth
				&& ::GetPixel(memDC, iPos, iOffsetY + y) == TransColor
				)
			{
				iX++;
				iPos++;
			}

			//Remember this start point
			int iLeftX = iX;

			//Look for the next transparent point
			while (iPos < iOffsetX + uWidth  && ::GetPixel(memDC, iPos, iOffsetY + y) != TransColor)
			{
				++iX;
				++iPos;
			}

			//Create a temporary "region" containing the 1-pixel-high strip between the start and end points
			rgnTemp = ::CreateRectRgn(iLeftX, y, iX, y+1);

			//Merge into the main "region".
			rgn = ::CombineRgn(rgn, rgnTemp, rgn, RGN_OR);
			//CombineRgn(&rgn, &rgnTemp, RGN_OR);

			//Delete the temporary "region", otherwise an error occurs on the next creation
			//rgnTemp.DeleteObject();
			::DeleteObject(rgnTemp);

		}while(iPos < iOffsetX + uWidth);
	}

	//memDC.SelectObject(pOldMemBmp);
	::SelectObject(memDC, pOldMemBmp);

	return rgn;
}
#endif
// oscar@snda

/**
 * Splash screen thread entry function
 */
DWORD WINAPI StartSplashScreenThread( LPVOID unused )
{
    WNDCLASS wc;
	wc.style       = CS_HREDRAW | CS_VREDRAW; 
	wc.lpfnWndProc = (WNDPROC) SplashScreenWindowProc; 
	wc.cbClsExtra  = 0; 
	wc.cbWndExtra  = 0; 
	wc.hInstance   = hInstance; 

	wc.hIcon       = LoadIcon(hInstance, MAKEINTRESOURCE(GIsEditor ? GEditorIcon : GGameIcon));
	if(wc.hIcon == NULL)
	{
		wc.hIcon   = LoadIcon((HINSTANCE) NULL, IDI_APPLICATION); 
	}

	wc.hCursor     = LoadCursor((HINSTANCE) NULL, IDC_ARROW); 
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = TEXT("SplashScreenClass"); 
 
	if(!RegisterClass(&wc)) 
	{
		return 0; 
    } 

	// Load splash screen image, display it and handle all window's messages
	//GSplashScreenBitmap = (HBITMAP) LoadImage(hInstance, (LPCTSTR)GSplashScreenFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if(g_image)
	{
		// oscar@snda
		SIZE sizeWindow = { g_image->GetWidth(), g_image->GetHeight()};
		GBmpSize = sizeWindow;
		// oscar@snda


// 		BITMAP bm;
// 		GetObject(GSplashScreenBitmap, sizeof(bm), &bm);
		INT ScreenPosX = (GetSystemMetrics(SM_CXSCREEN) - GBmpSize.cx) / 2;
		INT ScreenPosY = (GetSystemMetrics(SM_CYSCREEN) - GBmpSize.cy) / 2;
		

		// Force the editor splash screen to show up in the taskbar and alt-tab lists
		DWORD dwWindowStyle = WS_EX_APPWINDOW | WS_EX_TOOLWINDOW;
		
		if(!GIsEditor)
		{
			dwWindowStyle = dwWindowStyle | WS_EX_TOPMOST; // oscar@snda
		}

		GSplashScreenWnd = CreateWindowEx(dwWindowStyle, wc.lpszClassName, TEXT("SplashScreen"), WS_BORDER|WS_POPUP,
			ScreenPosX, ScreenPosY, GBmpSize.cx, GBmpSize.cy, (HWND) NULL, (HMENU) NULL, hInstance, (LPVOID) NULL); 

		// Setup font
		{
			HFONT SystemFontHandle = ( HFONT )GetStockObject( DEFAULT_GUI_FONT );

			// Create small font
			{
				LOGFONT MyFont;
				appMemzero( &MyFont, sizeof( MyFont ) );
				GetObject( SystemFontHandle, sizeof( MyFont ), &MyFont );
				MyFont.lfHeight = 14; // oscar@snda
				// MyFont.lfQuality = ANTIALIASED_QUALITY;
				GSplashScreenSmallTextFontHandle = CreateFontIndirect( &MyFont );
				if( GSplashScreenSmallTextFontHandle == NULL )
				{
					// Couldn't create font, so just use a system font
					GSplashScreenSmallTextFontHandle = SystemFontHandle;
				}
			}

			// Create normal font
			{
				LOGFONT MyFont;
				appMemzero( &MyFont, sizeof( MyFont ) );
				GetObject( SystemFontHandle, sizeof( MyFont ), &MyFont );
				MyFont.lfHeight = 14; // oscar@snda
				// MyFont.lfQuality = ANTIALIASED_QUALITY;
				GSplashScreenNormalTextFontHandle = CreateFontIndirect( &MyFont );
				if( GSplashScreenNormalTextFontHandle == NULL )
				{
					// Couldn't create font, so just use a system font
					GSplashScreenNormalTextFontHandle = SystemFontHandle;
				}
			}
		}

		// oscar@snda
// 		SIZE sizeWindow = { g_image->GetWidth(), g_image->GetHeight()};
// 		GBmpSize = sizeWindow;


		DWORD dwExStyle = ::GetWindowLong(GSplashScreenWnd, GWL_EXSTYLE);
		if ((dwExStyle & 0x80000) != 0x80000)
			::SetWindowLong(GSplashScreenWnd, GWL_EXSTYLE, dwExStyle | 0x80000);

		HDC hDC = ::GetDC(GSplashScreenWnd);
		HDC hdcMemory = CreateCompatibleDC(hDC);

		HBITMAP hBitMap = CreateCompatibleBitmap(hDC, sizeWindow.cx, sizeWindow.cy);
		::SelectObject(hdcMemory, hBitMap);

		RECT rcWindow;
		::GetWindowRect(GSplashScreenWnd, &rcWindow);

		BITMAPINFOHEADER stBmpInfoHeader = { 0 };   
		int nBytesPerLine = ((sizeWindow.cx * 32 + 31) & (~31)) >> 3;
		stBmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);   
		stBmpInfoHeader.biWidth = sizeWindow.cx;   
		stBmpInfoHeader.biHeight = sizeWindow.cy;   
		stBmpInfoHeader.biPlanes = 1;   
		stBmpInfoHeader.biBitCount = 32;   
		stBmpInfoHeader.biCompression = BI_RGB;   
		stBmpInfoHeader.biClrUsed = 0;   
		stBmpInfoHeader.biSizeImage = nBytesPerLine * sizeWindow.cy;   

		PVOID pvBits = NULL;   
		HBITMAP hbmpMem = ::CreateDIBSection(NULL, (PBITMAPINFO)&stBmpInfoHeader, DIB_RGB_COLORS, &pvBits, NULL, 0);
		check(hbmpMem != NULL);
		memset( pvBits, 0, sizeWindow.cx * 4 * sizeWindow.cy);
		if(hbmpMem)   
		{   
			HGDIOBJ hbmpOld = ::SelectObject( hdcMemory, hbmpMem);

			//POINT ptWinPos = { m_rcWindow.left, m_rcWindow.top };
			POINT ptWinPos = { rcWindow.left, rcWindow.top };
			Gdiplus::Graphics graph(hdcMemory);
			graph.SetSmoothingMode(Gdiplus::SmoothingModeNone);

			graph.DrawImage(g_image, 0, 0, sizeWindow.cx, sizeWindow.cy);

			HMODULE hFuncInst = LoadLibrary(_T("User32.DLL"));
			typedef BOOL (WINAPI *MYFUNC)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);          
			MYFUNC UpdateLayeredWindow;
#pragma warning(push)
#pragma warning(disable : 4191)
			UpdateLayeredWindow = (MYFUNC)::GetProcAddress(hFuncInst, "UpdateLayeredWindow");
#pragma warning(pop)

			POINT ptSrc = { 0, 0};
			UpdateLayeredWindow(GSplashScreenWnd, hDC, &ptWinPos, &sizeWindow, hdcMemory, &ptSrc, 0, &g_blend, 2);

			graph.ReleaseHDC(hdcMemory);
			::SelectObject( hdcMemory, hbmpOld);   
			::DeleteObject(hbmpMem); 
		}

		::DeleteDC(hdcMemory);
		::DeleteDC(hDC);
		//GSplashRegn = CU_SetupRegion(hdc, GSplashScreenBitmap, RGB(255, 0, 255));
		//::SetWindowRgn(GSplashScreenWnd, GSplashRegn, TRUE);		
		#ifndef _DEBUG
		::SetWindowPos(GSplashScreenWnd, (GIsEditor ? HWND_TOP : HWND_TOPMOST), ScreenPosX, ScreenPosY, GBmpSize.cx, GBmpSize.cy, SWP_NOMOVE|SWP_SHOWWINDOW);
		#endif
		// oscar@snda
		

		UBOOL bShowThirdPartyCopyrightInfo = FALSE;
#if UDK
		bShowThirdPartyCopyrightInfo = TRUE;
#endif
		// Setup bounds for version info text 1
		GSplashScreenTextRects[ SplashTextType::VersionInfo1 ].top = GBmpSize.cy - 60;
		GSplashScreenTextRects[ SplashTextType::VersionInfo1 ].bottom = GBmpSize.cy - 40;
		GSplashScreenTextRects[ SplashTextType::VersionInfo1 ].left = 10;
		GSplashScreenTextRects[ SplashTextType::VersionInfo1 ].right = GBmpSize.cx - 20;

		// Setup bounds for copyright info text
		if( GIsEditor )
		{
			GSplashScreenTextRects[ SplashTextType::CopyrightInfo ].top = GBmpSize.cy - 54;
			GSplashScreenTextRects[ SplashTextType::CopyrightInfo ].bottom = GBmpSize.cy - 34;
			GSplashScreenTextRects[ SplashTextType::ThirdPartyCopyrightInfo ].top = GBmpSize.cy - 44;
			GSplashScreenTextRects[ SplashTextType::ThirdPartyCopyrightInfo ].bottom = GBmpSize.cy - 24;
		}
		else
		{
			if( bShowThirdPartyCopyrightInfo )
			{
				GSplashScreenTextRects[ SplashTextType::CopyrightInfo ].top = GBmpSize.cy - 36;
				GSplashScreenTextRects[ SplashTextType::CopyrightInfo ].bottom = GBmpSize.cy - 16;
				GSplashScreenTextRects[ SplashTextType::ThirdPartyCopyrightInfo ].top = GBmpSize.cy - 16;
				GSplashScreenTextRects[ SplashTextType::ThirdPartyCopyrightInfo ].bottom = GBmpSize.cy - 6;
			}
			else
			{
				GSplashScreenTextRects[ SplashTextType::CopyrightInfo ].top = GBmpSize.cy - 26;
				GSplashScreenTextRects[ SplashTextType::CopyrightInfo ].bottom = GBmpSize.cy - 6;
			}
		}
		GSplashScreenTextRects[ SplashTextType::CopyrightInfo ].left = 20;
		GSplashScreenTextRects[ SplashTextType::CopyrightInfo ].right = GBmpSize.cx - 20;
		GSplashScreenTextRects[ SplashTextType::ThirdPartyCopyrightInfo ].left = 10;
		GSplashScreenTextRects[ SplashTextType::ThirdPartyCopyrightInfo ].right = GBmpSize.cx - 20;

		// Setup bounds for startup progress text
		GSplashScreenTextRects[ SplashTextType::StartupProgress ].top = GBmpSize.cy - 20;
		GSplashScreenTextRects[ SplashTextType::StartupProgress ].bottom = GBmpSize.cy;
		GSplashScreenTextRects[ SplashTextType::StartupProgress ].left = 10;
		GSplashScreenTextRects[ SplashTextType::StartupProgress ].right = GBmpSize.cx - 20;


		// In the editor, we'll display loading info
		if( GIsEditor )
		{
			// Set initial startup progress info
			{
				appSetSplashText( SplashTextType::StartupProgress,
								  *LocalizeUnrealEd( TEXT( "SplashScreen_InitialStartupProgress" ) ) );
			}

			// Set version info
			{
#if _WIN64
				const FString PlatformBitsString( TEXT( "64" ) );
#else
				const FString PlatformBitsString( TEXT( "32" ) );
#endif

				FString RHIName = ShaderPlatformToText( GRHIShaderPlatform, TRUE, TRUE );

#if UDK
				const FString AppName = FString::Printf( LocalizeSecure( LocalizeUnrealEd( "UDKTitle_RHI_F" ), *PlatformBitsString, *RHIName ) );
#else
				FString GameName = GConfig->GetStr(TEXT("URL"), TEXT("GameName"), GEngineIni);
				// oscar@snda
		FString GameVer = FString::Printf(TEXT("%d.%d.%d"), MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER );
				GameName = FString::Printf(TEXT("%s(%s)"), *GameName,  *GameVer);
				// oscar@snda

				//const FString AppName = FString::Printf( LocalizeSecure( LocalizeUnrealEd( "UnrealEdTitle_RHI_F" ), *GameName, *PlatformBitsString, *RHIName ) );
#endif
				//const FString VersionInfo1 = FString::Printf( LocalizeSecure( LocalizeUnrealEd( TEXT( "SplashScreen_VersionInfo1_F" ) ), *AppName, GEngineVersion, GBuiltFromChangeList ) );
				//appSetSplashText( SplashTextType::VersionInfo1, *VersionInfo1 );

				// Change the window text (which will be displayed in the taskbar)
				//SetWindowText(GSplashScreenWnd, *AppName);
			}
		}
		

		// Display copyright information in both editor and game splash screens
		{
			const FString CopyrightInfo = LocalizeGeneral( TEXT( "SplashScreen_CopyrightInfo" ), TEXT("Engine") );
			appSetSplashText( SplashTextType::CopyrightInfo, *CopyrightInfo );

			if( bShowThirdPartyCopyrightInfo )
			{
				// In UDK builds, we must also display third-party copyright info
				const FString ThirdPartyCopyrightInfo = LocalizeGeneral( TEXT( "SplashScreen_ThirdPartyCopyrightInfo" ), TEXT("Engine") );
				appSetSplashText( SplashTextType::ThirdPartyCopyrightInfo, *ThirdPartyCopyrightInfo );
			}
		}


		if (GSplashScreenWnd)
		{
#ifndef _DEBUG
			ShowWindow(GSplashScreenWnd, SW_SHOW); 
			UpdateWindow(GSplashScreenWnd); 
#endif
		 
			MSG message;
			while (GetMessage(&message, NULL, 0, 0))
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}

		//DeleteObject(GSplashScreenBitmap);
		//GSplashScreenBitmap = NULL;
	}

	UnregisterClass(wc.lpszClassName, hInstance);
    return 0; 
}

/**
 * Displays a splash window with the specified image.  The function does not use wxWidgets.  The splash
 * screen variables are stored in static global variables (SplashScreen*).  If the image file doesn't exist,
 * the function does nothing.
 *
 * @param	InSplashName	Name of splash screen file (and relative path if needed)
 */
void appShowSplash( const TCHAR* InSplashName )
{
	// oscar@snda
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	// oscar@snda

	if( ParseParam(appCmdLine(),TEXT("NOSPLASH")) != TRUE )
	{
		// make sure a splash was found
		FString SplashPath;
		if (appGetSplashPath(InSplashName, SplashPath) == TRUE)
		{
			const FString Filepath = FString( appBaseDir() ) * SplashPath;
			GSplashScreenFileName = (TCHAR*) appMalloc(MAX_FILEPATH_LENGTH*sizeof(TCHAR));
			appStrncpy(GSplashScreenFileName, *Filepath, MAX_FILEPATH_LENGTH);

			// oscar@snda
			g_image = Gdiplus::Image::FromFile(GSplashScreenFileName);
			g_blend.BlendOp = 0;
			g_blend.BlendFlags = 0;
			g_blend.AlphaFormat = 1;
			g_blend.SourceConstantAlpha = 255; // AC_SRC_ALPHA
			// oscar@snda

			GSplashScreenThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartSplashScreenThread, (LPVOID)NULL, 0, NULL);
		}
	}
}

/**
 * Destroys the splash window that was previously shown by appShowSplash(). If the splash screen isn't active,
 * it will do nothing.
 */
void appHideSplash()
{
	if(GSplashScreenThread)
	{
		if(GSplashScreenWnd)
		{
			// Send message to splash screen window to destroy itself
			PostMessage(GSplashScreenWnd, WM_DESTROY, 0, 0);
		}

		// Wait for splash screen thread to finish
		WaitForSingleObject(GSplashScreenThread, INFINITE);

		// Clean up
		CloseHandle(GSplashScreenThread);
		appFree(GSplashScreenFileName);
		GSplashScreenFileName = NULL; 
		GSplashScreenThread = NULL;
		GSplashScreenWnd = NULL;

		// oscar@snda
		Gdiplus::DllExports::GdipFree(g_image);
		g_image = 0;

		Gdiplus::GdiplusShutdown(gdiplusToken);
		// oscar@snda
	}
}



/**
 * Sets the text displayed on the splash screen (for startup/loading progress)
 *
 * @param	InType		Type of text to change
 * @param	InText		Text to display
 */
void appSetSplashText( const SplashTextType::Type InType, const TCHAR* InText )
{
	// We only want to bother drawing startup progress in the editor, since this information is
	// not interesting to an end-user (also, it's not usually localized properly.)
	//InType = SplashTextType::CopyrightInfo;
	
	// oscar@snda

	if( GSplashScreenThread )
	{
		// Only allow copyright text displayed while loading the game.  Editor displays all.
		//if( InType == SplashTextType::CopyrightInfo || InType == SplashTextType::ThirdPartyCopyrightInfo || GIsEditor )
		{
			{
				// Take a critical section since the splash thread may already be repainting using this text
				FScopeLock ScopeLock( &GSplashScreenSynchronizationObject );

				// Update splash text
				GSplashScreenText[ SplashTextType::CopyrightInfo ] = InText;
			}


			// oscar@snda
			INT ScreenPosX = (GetSystemMetrics(SM_CXSCREEN) - GBmpSize.cx) / 2;
			INT ScreenPosY = (GetSystemMetrics(SM_CYSCREEN) - GBmpSize.cy) / 2;

			if(!GIsEditor)
			{
#ifndef _DEBUG
				::SetWindowPos(GSplashScreenWnd, HWND_TOPMOST, ScreenPosX, ScreenPosY,
					GBmpSize.cx, GBmpSize.cy, SWP_NOMOVE|SWP_SHOWWINDOW);
#endif
			}
			// oscar@snda

			// Repaint the window
			InvalidateRect( GSplashScreenWnd, &GSplashScreenTextRects[ SplashTextType::CopyrightInfo ], FALSE );
		}
	}

	// oscar@snda
}


#endif