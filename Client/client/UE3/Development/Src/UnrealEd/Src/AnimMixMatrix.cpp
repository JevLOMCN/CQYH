/*=============================================================================
	hemu@snda 120416
Animation blend parameter table, used for smooth return playback of animations such as combo actions
=============================================================================*/

#include "UnrealEd.h"
#include "Factories.h"
#include "UnLinkedObjEditor.h"
#include "AnimMixMatrix.h"
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

#include "AnimMixMatrixTreeCtrl.h"
#include "FileHelpers.h"

#include "../../Engine/Inc/EngineClasses.h"
#include "../../Engine/Inc/EngineMeshClasses.h"
#include "../../Engine/Inc/EngineAnimClasses.h"
#include "../../Engine/Inc/EngineAIClasses.h"
#include "../../GameFramework/Inc/GameFrameworkClasses.h"
#include "../../MMOGame/Inc/MMOGameClasses.h"

#include "AnimMixEditState_Normal.h"

/*-----------------------------------------------------------------------------
	AnimMixMatrix
-----------------------------------------------------------------------------*/

enum EAnimMixEditState
{
	eAMES_Normal,
};


BEGIN_EVENT_TABLE( AnimMixMatrix, WxLinkedObjEd )
	EVT_CLOSE( AnimMixMatrix::OnClose )
	EVT_SIZE( AnimMixMatrix::OnSize )

	EVT_TOOL(IDM_AnimMixMATRIX_REFRESHALL, AnimMixMatrix::OnButtonRefreshAllData)	
	EVT_TOOL(IDM_AnimMixMATRIX_SAVE, AnimMixMatrix::OnButtonSave)

END_EVENT_TABLE()

/** Kismet constructor. Initialize variables, create toolbar and status bar, set view to supplied sequence and add to global list of Kismet windows. */
AnimMixMatrix::AnimMixMatrix( wxWindow* InParent, wxWindowID InID, const TCHAR* Title )
: WxLinkedObjEd( InParent, InID, *LocalizeUnrealEd(Title) )
, ActiveEditState(0)
, GamePlayDataPackage(0)
, DataManager(0)
{
	FString PackageName = TEXT("GamePlayData.upk");
	FString ExistingFile, SaveFileName;

	if( GPackageFileCache->FindPackageFile( *PackageName, NULL, ExistingFile ) )
	{
		// If the package already exists, save to the existing filename.
		SaveFileName = ExistingFile;
	}

	GamePlayDataPackage = UObject::LoadPackage( NULL, *PackageName, LOAD_NoWarn );
	if( !GamePlayDataPackage )
	{
		appErrorf( TEXT("Unable to load %s"), *PackageName );
	}

	if(!GamePlayDataPackage->IsFullyLoaded())
	{
		GamePlayDataPackage->FullyLoad();
	}

	for (INT j = 0; j < GamePlayDataPackage->GetNetObjectCount(0); ++j)
	{
		DataManager = Cast<UGamePlayDataManager>(GamePlayDataPackage->GetNetObjectAtIndex(j));
		if(DataManager)
		{
			break;
		}
	}
	EditStates[eAMES_Normal] = new AnimMixEditState_Normal(this);
}
//------------------------------------------------------------------------
/** Kismet destructor. Will remove itself from global list of Kismet windows. */
AnimMixMatrix::~AnimMixMatrix()
{
	EditStateMap::iterator it = EditStates.begin();
	for (; it != EditStates.end(); ++it)
	{
		delete it->second;
	}

	//GCallbackEvent->UnregisterAll(this);

	// Save out the window properties when the Kismet window is destroyed
	SaveProperties();

	//// Remove from global list of Kismet windows.

	check( GApp->AnimMixMatrixWindows.ContainsItem(this) );
	GApp->AnimMixMatrixWindows.RemoveItem(this);

	////GEditor->ResetTransaction( *LocalizeUnrealEd("QuitKismet") );
}
//------------------------------------------------------------------------
void AnimMixMatrix::InitEditor()
{
	//GlobalMapSingleton = UGlobalMapInfo::GetSingleton();

	CreateControls(TRUE);

	// Register the CALLBACK_RefreshEditor_Kismet event
	//GCallbackEvent->Register(CALLBACK_RefreshEditor_Kismet, this);
	//GCallbackEvent->Register(CALLBACK_Undo, this);
	
	LoadProperties();

	//PropertyWindow->SetObject( GlobalMapSingleton,
	//	EPropertyWindowFlags::Sorted | EPropertyWindowFlags::ShouldShowCategories);


	// Initialize the tree control.
	//RebuildTreeControl();

	ActiveState((INT)eAMES_Normal);
}
//------------------------------------------------------------------------
/**
 * Creates the controls for this window
 */
