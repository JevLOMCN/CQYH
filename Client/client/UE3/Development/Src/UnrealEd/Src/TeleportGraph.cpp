/*=============================================================================
	Kismet.cpp: Gameplay sequence editing
	Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.

@todo UI integration tasks:
- WxTeleportGraph::RebuildTreeControl (only grabs sequences from the level)
- WxTeleportGraph::FindOutputLinkTo (needs to use interface for retrieving the objects contained by the current sequencecontainer)
- WxTeleportGraph::CenterViewOnSeqObj (see General notes below)
- WxTeleportGraph::OpenKismet/OpenSequenceInKismet (assumes that we always want to use the WxTeleportGraph class for the editor)
- WxTeleportGraph::OnOpenNewWindow (calls static function to open a new kismet window, which doesn't use the correct WxTeleportGraph class)

General:
- all code that attempts to find the SequenceObject's ParentSequence by either directly accessing ParentSequence or casting the object's outer to Sequence is unsafe for UI sequence objects
=============================================================================*/

#include "UnrealEd.h"
#include "Factories.h"
#include "UnLinkedObjEditor.h"
#include "TeleportGraph.h"
#include "EngineSequenceClasses.h"
#include "EnginePrefabClasses.h"
#include "EngineAudioDeviceClasses.h"
#include "UnLinkedObjDrawUtils.h"
#include "ScopedTransaction.h"
#include "PropertyWindow.h"
#include "LevelBrowser.h"
#include "DlgRename.h"
#include "ScopedObjectStateChange.h"
#include "InterpEditor.h"

#include "GFxUIClasses.h"
#include "GFxUIUISequenceClasses.h"

#include "TeleportGraphTreeCtrl.h"
#include "FileHelpers.h"

#include "../../Engine/Inc/EngineClasses.h"
#include "../../Engine/Inc/EngineMeshClasses.h"
#include "../../Engine/Inc/EngineAnimClasses.h"
#include "../../Engine/Inc/EngineAIClasses.h"
#include "../../GameFramework/Inc/GameFrameworkClasses.h"
#include "../../MMOGame/Inc/MMOGameClasses.h"

static INT	MultiEventSpacing = 100;
static INT	PasteOffset = 30;
static INT	FitCommentToSelectedBorder = 30;

/** Maximum region that is valid for sequence objects to exist in. */
static INT	MaxSequenceSize = 10000;

static const FColor SelectedColor( 255, 255, 0 );
static const FColor BorderColor( 0, 0, 0 );
static const FColor TitleBgColor( 112, 112, 112 );
static const FColor HighlightColor( 0, 255, 0);
static const FLOAT MapCoordScale(0.01f);

FColor TeleOrigClr(0, 0, 255);
FColor TeleTargetClr(0, 128, 0);
FColor TeleNetNodeClr(4, 187, 169);

enum ETeleportObject
{
	ETO_Map = 0,
	ETO_Orig,
	ETO_Target,
	ETO_TeleportLink,
	ETO_NetNode
};

/*-----------------------------------------------------------------------------
	WxTeleportGraph
-----------------------------------------------------------------------------*/

BEGIN_EVENT_TABLE( WxTeleportGraph, WxLinkedObjEd )
	EVT_CLOSE( WxTeleportGraph::OnClose )
	EVT_SIZE( WxTeleportGraph::OnSize )

	EVT_TOOL(IDM_TELEPORTGRAPH_REFRESHALL, WxTeleportGraph::OnButtonRefreshAllTeleportGraph)	
	EVT_TOOL(IDM_TELEPORTGRAPH_SAVE, WxTeleportGraph::OnButtonSave)
	EVT_TOOL(IDM_TELEPORTGRAPH_DELETE, WxTeleportGraph::OnButtonDeleteSelected)
	EVT_TOOL(IDM_SERVERMAPINFO_EXPORT, WxTeleportGraph::OnButtonExportServerMapInfos)
	EVT_TOOL(IDM_SERVERMAPINFO_IMPORT, WxTeleportGraph::OnButtonImportServerMapInfos)
END_EVENT_TABLE()



/** Kismet constructor. Initialize variables, create toolbar and status bar, set view to supplied sequence and add to global list of Kismet windows. */
WxTeleportGraph::WxTeleportGraph( wxWindow* InParent, wxWindowID InID, const TCHAR* Title )
: WxLinkedObjEd( InParent, InID, *LocalizeUnrealEd(Title) )
, GlobalMapSingleton(0)
, SelectedMap(0)
, SelectedLink(0)
, SelectedOrig(0)
, SelectedTarget(0)
, SelectedTargetMap(0)
, SelectedOrigMap(0)
, HighlightSelectedMap(0)
, HighlightSelectedTeleObj(0)
, HighlightSelectedLink(0)
{
}
//------------------------------------------------------------------------
/** Kismet destructor. Will remove itself from global list of Kismet windows. */
WxTeleportGraph::~WxTeleportGraph()
{
	GCallbackEvent->UnregisterAll(this);

	// Save out the window properties when the Kismet window is destroyed
	SaveProperties();

	// Remove from global list of Kismet windows.
	check( GApp->TeleportGraphWindows.ContainsItem(this) );
	GApp->TeleportGraphWindows.RemoveItem(this);

	//GEditor->ResetTransaction( *LocalizeUnrealEd("QuitKismet") );
}
//------------------------------------------------------------------------
void WxTeleportGraph::InitEditor()
{
	GlobalMapSingleton = UGlobalMapInfo::GetSingleton();

	CreateControls(TRUE);

	this->LinkedObjVC->MinZoom2D = 0.01f;

	// Register the CALLBACK_RefreshEditor_Kismet event
	//GCallbackEvent->Register(CALLBACK_RefreshEditor_Kismet, this);
	//GCallbackEvent->Register(CALLBACK_Undo, this);
	
	LoadProperties();

	PropertyWindow->SetObject( GlobalMapSingleton,
		EPropertyWindowFlags::Sorted | EPropertyWindowFlags::ShouldShowCategories);


	// Initialize the tree control.
	//RebuildTreeControl();
}
//------------------------------------------------------------------------
/**
 * Creates the controls for this window
 */