void AnimMixMatrix::CreateControls( UBOOL bTreeControl )
{
	//WxLinkedObjEd::CreateControls(bTreeControl);
	GraphWindow = new WxLinkedObjVCHolder( this, -1, this );
	LinkedObjVC = GraphWindow->LinkedObjVC;

	PropertyWindow = new WxPropertyWindowHost;
	PropertyWindow->Create( this, this );
	AddDockingWindow(PropertyWindow, FDockingParent::DH_Right, *LocalizeUnrealEd("Properties"));

	//if(bTreeControl)
	//{
	//	//PropertyWindow = new WxPropertyWindowHost;
	//	//PropertyWindow->Create( this, this );
	//	//AddDockingWindow(PropertyWindow, FDockingParent::DH_Bottom, *LocalizeUnrealEd("Properties"));

	//	CreateTreeControl(this);
	//	check(TreeControl);
	//	AddDockingWindow(TreeControl, FDockingParent::DH_Bottom, *LocalizeUnrealEd("Maps"));
	//}
	//else
	//{
	//	//PropertyWindow = new WxPropertyWindowHost;
	//	//PropertyWindow->Create( this, this );
	//	//AddDockingWindow(PropertyWindow, FDockingParent::DH_Bottom, *LocalizeUnrealEd("Properties"));
	//}

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
	ToolBar = new AnimMixMatrixToolBar( this, -1 );
	SetToolBar( ToolBar );

	// Attach the nav. history tools (back/forward/drop-down) to the toolbar
	//NavHistory.AttachToToolBar( ToolBar, FALSE, TRUE, 0 );

	// Bookmarks
// 	wxMenu* HistoryMenu = NavHistory.HistoryMenu;
// 	if ( HistoryMenu )
// 	{
// 		wxMenu* BookmarkMenu = new wxMenu();
// 		//BuildBookmarkMenu(BookmarkMenu, FALSE);
// 		HistoryMenu->AppendSubMenu( BookmarkMenu, *LocalizeUnrealEd("LevelViewportContext_Bookmarks"), *LocalizeUnrealEd("ToolTip_Bookmarks" ) );
// 		HistoryMenu->AppendSeparator();
// 	}

	// Create status bar
	StatusBar = new AnimMixMatrixStatusBar( this, -1 );
	SetStatusBar( StatusBar );

	// load the background texture
	BackgroundTexture = LoadObject<UTexture2D>(NULL, TEXT("EditorMaterials.KismetBackground"), NULL, LOAD_None, NULL);

	//InitSeqObjClasses();

	//TreeImages->Add( WxBitmap( "KIS_SeqTreeClosed" ), wxColor( 192,192,192 ) ); // 0
	//TreeImages->Add( WxBitmap( "KIS_SeqTreeOpen" ), wxColor( 192,192,192 ) ); // 1

	// Keep a list of all Kismets in case we need to close them etc.
	GApp->AnimMixMatrixWindows.AddItem(this);
}
//------------------------------------------------------------------------
/**
 * Creates the tree control for this linked object editor.  Only called if TRUE is specified for bTreeControl
 * in the constructor.
 *
 * @param	TreeParent	the window that should be the parent for the tree control
 */