void WxTeleportGraph::CreateControls( UBOOL bTreeControl )
{
	//WxLinkedObjEd::CreateControls(bTreeControl);
	GraphWindow = new WxLinkedObjVCHolder( this, -1, this );
	LinkedObjVC = GraphWindow->LinkedObjVC;

	PropertyWindow = new WxPropertyWindowHost;
	PropertyWindow->Create( this, this );
	AddDockingWindow(PropertyWindow, FDockingParent::DH_Bottom, *LocalizeUnrealEd("Properties"));

	if(bTreeControl)
	{
		//PropertyWindow = new WxPropertyWindowHost;
		//PropertyWindow->Create( this, this );
		//AddDockingWindow(PropertyWindow, FDockingParent::DH_Bottom, *LocalizeUnrealEd("Properties"));

		CreateTreeControl(this);
		check(TreeControl);
		AddDockingWindow(TreeControl, FDockingParent::DH_Bottom, *LocalizeUnrealEd("Maps"));
	}
	else
	{
		//PropertyWindow = new WxPropertyWindowHost;
		//PropertyWindow->Create( this, this );
		//AddDockingWindow(PropertyWindow, FDockingParent::DH_Bottom, *LocalizeUnrealEd("Properties"));
	}

	// Add main docking pane.
	AddDockingWindow( GraphWindow, FDockingParent::DH_None, NULL );

	// Try to load a existing layout for the docking windows.
	LoadDockingLayout();

	wxMenuBar* MenuBar = new wxMenuBar();
	AppendWindowMenu(MenuBar);
	SetMenuBar(MenuBar);

	// activate the base sequence
// 	USequence* InitialActiveSequence = Sequence;
// 	Sequence = NULL;

	//ChangeActiveSequence( InitialActiveSequence, TRUE, FALSE );

	// Create toolbar
	ToolBar = new WxTeleportGraphToolBar( this, -1 );
	SetToolBar( ToolBar );

	// Attach the nav. history tools (back/forward/drop-down) to the toolbar
	//NavHistory.AttachToToolBar( ToolBar, FALSE, TRUE, 0 );

	// Bookmarks
	wxMenu* HistoryMenu = NavHistory.HistoryMenu;
	if ( HistoryMenu )
	{
		wxMenu* BookmarkMenu = new wxMenu();
		//BuildBookmarkMenu(BookmarkMenu, FALSE);
		HistoryMenu->AppendSubMenu( BookmarkMenu, *LocalizeUnrealEd("LevelViewportContext_Bookmarks"), *LocalizeUnrealEd("ToolTip_Bookmarks" ) );
		HistoryMenu->AppendSeparator();
	}

	// Create status bar
	StatusBar = new WxTeleportGraphStatusBar( this, -1 );
	SetStatusBar( StatusBar );

	// load the background texture
	BackgroundTexture = LoadObject<UTexture2D>(NULL, TEXT("EditorMaterials.KismetBackground"), NULL, LOAD_None, NULL);

	//InitSeqObjClasses();

	TreeImages->Add( WxBitmap( "KIS_SeqTreeClosed" ), wxColor( 192,192,192 ) ); // 0
	TreeImages->Add( WxBitmap( "KIS_SeqTreeOpen" ), wxColor( 192,192,192 ) ); // 1

	// Keep a list of all Kismets in case we need to close them etc.
	GApp->TeleportGraphWindows.AddItem(this);
}
//------------------------------------------------------------------------
/**
 * Creates the tree control for this linked object editor.  Only called if TRUE is specified for bTreeControl
 * in the constructor.
 *
 * @param	TreeParent	the window that should be the parent for the tree control
 */
void WxTeleportGraph::CreateTreeControl( wxWindow* TreeParent )
{
	TreeImages = new wxImageList( 16, 15 );
	TreeControl = new WxTeleportGraphTreeCtrl;
	((WxTeleportGraphTreeCtrl*)TreeControl)->Create(TreeParent, IDM_LINKEDOBJED_TREE, this);
	TreeControl->AssignImageList(TreeImages);
}
//------------------------------------------------------------------------
/** Called when the window closes (but may be some time before actually destroyed) */
void WxTeleportGraph::OnClose( wxCloseEvent& In )
{
	// Clear out any navigation history
	//NavHistory.ClearHistory();

	WxTeleportGraphTreeCtrl* TreeCtrl = wxDynamicCast(TreeControl,WxTeleportGraphTreeCtrl);
	if ( TreeCtrl != NULL )
	{
		TreeCtrl->ClearTree();
	}
	else
	{
		TreeControl->DeleteAllItems();
	}

	// Save out the window properties when the Kismet window is closed
	SaveProperties();

	this->Destroy();
}
//------------------------------------------------------------------------
void WxTeleportGraph::OnSize(wxSizeEvent& In)
{
	RefreshViewport();
	In.Skip();
}
//------------------------------------------------------------------------
wxFrame* WxTeleportGraph::OpenTeleportGraph( UBOOL bCreateNewWindow, class wxWindow* WindowParent )
{
	if ( bCreateNewWindow || GApp->TeleportGraphWindows.Num() == 0 )
	{
		// Create a new window if requested by the user or if none exist.
		WxTeleportGraph* TeleportGraphEditor = new WxTeleportGraph( WindowParent, -1);
		TeleportGraphEditor->InitEditor();
		TeleportGraphEditor->Show();
		return TeleportGraphEditor;

		// Add a new navigation history item for this action
		//TeleportGraphEditor->AddNewNavigationHistoryDataItem( TeleportGraphEditor->GenerateNavigationHistoryString() );
	}
	else
	{
		wxFrame* TeleportGraphEditor = 0;
		// Bring all kismet windows to focus.
		for (INT Idx = 0; Idx < GApp->TeleportGraphWindows.Num(); Idx++)
		{
			WxTeleportGraph* TeleportGraphWindow = GApp->TeleportGraphWindows(Idx);
			TeleportGraphWindow->Show();
			TeleportGraphWindow->Restore();
			TeleportGraphWindow->SetFocus();
			TeleportGraphEditor = TeleportGraphWindow;
		}

		return TeleportGraphEditor;
	}
}
//------------------------------------------------------------------------
FString WxTeleportGraph::GenerateNavigationHistoryString() const
{
	FString NavHistoryString;
	//if ( Sequence )
	//{
	//	// First compile the sequences name
	//	NavHistoryString += Sequence->GetOutermost()->GetName();
	//	NavHistoryString += TEXT(".");
	//	NavHistoryString += Sequence->GetSeqObjFullName();

	//	const INT NumSelectedObjects = SelectedSeqObjs.Num();

	//	// If only one object is selected, display its name
	//	if ( NumSelectedObjects == 1 )
	//	{
	//		NavHistoryString += TEXT(": ");
	//		NavHistoryString += SelectedSeqObjs(0)->GetName();
	//	}
	//	// If multiple objects are selected, display the number selected
	//	else if ( NumSelectedObjects > 1 )
	//	{
	//		NavHistoryString += TEXT(": ");
	//		NavHistoryString += FString::Printf( LocalizeSecure( LocalizeUnrealEd("Kismet_NavHistory_NodesSelected"), NumSelectedObjects ) );
	//	}
	//}
	return NavHistoryString; 
}
//------------------------------------------------------------------------
void WxTeleportGraph::EdHandleKeyInput( FViewport* Viewport, FName Key, EInputEvent Event )
{
	if( Event == IE_Pressed )
	{		
		if( Key == KEY_Delete )
		{
			DeleteSelected();
		}
	}
}
//------------------------------------------------------------------------
void WxTeleportGraph::Draw_MapTeleports(FMapTeleports* map, FViewport* Viewport, FCanvas* Canvas, INT idx)
{
	FVector2D MapSz = FVector2D(map->CanvasDrawSize)
		* GlobalMapSingleton->MapCanvasScale * map->DrawScale;

	if(Canvas->IsHitTesting())
	{
		Canvas->SetHitProxy( new HAnyObjectProxy(map, (INT)ETO_Map, idx) );
	}

	FColor boxClr = BorderColor;
	if(map->IsSelected)
	{
		boxClr = SelectedColor;
	}
	
	//if(SelectedOrigMap == map)
	//{
	//	boxClr = FColor(1, 0, 0);
	//}

	if(map->IsHighlighted)
	{
		boxClr = HighlightColor;
	}

	if(map->MapMiniMapTexture != 0)
	{
		DrawTile(Canvas, (FLOAT)map->CanvasLocation.X, (FLOAT)map->CanvasLocation.Y,
			(FLOAT)MapSz.X, (FLOAT)MapSz.Y,
			0.0f, 0.0f,
			1.0f, 1.0f, FLinearColor(1, 1, 1),
			map->MapMiniMapTexture->Resource);

		DrawBox2D(Canvas, FVector2D(map->CanvasLocation.X, map->CanvasLocation.Y),
			FVector2D(map->CanvasLocation.X + MapSz.X, map->CanvasLocation.Y + MapSz.Y), boxClr);

		/*FLinkedObjDrawUtils::AABBLiesWithinViewport(Canvas,
			CanvasLocation.X, CanvasLocation.Y, MapSz.X, MapSz.Y);*/
	}
	else
	{
		FLinkedObjDrawUtils::DrawTitleBar(Canvas, map->CanvasLocation, FIntPoint(MapSz.X, MapSz.Y),
			boxClr, FColor(255,255,128),TitleBgColor, *map->MapName, TArray<FString>());

		//song@snda.com
		Canvas->PushAbsoluteTransform(FTranslationMatrix(FVector(LinkedObjVC->Origin2D.X,LinkedObjVC->Origin2D.Y,0))/*FMatrix::Identity*/);
		INT mapID = UGlobalMapInfo::GetSingleton()->FindMapID(map->MapName);
		FLinkedObjDrawUtils::DrawString(Canvas, (map->CanvasLocation.X + 5)*LinkedObjVC->Zoom2D, (map->CanvasLocation.Y + 5)*LinkedObjVC->Zoom2D,
			*FString::Printf(TEXT("MapID:%d MapName:%s"), mapID, *map->MapName), FLinkedObjDrawUtils::GetFont(), FLinearColor(1, 1, 0));
		Canvas->PopTransform();
		//song@snda.com
	}

// 	//const FLOAT Zoom2D = GetUniformScaleFromMatrix(Canvas->GetTransform());
// 
// 	FTranslationMatrix tmx((FVector(map->CanvasLocation) + FVector(LinkedObjVC->Origin2D)) * LinkedObjVC->Zoom2D);
// 	//FScaleMatrix smx(LinkedObjVC->Zoom2D);
// 	//FMatrix mtx = tmx * smx + FTranslationMatrix(FVector(LinkedObjVC->Origin2D));
// 
// 	Canvas->PushAbsoluteTransform(tmx);
// 	FLinkedObjDrawUtils::DrawShadowedString(Canvas, 0, 0, *map->MapName,
// 		FLinkedObjDrawUtils::GetFont(), FColor(255,255,128));
// 	Canvas->PopTransform();

	if(Canvas->IsHitTesting())
	{
		Canvas->SetHitProxy( NULL );
	}

	FIntPoint teleportObjLoc;
	for (INT i = 0; i < map->TeleportOrigList.Num(); ++i)
	{
		FTeleportObject* teleObj = &map->TeleportOrigList(i);
		FVector worldLoc(teleObj->Location.X, teleObj->Location.Y, 0);
		FIntPoint teleportObjLoc = ConvertToCanvasCoord(worldLoc, map, MapSz);

		
		if(Canvas->IsHitTesting())
		{
			Canvas->SetHitProxy( new HAnyObjectProxy(teleObj, (INT)ETO_Orig, i, idx, map) );
		}

		FColor boxClr = BorderColor;
		if(teleObj->IsSelected)
		{
			boxClr = SelectedColor;
		}

		if(teleObj->IsHighlighted)
		{
			boxClr = HighlightColor;
		}

		//song@snda.com
		Canvas->PushAbsoluteTransform(FTranslationMatrix(FVector(LinkedObjVC->Origin2D.X,LinkedObjVC->Origin2D.Y,0))/*FMatrix::Identity*/);

		if(teleObj->IsHighlighted || teleObj->IsSelected)
		{
			FLinkedObjDrawUtils::DrawNGon( Canvas, 
				FVector2D(teleportObjLoc.X*LinkedObjVC->Zoom2D, teleportObjLoc.Y*LinkedObjVC->Zoom2D), boxClr, 18, 8 );
		}

		FLinkedObjDrawUtils::DrawNGon( Canvas, 
			FVector2D(teleportObjLoc.X*LinkedObjVC->Zoom2D, teleportObjLoc.Y*LinkedObjVC->Zoom2D), TeleOrigClr, 16, 6 );

		FString displayName = teleObj->Name;
		if(!teleObj->DisplayName.IsEmpty())
		{
			displayName = teleObj->DisplayName;
		}

		FLinkedObjDrawUtils::DrawShadowedString(Canvas, teleportObjLoc.X*LinkedObjVC->Zoom2D,
			teleportObjLoc.Y*LinkedObjVC->Zoom2D, *displayName, GEngine->TinyFont, TeleOrigClr);
		Canvas->PopTransform();
		//song@snda.com

		if(Canvas->IsHitTesting())
		{
			Canvas->SetHitProxy( NULL );
		}
	}

	for (INT i = 0; i < map->TeleportTargetList.Num(); ++i)
	{
		FTeleportObject* teleObj = &map->TeleportTargetList(i);
		FVector worldLoc(teleObj->Location.X, teleObj->Location.Y, 0);
		FIntPoint teleportObjLoc = ConvertToCanvasCoord(worldLoc, map, MapSz);

		if(Canvas->IsHitTesting())
		{
			Canvas->SetHitProxy( new HAnyObjectProxy(teleObj, (INT)ETO_Target, i, idx, map) );
		}

		FColor boxClr = BorderColor;
		if(teleObj->IsSelected)
		{
			boxClr = SelectedColor;
		}

		if(teleObj->IsHighlighted)
		{
			boxClr = HighlightColor;
		}

		//song@snda.com
		Canvas->PushAbsoluteTransform(FTranslationMatrix(FVector(LinkedObjVC->Origin2D.X,LinkedObjVC->Origin2D.Y,0)));

		if(teleObj->IsHighlighted || teleObj->IsSelected)
		{
			FLinkedObjDrawUtils::DrawNGon( Canvas, 
				FVector2D(teleportObjLoc.X*LinkedObjVC->Zoom2D, teleportObjLoc.Y*LinkedObjVC->Zoom2D), boxClr, 18, 8 );
		}

		FLinkedObjDrawUtils::DrawNGon( Canvas, 
			FVector2D(teleportObjLoc.X*LinkedObjVC->Zoom2D, teleportObjLoc.Y*LinkedObjVC->Zoom2D), TeleTargetClr, 16, 6 );

		FString displayName = teleObj->Name;
		if(!teleObj->DisplayName.IsEmpty())
		{
			displayName = teleObj->DisplayName;
		}

		FLinkedObjDrawUtils::DrawShadowedString(Canvas, teleportObjLoc.X*LinkedObjVC->Zoom2D,
			teleportObjLoc.Y*LinkedObjVC->Zoom2D, *displayName, GEngine->TinyFont, TeleTargetClr);
		Canvas->PopTransform();
		//song@snda.com

		if(Canvas->IsHitTesting())
		{
			Canvas->SetHitProxy( NULL );
		}
	}

	//
	INT sceneID = GlobalMapSingleton->FindMapID( map->MapName );
	FColor nodeColor = TeleNetNodeClr;
	for (INT i = 0; i < GlobalMapSingleton->TeleportNetNodeList.Num(); ++i)
	{
		FTeleportNodeObj* teleObj = &GlobalMapSingleton->TeleportNetNodeList(i);
		if( sceneID != teleObj->InSceneID ){
			continue;
		}

		nodeColor.R = ( teleObj->Region << 4 ) + TeleNetNodeClr.R;
		FVector worldLoc(teleObj->Location.X, teleObj->Location.Y, 0);
		FIntPoint teleportObjLoc = ConvertToCanvasCoord(worldLoc, map, MapSz);

		if(Canvas->IsHitTesting())
		{
			Canvas->SetHitProxy( new HAnyObjectProxy(teleObj, (INT)ETO_NetNode, i, idx, map) );
		}

		FColor boxClr = BorderColor;
		if(teleObj->IsSelected)
		{
			boxClr = SelectedColor;
		}

		if(teleObj->IsHighlighted)
		{
			boxClr = HighlightColor;
		}

		if(teleObj->IsHighlighted || teleObj->IsSelected)
		{
			FLinkedObjDrawUtils::DrawNGon( Canvas, 
				teleportObjLoc, boxClr, 18, 8 );
		}

		FLinkedObjDrawUtils::DrawNGon( Canvas, 
			teleportObjLoc, nodeColor, 16, 6 );

		FString displayName = teleObj->Name;
		displayName += " ServerID:";
		displayName += appItoa( teleObj->ServerID );
		//if(!teleObj->DisplayName.IsEmpty())
		//{
		//	displayName = teleObj->DisplayName;
		//}

		FLinkedObjDrawUtils::DrawShadowedString(Canvas, teleportObjLoc.X,
			teleportObjLoc.Y, *displayName, GEngine->TinyFont, nodeColor);

		if(Canvas->IsHitTesting())
		{
			Canvas->SetHitProxy( NULL );
		}
	}
	// draw coordinate axes
	//DrawLine2D(Canvas, FVector2D(0, 0), FVector2D(100, 0), FLinearColor(1, 0, 0));
	//DrawLine2D(Canvas, FVector2D(0, 0), FVector2D(0, 100), FLinearColor(0, 1, 0));
}
//------------------------------------------------------------------------
void WxTeleportGraph::Draw_TeleportLink(FTeleportLink* lnk, FViewport* Viewport, FCanvas* Canvas, INT idx)
{
	if(Canvas->IsHitTesting())
	{
		Canvas->SetHitProxy( new HAnyObjectProxy(lnk, (INT)ETO_TeleportLink, idx) );
	}

	FColor lnkClr = BorderColor;
	if(lnk->IsInvalid)
	{
		lnkClr = FColor(255, 0, 0);
	}
	else if(lnk->IsSelected || lnk->IsHighlighted)
	{
		lnkClr = SelectedColor;
	}

	FMapTeleports* fromMap = GetMap(lnk->FromMapName);	
	FMapTeleports* toMap = GetMap(lnk->ToMapName);
	
	FVector2D fromMapSz = FVector2D(fromMap->CanvasDrawSize) * GlobalMapSingleton->MapCanvasScale * fromMap->DrawScale;
	FVector2D toMapSz = FVector2D(toMap->CanvasDrawSize) * GlobalMapSingleton->MapCanvasScale * toMap->DrawScale;

	FTeleportObject* teleObjFrom = &lnk->From;
	FVector rotOffsetLocFrom(teleObjFrom->Location.X, teleObjFrom->Location.Y, 0);
	FIntPoint fromPoint = ConvertToCanvasCoord(rotOffsetLocFrom, fromMap, fromMapSz);

	FTeleportObject* teleObjTo = &lnk->To;
	FVector rotOffsetLocTo(teleObjTo->Location.X, teleObjTo->Location.Y, 0);
	FIntPoint toPoint = ConvertToCanvasCoord(rotOffsetLocTo, toMap, toMapSz);

	const FLOAT Tension = Abs<INT>(fromPoint.X - toPoint.X);

	FVector2D dirFrom(0, 0), dirTo(0, 0);
	if(fromPoint.X > toPoint.X)
	{
		dirFrom.X = 1;
		dirTo.X = -1;
	}
	else
	{
		dirFrom.X = -1;
		dirTo.X = 1;
	}

	/*if(fromPoint.Y > toPoint.Y)
	{
		dirFrom.Y = 1;
		dirTo.Y = -1;
	}
	else
	{
		dirFrom.Y = -1;
		dirTo.Y = 1;
	}*/


	FLinkedObjDrawUtils::DrawSpline(Canvas, fromPoint, Tension * dirTo,
		toPoint, Tension * dirTo, lnkClr, TRUE);

	if(Canvas->IsHitTesting())
	{
		Canvas->SetHitProxy( NULL );
	}
}
//------------------------------------------------------------------------
void WxTeleportGraph::DrawObjects( FViewport* Viewport, FCanvas* Canvas )
{
	WxLinkedObjEd::DrawObjects( Viewport, Canvas );

	//DrawBox2D(Canvas, FVector2D(0, 0), FVector2D(100, 100), FLinearColor(1, 0, 0));

	//UGlobalMapInfo::GetSingleton()->Draw(Viewport, Canvas);

	if(GlobalMapSingleton)
	{
		for (INT i = 0; i < GlobalMapSingleton->Teleports.Num(); ++i)
		{
			Draw_MapTeleports(&GlobalMapSingleton->Teleports(i), Viewport, Canvas, i);
		}

		for (INT i = 0; i < GlobalMapSingleton->TeleportLinks.Num(); ++i)
		{
			Draw_TeleportLink(&GlobalMapSingleton->TeleportLinks(i), Viewport, Canvas, i);
		}

		// draw selected info
		Canvas->PushAbsoluteTransform(FMatrix::Identity);

		FLOAT absY = 0.0f;
		FLOAT absX = 10.0f;
		UFont* font = FLinkedObjDrawUtils::GetFont();
		const FLOAT CharHeight = font->GetMaxCharHeight();
		FLOAT xl = 300, yl = CharHeight + 5;
		FColor TextClr(255, 255, 0), BgClr(150, 150, 150, 128);

		FMapTeleports* SelMap = 0;
		if(SelectedOrigMap)
		{
			SelMap = SelectedOrigMap; 
		}

		if(SelectedTargetMap)
		{
			SelMap = SelectedTargetMap;
		}

		if(SelectedMap)
		{
			SelMap = SelectedMap;
		}

		if(SelMap)
		{	
			FLinkedObjDrawUtils::DrawTitleBar(Canvas, FIntPoint(absX, absY),
				FIntPoint(xl, yl), TextClr, FColor(255,255,128),BgClr,
				*SelMap->MapName, TArray<FString>());

			absY += yl;
		}

		FLinkedObjDrawInfo drawInfo;

		if(HighlightSelectedMap)
		{
			drawInfo.ToolTips.AddItem(FString::Printf( TEXT("Map:%s") , *HighlightSelectedMap->MapName));
		}

		if(SelectedOrig)
		{
			FString displayName = SelectedOrig->Name;
			if(!SelectedOrig->DisplayName.IsEmpty())
			{
				displayName = SelectedOrig->DisplayName;
			}

			FLinkedObjDrawUtils::DrawTitleBar(Canvas, FIntPoint(absX, absY),
				FIntPoint(xl, yl), TeleOrigClr, FColor(255,255,128),FColor(0, 0, 255, 50),
				*displayName, TArray<FString>());
			absY += yl;
		}


		if(SelectedTarget)
		{
			FString displayName = SelectedTarget->Name;
			if(!SelectedTarget->DisplayName.IsEmpty())
			{
				displayName = SelectedTarget->DisplayName;
			}
			FLinkedObjDrawUtils::DrawTitleBar(Canvas, FIntPoint(absX, absY),
				FIntPoint(xl, yl), TeleTargetClr, FColor(255,255,128),FColor(0, 128, 0, 50),
				*displayName, TArray<FString>());
			absY += yl;
		}

		if(HighlightSelectedTeleObj)
		{
			FString str = FString::Printf( TEXT("Teleport Object:%s") ,
				*(HighlightSelectedTeleObj->DisplayName.IsEmpty() ? 
				HighlightSelectedTeleObj->Name : HighlightSelectedTeleObj->DisplayName));
				
			drawInfo.ToolTips.AddItem( str );
		}
		
		if(HighlightSelectedLink)
		{
			FString str = FString::Printf( TEXT("Link: [%s]%s -> [%s]%s"),
				*HighlightSelectedLink->FromMapName,
				*(HighlightSelectedLink->From.DisplayName.IsEmpty() ? HighlightSelectedLink->From.Name : HighlightSelectedLink->From.DisplayName),
				*HighlightSelectedLink->ToMapName,
				*(HighlightSelectedLink->To.DisplayName.IsEmpty() ? HighlightSelectedLink->To.Name : HighlightSelectedLink->To.DisplayName));

			drawInfo.ToolTips.AddItem( str );
		}

		FLinkedObjDrawUtils::DrawToolTips(Canvas, drawInfo,
			FIntPoint(LinkedObjVC->OldMouseX, LinkedObjVC->OldMouseY),
			FIntPoint(200, 30));

		Canvas->PopTransform();
	}
}
//------------------------------------------------------------------------
void WxTeleportGraph::UpdatePropertyWindow()
{
	//PropertyWindow->SetObjectArray( GlobalMapSingleton,
	//	EPropertyWindowFlags::Sorted | EPropertyWindowFlags::ShouldShowCategories);
}
//------------------------------------------------------------------------
void WxTeleportGraph::EmptySelection()
{
	HighlightSelectedTeleObj = 0;
	HighlightSelectedLink = 0;
	HighlightSelectedMap = 0;

	if(SelectedMap)
	{
		SelectedMap->IsSelected = FALSE;
	}

	if(SelectedLink)
	{
		SelectedLink->IsSelected = FALSE;
	}

	if(SelectedOrig)
	{
		SelectedOrig->IsSelected = FALSE;
	}

	if(SelectedTarget)
	{
		SelectedTarget->IsSelected = FALSE;
	}

	SelectedSeqObjs.Empty();
	SelectedMap = 0;
	SelectedOrig = 0;
	SelectedTarget = 0;
	SelectedOrigMap = 0;
	SelectedTargetMap = 0;
	SelectedLink = 0;
	/*if (PropertyWindow != NULL)
	{
		PropertyWindow->SetObject(NULL,EPropertyWindowFlags::NoFlags);
	}*/
}
//------------------------------------------------------------------------
void WxTeleportGraph::AddToSelection( UObject* Obj )
{
	checkSlow( Obj->IsA(UGlobalMapInfo::StaticClass()) );
	if ( Obj->IsA(UGlobalMapInfo::StaticClass()) )
	{
		//SelectedSeqObjs.AddUniqueItem( (USequenceObject*)Obj );
	}
}
//------------------------------------------------------------------------
void WxTeleportGraph::RemoveFromSelection( UObject* Obj )
{
	//checkSlow( Obj->IsA(USequenceObject::StaticClass()) );
	/*if (Obj->IsA(USequenceObject::StaticClass()))
	{
		SelectedSeqObjs.RemoveItem( (USequenceObject*)Obj );
	}*/
}
//------------------------------------------------------------------------
UBOOL WxTeleportGraph::IsInSelection( UObject* Obj ) const
{
	//checkSlow( Obj->IsA(USequenceObject::StaticClass()) );
	//return (Obj->IsA(USequenceObject::StaticClass()) && SelectedSeqObjs.ContainsItem( (USequenceObject*)Obj ));
	return false;
}
//------------------------------------------------------------------------
INT WxTeleportGraph::GetNumSelected() const
{
	INT cnt = 0;
	if(SelectedOrig)
	{
		++cnt;
	}

	if(SelectedTarget)
	{
		++cnt;
	}

	if(SelectedMap
		|| SelectedOrigMap
		|| SelectedTargetMap)
	{
		++cnt;
	}

	return cnt;
}
//------------------------------------------------------------------------
void WxTeleportGraph::SetSelectedConnector( FLinkedObjectConnector& Connector )
{
// 	switch(Connector.ConnType)
// 	{
// 	//case TeleMapType:
// 	//	{
// 	//		//SelectedMap = GetMap(Connector.ConnIndex);
// 	//		break;
// 	//	}
// 	}
}
//------------------------------------------------------------------------
FMapTeleports* WxTeleportGraph::GetMap( INT mapIdx ) const
{
	checkSlow(mapIdx < GlobalMapSingleton->Teleports.Num());
	return &GlobalMapSingleton->Teleports(mapIdx);
}
//------------------------------------------------------------------------
FMapTeleports* WxTeleportGraph::GetMap( const FString& mapName ) const
{
	for (INT i = 0; i < GlobalMapSingleton->Teleports.Num(); ++i)
	{
		if(mapName == GlobalMapSingleton->Teleports(i).MapName)
		{
			return &GlobalMapSingleton->Teleports(i);
		}
	}

	return 0;
}