void AnimMixMatrix::CreateTreeControl( wxWindow* TreeParent )
{
	/*
	TreeImages = new wxImageList( 16, 15 );
	TreeControl = new AnimMixMatrixTreeCtrl;
	((AnimMixMatrixTreeCtrl*)TreeControl)->Create(TreeParent, IDM_LINKEDOBJED_TREE, this);
	TreeControl->AssignImageList(TreeImages);
	*/
}
//------------------------------------------------------------------------
/** Called when the window closes (but may be some time before actually destroyed) */
void AnimMixMatrix::OnClose( wxCloseEvent& In )
{
	// Clear out any navigation history
	//NavHistory.ClearHistory();

// 	AnimMixMatrixTreeCtrl* TreeCtrl = wxDynamicCast(TreeControl,AnimMixMatrixTreeCtrl);
// 	if ( TreeCtrl != NULL )
// 	{
// 		TreeCtrl->ClearTree();
// 	}
// 	else
// 	{
// 		TreeControl->DeleteAllItems();
// 	}
	//Always save
	//MakePackageDirty();
	//Save();
	// Save out the window properties when the Kismet window is closed
	SaveProperties();

	this->Destroy();
}
//------------------------------------------------------------------------
void AnimMixMatrix::OnSize(wxSizeEvent& In)
{
	RefreshViewport();
	In.Skip();
}
//------------------------------------------------------------------------
void AnimMixMatrix::OpenEditor( UBOOL bCreateNewWindow, class wxWindow* WindowParent )
{
	if ( bCreateNewWindow || GApp->AnimMixMatrixWindows.Num() == 0 )
	{
		// Create a new window if requested by the user or if none exist.
		AnimMixMatrix* editor = new AnimMixMatrix( WindowParent, -1);
		editor->InitEditor();
		editor->Show();

		// Add a new navigation history item for this action
		//TeleportGraphEditor->AddNewNavigationHistoryDataItem( TeleportGraphEditor->GenerateNavigationHistoryString() );
	}
	else
	{
		// Bring all kismet windows to focus.
		for (INT Idx = 0; Idx < GApp->AnimMixMatrixWindows.Num(); Idx++)
		{
			AnimMixMatrix* editorWindow = GApp->AnimMixMatrixWindows(Idx);
			editorWindow->Show();
			editorWindow->Restore();
			editorWindow->SetFocus();
		}
	}
}
//------------------------------------------------------------------------
FString AnimMixMatrix::GenerateNavigationHistoryString() const
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
void AnimMixMatrix::EdHandleKeyInput( FViewport* Viewport, FName Key, EInputEvent Event )
{

}
//------------------------------------------------------------------------