//------------------------------------------------------------------------
void WxTeleportGraph::ClickedAnyObject( struct HAnyObjectProxy* proxy )
{
	switch(proxy->ObjectTypeFlag)
	{
	case ETO_Map:
		{
			if(SelectedMap)
			{
				SelectedMap->IsSelected = FALSE;
			}

			SelectedMap = (FMapTeleports*)proxy->AnyObject;
			SelectedMap->IsSelected = TRUE;
			break;
		}

	case ETO_Orig:
		{
			if(SelectedOrig)
			{
				SelectedOrig->IsSelected = FALSE;
			}

			if(SelectedOrigMap)
			{
				SelectedOrigMap->IsSelected = FALSE;
			}

			SelectedOrig = (FTeleportObject*)proxy->AnyObject;
			SelectedOrigMap = (FMapTeleports*)proxy->ExtraData;
			SelectedOrig->IsSelected = TRUE;
			break;
		}

	case ETO_Target:
		{
			if(SelectedTarget)
			{
				SelectedTarget->IsSelected = FALSE;
			}

			if(SelectedTargetMap)
			{
				SelectedTargetMap->IsSelected = FALSE;
			}

			SelectedTarget = (FTeleportObject*)proxy->AnyObject;
			SelectedTargetMap = (FMapTeleports*)proxy->ExtraData;
			SelectedTarget->IsSelected = TRUE;
			break;
		}
	case ETO_NetNode:
		{
			if(SelectedOrig)
			{
				SelectedOrig->IsSelected = FALSE;
			}

			if(SelectedOrigMap)
			{
				SelectedOrigMap->IsSelected = FALSE;
			}

			if(SelectedTarget)
			{
				SelectedTarget->IsSelected = FALSE;
			}

			if(SelectedTargetMap)
			{
				SelectedTargetMap->IsSelected = FALSE;
			}

			SelectedOrig = NULL;
			SelectedTarget = NULL;
			SelectedOrigMap = NULL;
			SelectedTargetMap = NULL;
		}
		break;
	case ETO_TeleportLink:
		{
			if(SelectedLink)
			{
				SelectedLink->IsSelected = FALSE;
			}

			SelectedLink = (FTeleportLink*)proxy->AnyObject;
			SelectedLink->IsSelected = TRUE;
			break;
		}
	}
}
//------------------------------------------------------------------------
void WxTeleportGraph::CtrlClickedAnyObject( struct HAnyObjectProxy* proxy )
{
	switch(proxy->ObjectTypeFlag)
	{
	case ETO_Map:
		{
			//SelectedMap = (FMapTeleports*)proxy->AnyObject;
			break;
		}

	case ETO_Orig:
		{
			if(SelectedTarget)
			{
				FTeleportObject* TmpSelectedOrig = (FTeleportObject*)proxy->AnyObject;
				FMapTeleports* TmpSelectedMap = (FMapTeleports*)proxy->ExtraData;

				AddConnect(TmpSelectedMap, TmpSelectedOrig, SelectedTargetMap, SelectedTarget);
			}
			break;
		}
	case ETO_NetNode:
		{
		}
		break;
	case ETO_Target:
		{
			if(SelectedOrig)
			{
				FTeleportObject* TmpSelectedTarget = (FTeleportObject*)proxy->AnyObject;
				FMapTeleports* TmpSelectedMap = (FMapTeleports*)proxy->ExtraData;

				AddConnect(SelectedOrigMap, SelectedOrig, TmpSelectedMap, TmpSelectedTarget);
			}
			break;
		}
	}
}
//------------------------------------------------------------------------
void WxTeleportGraph::AddConnect( struct FMapTeleports* fromMap, struct FTeleportObject* fromOrig, struct FMapTeleports* toMap, struct FTeleportObject* toTarget )
{
	FTeleportLink* ModifyLink = 0;
	for (INT i = 0; i < GlobalMapSingleton->TeleportLinks.Num(); ++i)
	{
		FTeleportLink* lnk = &GlobalMapSingleton->TeleportLinks(i);
		if((lnk->FromMapName == fromMap->MapName)
			&& (lnk->ToMapName == toMap->MapName)
			&& (lnk->From.Name == fromOrig->Name)
			&& (lnk->To.Name == toTarget->Name))
		{
			return;
		}

		//junjie@snda: one to N
		//// only one link from a orig
		//if((lnk->FromMapName == fromMap->MapName)
		//	&& (lnk->From.Name == fromOrig->Name))
		//{
		//	ModifyLink = lnk;
		//	break;
		//}
	}

	bool addNew = false;
	FTeleportLink lnk(EC_EventParm);
	if(!ModifyLink)
	{
		ModifyLink = &lnk;
		addNew = true;
	}

	ModifyLink->FromMapName = fromMap->MapName;
	ModifyLink->ToMapName = toMap->MapName;
	ModifyLink->From = *fromOrig;
	ModifyLink->To = *toTarget;
	int iCurLnkIdx = GlobalMapSingleton->FindLinkByOrig( NULL, ModifyLink->FromMapName, ModifyLink->From.Name ) + 1;
	//ModifyLink->srcLinkID = ( ( ModifyLink->From.ServerID) << 16 ) + ( iCurLnkIdx & 0x0000ffff );
	ModifyLink->srcLinkID = iCurLnkIdx;

	if(addNew)
	{
		GlobalMapSingleton->TeleportLinks.AddItem(lnk);
	}

	MakePackageDirty();
}
//------------------------------------------------------------------------
FIntPoint WxTeleportGraph::ConvertToCanvasCoord( const FVector& worldLoc, FMapTeleports* map, const FVector2D& MapSz )
{
	FVector tmpRotOffsetLoc = worldLoc.RotateAngleAxis(90.0f * C_DegToUnrRot, FVector(0, 0, 1));
	FVector2D offsetLoc(tmpRotOffsetLoc.X * MapCoordScale * map->DrawScale * GlobalMapSingleton->MapCanvasScale,
		tmpRotOffsetLoc.Y * MapCoordScale * map->DrawScale * GlobalMapSingleton->MapCanvasScale);

	FIntPoint canvasCoord = map->CanvasLocation + FIntPoint(-offsetLoc.X, MapSz.Y - offsetLoc.Y);
	return canvasCoord;
}
//------------------------------------------------------------------------
void WxTeleportGraph::MoveSelectedObjects( INT DeltaX, INT DeltaY )
{
	if(SelectedMap)
	{
		SelectedMap->CanvasLocation.X += DeltaX;
		SelectedMap->CanvasLocation.Y += DeltaY;

		MakePackageDirty();
	}
}
//------------------------------------------------------------------------
void WxTeleportGraph::HighlightAnyObject( struct HAnyObjectProxy* proxy )
{
	// clear all highlight
	HighlightSelectedMap = 0;
	HighlightSelectedTeleObj = 0;
	HighlightSelectedLink = 0;

	for (INT i = 0; i < GlobalMapSingleton->Teleports.Num(); ++i)
	{
		FMapTeleports& map = GlobalMapSingleton->Teleports(i);
		map.IsHighlighted = FALSE;

		for (INT j = 0; j < map.TeleportOrigList.Num(); ++j)
		{
			map.TeleportOrigList(j).IsHighlighted = FALSE;
		}

		for (INT j = 0; j < map.TeleportTargetList.Num(); ++j)
		{
			map.TeleportTargetList(j).IsHighlighted = FALSE;
		}
	}

	for (INT i = 0; i < GlobalMapSingleton->TeleportLinks.Num(); ++i)
	{
		GlobalMapSingleton->TeleportLinks(i).IsHighlighted = FALSE;
	}
	//Region teleport network
	for (INT i = 0; i < GlobalMapSingleton->TeleportNetNodeList.Num(); ++i)
	{
		GlobalMapSingleton->TeleportNetNodeList(i).IsHighlighted = FALSE;
	}

	if(!proxy)
	{
		return;
	}

	switch(proxy->ObjectTypeFlag)
	{
	case ETO_Map:
		{
			HighlightSelectedMap = (FMapTeleports*)proxy->AnyObject;
			HighlightSelectedMap->IsHighlighted = TRUE;
			StatusBar->SetStatusText(*HighlightSelectedMap->MapName, 0);
			break;
		}

	case ETO_Orig:
		{
			FTeleportObject* TmpSelectedOrig = (FTeleportObject*)proxy->AnyObject;
			TmpSelectedOrig->IsHighlighted = TRUE; 
			StatusBar->SetStatusText(
				*(TmpSelectedOrig->DisplayName.IsEmpty() ?
				TmpSelectedOrig->Name : TmpSelectedOrig->DisplayName), 0);
		
			HighlightSelectedTeleObj = TmpSelectedOrig;
			break;
		}

	case ETO_NetNode:
		{
			FTeleportNodeObj* TmpSelectedNode = (FTeleportNodeObj*)proxy->AnyObject;
			TmpSelectedNode->IsHighlighted = TRUE; 
			StatusBar->SetStatusText( *appItoa( TmpSelectedNode->ServerID ), 0);

			HighlightSelectedTeleObj = TmpSelectedNode;
			break;
		}
	case ETO_Target:
		{
			FTeleportObject* TmpSelectedOrig = (FTeleportObject*)proxy->AnyObject;
			TmpSelectedOrig->IsHighlighted = TRUE;
			StatusBar->SetStatusText(
				*(TmpSelectedOrig->DisplayName.IsEmpty() ?
				TmpSelectedOrig->Name : TmpSelectedOrig->DisplayName), 0);

			HighlightSelectedTeleObj = TmpSelectedOrig;
			break;
		}

	case ETO_TeleportLink:
		{
			FTeleportLink* TmpSelectedLink = (FTeleportLink*)proxy->AnyObject;
			TmpSelectedLink->IsHighlighted = TRUE; 
			StatusBar->SetStatusText(*FString::Printf( TEXT("[%s]%s -> [%s]%s"),
				*TmpSelectedLink->FromMapName,
				*(TmpSelectedLink->From.DisplayName.IsEmpty() ? TmpSelectedLink->From.Name : TmpSelectedLink->From.DisplayName),
				*TmpSelectedLink->ToMapName,
				*(TmpSelectedLink->To.DisplayName.IsEmpty() ? TmpSelectedLink->To.Name : TmpSelectedLink->To.DisplayName)),
				0);

			HighlightSelectedLink = TmpSelectedLink;
			break;
		}
	}
}
//------------------------------------------------------------------------
void WxTeleportGraph::DoubleClickedAnyObject( struct HAnyObjectProxy* proxy )
{
	if(proxy && (proxy->ObjectTypeFlag == ETO_Map))
	{
		FMapTeleports* TmpSelectedMap = (FMapTeleports*)proxy->AnyObject;

		//Bruce@snda 
		UBOOL bPromptUserToSave = TRUE;
		UBOOL bSaveMapPackages = TRUE;
		UBOOL bSaveContentPackages = FALSE;
		if( FEditorFileUtils::SaveDirtyPackages(bPromptUserToSave, bSaveMapPackages, bSaveContentPackages) == FALSE )
		{
			// something went wrong or the user pressed cancel.  Return to the editor so the user doesn't lose their changes		
			return;
		}
		//Bruce@snda 

		FEditorFileUtils::LoadMap(TmpSelectedMap->MapName);
		//GEditor->Exec(*FString::Printf(TEXT("MAP LOAD FILE=%s"), *TmpSelectedMap->MapName));
	}
}