void AnimMixMatrix::DrawObjects( FViewport* Viewport, FCanvas* Canvas )
{
	WxLinkedObjEd::DrawObjects( Viewport, Canvas );

	if(ActiveEditState)
	{
		ActiveEditState->DrawObjects(Viewport, Canvas);
	}
}
//------------------------------------------------------------------------
void AnimMixMatrix::UpdatePropertyWindow()
{
	//PropertyWindow->SetObjectArray( GlobalMapSingleton,
	//	EPropertyWindowFlags::Sorted | EPropertyWindowFlags::ShouldShowCategories);
}
//------------------------------------------------------------------------
void AnimMixMatrix::EmptySelection()
{
	if(ActiveEditState)
	{
		ActiveEditState->EmptySelection();
	}
}
//------------------------------------------------------------------------
void AnimMixMatrix::AddToSelection( UObject* Obj )
{
	//checkSlow( Obj->IsA(UGlobalMapInfo::StaticClass()) );
	//if ( Obj->IsA(UGlobalMapInfo::StaticClass()) )
	//{
	//	//SelectedSeqObjs.AddUniqueItem( (USequenceObject*)Obj );
	//}
}
//------------------------------------------------------------------------
void AnimMixMatrix::RemoveFromSelection( UObject* Obj )
{
	//checkSlow( Obj->IsA(USequenceObject::StaticClass()) );
	/*if (Obj->IsA(USequenceObject::StaticClass()))
	{
		SelectedSeqObjs.RemoveItem( (USequenceObject*)Obj );
	}*/
}
//------------------------------------------------------------------------
UBOOL AnimMixMatrix::IsInSelection( UObject* Obj ) const
{
	//checkSlow( Obj->IsA(USequenceObject::StaticClass()) );
	//return (Obj->IsA(USequenceObject::StaticClass()) && SelectedSeqObjs.ContainsItem( (USequenceObject*)Obj ));
	return false;
}
//------------------------------------------------------------------------
INT AnimMixMatrix::GetNumSelected() const
{
	if(ActiveEditState)
	{
		return ActiveEditState->GetNumSelected();
	}
	return 0;
}
//------------------------------------------------------------------------
void AnimMixMatrix::SetSelectedConnector( FLinkedObjectConnector& Connector )
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
void AnimMixMatrix::ClickedAnyObject( struct HAnyObjectProxy* proxy )
{
	if(ActiveEditState)
	{
		ActiveEditState->ClickedAnyObject(proxy);
	}
}
//------------------------------------------------------------------------
void AnimMixMatrix::CtrlClickedAnyObject( struct HAnyObjectProxy* proxy )
{
	if(ActiveEditState)
	{
		ActiveEditState->CtrlClickedAnyObject(proxy);
	}
}
//------------------------------------------------------------------------
void AnimMixMatrix::MoveSelectedObjects( INT DeltaX, INT DeltaY )
{	
}
//------------------------------------------------------------------------
void AnimMixMatrix::HighlightAnyObject( struct HAnyObjectProxy* proxy )
{
	if(ActiveEditState)
	{
		ActiveEditState->HighlightAnyObject(proxy);
	}
}
//------------------------------------------------------------------------
void AnimMixMatrix::DoubleClickedAnyObject( struct HAnyObjectProxy* proxy )
{
	if(ActiveEditState)
	{
		ActiveEditState->DoubleClickedAnyObject(proxy);
	}
}
//------------------------------------------------------------------------
void AnimMixMatrix::OnButtonRefreshAllData( wxCommandEvent& In )
{
	/*EmptySelection();

	GlobalMapSingleton->LoadTeleportsFromXML();

	MakePackageDirty();
*/
	RefreshViewport();
}
//------------------------------------------------------------------------
void AnimMixMatrix::OnButtonSave( wxCommandEvent& In )
{
	Save();
}
//------------------------------------------------------------------------
void AnimMixMatrix::Save()
{
	if(ActiveEditState)
	{
		ActiveEditState->SaveData();
	}
	TArray<UPackage*> Packages;
	Packages.AddItem(GamePlayDataPackage);
	FEditorFileUtils::PromptForCheckoutAndSave(Packages, TRUE, TRUE);
}
//------------------------------------------------------------------------
void AnimMixMatrix::MakePackageDirty()
{
	GamePlayDataPackage->MarkPackageDirty();
}
//------------------------------------------------------------------------
void AnimMixMatrix::ActiveState( INT StateID )
{
	if(ActiveEditState)
	{
		ActiveEditState->OnExit();
	}
	ActiveEditState = EditStates[StateID];
	if(ActiveEditState)
	{
		ActiveEditState->OnEnter();
	}
	RefreshViewport();
}
//------------------------------------------------------------------------
UGamePlayDataManager* AnimMixMatrix::GetDataManager()
{
	checkSlow(DataManager != 0);
	return DataManager;
}
//------------------------------------------------------------------------
void AnimMixMatrix::SetObjectToPropertyWindow( UObject* object )
{
	PropertyWindow->SetObject(object, EPropertyWindowFlags::Sorted);

	// | EPropertyWindowFlags::ShouldShowCategories
	//PropertyWindow->ExpandAllItems();
}
//------------------------------------------------------------------------