/*-----------------------------------------------------------------------------
	WxTeleportGraph
-----------------------------------------------------------------------------*/

//
//void WxTeleportGraph::Serialize(FArchive& Ar)
//{
//	WxLinkedObjEd::Serialize(Ar);
//
//	Ar << SeqObjClasses;
//	Ar << ConnSeqOp;
//	Ar << SelectedSeqObjs;
//	Ar << Sequence;
//	Ar << OldSequence;
//	Ar << CopyConnInfo;
//	Ar << OpExposeVarLinkMap;
//}
//
//FArchive& operator<<(FArchive &Ar, FCopyConnInfo &Info)
//{
//	return Ar << Info.SeqObject;
//}
//
//FArchive& operator<<(FArchive &Ar, FExposeVarLinkInfo &Info)
//{
//	Ar << Info.Property << Info.VariableClass << Info.LinkIdx;
//	INT Type = (INT)Info.Type;
//	Ar << Type;
//	if (Ar.IsLoading())
//	{
//		Info.Type = (FExposeVarLinkInfo::EExposeType)Type;
//	}
//	return Ar;
//}
//------------------------------------------------------------------------
void WxTeleportGraph::OnButtonRefreshAllTeleportGraph( wxCommandEvent& In )
{
	EmptySelection();

	//
	GlobalMapSingleton->LoadTeleportsFromXML();
	//refresh Map Npc Data
	GlobalMapSingleton->LoadNPCInfoFromXMLFiles();
	GlobalMapSingleton->LoadNpcSpawnGroupInfoFromXMLFiles();
	//
	GlobalMapSingleton->LoadQuestVolumeInfoFromXMLFiles();
	//refresh Map MapSizeVolume Data
	GlobalMapSingleton->LoadMapSizeVolumeFromXML();
	GlobalMapSingleton->LoadSearchPointFromXMLFiles();

	MakePackageDirty();

	RefreshViewport();
}
//------------------------------------------------------------------------
void WxTeleportGraph::OnButtonDeleteSelected( wxCommandEvent& In )
{
	DeleteSelected();

}
//------------------------------------------------------------------------
void WxTeleportGraph::OnButtonSave( wxCommandEvent& In )
{
	GlobalMapSingleton->SavePackage();
}
//------------------------------------------------------------------------
void WxTeleportGraph::MakePackageDirty()
{
	GlobalMapSingleton->GetGlobalMapInfoPackage()->MarkPackageDirty();
}
//------------------------------------------------------------------------
void WxTeleportGraph::ZoomSelectedAnyObject( FLOAT delta )
{
	if(SelectedMap)
	{
		SelectedMap->DrawScale += delta;
		SelectedMap->DrawScale = Clamp<FLOAT>(SelectedMap->DrawScale,
			0.1f, 5.0f);
	}
}
//------------------------------------------------------------------------
void WxTeleportGraph::OnButtonExportServerMapInfos( wxCommandEvent& In )
{
	GlobalMapSingleton->ExportToServer();
	GlobalMapSingleton->SavePackage();
}

#include "MServerMapCfgExport.h"

void WxTeleportGraph::OnButtonImportServerMapInfos( wxCommandEvent& In )
{
	GWarn->BeginSlowTask(TEXT("Importing scene config data from excel..."), TRUE);
	GWarn->MapCheck_Clear();

	//TArray< TArray<FString> > sceneConfigDat, dungeonDat;	
	//FString sceneFile, dunFile;

	//GConfig->GetString(TEXT("Configuration"), TEXT("ServerDungeonCfgExcelFile"), dunFile, GEditorIni);
	//GConfig->GetString(TEXT("Configuration"), TEXT("ServerMapCfgExcelFile"), sceneFile, GEditorIni);

	//dunFile = GFileManager->ConvertToAbsolutePath(*dunFile);
	//sceneFile = GFileManager->ConvertToAbsolutePath(*sceneFile);

	//MServerMapCfgExport::ImportSceneConfigFromExcel(sceneFile, sceneConfigDat, dunFile, dungeonDat);

	//GlobalMapSingleton->ImportFromExcel(sceneConfigDat, dungeonDat);
	GlobalMapSingleton->ImportFromText();
	GlobalMapSingleton->SavePackage();

	GWarn->EndSlowTask();
	GWarn->MapCheck_ShowConditionally();
}
//------------------------------------------------------------------------
UBOOL operator ==(const FMapTeleports& src, const FMapTeleports& other)
{
	return src.MapName == other.MapName;
}
//------------------------------------------------------------------------
void WxTeleportGraph::DeleteSelected()
{
	if(SelectedLink)
	{
		FTeleportLink* lnk = SelectedLink;
		EmptySelection();

		for (INT i = 0; i < GlobalMapSingleton->TeleportLinks.Num(); ++i)
		{
			if(lnk == &GlobalMapSingleton->TeleportLinks(i))
			{
				GlobalMapSingleton->TeleportLinks.Remove(i);
				//update link id
				GlobalMapSingleton->UpdateLinks();

				MakePackageDirty();
				RefreshViewport();

				return;
			}

		}
	}
	else if(SelectedMap)
	{
		FMapTeleports* map = SelectedMap;
		EmptySelection();

		for (INT i = 0; i < GlobalMapSingleton->TeleportLinks.Num(); )
		{
			if((map->MapName == GlobalMapSingleton->TeleportLinks(i).FromMapName)
				|| (map->MapName == GlobalMapSingleton->TeleportLinks(i).ToMapName))
			{
				GlobalMapSingleton->TeleportLinks.Remove(i);
				i = 0; // reset for find from start
			}
			else
			{
				++i;
			}
		}

		for (INT i = 0; i < GlobalMapSingleton->Teleports.Num(); ++i)
		{
			if(GlobalMapSingleton->Teleports(i).MapName == map->MapName)
			{
				GlobalMapSingleton->Teleports.Remove(i);

				MakePackageDirty();
				RefreshViewport();

				return;
			}
		}
	}
}

//------------------------------------------------------------------------
