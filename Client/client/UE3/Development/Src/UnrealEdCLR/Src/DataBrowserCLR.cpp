/*=============================================================================
Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

#include "UnrealEdCLR.h"

#include "ManagedCodeSupportCLR.h"
#include "DataBrowserShared.h"
#include "GameAssetDatabaseShared.h"

#include "SourceControlWindowsShared.h"
#include "ConsolidateWindowShared.h"

#include "ExportMeshUtils.h"
#include "ImageUtils.h"
#include "EngineProcBuildingClasses.h"

#include "UnLinkedObjEditor.h"
#include "SoundCueEditor.h"
#include "PropertyWindowManager.h"	// required for access to GPropertyWindowManager

#include "FileHelpers.h"

#include "ThumbnailToolsCLR.h"
#include "../../Engine/Inc/EngineClasses.h"
#include "../../Engine/Inc/EngineMeshClasses.h"
#include "../../Engine/Inc/EngineAnimClasses.h"
#include "../../Engine/Inc/EngineAIClasses.h"
#include "../../GameFramework/Inc/GameFrameworkClasses.h"
#include "../../MMOGame/Inc/MMOGameClasses.h"
#include "../../UnrealEd/Src/ExportExcel.h"
#include "modifier_file_def.h"
#include "DlgMoveAssets.h"

int g_debugIndex = 0;

using namespace System::Windows::Controls;
using namespace System::Collections::ObjectModel;
using namespace System::Diagnostics;
using namespace System::Windows::Input;

// Swap these comments to enable debug spew with timing numbers for asset queries
//#define PROFILE_CONTENT_BROWSER( Statement ) Statement
#define PROFILE_CONTENT_BROWSER( Statement )


/**
 * If FaceFX is open, gives the user the option to close it.
 *
 * @return						TRUE if FaceFX is closed.
 */




/** Current state of the content browser's query engine */
enum class EDataBrowserQueryState
{
	/** Idle */
	Idle = 0,

	/** Start a fresh query */
	StartNewQuery,

	/** Gather objects and packages */
	GatherObjects,

	/** Add loaded assets */
	AddLoadedAssets,

	/** Add non-loaded assets */
	AddNonLoadedAssets,

	/** Remove  */
	RemoveMissingAssets,
};

/**
 * Utility method for determining which AssetListRefreshMode based on the refresh flags from a CALLBACK_RefreshDataBrowser.
 * 
 * @param RefreshFlags Flags parameter when responding to a CALLBACK_RefreshDataBrowser.
 *
 * @return AssetListRefreshMode to use based on the flags.
 */

typedef	TMap<UGenericBrowserType*, TArray<UClass*> >	GBTToClassMap;

typedef TMap<UClass*, TArray<UGenericBrowserType*> >	ClassToGBTMap;
/**
 * DataBrowser window control (managed)
 */
ref class MDataBrowserControl : ContentBrowser::DataIContentBrowserBackendInterface
{

public:

	/** Constructor */
	MDataBrowserControl( FDataBrowser* InNativeBrowserPtr )
	: NativeBrowserPtr( InNativeBrowserPtr )
	{ 
		LastPreviewedAssetFullName = nullptr;
	}


	/** Destructor (deterministic; effectively IDisposable.Dispose() to the CLR) */
	~MDataBrowserControl()
	{
		NativeBrowserPtr = NULL;
		DisposeOfInteropWindow();
	}


protected:

	/** Finalizer (non-deterministic destructor) */
	!MDataBrowserControl()
	{
	}



	/** Clean up the interop window if it still exists */
	void DisposeOfInteropWindow()
	{
		// Dispose of the window.  This also cancels our various event handlers and message hooks.
		if( InteropWindow != nullptr )
		{
			delete InteropWindow;
			InteropWindow = nullptr;
		}
	}



public:

	/**
	 * Initialize the content browser
	 *
	 * @param	InParentBrowser			Parent browser window (or NULL if we're not parented to a browser.)
	 * @param	InParentWindowHandle	Parent window handle
	 *
	 * @return	TRUE if successful
	 */
	UBOOL InitDataBrowser( WxDataBrowserHost* InParentBrowser, const HWND InParentWindowHandle )
	{
		// Load info needed by quest zhangrui05@snda
		UGlobalMapInfo* GlobalMapSingleton = UGlobalMapInfo::GetSingleton();
		if (GlobalMapSingleton)
		{
			GlobalMapSingleton->LoadNPCInfoFromXMLFiles();
			GlobalMapSingleton->LoadNaviPointInfoFromXMLFiles();
		}

		ParentBrowserWindow = InParentBrowser;

		// Start off idle
		QueryEngineState = EDataBrowserQueryState::Idle;
		bIsSCCStateDirty = false;

		LastExportPath = CLRTools::ToString(GApp->LastDir[LD_GENERIC_EXPORT]);
		LastImportPath = CLRTools::ToString(GApp->LastDir[LD_GENERIC_IMPORT]);
		LastOpenPath = CLRTools::ToString(GApp->LastDir[LD_GENERIC_OPEN]);
		LastSavePath = CLRTools::ToString(GApp->LastDir[LD_GENERIC_SAVE]);

		LastImportFilter = 0;

		// Initialize browsable object type list
		InitBrowsableObjectTypeList();

		InitFactoryClassList();

		// Setup WPF window to be created as a child of the browser window
		Interop::HwndSourceParameters sourceParams( "DataBrowserHost" );
		sourceParams.PositionX = 0;
		sourceParams.PositionY = 0;
		sourceParams.ParentWindow = (IntPtr)InParentWindowHandle;
		sourceParams.WindowStyle = (WS_VISIBLE | WS_CHILD);
		
		InteropWindow = gcnew Interop::HwndSource(sourceParams);
		InteropWindow->SizeToContent = SizeToContent::Manual;

		// Need to make sure any faulty WPF methods are hooked as soon as possible after a WPF window
		// has been created (it loads DLLs internally, which can't be hooked until after creation)
#if WITH_EASYHOOK
		WxUnrealEdApp::InstallHooksWPF();
#endif

		DataBrowserCtrl = gcnew ContentBrowser::DataMainControl();
		DataBrowserCtrl->Initialize(this);

		InteropWindow->RootVisual = DataBrowserCtrl;

		// Init the recently accessed browser objects stack
		InitRecentObjectsList();

		// Attach our message hook routine.  This is only for catching messages that do not
		// have WPF equivalents.  The WPF messages can be handled through the HwndSource
		// object itself!
		InteropWindow->AddHook(
			gcnew Interop::HwndSourceHook( this, &MDataBrowserControl::MessageHookFunction ) );


		// Setup "Close" button
		DataBrowserCtrl->CloseButton->IsEnabled = true;
		DataBrowserCtrl->CloseButton->Click +=
				gcnew RoutedEventHandler( this, &MDataBrowserControl::OnCloseButtonClicked );



		// Setup "Clone" button
		DataBrowserCtrl->CloneButton->IsEnabled = true;
		DataBrowserCtrl->CloneButton->Click +=
				gcnew RoutedEventHandler( this, &MDataBrowserControl::OnCloneButtonClicked );



		// We always enable the float/dock button when we're associated with a WxBrowser
		if( ParentBrowserWindow != NULL )
		{
			// Enable the button and route click events
			DataBrowserCtrl->FloatOrDockButton->IsEnabled = true;
			DataBrowserCtrl->FloatOrDockButton->Click +=
				gcnew RoutedEventHandler( this, &MDataBrowserControl::OnFloatOrDockButtonClicked );
		}
		else
		{
			// Disable and hide the button
			DataBrowserCtrl->FloatOrDockButton->IsEnabled = false;
			DataBrowserCtrl->FloatOrDockButton->Visibility = Visibility::Collapsed;
		}

		// Capture keyboard input
		DataBrowserCtrl->KeyDown += gcnew System::Windows::Input::KeyEventHandler( this, &MDataBrowserControl::OnKeyPressed );
		// Capture mouse input
		DataBrowserCtrl->MouseDown += gcnew System::Windows::Input::MouseButtonEventHandler( this, &MDataBrowserControl::OnMouseButtonPressed );

		//force collection list to pre-populate
		UpdateCollectionsList();

		// Show the window!
		const HWND WPFWindowHandle = ( HWND )InteropWindow->Handle.ToPointer();
		::ShowWindow( WPFWindowHandle, SW_SHOW );

		return TRUE;
	}


	/** Returns pointer to the WxDataBrowserHost that we're parented to (may be NULL if there is none.) */
	WxDataBrowserHost* GetParentBrowserWindow()
	{
		return ParentBrowserWindow;
	}


	/**
	 * Propagates focus to the WPF control.
	 */
	void SetFocus()
	{
		if ( InteropWindow != nullptr )
		{
			DataBrowserCtrl->Focus();
		}
	}


	/** Either enables or disables the WPF window.  Used when another editor window is becoming modal. */
	void EnableWindow( bool bInEnable )
	{
		if( DataBrowserCtrl != nullptr )
		{
			DataBrowserCtrl->IsEnabled = bInEnable;
		}
	}


	/** @return	the list of classes which render static shared thumbnail (unique per class) */
	TArray<UClass*>* GetSharedThumbnailClasses()
	{
		TArray<UClass*>* Result = NULL;
		if ( SharedThumbnailClasses.IsValid() )
		{
			Result = SharedThumbnailClasses.Get();
		}
		return Result;
	}


	/** Returns the map of classes to browsable object types */
	ClassToGBTMap& GetBrowsableObjectTypeMap()
	{
		check( ClassToBrowsableObjectTypeMap.IsValid() );
		return *ClassToBrowsableObjectTypeMap.Get();
	}



	/**
	 * Begin a drag-n-drop operation.
	 *
	 * @param	SelectedAssetPaths	string containing fully qualified pathnames for the assets which will be part
	 *			of the d&d operation, delimited by the pipe character
	 */
	virtual void BeginDragDrop( String^ SelectedAssetPaths )
	{
		wxTextDataObject Obj(*CLRTools::ToFString(SelectedAssetPaths));

		wxDropSource dragSource( Obj, ParentBrowserWindow/*,const wxIconOrCursor& iconCopy = wxNullIconOrCursor, const wxIconOrCursor& iconMove = wxNullIconOrCursor, const wxIconOrCursor& iconNone = wxNullIconOrCursor*/ );
		wxDragResult result = dragSource.DoDragDrop(wxDrag_CopyOnly);
	}

	/**
	* Begin a drag-n-drop operation for import.  Called when a user drags files from windows to either the asset canvas or the package tree
	*
	* @param	A list of filenames the user dropped
	*
	*/
	virtual void BeginDragDropForImport( List<String^>^ DroppedFiles )
	{
		//ObjectTools::ImportFiles requires a wxArrayString
		wxArrayString FilesToImport;
		for each ( String^ Str in DroppedFiles )
		{
			FilesToImport.Add( *CLRTools::ToFString( Str ) );
		}

		// There are no files to import
		if(FilesToImport.GetCount() == 0)
			return;

		GWarn->BeginSlowTask( *LocalizeUnrealEd("Importing"), TRUE );

		// All the factories we can possibly use
		TArray<UFactory*> Factories;		

		// Needed by AssembleListofImportFactories but unused here
		FString FileTypes, AllExtensions;
		
		// Obtain a list of factories to use for importing.
		TMultiMap<INT, UFactory*> MapFactory;//zhaojun@snda ???
		ObjectTools::AssembleListOfImportFactories(Factories, FileTypes, AllExtensions, MapFactory);

		// Set the package to a default if no package was selected prior to dropping files.
		FString SelectedPackageName=TEXT("MyPackage"), SelectedGroupName;
		
		// Find a selected package if any.
		GetSelectedPackageAndGroupName(SelectedPackageName, SelectedGroupName);
	
		if ( ObjectTools::ImportFiles( FilesToImport, Factories, NULL, SelectedPackageName, SelectedGroupName ) )
		{
			// Collect garbage.  Some factories create some extra garbage.
			UObject::CollectGarbage( GARBAGE_COLLECTION_KEEPFLAGS );
		}

		GWarn->EndSlowTask();

	}

	/**
	 * Get a list of all the known tags
	 */
	virtual void UpdateTagsCatalogue()
	{

		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return;
		// oscar@snda

		List<String^>^ dictionary = gcnew List<String^>();
		FGameAssetDatabase::Get().QueryAllTags(
			dictionary,
			CBDefs::ShowSystemTags ? ETagQueryOptions::AllTags : ETagQueryOptions::UserTagsOnly );
		DataBrowserCtrl->SetTagsCatalog( dictionary );
	}



	/**
	 * Applies all source/search/filter settings and rebuilds the list of assets
	 */
	virtual void UpdateAssetsInView()
	{
		bDoFullAssetViewUpdate = true;
		StartAssetQuery();
	}


	/**
	 * Get tag-related parameters from the engine. E.g. max tag length
	 */
	virtual ContentBrowser::DataTagUtils::EngineTagDefs^ GetTagDefs()
	{
		return gcnew ContentBrowser::DataTagUtils::EngineTagDefs(GADDefs::MaxTagLength,
															GADDefsNative::SystemTagPreDelimiter,
															GADDefsNative::SystemTagPostDelimiter);
	}

	/**
	 * Create a new Tag
	 * 
	 * @param InTag   A string tag to be created
	 *
	 * @return TRUE is succeeded, FALSE if failed.
	 */
	virtual bool CreateTag( String^ InTag )
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return false;
		// oscar@snda

		FGameAssetDatabase &Gad = FGameAssetDatabase::Get();
		if ( !Gad.CreateTag( InTag ) )
		{
			warnf( *Gad.GetErrorString() );
			String^ Warning = UnrealEd::Utils::Localize("DataBrowser_Warning_FailedCreateTag");
			DataBrowserCtrl->PlayWarning( Warning );
			
			return false;
		}

		return true;
	}

	/**
	 * Destroy an existing Tag
	 * 
	 * @param InTag   A string tag to be destroyed
	 *
	 * @return TRUE is succeeded, FALSE if failed.
	 */
	virtual bool DestroyTag( String^ InTag )
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return false;
		// oscar@snda

		FGameAssetDatabase &Gad = FGameAssetDatabase::Get();
		if ( !Gad.DestroyTag( InTag ) )
		{
			warnf( *Gad.GetErrorString() );
			String^ Warning = UnrealEd::Utils::Localize("DataBrowser_Warning_FailedDestroyTag");
			DataBrowserCtrl->PlayWarning( Warning );

			return false;
		}

		return true;
	}

	/**
	 * Copies (or renames/moves) a tag
	 * 
	 * @param InCurrentTagName The tag to rename
	 * @param InNewTagName The new tag name
	 * @param bInMove True if the old tag should be destroyed after copying it
	 *
	 * @return TRUE is succeeded, FALSE if failed.
	 */
	virtual bool CopyTag( String^ InCurrentTagName, String^ InNewTagName, bool bInMove )
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return false;
		// oscar@snda

		FGameAssetDatabase& Gad = FGameAssetDatabase::Get();
		if ( Gad.CopyTag( InCurrentTagName, InNewTagName, bInMove ) )
		{
			// @todo cb: refresh here?
		}
		else
		{
			warnf( *Gad.GetErrorString() );
			String^ Warning = UnrealEd::Utils::Localize("DataBrowser_Warning_FailedCopyOrRenameTag");
			DataBrowserCtrl->PlayWarning( Warning );

			return false;
		}

		return true;
	}

	/**
	 * Converts an EBrowserCollectionType to an EGADCollection type
	 * 
	 * @param InType	Type to convert
	 */
	static EGADCollection::Type BrowserCollectionTypeToGADType( ContentBrowser::EBrowserCollectionType InType )
	{
		EGADCollection::Type OutType = EGADCollection::Shared;
		switch( InType )
		{
		case ContentBrowser::EBrowserCollectionType::Shared:
			OutType = EGADCollection::Shared;
			break;
		case ContentBrowser::EBrowserCollectionType::Private:
			OutType = EGADCollection::Private;
			break;
		case ContentBrowser::EBrowserCollectionType::Local:
			OutType = EGADCollection::Local;
			break;
		default:
			// Invalid value
			check(FALSE);
			break;
		}

		return OutType;
	}

	/**
	 * Add a new collection.
	 * 
	 * @param InCollectionName The name of collection to add
	 * @param InType The type of collection specified by InCollectionName
	 *
	 * @return TRUE is succeeded, FALSE if failed.
	 */
	virtual bool CreateCollection( String^ InCollectionName, ContentBrowser::EBrowserCollectionType InType )
	{
		EGADCollection::Type GADCollectionType = BrowserCollectionTypeToGADType( InType );

		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return false;
		// oscar@snda

		FGameAssetDatabase& Gad = FGameAssetDatabase::Get();
		if ( Gad.CreateCollection( InCollectionName, GADCollectionType ) )
		{
			RequestCollectionListUpdate(false);
		}
		else
		{
			warnf( *Gad.GetErrorString() );
			String^ Warning = UnrealEd::Utils::Localize("DataBrowser_Warning_FailedCreateCollection");
			DataBrowserCtrl->PlayWarning( Warning );

			return false;
		}

		return true;
	}

	/**
	 * Destroy an existing collection
	 * 
	 * @param	InCollectionName	The name of collection to destroy
	 * @param	InType				The type of collection specified by InCollectionName
	 *
	 * @return TRUE is succeeded, FALSE if failed.
	 */
	virtual bool DestroyCollection( String^ InCollectionName, ContentBrowser::EBrowserCollectionType InType )
	{
		bool bResult = false;

		EGADCollection::Type GADCollectionType = BrowserCollectionTypeToGADType( InType );

		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return false;
		// oscar@snda

		FGameAssetDatabase& Gad = FGameAssetDatabase::Get();
		if ( Gad.DestroyCollection( InCollectionName, GADCollectionType ) )
		{
			RequestCollectionListUpdate(false);
			bResult = true;
		}
		else
		{
			warnf( *Gad.GetErrorString() );
			String^ Warning = UnrealEd::Utils::Localize("DataBrowser_Warning_FailedDestroyCollection");
			DataBrowserCtrl->PlayWarning( Warning );
		}

		return bResult;
	}


	/**
	 * Copies (or renames/moves) a collection
	 * 
	 * @param InCurrentCollectionName The collection to rename
	 * @param InCurrentType The type of collection specified by InCurrentCollectionName
	 * @param InNewCollectionName The new name of the collection
	 * @param InNewType The type of collection specified by InNewCollectionName
	 * @param bInMove True if the old collection should be destroyed after copying it
	 *
	 * @return TRUE is succeeded, FALSE if failed.
	 */
	virtual bool CopyCollection( String^ InCurrentCollectionName, ContentBrowser::EBrowserCollectionType InCurrentType, String^ InNewCollectionName, ContentBrowser::EBrowserCollectionType InNewType, bool bInMove )
	{
		EGADCollection::Type CurrentGADCollectionType = BrowserCollectionTypeToGADType( InCurrentType );
		EGADCollection::Type NewGADCollectionType = BrowserCollectionTypeToGADType( InNewType );

		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return false;
		// oscar@snda

		FGameAssetDatabase& Gad = FGameAssetDatabase::Get();
		if ( Gad.CopyCollection( InCurrentCollectionName, CurrentGADCollectionType, InNewCollectionName, NewGADCollectionType, bInMove ) )
		{
			RequestCollectionListUpdate(false);
		}
		else
		{
			warnf( *Gad.GetErrorString() );
			String^ Warning = UnrealEd::Utils::Localize("DataBrowser_Warning_FailedCopyOrRenameCollection");
			DataBrowserCtrl->PlayWarning( Warning );

			return false;
		}

		return true;
	}

	/**
	 * Add a list of assets to collection
	 * 
	 * @param InAssetFullNames	Full names of assets to be added
	 * @param InCollection		Collections to which we add
	 * @param InType			The type of collection specified by InCollectionName
	 *
	 * @return TRUE is succeeded, FALSE if failed.
	 */
	virtual bool AddAssetsToCollection( Generic::ICollection<String^>^ InAssetFullNames, ContentBrowser::Collection^ InCollection, ContentBrowser::EBrowserCollectionType InType )
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return false;
		// oscar@snda

		FGameAssetDatabase& Gad = FGameAssetDatabase::Get();
		bool bSuccess = false;
		
		EGADCollection::Type GADCollectionType = BrowserCollectionTypeToGADType( InType );

		if ( !Gad.IsReadOnly() )
		{			
			bSuccess = Gad.AddAssetsToCollection( InCollection->Name, GADCollectionType, InAssetFullNames );

			if ( bSuccess )
			{
				//@todo cb: could optimize this by passing CBR_UpdateCollectionListUI instead, if the collection isn't the current source
				// the NativeBrowserPtr could be used by FDataBrowser::Send to determine whether to check whether the collection is the source
				FCallbackEventParameters Parms(NativeBrowserPtr, CALLBACK_RefreshContentBrowser, CBR_UpdateCollectionList);
				GCallbackEvent->Send(Parms);
			}
			else
			{
				warnf( *Gad.GetErrorString() );
				String^ Warning = UnrealEd::Utils::Localize("DataBrowser_Warning_FailedAddToCollection");
				DataBrowserCtrl->PlayWarning( Warning );
			}
		}

		return bSuccess;
	}


	/**
	 * Remove assets from a collection
	 * 
	 * @param InAssetFullNames  Assets to be removed
	 * @param InCollection 		Collection from which to remove
	 * @param InType			The type of collection specified by InCollectionName
	 *
	 * @return TRUE is succeeded, FALSE if failed.
	 */
	virtual bool RemoveAssetsFromCollection( Generic::ICollection<String^>^ InAssetFullNames, ContentBrowser::Collection^ InCollection, ContentBrowser::EBrowserCollectionType InType )
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return false;
		// oscar@snda

		FGameAssetDatabase& Gad = FGameAssetDatabase::Get();

		EGADCollection::Type GADCollectionType = BrowserCollectionTypeToGADType( InType );

		bool bSuccess = false;
		if ( !Gad.IsReadOnly() )
		{			
			bSuccess = Gad.RemoveAssetsFromCollection( InCollection->Name, GADCollectionType, InAssetFullNames );

			if ( bSuccess )
			{
				//@todo cb: could optimize this by passing CBR_UpdateCollectionListUI instead, if the collection isn't the current source
				// the NativeBrowserPtr could be used by FDataBrowser::Send to determine whether to check whether the collection is the source
				FCallbackEventParameters Parms(NativeBrowserPtr, CALLBACK_RefreshContentBrowser, CBR_UpdateCollectionList);
				GCallbackEvent->Send(Parms);
			}
			else
			{
				warnf( *Gad.GetErrorString() );
				String^ Warning = UnrealEd::Utils::Localize("DataBrowser_Warning_FailedRemoveFromCollection");
				DataBrowserCtrl->PlayWarning( Warning );
			}
		}

		return bSuccess;
	}


	/**
	 * Creates a select collection request that is handled during the next content browser tick.
	 * 
	 * @param InCollectionName	Name of the collection to select
	 * @param InType			Type of collection to select
	 */
	void SelectCollection( String^ InCollectionName, ContentBrowser::EBrowserCollectionType InType )
	{
		// Create the collection requests list if it doesnt exist
		if( CollectionSelectRequests == nullptr )
		{
			CollectionSelectRequests = gcnew List<CollectionSelectRequest^>();
		}

		// Add a new  request
		CollectionSelectRequest^ NewRequest = gcnew CollectionSelectRequest();
		NewRequest->CollectionName = gcnew String(InCollectionName);
		NewRequest->CollectionType = InType;
		CollectionSelectRequests->Add( NewRequest );
		// We need to sync to collections
		bCollectionSyncRequested = true;
	}

	/**
	 * Mark assets as quarantined.
	 * 
	 * @param AssetsToQuarantine Assets that will be put under quarantine.
	 * 
	 * @return TRUE if succeeded, FALSE if failed.
	 */
	virtual bool QuarantineAssets( Generic::ICollection<ContentBrowser::DataAssetItem^>^ AssetsToQuarantine )
	{
		return AddTagToAssets( AssetsToQuarantine, FGameAssetDatabase::MakeSystemTag( ESystemTagType::Quarantined, "" ) );
	}

	/**
	 * List the quarantine on assets.
	 * 
	 * @param AssetsToRelease Lift quarantine on these assets.
	 * 
	 * @return TRUE if succeeded, FALSE if failed.
	 */
	virtual bool LiftQuarantine( Generic::ICollection<ContentBrowser::DataAssetItem^>^ AssetsToRelease )
	{
		return RemoveTagFromAssets( AssetsToRelease, FGameAssetDatabase::MakeSystemTag( ESystemTagType::Quarantined, "" ) );
	}

	/**
	 * Wrapper method for building a string containing a list of class name + path names delimited by tokens.
	 *
	 * @param	out_ResultString	receives the value of the tokenized string containing references to the selected assets.
	 *
	 * @return	the number of currently selected assets
	 */
	INT GenerateSelectedAssetString( FString* out_ResultString );

	/**
	* Marshal a collection of asset items into a string (only marshals the Type and PathName)
	* 
	* @param InAssetItems A collection of AssetItems to marshal
	* 
	* @return A string representing the AssetItems
	*/
	virtual String^ MarshalAssetItems( Generic::ICollection<ContentBrowser::DataAssetItem^>^ InAssetItems );

	/**
	* Extract asset full names from a marshaled string.
	* 
	* @param InAssetItems A collection of AssetItems to marshal
	* 
	* @return A list of asset full names
	*/
	virtual List<String^>^ UnmarshalAssetFullNames( String^ MarshaledAssets );
	
	/**
	* Update the list of sources (packages, collections, smart collection, etc)
	* 
	* @param UpdateSCC Whether an SCC state update should be included in this refresh
	*/
	virtual void UpdateSourcesList( bool ShouldUpdateSCC )
	{
		DWORD RefreshFlags = CBR_UpdatePackageList|CBR_UpdateAssetList|CBR_UpdateCollectionList;
		if (ShouldUpdateSCC)
		{
			RefreshFlags |= CBR_UpdateSCCState;
		}

		FCallbackEventParameters Parms( NativeBrowserPtr, CALLBACK_RefreshContentBrowser, RefreshFlags );
		GCallbackEvent->Send(Parms);
	}

	/**
	 * Expand package directories mentioned in "[Core.System] Paths"
	 */
	virtual void ExpandDefaultPackages()
	{
		TArray<FString> Paths;
		if ( GConfig->GetArray( TEXT("Core.System"), TEXT("Paths"), Paths, GEngineIni ) > 0 )
		{
			for ( INT PathIdx = 0; PathIdx < Paths.Num(); PathIdx++ )
			{
				ContentBrowser::PackagePanelModel^ Source = DataBrowserCtrl->MySources;
				ContentBrowser::SourceTreeNode^ TreeNode = Source->FindFolder( CLRTools::ToString(Paths(PathIdx)) );
				if (TreeNode != nullptr)
				{
					TreeNode->ExpandToRoot();
				}
			}
		}
	}

	/**
	 * Check whether the game asset database is in read-only mode.
	 * 
	 * @return True if the GAD is read-only
	 */
	virtual bool IsGameAssetDatabaseReadonly()
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return false;
		// oscar@snda

		return FGameAssetDatabase::Get().IsReadOnly();
	}


	/**
	 * Determine if the object type needs to be cleared if there exists 
	 * one sync object that does not pass the object type filter test. 
	 *
	 * @param	InObjects	A list of objects to test against the existing object type filter.
	 * @return	bool		true if all the given objects pass the object type filter test. 
	 */
	bool DoObjectsPassObjectTypeTest( const TArray<UObject*>& InObjects )
	{
		bool bPassesTest = true;

		for( INT ObjectIndex = 0; ObjectIndex < InObjects.Num(); ++ObjectIndex )
		{
			UObject* CurrentObject = InObjects(ObjectIndex);

			// The object type filter test only needs two values: the asset type and whether or not the asset is an archetype.
			String^ AssetTypeName = CLRTools::ToString( CurrentObject->GetClass()->GetName() );
			String^ AssetName = CLRTools::ToString( CurrentObject->GetName() );
			bool IsArchetype = ( CurrentObject->HasAllFlags( RF_ArchetypeObject ) && !CurrentObject->IsAPrefabArchetype() );

			if( !DataBrowserCtrl->AssetView->PassesObjectTypeFilter( AssetTypeName, IsArchetype ) ||
				!DataBrowserCtrl->AssetView->PassesShowFlattenedTextureFilter(AssetTypeName, AssetName))
			{
				bPassesTest = false;
				break;
			}
		}

		return bPassesTest;
	}


	/**
	 * Check whether this asset belongs to any of the browsable types.
	 * 
	 * @param ItemToCheck			Check this item's type
	 * @param BrowsableTypeNames	List of BrowsableTypeNames that this asset could be a type of
	 */
	virtual bool IsAssetAnyOfBrowsableTypes( ContentBrowser::DataAssetItem^ ItemToCheck, Generic::ICollection<String^>^ BrowsableTypeNames )
	{
		return IsAssetAnyOfBrowsableTypes( ItemToCheck->AssetType, ItemToCheck->IsArchetype, BrowsableTypeNames );
	}


	/**
	 * Check whether this asset type belongs to any of the browsable types.
	 * 
	 * @param AssetType				The asset type to check.
	 * @param bIsArchetype			Is the asset an archetype. 
	 * @param BrowsableTypeNames	List of BrowsableTypeNames that this asset could be a type of
	 */
	virtual bool IsAssetAnyOfBrowsableTypes( String^ AssetType, bool IsArchetype, Generic::ICollection<String^>^ BrowsableTypeNames )
	{
		// First grab the list of browsable types for this asset's UObject class
		List< String^ >^ TypesOnAsset;
		ClassNameToBrowsableTypeNameMap->TryGetValue( AssetType, TypesOnAsset );

		// If the object is an archetype then make sure that's in it's list of browsable types that
		// we'll compare with the UI's list of selected browsable types
		if( IsArchetype )
		{
			if( TypesOnAsset == nullptr )
			{
				TypesOnAsset = gcnew List< String^ >();
			}
			TypesOnAsset->Add( "Archetypes" );
		}


		if ( TypesOnAsset == nullptr )
		{
			return false;
		}

		return ContentBrowser::DataTagUtils::AnyElementsInCommon( BrowsableTypeNames, TypesOnAsset );		
	}
	
	
	/**
	 * Makes sure the currently selected objects are loaded into memory
	 */
	virtual void LoadSelectedObjectsIfNeeded();

	/** Syncs the currently selected objects in the asset view with the engine's global selection set */
	virtual void SyncSelectedObjectsWithGlobalSelectionSet();

	/**
	 * Called after the editor's selection set has been updated
	 *
	 * @param	ModifiedSelection	the selection that was changed
	 */
	void NotifySelectionChanged( USelection* ModifiedSelection );


	/**
	 * Add tag to the assets. If a tag is on any of the assets already it will not be added again.
	 * 
	 * @param InAssets   AssetItems which need to be tagged
	 * @param TagToAdd   The tag to be added to the AssetItems
	 *
	 * @return TRUE is succeeded, FALSE if failed.
	 */
	virtual bool AddTagToAssets( Generic::ICollection<ContentBrowser::DataAssetItem^>^ InAssets, String^ TagToAdd )
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return false;
		// oscar@snda

		FGameAssetDatabase& MyGad = FGameAssetDatabase::Get();

		bool bSuccess = FALSE;
		if ( !MyGad.IsReadOnly() )
		{			
			Generic::List<String^>^ AssetsToTag = gcnew Generic::List<String^>();

			// Build a list of assets that actually need this tag
			for each ( ContentBrowser::DataAssetItem^ Asset in InAssets )
			{
				String^ AssetFullName = Asset->FullName;
				List<String^>^ TagsOnAsset;
				MyGad.QueryTagsForAsset(
					AssetFullName,
					ETagQueryOptions::AllTags,
					TagsOnAsset );
				
				if ( !TagsOnAsset->Contains(TagToAdd) )
				{
					AssetsToTag->Add( AssetFullName );
				}
			}

			bSuccess = MyGad.AddTagToAssets( AssetsToTag, TagToAdd );
		}

		if ( !bSuccess )
		{
			warnf( *MyGad.GetErrorString() );
			DataBrowserCtrl->PlayWarning(UnrealEd::Utils::Localize("DataBrowser_Warning_FailedTagAsset"));
		}
		
		return bSuccess;
	}



	/**
	 * Remove a tag from a list of assets. Do nothing to assets that already lacks this tag.
	 * 
	 * @param InAssets    Assets to untag.
	 * @param TagToRemove Tags to remove.
	 */
	virtual bool RemoveTagFromAssets( Generic::ICollection<ContentBrowser::DataAssetItem^>^ InAssets, String^ TagToRemove )
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return false;
		// oscar@snda

		FGameAssetDatabase& MyGad = FGameAssetDatabase::Get();
		bool bSuccess = FALSE;
		if ( !MyGad.IsReadOnly() )
		{
			Generic::List<String^>^ AssetsToUntag = gcnew Generic::List<String^>();

			for each ( ContentBrowser::DataAssetItem^ Asset in InAssets )
			{
				String^ AssetFullName = Asset->FullName;
				List<String^>^ TagsOnAsset;
				MyGad.QueryTagsForAsset(
					AssetFullName,
					ETagQueryOptions::AllTags,
					TagsOnAsset );

				if ( TagsOnAsset->Contains( TagToRemove ) )
				{
					AssetsToUntag->Add( AssetFullName );
				}
			}
			
			bSuccess = MyGad.RemoveTagFromAssets( AssetsToUntag, TagToRemove );
		}

		if ( !bSuccess )
		{
			DataBrowserCtrl->PlayWarning(UnrealEd::Utils::Localize("DataBrowser_Warning_FailedUntagAsset"));
			warnf( *MyGad.GetErrorString() );
		}

		return bSuccess;
	}


	/**
	 * Updates the status of the specified asset item
	 *
	 * @param	Asset		Asset to update
	 */
	virtual void UpdateAssetStatus( ContentBrowser::DataAssetItem^ Asset, ContentBrowser::AssetStatusUpdateFlags UpdateFlags )
	{
		if( ( UpdateFlags & ContentBrowser::AssetStatusUpdateFlags::LoadedStatus ) == ContentBrowser::AssetStatusUpdateFlags::LoadedStatus )
		{
			const UBOOL bWasLoaded = ( Asset->LoadedStatus != ContentBrowser::DataAssetItem::LoadedStatusType::NotLoaded );

			// Make sure the loaded status is up to date for this item
			UObject* FoundObject = NULL;
			UClass* AssetClass = FindObject<UClass>( ANY_PACKAGE, *CLRTools::ToFString( Asset->AssetType ) );
			if( AssetClass != NULL )
			{
				FoundObject = UObject::StaticFindObject( AssetClass, ANY_PACKAGE, *CLRTools::ToFString( Asset->FullyQualifiedPath ) );
			}

			// Update the loaded status of the object
			if( FoundObject != NULL )
			{
				if( ( (UPackage*)FoundObject->GetOutermost() )->IsDirty() )
				{
					Asset->LoadedStatus = ContentBrowser::DataAssetItem::LoadedStatusType::LoadedAndModified;
				}
				else
				{
					Asset->LoadedStatus = ContentBrowser::DataAssetItem::LoadedStatusType::Loaded;
				}
			}
			else
			{
				Asset->LoadedStatus = ContentBrowser::DataAssetItem::LoadedStatusType::NotLoaded;
			}

			// If the asset just became loaded, then also mark it's thumbnail as needing to be refreshed
			if( !bWasLoaded && ( FoundObject != NULL ) )
			{
				// Loaded assets are always verified (locally at least)
				if( FGameAssetDatabase::IsInitialized() && !Asset->IsVerified ) // oscar@snda
				{
					Asset->IsVerified = true;

					// Remove the 'unverified' tag from this asset (locally)
					FGameAssetDatabase::Get().RemoveTagMapping(
						Asset->FullName,
						FGameAssetDatabase::Get().MakeSystemTag( ESystemTagType::Unverified, "" ) );
				}

				if( Asset->Visual != nullptr && Asset->Visual->Thumbnail != nullptr )
				{
					Asset->Visual->ShouldRefreshThumbnail = ContentBrowser::ThumbnailGenerationPolicy::OnlyIfNotInFreeList;
				}

				// Also update it's custom label data
				{
					Asset->MarkCustomLabelsAsDirty();
					Asset->MarkCustomDataColumnsAsDirty();

					// The user might be looking at the asset right now, so we'll force a refresh
					Asset->UpdateCustomLabelsIfNeeded();
					Asset->UpdateCustomDataColumnsIfNeeded();
				}
			}
		}
		
		// oscar@snda
		if( FGameAssetDatabase::IsInitialized() && ( UpdateFlags & ContentBrowser::AssetStatusUpdateFlags::Tags ) == ContentBrowser::AssetStatusUpdateFlags::Tags )
		{
			// Update the tags assigned to this object
			List<String^>^ MyObjectTags;
			FGameAssetDatabase::Get().QueryTagsForAsset(
				Asset->FullName,
				CBDefs::ShowSystemTags ? ETagQueryOptions::AllTags : ETagQueryOptions::UserTagsOnly,
				MyObjectTags );

			// Sort the tag list alphabetically
			MyObjectTags->Sort();

			Asset->Tags = MyObjectTags; 
		}

		// oscar@snda
		if(FGameAssetDatabase::IsInitialized() && ( UpdateFlags & ContentBrowser::AssetStatusUpdateFlags::Quarantined ) == ContentBrowser::AssetStatusUpdateFlags::Quarantined )
		{
			// Update the tags assigned to this object
			List<String^>^ MyObjectTags;
			FGameAssetDatabase::Get().QueryTagsForAsset(
				Asset->FullName,
				ETagQueryOptions::AllTags,
				MyObjectTags );
			Asset->IsQuarantined = MyObjectTags->Contains( FGameAssetDatabase::MakeSystemTag(ESystemTagType::Quarantined, "") );			
		}

	}

	/**
	 * Populate a context menu with a menu item for each type of object that can be created using a class factory.
	 *
	 * @param	ctxMenu		the context menu to populate items for
	 */
	virtual void PopulateObjectFactoryContextMenu( ContextMenu^ ctxMenu );

	/** Populates an item collection with package command menu items for use in a ContextMenu or MenuItem.  
	 *  Note: This function also populates package menu with dynamic menu items which do not use the passed in event handlers.  
	 *  Instead they use ExecuteCustomObjectCommand and CanExecuteCustomObjectCommand.
	 *  
	 * @param OutPackageListMenuItems		The context menu where menu items should be added.
	 * @param OutCommandBindings			The command binding collection where new command bindings should be added.
	 * @param InTypeId						The typeid of the ui element we are registering the commands with
	 * @param InEventHandler				Optional new event handler that should be called on these menu items.
	 * @param InCanExecuteEventHandler		Optional new event handler that should be called when determining if these menu items can be used.
	 */
	virtual void PopulatePackageListMenuItems( ItemCollection^ OutPackageListMenuItems, Input::CommandBindingCollection^ OutCommandBindings, Type^ InTypeId, Input::ExecutedRoutedEventHandler^ InEventHandler, Input::CanExecuteRoutedEventHandler^ InCanExecuteEventHandler  );

	/**
	 * Handler for the Close event of the asset view's context menu.  Removes any command bindings that were added for object factories.
	 */
	void OnObjectFactoryContextMenuClosed( Object^ Sender, RoutedEventArgs^ CloseEventArgs )
	{
		for ( int BindingIndex = DataBrowserCtrl->AssetView->CommandBindings->Count - 1; BindingIndex >= 0; BindingIndex-- )
		{
			Input::CommandBinding^ Binding = DataBrowserCtrl->AssetView->CommandBindings[BindingIndex];
			Input::RoutedCommand^ Command = dynamic_cast<Input::RoutedCommand^>(Binding->Command);
			if ( Command != nullptr )
			{
				DataBrowserCtrl->AssetView->CommandBindings->RemoveAt(BindingIndex);
			}
		}
	}

	/** Opens type-sensitive editor for the selected objects */
	void OpenObjectEditorFor( TArray<UObject*>& InObjects )
	{
		for( TArray<UObject*>::TIterator ObjectIt(InObjects); ObjectIt; ++ObjectIt )
		{
			UObject* CurObject = *ObjectIt;
			if( CurObject != NULL )
			{
				// NOTE: This list is ordered such that child object types appear first
				const TArray< UGenericBrowserType* >* BrowsableObjectTypes =
					ClassToBrowsableObjectTypeMap->Find( CurObject->GetClass() );
				if( BrowsableObjectTypes != NULL )
				{
					for( INT CurBrowsableTypeIndex = 0; CurBrowsableTypeIndex < BrowsableObjectTypes->Num(); ++CurBrowsableTypeIndex )
					{
						UGenericBrowserType* CurObjType = ( *BrowsableObjectTypes )( CurBrowsableTypeIndex );
						if( CurObjType->Supports( CurObject ) )
						{
							// Opens an editor for all selected objects of the specified type
							// @todo CB: Warn if opening editors for more than 5 objects or so (See GB's check for this)
							UBOOL bShowedAtLeastOneEditor = CurObjType->ShowObjectEditor( CurObject );
							if( bShowedAtLeastOneEditor )
							{
								break;
							}
						}
					}
				}
			}
		}
	}

	/**
	 * Returns menu item descriptions, to be displayed in the asset view's right click context menu
	 *
	 * @param	OutMenuItems	(Out) List of WPF menu items
	 */
	virtual void QueryAssetViewContextMenuItems( List< Object^ >^% OutMenuItems );

	/**
	* Returns menu item descriptions that are created on the fly, to be displayed in the package tree's right click context menu
	*
	* @param	OutMenuItems	(Out) List of WPF menu items
	*/
	void QueryPackageTreeContextMenuItems( TArray<FObjectSupportedCommandType>& OutSupportedCommands );

	/**
	* Returns an array of commands supported by the current selection.
	*
	* @param	OutSupportedCommands		(Out) Array of commands supported by the current selection
	* @param	DefaultCommandID			(Out) CommandID to execute when the user does not specify an explicit command (e.g. on doubleclick)
	*/
	void QuerySupportedCommands( TArray<FObjectSupportedCommandType>& OutSupportedCommands, INT& DefaultCommandID );

	/**
	 * Given a list of commands, build a list of context menu items to represent it.
	 * 
	 * @param	InCommands		List of commands from which to build a menu
	 * @param	InDefaultCommandID		This command should appear in bold
	 * @param	InTypeID				The typeid to pass to RoutedUICommand.  This is needed since different ui elements use this function and we want to make sure commands are registered properly.
	 * @param	InOutCommandBindingList	The command binding list where newly created commands should be added.
	 * @param	OutMenuItems			(Out) List of WPF menu items
	 */
	void BuildContextMenu( const TArray<FObjectSupportedCommandType>& InCommands, INT InDefaultCommandId,  Type^ InTypeID,  Input::CommandBindingCollection^ InOutCommandBindingList, List< Object^ >^% OutMenuItems);


	/**
	 * Queries the list of collection names the specified asset is in (sorted alphabetically).  Note that
	 * for private collections, only collections created by the current user will be returned.
	 *
	 * @param	InFullName			The full name for the asset
	 * @param	InType				Type of collection to search for
	 *
	 * @return	List of collection names, sorted alphabetically
	 */
	virtual List< String^ >^ QueryCollectionsForAsset( String^ InFullName, ContentBrowser::EBrowserCollectionType InType );


	/** Called by the asset view when the selection has changed */
	virtual void OnAssetSelectionChanged( Object^ Sender, ContentBrowser::DataAssetView::AssetSelectionChangedEventArgs^ Args )
    {
		// @todo CB [reviewed; discuss]: Contention between multiple Content Browser windows.  They'll all fight over this global selection set.

		// Update the engine's set of selected objects.
		SyncSelectedObjectsWithGlobalSelectionSet();
	}


	/** Called when the user opts to "activate" an asset item (usually by double clicking on it) */
	virtual void LoadAndActivateSelectedAssets();

	/**
	 *  Called when the user opts to view the properties of asset items via hot-key; only displays properties if all of
	 *  the selected assets do not have a specific editor for setting their attributes
	 */
	virtual void LoadAndDisplayPropertiesForSelectedAssets();

	/**
	 * Called when the user activates the preview action on an asset (not all assets support preview; currently used for sounds)
	 * 
	 * @param ObjectFullName		Full name of asset to preview
	 * 
	 * @return true if the preview process started; false if the action actually stopped an ongoing preview process.
	 */
	virtual bool PreviewLoadedAsset( String^ ObjectFullName );


	/**
	 * Fill out the list of ObjectTypes in the MetadataFilter Panel.
	 * These ObjectTypes are "fake" object types made for artists and LDs.
	 * e.g. Texture -> Texture2d, TextureCube, TextureRenderTarget, etc
	 */
	virtual List<String^>^ GetObjectTypeFilterList()
	{
		List<String^>^ ObjectTypeList = gcnew List<String^>();
		for (int ObjTypeIdx=0; ObjTypeIdx<BrowsableObjectTypeList->Num(); ObjTypeIdx++)
		{
			ObjectTypeList->Add( CLRTools::ToString( (*BrowsableObjectTypeList.Get())(ObjTypeIdx)->GetBrowserTypeDescription() ) );
		}

		return ObjectTypeList;
	}

    /**
     * Return a list of Browsable type names from a class name
     */
    virtual List<String^>^ GetBrowsableTypeNameList(List<String^>^ ClassNameList)
    {
        List<String^>^ BrowsableTypeNameList = gcnew List<String^>();
        List<String^>^ TempList;
        int ClassNameCount = ClassNameList->Count;
        int TempListCount;
        // For each provided class name
        for (int ClassIdx=0; ClassIdx < ClassNameCount; ClassIdx++)
        {
            // Get the Browsable types
            TempList = ClassNameToBrowsableTypeNameMap[ClassNameList[ClassIdx]];
            TempListCount = TempList->Count;

            // And add the Browsable types to the running list if not already in there
            for (int TypeNameIdx=0; TypeNameIdx < TempListCount; TypeNameIdx++)
            {
                if( !BrowsableTypeNameList->Contains(TempList[TypeNameIdx]) )
                {
                    BrowsableTypeNameList->Add(TempList[TypeNameIdx]);
                }
            }
        }
        return BrowsableTypeNameList;
    }


	/**
	 * Save the content browser UI state to config
	 * 
	 * @param SaveMe DataBrowserUIState to save
	 */
	virtual void SaveContentBrowserUIState(ContentBrowser::DataContentBrowserUIState^ SaveMe)
	{
		System::Type^ DataBrowserUIStateType = ContentBrowser::DataContentBrowserUIState::typeid;
		array<System::Reflection::PropertyInfo^>^ DataBrowserUIStateFieldInfo = DataBrowserUIStateType->GetProperties(	System::Reflection::BindingFlags::Public |
																									System::Reflection::BindingFlags::Instance);

		FString Section(TEXT("DataBrowserUIState"));

		for (int FieldIdx=0; FieldIdx<DataBrowserUIStateFieldInfo->Length; ++FieldIdx)
		{
			FString FieldName( CLRTools::ToFString(DataBrowserUIStateFieldInfo[FieldIdx]->Name) );
			if (DataBrowserUIStateFieldInfo[FieldIdx]->PropertyType == bool::typeid)
			{
				GConfig->SetBool(	*Section,
									*FieldName,
									(bool)DataBrowserUIStateFieldInfo[FieldIdx]->GetValue(SaveMe, nullptr),
									GEditorUserSettingsIni);
			}
			else if (DataBrowserUIStateFieldInfo[FieldIdx]->PropertyType == double::typeid)
			{
				GConfig->SetDouble(	*Section,
									*FieldName,
									(double)DataBrowserUIStateFieldInfo[FieldIdx]->GetValue(SaveMe, nullptr),
									GEditorUserSettingsIni);
			}
			else if (DataBrowserUIStateFieldInfo[FieldIdx]->PropertyType == System::String::typeid)
			{
				System::String^ StringToSave = (System::String^)DataBrowserUIStateFieldInfo[FieldIdx]->GetValue(SaveMe, nullptr);
				GConfig->SetString(	*Section,
									*FieldName,
									*CLRTools::ToFString(StringToSave),
									GEditorUserSettingsIni);
			}
			else
			{
				appErrorf( TEXT("DataBrowserUIState.%s could not be saved because its type is unsupported."), *FieldName);
			}
		}
	}

	/**
	 * Load the content browser UI state from config
	 * 
	 * @param LoadMe DataBrowserUIState to populate
	 */
	virtual void LoadContentBrowserUIState(ContentBrowser::DataContentBrowserUIState^ LoadMe)
	{

		LoadMe->SetToDefault();

		System::Type^ DataBrowserUIStateType = ContentBrowser::DataContentBrowserUIState::typeid;
		array<System::Reflection::PropertyInfo^>^ DataBrowserUIStateFieldInfo = DataBrowserUIStateType->GetProperties(	System::Reflection::BindingFlags::Public |
																										System::Reflection::BindingFlags::Instance);

		FString Section(TEXT("DataBrowserUIState"));

		for (int FieldIdx=0; FieldIdx<DataBrowserUIStateFieldInfo->Length; ++FieldIdx)
		{
			FString FieldName( CLRTools::ToFString(DataBrowserUIStateFieldInfo[FieldIdx]->Name) );
			if (DataBrowserUIStateFieldInfo[FieldIdx]->PropertyType == bool::typeid)
			{
				UBOOL TmpValue;
				UBOOL ConfigValueFound = GConfig->GetBool(	*Section,
															*FieldName,
															TmpValue,
															GEditorUserSettingsIni);
				if (ConfigValueFound)
				{
					DataBrowserUIStateFieldInfo[FieldIdx]->SetValue(LoadMe, TmpValue==TRUE, nullptr);
				}
			}
			else if (DataBrowserUIStateFieldInfo[FieldIdx]->PropertyType == double::typeid)
			{
				DOUBLE TmpValue;
				UBOOL ConfigValueFound = GConfig->GetDouble(*Section,
															*FieldName,
															TmpValue,
															GEditorUserSettingsIni);
				if (ConfigValueFound)
				{
					TmpValue = Max(TmpValue, 0.0);
					DataBrowserUIStateFieldInfo[FieldIdx]->SetValue(LoadMe, TmpValue, nullptr);
				}
			}
			else if (DataBrowserUIStateFieldInfo[FieldIdx]->PropertyType == System::String::typeid)
			{
				FString TmpValue;				
				UBOOL ConfigValueFound = GConfig->GetString(*Section,
															*FieldName,
															TmpValue,
															GEditorUserSettingsIni);
				if (ConfigValueFound)
				{
					DataBrowserUIStateFieldInfo[FieldIdx]->SetValue(LoadMe, CLRTools::ToString(TmpValue), nullptr);
				}
			}
		}
	}


	/** Called when the "Close" button is clicked on the main tool bar */
	void OnCloseButtonClicked( Object^ Sender, RoutedEventArgs^ CloseEventArgs )
	{
		if( ParentBrowserWindow != NULL )
		{
			// If the window is floating then we'll always allow the user to close it.  If it's
			// docked we'll make sure that it's not the first browser of it's type before we allow that
			if( ParentBrowserWindow->IsFloating() ||
				!GUnrealEd->GetDataBrowserManager()->IsCanonicalBrowser( ParentBrowserWindow->GetDockID() ) )
			{
				// In case removing this browser will not focus another content browser activate some content browser;
				// it's in the user's expectation that there is at least one content browser out there providing a selection.
				this->NativeBrowserPtr->MakeAppropriateDataBrowserActive();

				// Remove the window
				WxDockEvent NewEvent( ParentBrowserWindow->GetDockID(), DCT_DataRemove );
				NewEvent.SetEventType( wxEVT_DOCKINGCHANGE );
				::wxPostEvent( GApp->EditorFrame, NewEvent );
			}
		}
	}



	/** Called when the "Clone" button is clicked on the main tool bar */
	void OnCloneButtonClicked( Object^ Sender, RoutedEventArgs^ CloseEventArgs )
	{
		// Make sure the Browser Manager frame is visible, since the new window will appear there
		GUnrealEd->GetDataBrowserManager()->ShowDockingContainer();

		if( ParentBrowserWindow != NULL )
		{
			// Create a new window
			WxDockEvent NewEvent( ParentBrowserWindow->GetDockID(), DCT_DataClone );
			NewEvent.SetEventType( wxEVT_DOCKINGCHANGE );
			::wxPostEvent( GApp->EditorFrame, NewEvent );
		}
	}



	/** Called when the float/dock button is clicked on the main tool bar */
	void OnFloatOrDockButtonClicked( Object^ Sender, RoutedEventArgs^ CloseEventArgs )
	{
		if( ParentBrowserWindow != NULL )
		{
			// Toggle docked state
			EDockingChangeType NewDockMode = ParentBrowserWindow->IsDocked() ? DCT_DataFloating : DCT_DataDocking;
			WxDockEvent evt( ParentBrowserWindow->GetDockID(), NewDockMode );
			evt.SetEventType( wxEVT_DOCKINGCHANGE );
			::wxPostEvent( GApp->EditorFrame, evt );
		}
	}

	/** 
	* Captures keyboard input
	*
	* @param Sender			Object sending the event
	* @param EventArgs		Key event arguments
	*/
	void OnKeyPressed( Object^ Sender, KeyEventArgs^ EventArgs )
	{
		UBOOL bCtrlPressed = (	(Keyboard::IsKeyDown(Key::RightCtrl)) ||	
								(Keyboard::IsKeyDown(Key::LeftCtrl)) );

		UBOOL bShiftPressed = ( (Keyboard::IsKeyDown(Key::RightShift)) ||	
								(Keyboard::IsKeyDown(Key::LeftShift)) );

		UBOOL bAltPressed = (	(Keyboard::IsKeyDown(Key::RightAlt)) ||	
								(Keyboard::IsKeyDown(Key::LeftAlt)) );

		UBOOL bEnterPressed = (	(Keyboard::IsKeyDown(Key::Return)) ||	
								(Keyboard::IsKeyDown(Key::Enter)) );

		FString KeyString = CLRTools::ToFString(EventArgs->Key.ToString());
		// Alt + Key Events are handled as system events.  The KeyName is "System"
		FName KeyName = bEnterPressed ? FName(TEXT("Enter")) : FName( *KeyString );
		GApp->CheckIfGlobalHotkey( KeyName, bCtrlPressed, bShiftPressed, bAltPressed);
	}

	/**
	 * Called when a mouse button is pressed when the content browser has focus
	 */
	void OnMouseButtonPressed( Object ^Sender, MouseButtonEventArgs^ EventArgs )
	{
		switch( EventArgs->ChangedButton )
		{
		case MouseButton::XButton1:
			// XButton1 is the browser "Back" button on a mouse
			DataBrowserCtrl->HistoryGoBack();
			break;
		case MouseButton::XButton2:
			// XButton1 is the browser "Forward" button on a mouse
			DataBrowserCtrl->HistoryGoForward();
			break;
		default:
			break;

		}
	}

	/**
	 * Updates a writeable WPF bitmap with the specified thumbnail's data
	 * 
	 * @param	InThumbnail		The thumbnail object to create a bitmap for
	 */
	void CopyThumbnailToWriteableBitmap( const FObjectThumbnail& InThumbnail, Imaging::WriteableBitmap^ WriteableBitmap );


	/**
	 * Generates a writeable WPF bitmap for the specified thumbnail
	 * 
	 * @param	InThumbnail		The thumbnail object to create a bitmap for
	 * 
	 * @return	Returns the BitmapSource for the thumbnail, or null on failure
	 */
	Imaging::WriteableBitmap^ CreateWriteableBitmapForThumbnail( const FObjectThumbnail& InThumbnail );



	/** Returns true if the specified asset uses a stock thumbnail resource */
	virtual bool AssetUsesSharedThumbnail( ContentBrowser::DataAssetItem^ Asset );

	/**
	 * Capture active viewport to thumbnail and assigns that thumbnail to incoming assets
	 *
	 * @param InAssetsToAssign - assets that should receive the new thumbnail ONLY if they are assets that use SharedThumbnails
	 */
	void CaptureThumbnailFromViewport (FViewport* InViewport, List<ContentBrowser::DataAssetItem^>^ InAssetsToAssign);
	/**
	 * Clears custom thumbnails for the selected assets
	 *
	 * @param InAssetsToAssign - assets that should have their thumbnail cleared
	 */
	void ClearCustomThumbnails (List<ContentBrowser::DataAssetItem^>^ InAssetsToAssign);

    /**
     * Filter the selected Object Types
     *
     * @param SelectedItems - List of selected items 
     */
    void FilterSelectedObjectTypes ( List<ContentBrowser::DataAssetItem^>^ SelectedItems );


	/**
	 * Attempts to generate a thumbnail for the specified object
	 * 
	 * @param	Asset	the asset that needs a thumbnail
	 * @param	CheckSharedThumbnailAssets	True if we should even check to see if assets that use a stock thumbnail resource exist in the package file
	 * @param	OutFailedToLoadThumbnail	True if we tried to load the asset's thumbnail from disk and couldn't find it.
	 * 
	 * @return	Returns the BitmapSource for the generated thumbnail, or null if a thumbnail is not available
	 */
	virtual Imaging::BitmapSource^ GenerateThumbnailForAsset( ContentBrowser::DataAssetItem^ Asset, bool% OutFailedToLoadThumbnail );


	/**
	 * Attempts to generate a *preview* thumbnail for the specified object
	 * 
	 * @param	ObjectFullName Full name of the object
	 * @param	PreferredSize The preferred resolution of the thumbnail, or 0 for "lowest possible"
	 * @param	IsAnimating True if the thumbnail will be updated frequently
	 * @param	ExistingThumbnail The current preview thumbnail for the asset, if any
	 * 
	 * @return	Returns the BitmapSource for the generated thumbnail, or null if a thumbnail is not available
	 */
	virtual Imaging::BitmapSource^ GeneratePreviewThumbnailForAsset( String^ ObjectFullName, int PreferredSize, bool IsAnimating, Imaging::BitmapSource^ ExistingThumbnail );

	/**
	 * Removes the uncompressed image data held by the UPackage for the specified asset.
	 *
	 * @param	AssetFullName	the Unreal full name for the object to remove the thumbnail data for
	 */
	virtual void ClearCachedThumbnailForAsset( String^ AssetFullName );

	
	/** Locally tags the specified asset as a "ghost" so that it will no longer appear in the editor */
	virtual void LocallyTagAssetAsGhost( ContentBrowser::DataAssetItem^ Asset )
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return;
		// oscar@snda

		String^ CurrentDateTimeString = DateTime::Now.Ticks.ToString();
		const bool bIsAuthoritative = true;

		FGameAssetDatabase::Get().AddTagMapping(
			Asset->FullName,
			FGameAssetDatabase::MakeSystemTag( ESystemTagType::Ghost, CurrentDateTimeString ),
			bIsAuthoritative );
	}


	/** Locally removes the "unverified" tag from the specified asset */
	virtual void LocallyRemoveUnverifiedTagFromAsset( ContentBrowser::DataAssetItem^ Asset )
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return;
		// oscar@snda

		FGameAssetDatabase::Get().RemoveTagMapping(
			Asset->FullName,
			FGameAssetDatabase::MakeSystemTag( ESystemTagType::Unverified, "" ) );
	}


	/**
	 * Applies the currently selected assets to the objects selected in the level viewport.  For example,
	 * if a material is selected in the content browser and a surface is selected in the 3D view, this
	 * could assign the material to that surface
	 */
	virtual void ApplyAssetSelectionToViewport();

	/**
	 * Get a list of all known asset type names as strings.
	 * The Archetype classes are ignored here because just about any class can be an archetype.
	 * Note, these are the Unreal class names and NOT generic browser type names.
	 */
	virtual UnrealEd::NameSet^ GetAssetTypeNames()
	{
		return AssetTypeNames;
	}

	/**
	* Get a list of the Favorites for the TypeFilter from [Game]EditorUserSettings.
	* Note that any invalid Favorites are removed. Invalid favorites are
	* ones that are not GenericBrowserTypes.
	* 
	* @return A list of the favorite type names .
	*/
	virtual List<String^>^ LoadTypeFilterFavorites()
	{
		FString FavoriteTypesFString;
		UBOOL bConfigValueFound = GConfig->GetString(	TEXT("DataBrowserFilter"),
														TEXT("FavoriteTypes_0"),
														FavoriteTypesFString,
														GEditorUserSettingsIni);
		if (bConfigValueFound)
		{
			String^ FavoriteTypesString = CLRTools::ToString( FavoriteTypesFString );
			array<TCHAR>^ Delimiter = gcnew array<TCHAR>{ ';' };
			array<String^>^ FavoriteTypes = FavoriteTypesString->Split( Delimiter, StringSplitOptions::RemoveEmptyEntries );
			List<String^>^ ObjectTypes = this->GetObjectTypeFilterList();

			List<String^>^ Favorites = gcnew List<String^>();
			// Validate the list of favorites.
			for each( String^ Favorite in FavoriteTypes )
			{
				if ( ObjectTypes->Contains( Favorite ) )
				{
					Favorites->Add( Favorite );
				}
			}
			return Favorites;
		}
		else
		{
			return gcnew List<String^>(0);
		}
	}

	/**
	* Save the list of the Favorites for the TypeFilter into [Game]EditorUserSettings.
	* 
	* @param InFavorites A list of Favorite Type names.
	*/
	virtual void SaveTypeFilterFavorites( List<String^>^ InFavorites )
	{
		String^ FavoritesString = String::Join( ";", InFavorites->ToArray() );
		FString FavoriteTypes = CLRTools::ToFString( FavoritesString );
		GConfig->SetString(	TEXT("DataBrowserFilter"),
							TEXT("FavoriteTypes_0"),
							*FavoriteTypes,
							GEditorUserSettingsIni );
	}

	/** Tags all in use objects based on content browser filter options */
	virtual void TagInUseObjects()
	{
		// This shouldnt even be called if the in use filter is off 
 		check( DataBrowserCtrl->Filter->InUseOff == false );
 
 		// Assume current level is selected by default
 		ObjectTools::EInUseSearchOption SearchOption = ObjectTools::EInUseSearchOption::SO_CurrentLevel;
 		
 		if( DataBrowserCtrl->Filter->ShowVisibleLevelsInUse )
 		{
 			SearchOption = ObjectTools::EInUseSearchOption::SO_VisibleLevels;
 		}
 		else if( DataBrowserCtrl->Filter->ShowLoadedLevelsInUse )
 		{
 			SearchOption = ObjectTools::EInUseSearchOption::SO_LoadedLevels;
 		}
 
 		ObjectTools::TagInUseObjects( SearchOption );
	}

	/** 
	 * Determines if an asset is in use by looking if RF_TagExp was set by TagInUseObjects.
	 *
	 * @param Asset	The asset to check
	 */
	virtual bool IsObjectInUse( ContentBrowser::DataAssetItem^ Asset )
	{
		// Objects are not in use by default
		bool bObjectInUse = false;
		UClass* AssetClass = FindObject<UClass>( ANY_PACKAGE, *CLRTools::ToFString( Asset->AssetType ) );
		if( AssetClass != NULL )
		{
			// Find the object.  If it cant be found it certainly isn't in use.
			UObject* Obj = UObject::StaticFindObject( AssetClass, ANY_PACKAGE, *CLRTools::ToFString( Asset->FullyQualifiedPath ) );
			if( Obj )
			{
				const UBOOL bUnreferenced = !Obj->HasAnyFlags( RF_TagExp );
				const UBOOL bIndirectlyReferencedObject = Obj->HasAnyFlags( RF_TagImp );
				const UBOOL bRejectObject =
					Obj->GetOuter() == NULL || // Skip objects with null outers
					Obj->HasAnyFlags( RF_Transient ) || // Skip transient objects (these shouldn't show up in the CB anyway)
					Obj->IsPendingKill() || // Objects that will be garbage collected 
					bUnreferenced || // Unreferenced objects 
					bIndirectlyReferencedObject; // Indirectly referenced objects

				if( !bRejectObject && Obj->HasAnyFlags( RF_Public ) )
				{
					// The object is in use 
					bObjectInUse = true;
				}
			}
		}

		return bObjectInUse;
	}
	/**
	 * Is the user allowed to Create/Destroy tags (a.k.a TagAdmin).
	 * 
	 * @return	True if the user is TagAdmin, false otherwise.
	 */
	virtual bool IsUserTagAdmin()
	{
		UBOOL bIsUserTagAdmin = TRUE;
		UBOOL ConfigValueFound = GConfig->GetBool(	TEXT("DataBrowserSecurity"),
													TEXT("bIsUserTagAdmin"),
													bIsUserTagAdmin,
													GEditorUserSettingsIni);
		return bIsUserTagAdmin ? true : false;
	}

	/**
	 * Is the user allowed to Create/Destroy/AddTo/RemoveFrom shared collections (a.k.a CollectionsAdmin)
	 *
	 * @return	True if the user is CollectionAdmin, false otherwise.
	 */
	virtual bool IsUserCollectionsAdmin()
	{
		UBOOL bIsUserCollectionsAdmin = TRUE;
		UBOOL ConfigValueFound = GConfig->GetBool(	TEXT("DataBrowserSecurity"),
													TEXT("bIsUserCollectionsAdmin"),
													bIsUserCollectionsAdmin,
													GEditorUserSettingsIni);
		return bIsUserCollectionsAdmin ? true : false;
	}


	/** @return	TRUE if the any of the packages in the specified array have been cooked */
	UBOOL ContainsCookedPackageInSet( const TArray<UPackage*>& Packages )
	{
		for ( INT PackageIndex = 0; PackageIndex < Packages.Num(); PackageIndex++ )
		{
			const UPackage* Pkg = Packages(PackageIndex);
			if ( Pkg != NULL && (Pkg->PackageFlags&PKG_Cooked) != 0 )
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	/**
	 * Attempts to fully load the specified package name
	 *
	 * @param	PackageName		Name of the package to fully load
	 *
	 * @return	True if the package was loaded successfully
	 */
	virtual bool FullyLoadPackage( String^ PackageName )
	{
		UPackage* LoadedPackage = NULL;

		// Try to load the package from disk
		if ( IsAssetValidForLoading(PackageName) )
		{			
			LoadedPackage = FindObject<UPackage>( NULL, *CLRTools::ToFString( PackageName ) );
			// Partially loaded.
			if( LoadedPackage != NULL )
			{
				// Make sure the package is fully loaded
				LoadedPackage->FullyLoad();
			}
			// Not loaded yet.
			else
			{
				LoadedPackage = PackageTools::LoadPackage( FFilename( CLRTools::ToFString( PackageName ) ) );
			}
		}

		return ( LoadedPackage != NULL && LoadedPackage->IsFullyLoaded() );
	}


	/** Returns a border color to use for the specified object type string */
	virtual Color GetAssetVisualBorderColorForObjectTypeName( String^ InObjectTypeName )
	{
		Color BorderColor;		
		if ( ClassNameToBorderColorMap->TryGetValue( InObjectTypeName, BorderColor ) )
		{
			return BorderColor;
		}
		else
		{
			return Colors::Black;
		}
	}



	/** Updates custom label text for this asset if it has any */
	virtual List<String^>^ GenerateCustomLabelsForAsset( ContentBrowser::DataAssetItem^ InAssetItem )
	{
		List<String^>^ NewLabelList = gcnew List<String^>;

		// Is the asset loaded?
		INT NextLabelIndex = 0;
		if( InAssetItem->LoadedStatus != ContentBrowser::DataAssetItem::LoadedStatusType::NotLoaded )
		{
			// Let's make sure!
			UObject* FoundObject = NULL;
			UClass* AssetClass = FindObject<UClass>( ANY_PACKAGE, *CLRTools::ToFString( InAssetItem->AssetType ) );
			if( AssetClass != NULL )
			{
				FoundObject = UObject::StaticFindObject( AssetClass, ANY_PACKAGE, *CLRTools::ToFString( InAssetItem->FullyQualifiedPath ) );
			}
			if( FoundObject != NULL )
			{
				// If the object has a label renderer, add those labels!
				{
					TArray< FString > LabelLines;

					// Get the rendering info for this object
					FThumbnailRenderingInfo* RenderInfo = GUnrealEd->GetThumbnailManager()->GetRenderingInfo( FoundObject );
					if( RenderInfo != NULL && RenderInfo->LabelRenderer != NULL )
					{

						// This is the object that will render the labels
						UThumbnailLabelRenderer* LabelRenderer = RenderInfo->LabelRenderer;


						{
							UThumbnailLabelRenderer::ThumbnailOptions ThumbOpts;
							LabelRenderer->BuildLabelList( FoundObject, ThumbOpts, LabelLines );
						}
					}
					
					// NOTE: The first custom label is always the object name (which we don't care about), so
					//		 we skip it and start at the second label line
					for( INT CurLineIndex = 1; CurLineIndex < LabelLines.Num(); ++CurLineIndex )
					{
						String^ CurLabelLine = CLRTools::ToString( LabelLines( CurLineIndex ) );

						if( CurLabelLine->Length > 0 )
						{
							if( NewLabelList->Count < ContentBrowser::ContentBrowserDefs::MaxAssetListCustomLabels )
							{
								NewLabelList->Add( CurLabelLine );
							}
						}
					}
				}

				// If we didn't find any custom label text, then go ahead and add the UObject description label
				if( NewLabelList->Count == 0 )
				{
					String^ DescString = CLRTools::ToString( FoundObject->GetDesc() );

					if( DescString->Length > 0 )
					{
						NewLabelList->Add( DescString );
					}
				}
			}			
		}

		// Pad out the custom label list so that we at least have the minimum number of labels for a tool tip
		while( NewLabelList->Count < ContentBrowser::ContentBrowserDefs::MaxAssetListCustomLabels )
		{
			NewLabelList->Add( "" );
		}

		// Loop through all of the labels and see if any of them are warnings
		INT NumberOfWarnings = 0;
		String^ WarningPrefix = "_WARNING_";
		for( INT LabelIndex = 0; LabelIndex < NewLabelList->Count; ++LabelIndex)
		{
			String^ CurrentLabel = NewLabelList[LabelIndex];

			// All warnings should have "_WARNING_" as a prefix
			if( CurrentLabel->StartsWith( WarningPrefix ) )
			{
				String^ WarningText = CurrentLabel->Substring( WarningPrefix->Length );

				NewLabelList[LabelIndex] = WarningText;
				
				// If the asset has a single warning, it will be displayed, but in the case
				// of multiple warnings, the number of warnings will be displayed instead
				if( !NumberOfWarnings )
				{
					NewLabelList->Add( WarningText );
				}
				else
				{
					NewLabelList[ContentBrowser::ContentBrowserDefs::MaxAssetListCustomLabels] = (NumberOfWarnings + 1) + " WARNINGS!";
				}

				++NumberOfWarnings;
			}
		}
		// If there were no warnings, pad out the label list with an extra empty string
		if( NewLabelList->Count == ContentBrowser::ContentBrowserDefs::MaxAssetListCustomLabels )
		{
			NewLabelList->Add( "" );
		}

		return NewLabelList;
	}



	/** Updates custom label text for this asset if it has any */
	virtual List<String^>^ GenerateCustomDataColumnsForAsset( ContentBrowser::DataAssetItem^ InAssetItem )
	{
		List<String^>^ CustomDataColumns = gcnew List<String^>;

		// Is the asset loaded?
		INT NextLabelIndex = 0;
		if( InAssetItem->LoadedStatus != ContentBrowser::DataAssetItem::LoadedStatusType::NotLoaded )
		{
			// Let's make sure!
			UObject* FoundObject = NULL;
			UClass* AssetClass = FindObject<UClass>( ANY_PACKAGE, *CLRTools::ToFString( InAssetItem->AssetType ) );
			if( AssetClass != NULL )
			{
				FoundObject = UObject::StaticFindObject( AssetClass, ANY_PACKAGE, *CLRTools::ToFString( InAssetItem->FullyQualifiedPath ) );
			}
			if( FoundObject != NULL )
			{
				// Also update "detailed descriptions" for loaded assets.  You can view these in the asset list view.
				for( INT CurColumnIndex = 0; CurColumnIndex < ContentBrowser::ContentBrowserDefs::MaxAssetListCustomDataColumns; ++CurColumnIndex )
				{
					String^ CurColumnCustomData = CLRTools::ToString( FoundObject->GetDetailedDescription( CurColumnIndex ) );
					while( CurColumnIndex > CustomDataColumns->Count )
					{
						CustomDataColumns->Add( "" );
					}
					CustomDataColumns->Add( CurColumnCustomData );
				}
			}			
		}

		// Also pad out the custom data column list
		while( CustomDataColumns->Count < ContentBrowser::ContentBrowserDefs::MaxAssetListCustomDataColumns )
		{
			CustomDataColumns->Add( "" );
		}

		return CustomDataColumns;
	}



	/** Updates the 'Date Added' for an asset item */
	virtual DateTime GenerateDateAddedForAsset( ContentBrowser::DataAssetItem^ InAssetItem )
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return DateTime::Today;
		// oscar@snda

		// Grab all of the tags associated with this asset, even the system tags
		List<String^>^ SystemTagsOnAsset;
		FGameAssetDatabase::Get().QueryTagsForAsset(
			InAssetItem->FullName,
			ETagQueryOptions::SystemTagsOnly,
			SystemTagsOnAsset );

		for each( String^ CurSystemTag in SystemTagsOnAsset )
		{
			ESystemTagType TagType = FGameAssetDatabase::GetSystemTagType( CurSystemTag );
			if( TagType == ESystemTagType::DateAdded )
			{
				// Grab the tick count from the system tag and convert it to a number
				String^ DateAddedString = FGameAssetDatabase::GetSystemTagValue( CurSystemTag );
				const UINT64 DateAddedTickCount = Convert::ToUInt64( DateAddedString );

				// Create a date time structure using the tick count
				DateTime DateAdded( DateAddedTickCount );
				return DateAdded;
			}
		}

		// No date time tag, so give it today's date by default.  It's probably an unverified/ghost asset
		// that doesn't have a date yet
		return DateTime::Today;
	}

	/** Calculates memory usage for the passed in asset */
	virtual INT CalculateMemoryUsageForAsset( ContentBrowser::DataAssetItem^ InAssetItem )
	{
		// Default to -1 if the asset couldn't isn't or found.
		INT ResourceSize = -1;

		if( InAssetItem->LoadedStatus != ContentBrowser::DataAssetItem::LoadedStatusType::NotLoaded )
		{
			UObject* FoundObject = NULL;
			UClass* AssetClass = FindObject<UClass>( ANY_PACKAGE, *CLRTools::ToFString( InAssetItem->AssetType ) );
			if( AssetClass != NULL )
			{
				FoundObject = UObject::StaticFindObject( AssetClass, ANY_PACKAGE, *CLRTools::ToFString( InAssetItem->FullyQualifiedPath ) );
			}
			if( FoundObject != NULL )
			{
				ResourceSize = FoundObject->GetResourceSize();
			}
		}
		
		return ResourceSize;
	}

	/**
	 * Find the UPackage associated with a particular C# Package
	 *
	 * @param	Pkg	the C# package to search for
	 *
	 * @return	a pointer to the UPackage corresponding to the input package
	 */
	UPackage* PackageToUPackage( ContentBrowser::ObjectContainerNode^ Pkg )
	{
		UPackage* Result = NULL;

		if ( Pkg != nullptr )
		{
			Result = FindObject<UPackage>(NULL, *CLRTools::ToFString(Pkg->ObjectPathName));
		}

		return Result;
	}

	/**
	 * Find the C# Package associated with a particular UPackage
	 *
	 * @param	Pkg	the UPackage to search for
	 *
	 * @return	the C# Package corresponding to the input UPackage
	 */
	ContentBrowser::ObjectContainerNode^ UPackageToPackage( UPackage* Pkg )
	{
		ContentBrowser::ObjectContainerNode^ Result = nullptr;

		if ( Pkg != NULL && DataBrowserCtrl != nullptr )
		{
			System::String^ PackageName = CLRTools::ToString(Pkg->GetPathName());
			Result = DataBrowserCtrl->MySources->FindPackage(PackageName);
		}

		return Result;
	}

	/**
	 * Wrapper method for generating a list of UPackages, given a list of source UObjects.
	 *
	 * @param	InObjects		the list of objects to get packages for
	 * @param	out_Packages	will be filled with the top-level packages containing the objects from the input array.
	 */
	void GeneratePackageList( const TArray<UObject*>& InObjects, TArray<UPackage*>& out_Packages )
	{
		out_Packages.Empty();
		for ( INT ObjIndex = 0; ObjIndex < InObjects.Num(); ObjIndex++ )
		{
			//UPackage* AssetsOuter = Cast<UPackage>(InObjects(ObjIndex)->GetOuter());
			UPackage* AssetsOuter = Cast<UPackage>(InObjects(ObjIndex)->GetOutermost()); // oscar@snda

			// If the object is a level actor then it's outer won't be a UPackage so we'll check
			// for that here.  Those cases can come up when syncing the browser to level actors.
			if( AssetsOuter != NULL )
			{
				out_Packages.AddUniqueItem( AssetsOuter );
			}
		}
	}

	/**
	 * Update the list of packages
	 * 
	 * @param UseFlatView When true add all packages to the top-level layer.
	 */
	virtual void UpdatePackagesTree( bool IsUsingFlatView )
	{
		if ( !DataBrowserCtrl->bIsLoaded )
		{
			bPackageListUpdateRequested = true;
			bPackageFilterUpdateRequested = true;
			return;
		}

		// The sources ViewModel that we are going to populate
		ContentBrowser::PackagePanelModel^ Sources = DataBrowserCtrl->MySources;
		FString RootDirectoryPath = appBaseDir();
		FPackageFileCache::NormalizePathSeparators(RootDirectoryPath);

		Sources->SetRootDirectoryPath(CLRTools::ToString(RootDirectoryPath));

		// - - Populate package files - -

		// Find all the package files and show them in their respective folders (as they appear on disk)
		// Packages are presumed to be unloaded at this point.
		TArray< FString > PackageFiles( GPackageFileCache->GetDataFileList() );

		// Add the packages to the TreeView

		Dictionary< String^, ContentBrowser::ObjectContainerNode^ > StringPackageMap( StringComparer::OrdinalIgnoreCase );
		for (int PkgFileIdx=0; PkgFileIdx<PackageFiles.Num(); ++PkgFileIdx)
		{
			FString PkgPath = *PackageFiles(PkgFileIdx);

			FPackageFileCache::NormalizePathSeparators( PkgPath );

			if( PackageTools::IsPackagePathExternal( PkgPath ) )
			{
				// Skip packages that are external so they do not show up in the package tree twice.
				continue;
			}

			ContentBrowser::Package^ PackageNode = Sources->AddPackage( CLRTools::ToString( PkgPath ), IsUsingFlatView );
			StringPackageMap.Add( PackageNode->Name, PackageNode );
		}

		// - - Mark any loaded package files - -

		// Mark any packages that are in memory as loaded and partially loaded
		// Also build a map of LoadedPackage -> TreeNode; we will need this map for populating groups into packages.
		TSet<const UPackage*> FilteredPackageMap;
		TArray<UPackage*> LoadedPackageList;
		TSet<UPackage*> GroupPackages;
		GetFilteredPackageList( NULL, FilteredPackageMap, &GroupPackages, LoadedPackageList);


		Dictionary< IntPtr, ContentBrowser::ObjectContainerNode^ > PackageMap;

		FString NewPackageFolderName(TEXT("..\\..\\NewPackages\\"));
		FString ExternalPackageFolderName(TEXT("..\\..\\External\\"));

		for (int PkgIdx=0; PkgIdx<LoadedPackageList.Num(); PkgIdx++)
		{
			FString OutFilename;
			UPackage *Pkg = LoadedPackageList(PkgIdx);
			const FString PackageName = Pkg->GetName();
			const FGuid PackageGuid = Pkg->GetGuid();

			GPackageFileCache->FindPackageFile(*PackageName, &PackageGuid, OutFilename);

			if ( OutFilename.Len() != 0)
			{
				if(StringPackageMap.ContainsKey( CLRTools::ToString(Pkg->GetName())))
				{
					String^ CLRString = CLRTools::ToString(OutFilename);
					ContentBrowser::Package^ PackageViewModel = Sources->AddPackage( CLRString, IsUsingFlatView );
					if ( PackageViewModel == nullptr )
					{
						continue;
					}

					PackageMap.Add( (IntPtr)Pkg, PackageViewModel );

					// only do this if this method was called from somewhere besides ConditionalUpdatePackages b/c in that
					// case, we'll perform a full update after populating the tree control
					if ( !bPackageListUpdateUIRequested )
					{
						UpdatePackagesTreeUI(PackageViewModel, Pkg);
					}

				}

			}
		}


		// Since objects (and hence, packages) can appear in any order in memory, we need to add the group packages to the tree
		// iteratively.  Each pass checks each group in the GroupPackages array against the package map to see if its immediate parent 
		// has been added to the tree already.  If it has, it adds the group package as a child of that parent item and removes it from the 
		// GroupPackages array.  If not, it ignores that group and leaves it to be handled in a future iteration.  Once all the 
		// groups are removed from the GroupPackages array, the tree is fully loaded.
		TArray<UPackage*> GroupList;
		for(TSet<UPackage*>::TIterator Iter(GroupPackages); Iter; ++Iter)
		{
			GroupList.AddItem(*Iter);
		}

		while ( GroupList.Num() )
		{
			for(INT x = 0; x < GroupList.Num(); ++x)
			{
				UPackage* CurGroup = GroupList(x);

				FString GroupName = CurGroup->GetName();
				if(PackageMap.ContainsKey( (IntPtr)CurGroup) )
				{
					GroupList.Remove(x);
					x--;
					continue;
				}

				FString GroupOutName = CurGroup->GetOuter()->GetName();
				if( PackageMap.ContainsKey((IntPtr)CurGroup->GetOuter() ) )
				{
					ContentBrowser::ObjectContainerNode^ ParentNode = PackageMap[(IntPtr)CurGroup->GetOuter()];

					// If this packages outer is in the tree, add this package to the tree as a child of that item and remove it from the array.
					ContentBrowser::GroupPackage^ Group = gcnew ContentBrowser::GroupPackage(ParentNode, CLRTools::ToString(CurGroup->GetName()));
					Group = ParentNode->AddChildNode<ContentBrowser::GroupPackage^>(Group);
					PackageMap.Add( (IntPtr)CurGroup, Group );

					GroupList.Remove(x);
					x--;
				}
				else
				{
					GroupList.Remove(x);
					x--;

				}
			}
		}


		
		// Also add all of the groups that we know about from the GAD
		if(FGameAssetDatabase::IsInitialized())// oscar@snda
		{
			// Grab all unique asset paths from the GAD
			RefCountedStringDictionary^ UniqueAssetPaths = FGameAssetDatabase::Get().GetUniqueAssetPaths();

			// Add the unloaded groups!
			for each( String^ AssetPathOnly in UniqueAssetPaths->Keys )
			{
				// @todo CB: Should not include packages/groups from ghost assets?

				// Grab the object's name and package by splitting the full path string
				cli::array< String^ >^ AssetPackageAndGroupNames = AssetPathOnly->Split( TCHAR( '.' ) );

				// Grab the package name
				int CurPackageDepth = 0;
				String^ PackageName = AssetPackageAndGroupNames[ CurPackageDepth++ ];

				// Do we even have the package in our tree?
				ContentBrowser::ObjectContainerNode^ FoundPackageNode = nullptr;
				if( StringPackageMap.TryGetValue( PackageName, FoundPackageNode ) )
				{
					// Descend through the package's groups
					for( ; CurPackageDepth < AssetPackageAndGroupNames->Length; ++CurPackageDepth )
					{
						String^ CurGroupName = AssetPackageAndGroupNames[ CurPackageDepth ];

						bool bAllowRecursion = false;
						ContentBrowser::ObjectContainerNode^ FoundChildGroupNode = 
							FoundPackageNode->FindChildNode< ContentBrowser::ObjectContainerNode^ >( CurGroupName, bAllowRecursion );

						if( FoundChildGroupNode == nullptr )
						{
							// OK this group was missing so we'll need to create it now
							ContentBrowser::GroupPackage^ NewGroup =
								gcnew ContentBrowser::GroupPackage( FoundPackageNode, CurGroupName );
							FoundChildGroupNode = FoundPackageNode->AddChildNode< ContentBrowser::GroupPackage^ >( NewGroup );
						}


						// Continue descent!
						FoundPackageNode = FoundChildGroupNode;
					}
				}
				else
				{
					// Package from the GAD didn't exist on our package file cache.  We won't bother displaying it.
				}

			}

		}


		// now time to purge the tree nodes that should not be in the list of packages
		// we do this after because packages could have been added by the a group being added or
		// because the package has a file on disk, in which case it would have been added by the
		// first for-loop
		PurgeInvalidNodesFromTree(PackageFiles, LoadedPackageList, GroupPackages);
	}

	/**
	 * Returns whether saving the specified package is allowed
	 */
	UBOOL AllowPackageSave( UPackage* PackageToSave )
	{
		// DB: We explicitly don't check for cooked packages here, because
		// DB: this code needs to work for packages being saved for PIE.
		//@fixme ronp - should we still take selected object type into account?
// 		return LeftContainer->CurrentResourceType->IsSavePackageAllowed(PackageToSave);
		return TRUE;
	}

	/**
	 * Attempts to save the specified packages; helper function for by e.g. SaveSelectedPackages().
	 *
	 * @param		PackagesToSave				The content packages to save.
	 * @param		bUnloadPackagesAfterSave	If TRUE, unload each package if it was saved successfully.
	 * @return									TRUE if all packages were saved successfully, FALSE otherwise.
	 */
	virtual UBOOL SavePackages( const TArray<UPackage*>& PackagesToSave, UBOOL bUnloadPackagesAfterSave );

	void GetPackagesForSelectedAssetItems( TArray<UPackage*>& out_Packages, TArray<FString>& out_PackageNames );

	/**
	 * Generates a list of UPackage objects which are currently selected.  If a group is selected, the group's root will be selected instead.
	 *
	 * @param	OutPackageList		receives the list of UPackage objects
	 * @param	pOutPacakgeNames	if specified, receives the array of package names as FStrings; this can be used to
	 *								easily determine if any selected packages are not currently loaded.
	 * @param	bCullNullPackages	specify FALSE to add NULL entries to the OutPackageList array for any packages which couldn't be found
	 */
	void GetSelectedRootPackages( TArray<UPackage*>& OutPackageList )
	{
		TArray<FString>* pUnused = NULL;
		UBOOL bUnusedCull = TRUE;
		GetSelectedRootPackages(OutPackageList, pUnused, bUnusedCull);
	}

	/**
	* Generates a list of UPackage objects which are currently selected. Groups individually selected will appear as their own package.  This way you can select groups seperatley from their outer.
	*
	* @param	OutPackageList		receives the list of UPackage objects
	* @param	pOutPacakgeNames	if specified, receives the array of package names as FStrings; this can be used to
	*								easily determine if any selected packages are not currently loaded.
	* @param	bCullNullPackages	specify FALSE to add NULL entries to the OutPackageList array for any packages which couldn't be found
	*/
	void GetSelectedPackages( TArray<UPackage*>& OutPackageList )
	{
		TArray<FString>* pUnused = NULL;
		UBOOL bUnusedCull = TRUE;
		GetSelectedPackages(OutPackageList, pUnused, bUnusedCull);
	}

	/**
	 * Generates a list of UPackage objects which are currently selected.  If a group is selected, the group's root will be selected instead.
	 *
	 * @param	OutPackageList		receives the list of UPackage objects
	 * @param	pOutPacakgeNames	if specified, receives the array of package names as FStrings; this can be used to
	 *								easily determine if any selected packages are not currently loaded.
	 * @param	bCullNullPackages	specify FALSE to add NULL entries to the OutPackageList array for any packages which couldn't be found
	 */
	void GetSelectedRootPackages( TArray<UPackage*>& OutPackageList, TArray<FString>* pOutPackageNames, UBOOL bCullNullPackages )
	{
		TArray<FString> tmpNames;
		TArray<FString>& PackageNames = pOutPackageNames != NULL ? *pOutPackageNames : tmpNames;

		OutPackageList.Empty();
		PackageNames.Empty();

		ReadOnlyCollection<ContentBrowser::Package^>^ TopLevelPackages = DataBrowserCtrl->MyPackagePanel->MakeSelectedTopLevelPackageList();
		for ( INT PackageIndex = 0; PackageIndex < TopLevelPackages->Count; PackageIndex++ )
		{
			ContentBrowser::Package^ Pkg = TopLevelPackages[PackageIndex];

			UPackage* UPkg = PackageToUPackage(Pkg);
			if ( UPkg != NULL || !bCullNullPackages )
			{
				OutPackageList.AddUniqueItem(UPkg);
			}

			PackageNames.AddItem(CLRTools::ToFString(Pkg->Name));
		}
	}

	/**
	 * Generates a list of UPackage objects which are currently selected.   Groups individually selected will appear as their own package.  This way you can select groups seperatley from their outer.
	 *
	 * @param	OutPackageList		receives the list of UPackage objects
	 * @param	pOutPacakgeNames	if specified, receives the array of package names as FStrings; this can be used to
	 *								easily determine if any selected packages are not currently loaded.
	 * @param	bCullNullPackages	specify FALSE to add NULL entries to the OutPackageList array for any packages which couldn't be found
	 */
	void GetSelectedPackages( TArray<UPackage*>& OutPackageList, TArray<FString>* pOutPackageNames, UBOOL bCullNullPackages )
	{
		TArray<FString> tmpNames;
		TArray<FString>& PackageNames = pOutPackageNames != NULL ? *pOutPackageNames : tmpNames;

		OutPackageList.Empty();
		PackageNames.Empty();

		// Get the currently selected nodes from the sources panel
		ReadOnlyCollection<ContentBrowser::ObjectContainerNode^>^ Packages = DataBrowserCtrl->MyPackagePanel->MakeSelectedPackageAndGroupList();
		
		// Iterate through each selected pacakge, including groups
		for ( INT PackageIndex = 0; PackageIndex < Packages->Count; PackageIndex++ )
		{
			ContentBrowser::ObjectContainerNode^ Pkg = Packages[PackageIndex];

			UPackage* UPkg = PackageToUPackage(Pkg);
			if ( UPkg != NULL || !bCullNullPackages )
			{
				OutPackageList.AddUniqueItem(UPkg);
			}

			PackageNames.AddItem(CLRTools::ToFString(Pkg->Name));
		}
	}

	/**
	 * Retrieves the names of the package and group which are currently selected in the package tree view
	 */
	void GetSelectedPackageAndGroupName( FString& SelectedPackageName, FString& SelectedGroupName );

	/**
	 * Generates a WPF bitmap for the specified thumbnail
	 * 
	 * @param	InThumbnail		The thumbnail object to create a bitmap for
	 * 
	 * @return	Returns the BitmapSource for the thumbnail, or null on failure
	 */
	static Imaging::BitmapSource^ CreateBitmapSourceForThumbnail( const FObjectThumbnail& InThumbnail );

protected:
	/** Helper function that attempts to check out the specified top-level packages. */
	void CheckOutRootPackages( const TArray<UPackage*>& Packages )
	{
		PackageTools::CheckOutRootPackages(Packages);
	}

	/**
	 * CanExecuteRoutedEventHandler for the DataBrowser's custom commands
	 * 
	 * @param	Sender	the object that generated the event
	 * @param	EvtArgs	details about the event that was generated
	 */
	void OnCanExecutePackageCommand( System::Object^ Sender, System::Windows::Input::CanExecuteRoutedEventArgs^ EvtArgs, const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames )
	{
		//no one else should ever handle this event.
		EvtArgs->Handled = true;

		bool bCanExecute = true;
		if ( EvtArgs->Command == ContentBrowser::PackageCommands::FullyLoadPackage )
		{
			bCanExecute = false;

			// only enabled if at least one selected package isn't already fully loaded
			for ( INT PackageIdx = 0; PackageIdx < Packages.Num(); PackageIdx++ )
			{
#if _DEBUG
				const TCHAR* PkgName = *PackageNames(PackageIdx);
#endif
				UPackage* Pkg = Packages(PackageIdx);
				if ( Pkg == NULL )
				{
					if ( IsAssetValidForLoading(CLRTools::ToString(PackageNames(PackageIdx))) )
					{
						bCanExecute = true;
						break;
					}
				}
				else if ( !Pkg->IsFullyLoaded() )
				{
					bCanExecute = true;
					break;
				}
			}
		}
		else if ( EvtArgs->Command == ContentBrowser::PackageCommands::UnloadPackage
			||	EvtArgs->Command == Input::ApplicationCommands::Save 
			||	EvtArgs->Command == ContentBrowser::PackageCommands::SaveAsset
			||	EvtArgs->Command == ContentBrowser::PackageCommands::CheckErrors )
		{
			bCanExecute = false;

			// only enabled if at least one selected package is loaded at all
			for ( INT PackageIdx = 0; PackageIdx < Packages.Num(); PackageIdx++ )
			{
#if _DEBUG
				const TCHAR* PkgName = *PackageNames(PackageIdx);
#endif
				UPackage* Pkg = Packages(PackageIdx);
				if ( Pkg != NULL )
				{
					bCanExecute = true;
					break;
				}
			}
		}

		EvtArgs->CanExecute = bCanExecute;
	}

	/**
	 * CanExecuteRoutedEventHandler for the DataBrowser's custom commands
	 * 
	 * @param	Sender	the object that generated the event
	 * @param	EvtArgs	details about the event that was generated
	 */
	void OnCanExecuteSCCCommand( System::Object^ Sender, System::Windows::Input::CanExecuteRoutedEventArgs^ EvtArgs, const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames )
	{
		//no one else should ever handle this event.
		EvtArgs->Handled = true;

		bool bCanExecute = false;
#if HAVE_SCC
		if (FSourceControl::IsEnabled())
		{
			if ( EvtArgs->Command == ContentBrowser::SourceControlCommands::CheckInSCC )
			{
				for ( INT PackageIndex = 0; PackageIndex < PackageNames.Num(); PackageIndex++ )
				{
					INT SCCState = GPackageFileCache->GetSourceControlState(*PackageNames(PackageIndex));
					if (( SCCState == SCC_CheckedOut) || (SCCState == SCC_NotInDepot))
					{
						bCanExecute = true;
						break;
					}
				}
			}
			else if ( EvtArgs->Command == ContentBrowser::SourceControlCommands::CheckOutSCC )
			{
				for ( INT PackageIndex = 0; PackageIndex < PackageNames.Num(); PackageIndex++ )
				{
					INT SCCState = GPackageFileCache->GetSourceControlState(*PackageNames(PackageIndex));
					if ( SCCState == SCC_ReadOnly )
					{
						bCanExecute = true;
						break;
					}
				}
			}
			else if ( EvtArgs->Command == ContentBrowser::SourceControlCommands::RevertSCC )
			{
				for ( INT PackageIndex = 0; PackageIndex < PackageNames.Num(); PackageIndex++ )
				{
					INT SCCState = GPackageFileCache->GetSourceControlState(*PackageNames(PackageIndex));
					if ( SCCState == SCC_CheckedOut )
					{
						bCanExecute = true;
						break;
					}
				}
			}
			else if ( EvtArgs->Command == ContentBrowser::SourceControlCommands::RevisionHistorySCC )
			{
				if (PackageNames.Num()==1)
				{
					INT SCCState = GPackageFileCache->GetSourceControlState(*PackageNames(0));
					if ( SCCState != SCC_NotInDepot )
					{
						bCanExecute = true;
					}
				}
			}
			else
			{
				bCanExecute = true;
			}
		}
#endif

		EvtArgs->CanExecute = bCanExecute;
	}

	/**
	 * ExecutedRoutedEventHandler for the DataBrowser's custom commands
	 * 
	 * @param	Sender	the object that generated the event
	 * @param	EvtArgs	details about the event that was generated
	 */
	void ExecutePackageCommand( System::Object^ Sender, System::Windows::Input::ExecutedRoutedEventArgs^ EvtArgs, const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames );

	/**
	 * ExecutedRoutedEventHandler for the DataBrowser's custom commands
	 * 
	 * @param	Sender	the object that generated the event
	 * @param	EvtArgs	details about the event that was generated
	 */
	void ExecuteSCCCommand( System::Object^ Sender, System::Windows::Input::ExecutedRoutedEventArgs^ EvtArgs, const TArray<UPackage*>& Packages, const TArray<FString>& InPackageNames )
	{
		//no one else should ever handle this event.
		EvtArgs->Handled = true;
#if HAVE_SCC
		if (FSourceControl::IsEnabled())
		{
			TArray <FString> PackageNames = InPackageNames;
			FSourceControl::ConvertPackageNamesToSourceControlPaths(PackageNames);
			if ( EvtArgs->Command == ContentBrowser::SourceControlCommands::RefreshSCC )
			{
				EvtArgs->Handled = true;
				FSourceControl::IssueUpdateState(NativeBrowserPtr, PackageNames);
			}
			else if ( EvtArgs->Command == ContentBrowser::SourceControlCommands::RevisionHistorySCC )
			{
				EvtArgs->Handled = true;
				SourceControlWindows::DisplayRevisionHistory(PackageNames);
			}
			else if ( EvtArgs->Command == ContentBrowser::SourceControlCommands::CheckInSCC )
			{
				EvtArgs->Handled = true;

				// Prompt the user to ask if they would like to first save any dirty packages they are trying to check-in
				const FEditorFileUtils::EPromptReturnCode UserResponse = FEditorFileUtils::PromptForCheckoutAndSave( Packages, TRUE, TRUE );

				// If the user elected to save dirty packages, but one or more of the packages failed to save properly OR if the user
				// cancelled out of the prompt, don't follow through on the check-in process
				const UBOOL bShouldProceed = ( UserResponse == FEditorFileUtils::EPromptReturnCode::PR_Success || UserResponse == FEditorFileUtils::EPromptReturnCode::PR_Declined );
				if ( bShouldProceed )
				{
					SourceControlWindows::PromptForCheckin(NativeBrowserPtr, InPackageNames);
				}
				else
				{
					// If a failure occurred, alert the user that the check-in was aborted. This warning shouldn't be necessary if the user cancelled
					// from the dialog, because they obviously intended to cancel the whole operation.
					if ( UserResponse == FEditorFileUtils::EPromptReturnCode::PR_Failure )
					{
						appMsgf( AMT_OK, *LocalizeUnrealEd("SCC_Checkin_Aborted") );
					}
				}
			}
			else if ( EvtArgs->Command == ContentBrowser::SourceControlCommands::CheckOutSCC )
			{
				EvtArgs->Handled = true;
				FSourceControl::CheckOut(NativeBrowserPtr, PackageNames);
			}
			else if ( EvtArgs->Command == ContentBrowser::SourceControlCommands::RevertSCC )
			{
				EvtArgs->Handled = true;

				// Prompt the user with the revert file dialog so they don't just revert things
				// on accident
				SourceControlWindows::PromptForRevert( NativeBrowserPtr, InPackageNames );
			}
		}
#endif
	}

	/**
	 * CanExecuteRoutedEventHandler for the object factory context menu
	 * 
	 * @param	Sender	the object that generated the event
	 * @param	EvtArgs	details about the event that was generated
	 */
	void CanExecuteNewObjectCommand( Object^ Sender, Input::CanExecuteRoutedEventArgs^ EventArgs )
	{
		//no one else should ever handle this event.
		EventArgs->Handled = true;

		bool bCanExecute = false;
		
		Input::RoutedCommand^ Command = (Input::RoutedCommand^)EventArgs->Command;
		if ( Command->OwnerType == ContentBrowser::ObjectFactoryCommands::typeid )
		{
			// for now, assume that if parameter is null, it means this is the New Object button's command
			if ( EventArgs->Parameter == nullptr )
			{
				bCanExecute = true;
			}
			else
			{
				int FactoryClassIndex = (int)EventArgs->Parameter - ID_NEWOBJ_START;

				if (ObjectFactoryClasses.IsValid()
				&&	FactoryClassIndex >= 0
				&&	FactoryClassIndex < ObjectFactoryClasses->Num()
				&&	(*ObjectFactoryClasses.Get())(FactoryClassIndex) != NULL)
				{
					bCanExecute = true;
				}
			}
		}

		EventArgs->CanExecute = bCanExecute;
	}

	/**
	 * ExecutedRoutedEventHandler for the object factory context menu
	 * 
	 * @param	Sender	the object that generated the event
	 * @param	EvtArgs	details about the event that was generated
	 */
	void ExecuteNewObjectCommand( Object^ Sender, Input::ExecutedRoutedEventArgs^ EventArgs )
	{ 
		//no one else should ever handle this event.
		EventArgs->Handled = true;

		Input::RoutedCommand^ Command = (Input::RoutedCommand^)EventArgs->Command;

		if ( Command->OwnerType == ContentBrowser::ObjectFactoryCommands::typeid )
		{
			UClass* FactoryClass = NULL;

			// for now, assume that if parameter is null, it means this is the New Object button's command
			if ( EventArgs->Parameter != nullptr )
			{
				int FactoryClassIndex = (int)EventArgs->Parameter - ID_NEWOBJ_START;
				check(FactoryClassIndex >= 0);
				check(FactoryClassIndex < ObjectFactoryClasses->Num());

				TArray<UClass*>& FactoryClasses = *ObjectFactoryClasses.Get();
				FactoryClass = FactoryClasses(FactoryClassIndex);
			}

			check(FactoryClass == NULL || FactoryClass->IsChildOf(UFactory::StaticClass()));

			FString SelectedPackage = TEXT("MyPackage"), SelectedGroup;
			GetSelectedPackageAndGroupName(SelectedPackage, SelectedGroup);
			
			WxDlgNewGeneric Dialog;
			if( Dialog.ShowModal( SelectedPackage, SelectedGroup, FactoryClass, BrowsableObjectTypeList.Get() ) == wxID_OK )
			{
				// The user hit OK, now we should try to open the corresponding object editor if possible

				if( FactoryClass == NULL )
				{
					FactoryClass = Dialog.GetFactoryClass();
				}
				check(FactoryClass);

				UFactory* FactoryCDO = FactoryClass->GetDefaultObject<UFactory>();

				// Does the created object have an editor to open after being created?
				if( FactoryCDO->bEditAfterNew )
				{
					const FScopedBusyCursor BusyCursor;

					UObject* CreatedObject = Dialog.GetCreatedObject();

					if( CreatedObject )
					{
						// An object was successfully created and the object created has an editor to open.
						// So, inform the content browser to open the editor associated to this object. 
						GCallbackEvent->Send( FCallbackEventParameters( NULL, CALLBACK_RefreshContentBrowser, CBR_ActivateObject, CreatedObject ) );
					}
				}
			}
		}
	}



	/**
	 * Updates whether or not a specific collection command can be executed
	 * 
	 * @param	Sender	the object that generated the event
	 * @param	EvtArgs	details about the event that was generated
	 */
	void CanExecuteCollectionCommand( Object^ Sender, Input::CanExecuteRoutedEventArgs^ EventArgs )
	{
		//no one else should ever handle this event.
		EventArgs->Handled = true;

		bool bCanExecute = false;
		
		Input::RoutedCommand^ Command = (Input::RoutedCommand^)EventArgs->Command;
		if( Command->OwnerType == ContentBrowser::CollectionCommands::typeid )
		{
			// Figure out what type of collection we're dealing with here, and get it's name!
			bool bIsSharedCollection = false;
			bool bIsLocalCollection = false;
// 			ReadOnlyCollection< String^ >^ SelectedSharedCollectionNames =
// 				DataBrowserCtrl->MyPackagePanel->GetSelectedCollectionNames( ContentBrowser::EBrowserCollectionType::Shared )->AsReadOnly();
// 			ReadOnlyCollection< String^ >^ SelectedPrivateCollectionNames =
// 				DataBrowserCtrl->MyPackagePanel->GetSelectedCollectionNames( ContentBrowser::EBrowserCollectionType::Private )->AsReadOnly();
// 			ReadOnlyCollection< String^ >^ SelectedLocalCollectionNames =
// 				DataBrowserCtrl->MyPackagePanel->GetSelectedCollectionNames( ContentBrowser::EBrowserCollectionType::Local )->AsReadOnly();
// 			if( SelectedSharedCollectionNames->Count > 0 )
// 			{
// 				bIsSharedCollection = true;
// 			}
// 			else if( SelectedPrivateCollectionNames->Count > 0 )
// 			{
// 				bIsSharedCollection = false;
// 			}
// 			else if( SelectedLocalCollectionNames->Count > 0 )
// 			{
// 				bIsLocalCollection = true;
// 			}

// 			bool bIsPrivateCollection = !bIsSharedCollection && !bIsLocalCollection;
// 			int SelectedCollectionCount = SelectedPrivateCollectionNames->Count + SelectedSharedCollectionNames->Count + SelectedLocalCollectionNames->Count;
// 
// 			if( SelectedCollectionCount > 0 )
// 			{
// 				// Rename
// 				if( Command == ContentBrowser::CollectionCommands::Rename )
// 				{
// 					// Cannot rename local collections right now.
// 					bCanExecute = SelectedCollectionCount == 1 && !bIsLocalCollection && ( bIsPrivateCollection || IsUserCollectionsAdmin() );
// 				}
// 
// 				// Create Shared Copy
// 				if( Command == ContentBrowser::CollectionCommands::CreateSharedCopy )
// 				{
// 					bCanExecute = SelectedCollectionCount == 1;
// 				}
// 
// 				// Create Private Copy
// 				if( Command == ContentBrowser::CollectionCommands::CreatePrivateCopy )
// 				{
// 					bCanExecute = SelectedCollectionCount == 1;
// 				}
// 
// 				// Destroy
// 				if( Command == ContentBrowser::CollectionCommands::Destroy )
// 				{
// 					// NOTE: Destroy supports multiple collections at once!
// 					bCanExecute = bIsLocalCollection || bIsPrivateCollection || IsUserCollectionsAdmin();
// 				}
// 			}
		}

		EventArgs->CanExecute = bCanExecute;
	}


	/**
	 * Executes a collection command
	 * 
	 * @param	Sender	the object that generated the event
	 * @param	EvtArgs	details about the event that was generated
	 */
	void ExecuteCollectionCommand( Object^ Sender, Input::ExecutedRoutedEventArgs^ EventArgs )
	{ 
		//no one else should ever handle this event.
		EventArgs->Handled = true;

		Input::RoutedCommand^ Command = (Input::RoutedCommand^)EventArgs->Command;
		if( Command->OwnerType == ContentBrowser::CollectionCommands::typeid )
		{
			// Figure out what type of collection we're dealing with here, and get it's name!
			bool bIsSharedCollection = false;
			bool bIsLocalCollection = false;
// 			ReadOnlyCollection< String^ >^ SelectedSharedCollectionNames =
// 				DataBrowserCtrl->MyPackagePanel->GetSelectedCollectionNames( ContentBrowser::EBrowserCollectionType::Shared )->AsReadOnly();
// 			ReadOnlyCollection< String^ >^ SelectedPrivateCollectionNames =
// 				DataBrowserCtrl->MyPackagePanel->GetSelectedCollectionNames( ContentBrowser::EBrowserCollectionType::Private )->AsReadOnly();
// 			ReadOnlyCollection< String^ >^ SelectedLocalCollectionNames =
// 				DataBrowserCtrl->MyPackagePanel->GetSelectedCollectionNames( ContentBrowser::EBrowserCollectionType::Local )->AsReadOnly();
// 
// 			if( SelectedSharedCollectionNames->Count > 0 )
// 			{
// 				bIsSharedCollection = true;
// 			}
// 			else if( SelectedPrivateCollectionNames->Count > 0 )
// 			{
// 				bIsSharedCollection = false;
// 			}
// 			else if( SelectedLocalCollectionNames->Count > 0 )
// 			{
// 				bIsLocalCollection = true;
// 			}
// 
// 			bool bIsPrivateCollection = !bIsSharedCollection && !bIsLocalCollection;
// 
// 			int SelectedCollectionCount = SelectedPrivateCollectionNames->Count + SelectedSharedCollectionNames->Count + SelectedLocalCollectionNames->Count;
// 
// 			if( SelectedCollectionCount > 0 )
// 			{
// 				// Rename
// 				if( Command == ContentBrowser::CollectionCommands::Rename )
// 				{
// 					if( SelectedCollectionCount == 1 )
// 					{
// 						// Prompt the user to rename the collection
// 						ContentBrowser::EBrowserCollectionType CollectionType =
// 							bIsSharedCollection ? ContentBrowser::EBrowserCollectionType::Shared : ContentBrowser::EBrowserCollectionType::Private;
// 						DataBrowserCtrl->MyPackagePanel->ShowPromptToRenameCollection( CollectionType );
// 					}
// 				}
// 
// 				// Create Shared/Private Copy
// 				if( Command == ContentBrowser::CollectionCommands::CreateSharedCopy ||
// 					Command == ContentBrowser::CollectionCommands::CreatePrivateCopy )
// 				{
// 					if( SelectedCollectionCount == 1 )
// 					{
// 						ContentBrowser::EBrowserCollectionType SourceType = ContentBrowser::EBrowserCollectionType::Shared;
// 						if( bIsPrivateCollection )
// 						{
// 							SourceType = ContentBrowser::EBrowserCollectionType::Private;
// 						}
// 						else if( bIsLocalCollection )
// 						{
// 							SourceType = ContentBrowser::EBrowserCollectionType::Local;
// 						}
// 
// 						// Prompt the user to create a shared copy
// 						ContentBrowser::EBrowserCollectionType TargetType =
// 							Command == ContentBrowser::CollectionCommands::CreateSharedCopy ? ContentBrowser::EBrowserCollectionType::Shared : ContentBrowser::EBrowserCollectionType::Private;
// 						DataBrowserCtrl->MyPackagePanel->ShowPromptToCopyCollection( SourceType, TargetType );
// 					}
// 				}
// 
// 				// Destroy
// 				if( Command == ContentBrowser::CollectionCommands::Destroy )
// 				{
// 					if( bIsPrivateCollection || bIsLocalCollection )
// 					{
// 						DataBrowserCtrl->MyPackagePanel->ShowPromptToDestroySelectedPrivateCollections();
// 					}
// 					else
// 					{
// 						DataBrowserCtrl->MyPackagePanel->ShowPromptToDestroySelectedSharedCollections();
// 					}
// 				}
// 			}
		}
	}


public:
	/**
	 * CanExecuteRoutedEventHandler for the DataBrowser's custom commands
	 * 
	 * @param	Sender	the object that generated the event
	 * @param	EvtArgs	details about the event that was generated
	 */
	virtual void CanExecuteMenuCommand( System::Object^ Sender, System::Windows::Input::CanExecuteRoutedEventArgs^ EvtArgs );

	/**
	 * ExecutedRoutedEventHandler for the DataBrowser's custom commands
	 * 
	 * @param	Sender	the object that generated the event
	 * @param	EvtArgs	details about the event that was generated
	 */
	virtual void ExecuteMenuCommand( System::Object^ Sender, System::Windows::Input::ExecutedRoutedEventArgs^ EvtArgs );

	/**
	 * CanExecuteRoutedEventHandler for the DataBrowser's custom commands
	 * 
	 * @param	Sender	the object that generated the event
	 * @param	EvtArgs	details about the event that was generated
	 */
	virtual void CanExecuteAssetCommand( System::Object^ Sender, System::Windows::Input::CanExecuteRoutedEventArgs^ EvtArgs );

	/**
	 * ExecutedRoutedEventHandler for the DataBrowser's custom commands
	 * 
	 * @param	Sender	the object that generated the event
	 * @param	EvtArgs	details about the event that was generated
	 */
	virtual void ExecuteAssetCommand( System::Object^ Sender, System::Windows::Input::ExecutedRoutedEventArgs^ EvtArgs );


	/**
	 * Returns true if confirmation prompt should be displayed
	 * 
	 * @param InType Type of confirmation prompt to check
	 */
	virtual bool ShouldShowConfirmationPrompt( ContentBrowser::ConfirmationPromptType InType )
	{
		return ShowConfirmationPrompt[ (int)InType ];
	}


	/**
	 * Disables a type of confirmation prompt
	 * 
	 * @param InType Type of confirmation prompt to disable
	 */
	virtual void DisableConfirmationPrompt( ContentBrowser::ConfirmationPromptType InType )
	{
		check( (UINT)InType < ARRAY_COUNT( ShowConfirmationPrompt ) );
		ShowConfirmationPrompt[ (int)InType ] = false;
	}



	/**
	 * Display the specified objects in the content browser
	 *
	 * @param	InObjects	One or more objects to display, or if empty deselects all
	 *
	 * @return	True if the package selection changed; false if the currently selected package is already selected.
	 */
	bool SyncToObjects( TArray< UObject* >& InObjects );

	/** 
	 * Select the specified packages in the content browser
	 * 
	 * @param	InPackages	One or more packages to select
	 */
	void SyncToPackages( const TArray< UPackage *>& InPackages );

	/**
	 * Places focus straight into the Search filter
	 */
	void GoToSearch();


	void Resize(HWND hWndParent, int x, int y, int Width, int Height)
	{
		SetWindowPos(static_cast<HWND>(InteropWindow->Handle.ToPointer()), NULL, 0, 0, Width, Height, SWP_NOMOVE | SWP_NOZORDER);
	}


	/** Returns any serializable Unreal Objects this object is currently holding on to */
	void QuerySerializableObjects( TArray< UObject* >& OutObjects );


	/** Called every main engine loop update */
	void Tick();

	
	/** Called right before an engine garbage collection occurs */
	void OnPendingGarbageCollection();


	/** Called when an object is modified (PostEditChange) */
	void OnObjectModification( UObject* InObject );
	void OnObjectModification( UObject* InObject, ContentBrowser::AssetStatusUpdateFlags UpdateFlags );

	void RequestSCCStateUpdate()
	{
		bIsSCCStateDirty = true;
	}

	/** Clears the content browser filter panel */
	void ClearFilter();

	/**
	 * Removes the given package from the content browser. As a result, the package will be removed from the 
	 * package view, which makes the package invisible. However, the package will not be deleted. 
	 *
	 * @param	PackageToRemove		The package to remove from the content browser.
	 */
	void MDataBrowserControl::RemoveFromPackageList(UPackage* PackageToRemove)
	{
		// The given package must be valid. 
		check(PackageToRemove);
		DataBrowserCtrl->MySources->RemovePackage(UPackageToPackage(PackageToRemove));
	}

	void RequestPackageListUpdate( /*DefaultValueParmBoolFalse*/bool bUpdateUIOnly, UObject* SourceObject );
	void RequestCollectionListUpdate( /*DefaultValueParmBoolFalse*/bool bUpdateUIOnly );
	void RequestAssetListUpdate( AssetListRefreshMode RefreshMode, UObject* SourceObject, UINT AssetUpdateFlagMask );
	void RequestSyncAssetView( UObject* SourceObject );


	/** Kicks off a new asset view query */
	void StartAssetQuery()
	{

		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return;
		// oscar@snda

		// Clear any existing queries
		AssetQuery_ClearQuery();

		QueryEngineState = EDataBrowserQueryState::StartNewQuery;


		// Update cached query data
		{
			if( CachedQueryData.get() == nullptr )
			{
				CachedQueryData.reset( gcnew CachedQueryDataType() );
			}

			// NOTE: The following accessors actually generate full COPIES of the data on demand
//  			CachedQueryData->SelectedSharedCollectionNames =
//  				DataBrowserCtrl->MyPackagePanel->GetSelectedCollectionNames( ContentBrowser::EBrowserCollectionType::Shared )->AsReadOnly();
//  			CachedQueryData->SelectedPrivateCollectionNames =
//  				DataBrowserCtrl->MyPackagePanel->GetSelectedCollectionNames( ContentBrowser::EBrowserCollectionType::Private )->AsReadOnly();
//  			CachedQueryData->SelectedLocalCollectionNames = 
//  				DataBrowserCtrl->MyPackagePanel->GetSelectedCollectionNames( ContentBrowser::EBrowserCollectionType::Local )->AsReadOnly();

			DataBrowserCtrl->MyPackagePanel->MakeSelectedPathNameAndOutermostFullNameList(
				CachedQueryData->SelectedPathNames,
				CachedQueryData->SelectedOutermostPackageNames,
				CachedQueryData->ExplicitlySelectedOutermostPackageNames);
			
			// Selecting collections and packages as sources simultaneously is unsupported. The UI should ensure this.
//  			check( ( CachedQueryData->SelectedSharedCollectionNames->Count > 0 ) +
//  				   ( CachedQueryData->SelectedPrivateCollectionNames->Count > 0 ) +
//  				   ( CachedQueryData->SelectedPathNames->Count > 0 ) <= 1 );
		}
	}

	
	/** Updated query for the asset */
	void UpdateAssetQuery()
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return;
		// oscar@snda

		const double UpdateStartTime = appSeconds();		
		QueryUpdateMode UpdateFlags = (bDoFullAssetViewUpdate) ? QueryUpdateMode::Amortizing : QueryUpdateMode::Amortizing | QueryUpdateMode::QuickUpdate;

		if( QueryEngineState != EDataBrowserQueryState::Idle )
		{
			if( QueryEngineState == EDataBrowserQueryState::StartNewQuery )
			{
				QueryEngineState = EDataBrowserQueryState::GatherObjects;
				DataBrowserCtrl->OnAssetPopulationStarted( (UpdateFlags & QueryUpdateMode::QuickUpdate) == QueryUpdateMode::QuickUpdate );
			}


			if( QueryEngineState == EDataBrowserQueryState::GatherObjects )
			{
				if( AssetQuery_GatherPackagesAndObjects() )
				{
					QueryEngineState = EDataBrowserQueryState::AddLoadedAssets;

					// Reset iterator index for the next phase
					CachedQueryIteratorIndex = 0;

					// Quick updates only add or remove items; we do not clear the list for that.
					if ( (UpdateFlags & QueryUpdateMode::QuickUpdate ) == QueryUpdateMode::None )
					{
						ContentBrowser::DataAssetViewModel^ AssetsInView = DataBrowserCtrl->MyAssets;
						AssetsInView->Clear();
					}
					else
					{
						// We're doing a quick update.
						// Remove any assets that should be missing.
						// Cannot amortize this as it must be atomic to the user; we do not want to allow the user to interact with an asset that's being deleted.
						AssetQuery_RemoveMissingAssets( QueryUpdateMode::QuickUpdate );
					}
				}
			}


			int TotalAssetsToPopulate = CachedQueryObjects->Num() + CachedQueryAssetFullNameFNamesFromGAD->Num();

			if( QueryEngineState == EDataBrowserQueryState::AddLoadedAssets )
			{
				// Populate the UI with assets that are currently loaded
				if( AssetQuery_AddLoadedAssets( UpdateStartTime, UpdateFlags ) )
				{
					QueryEngineState = EDataBrowserQueryState::AddNonLoadedAssets;

					// Reset iterator index for the next phase
					CachedQueryIteratorIndex = 0;
				}
				else
				{
					DataBrowserCtrl->SetAssetUpdateProgress( 100.0*static_cast<double>(CachedQueryIteratorIndex) / TotalAssetsToPopulate );
				}
				
			}

			if( QueryEngineState == EDataBrowserQueryState::AddNonLoadedAssets )
			{
				// Populate the UI with assets from the GAD (not loaded)
				if( AssetQuery_AddNonLoadedAssets( UpdateStartTime, UpdateFlags ) )
				{
					// Done!  Clean up our temporary data structures.
					AssetQuery_ClearQuery();
					bDoFullAssetViewUpdate = false;
					DataBrowserCtrl->RequestFilterRefresh( ContentBrowser::RefreshFlags::Default );

					DataBrowserCtrl->OnAssetPopulationComplete( (UpdateFlags & QueryUpdateMode::QuickUpdate) == QueryUpdateMode::QuickUpdate );
				}
				else
				{
					DataBrowserCtrl->SetAssetUpdateProgress( 100.0*static_cast<double>(CachedQueryIteratorIndex + CachedQueryObjects->Num()) / TotalAssetsToPopulate );
				}
			}

			DataBrowserCtrl->UpdateAssetCount();
		}
	}



	/** Clears a current query and resets all cached state */
	void AssetQuery_ClearQuery()
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return;
		// oscar@snda

		CachedQueryData.reset();

		if ( !LastQueryAssetFullNameFNamesFromGAD.IsValid() )
		{
			LastQueryAssetFullNameFNamesFromGAD.Reset( new TLookupMap< FName > );
		}

		// Clear out our cached object list		
		CachedQueryObjects.Reset();

		// Remember the last set of assets that we populated; it will be used to perform incremental updates.
		if ( !LastQueryLoadedAssetFullNames.IsValid() )
		{
			LastQueryLoadedAssetFullNames.Reset( new TSet< FName > );
		}		
		
		if ( CallbackEventObjects )
		{
			CallbackEventObjects->Empty();
		}

		CachedQueryIteratorIndex = 0;

		QueryEngineState = EDataBrowserQueryState::Idle;
	}

		
		
	/** Restarts the current query */
	void AssetQuery_RestartQuery()
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return;
		// oscar@snda

		if( QueryEngineState != EDataBrowserQueryState::Idle )
		{
			auto_handle< CachedQueryDataType > SavedQueryData = CachedQueryData;
			AssetQuery_ClearQuery();
			CachedQueryData = SavedQueryData;

			QueryEngineState = EDataBrowserQueryState::StartNewQuery;
			StartAssetQuery();
		}
	}



	/** Gathers packages and objects from the engine and GAD for display in the UI */
	bool AssetQuery_GatherPackagesAndObjects()
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return false;
		// oscar@snda

		PROFILE_CONTENT_BROWSER( const double GatherObjectsStartTime = appSeconds() );

		// Get assets from loaded packages (put them in LoadedObjects)
		{
			// Find all the loaded packages
			PROFILE_CONTENT_BROWSER( const double GetFilteredPackageListStartTime = appSeconds() );
			TSet<const UPackage*> FilteredPackageMap;
			TArray<UPackage*> LoadedPackageList;
			GetFilteredPackageList( NULL, FilteredPackageMap, NULL, LoadedPackageList);
			PROFILE_CONTENT_BROWSER( debugf( TEXT( "DataBrowser|  GetFilteredPackageList took %0.6f seconds" ), appSeconds() - GetFilteredPackageListStartTime ) );


			// Get all the objects in the selected & loaded packages
			{
				PROFILE_CONTENT_BROWSER( const double GetObjectsInPackagesStartTime = appSeconds() );
				CachedQueryObjects.Reset( new TArray< UObject* >() );

				
				// Any packages selected?
				if (CachedQueryData->SelectedOutermostPackageNames->Count > 0)
				{
					// Find all the packages that are both loaded and selected
					TArray<UPackage*> SelectedPackages;

					// If there is something selected figure out which packages are both loaded and selected
					for (int PkgIdx=0; PkgIdx<LoadedPackageList.Num(); PkgIdx++)
					{
						String^ LoadedPackageName = CLRTools::ToString( LoadedPackageList(PkgIdx)->GetPathName() );
						if( CachedQueryData->SelectedOutermostPackageNames->Contains(LoadedPackageName) )
						{
							SelectedPackages.AddItem(LoadedPackageList(PkgIdx));
						}
					}
					/*UGenericBrowserType_Material ObjType;*/
					
					FString ObjType = "MM";
					for each (String^ PackageName in CachedQueryData->SelectedOutermostPackageNames)
					{
						GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString(PackageName), ObjType);
						break;
					}

					PackageTools::GetObjectsInPackagesWithType( &SelectedPackages, ClassToBrowsableObjectTypeMap.Get(), ObjType, *CachedQueryObjects.Get() );
				}
				else
				{
					TArray< FString > PackageFiles( GPackageFileCache->GetDataFileList());

					TArray<UPackage*> SelectedPackages;
					// If there is something selected figure out which packages are both loaded and selected
					for (int PkgIdx=0; PkgIdx<LoadedPackageList.Num(); PkgIdx++)
					{
						String^ LoadedPackageName = CLRTools::ToString( LoadedPackageList(PkgIdx)->GetPathName() );
						ContentBrowser::ObjectContainerNode^ GroupNode = DataBrowserCtrl->MySources->FindPackage(LoadedPackageName);
						if ( GroupNode != nullptr )
						{
							SelectedPackages.AddItem(LoadedPackageList(PkgIdx));
						}
					}
					// No packages selected, so get all objects
					PackageTools::GetObjectsInPackages( &SelectedPackages, ClassToBrowsableObjectTypeMap.Get(), *CachedQueryObjects.Get() );
				}

				PROFILE_CONTENT_BROWSER( debugf( TEXT( "DataBrowser|  GetObjectsInPackages took %0.6f seconds" ), appSeconds() - GetObjectsInPackagesStartTime ) );
			}

			
			
			// Update our set of loaded asset paths
			{
				// Save our previous loaded asset paths.
				LastQueryLoadedAssetFullNames.Reset( CachedQueryLoadedAssetFullNames.Release() );
				CachedQueryLoadedAssetFullNames.Reset( new TSet< FName > );

				const TArray< UObject* >& LoadedObjects = *CachedQueryObjects.Get();
				for( int CurObjIndex = 0; CurObjIndex < LoadedObjects.Num(); ++CurObjIndex )
				{
					CachedQueryLoadedAssetFullNames->Add( FName( *LoadedObjects( CurObjIndex )->GetFullName() ) );
				}
			}
		}


		// Get all GAD assets in selected Collections and Packages
		{
			// A list of tags representing the selected Collections and Packages
			List<String^>^ SelectedSourcesTags = gcnew List<String^>();

// 			// Convert all shared collection names to collection system tags
// 			for each ( String^ SelectedSharedCollectionName in CachedQueryData->SelectedSharedCollectionNames )
// 			{
// 				SelectedSourcesTags->Add( FGameAssetDatabase::MakeSystemTag( ESystemTagType::SharedCollection, SelectedSharedCollectionName ) );
// 			}
// 
// 			// Convert all private collection names to collection system tags
// 			for each ( String^ SelectedPrivateCollectionName in CachedQueryData->SelectedPrivateCollectionNames )
// 			{
// 				SelectedSourcesTags->Add( FGameAssetDatabase::MakeSystemTag( ESystemTagType::PrivateCollection, SelectedPrivateCollectionName ) );
// 			}
// 			// Convert all local collection names to collection system tags
// 			for each ( String^ SelectedLocalCollectionName in CachedQueryData->SelectedLocalCollectionNames )
// 			{
// 				SelectedSourcesTags->Add( FGameAssetDatabase::MakeSystemTag( ESystemTagType::LocalCollection, SelectedLocalCollectionName ) );
// 			}
// 
// 			// Convert all package names to package system tags
 			for each (String^ PackageName in CachedQueryData->SelectedOutermostPackageNames )
 			{
 				SelectedSourcesTags->Add( FGameAssetDatabase::MakeSystemTag( ESystemTagType::OutermostPackage, PackageName ) );
 			}

			FGameAssetDatabase& GameAssetDatabase = FGameAssetDatabase::Get();

			// - - Get assets from asset database
			PROFILE_CONTENT_BROWSER( const double GatherAssetsFromGADStartTime = appSeconds() );
			{
				TArray< TArray< FString >* > TagSetList;
				try
				{
					// Add "package" system tags from source panel
					if( SelectedSourcesTags->Count > 0 )
					{
						TArray< FString > *SourcesTagSet = new TArray< FString >();
						CLRTools::ToFStringArray(
							SelectedSourcesTags,
								*SourcesTagSet );		// Out

						TagSetList.AddItem( SourcesTagSet );
					}

					// Query GAD for all assets that are associated with at least one tag in each set
					PROFILE_CONTENT_BROWSER( const double QueryAssetsStartTime = appSeconds() );
					
					// Save the asset FNames from GAD so that we can perform incremental updates.
					LastQueryAssetFullNameFNamesFromGAD.Reset( CachedQueryAssetFullNameFNamesFromGAD.Release() );
					CachedQueryAssetFullNameFNamesFromGAD.Reset( new TLookupMap< FName >() );
					GameAssetDatabase.QueryAssetsWithTagInAllSets( TagSetList, *CachedQueryAssetFullNameFNamesFromGAD.Get() );
					
					PROFILE_CONTENT_BROWSER( debugf( TEXT( "DataBrowser|  QueryAssets took %0.6f seconds" ), appSeconds() - QueryAssetsStartTime ) );

					// Find all asset currently under quarantine.
					List<String^>^ OutQuarantinedFullNames;
					GameAssetDatabase.QueryAssetsWithTag( FGameAssetDatabase::MakeSystemTag(ESystemTagType::Quarantined, "" ), OutQuarantinedFullNames );
					CachedQueryQuarantinedAssets = gcnew UnrealEd::NameSet( OutQuarantinedFullNames );
				}
				finally
				{
					for (int TagSetIdx=0; TagSetIdx < TagSetList.Num(); TagSetIdx++ )
					{
						delete TagSetList(TagSetIdx);
					}
				}
			}
			PROFILE_CONTENT_BROWSER( debugf( TEXT( "DataBrowser|  GatherAssetsFromGAD took %0.6f seconds" ), appSeconds() - GatherAssetsFromGADStartTime ) );
		}

		PROFILE_CONTENT_BROWSER( debugf( TEXT( "DataBrowser|  GatherObjectsAndPackages took %0.6f seconds" ), appSeconds() - GatherObjectsStartTime ) );



		// @debug cb
		//warnf(TEXT("==============UNIQUE FROM GAD===================="));
		//const TArray< FName >& DebugAssetFNames = CachedQueryAssetFullNameFNamesFromGAD->GetUniqueElements();
		//for(int i=0; i<DebugAssetFNames.Num(); ++i)
		//{
		//	warnf( *FString::Printf( TEXT("[%d] %s "), i, *(DebugAssetFNames(i).ToString()) ) ) ;
		//}



		return true;
	}


	
	/** Add currently-loaded assets */
	bool AssetQuery_AddLoadedAssets( double InUpdateStartTime, QueryUpdateMode UpdateModeFlags )
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return false;
		// oscar@snda

		PROFILE_CONTENT_BROWSER( const double AddLoadedAssets_StartTime = appSeconds() );

		// True if this method is being called over multiple ticks, false if it should begin and end within the same stack frame.
		bool bAmortized = (UpdateModeFlags & QueryUpdateMode::Amortizing) == QueryUpdateMode::Amortizing ;

		bool bQuickUpdate = (UpdateModeFlags & QueryUpdateMode::QuickUpdate) == QueryUpdateMode::QuickUpdate ;

		bool bInQuarantineMode = DataBrowserCtrl->IsInQuarantineMode();

		ContentBrowser::DataAssetViewModel^ AssetsInView = DataBrowserCtrl->MyAssets;
		FGameAssetDatabase& GameAssetDatabase = FGameAssetDatabase::Get();

		TArray< UObject* >& LoadedObjects = *CachedQueryObjects.Get();
		
		// Declared early to avoid reallocating memory often in the inner loop
		TArray<FString> MyObjectTagsNative;

		for( ; CachedQueryIteratorIndex < LoadedObjects.Num(); ++CachedQueryIteratorIndex )
		{
			UObject* MyObject = LoadedObjects( CachedQueryIteratorIndex );
			
			// Grab the object path
			const FName AssetFullNameFName( *MyObject->GetFullName() );
			String^ AssetFullName = CLRTools::FNameToString( AssetFullNameFName );
			String^ AssetPath = CLRTools::ToString( MyObject->GetPathName() );

			// Grab the object's name and path by splitting the full path string
			const int DotIndex = AssetPath->LastIndexOf( TCHAR( '.' ) );
			check( DotIndex != -1 );
			String^ AssetPathOnly = AssetPath->Substring( 0, DotIndex );	// Everything before the first dot
			String^ AssetName = AssetPath->Substring( DotIndex + 1 );	// Everything after the first dot
			
			// In case of a quick update we only want to add loaded assets that are missing from the list.
			bool bAddForQuickUpdate = false;
			bool bBecameLoadedForQuickUpdate = false;
			if ( bQuickUpdate )
			{
				// The asset was not loaded before and is now loaded.
				bBecameLoadedForQuickUpdate =
					!LastQueryLoadedAssetFullNames->Contains( AssetFullNameFName ) &&
					CachedQueryLoadedAssetFullNames->Contains( AssetFullNameFName );

				// Became loaded but did not exist before: i.e. it was just created.
				bAddForQuickUpdate = bBecameLoadedForQuickUpdate && !LastQueryAssetFullNameFNamesFromGAD->HasKey( AssetFullNameFName );					
			}

			bool bIsQuarantined = CachedQueryQuarantinedAssets->Contains( AssetFullName );


			// Did the user select the "show all assets"?
			UBOOL bVisibleViaShowAll = 
				CachedQueryData->SelectedPathNames->Count == 0;
//  				CachedQueryData->SelectedPathNames->Count == 0 &&
//  				CachedQueryData->SelectedSharedCollectionNames->Count == 0 &&
//  				CachedQueryData->SelectedPrivateCollectionNames->Count == 0 &&
//  				CachedQueryData->SelectedLocalCollectionNames->Count == 0;

			// !! See doc at definition of SelectedPathNames; it doesn't do what you think
			UBOOL bVisibleViaSelectedPackage = CachedQueryData->SelectedPathNames->Contains( AssetPathOnly );

			UBOOL bVisibleViaSelectedCollection = 
 				!bVisibleViaShowAll &&
				false &&
// 				( CachedQueryData->SelectedSharedCollectionNames->Count > 0 || CachedQueryData->SelectedPrivateCollectionNames->Count > 0 || CachedQueryData->SelectedLocalCollectionNames->Count > 0) && 
 				CachedQueryAssetFullNameFNamesFromGAD->Find( AssetFullNameFName );

			UBOOL bShouldAddAsset =
				( bInQuarantineMode || !bIsQuarantined ) && // We only see quarantined assets in quarantine mode.
				( bVisibleViaShowAll || bVisibleViaSelectedPackage || bVisibleViaSelectedCollection ) && // The asset is visible via one of the asset sources and
				( bBecameLoadedForQuickUpdate || bAddForQuickUpdate || !bQuickUpdate ); // It needs to be added for quick update or we aren't doing a quick update.

			// We should just refresh the asset; it's already in the list but its state is stale.
			UBOOL bShouldRefreshAssetForQuickUpdate = bQuickUpdate && bBecameLoadedForQuickUpdate && !bAddForQuickUpdate;
			
			if( bShouldAddAsset )
			{
				ContentBrowser::DataAssetItem^ NewAssetItem =
					gcnew ContentBrowser::DataAssetItem( DataBrowserCtrl, AssetName, AssetPathOnly );

				// Populate this asset's quarantined status
				NewAssetItem->IsQuarantined = bIsQuarantined;

				// Populate Asset Type
				String^ AssetTypeName = CLRTools::ToString( MyObject->GetClass()->GetName() );
				NewAssetItem->AssetType = AssetTypeName;
				NewAssetItem->IsArchetype = ( MyObject->HasAllFlags( RF_ArchetypeObject ) && !MyObject->IsAPrefabArchetype() && MyObject->IsNeedArchetype() );

				// Get the tags for this asset
				GameAssetDatabase.QueryTagsForAsset(
					AssetFullNameFName,
					CBDefs::ShowSystemTags ? ETagQueryOptions::AllTags : ETagQueryOptions::UserTagsOnly,
					MyObjectTagsNative );
				List<String^>^ MyObjectTags = CLRTools::ToStringArray( MyObjectTagsNative );

				// Sort the tag list alphabetically
				MyObjectTags->Sort();

				NewAssetItem->Tags = MyObjectTags; 

				// This object came straight from the engine so we already know that it's loaded
				if( ( (UPackage*)MyObject->GetOutermost() )->IsDirty() )
				{
					NewAssetItem->LoadedStatus = ContentBrowser::DataAssetItem::LoadedStatusType::LoadedAndModified;
				}
				else
				{
					NewAssetItem->LoadedStatus = ContentBrowser::DataAssetItem::LoadedStatusType::Loaded;
				}

				
				// Loaded assets are always verified since we know they exist for us (locally at least)
				NewAssetItem->IsVerified = true;

				// @todo CB: Slow O(N) lookup here in FindAssetIndex call
				int AssetIndexWhenAlreadyPresent = -1;
				if (bShouldRefreshAssetForQuickUpdate &&
					0 <= ( AssetIndexWhenAlreadyPresent = DataBrowserCtrl->AssetView->FindAssetIndex(NewAssetItem->FullName) ) )
				{
					// We are doing a quick update, and the asset that we just created is already in the list.
					// So, replace it instead of adding a new one.
					DataBrowserCtrl->MyAssets[AssetIndexWhenAlreadyPresent] = NewAssetItem;
				}
				else
				{
					// Add item
					AssetsInView->Add( NewAssetItem );
				}
			}


			// If we are amortizing the addition of assets, check to see if we've already spent too much time
			if( bAmortized && ( CachedQueryIteratorIndex % CBDefs::AssetQueryItemsPerTimerCheck ) == 0 )
			{
				if( ( appSeconds() - InUpdateStartTime ) > CBDefs::MaxAssetQuerySecondsPerTick )
				{
					// Increment the iterator; we're about to bail and the last for-loop clause won't execute.
					++CachedQueryIteratorIndex;
					// Out of time, so bail out; we'll resume our work in the next tick
					break;
				}
			}
		}

		PROFILE_CONTENT_BROWSER( warnf( TEXT( "DataBrowser| AssetQuery_AddLoadedAssets() Add took %0.6f seconds" ), appSeconds() - AddLoadedAssets_StartTime) );

		// Return true only if we've processed all objects
		return ( CachedQueryIteratorIndex >= LoadedObjects.Num() );
	}



	/** Add non-loaded assets (from the game asset database) */
	bool AssetQuery_AddNonLoadedAssets( DOUBLE InUpdateStartTime, QueryUpdateMode UpdateModeFlags )
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return false;
		// oscar@snda

		PROFILE_CONTENT_BROWSER( const double AddNonLoadedAssets_StartTime = appSeconds() );

		// During quick updates we do not touch the nonloaded assets, so we're done immediately
		if ( (UpdateModeFlags & QueryUpdateMode::QuickUpdate) == QueryUpdateMode::QuickUpdate )
		{
			return true;
		}

		bool bAmortized = (UpdateModeFlags & QueryUpdateMode::Amortizing) == QueryUpdateMode::Amortizing;
		
		FGameAssetDatabase& GameAssetDatabase = FGameAssetDatabase::Get();
		ContentBrowser::DataAssetViewModel^ AssetsInView = DataBrowserCtrl->MyAssets;

		const TArray< FName >& AssetFullNameFNames = CachedQueryAssetFullNameFNamesFromGAD->GetUniqueElements();
		const int AssetCount = AssetFullNameFNames.Num();

		bool bInQuarantineMode = DataBrowserCtrl->IsInQuarantineMode();
		
		// Declared early to avoid reallocating memory often in the inner loop
		TArray<FString> MyObjectTagsNative;


		for( ; CachedQueryIteratorIndex < AssetCount; ++CachedQueryIteratorIndex )
		{
			const FName& AssetFullNameFName = AssetFullNameFNames( CachedQueryIteratorIndex );

			// Make sure this asset isn't loaded.  Loaded assets were already added to the asset view so
			// we'll just skip right by them!
			if( !CachedQueryLoadedAssetFullNames->Contains( AssetFullNameFName ) )
			{
				// Split the asset's full name into separate class name and path strings
				String^ AssetFullName = CLRTools::FNameToString( AssetFullNameFName );
				const INT FirstSpaceIndex = AssetFullName->IndexOf( TCHAR( ' ' ) );
				check( FirstSpaceIndex != -1 );
				String^ AssetClassName = AssetFullName->Substring( 0, FirstSpaceIndex );	// Everything before the first space
				String^ AssetPath = AssetFullName->Substring( FirstSpaceIndex + 1 );	// Everything after the first space

				// Grab the object's name and package by splitting the full path string
				const INT LastDotIndex = AssetPath->LastIndexOf( TCHAR( '.' ) );
				const INT FirstDotIndex = AssetPath->IndexOf( TCHAR( '.' ) );
				check( LastDotIndex != -1 );
				String^ AssetPathOnly = AssetPath->Substring( 0, LastDotIndex );	// Everything before the last dot
				String^ AssetName = AssetPath->Substring( LastDotIndex + 1 );	// Everything after the last dot
				String^ PackageName = AssetPath->Substring( 0, FirstDotIndex ); // Everything before the first dot

				bool IsQuarantined = CachedQueryQuarantinedAssets->Contains( AssetFullName );

				// If there are no packages selected the asset automatically passes								 
				UBOOL bHasPackagesSelected = CachedQueryData->SelectedPathNames->Count > 0; // !! See doc at definition of SelectedPathNames; it doesn't do what you think

				UBOOL bShouldShowAsset = !bHasPackagesSelected; 
				
				// If there are packages selected then we have to check if this object is in one of those packages
				if ( bHasPackagesSelected )
				{
					bShouldShowAsset =
						( CachedQueryData->ExplicitlySelectedOutermostPackageNames->Contains( PackageName ) || // The asset's package is selected or ...
						CachedQueryData->SelectedPathNames->Contains( AssetPathOnly ) ); // ... the asset's group is selected.
				}

				bShouldShowAsset = bShouldShowAsset && ( bInQuarantineMode || !IsQuarantined ); // We only see quarantined assets in quarantine mode.
				
				if( bShouldShowAsset )
				{
					// Grab all of the tags associated with this asset, even the system tags
					GameAssetDatabase.QueryTagsForAsset(
						AssetFullNameFName,
						ETagQueryOptions::AllTags,
						MyObjectTagsNative );
					List<String^>^ MyObjectTags = CLRTools::ToStringArray( MyObjectTagsNative );


					bool bIsGhostAsset = false;
					bool bIsUnverifiedAsset = false;
					bool bHasOutermostPackageSystemTag = false;
					bool bHasObjectTypeSystemTag = false;
					bool bIsArchetype = false;
					for( int CurTagIndex = 0; CurTagIndex < MyObjectTags->Count; ++CurTagIndex )
					{
						String^ CurTag = MyObjectTags[ CurTagIndex ];

						ESystemTagType TagType = GameAssetDatabase.GetSystemTagType( CurTag );
						if( TagType != ESystemTagType::Invalid )
						{
							switch( TagType )
							{
								case ESystemTagType::OutermostPackage:
									{
										// @todo CB: Make sure OutermostPackage system tag matches our asset full name's package!
										bHasOutermostPackageSystemTag = true;
									}
									break;

								case ESystemTagType::ObjectType:
									{
										// @todo CB: Make sure ObjectType system tag matches our asset full name's type!
										bHasObjectTypeSystemTag = true;
									}
									break;

								case ESystemTagType::Archetype:
									{
										bIsArchetype = true;
									}
									break;

								case ESystemTagType::Ghost:
									{
										bIsGhostAsset = true;
									}
									break;

								case ESystemTagType::Unverified:
									{
										bIsUnverifiedAsset = true;
									}
									break;

								case ESystemTagType::DateAdded:
									{
										// Ignore for now.  We'll pull this out on demand if we need it.
									}
									break;
							}


							if( !CBDefs::ShowSystemTags )
							{
								// Remove the system tag from the tag list
								MyObjectTags->RemoveAt( CurTagIndex-- );
							}
						}
					}


					// Currently we'll never bother displaying ghost assets to the user.  Also, we won't display
					// assets that are missing a crucial system tag
					if( bHasObjectTypeSystemTag && bHasOutermostPackageSystemTag &&
						( !bIsGhostAsset || CBDefs::ShowSystemTags ) )
					{
						ContentBrowser::DataAssetItem^ NewAssetItem =
							gcnew ContentBrowser::DataAssetItem( DataBrowserCtrl, AssetName, AssetPathOnly );

						// Is this asset Quarantined?
						NewAssetItem->IsQuarantined = IsQuarantined;

						// Set asset type
						NewAssetItem->AssetType = AssetClassName;

						// Is this an archetype?
						NewAssetItem->IsArchetype = bIsArchetype;

						// Sort the tag list alphabetically
						MyObjectTags->Sort();
						NewAssetItem->Tags = MyObjectTags; 

						// These are the objects from the asset database that were not on our loaded objects list
						NewAssetItem->LoadedStatus = ContentBrowser::DataAssetItem::LoadedStatusType::NotLoaded;

						// Mark the asset as either verified or not
						NewAssetItem->IsVerified = !bIsUnverifiedAsset;

						// Add item				
						AssetsInView->Add( NewAssetItem );
					}
				}
			}

			// Check to see if we've already spent too much time
			if( bAmortized && ( CachedQueryIteratorIndex % CBDefs::AssetQueryItemsPerTimerCheck ) == 0 )
			{
				if( ( appSeconds() - InUpdateStartTime ) > CBDefs::MaxAssetQuerySecondsPerTick )
				{
					// Increment the iterator; we're about to bail and the last for-loop clause won't execute.
					++CachedQueryIteratorIndex;
					// Out of time, so bail out; we'll resume our work in the next tick
					break;
				}
			}
		}


		PROFILE_CONTENT_BROWSER( warnf( TEXT( "DataBrowser| AssetQuery_AddNonLoadedAssets() Add took %0.6f seconds" ), appSeconds() - AddNonLoadedAssets_StartTime) );
		// Return true only if we've processed all objects
		return ( CachedQueryIteratorIndex >= AssetCount );
	}


	/** Remove assets that used to be present and are now missing */
	bool AssetQuery_RemoveMissingAssets( QueryUpdateMode UpdateModeFlags )
	{
		PROFILE_CONTENT_BROWSER( const double RemoveMissingAssets_StartTime = appSeconds() );

		// During quick updates we do not touch the nonloaded assets, so we're done immediately
		check( ( UpdateModeFlags & QueryUpdateMode::QuickUpdate ) == QueryUpdateMode::QuickUpdate );

		// Remove Missing Assets currently does not support amortizing.
		check( ( UpdateModeFlags & QueryUpdateMode::Amortizing ) == QueryUpdateMode::None );

		ContentBrowser::DataAssetViewModel^ AssetItems = DataBrowserCtrl->MyAssets;
		for ( int AssetIndex=0; AssetIndex < AssetItems->Count; )
		{
			ContentBrowser::DataAssetItem^ CurAssetItem = AssetItems[AssetIndex];
			FName CurAssetFullNameFName( CLRTools::ToFName( CurAssetItem->FullName ) );
			if( !CachedQueryLoadedAssetFullNames->Contains( CurAssetFullNameFName ) &&
				!CachedQueryAssetFullNameFNamesFromGAD->HasKey( CurAssetFullNameFName ) )
			{
				// This item was not found in the gad or in the loaded assets; remove it from the view.
				AssetItems->RemoveAt( AssetIndex );
			}
			else
			{
				// We didn't remove an item, so go to the next one.
				++AssetIndex;
			}
			
		}


		PROFILE_CONTENT_BROWSER( warnf( TEXT( "DataBrowser| AssetQuery_RemoveMissingAssets() Add took %0.6f seconds" ), appSeconds() - RemoveMissingAssets_StartTime) );
		
		return true;
	}

#if HAVE_SCC
	/**
	 * Helper functions to determine the results of a status update for a package
	 * @param pkg - The content browser package to receive the new state information
	 * @param InNewState - The new source control state (checked out, locked, etc)
	 */
	void SetSourceControlState(ContentBrowser::Package^ pkg, ESourceControlState InNewState);
	/**
	 * Helper functions to determine the results of a status update for a package
	 * @param pkg - The content browser package to receive the new state information
	 * @param InResultsMap - String pairs generated from source control
	 */
	void SetSourceControlState(ContentBrowser::Package^ pkg, const TMap<FString, FString>& InResultsMap);

	/**
	 * Callback when a command is done executing
	 * @param InCommand - The Command passed from source control subsystem that has completed it's work
	 */
	virtual void SourceControlResultsProcess(FSourceControlCommand* InCommand);
#endif

protected:

	void ConditionalUpdateSCC( TArray<UPackage*>* PackageList );

	/**
	 * If the corresponding flags are set, triggers a rebuild of the package list, an update of the package list visual state,
	 * or both.
	 */
	void ConditionalUpdatePackages();
	void ConditionalUpdateCollections();
	void ConditionalUpdateAssetView();
	void ConditionalUpdatePackageFilter();

	/**
	 * Filters the global set of packages.
	 *
	 * @param	ObjectTypes					List of allowed object types (or NULL for all types)
	 * @param	OutGroupPackages			The map that receives the filtered list of group packages.
	 * @param	OutPackageList				The array that will contain the list of filtered packages.
	 */
	void GetFilteredPackageList( const ClassToGBTMap* ObjectTypes,
								 TSet<const UPackage*> &OutFilteredPackageMap,
								 TSet<UPackage*>* OutGroupPackages,
								 TArray<UPackage*> &OutPackageList )
	{
		PackageTools::GetFilteredPackageList(ObjectTypes, OutFilteredPackageMap, OutGroupPackages, OutPackageList);
	}


	/**
	 * Remove all tree items which should not be in the tree, such as tree items packages that shouldn't be displayed
	 * (shader-cache, script files, transient package, etc.)
	 *
	 * @param	PackageNamesOnDisk		file pathnames for all packages listed in the package file cache
	 * @param	LoadedPackages			all top-level packages currently in memory
	 * @param	LoadedGroups			all groups currently in memory
	 */
	void PurgeInvalidNodesFromTree( const TArray<FString>& PackageNamesOnDisk, TArray<UPackage*>& LoadedPackages, const TSet<UPackage*>& LoadedGroups )
	{
		// The sources ViewModel that we are going to populate
		ContentBrowser::PackagePanelModel^ Sources = DataBrowserCtrl->MySources;

		// PackageNamesOnDisk
		// - script files
		// - trashcan
		// - shadercache
		for ( INT PkgIndex = 0; PkgIndex < PackageNamesOnDisk.Num(); PkgIndex++ )
		{
			FFilename PkgFilename = PackageNamesOnDisk(PkgIndex);
			String^ NormalizedPathName = Sources->NormalizePackageFilePathName(CLRTools::ToString(PkgFilename.GetBaseFilename(FALSE)));

			if ( !UnrealEd::Utils::IsPackageValidForTree(NormalizedPathName) )
			{
				ContentBrowser::SourceTreeNode^ node = Sources->FindDescendantNode(NormalizedPathName);
				if ( node != nullptr )
				{
					if (node->GetType() == ContentBrowser::Package::typeid
					||	node->GetType() == ContentBrowser::GroupPackage::typeid)
					{
						Sources->RemovePackage((ContentBrowser::ObjectContainerNode^)node);
					}
				}
			}
		}

		// LoadedGroups
		// - same as loaded packages, so just add their outermost to the same array 
		for ( TSet<UPackage*>::TConstIterator It(LoadedGroups); It; ++It )
		{
			UPackage* Group = *It;
			UPackage* GroupOutermost = Group->GetOutermost();
#if _DEBUG
			String^ GroupName = CLRTools::ToString(Group->GetPathName());
			String^ PkgName = CLRTools::ToString(GroupOutermost->GetName());
#endif
			LoadedPackages.AddUniqueItem(GroupOutermost);
		}

		// loaded packages
		// - rf_transient
		// - UObject::GObjTransientPkg
		// - PKG_ContainsScript|PKG_Trash|PKG_PlayInEditor
		for ( INT PkgIndex = 0; PkgIndex < LoadedPackages.Num(); PkgIndex++ )
		{
			UPackage* UPkg = LoadedPackages(PkgIndex);
#if _DEBUG
			String^ PkgName = CLRTools::ToString(UPkg->GetName());
#endif
			if ( !CLRTools::IsPackageValidForTree(UPkg) )
			{
				ContentBrowser::ObjectContainerNode^ PackageItem = UPackageToPackage(UPkg);
				if ( PackageItem != nullptr )
				{
					Sources->RemovePackage(PackageItem);
				}
			}
		}
	}

public:
	/**
	 * Update the status of all packages in the tree.
	 */
	virtual void UpdatePackagesTreeUI()
	{
		List< ContentBrowser::ObjectContainerNode^ > PackagesList;
		DataBrowserCtrl->MySources->GetChildNodes< ContentBrowser::ObjectContainerNode^ >(%PackagesList);

		for each ( ContentBrowser::ObjectContainerNode^ Pkg in PackagesList )
		{
			UpdatePackagesTreeUI(Pkg);
		}
	}

	/**
	 * Updates the status of the specified package
	 *
	 * @param	Pkg		Package to update
	 */
	virtual void UpdatePackagesTreeUI( ContentBrowser::ObjectContainerNode^ Pkg )
	{
		// Grab the package from the engine
		UpdatePackagesTreeUI(Pkg, PackageToUPackage(Pkg));
	}

	/** Notifies an instance of the Content Browser that it is now responsible for the current selection */
	void OnBecameSelectionAuthority()
	{
		SyncSelectedObjectsWithGlobalSelectionSet();
		DataBrowserCtrl->AssetView->AssetCanvas->SetSelectionAppearsAuthoritative(true);
	}

	/** Notify an instance of the Content Browser that it is giving up being the selection authority */
	void OnYieldSelectionAuthority()
	{
		DataBrowserCtrl->AssetView->AssetCanvas->SetSelectionAppearsAuthoritative(false);
	}

protected:

	/** @return true if we can perform a quick update; false if a full update is our only option */
	bool IsCapableOfQuickUpdate()
	{
		return CachedQueryAssetFullNameFNamesFromGAD.IsValid() && LastQueryAssetFullNameFNamesFromGAD.IsValid();
	}


	void UpdatePackagesTreeUI( UPackage* UnrealPackage )
	{
		UpdatePackagesTreeUI(UPackageToPackage(UnrealPackage), UnrealPackage);
	}

	/**
	 * Updates the loading and modification status of the specified package
	 *
	 * @param	PackageItem		the C# representation of the specified UPackage
	 * @param	UnrealPackage	the UPackage corresponding to the specified DataBrowser::Package
	 */
	void UpdatePackagesTreeUI( ContentBrowser::ObjectContainerNode^ PackageItem, UPackage* UnrealPackage )
	{
		// We will need to refresh the filter next tick.
		bPackageFilterUpdateRequested = true;

		if( PackageItem != nullptr )
		{
			if ( UnrealPackage != NULL )
			{
				if( UnrealPackage->GetOutermost()->IsFullyLoaded() )
				{
					PackageItem->Status = ContentBrowser::ObjectContainerNode::PackageStatus::FullyLoaded;
				}
				else
				{
					PackageItem->Status = ContentBrowser::ObjectContainerNode::PackageStatus::PartiallyLoaded;
				}

				// apply the UPackage's dirty state to the IsModified var
				// for now, only allow top-level packages to be considered modified.
				if ( PackageItem->GetType() == ContentBrowser::Package::typeid )
				{
					ContentBrowser::Package^ Pkg = (ContentBrowser::Package^)PackageItem;
					Pkg->IsModified = UnrealPackage->GetOuter() == NULL && UnrealPackage->IsDirty();
				}

				if ( CallbackEventPackages )
				{
					CallbackEventPackages->RemoveItem(UnrealPackage);
				}

				if ( UnrealPackage->GetOuter() == NULL )
				{
					UpdatePackageSCCState(PackageItem, UnrealPackage);
				}
			}
			else
			{
				// see if the package is in the file cache, which implies it exists on disk
				FString PackageFilename;
				if ( GPackageFileCache->FindPackageFile(*CLRTools::ToFString(PackageItem->Name), NULL, PackageFilename) )
				{
					PackageItem->Status = ContentBrowser::ObjectContainerNode::PackageStatus::NotLoaded;
					if ( PackageItem->GetType() == ContentBrowser::Package::typeid )
					{
						ContentBrowser::Package^ Pkg = (ContentBrowser::Package^)PackageItem;
						Pkg->IsModified = false;
					}
				}
				else
				{
					// Unloaded groups from the GAD that exist in loaded packages are OK, we'll just skip those
					const bool bIsGroupPackage = dynamic_cast< ContentBrowser::GroupPackage^ >( PackageItem ) != nullptr;
					if( !bIsGroupPackage )
					{
						// not on disk either - package was garbage collected, so remove it.
						DataBrowserCtrl->MySources->RemovePackage(PackageItem);
					}
				}
			}
		}
	}

	void UpdatePackageSCCState( const TArray<UPackage*>& UnrealPackages )
	{
		for ( INT PkgIndex = 0; PkgIndex < UnrealPackages.Num(); PkgIndex++ )
		{
			UPackage* Pkg = UnrealPackages(PkgIndex);
			UpdatePackageSCCState(UPackageToPackage(Pkg), Pkg);
		}
	}

	/**
	 * Updates the scc status of the specified package
	 *
	 * @param	PackageItem		the C# representation of the specified UPackage
	 * @param	UnrealPackage	the UPackage corresponding to the specified DataBrowser::Package
	 */
	void UpdatePackageSCCState( ContentBrowser::ObjectContainerNode^ PackageItem, UPackage* UnrealPackage )
	{
		if ( PackageItem != nullptr)
		{
			// apply the UPackage's current SCC state to the Package::NodeIcon property - this will trigger an event which will
			// result in the icon for the Package in the tree view to be updated to reflect the new state
			//@todo ronp - move this to a function?
			ESourceControlState PackageSCCState = (ESourceControlState)GPackageFileCache->GetSourceControlState(*CLRTools::ToFString(PackageItem->Name));
			UpdatePackageSCCState(PackageItem, PackageSCCState);
		}
	}
	void UpdatePackageSCCState( ContentBrowser::ObjectContainerNode^ PackageItem, ESourceControlState PackageSCCState )
	{
#if !SHIPPING_PC_GAME
		if ( PackageItem != nullptr )
		{
			PackageItem->ToolTipEnabled = false;
			switch ( PackageSCCState )
			{
			case SCC_DontCare:
				PackageItem->NodeIcon = ContentBrowser::ObjectContainerNode::TreeNodeIconType::ICON_Unknown;
				break;

			case SCC_ReadOnly:
				PackageItem->NodeIcon = ContentBrowser::ObjectContainerNode::TreeNodeIconType::ICON_CheckedIn;
				break;

			case SCC_CheckedOut:
				PackageItem->NodeIcon = ContentBrowser::ObjectContainerNode::TreeNodeIconType::ICON_CheckedOut;
				break;

			case SCC_NotCurrent:
				PackageItem->NodeIcon = ContentBrowser::ObjectContainerNode::TreeNodeIconType::ICON_NotCurrent;
				break;

			case SCC_NotInDepot:
				PackageItem->NodeIcon = ContentBrowser::ObjectContainerNode::TreeNodeIconType::ICON_NotInDepot;
				break;

			case SCC_CheckedOutOther:
				PackageItem->NodeIcon = ContentBrowser::ObjectContainerNode::TreeNodeIconType::ICON_CheckedOutOther;
				PackageItem->ToolTipLabel = CLRTools::ToString(FSourceControl::CheckOutUserName);
				PackageItem->ToolTipEnabled = true;
				break;

			case SCC_Ignore:
			default:
				PackageItem->NodeIcon = ContentBrowser::ObjectContainerNode::TreeNodeIconType::ICON_Ignore;
				break;
			}
		}
#endif
	}

	/** 
	 * Refresh the collections list in the sources panel
	 */
	void UpdateCollectionsList()
	{
		// oscar@snda
		if(!FGameAssetDatabase::IsInitialized())
			return;
		// oscar@snda

		// The sources ViewModel that we are going to populate
		ContentBrowser::PackagePanelModel^ Sources = DataBrowserCtrl->MySources;
				
		FGameAssetDatabase &Gad = FGameAssetDatabase::Get();
		
		// Grab all system tags
		Generic::List<String^>^ AllCollectionTags;
		Gad.QueryAllTags( AllCollectionTags, ETagQueryOptions::CollectionsOnly );

		Generic::List<String^>^ SharedCollectionNames = gcnew Generic::List<String^>();
		Generic::List<String^>^ PrivateCollectionNames = gcnew Generic::List<String^>();
		Generic::List<String^>^ LocalCollectionNames = gcnew Generic::List<String^>();

		for each(String^ Tag in AllCollectionTags)
		{
			if ( FGameAssetDatabase::IsCollectionTag(Tag, EGADCollection::Shared) )
			{
				SharedCollectionNames->Add( FGameAssetDatabase::GetSystemTagValue(Tag) );
			}
			else if ( FGameAssetDatabase::IsCollectionTag(Tag, EGADCollection::Private) )
			{
				// We have a private collection, but we need to make sure it belongs to the current user!
				if( FGameAssetDatabase::IsMyPrivateCollection( Tag ) )
				{
					PrivateCollectionNames->Add( FGameAssetDatabase::GetSystemTagValue(Tag) );
				}
			}
			else if( FGameAssetDatabase::IsCollectionTag(Tag, EGADCollection::Local ) )
			{
				String^ SystemTagValue = FGameAssetDatabase::GetSystemTagValue(Tag);
				LocalCollectionNames->Add( SystemTagValue );
				PrivateCollectionNames->Add( SystemTagValue );
			}
		}

		Sources->SetCollectionNames( SharedCollectionNames, PrivateCollectionNames, LocalCollectionNames );
		
		UpdateCollectionsListUI();
	}

	/**
	 * Refresh the collection list UI; currently does nothing, but left around for ease of future implementations, if required
	 */
	void UpdateCollectionsListUI()
	{
	}


	/** Called when the HwndSource receives a windows message */
	IntPtr MessageHookFunction( IntPtr HWnd, int Msg, IntPtr WParam, IntPtr LParam, bool% OutHandled )
	{
		IntPtr Result = (IntPtr)0;
		OutHandled = false;

		if( Msg == WM_GETDLGCODE )
		{
			OutHandled = true;

			// This tells Windows (and Wx) that we'll need keyboard events for this control
			Result = IntPtr( DLGC_WANTALLKEYS );
		}


		// Tablet PC software (e.g. Tablet PC Input Panel) sends WM_GETOBJECT events which
		// trigger a massive performance bug in WPF (http://wpf.codeplex.com/Thread/View.aspx?ThreadId=41964)
		// We'll intercept this message and skip it.
		if( Msg == WM_GETOBJECT )
		{
			OutHandled = true;
			Result = (IntPtr)0;
		}

		return Result;
	}


	/** Sets up the list of Unreal object types that we're able to display */
	void InitBrowsableObjectTypeList();

	/**
	 * Initialize the list of UFactory classes which are valid for populating the "New Object" context menu.
	 */
	void InitFactoryClassList();

	/** Parse the recently accessed browser objects from config file */
	void InitRecentObjectsList();

	/** If the recent items list is initialized */
	static bool RecentItemsInitialized = false;

	/** Opens type-sensitive editor for the selected objects */
	void OpenObjectEditorForSelectedObjects()
	{
		for each( ContentBrowser::DataAssetItem^ CurSelectedAsset in DataBrowserCtrl->AssetView->AssetListView->SelectedItems )
		{
			UObject* FoundObject = NULL;
			UClass* AssetClass = FindObject<UClass>( ANY_PACKAGE, *CLRTools::ToFString( CurSelectedAsset->AssetType ) );
			if( AssetClass != NULL )
			{
				FoundObject = UObject::StaticFindObject( AssetClass, ANY_PACKAGE, *CLRTools::ToFString( CurSelectedAsset->FullyQualifiedPath ) );
			}
			if( FoundObject != NULL )
			{
				// NOTE: This list is ordered such that child object types appear first
				const TArray< UGenericBrowserType* >* BrowsableObjectTypes =
					ClassToBrowsableObjectTypeMap->Find( FoundObject->GetClass() );
				if( BrowsableObjectTypes != NULL )
				{
					for( INT CurBrowsableTypeIndex = 0; CurBrowsableTypeIndex < BrowsableObjectTypes->Num(); ++CurBrowsableTypeIndex )
					{
						UGenericBrowserType* CurObjType = ( *BrowsableObjectTypes )( CurBrowsableTypeIndex );
						if( CurObjType->Supports( FoundObject ) )
						{
							// Opens an editor for all selected objects of the specified type
							// @todo CB: Warn if opening editors for more than 5 objects or so (See GB's check for this)
							UBOOL bShowedAtLeastOneEditor = CurObjType->ShowObjectEditor( FoundObject );
							if( bShowedAtLeastOneEditor )
							{
								break;
							}
						}
					}
				}
			}
		}
	}



	/** Opens property editor for the selected objects */
	void OpenPropertiesForSelectedObjects()
	{
		// Grab selected objects
		TArray< UObject* > SelectedObjects;
		GetSelectedObjects( SelectedObjects );

		if ( SelectedObjects.Num() > 0 )
		{
			
// 			// Always create a new property window because these properties are not
// 			// lockable like Actor properties are; if we always reuse existing window we cannot
// 			// compare properties.
// 			WxPropertyWindowFrame* PropertiesWindow = new WxPropertyWindowFrame();
// 			PropertiesWindow->Create( GApp->EditorFrame, -1, GUnrealEd );
// 			PropertiesWindow->AllowClose();
// 
// 			// Setup the property window
// 			PropertiesWindow->SetObjectArray(SelectedObjects, EPropertyWindowFlags::Sorted | EPropertyWindowFlags::ShouldShowCategories );
 			GetParentBrowserWindow()->GetParentBrowserWindow()->GetPropertyWindowHost()->SetObjectArray(SelectedObjects, EPropertyWindowFlags::Sorted | EPropertyWindowFlags::ShouldShowCategories );
// 			// Display the properties!
// 			PropertiesWindow->Show();
		}
	}



	/** Gathers selected objects into an array */
	void GetSelectedObjects( TArray< UObject* >& OutObjects )
	{
		OutObjects.Reset();
		for each( ContentBrowser::DataAssetItem^ CurSelectedAsset in DataBrowserCtrl->AssetView->AssetListView->SelectedItems )
		{
			UObject* FoundObject = NULL;
			UClass* AssetClass = FindObject<UClass>( ANY_PACKAGE, *CLRTools::ToFString( CurSelectedAsset->AssetType ) );
			if( AssetClass != NULL )
			{
				FoundObject = UObject::StaticFindObject( AssetClass, ANY_PACKAGE, *CLRTools::ToFString( CurSelectedAsset->FullyQualifiedPath ) );
			}
			if( FoundObject != NULL )
			{
				OutObjects.AddItem( FoundObject );
			}
		}
	}

	/**
	 * Given a proposed default command and an array of supported commands, determine which command should actually be executed by default if the proposed command is
	 *	not explicitly specified.
	 */
	INT DetermineDefaultCommand( INT InProposedDefaultCommand, TArray<FObjectSupportedCommandType>& SupportedCommands )
	{
		INT FinalCommandCode = InProposedDefaultCommand;
		if ( FinalCommandCode == INDEX_NONE )
		{			
			// There GenericBrowserType does not specify a default. In this case we want to open the editor (if possible).
			for( int CurCommandIndex = 0; FinalCommandCode == INDEX_NONE && CurCommandIndex < SupportedCommands.Num(); ++CurCommandIndex )
			{
				const FObjectSupportedCommandType& CurCommand = SupportedCommands( CurCommandIndex );
				if ( CurCommand.CommandID == IDMN_ObjectContext_Editor )
				{
					FinalCommandCode = IDMN_ObjectContext_Editor;
				}
			}

			// This object type does not support editing, so return the "display properties" command.
			if (FinalCommandCode == INDEX_NONE)
			{
				FinalCommandCode  = IDMN_ObjectContext_Properties;
			}
		}

		return FinalCommandCode;

	}

	/** Called by the asset view or package tree context menu when a custom object command is selected */
	void OnCustomObjectCommand( Object^ Sender, Input::ExecutedRoutedEventArgs^ EventArgs )
	{
		//no one else should ever handle this event.
		EventArgs->Handled = true;

		INT CommandID = (INT)EventArgs->Parameter;
		ExecuteCustomObjectCommand( CommandID );
	}	
	
	/** Execute the command specified by InCommandId on the currently selected assets. */
	void ExecuteCustomObjectCommand( INT InCommandId )
	{
#if !SHIPPING_PC_GAME
		// Make sure all selected objects are loaded, where possible
		LoadSelectedObjectsIfNeeded();
		

		// Certain commands are shared by many types and handled specially
		switch( InCommandId )
		{
			case IDMN_ObjectContext_Editor:
				{
					// Opens an editor for all selected objects of the specified type
					OpenObjectEditorForSelectedObjects();
				}
				break;


			case IDMN_ObjectContext_Properties:
				{
					// Displays properties for the selected objects
					OpenPropertiesForSelectedObjects();
					//OpenObjectEditorForSelectedObjects();
				}
				break;


			case IDMN_ObjectContext_DuplicateWithRefs:
				{
					TArray< UObject* > SelectedObjects;
					GetSelectedObjects( SelectedObjects );

					ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ Packages = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();
					ContentBrowser::SourceTreeNode^ Node = Packages[0];
					FString ObjType = "MMOAbility";
					if(Node->GetType() == ContentBrowser::Folder::typeid)
						GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString( Node->Name), ObjType);
					else
					{
						ContentBrowser::SourceTreeNode^ ParentNode = (ContentBrowser::SourceTreeNode^)(((CustomControls::AbstractTreeNode^)Node)->Parent);
						GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString( ParentNode->Name), ObjType);
					}
					

					const FString DialogTitle( LocalizeUnrealEd("DuplicateWithReferences") );
					WxDlgMoveAssets MoveDialog;
					UObject* Object = SelectedObjects(0);
					FString PreviousPackage;
					FString PreviousGroup;
					FString PreviousName;

					TArray<UPackage*> PackagesUserRefusedToFullyLoad;
					TArray<UPackage*> OutermostPackagesToSave;
					const FString DlgPackage = PreviousPackage.Len() ? PreviousPackage : Object->GetOutermost()->GetName();
					FString DlgGroup = PreviousGroup.Len() ? PreviousGroup : (Object->GetOuter()->GetOuter() ? Object->GetFullGroupName( 1 ) : TEXT(""));

					const int MoveDialogResult = MoveDialog.ShowModal( DlgPackage, DlgGroup, Object->GetName() , TRUE );
					// Abort if the user cancelled.
					if ( MoveDialogResult != wxID_OK && MoveDialogResult != ID_OK_ALL)
					{
						return;
					}
					PreviousPackage = MoveDialog.GetNewPackage();
					PreviousGroup = MoveDialog.GetNewGroup();
					MoveDialog.GetNewName( PreviousName );

					FullyLoadPackage(CLRTools::ToString(PreviousPackage));

					UPackage* ResultPackage = NULL;
					UPackage* Group = NULL;

					ResultPackage = FindObject<UPackage>(NULL, *PreviousPackage);

					FString LastSaveDirectory;
					FString LastSaveFile;
					if(!ResultPackage)
					{
						ResultPackage = GEngine->CreatePackage(NULL,*PreviousPackage);

						const FString File = FString::Printf( TEXT("%s.upk"), *PreviousPackage );
						//debugf(TEXT("NO_EXISTING: %s, %s, %s"), *ExistingFile, *File, *LastSaveDirectory);
						// Present the user with a 'save file' dialog.
						FString FileTypes( TEXT("All Files|*.*") );
						for(INT i=0; i<GSys->Extensions.Num(); i++)
						{
							FileTypes += FString::Printf( TEXT("|(*.%s)|*.%s"), *GSys->Extensions(i), *GSys->Extensions(i) );
						}

						WxFileDialog SaveFileDialog(GApp->EditorFrame, 
							*LocalizeUnrealEd("SavePackage"), 
							*LastSaveDirectory,
							*File,
							*FileTypes,
							wxSAVE,
							wxDefaultPosition);

						if( SaveFileDialog.ShowModal() == wxID_OK )
						{
							LastSaveDirectory = SaveFileDialog.GetDirectory().c_str();
							LastSaveFile = LastSaveDirectory + "\\"+PreviousPackage + ".upk";

							debugf(TEXT("saving as %s"), *LastSaveFile);
							if( GFileManager->IsReadOnly( *LastSaveFile ) )
							{
								appMsgf( AMT_OK, *FString::Printf( LocalizeSecure(LocalizeUnrealEd("Error_CouldntWriteToFile_F"), *LastSaveFile)) );
							}
							else if( !GUnrealEd->Exec( *FString::Printf(TEXT("OBJ SAVEPACKAGE PACKAGE=\"%s\" FILE=\"%s\""), *PreviousPackage, *LastSaveFile) ) )
							{
								// Couldn't save.
								appMsgf( AMT_OK, *LocalizeUnrealEd("Error_CouldntSavePackage") );
							}
							else
							{
							}

						}
						else
							return;


					}
					else
					{
						const FString PName = ResultPackage->GetName();
						const FGuid PGuid = ResultPackage->GetGuid();
						FString OutFilename;
						GPackageFileCache->FindPackageFile(*PName, &PGuid, OutFilename);
						LastSaveFile = OutFilename;
					}

					if(!ResultPackage)
						return;

					ResultPackage = GEngine->CreatePackage(NULL,*PreviousPackage);
					if( PreviousGroup.Len() )
					{
						// first try loading the group, in case it already exists
						Group = LoadObject<UPackage>(ResultPackage, *PreviousGroup, NULL, LOAD_NoWarn|LOAD_FindIfFail,NULL);
						if ( Group == NULL )
						{
							Group = GEngine->CreatePackage(ResultPackage,*PreviousGroup);
						}	
					}

					ResultPackage->SetDirtyFlag(TRUE);

					if ( Group != NULL )
					{
						ResultPackage = Group;
					}
					UObject* DupObject = GEditor->StaticDuplicateObject( Object, Object, ResultPackage, *PreviousName );
//					UObject* DupObject = GEditor->StaticDuplicateObject( Object, Object, UObject::CreatePackage(NULL,*PreviousPackage), *PreviousName );
					

					FString ErrTxt;
					FEditorFileUtils::IsFilenameValidForSaving( LastSaveFile, ErrTxt );

					GWarn->StatusUpdatef( 0, 1, *FString::Printf( LocalizeSecure(LocalizeUnrealEd("SavingPackagef"), *PreviousPackage )) );
					const FScopedBusyCursor BusyCursor;
					FCheckoutResult sccResult = FSourceControl::TryCheckOut(LastSaveFile);
					GPackageFileCache->CacheDataPath(*LastSaveDirectory, *ObjType);
					FullyLoadPackage(CLRTools::ToString(LastSaveFile));

					sccResult.TryAddToDefaultChangelist();

					GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_UpdatePackageListUI));






//					UObject* RefObject = SelectedObjects(0);
//					UPackage* ObjPackage = Cast<UPackage>(RefObject->GetOutermost());
//
//#if HAVE_SCC
//
//					if(!FSourceControl::ConnectToSource())
//						return;
//
//
//					WxDlgCopyObject dlg;
//					TArray<FString> PackageFiles;// = GPackageFileCache->GetDataFileList();
//
//					FString Path;
//					ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ Packages = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();
//					//for ( INT PackageIndex = 1; PackageIndex < Packages->Count; PackageIndex++ )
//					//{
//					//	ContentBrowser::SourceTreeNode^ Node = Packages[PackageIndex];
//					//	if ( Node->GetType() == ContentBrowser::Folder::typeid)
//					//	{
//
//
//					//		FString name = CLRTools::ToFString( Node->Name );
//					//		//name.ReplaceInline( TEXT( "." ), TEXT( "_" ));
//					//		PackageFiles.AddItem(*name);
//					//	}
//
//					//}
//					ContentBrowser::SourceTreeNode^ Node = Packages[0];
//					FString RootPath;
//					FString RootPathName;
//					FString FolderName, GroupName, PackageName, ObjName;
//					if(Node->GetType() == ContentBrowser::Folder::typeid)
//						GPackageFileCache->FindDataRoot(*CLRTools::ToFString(Node->Name), RootPath);
//					else
//					{
//						ContentBrowser::SourceTreeNode^ ParentNode = (ContentBrowser::SourceTreeNode^)(((CustomControls::AbstractTreeNode^)Node)->Parent);
//						GPackageFileCache->FindDataRoot(*CLRTools::ToFString(ParentNode->Name), RootPath);
//					}
//
//					const FString PName = ObjPackage->GetName();
//					const FGuid PGuid = ObjPackage->GetGuid();
//					FString OutFilename;
//					GPackageFileCache->FindPackageFile(*PName, &PGuid, OutFilename);
//
//					INT Pos = RootPath.InStr(TEXT("\\"), TRUE);
//					if (Pos != INDEX_NONE)
//					{
//						RootPathName = RootPath.Mid(Pos+1);
//						Pos = OutFilename.InStr(RootPathName, TRUE);
//						if (Pos != INDEX_NONE)
//						{
//							FolderName = OutFilename.Mid(Pos+RootPathName.Len()+1);
//						}
//						Pos = FolderName.InStr(TEXT("\\"), TRUE);
//						if (Pos != INDEX_NONE)
//						{
//							FolderName = FolderName.Left(Pos);
//						}
//					}
//
//
//					FName *PackagePath = UObject::GetPackageNameToFileMapping()->Find(FName(*ObjPackage->GetName()));
//
//
//
//					Text::StringBuilder^ NodeTreeViewPath = gcnew Text::StringBuilder(CLRTools::ToString(appBaseDir()));
//					//NodeTreeViewPath->Append("..\\..");
//					NodeTreeViewPath->Append(CLRTools::ToString(RootPath));
//					//NodeTreeViewPath->Append(Node->FullTreeviewPath->Replace(ContentBrowser::SourceTreeNode::TreeViewNodeSeparator, System::IO::Path::DirectorySeparatorChar));
//					Path = CLRTools::ToFString( NodeTreeViewPath->ToString() );
//					FString ObjType = "MMOAbility";
//					GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString( Node->Name), ObjType);
//
//					if(FolderName.Len() > 0)
//						dlg.SetForder(FolderName);
//					if(ObjPackage)
//						dlg.SetPackage(ObjPackage->GetName());
//					
//					int Result = dlg.ShowModal(FolderName,GroupName,PackageName);
//					if ( Result == wxID_CANCEL )
//						return;
//
//					FolderName = dlg.GetForder();
//					PackageName = dlg.GetPackage();
//					GroupName = dlg.GetGroup();
//					ObjName = dlg.GetObjectName();
//
//					if ( FolderName.Len() > 0 && PackageName.Len() > 0 )
//					{
//						FString name = PackageName;
//						// create or load the package specified
//						UPackage* Package = GEngine->CreatePackage(NULL,*name);
//						UPackage* Group = NULL;
//						if( GroupName.Len() )
//						{
//							// first try loading the group, in case it already exists
//							Group = LoadObject<UPackage>(Package, *GroupName, NULL, LOAD_NoWarn|LOAD_FindIfFail,NULL);
//							if ( Group == NULL )
//							{
//								Group = GEngine->CreatePackage(Package,*GroupName);
//							}	
//						}
//
//						Package->SetDirtyFlag(TRUE);
//
//						// Group will be used as the archetype's outer, so if the group is invalid, set it to the package itself
//						if ( Group == NULL )
//						{
//							Group = Package;
//						}
//
//						FString SaveFileName = Path + "\\"+  FolderName+"\\"+PackageName +".upk";
//
//						UObject* DupObject = GEditor->StaticDuplicateObject( RefObject, RefObject, Package, *ObjName );
//
//
//						FString ErrTxt;
//						FEditorFileUtils::IsFilenameValidForSaving( SaveFileName, ErrTxt );
//
//						GWarn->StatusUpdatef( 0, 1, *FString::Printf( LocalizeSecure(LocalizeUnrealEd("SavingPackagef"), *PackageName )) );
//						const FScopedBusyCursor BusyCursor;
//						debugf(TEXT("saving as %s"), *SaveFileName);
//						if( GFileManager->IsReadOnly( *SaveFileName ) )
//						{
//							appMsgf( AMT_OK, *FString::Printf( LocalizeSecure(LocalizeUnrealEd("Error_CouldntWriteToFile_F"), *SaveFileName)) );
//						}
//						else if( !GUnrealEd->Exec( *FString::Printf(TEXT("OBJ SAVEPACKAGE PACKAGE=\"%s\" FILE=\"%s\""), *PackageName, *SaveFileName) ) )
//						{
//							// Couldn't save.
//							appMsgf( AMT_OK, *LocalizeUnrealEd("Error_CouldntSavePackage") );
//						}
//						else
//						{
//						}
//						FCheckoutResult sccResult = FSourceControl::TryCheckOut(SaveFileName);
//
//						FString SaveFilePath = Path + "\\"+  FolderName;
//						GPackageFileCache->CacheDataPath(*SaveFilePath, *ObjType);
//						FullyLoadPackage(CLRTools::ToString(SaveFileName));
//
//						sccResult.TryAddToDefaultChangelist();
//
//						GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_UpdatePackageListUI));
//					}
//
//					
//
//#endif
					// Copy the specified objects
					//ObjectTools::DuplicateWithRefs( SelectedObjects, ClassToBrowsableObjectTypeMap.Get() );
					//GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_UpdatePackageListUI));
				}
				break;


			case IDMN_ObjectContext_RenameWithRefs:
				{
					TArray< UObject* > SelectedObjects;
					GetSelectedObjects( SelectedObjects );

					// Move/Rename the selected objects
					const UBOOL bIncludeLoc = FALSE;
					ObjectTools::RenameObjectsWithRefs( SelectedObjects, bIncludeLoc, ClassToBrowsableObjectTypeMap.Get() );
				}
				break;


			case IDMN_ObjectContext_RenameLocWithRefs:
				{
					TArray< UObject* > SelectedObjects;
					GetSelectedObjects( SelectedObjects );

					// Move/Rename the selected objects (with loc)
					const UBOOL bIncludeLoc = TRUE;
					ObjectTools::RenameObjectsWithRefs( SelectedObjects, bIncludeLoc, ClassToBrowsableObjectTypeMap.Get() );
				}
				break;


			case IDMN_ObjectContext_Delete:
				{
					TArray< UObject* > SelectedObjects;
					GetSelectedObjects( SelectedObjects );

					GPropertyWindowManager->ClearAllThatContainObjects(SelectedObjects);

					// Delete the objects
					INT NumDeleted = ObjectTools::DeleteObjects( SelectedObjects, *BrowsableObjectTypeList.Get() );
				}
				break;


			case IDMN_ObjectContext_DeleteWithRefs:
				{
					TArray< UObject* > SelectedObjects;
					GetSelectedObjects( SelectedObjects );

					// Delete the objects
					ObjectTools::ForceDeleteObjects( SelectedObjects, *BrowsableObjectTypeList.Get() );
				}
				break;


			case IDMN_ObjectContext_ConsolidateObjs:
				{
					TArray< UObject* > SelectedObjects;
					GetSelectedObjects( SelectedObjects );

					// Display the object consolidation tool with the selected objects already inside the window
					FConsolidateWindow::AddConsolidationObjects( SelectedObjects, *BrowsableObjectTypeList.Get() );
				}
				break;


			case IDMN_ObjectContext_SelectInLevel:
				{
					TArray< UObject* > SelectedObjects;
					GetSelectedObjects( SelectedObjects );

					if( SelectedObjects.Num() == 1 )
					{
						// Select level actors that reference the object selected in the browser
						ObjectTools::SelectObjectAndExternalReferencersInLevel( SelectedObjects( 0 ) );
					}
				}
				break;
			case IDMN_ObjectContext_CaptureThumbnailFromActiveViewport:
				{
					check(GCurrentLevelEditingViewportClient);
					FViewport* Viewport = GCurrentLevelEditingViewportClient->Viewport;
					check(Viewport);

					//have to re-render the requested viewport
					FEditorLevelViewportClient* OldViewportClient = GCurrentLevelEditingViewportClient;
					//remove selection box around client during render
					GCurrentLevelEditingViewportClient = NULL;
					Viewport->Draw();

					List<ContentBrowser::DataAssetItem^>^ SelectedItems = DataBrowserCtrl->AssetView->AssetListView->CloneSelection();
					CaptureThumbnailFromViewport (Viewport, SelectedItems);

					//redraw viewport to have the yellow highlight again
					GCurrentLevelEditingViewportClient = OldViewportClient;
					Viewport->Draw();
				}
				break;
			case IDMN_ObjectContext_CaptureThumbnailFromPlayInEditorViewport:
				{
					FViewport* PlayWorldViewport = GEditor->Client->GetPlayWorldViewport();
					check (PlayWorldViewport);

					List<ContentBrowser::DataAssetItem^>^ SelectedItems = DataBrowserCtrl->AssetView->AssetListView->CloneSelection();
					CaptureThumbnailFromViewport (PlayWorldViewport, SelectedItems);
				}
				break;
			case IDMN_ObjectContext_ClearCustomThumbnails:
				{
					List<ContentBrowser::DataAssetItem^>^ SelectedItems = DataBrowserCtrl->AssetView->AssetListView->CloneSelection();
					ClearCustomThumbnails (SelectedItems);
				}
				break;

			case IDMN_ObjectContext_CopyReference:
				{
					this->DataBrowserCtrl->AssetView->CopySelectedAssets();
				}
				break;


			case IDMN_ObjectContext_ShowRefObjs:
			case IDMN_ObjectContext_GenerateResourceCollection:
				{
					TArray< UObject* > SelectedObjects;
					GetSelectedObjects( SelectedObjects );

					if( SelectedObjects.Num() == 1 )
					{
						// Show a list of objects that the selected object references
						ObjectTools::ShowReferencedObjs( SelectedObjects( 0 ), (InCommandId == IDMN_ObjectContext_GenerateResourceCollection) );
					}
				}
				break;


			case IDMN_ObjectContext_ShowRefs:
				{
					TArray< UObject* > SelectedObjects;
					GetSelectedObjects( SelectedObjects );

					// Show a list of objects that reference the selected object
					ObjectTools::ShowReferencers( SelectedObjects );
				}
				break;

			case IDMN_ObjectContext_ShowReferenceGraph:
				{

					TArray< UObject* > SelectedObjects;
					GetSelectedObjects( SelectedObjects );
					if( SelectedObjects.Num() )
					{
						// Only support 1 selected object at a time
						UObject* FirstObject = SelectedObjects(0);
						GEditor->GetSelectedObjects()->Deselect( FirstObject );
				
						ObjectTools::ShowReferenceGraph( FirstObject, *ClassToBrowsableObjectTypeMap.Get() );
					
						GEditor->GetSelectedObjects()->Select( FirstObject );
					}

				}
				break;
			case IDMN_ObjectContext_Export:
				{
					TArray< UObject* > SelectedObjects;
					GetSelectedObjects( SelectedObjects );

					// Export objects to disk
					const UBOOL bPromptForEachFileName = TRUE;

					FString ExportPath = CLRTools::ToFString(LastExportPath);
					ObjectTools::ExportObjects( SelectedObjects, bPromptForEachFileName, &ExportPath );
					LastExportPath = CLRTools::ToString(ExportPath);
				}
				break;
				
			case IDMN_ObjectContext_BulkExport:
				{
					TArray< UObject* > SelectedObjects;
					GetSelectedObjects( SelectedObjects );

					// Export objects to disk
					const UBOOL bPromptForEachFileName = FALSE;

					FString ExportPath = CLRTools::ToFString(LastExportPath);
					ObjectTools::ExportObjects( SelectedObjects, bPromptForEachFileName, &ExportPath );
					LastExportPath = CLRTools::ToString(ExportPath);
				}
				break;
			case IDMN_ObjectContext_InsertRadioChirp:
				{
					TArray< UObject* > SelectedObjects;
					GetSelectedObjects( SelectedObjects );

					// Add a chirp sound node for the cues in the selected packages. 
					WxSoundCueEditor::BatchProcessInsertRadioChirp( SelectedObjects );
				}
				break;
			case IDMN_ObjectContext_InsertMatureDialog:
				{
					TArray< UObject* > SelectedObjects;
					GetSelectedObjects( SelectedObjects );

					// Add a chirp sound node for the cues in the selected packages. 
					WxSoundCueEditor::BatchProcessInsertMatureNode( SelectedObjects );
				}
				break;
			case IDMN_PackageContext_ClusterSounds:
				{
			      // Get selected packages (including selected groups)
			      TArray< UPackage* > SelectedPackages;
			      GetSelectedPackages( SelectedPackages );
      
			      // Cluster sounds without attenuation
			      WxSoundCueEditor::BatchProcessClusterSounds( SelectedPackages, FALSE );
      
			      // Refresh the package list so our created cues will be visible
			      GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_UpdatePackageList));
				}
				break;
				
			case IDMN_PackageContext_ClusterSoundsWithAttenuation:
				{
					// Get selected packages (including selected groups)
					TArray< UPackage* > SelectedPackages;
					GetSelectedPackages( SelectedPackages );

					// Cluster sounds with attenuation
					WxSoundCueEditor::BatchProcessClusterSounds( SelectedPackages, TRUE );

					// Refresh the package list so our created cues will be visible
					GCallbackEvent->Send( FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_UpdatePackageList) );
				}
				break;

			case IDMN_PackageContext_InsertRadioChirp:
				{
					// Get selected packages (including selected groups)
					TArray< UPackage* > SelectedPackages;
					GetSelectedPackages( SelectedPackages );

					// Add a chirp sound node for the cues in the selected packages. 
					WxSoundCueEditor::BatchProcessInsertRadioChirp( SelectedPackages );

					// Refresh the package list so our created cues will be visible
					GCallbackEvent->Send( FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_UpdateAssetList) );
				}
				break;

            case IDMN_ObjectContext_FilterSelectedObjectTypes:
                {
                    List<ContentBrowser::DataAssetItem^>^ SelectedItems = DataBrowserCtrl->AssetView->AssetListView->CloneSelection();
                    FilterSelectedObjectTypes(SelectedItems);
                }
				break;
				
			default:
				// Ignore requests to this event when an asset context menu is open or they will conflict with the UGenericBrowserType_Sounds handler
  				if( InCommandId > IDMN_ObjectContext_SoundCue_SoundClasses_START 
	  				&& InCommandId < IDMN_ObjectContext_SoundCue_SoundClasses_END 
					&& !DataBrowserCtrl->AssetView->IsContextMenuOpen )
				{
					// A sound class was selected, so process each sound cue found in the selected packages.
					TArray< UPackage* > SelectedPackages;
					GetSelectedPackages( SelectedPackages );

					WxSoundCueEditor::BatchProcessSoundClass( SelectedPackages, InCommandId );

					// Refresh the asset view as we have changed some sound cues
					GCallbackEvent->Send(FCallbackEventParameters( NULL, CALLBACK_RefreshContentBrowser, CBR_UpdateAssetList) );
				}
				else
				{
					// Ask the object type to handle the custom command itself!
					List<ContentBrowser::DataAssetItem^>^ SelectedItems = DataBrowserCtrl->AssetView->AssetListView->CloneSelection();
					//@TODO - Convert custom commands to allow for "common" custom commands for multi-types being selected
					ContentBrowser::DataAssetItem::SortByAssetType(SelectedItems);		//Make sure all types are grouped together

					String^ LastTypeName;
					TArray<UObject*> SelectedObjectsWithSameType;
					for each( ContentBrowser::DataAssetItem^ CurSelectedAsset in SelectedItems )
					{
						//New type?  If so, invoke the command on the array that is built up so far
						if (LastTypeName != CurSelectedAsset->AssetType) {
							InvokeCustomCommandOnArray(InCommandId, SelectedObjectsWithSameType);
							SelectedObjectsWithSameType.Empty();
						}
						LastTypeName = CurSelectedAsset->AssetType;

						UObject* FoundObject = NULL;
						UClass* AssetClass = FindObject<UClass>( ANY_PACKAGE, *CLRTools::ToFString( CurSelectedAsset->AssetType ) );
						if( AssetClass != NULL )
						{
							FoundObject = UObject::StaticFindObject( AssetClass, ANY_PACKAGE, *CLRTools::ToFString( CurSelectedAsset->FullyQualifiedPath ) );
						}
						SelectedObjectsWithSameType.AddItem(FoundObject);
					}
					//if there is any left over
					InvokeCustomCommandOnArray(InCommandId, SelectedObjectsWithSameType);
				}
				break;
		}

		// Add to recent assets
		TArray< UObject* > SelectedObjects;
		GetSelectedObjects( SelectedObjects );
		for ( TArray<UObject*>::TConstIterator ObjectItr(SelectedObjects); ObjectItr; ++ObjectItr )
		{
			UObject* CurrentObject = *ObjectItr;
			FString ObjectName;
			ObjectName = CurrentObject->GetPathName();
			DataBrowserCtrl->AddRecentItem( CLRTools::ToString(ObjectName) );
		}
		WriteRecentObjectsToConfig();
#endif
	}


	void InvokeCustomCommandOnArray(INT CommandID, TArray<UObject*> SelectedObjectsWithSameType)
	{
		if (SelectedObjectsWithSameType.Num())
		{
			UObject* SampleObject = SelectedObjectsWithSameType(0);
			// NOTE: This list is ordered such that child object types appear first
			const TArray< UGenericBrowserType* >* BrowsableObjectTypes =
				ClassToBrowsableObjectTypeMap->Find( SampleObject->GetClass() );
			if( BrowsableObjectTypes != NULL )
			{
				for( INT CurBrowsableTypeIndex = 0; CurBrowsableTypeIndex < BrowsableObjectTypes->Num(); ++CurBrowsableTypeIndex )
				{
					UGenericBrowserType* CurObjType = ( *BrowsableObjectTypes )( CurBrowsableTypeIndex );
					if( CurObjType->Supports( SampleObject ) )
					{
						CurObjType->InvokeCustomCommand( CommandID, SelectedObjectsWithSameType );
					}
				}
			}
		}
	}

	/** Called by the asset view or package tree context menu to check to see a custom object command should be enabled */
	void CanExecuteCustomObjectCommand( Object^ Sender, Input::CanExecuteRoutedEventArgs^ EventArgs )
	{
		//no one else should ever handle this event.
		EventArgs->Handled = true;

		int CommandID = (int)EventArgs->Parameter;

		EventArgs->CanExecute = false;
			
		// Check to see if the window is disabled.  This usually means that a modal dialog is up and
		// we don't want to be processing commands which could be reentrant
		if( DataBrowserCtrl->IsEnabled )
		{
			if( ( CommandID >= IDMN_ObjectContext_SoundCue_SoundClasses_START && CommandID < IDMN_ObjectContext_SoundCue_SoundClasses_END ) 
				|| CommandID == IDMN_PackageContext_ClusterSounds 
				|| CommandID == IDMN_PackageContext_ClusterSoundsWithAttenuation
				|| CommandID == IDMN_PackageContext_InsertRadioChirp
				|| CommandID == IDMN_PackageContext_BatchProcess )
			{
				// always allow batch process commands in the package list context menu
				EventArgs->CanExecute = true;
			}
			else
			{
				for each( ContentBrowser::DataAssetItem^ CurAssetItem in DataBrowserCtrl->AssetView->AssetListView->SelectedItems )
				{
					// As long as we have at least one asset selected we'll enable the command
					EventArgs->CanExecute = true;
					break;
				}
			}
		}
	}

public:

	/**
	 * Wrapper for determining whether an asset is a map package or contained in a map package
	 *
	 * @param	AssetPathName	the fully qualified [Unreal] pathname of the asset to check
	 *
	 * @return	TRUE if the specified asset is a map package or contained in a map package
	 */
	virtual bool IsMapPackageAsset( String^ AssetPathName )
	{
		return CLRTools::IsMapPackageAsset(AssetPathName);
	}

	/**
	 * Wrapper for determining whether an asset is eligible to be loaded on its own.
	 * 
	 * @param	AssetPathName	the fully qualified [Unreal] pathname of the asset to check
	 * 
	 * @return	true if the specified asset can be loaded on its own
	 */
	virtual bool IsAssetValidForLoading( String ^AssetPathName )
	{
		return CLRTools::IsAssetValidForLoading(AssetPathName);
	}

	/**
	 * Wrapper for determining whether an asset is eligible to be placed in a level.
	 * 
	 * @param	AssetPathName	the fully qualified [Unreal] pathname of the asset to check
	 * 
	 * @return	true if the specified asset can be placed in a level
	 */
	virtual bool IsAssetValidForPlacing( String ^AssetPathName )
	{
		return CLRTools::IsAssetValidForPlacing(AssetPathName);
	}

	/**
	 * Wrapper for determining whether an asset is eligible to be tagged.
	 * 
	 * @param	AssetFullName	the full name of the asset to check
	 * 
	 * @return	true if the specified asset can be tagged
	 */
	virtual bool IsAssetValidForTagging( String^ AssetFullName )
	{
		return CLRTools::IsAssetValidForTagging( AssetFullName );
	}

	/** Write the contents of the recent objects array to a config file */
	void WriteRecentObjectsToConfig();

	UObject* CreateFromObject( UObject* ArchetypeBase, FString& ArchetypeName, FString& PackageName, FString& GroupName );
	UObject* CreatePackage( FString& ObjType, FString& PackageName, FString& PathName, FString& GroupName );
	void SaveAsExcel(const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames, const FString& Path, int nClassType, const FString& ExcelName);
	void ImportFromExcel(const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames, const FString& Path, int nClassType, UClass* classType);
	void SaveModifier(const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames, const FString& Path, int nClassType);
	BOOL SaveModifierFile(const TCHAR* file, MODIFIER_FILE_HEADER* Header, MODIFIER_FILE_NODE_NEW* Node);
	void SaveSpellNode(const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames, const FString& Path, int nClassType);
	void SaveAsExcelWithoutSelected(FString NodeName, FString NodeConfigDir);
	UBOOL ShouldSerializeValue( QWORD PropertyFlags);
	void ExportProperty(BYTE*Data, UProperty* ObjPro, PropertyNode& ParentNode, int nIndex);
	void ExportPropertyToTextArray(PropertyNode &ParentNode, TArray<FString>& Items);
	UBOOL ExportExcelHeader( const TCHAR* Section, TArray<FString>& Header, TArray<FString>& DefaultValue, const TCHAR* Filename );
	void ImportProperty(PropertyNode& ParentNode, int& nHeadIndex, TArray<FString>& DefaultValue);
	void SetDefaultValue(UObject *Obj, const TCHAR* Section);
	void FullLoadPackages(FString ConfigDir, TArray<UPackage*> &Packages, TArray<FString> &PackageNames);
	void ExportTable(const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames, const FString& Path, int nClassType);
	void ImportTable(const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames, const FString& Path, int nClassType);
	void CLRAnalyzeReference(FString FolderName, TArray<FString>& PackageNames);

protected:

	/** The actual WPF control that is being hosted */
	ContentBrowser::DataMainControl^ DataBrowserCtrl;

	/** Pointer to the WxDataBrowserHost that we're parented to (may be NULL if there is none.) */
	WxDataBrowserHost* ParentBrowserWindow;

	/** WPF interop wrapper around the HWND and WPF controls */
	Interop::HwndSource^ InteropWindow;

	/** pointer to the native version of DataBrowser */
	FDataBrowser* NativeBrowserPtr;

	/** Array of browsable object types */
	MScopedNativePointer< TArray< UGenericBrowserType* > > BrowsableObjectTypeList;

	/** Array of classes which use a static shared thumbnail */
	MScopedNativePointer< TArray<UClass*> > SharedThumbnailClasses;

	/** array of classes which are used for creating new factory classes */
	MScopedNativePointer< TArray<UClass*> > ObjectFactoryClasses;

	/** Stores a list of classes encompassed by an artist-friendly "browser type". */
	MScopedNativePointer< GBTToClassMap > BrowsableObjectTypeToClassMap;
	
	/** Stores a list of browsable types associated with each object class */
	MScopedNativePointer< ClassToGBTMap > ClassToBrowsableObjectTypeMap;

	/** Stores a list class names associated with artist-friendly "browser type" names */
	Dictionary< String^, List<String^>^ >^ ClassNameToBrowsableTypeNameMap;

	/** Maps object type names to their type's displayed border color */
	Dictionary< String^, Color >^ ClassNameToBorderColorMap;

	/** All AssetType names (ignores UClasses listed under archetype as those can be just about any UClass ) */
	UnrealEd::NameSet^ AssetTypeNames;

	/** Array of Unreal objects that we're currently maintaining references to because the UI needs them for awhile */
	MScopedNativePointer< TArray< UObject* > > ReferencedObjects;

	/** Current state of the browser's query engine */
	EDataBrowserQueryState QueryEngineState;

	/** Indicates that the source control state of the loaded packages is out-of-date. */
	bool bIsSCCStateDirty;

	/**
	 * Flags for indicating that various updates have been requested. 
	 * Checked in Tick() and if true, performs the work
	 */
	bool bPackageListUpdateRequested, bPackageListUpdateUIRequested, bPackageFilterUpdateRequested;
	bool bCollectionListUpdateRequested, bCollectionListUpdateUIRequested;
	bool bAssetViewUpdateRequested, bAssetViewUpdateUIRequested, bDoFullAssetViewUpdate;
	bool bAssetViewSyncRequested;
	bool bCollectionSyncRequested;

	/** 
	 * Bitmask of AssetStatusUpdateFlags values to apply when asset update is requested.
	 */
	UINT AssetUpdateFlags;


	/**
	 * Indicates whether the package treeview's expansion state has been loaded from the stored layout info
	 */
	bool bHasInitializedTree;

	String^ LastExportPath;
	String^ LastImportPath;
	String^ LastOpenPath;
	String^ LastSavePath;

	INT		LastImportFilter;


	/**
	 * Cached state of user's selected objects and filter settings when a query is started
	 */
	ref struct CachedQueryDataType
	{
		ReadOnlyCollection< String^ >^ SelectedSharedCollectionNames;
		ReadOnlyCollection< String^ >^ SelectedPrivateCollectionNames;
		ReadOnlyCollection< String^ >^ SelectedLocalCollectionNames;

		// Note that SelectedPathNames contains a path for each group/package you have selected.
		// e.g. If you have
		//       pkg
		//         |-Grp0
		//         |-Grp1
		// Then SelectedPathNames contains "pkg", "pkg.Grp0", "pkg.Grp1"
		UnrealEd::NameSet^ SelectedPathNames;
		// Contains packages that are selected, selected as a result of being in a selected folder, or partially selected because one of the groups within this package is selected.
		UnrealEd::NameSet^ SelectedOutermostPackageNames;
		// These are packages that are selected by actually being highlighted or having one of their ancestor folders highlighted.
		UnrealEd::NameSet^ ExplicitlySelectedOutermostPackageNames;
	};

	ref struct CollectionSelectRequest
	{
		ContentBrowser::EBrowserCollectionType CollectionType;
		String^ CollectionName;
	};

	/** The full name of the last asset that was previewed */
	String^ LastPreviewedAssetFullName;

	typedef List< ContentBrowser::DataAssetItem^ > AssetItemList;


	/** Cached query data */
	auto_handle< CachedQueryDataType > CachedQueryData;

	/** Cached list of asset names we pulled from the GAD for the current asset query */
	MScopedNativePointer< TLookupMap< FName > > CachedQueryAssetFullNameFNamesFromGAD;
	MScopedNativePointer< TLookupMap< FName > > LastQueryAssetFullNameFNamesFromGAD;
	
	/** All quarantined assets */
	UnrealEd::NameSet^ CachedQueryQuarantinedAssets;

	/** Current iterator index into cached GAD asset name list, object list, etc. */
	int CachedQueryIteratorIndex;

	/** Cached list of Unreal objects we gathered from the engine.  Note that we do NOT keep references to
	    these objects.  Instead, we sign up for a "pre-GC" callback and invalidate the list if a GC occurs */
	MScopedNativePointer< TArray< UObject* > > CachedQueryObjects;

	/** Cached set of full names for Unreal objects we gathered from the engine.  This is
	    used to check to see if an object is currently loaded while populating the asset list */
	MScopedNativePointer< TSet< FName > > CachedQueryLoadedAssetFullNames;
	MScopedNativePointer< TSet< FName > > LastQueryLoadedAssetFullNames;

	/** list of UObjects which generated callback events */
	MScopedNativePointer< TArray< UObject* > >		CallbackEventObjects;
	MScopedNativePointer< TArray< UPackage* > >		CallbackEventPackages;
	MScopedNativePointer< TArray< UObject* > >		CallbackEventSyncObjects;

	/** List of collections to select, the next tick */
	List<CollectionSelectRequest^>^					CollectionSelectRequests;

	/** Static: True unless the user has asked to suppress all Content Browser "are you sure?" prompts for the current session */
	static cli::array<bool>^ ShowConfirmationPrompt = gcnew cli::array<bool>( (int)ContentBrowser::ConfirmationPromptType::NumPromptTypes ) { true, true, true, true };


	/** True if we're currently executing a menu command, and thus, shouldn't execute another (reentrancy guard) */
	bool bIsExecutingMenuCommand;

};


/** Sets up the list of Unreal object types that we're able to display */
void MDataBrowserControl::InitBrowsableObjectTypeList()
{
	BrowsableObjectTypeList.Reset( new TArray< UGenericBrowserType* >() );
	SharedThumbnailClasses.Reset(new TArray<UClass*>());

	BrowsableObjectTypeToClassMap.Reset( new GBTToClassMap() );
	ClassToBrowsableObjectTypeMap.Reset( new ClassToGBTMap() );

	ObjectTools::CreateBrowsableObjectTypeMaps(
		*BrowsableObjectTypeList.Get(),
		*BrowsableObjectTypeToClassMap.Get(),
		*ClassToBrowsableObjectTypeMap.Get() );

	UThumbnailManager& ThumbnailManger = *GUnrealEd->GetThumbnailManager();
	
	// Build a list of all AssetTypes and a list of classes that use shared thumbnails
	// Do this by getting all supported classes from all browser types (except for Archetypes because they include non-asset UClass)
	AssetTypeNames = gcnew UnrealEd::NameSet();
	for ( GBTToClassMap::TConstIterator GBTypeIt(*BrowsableObjectTypeToClassMap.Get()); GBTypeIt; ++GBTypeIt )
	{
		const UGenericBrowserType* CurBrowserType = GBTypeIt.Key();
		const TArray<UClass*>& SupportedClasses = GBTypeIt.Value();

		const bool bIsArchetypeType = ( ConstCast<UGenericBrowserType_Archetype>( CurBrowserType ) != NULL );
		
		
		for(TArray<UClass*>::TConstIterator ClassIt(SupportedClasses); ClassIt; ++ClassIt )
		{
			UClass* SupportedClass = *ClassIt; 

			if ( !bIsArchetypeType )
			{
				FThumbnailRenderingInfo* RenderInfo = ThumbnailManger.GetRenderingInfo( SupportedClass->GetDefaultObject() );
				if( !(SupportedClass->ClassFlags & CLASS_Abstract) && (!RenderInfo || RenderInfo->Renderer->GetClass() == UIconThumbnailRenderer::StaticClass() ) )
				{
					// If there is no thumbnail render info for this class or the renderer is an icon renderer (Legacy generic browser support)
					// the class uses a shared thumbnail.  Additionally we should skip abstract classes that show up here.
					SharedThumbnailClasses->AddUniqueItem(SupportedClass);
				}

				AssetTypeNames->Add( CLRTools::ToString( SupportedClass->GetName() ) );
			}
			else
			{
				// Archetypes support just about every UClass; we do not consider them AssetTypes.
				// Thus, not adding to AssetTypeNames.
			}
		}
	}



	// Also populate a list of Class Names to Generic Browser Type Names for easy use in C# land.
	ClassNameToBrowsableTypeNameMap = gcnew Dictionary<String^, List<String^>^>();
	ClassNameToBorderColorMap = gcnew Dictionary<String^, Color>();
	for ( ClassToGBTMap::TConstIterator ClassIt(GetBrowsableObjectTypeMap()); ClassIt; ++ClassIt )
	{
		const UClass* CurClass = ClassIt.Key();
		const TArray< UGenericBrowserType* >& BrowserTypes = ClassIt.Value();

		String^ CurObjectTypeName = CLRTools::ToString( CurClass->GetName() );
	
		List<String^>^ BrowserTypeNames = gcnew List<String^>( BrowserTypes.Num() );
		for (TArray<UGenericBrowserType*>::TConstIterator BrowserTypeIt(BrowserTypes); BrowserTypeIt; ++BrowserTypeIt)
		{
			// We never add the Archetypes browsable type to this map as that's handled specially
			// in the IsAssetAnyOfBrowsableTypes() method.  )
			const bool bIsArchetypeType = ( Cast<UGenericBrowserType_Archetype>( *BrowserTypeIt ) != NULL );
			if( !bIsArchetypeType )
			{
				BrowserTypeNames->Add( CLRTools::ToString( (*BrowserTypeIt)->GetBrowserTypeDescription() ) );
			}

			if( !ClassNameToBorderColorMap->ContainsKey( CurObjectTypeName ) )
			{
				FColor EngineBorderColor = (*BrowserTypeIt)->SupportInfo( 0 ).BorderColor;
				Color WPFBorderColor = Color::FromArgb(
					200,		// Opacity
					127 + EngineBorderColor.R / 2,		// Desaturate the colors a bit (GB colors were too.. much)
					127 + EngineBorderColor.G / 2,
					127 + EngineBorderColor.B / 2 );

				ClassNameToBorderColorMap->Add( CurObjectTypeName, WPFBorderColor );
			}
		}
		
		if( ClassNameToBrowsableTypeNameMap->ContainsKey(CurObjectTypeName) )
		{
			debugf( NAME_Warning, TEXT("Duplicate class names.  Two uc files in separate packages likely have the same name: %s"), *CLRTools::ToFString(CurObjectTypeName) );
		}
		else
		{
			ClassNameToBrowsableTypeNameMap->Add( CurObjectTypeName, BrowserTypeNames );
		}
	}
}

/** Parse the recently accessed browser objects from config file */
void MDataBrowserControl::InitRecentObjectsList()
{
	if ( !RecentItemsInitialized )
	{
		// Find the max allowed number of recent items
		DataBrowserCtrl->InitRecentItems( 30 );

		// Add the path to each object to the list of recent items
		TArray<FString> RecentAssetArray;
		GConfig->GetSingleLineArray(TEXT("UnrealEd"), TEXT("RecentAssets"), RecentAssetArray, GEditorUserSettingsIni);
		for ( TArray<FString>::TConstIterator RecentItemItr(RecentAssetArray); RecentItemItr; ++RecentItemItr )
		{
			const FString& CurItemPath = *RecentItemItr;
			//DataBrowserCtrl->AddRecentItem( CLRTools::ToString(CurItemPath) );
		}
		RecentItemsInitialized = true;
	}
}

/** Write the contents of the recent objects array to a config file */
void MDataBrowserControl::WriteRecentObjectsToConfig()
{
	// Clear the section so that we remove any stale entries
	FString SectionName = FString( TEXT("UnrealEd.RecentAssets") );
	GConfig->EmptySection( *SectionName, GEditorUserSettingsIni );

	// Check for out of bounds exception
	INT NumRecents = DataBrowserCtrl->RecentAssets->Count;

	// Add the recent items back to the ini
	TArray<FString> RecentAssetArray;
	RecentAssetArray.AddZeroed( NumRecents );
	for ( INT RecentsIndex = (NumRecents > DataBrowserCtrl->MaxNumberRecentItems) ? DataBrowserCtrl->MaxNumberRecentItems : NumRecents-1;
		RecentsIndex >= 0;
		--RecentsIndex
		)
	{
		String^ CurrentRecentAsset = DataBrowserCtrl->RecentAssets[RecentsIndex];
		RecentAssetArray.AddItem( CLRTools::ToFString(CurrentRecentAsset) );
	}
	GConfig->SetSingleLineArray(TEXT("UnrealEd"), TEXT("RecentAssets"), RecentAssetArray, GEditorUserSettingsIni);

	GConfig->Flush(FALSE, GEditorUserSettingsIni);
}

IMPLEMENT_COMPARE_POINTER( UClass, DataBrowserCLR, { return appStricmp(*A->GetDefaultObject<UFactory>()->Description, *B->GetDefaultObject<UFactory>()->Description); } )

/**
 * Initialize the list of UFactory classes which are valid for populating the "New Object" context menu.
 */
void MDataBrowserControl::InitFactoryClassList()
{
	ObjectFactoryClasses.Reset(new TArray<UClass*>());

	// Initialize array of UFactory classes that can create new objects.
	for( TObjectIterator<UClass> It ; It ; ++It )
	{
		UClass* Cls = *It;
		if ( Cls->IsChildOf(UFactory::StaticClass()) && !Cls->HasAnyClassFlags(CLASS_Abstract) )
		{
			// Add to list if factory creates new objects and the factory is valid for this game (ie Wargame)
			UFactory* Factory = Cast<UFactory>( Cls->GetDefaultObject() );
			if( Factory->bCreateNew && Factory->ValidForCurrentGame() )
			{
				ObjectFactoryClasses->AddItem( Cls );
			}
		}
	}

	TArray<UClass*>& ObjectFactoryClassesRef = *ObjectFactoryClasses.Get();
	Sort<USE_COMPARE_POINTER(UClass,DataBrowserCLR)>( &ObjectFactoryClassesRef(0), ObjectFactoryClassesRef.Num() );
}

/**
 * Populate a context menu with a menu item for each type of object that can be created using a class factory.
 *
 * @param	ctxMenu		the context menu to populate items for
 */
void MDataBrowserControl::PopulateObjectFactoryContextMenu( ContextMenu^ ctxMenu )
{
	String^ NewObjectString = UnrealEd::Utils::Localize("DataBrowser_AssetView_NewObject");

	if ( ObjectFactoryClasses )
	{
		ctxMenu->Closed += gcnew RoutedEventHandler(this, &MDataBrowserControl::OnObjectFactoryContextMenuClosed);
		TArray<UClass*>& FactoryClassArray = *ObjectFactoryClasses.Get();
		for ( INT ClassIdx = 0; ClassIdx < FactoryClassArray.Num(); ClassIdx++ )
		{
			UClass* FactoryClass = FactoryClassArray(ClassIdx);
			UFactory* FactoryCDO = FactoryClass->GetDefaultObject<UFactory>();

			String^ FactoryName = CLRTools::ToString(FactoryCDO->Description);

			MenuItem^ FactoryMI = gcnew MenuItem();

			Input::RoutedUICommand^ Command = gcnew Input::RoutedUICommand(
				NewObjectString->Concat(FactoryName), 
				FactoryName, 
				ContentBrowser::ObjectFactoryCommands::typeid
				);
			Input::CommandBinding^ Binding = gcnew Input::CommandBinding(
				Command,
				gcnew Input::ExecutedRoutedEventHandler(this, &MDataBrowserControl::ExecuteMenuCommand),
				gcnew Input::CanExecuteRoutedEventHandler(this, &MDataBrowserControl::CanExecuteMenuCommand)
				);


			DataBrowserCtrl->AssetView->CommandBindings->Add(Binding);

			FactoryMI->Header = NewObjectString + FactoryName;
			FactoryMI->Command = Command;
			FactoryMI->CommandParameter = ID_NEWOBJ_START + ClassIdx;
			FactoryMI->IsEnabled = true;
			ctxMenu->Items->Add(FactoryMI);
		}
	}
}

/** Populates an item collection with package command menu items for use in a ContextMenu or MenuItem.  
 *  Note: This function also populates package menu with dynamic menu items which do not use the passed in event handlers.  
 *  Instead they use ExecuteCustomObjectCommand and CanExecuteCustomObjectCommand.
 *  
 * @param OutPackageListMenuItems		The context menu where menu items should be added.
 * @param OutCommandBindings			The command binding collection where new command bindings should be added.
 * @param InTypeId						The typeid of the ui element we are registering the commands with
 * @param InEventHandler				Optional new event handler that should be called on these menu items.
 * @param InCanExecuteEventHandler		Optional new event handler that should be called when determining if these menu items can be used.
 */
void MDataBrowserControl::PopulatePackageListMenuItems( ItemCollection^ OutPackageListMenuItems, Input::CommandBindingCollection^ OutCommandBindings, Type^ InTypeId, Input::ExecutedRoutedEventHandler^ InEventHandler, Input::CanExecuteRoutedEventHandler^ InCanExecuteEventHandler  )
{
	ContextMenu^ PackageListCM = nullptr;

	PackageListCM = (ContextMenu^)DataBrowserCtrl->MyPackagePanel->FindResource("PackageListContextMenu");
	
	if( PackageListCM != nullptr )
	{
		for each( Object^ Obj in PackageListCM->Items )
		{
			if( dynamic_cast< Separator^>( Obj ) != nullptr )
			{
				// this object is a separator
				OutPackageListMenuItems->Add( gcnew Separator() );
			}
			else 
			{
				MenuItem^ SourceItem = dynamic_cast< MenuItem^ > ( Obj );
				if( SourceItem != nullptr )
				{
					// this object is a MenuItem
					MenuItem^ ItemCopy = gcnew MenuItem();
					ItemCopy->Command = SourceItem->Command;
					ItemCopy->Header = SourceItem->Header;
					OutPackageListMenuItems->Add( ItemCopy );
					
					if( ItemCopy->Command != nullptr && InEventHandler != nullptr && InCanExecuteEventHandler != nullptr )
					{
						// Add a new command binding if the user passed in new event handlers
						Input::CommandBinding^ NewCmdBinding = gcnew Input::CommandBinding( ItemCopy->Command, InEventHandler, InCanExecuteEventHandler );
						OutCommandBindings->Add( NewCmdBinding );
					}

					if( SourceItem->Items->Count > 0)
					{
						// there are sub menu items.  Append them all
						for each( Object^ SubObj in SourceItem->Items )
						{
							if( dynamic_cast< Separator^>( SubObj ) != nullptr )
							{
								//the sub object is a separator
								ItemCopy->Items->Add( gcnew Separator() );
							}
							else
							{
								MenuItem^ SubMenuItem = dynamic_cast< MenuItem^ >(SubObj);
								if( SubMenuItem != nullptr )
								{
									Input::RoutedCommand^ Command = dynamic_cast< Input::RoutedCommand^ >(SubMenuItem->Command);
									if( Command == nullptr || Command->OwnerType == ContentBrowser::SourceControlCommands::typeid )
									{
										MenuItem^ SubItemCopy = gcnew MenuItem();
										SubItemCopy->Command = SubMenuItem->Command;
										SubItemCopy->Header = SubMenuItem->Header;

										ItemCopy->Items->Add( SubItemCopy );

										if( SubItemCopy->Command != nullptr  && InEventHandler != nullptr && InCanExecuteEventHandler != nullptr )
										{
											// Add a new command binding if the user passed in new event handlers
											Input::CommandBinding^ NewCmdBinding = gcnew Input::CommandBinding( SubItemCopy->Command, InEventHandler, InCanExecuteEventHandler );
											OutCommandBindings->Add( NewCmdBinding );
										}

									}	
								}
							}
						}
					}
				}
			}
		}
	}

// 	List< Object ^>^ CustomMenuItems = gcnew List< Object^ >();
// 	TArray< FObjectSupportedCommandType > SupportedCommands;
// 	QueryPackageTreeContextMenuItems( SupportedCommands );
// 	
// 	// Create the menu items.
// 	BuildContextMenu( SupportedCommands, INDEX_NONE, InTypeId, OutCommandBindings, CustomMenuItems );
// 
// 	// Add a separator to separate custom menu items
// 	OutPackageListMenuItems->Add( gcnew Separator() );
// 	for each( Object^ Obj in CustomMenuItems )
// 	{
// 		OutPackageListMenuItems->Add( Obj );
// 	}
	
}


/**
 * Returns menu item descriptions, to be displayed in the asset view's right click context menu
 *
 * @param	OutMenuItems	(Out) List of WPF menu items
 */
void MDataBrowserControl::QueryAssetViewContextMenuItems( List< Object^ >^% OutMenuItems )
{
	// Make sure all selected objects are loaded, where possible
	LoadSelectedObjectsIfNeeded();

	TArray<FObjectSupportedCommandType> SupportedCommands;
	INT DefaultCommandID;
	QuerySupportedCommands( SupportedCommands, DefaultCommandID );
	BuildContextMenu( SupportedCommands, DefaultCommandID, ContentBrowser::AssetView::typeid, DataBrowserCtrl->AssetView->CommandBindings, OutMenuItems );
}

/**
* Returns dynamic menu item descriptions, to be displayed in the package tree view's right click context menu
*
* @param	OutMenuItems	(Out) List of WPF menu items
*/
void MDataBrowserControl::QueryPackageTreeContextMenuItems( TArray<FObjectSupportedCommandType>& OutSupportedCommands )
{
	// Should be no elements before starting
	OutSupportedCommands.Empty();

	// Create the batch proces menu item first so the sound class menu items are parented correctly.
	FObjectSupportedCommandType BatchProcess( IDMN_PackageContext_BatchProcess, LocalizeUnrealEd( "PackageContext_BatchProcess") );
	INT BatchIndex = OutSupportedCommands.AddItem( BatchProcess ); 

	// Generate the list of available sounds classes
	GEditor->CreateSoundClassMenuForContentBrowser( OutSupportedCommands, BatchIndex );

	// Add the cluster commands
	FObjectSupportedCommandType ClusterCommand( IDMN_PackageContext_ClusterSounds,  LocalizeUnrealEd( "PackageContext_ClusterSounds" ), TRUE, BatchIndex );
	FObjectSupportedCommandType ClusterAttCommand( IDMN_PackageContext_ClusterSoundsWithAttenuation, LocalizeUnrealEd( "PackageContext_ClusterSoundsWithAtt" ), TRUE, BatchIndex );
	FObjectSupportedCommandType InsertChirpCommand( IDMN_PackageContext_InsertRadioChirp, LocalizeUnrealEd( "PackageContext_InsertRadioChirp" ), TRUE, BatchIndex );
	OutSupportedCommands.AddItem ( ClusterCommand );
	OutSupportedCommands.AddItem ( ClusterAttCommand );
	OutSupportedCommands.AddItem ( InsertChirpCommand );

}
/**
 * Returns an array of commands supported by the current selection.
 *
 * @param	OutSupportedCommands		(Out) Array of commands supported by the current selection
 * @param	OutDefaultCommandID			(Out) CommandID to execute when the user does not specify an explicit command (e.g. on doubleclick)
 */
void MDataBrowserControl::QuerySupportedCommands( TArray<FObjectSupportedCommandType>& OutSupportedCommands, INT& OutDefaultCommandID )
{
	OutSupportedCommands.Empty();

	// Figure out which types of objects are selected
	TArray< UGenericBrowserType* > SelectedObjectTypes;
	for each ( ContentBrowser::DataAssetItem^ CurSelectedAsset in DataBrowserCtrl->AssetView->AssetListView->SelectedItems )
	{
		UObject* FoundObject = NULL;
		UClass* AssetClass = FindObject<UClass>( ANY_PACKAGE, *CLRTools::ToFString( CurSelectedAsset->AssetType ) );
		if( AssetClass != NULL )
		{
			FoundObject = UObject::StaticFindObject( AssetClass, ANY_PACKAGE, *CLRTools::ToFString( CurSelectedAsset->FullyQualifiedPath ) );
		}
		if( FoundObject != NULL )
		{
			// NOTE: This list is ordered such that child object types appear first
			const TArray< UGenericBrowserType* >* BrowsableObjectTypes = ClassToBrowsableObjectTypeMap->Find( FoundObject->GetClass() );
			if( BrowsableObjectTypes != NULL )
			{
				for( INT CurBrowsableTypeIndex = 0; CurBrowsableTypeIndex < BrowsableObjectTypes->Num(); ++CurBrowsableTypeIndex )
				{
					UGenericBrowserType* CurObjType = (*BrowsableObjectTypes)(CurBrowsableTypeIndex);
					if( CurObjType->Supports( FoundObject ) )
					{
						SelectedObjectTypes.AddUniqueItem( CurObjType );

						// Break out after the first type we find that matches this asset item
						break;
					}
				}
			}
		}
	}


	OutDefaultCommandID = INDEX_NONE;

	// Only display type-specific commands if a single type of object was selected
	if( SelectedObjectTypes.Num() == 1 )
	{
		UGenericBrowserType* CurObjType = SelectedObjectTypes(0);

		bool bAnyObjectsForCurObjType = false;
		for each ( ContentBrowser::DataAssetItem^ CurSelectedAsset in DataBrowserCtrl->AssetView->AssetListView->SelectedItems )
		{
			UObject* FoundObject = NULL;
			UClass* AssetClass = FindObject<UClass>( ANY_PACKAGE, *CLRTools::ToFString( CurSelectedAsset->AssetType ) );
			if( AssetClass != NULL )
			{
				FoundObject = UObject::StaticFindObject( AssetClass, ANY_PACKAGE, *CLRTools::ToFString( CurSelectedAsset->FullyQualifiedPath ) );
			}
			if( FoundObject != NULL )
			{
				if ( CurObjType->Supports( FoundObject ) )
				{
					// OK, we found at least one object in the selection set that the current object type supports.
					// We'll definitely want to give this object type a chance to populate the context menu!
					bAnyObjectsForCurObjType = true;

					break;
				}
			}
		}

		if( bAnyObjectsForCurObjType )
		{
			USelection* Selection = GEditor->GetSelectedObjects();

			// Query the commands that this object supports
			CurObjType->QuerySupportedCommands( Selection, OutSupportedCommands );

			// Figure out the default action ( appears in bold )
			TArray<UObject*> SelectedObjects;
			Selection->GetSelectedObjects( SelectedObjects );
			OutDefaultCommandID = CurObjType->QueryDefaultCommand( SelectedObjects );
		}
	}

	// Append standard commands if we have at least one viable asset
	if ( SelectedObjectTypes.Num() > 0 )
	{
		// Add separator if we need to
		if( OutSupportedCommands.Num() > 0 )
		{
			OutSupportedCommands.AddItem( FObjectSupportedCommandType( INDEX_NONE, "" ) );
		}

		// Query standard commands
		UGenericBrowserType::QueryStandardSupportedCommands(
			GEditor->GetSelectedObjects(),
			OutSupportedCommands );	// Out

		// Filter menu option
		OutSupportedCommands.AddItem( FObjectSupportedCommandType( INDEX_NONE, "" ) );
		OutSupportedCommands.AddItem( FObjectSupportedCommandType( IDMN_ObjectContext_FilterSelectedObjectTypes, 
			*FString::Printf(LocalizeSecure(LocalizeUnrealEd( TEXT("ObjectContext_FilterSelectedObjectTypes") ), 
			SelectedObjectTypes.Num() == 1 ? *(SelectedObjectTypes(0)->Description) : TEXT("Selected Objects") ) ) ) ); 
	}

	//Append option to capture thumbnails from viewport for shared thumbnail type assets
	UBOOL bAllowThumbnailCapture = FALSE;
	for each ( ContentBrowser::DataAssetItem^ CurSelectedAsset in DataBrowserCtrl->AssetView->AssetListView->SelectedItems )
	{
		// check whether this is a type that uses one of the shared static thumbnails
		bAllowThumbnailCapture = bAllowThumbnailCapture || AssetUsesSharedThumbnail( CurSelectedAsset );
	}
	if (bAllowThumbnailCapture)
	{
		//only enable if there is a selected viewport
		const UBOOL bEnabled = (GCurrentLevelEditingViewportClient!=NULL);
		OutSupportedCommands.AddItem( FObjectSupportedCommandType( IDMN_ObjectContext_CaptureThumbnailFromActiveViewport, *LocalizeUnrealEd( "ObjectContext_CaptureThumbnailsFromViewport" ), bEnabled ? true : false ) );

		UBOOL bPlayInViewportEnabled = FALSE;
		FViewport* PlayWorldViewport = GEditor->Client->GetPlayWorldViewport();
		if (PlayWorldViewport)
		{
			bPlayInViewportEnabled = TRUE;
		}

		OutSupportedCommands.AddItem( FObjectSupportedCommandType( IDMN_ObjectContext_CaptureThumbnailFromPlayInEditorViewport, *LocalizeUnrealEd( "ObjectContext_CaptureThumbnailsFromPlayInEditorViewport" ), bPlayInViewportEnabled ? true : false ) );
		OutSupportedCommands.AddItem( FObjectSupportedCommandType( IDMN_ObjectContext_ClearCustomThumbnails, *LocalizeUnrealEd( "ObjectContext_ClearCustomThumbnails" ), true) );
	}

	OutDefaultCommandID = DetermineDefaultCommand( OutDefaultCommandID, OutSupportedCommands );
}

/**
 * Given a list of commands, build a list of context menu items to represent it.
 * 
 * @param	InCommands		List of commands from which to build a menu
 * @param	InDefaultCommandID		This command should appear in bold
 * @param	InTypeID				The typeid to pass to RoutedUICommand.  This is needed since different ui elements use this function and we want to make sure commands are registered properly.
 * @param	InOutCommandBindingList	The command binding list where newly created commands should be added.
 * @param	OutMenuItems			(Out) List of WPF menu items
 */
void MDataBrowserControl::BuildContextMenu( const TArray<FObjectSupportedCommandType>& InCommands, INT InDefaultCommandId, Type^ InTypeID, Input::CommandBindingCollection^ InOutCommandBindingList, List< Object^ >^% OutMenuItems )
{
	Dictionary<INT, MenuItem^>^ MenuItems = gcnew Dictionary<INT, MenuItem^>();

	OutMenuItems = gcnew List< Object^ >();

	// Construct the context menu
	for( int CurCommandIndex = 0; CurCommandIndex < InCommands.Num(); ++CurCommandIndex )
	{
		const FObjectSupportedCommandType& CurCommand = InCommands( CurCommandIndex );

		// Create a new context menu item!
		if( CurCommand.CommandID != INDEX_NONE )
		{
			MenuItem^ NewMenuItem = gcnew MenuItem();

			// Setup a command
			NewMenuItem->Command = gcnew Input::RoutedUICommand(
				CLRTools::ToString(CurCommand.LocalizedName),
				// Only for .xaml visibility, not used in our case.
				"CustomObjectCommand",
				InTypeID
				);

			InOutCommandBindingList->Add(
				gcnew Input::CommandBinding(
					NewMenuItem->Command,
					gcnew Input::ExecutedRoutedEventHandler(this, &MDataBrowserControl::OnCustomObjectCommand),
					gcnew Input::CanExecuteRoutedEventHandler(this, &MDataBrowserControl::CanExecuteCustomObjectCommand)
					)
					);


			// Store the ID so we can map back to the object type's command list
			NewMenuItem->CommandParameter = CurCommand.CommandID;

			// If this is the "Show Object Editor" command, then we'll make it bold since that's
			// the default action when double-clicking on an asset item.
			const UBOOL IsDefaultCommand = (InDefaultCommandId != INDEX_NONE && CurCommand.CommandID == InDefaultCommandId);
			if( IsDefaultCommand )
			{
				NewMenuItem->FontWeight = FontWeights::Bold;
			}

			// Set whether the command should appear enabled or not
			NewMenuItem->IsEnabled = CurCommand.bIsEnabled;

			// Remember the item in case it is a parent
			MenuItems->Add( CurCommandIndex, NewMenuItem );

			if( CurCommand.ParentIndex == -1 )
			{
				// Add the new menu item to the list
				OutMenuItems->Add( NewMenuItem );
			}
			else
			{
				MenuItem^ ParentMenuItem = MenuItems[CurCommand.ParentIndex];
				if( ParentMenuItem )
				{
					ParentMenuItem->Items->Add( NewMenuItem );
				}
			}
		}
		else
		{
			// Command is set to -1 so we'll consider this a separator
			Separator^ NewSeparator = gcnew Separator();

			OutMenuItems->Add( NewSeparator );
		}
	}
}


/** Syncs the currently selected objects in the asset view with the engine's global selection set */
void MDataBrowserControl::SyncSelectedObjectsWithGlobalSelectionSet()
{
	USelection* EditorSelection = GEditor->GetSelectedObjects();
	check( EditorSelection != NULL );

	EditorSelection->BeginBatchSelectOperation();
	{
		TSet< UObject* > SelectedObjects;
		// Lets see what the user has selected and add any new selected objects to the global selection set
		for each( ContentBrowser::DataAssetItem^ CurSelectedAsset in DataBrowserCtrl->AssetView->AssetListView->SelectedItems )
		{
			// Grab the object
			UObject* FoundObject = NULL;
			UClass* AssetClass = FindObject<UClass>( ANY_PACKAGE, *CLRTools::ToFString( CurSelectedAsset->AssetType ) );
			if( AssetClass != NULL )
			{
				FoundObject = UObject::StaticFindObject( AssetClass, ANY_PACKAGE, *CLRTools::ToFString( CurSelectedAsset->FullyQualifiedPath ) );
			}
			if( FoundObject != NULL )
			{
				SelectedObjects.Add( FoundObject );

				// Select this object!
				EditorSelection->Select( FoundObject );
			}
		}


		// Now we'll build a list of objects that need to be removed from the global selection set
		for( INT CurEditorObjectIndex = 0; CurEditorObjectIndex < EditorSelection->Num(); ++CurEditorObjectIndex )
		{
			UObject* CurEditorObject = ( *EditorSelection )( CurEditorObjectIndex );
			if( CurEditorObject != NULL )
			{
				if( !SelectedObjects.Contains( CurEditorObject ) )
				{
					EditorSelection->Deselect( CurEditorObject );
				}
			}
		}
	}
	EditorSelection->EndBatchSelectOperation();
}

/**
 * Called after the editor's selection set has been updated
 *
 * @param	ModifiedSelection	the selection that was changed
 */
void MDataBrowserControl::NotifySelectionChanged( USelection* ModifiedSelection )
{
	check(ModifiedSelection);

	USelection* EditorMasterSelection = GEditor->GetSelectedObjects();
	if ( ModifiedSelection == EditorMasterSelection )
	{
		if ( !DataBrowserCtrl->AssetView->AssetListView->IsNotifyingSelectionChange )
		{
			EditorMasterSelection->BeginBatchSelectOperation();
			{
				TArray<UObject*> SelectedObjects;
				ModifiedSelection->GetSelectedObjects(SelectedObjects);

				List<ContentBrowser::DataAssetItem^>^ AssetsToSelect = gcnew List<ContentBrowser::DataAssetItem^>();
				for ( INT ObjIndex = 0; ObjIndex < SelectedObjects.Num(); ObjIndex++ )
				{
					UObject* SelectedObj = SelectedObjects(ObjIndex);
					if ( SelectedObj != NULL )
					{
						String^ AssetFullName = CLRTools::ToString( SelectedObj->GetFullName() );
						ContentBrowser::DataAssetItem^ item = DataBrowserCtrl->AssetView->FindAssetItem( AssetFullName );
						if ( item != nullptr )
						{
							AssetsToSelect->Add(item);
						}
						else
						{
							//@todo cb  [reviewed; discuss]- handle error
						}
					}
				}

				//@todo cb - we should pass true for value of bClearExistingSelection only if we are not the source of this selection change event
				DataBrowserCtrl->AssetView->SelectMultipleAssets(AssetsToSelect, false);
			}
			EditorMasterSelection->EndBatchSelectOperation();
		}
	}
}


/**
 * Queries the list of collection names the specified asset is in (sorted alphabetically).  Note that
 * for private collections, only collections created by the current user will be returned.
 *
 * @param	InFullName			The full name for the asset
 * @param	InType				Type of collection to search for
 *
 * @return	List of collection names, sorted alphabetically
 */
List< String^ >^ MDataBrowserControl::QueryCollectionsForAsset( String^ InFullName, ContentBrowser::EBrowserCollectionType InType )
{
	// Grab the tags for this asset
	List<String^>^ MyObjectTags;

	// oscar@snda
	if(!FGameAssetDatabase::IsInitialized())
		return MyObjectTags;
	// oscar@snda

	FGameAssetDatabase::Get().QueryTagsForAsset(
		InFullName,
		ETagQueryOptions::CollectionsOnly,
		MyObjectTags );

	ESystemTagType CollectionTagType =
		InType == ContentBrowser::EBrowserCollectionType::Shared ? ESystemTagType::SharedCollection : ESystemTagType::PrivateCollection;

	// Figure out which tags are collections and build our list!
	List<String^>^ CollectionNames = gcnew List<String^>();
	for each( String^ CurTag in MyObjectTags )
	{
		if( FGameAssetDatabase::Get().GetSystemTagType( CurTag ) == CollectionTagType )
		{
			// For private collections, make sure that the collection was created by the active user.  Other user's
			// private collections will not be reported
			if( CollectionTagType != ESystemTagType::PrivateCollection || FGameAssetDatabase::Get().IsMyPrivateCollection( CurTag ) )
			{
				String^ CollectionName = FGameAssetDatabase::Get().GetSystemTagValue( CurTag );
				CollectionNames->Add( CollectionName );
			}
		}
	}

	// Sort the tag list alphabetically
	CollectionNames->Sort();

	return CollectionNames;
}




/**
 * Makes sure the currently selected objects are loaded into memory
 */
void MDataBrowserControl::LoadSelectedObjectsIfNeeded()
{
	// Notify DataBrowser when an object has loaded. We need this to catch any dependencies that get loaded so we can update their status.
	TGuardValue<UBOOL> WatchingLoadEnd( GIsWatchingEndLoad, TRUE );

	bool bAnyObjectsWereLoadedOrUpdated = false;

	// Build a list of assets that are not currently loaded.
	List<ContentBrowser::DataAssetItem^>^ UnloadedItems = gcnew List<ContentBrowser::DataAssetItem^>();
	for each( ContentBrowser::DataAssetItem^ CurSelectedAsset in DataBrowserCtrl->AssetView->AssetListView->SelectedItems )
	{
		UObject* FoundObject = NULL;
		UClass* AssetClass = FindObject<UClass>( ANY_PACKAGE, *CLRTools::ToFString( CurSelectedAsset->AssetType ) );
		if( AssetClass != NULL )
		{
			FoundObject = UObject::StaticFindObject( AssetClass, ANY_PACKAGE, *CLRTools::ToFString( CurSelectedAsset->FullyQualifiedPath ) );
		}
		if( FoundObject != NULL )
		{
			// Already loaded; just update its status.
			UpdateAssetStatus( CurSelectedAsset, ContentBrowser::AssetStatusUpdateFlags::LoadedStatus );
			bAnyObjectsWereLoadedOrUpdated = true;
		}
		else
		{
			UnloadedItems->Add(CurSelectedAsset);
		}
	}

	// ask for confirmation if the user is attempting to load a large number of assets
	// @todo cb [reviewed; waiting till we add general support for options] - make this number configurable
	const int NumUnloadedItems = UnloadedItems->Count;
	if ( NumUnloadedItems > 20 )
	{
		String^ Question = UnrealEd::Utils::Localize("DataBrowser_Questions_ConfirmLoadAssets", NumUnloadedItems, Environment::NewLine);
		if ( appMsgf(AMT_YesNo, *CLRTools::ToFString(Question)) == 0 )
		{
			return;
		}
	}

	// Make sure all selected objects are loaded, where possible
	// NOTE: We're not retaining references to these objects outside of this stack frame, so they may
	//		 be purged unless something else grabs them!
	if ( NumUnloadedItems > 0 )
	{
		const UBOOL bShowProgressDialog = NumUnloadedItems > 20;
		GWarn->BeginSlowTask(*CLRTools::ToFString(UnrealEd::Utils::Localize("DataBrowser_TaskProgress_LoadingObjects")), bShowProgressDialog);

		for ( int ItemIdx = 0; ItemIdx < NumUnloadedItems; ItemIdx++ )
		{
			ContentBrowser::DataAssetItem^ CurSelectedAsset = UnloadedItems[ItemIdx];
			if ( IsAssetValidForLoading(CurSelectedAsset->FullyQualifiedPath) )
			{
				// We never want to follow redirects when loading objects for the Content Browser.  It would
				// allow a user to interact with a ghost/unverified asset as if it were still alive.
				const ELoadFlags LoadFlags = LOAD_NoRedirects;

				// Load up the object
				UClass* AssetClass = FindObject<UClass>(ANY_PACKAGE, *CLRTools::ToFString(CurSelectedAsset->AssetType));
				if( AssetClass == NULL )
				{
					AssetClass = UObject::StaticClass();
				}
				const FString ObjectPath = CLRTools::ToFString( CurSelectedAsset->FullyQualifiedPath );
				UObject* CurObject = UObject::StaticLoadObject( AssetClass, NULL, *ObjectPath, NULL, LoadFlags, NULL );
				if( CurObject != NULL )
				{
					// Update the loaded status.  This may queue rendering of a thumbnail for newly-loaded (visible) assets.
					FCallbackEventParameters Parms(NULL, CALLBACK_RefreshContentBrowser, CBR_UpdateAssetListUI, CurObject);
					GCallbackEvent->Send(Parms);

					// we've loaded a new asset - it may have been in a group that was previously unloaded.  Since we don't add group nodes
					// for packages/groups which aren't loaded, we'll need to do that manually now.  And we may end up doing nothing here if
					// this object wasn't in a group or other objects in that group were already loaded.
					UPackage* TopLevelPackage = CurObject->GetOutermost();
					ContentBrowser::Package^ TopLevelPackageNode = dynamic_cast<ContentBrowser::Package^>(UPackageToPackage(TopLevelPackage));
					
					// If there is no toplevel package is non-existent then this package is invalid for the packages tree.
					if ( TopLevelPackageNode != nullptr )
					{
						// the new asset being loaded might have been the only item not loaded in its top level package; if so, that package is
						// now "fully loaded" and the package list needs a UI update
						if ( TopLevelPackage->IsFullyLoaded() )
						{
							GCallbackEvent->Send( FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_UpdatePackageListUI, TopLevelPackage) );
						}
						bAnyObjectsWereLoadedOrUpdated = true;

						// iterate up the Outer chain to build a list of groups in the correct order
						TArray<UPackage*> Groups;
						for ( UObject* NextOuter = CurObject->GetOuter(); NextOuter != NULL && NextOuter != TopLevelPackage; NextOuter = NextOuter->GetOuter() )
						{
							UPackage* NextGroup = Cast<UPackage>(NextOuter);
							if ( NextGroup != NULL )
							{
								Groups.InsertItem(NextGroup, 0);
							}
						}

						// start with the top level package, add a node for each group in the chain if it doesn't already exist
						ContentBrowser::ObjectContainerNode^ NextParentNode = TopLevelPackageNode;
						for ( INT GroupIdx = 0; GroupIdx < Groups.Num(); GroupIdx++ )
						{
							UPackage* NextGroup = Groups(GroupIdx);
							check(NextGroup);

							String^ GroupName = CLRTools::ToString(NextGroup->GetName());
							ContentBrowser::ObjectContainerNode^ GroupNode = NextParentNode->FindPackage(GroupName);
							if ( GroupNode == nullptr )
							{
								GroupNode = NextParentNode->AddChildNode<ContentBrowser::GroupPackage^>(gcnew ContentBrowser::GroupPackage(NextParentNode, GroupName));
							}

							if ( GroupNode != nullptr )
							{
								NextParentNode = GroupNode;
							}
							else
							{
								break;
							}
						}
					}
				}
				else
				{
					// Load failed for some reason.  Play a warning message.
					String^ Warning = UnrealEd::Utils::Localize( "DataBrowser_Warning_LoadObjectFailed", CurSelectedAsset->FullyQualifiedPath );
					DataBrowserCtrl->PlayWarning( Warning );

					// Add the 'unverified' tag to this asset (locally)
					bool bIsAuthoritative = true;
					// oscar@snda
					if(FGameAssetDatabase::IsInitialized())
					{

						FGameAssetDatabase::Get().AddTagMapping(
							CurSelectedAsset->FullName,
							FGameAssetDatabase::Get().MakeSystemTag( ESystemTagType::Unverified, "" ),
							bIsAuthoritative );
					}
					// oscar@snda
					// Mark the asset item as unverified directly (if we have one)
					CurSelectedAsset->IsVerified = false;
				}
			}

			if ( bShowProgressDialog )
			{
				GWarn->UpdateProgress(ItemIdx, NumUnloadedItems);
			}
		}
		GWarn->EndSlowTask();
	}
	// If we loaded or updated anything, make sure to re-sync our selected object set so that we have the full picture
	if( bAnyObjectsWereLoadedOrUpdated )
		{
			SyncSelectedObjectsWithGlobalSelectionSet();
		}

	}



/** Returns any serializable Unreal Objects this object is currently holding on to */
void MDataBrowserControl::QuerySerializableObjects( TArray< UObject* >& OutObjects )
{
	OutObjects.Reset();

	if ( ClassToBrowsableObjectTypeMap.IsValid() )
	{
		ClassToGBTMap& MapRef = GetBrowsableObjectTypeMap();
		for ( ClassToGBTMap::TIterator It(MapRef); It; ++It )
		{
			UClass* Cls = It.Key();
			OutObjects.AddUniqueItem(Cls);
		}
	}


	// Browsable object types
	if( BrowsableObjectTypeList.IsValid() )
	{
		for( INT CurObjectIndex = 0; CurObjectIndex < BrowsableObjectTypeList->Num(); ++CurObjectIndex )
		{
			OutObjects.AddItem( ( *BrowsableObjectTypeList.Get() )( CurObjectIndex ) );
		}
	}


	// Referenced objects
	if( ReferencedObjects.IsValid() )
	{
		TArray< UObject* >& ObjectList = *ReferencedObjects.Get();
		for( INT CurObjectIndex = 0; CurObjectIndex < ObjectList.Num(); ++CurObjectIndex )
		{
			OutObjects.AddItem( ObjectList( CurObjectIndex ) );
		}
	}

	if ( CallbackEventObjects )
	{
		const TArray<UObject*>& ObjectList = *(CallbackEventObjects.Get());
		for ( INT ObjIndex = 0; ObjIndex < ObjectList.Num(); ObjIndex++ )
		{
			OutObjects.AddItem(ObjectList(ObjIndex));
		}
	}

	if ( CallbackEventPackages )
	{
		const TArray<UPackage*>& PackageList = *(CallbackEventPackages.Get());
		for ( INT ObjIndex = 0; ObjIndex < PackageList.Num(); ObjIndex++ )
		{
			OutObjects.AddItem(PackageList(ObjIndex));
		}
	}


	if ( CallbackEventSyncObjects )
	{
		const TArray<UObject*>& ObjectList = *(CallbackEventSyncObjects.Get());
		for ( INT ObjIndex = 0; ObjIndex < ObjectList.Num(); ObjIndex++ )
		{
			OutObjects.AddItem(ObjectList(ObjIndex));
		}
	}

}



/** Called every main engine loop update */
void MDataBrowserControl::Tick()
{
	// don't perform any updates while PIE is running as some of these functions can cause hitching
	if ( !GIsPlayInEditorWorld )
	{
		bPackageFilterUpdateRequested =
			bPackageFilterUpdateRequested ||
			bPackageListUpdateRequested || bPackageListUpdateUIRequested || bIsSCCStateDirty;
		
		ConditionalUpdateCollections();
		ConditionalUpdatePackages();
		ConditionalUpdateSCC(NULL);
		ConditionalUpdatePackageFilter();

 		ConditionalUpdateAssetView();
 		UpdateAssetQuery();		

		

		// Tell the asset view to update thumbnails and such
 		bool bIsAssetQueryInProgress = ( QueryEngineState != EDataBrowserQueryState::Idle );
 		DataBrowserCtrl->AssetView->FilterRefreshTick( bIsAssetQueryInProgress );
 		DataBrowserCtrl->AssetView->AssetCanvas->UpdateAssetCanvas();
 
 
 		// Select any assets that were deferred if we need to
 		if( !bAssetViewUpdateRequested &&
 			QueryEngineState == EDataBrowserQueryState::Idle )
 		{
 			DataBrowserCtrl->AssetView->SelectDeferredAssetsIfNeeded();
 		}
	}
}



/** Called right before an engine garbage collection occurs */
void MDataBrowserControl::OnPendingGarbageCollection()
{
	// the content browser needs to free up the cached object lists because they can 
	// keep worlds from unloading, etc, if CB callbacks happen during map save/load operations
	if( ReferencedObjects.IsValid() )
	{
		TArray< UObject* >& ObjectList = *ReferencedObjects.Get();
		ObjectList.Empty();
	}

	if ( CallbackEventObjects )
	{
		TArray<UObject*>& ObjectList = *(CallbackEventObjects.Get());
		ObjectList.Empty();
	}

	if ( CallbackEventPackages )
	{
		TArray<UPackage*>& PackageList = *(CallbackEventPackages.Get());
		PackageList.Empty();
	}


	if ( CallbackEventSyncObjects )
	{
		TArray<UObject*>& ObjectList = *(CallbackEventSyncObjects.Get());
		ObjectList.Empty();
	}




	// Is there a query in progress?  If so, we may be holding onto UObjects (soft reference) that
	// will be deleted, so we'll enqueue a new query so that the object list will be refreshed.
	if( QueryEngineState != EDataBrowserQueryState::Idle )
	{
		AssetQuery_RestartQuery();
	}
}



/** Called when the user opts to "activate" an asset item (usually by double clicking on it) */
void MDataBrowserControl::LoadAndActivateSelectedAssets()
{
	// Make sure the objects are loaded
	// This implicitly synchronizes selection with the global selection set.
	LoadSelectedObjectsIfNeeded();


	INT CommandToExecute;
	// Figure out which commands are supported by the current selection.
	TArray<FObjectSupportedCommandType> SupportedCommands;
	QuerySupportedCommands( SupportedCommands, CommandToExecute ); // We will execute the default command.
	
	ExecuteCustomObjectCommand( CommandToExecute );
}

/**
 *  Called when the user opts to view the properties of asset items via hot-key; only displays properties if all of
 *  the selected assets do not have a specific editor for setting their attributes
 */
void MDataBrowserControl::LoadAndDisplayPropertiesForSelectedAssets()
{
	// Make sure the objects are loaded
	LoadSelectedObjectsIfNeeded();

	// Figure out which commands are supported by the current selection
	INT CommandToExecute;
	TArray<FObjectSupportedCommandType> SupportedCommands;
	QuerySupportedCommands( SupportedCommands, CommandToExecute );

	// Check if the selected assets support displaying their properties (that is, they do not have a custom editor of their own)
	for ( TArray<FObjectSupportedCommandType>::TConstIterator CommandIterator(SupportedCommands); CommandIterator; ++CommandIterator )
	{
		const FObjectSupportedCommandType& CurCommand = *CommandIterator;
		if ( CurCommand.CommandID == IDMN_ObjectContext_Properties )
		{
			// Explicitly execute the properties command, as some assets could support displaying properties but have a different 
			// default command
			ExecuteCustomObjectCommand( IDMN_ObjectContext_Properties );
			break;
		}
	}
}


/**
 * Called when the user activates the preview action on an asset (not all assets support preview; currently used for sounds)
 * 
 * @param ObjectFullName		Full name of asset to preview
 * 
 * @return true if the preview process started; false if the action actually stopped an ongoing preview process.
 */
bool MDataBrowserControl::PreviewLoadedAsset( String^ ObjectFullName )
{
	bool StartedPlaying = false;
	
	if( ObjectFullName != nullptr )
	{
		const FString ClassName = CLRTools::ToFString( UnrealEd::Utils::GetClassNameFromFullName( ObjectFullName ) );
		const FString ObjectPathName = CLRTools::ToFString( UnrealEd::Utils::GetPathFromFullName( ObjectFullName ) );

		// Grab the object
		UObject* FoundObject = NULL;
		UClass* AssetClass = FindObject<UClass>( ANY_PACKAGE, *ClassName );
		if( AssetClass != NULL )
		{
			FoundObject = UObject::StaticFindObject( AssetClass, ANY_PACKAGE, *ObjectPathName );
		}
		if( FoundObject != NULL )
		{		
			// Preview is currently only supported for sounds, sound cues, etc.
			const TArray< UGenericBrowserType* >* BrowsableObjectTypes = ClassToBrowsableObjectTypeMap->Find( FoundObject->GetClass() );
			if( BrowsableObjectTypes != NULL )
			{
				for( INT CurBrowsableTypeIndex = 0; CurBrowsableTypeIndex < BrowsableObjectTypes->Num(); ++CurBrowsableTypeIndex )
				{						
					UGenericBrowserType* CurObjType = ( *BrowsableObjectTypes )( CurBrowsableTypeIndex );
					if( CurObjType->IsA( UGenericBrowserType_Sounds::StaticClass() ) )
					{
						// If we are previewing the same asset as last time, we should just stop the preview.
						bool SameAssetAsLastTime = String::Equals( ObjectFullName, LastPreviewedAssetFullName, System::StringComparison::OrdinalIgnoreCase );

						UGenericBrowserType_Sounds* SoundsBrowserType = Cast<UGenericBrowserType_Sounds>(CurObjType);

						USoundCue*	SoundCue = Cast<USoundCue>( FoundObject );
						USoundNode* SoundNodeWave = Cast<USoundNodeWave>( FoundObject );
						if( SoundCue )
						{
							if (SoundsBrowserType->IsPlaying(SoundCue) && SameAssetAsLastTime)
							{
								SoundsBrowserType->Stop();
							}
							else
							{
								StartedPlaying = true;
								Cast<UGenericBrowserType_Sounds>(CurObjType)->Play( SoundCue );
								LastPreviewedAssetFullName = ObjectFullName;
							}								
							break;
						}
						else if( SoundNodeWave )
						{
							if (SoundsBrowserType->IsPlaying(SoundNodeWave) && SameAssetAsLastTime)
							{
								SoundsBrowserType->Stop();
							}
							else
							{
								StartedPlaying = true;
								Cast<UGenericBrowserType_Sounds>(CurObjType)->Play( SoundNodeWave );
								LastPreviewedAssetFullName = ObjectFullName;
							}								
							break;
						}
					}
				}
			}
		}
	}

	return StartedPlaying;
}

	
	
/** Called when an object is modified (PostEditChange) */
void MDataBrowserControl::OnObjectModification( UObject* InObject )
{
	OnObjectModification(InObject, ContentBrowser::AssetStatusUpdateFlags::LoadedStatus);
}

void MDataBrowserControl::OnObjectModification( UObject* InObject, ContentBrowser::AssetStatusUpdateFlags UpdateFlags )
{
	check( InObject != NULL );

	BOOL bNeedsUpdate = InObject->GetOuter() != NULL && InObject->GetOuter()->GetClass() == UPackage::StaticClass();
	// @todo cb  [reviewed; discuss]: We do not bother updating the status of anything that is not an inner.
	if ( bNeedsUpdate )
	{

		// An object in memory was modified.  We'll mark it's thumbnail as dirty so that it'll be
		// regenerated on demand later. (Before being displayed in the browser, or package saves, etc.)
		FObjectThumbnail* Thumbnail = ThumbnailTools::GetThumbnailForObject( InObject );
		if( Thumbnail != NULL )
		{
			// Mark the thumbnail as dirty
			Thumbnail->MarkAsDirty();
		}
		else
		{
			// No thumbnail exists yet.
		}

		// Initiate a deferred refresh of the asset visual for this object
		{
			// Do we even have an asset item for this object right now?
			String^ AssetFullName = CLRTools::ToString( InObject->GetFullName() );
			ContentBrowser::DataAssetItem^ AssetItemForObject = DataBrowserCtrl->AssetView->FindAssetItem( AssetFullName );
			if( AssetItemForObject != nullptr )
			{
				if( AssetItemForObject->Visual != nullptr )
				{
					AssetItemForObject->Visual->ShouldRefreshThumbnail = ContentBrowser::ThumbnailGenerationPolicy::Force;
				}


				// Update the asset item's loaded/modified status too!
				UpdateAssetStatus( AssetItemForObject, UpdateFlags );


				// Also update it's custom label and memory data
				{
					AssetItemForObject->MarkCustomLabelsAsDirty();
					AssetItemForObject->MarkCustomDataColumnsAsDirty();
					AssetItemForObject->MarkMemoryUsageAsDirty();

					// The user might be looking at the asset right now, so we'll force a refresh
					AssetItemForObject->UpdateCustomLabelsIfNeeded();
					AssetItemForObject->UpdateCustomDataColumnsIfNeeded();
					AssetItemForObject->UpdateMemoryUsageIfNeeded();
				}
			}
		}
	}
}


void MDataBrowserControl::GetPackagesForSelectedAssetItems( TArray<UPackage*>& out_Packages, TArray<FString>& out_PackageNames )
{
	for each( ContentBrowser::DataAssetItem^ CurSelectedAsset in DataBrowserCtrl->AssetView->AssetListView->SelectedItems )
	{
		const FString PackageString = *CLRTools::ToFString( CurSelectedAsset->PackageName );

		// Selected assets may be in the same package, only add the package once.
		out_PackageNames.AddUniqueItem(*PackageString);
		out_Packages.AddUniqueItem(FindObject<UPackage>(NULL, *PackageString));
	}
}

/**
 * Generates a WPF bitmap for the specified thumbnail
 * 
 * @param	InThumbnail		The thumbnail object to create a bitmap for
 * 
 * @return	Returns the BitmapSource for the thumbnail, or null on failure
 */
Imaging::BitmapSource^ MDataBrowserControl::CreateBitmapSourceForThumbnail( const FObjectThumbnail& InThumbnail )
{
	PROFILE_CONTENT_BROWSER( UnrealEd::ScopedPerfTimer MyTimer( "BitmapSource.Create" ) );

	return ThumbnailToolsCLR::CreateBitmapSourceForThumbnail(InThumbnail);
}


/**
 * Updates a writeable WPF bitmap with the specified thumbnail's data
 * 
 * @param	InThumbnail		The thumbnail object to create a bitmap for
 */
void MDataBrowserControl::CopyThumbnailToWriteableBitmap( const FObjectThumbnail& InThumbnail, Imaging::WriteableBitmap^ WriteableBitmap )
{
	PROFILE_CONTENT_BROWSER( UnrealEd::ScopedPerfTimer MyTimer( "Copy to WriteableBitmap" ) );

	ThumbnailToolsCLR::CopyThumbnailToWriteableBitmap(InThumbnail,WriteableBitmap);
}


/**
 * Generates a writeable WPF bitmap for the specified thumbnail
 * 
 * @param	InThumbnail		The thumbnail object to create a bitmap for
 * 
 * @return	Returns the BitmapSource for the thumbnail, or null on failure
 */
Imaging::WriteableBitmap^ MDataBrowserControl::CreateWriteableBitmapForThumbnail( const FObjectThumbnail& InThumbnail )
{
	PROFILE_CONTENT_BROWSER( UnrealEd::ScopedPerfTimer MyTimer( "WriteableBitmap.Create" ) );

	return ThumbnailToolsCLR::CreateWriteableBitmapForThumbnail(InThumbnail);
}



/** Returns true if the specified asset uses a stock thumbnail resource */
bool MDataBrowserControl::AssetUsesSharedThumbnail( ContentBrowser::DataAssetItem^ Asset )
{
	// check whether this is a type that uses one of the shared static thumbnails
	UBOOL bUsesSharedThumbnail = FALSE;
	FString ObjectClassName = CLRTools::ToFString(Asset->AssetType);
	UClass* ObjectClass = FindObject<UClass>(ANY_PACKAGE, *ObjectClassName, TRUE);
	if ( ObjectClass != NULL && SharedThumbnailClasses.IsValid() )
	{
		for ( INT Idx = 0; Idx < SharedThumbnailClasses->Num(); Idx++ )
		{
			UClass* Cls = (*SharedThumbnailClasses.Get())(Idx);
			if ( ObjectClass->IsChildOf(Cls) )
			{
				bUsesSharedThumbnail = TRUE;
				break;
			}
		}
	}

	// Archetype objects always use a shared thumbnail
	if( Asset->IsArchetype )
	{
		bUsesSharedThumbnail = TRUE;
	}

	return bUsesSharedThumbnail ? true : false;
}

/**
 * Capture active viewport to thumbnail and assigns that thumbnail to incoming assets
 *
 * @param InViewport - viewport to sample from
 * @param InAssetsToAssign - assets that should receive the new thumbnail ONLY if they are assets that use SharedThumbnails
 */
void MDataBrowserControl::CaptureThumbnailFromViewport (FViewport* InViewport, List<ContentBrowser::DataAssetItem^>^ InAssetsToAssign)
{

	//capture the thumbnail
	DWORD SrcWidth = InViewport->GetSizeX();
	DWORD SrcHeight = InViewport->GetSizeY();
	// Read the contents of the viewport into an array.
	TArray<FColor> OrigBitmap;
	if (InViewport->ReadPixels(OrigBitmap))
	{
		check(OrigBitmap.Num() == SrcWidth * SrcHeight);

		//pin to smallest value
		INT CropSize = Min<DWORD>(SrcWidth, SrcHeight);
		//pin to max size
		INT ScaledSize  = Min<DWORD>(ThumbnailTools::DefaultThumbnailSize, CropSize);


		//calculations for cropping
		TArray<FColor> CroppedBitmap;
		CroppedBitmap.Add(CropSize*CropSize);
		//Crop the image
		INT CroppedSrcTop  = (SrcHeight - CropSize)/2;
		INT CroppedSrcLeft = (SrcWidth - CropSize)/2;
		for (INT Row = 0; Row < CropSize; ++Row)
		{
			//Row*Side of a row*byte per color
			INT SrcPixelIndex = (CroppedSrcTop+Row)*SrcWidth + CroppedSrcLeft;
			const void* SrcPtr = &(OrigBitmap(SrcPixelIndex));
			void* DstPtr = &(CroppedBitmap(Row*CropSize));
			appMemcpy(DstPtr, SrcPtr, CropSize*4);
		}

		//Scale image down if needed
		TArray<FColor> ScaledBitmap;
		if (ScaledSize < CropSize)
		{
			FImageUtils::ImageResize( CropSize, CropSize, CroppedBitmap, ScaledSize, ScaledSize, ScaledBitmap, TRUE );
		}
		else
		{
			//just copy the data over. sizes are the same
			ScaledBitmap = CroppedBitmap;
		}

		//setup actual thumbnail
		FObjectThumbnail TempThumbnail;
		TempThumbnail.SetImageSize( ScaledSize, ScaledSize );
		TArray<BYTE>& ThumbnailByteArray = TempThumbnail.AccessImageData();

		// Copy scaled image into destination thumb
		INT MemorySize = ScaledSize*ScaledSize*sizeof(FColor);
		ThumbnailByteArray.Add(MemorySize);
		appMemcpy(&(ThumbnailByteArray(0)), &(ScaledBitmap(0)), MemorySize);


		//check if each asset should receive the new thumb nail
		for each ( ContentBrowser::DataAssetItem^ CurrentAsset in InAssetsToAssign )
		{
			// check whether this is a type that uses one of the shared static thumbnails
			if (AssetUsesSharedThumbnail( CurrentAsset ))
			{
				//assign the thumbnail and dirty
				const FString ObjectPathName = CLRTools::ToFString( CurrentAsset->FullName );
				const FString PackageName    = CLRTools::ToFString( CurrentAsset->PackageName );

				UPackage* AssetPackage = FindObject<UPackage>( NULL, *PackageName );
				check(AssetPackage);

				FObjectThumbnail* NewThumbnail = ThumbnailTools::CacheThumbnail( ObjectPathName, &TempThumbnail, AssetPackage);
				check(NewThumbnail);

				//we need to indicate that the package needs to be resaved
				AssetPackage->MarkPackageDirty();

				// Let the content browser know that we've changed the thumbnail
				NewThumbnail->MarkAsDirty();
				if (CurrentAsset->Visual)
				{
					CurrentAsset->Visual->ShouldRefreshThumbnail = ContentBrowser::ThumbnailGenerationPolicy::Force;
				}

				//Set that thumbnail as a valid custom thumbnail so it'll be saved out
				NewThumbnail->SetCreatedAfterCustomThumbsEnabled();
			}
		}
	}
}

/**
 * Clears custom thumbnails for the selected assets
 *
 * @param InAssetsToAssign - assets that should have their thumbnail cleared
 */
void MDataBrowserControl::ClearCustomThumbnails (List<ContentBrowser::DataAssetItem^>^ InAssetsToAssign)
{
	//check if each asset should receive the new thumb nail
	for each ( ContentBrowser::DataAssetItem^ CurrentAsset in InAssetsToAssign )
	{
		// check whether this is a type that uses one of the shared static thumbnails
		if (AssetUsesSharedThumbnail( CurrentAsset ))
		{
			//assign the thumbnail and dirty
			const FString ObjectPathName = CLRTools::ToFString( CurrentAsset->FullName );
			const FString PackageName    = CLRTools::ToFString( CurrentAsset->PackageName );

			UPackage* AssetPackage = FindObject<UPackage>( NULL, *PackageName );
			check(AssetPackage);

			ThumbnailTools::CacheEmptyThumbnail( ObjectPathName, AssetPackage);

			//we need to indicate that the package needs to be resaved
			AssetPackage->MarkPackageDirty();

			// Let the content browser know that we've changed the thumbnail
			if (CurrentAsset->Visual)
			{
				CurrentAsset->Visual->ShouldRefreshThumbnail = ContentBrowser::ThumbnailGenerationPolicy::Force;
			}
		}
	}
}

void MDataBrowserControl::FilterSelectedObjectTypes ( List<ContentBrowser::DataAssetItem^>^ SelectedItems)
{
    // Get the list of currently selected options
    List<String^>^ SelectedOptions = DataBrowserCtrl->FilterPanel->ObjectTypeFilterTier->GetSelectedOptions();
    // Create a list of item classes from the selected items
    List<String^>^ SelectedItemClasses = gcnew List<String^>();
    for each (ContentBrowser::DataAssetItem^ Item in SelectedItems)
    {
        if( !SelectedItemClasses->Contains(Item->AssetType))
        {
            SelectedItemClasses->Add(Item->AssetType);
        }
    }
    // Get our new items from the list of selected classes
    List<String^>^ NewSelectedOptions = GetBrowsableTypeNameList(SelectedItemClasses);
    // Compare the Selected Options and new options; if there are any duplicates, remove them from the new item list
    for (int NewOptionIdx = 0; NewOptionIdx < NewSelectedOptions->Count; NewOptionIdx++)
    {
        if (SelectedOptions->Contains(NewSelectedOptions[NewOptionIdx]))
        {
            SelectedOptions->Remove(NewSelectedOptions[NewOptionIdx]);
        }
        else
        {
            SelectedOptions->Add(NewSelectedOptions[NewOptionIdx]);
        }
    }

    DataBrowserCtrl->FilterPanel->ObjectTypeFilterTier->SetSelectedOptions(SelectedOptions);
}

/**
 * Attempts to generate a thumbnail for the specified object
 * 
 * @param	Asset	the asset that needs a thumbnail
 * @param	OutFailedToLoadThumbnail	True if we tried to load the asset's thumbnail from disk and couldn't find it.
 * @param	CheckSharedThumbnailAssets	True if we should even check to see if assets that use a stock thumbnail resource exist in the package file
 * 
 * @return	Returns the BitmapSource for the generated thumbnail, or null if a thumbnail is not available
 */
Imaging::BitmapSource^ MDataBrowserControl::GenerateThumbnailForAsset( ContentBrowser::DataAssetItem^ Asset, bool% OutFailedToLoadThumbnail )
{
	Imaging::BitmapSource^ MyBitmapSource = nullptr;
	OutFailedToLoadThumbnail = false;

	const FString ObjectPathName = CLRTools::ToFString( Asset->FullyQualifiedPath );

	// check whether this is a type that uses one of the shared static thumbnails
	const UBOOL bUsesSharedThumbnail = AssetUsesSharedThumbnail( Asset );

	// Declared at this scope because we may have a pointer pointing to some data here
	FThumbnailMap LoadedThumbnails;

	const FString ObjectFullName = CLRTools::ToFString( Asset->FullName );

	// Check to see if the thumbnail is already in memory for this object.  Note that the object itself
	// may not even be loaded right now.
	const FObjectThumbnail* FoundThumbnail = ThumbnailTools::FindCachedThumbnail( ObjectFullName );
		
	// We don't care about empty thumbnails here
	if( FoundThumbnail != NULL && FoundThumbnail->IsEmpty() )
	{
		FoundThumbnail = NULL;
	}
	
	// If we didn't find it in memory, OR if the thumbnail needs to be refreshed...
	if( FoundThumbnail == NULL || FoundThumbnail->IsDirty() )
	{
		PROFILE_CONTENT_BROWSER( UnrealEd::ScopedPerfTimer MyTimer( "Render Low Res Thumbnail" ) );

		UObject* FoundObject = NULL;
		// Try to find the object.
		FString ObjectClassName = CLRTools::ToFString(Asset->AssetType);
		UClass* ObjectClass = FindObject<UClass>(ANY_PACKAGE, *ObjectClassName, TRUE);
		FoundObject = UObject::StaticFindObject( ObjectClass, ANY_PACKAGE, *ObjectPathName );
		
		if ( !bUsesSharedThumbnail )
		{
			//If it's in memory we can go ahead and generate a thumbnail for it now!
			if( FoundObject != NULL )
			{
				// NOTE: We don't want to mark the package as dirty since that would result in many packages
				//    being dirtied (unexpected by the user.)  Instead, we'll generate thumbnails for new/dirty
				//    objects when the package is saved in the editor

				// Generate a thumbnail!
				FoundThumbnail = ThumbnailTools::GenerateThumbnailForObject( FoundObject );
				if( FoundThumbnail == NULL )
				{
					// Couldn't generate a thumb; perhaps this object doesn't support thumbnails?
				}
			}
		}

		//still haven't found a thumb nail AND it's not in memory
		if( FoundThumbnail == NULL )
		{
			//FOR SHARED THUMBNAILS
			// For share thumbnails, we usually don't need to load anything.  However, if the asset is
			// flagged as "unverified", then we'll scan the package file on disk to make sure the
			// asset actually exists.  This allows the UI to hide assets that don't exist.

			//FOR NON-SHARED THUMBNAILS
			// Object isn't loaded right now, so we'll try to quickly pull the thumbnail out of
			// the asset's package file.  We do this without ever actually loading the package or
			// even creating a linker resource.

			// @todo CB: Batch up requests for multiple thumbnails!
			TArray< FName > ObjectFullNames;
			FName ObjectFullNameFName( *ObjectFullName );
			ObjectFullNames.AddItem( ObjectFullNameFName );

			// Load thumbnails
			if( ThumbnailTools::LoadThumbnailsForObjects( ObjectFullNames, LoadedThumbnails ) )
			{
				FoundThumbnail = LoadedThumbnails.Find( ObjectFullNameFName );
				if( FoundThumbnail == NULL )
				{
					// Thumbnails were loaded, but one of the thumbs we were looking for failed to load
					OutFailedToLoadThumbnail = true;
					if( FoundObject == NULL )
					{
						// If the thumbnail failed to load and the object could not be found
						// we cant verify the object exists.
						Asset->IsVerified = false;
					}
				}
			}
			else
			{
				// Couldn't load thumbnail data
				OutFailedToLoadThumbnail = true;
			}
		}

		//if this is a shared type and had an old custom thumb, mark it invalid
		if (bUsesSharedThumbnail && FoundThumbnail && !FoundThumbnail->IsCreatedAfterCustomThumbsEnabled())
		{
			//Casting away const to save memory behind the scenes
			FObjectThumbnail* ThumbToClear = (FObjectThumbnail*)FoundThumbnail;
			ThumbToClear->SetImageSize(0, 0);
			ThumbToClear->AccessImageData().Empty();

			//NOTE - not caching a NULL thumb here because the package might not be loaded

			//report that this was a failure
			FoundThumbnail = NULL;

			//we've found that it is indeed on disk.  Therefore it's verified
			Asset->IsVerified = true;
		}

	}

	if( FoundThumbnail != NULL )
	{
		// Create a WPF bitmap object for the thumbnail
		MyBitmapSource = CreateBitmapSourceForThumbnail( *FoundThumbnail );
		if( MyBitmapSource == nullptr )
		{
			// Couldn't create the bitmap for some reason
		}
	}


	return MyBitmapSource;
}



/**
 * Attempts to generate a *preview* thumbnail for the specified object
 * 
 * @param	ObjectFullName Full name of the object
 * @param	PreferredSize The preferred resolution of the thumbnail, or 0 for "lowest possible"
 * @param	IsAnimating True if the thumbnail will be updated frequently
 * @param	ExistingThumbnail The current preview thumbnail for the asset, if any
 * 
 * @return	Returns the BitmapSource for the generated thumbnail, or null if a thumbnail is not available
 */
Imaging::BitmapSource^ MDataBrowserControl::GeneratePreviewThumbnailForAsset( String^ ObjectFullName, int PreferredSize, bool IsAnimating, Imaging::BitmapSource^ ExistingThumbnail )
{
	check( PreferredSize > 0 );

	Imaging::BitmapSource^ MyBitmapSource = nullptr;

	const FString ClassName = CLRTools::ToFString( UnrealEd::Utils::GetClassNameFromFullName( ObjectFullName ) );
	const FString ObjectPathName = CLRTools::ToFString( UnrealEd::Utils::GetPathFromFullName( ObjectFullName ) );


	// Generate a real-time preview thumbnail if we need to
	FObjectThumbnail TempPreviewThumbnailData;


	PROFILE_CONTENT_BROWSER( UnrealEd::ScopedPerfTimer MyTimer( "Render Preview Res Thumbnail" ) );

	const FObjectThumbnail* FoundThumbnail = NULL;


	// Try to find the object.  If it's in memory we can go ahead and generate a thumbnail for it now!
	UObject* FoundObject = NULL;
	UClass* AssetClass = FindObject<UClass>( ANY_PACKAGE, *ClassName );
	if( AssetClass != NULL )
	{
		FoundObject = UObject::StaticFindObject( AssetClass, ANY_PACKAGE, *ObjectPathName );
	}
	if( FoundObject != NULL )
	{
		// Does the object support thumbnails?
		if( GUnrealEd->GetThumbnailManager()->GetRenderingInfo( FoundObject ) != NULL )
		{
			// Never create a thumbnail larger than 2048, and choose a power of two resolution that's
			// at least as large as the requested size
			INT ImageWidth = 2048;
			INT ImageHeight = 2048;
			while( ImageWidth > ContentBrowser::AssetCanvasDefs::NormalThumbnailResolution &&
				   ImageWidth / 2 >= PreferredSize )
			{
				ImageWidth /= 2;
				ImageHeight /= 2;
			}

			// We never care to flush texture streaming while generating dynamic thumbnails as they won't be
			// saved out to disk anyway
			ThumbnailTools::EThumbnailTextureFlushMode::Type TextureFlushMode = ThumbnailTools::EThumbnailTextureFlushMode::NeverFlush;

			// Generate the thumbnail
			ThumbnailTools::RenderThumbnail(
				FoundObject,
				ImageWidth,
				ImageHeight,
				TextureFlushMode,
				TempPreviewThumbnailData );		// Out

			// This memory will be discarded when the function returns and we'll only keep the
			// bitmap source that we generated from it (down below)
			FoundThumbnail = &TempPreviewThumbnailData;
		}
	}


	if( FoundThumbnail != NULL )
	{
		if( IsAnimating )
		{
			Imaging::WriteableBitmap^ WriteableBitmap = nullptr;

			// Check to see if we can use the existing thumbnail data in place instead of allocating new data
			if( ExistingThumbnail != nullptr )
			{
				// Is the existing thumbnail already a writeable bitmap?
				Imaging::WriteableBitmap^ ExistingWriteableBitmap = dynamic_cast< Imaging::WriteableBitmap^ >( ExistingThumbnail );
				if( ExistingWriteableBitmap != nullptr )
				{
					// OK it's writeable, but we also need to make sure the size matches
					if( ExistingWriteableBitmap->Width == FoundThumbnail->GetImageWidth() &&
						ExistingWriteableBitmap->Height == FoundThumbnail->GetImageHeight() )
					{
						// Great, let's use it!
						WriteableBitmap = ExistingWriteableBitmap;
					}
				}
			}

			// Allocate new bitmap data if we need it
			if( WriteableBitmap == nullptr )
			{
				WriteableBitmap = CreateWriteableBitmapForThumbnail( *FoundThumbnail );
			}
			else
			{
				// Otherwise just update the existing bitmap in place
				CopyThumbnailToWriteableBitmap( *FoundThumbnail, WriteableBitmap );
			}

			MyBitmapSource = WriteableBitmap;
		}
		else
		{
			// Create a WPF bitmap object for the thumbnail
			MyBitmapSource = CreateBitmapSourceForThumbnail( *FoundThumbnail );
			if( MyBitmapSource == nullptr )
			{
				// Couldn't create the bitmap for some reason
			}
		}
	}


	return MyBitmapSource;
}

/**
 * Removes the uncompressed image data held by the UPackage for the specified asset.
 *
 * @param	AssetFullName	the Unreal full name for the object to remove the thumbnail data for
 */
void MDataBrowserControl::ClearCachedThumbnailForAsset( String^ AssetFullName )
{
	FString NativeAssetFullName = CLRTools::ToFString( AssetFullName );

	// object isn't loaded - try to find the package for this object
	FString PackageFileName;
	if ( ThumbnailTools::QueryPackageFileNameForObject( NativeAssetFullName, PackageFileName) )
	{
		UPackage* Package = UObject::FindPackage(NULL, *FPackageFileCache::PackageFromPath(*PackageFileName));
		ThumbnailTools::CacheThumbnail( NativeAssetFullName, NULL, Package );
	}
}

/**
 * Applies the currently selected assets to the objects selected in the level viewport.  For example,
 * if a material is selected in the content browser and a surface is selected in the 3D view, this
 * could assign the material to that surface
 */
void MDataBrowserControl::ApplyAssetSelectionToViewport()
{
	// Make sure the assets are loaded and sync'd with the global selection set
	LoadSelectedObjectsIfNeeded();

	USelection* EditorSelection = GEditor->GetSelectedObjects();
	check( EditorSelection != NULL );

	// Do we have a single material selected in the content browser?
	if( EditorSelection->Num() == 1 )
	{
		if( EditorSelection->GetTop( UMaterialInterface::StaticClass() ) != NULL )
		{
			// Apply the material to the currently selected surface(s) (if there are any)
			GUnrealEd->Exec( TEXT("POLY SETMATERIAL") );
		}
	}
}


/**
 * Attempts to save the specified packages; helper function for by e.g. SaveSelectedPackages().
 *
 * @param		InPackages					The content packages to save.
 * @param		bUnloadPackagesAfterSave	If TRUE, unload each package if it was saved successfully.
 * @return									TRUE if all packages were saved successfully, FALSE otherwise.
 */
UBOOL MDataBrowserControl::SavePackages( const TArray<UPackage*>& PackagesToSave, UBOOL bUnloadPackagesAfterSave )
{
	// FaceFX must be closed before saving.
	if ( !CloseFaceFX() )
	{
		return FALSE;
	}

	TArray< UPackage* > PackagesWithExternalRefs;
	FString PackageNames;
	if( PackageTools::CheckForReferencesToExternalPackages( &PackagesToSave, &PackagesWithExternalRefs ) )
	{
		for(INT PkgIdx = 0; PkgIdx < PackagesWithExternalRefs.Num(); ++PkgIdx)
		{
			PackageNames += FString::Printf(TEXT("%s\n"), *PackagesWithExternalRefs( PkgIdx )->GetName());
		}
		UBOOL bProceed = appMsgf( AMT_YesNo, LocalizeSecure( LocalizeUnrealEd("Warning_ExternalPackageRef"), *PackageNames ) );
		if(!bProceed)
		{
			return FALSE;
		}
	}

	FString LastSavePathString = CLRTools::ToFString(LastSavePath);


	TArray< UPackage*> PackagesNotNeedingCheckout;
	UBOOL bResult = FEditorFileUtils::PromptToCheckoutPackages( FALSE, PackagesToSave, &PackagesNotNeedingCheckout );

	if( bResult || PackagesNotNeedingCheckout.Num() > 0 )
	{
		// Even if the user cancelled the checkout dialog, we should still save packages that didn't need to be checked out.
		const TArray< UPackage* >& FinalSaveList = bResult ? PackagesToSave : PackagesNotNeedingCheckout;
		bResult = PackageTools::SavePackages( FinalSaveList, bUnloadPackagesAfterSave, &LastSavePathString, BrowsableObjectTypeList.Get());
		if ( bResult )
		{
			LastSavePath = CLRTools::ToString(LastSavePathString);
		}
	}

	return bResult;
}

/**
 * Display the specified objects in the content browser
 *
 * @param	InObjects	One or more objects to display
 *
 * @return	True if the package selection changed; false if the currently selected package is already selected.
 */
bool MDataBrowserControl::SyncToObjects( TArray< UObject* >& InObjects )
{
	bool PackageSyncWasNecessary = false;
	if( InObjects.Num() > 0 )
	{
		// Perform a test on the given objects to see if they will visible according to the current filter options.
		if( DoObjectsPassObjectTypeTest(InObjects) == false )
		{
			// There exists a sync object that doesn't pass the object type filter test, which mean it 
			// wouldn't be visible. So, we need to clear all filter options to see the sync object.
			DataBrowserCtrl->FilterPanel->ClearFilterForBrowserSync();
		}
		else
		{
			// No matter what, clear all non-object-type filter options.
			DataBrowserCtrl->FilterPanel->ClearFilterExceptObjectType();
		}

		// get a list of Package items that correspond to the packages containing the objects in the set
		TArray<UPackage*> ObjPackages;
		GeneratePackageList(InObjects, ObjPackages);

		// We may not have any packages if we were asked to sync level actors or some other non-browsable
		// object type, so we'll only proceed if we have some packages we recognize
		if( ObjPackages.Num() > 0 )
		{

			List< ContentBrowser::ObjectContainerNode^ >^ PackagesList = gcnew List< ContentBrowser::ObjectContainerNode^ >();
			for ( INT PkgIndex = 0; PkgIndex < ObjPackages.Num(); PkgIndex++ )
			{
				ContentBrowser::ObjectContainerNode^ PackageItem = dynamic_cast<ContentBrowser::ObjectContainerNode^>(UPackageToPackage(ObjPackages(PkgIndex)));
				if ( PackageItem != nullptr )
				{
					PackagesList->Add(PackageItem);
				}
			}

			// now sync the packages list to these packages			
			PackageSyncWasNecessary = DataBrowserCtrl->MyPackagePanel->SetSelectedPackages(PackagesList);
			
			// Also kick off a deferred selection
			{
				// Only check if assets are quarantined if the GAD is initialized and Quarantine Mode is disabled (which would result in a synching failure)
				const UBOOL bCheckForQuarantined = !DataBrowserCtrl->IsInQuarantineMode() && FGameAssetDatabase::IsInitialized();
				UBOOL bDisplayQuarantinedWarning = FALSE;
				String^ QuarantinedTag = bCheckForQuarantined ? FGameAssetDatabase::MakeSystemTag( ESystemTagType::Quarantined, "" ) : String::Empty;

				// Create a search string
				List<String^> AssetFullNamesToSelect;
				for( int CurObjectIndex = 0; CurObjectIndex < InObjects.Num(); ++CurObjectIndex )
				{
					UObject* CurObject = InObjects( CurObjectIndex );
					if( CurObject != NULL )
					{
						String^ CurAssetFullName = CLRTools::ToString( CurObject->GetFullName() );
						
						UBOOL bAssetIsQuarantined = FALSE;

						// If we should check if an asset is quarantined, query for the system tags of the asset to see if it contains the
						// special quarantined tag
						if ( FGameAssetDatabase::IsInitialized() && bCheckForQuarantined ) // oscar@snda
						{
							List<String^>^ CurAssetTags;
							FGameAssetDatabase::Get().QueryTagsForAsset( CurAssetFullName, ETagQueryOptions::SystemTagsOnly, CurAssetTags );
							if ( CurAssetTags->Contains( QuarantinedTag ) )
							{
								bAssetIsQuarantined = TRUE;
								bDisplayQuarantinedWarning = TRUE;
							}
						}
						
						// If the asset isn't quarantined (or we're not checking quarantined status), add it to the list of assets to select
						if ( !bAssetIsQuarantined )
						{
							AssetFullNamesToSelect.Add( CurAssetFullName );
						}
					}
				}

				// If any assets were quarantined and can't be synched to correctly, display a warning for the user indicating the problem
				if ( bDisplayQuarantinedWarning )
				{
					DataBrowserCtrl->PlayWarning( UnrealEd::Utils::Localize("DataBrowser_Warning_FailedSyncToQuarantinedAssets") );
				}

				DataBrowserCtrl->AssetView->StartDeferredAssetSelection( %AssetFullNamesToSelect );
			}
		}
	} 
	else
	{
		// Clear the current selection and refresh viewport
		DataBrowserCtrl->AssetView->SetSelection(nullptr);

	}
	return PackageSyncWasNecessary;
}

/** 
 * Select the specified packages in the content browser
 * 
 * @param	InPackages	One or more packages to select
 */
void MDataBrowserControl::SyncToPackages( const TArray< UPackage* >& InPackages )
{
	List< ContentBrowser::ObjectContainerNode^ >^ PackagesList = gcnew List< ContentBrowser::ObjectContainerNode^ >();
	for ( INT PkgIndex = 0; PkgIndex < InPackages.Num(); PkgIndex++ )
	{
		ContentBrowser::ObjectContainerNode^ PackageItem = dynamic_cast<ContentBrowser::ObjectContainerNode^>(UPackageToPackage(InPackages(PkgIndex)));
		if ( PackageItem != nullptr )
		{
			PackagesList->Add(PackageItem);
		}
	}

	DataBrowserCtrl->MyPackagePanel->SetSelectedPackages(PackagesList);
}

/**
 * Places focus straight into the Search filter
 */
void MDataBrowserControl::GoToSearch()
{
	if( ParentBrowserWindow != NULL )
	{
		// Make sure the window is visible
		GUnrealEd->GetBrowserManager()->ShowWindow( ParentBrowserWindow->GetDockID(), TRUE );
	}

	DataBrowserCtrl->FilterPanel->GoToSearchBox();
}



/**
 * Wrapper method for building a string containing a list of class name + path names delimited by tokens.
 *
 * @param	out_ResultString	receives the value of the tokenized string containing references to the selected assets.
 *
 * @return	the number of currently selected assets
 */
INT MDataBrowserControl::GenerateSelectedAssetString( FString* out_ResultString )
{
	INT Result = 0;

	if ( DataBrowserCtrl != nullptr && DataBrowserCtrl->AssetView != nullptr && out_ResultString != NULL )
	{
		String^ SelectedAssetPaths = MarshalAssetItems(DataBrowserCtrl->AssetView->SelectedAssets);
		*out_ResultString = CLRTools::ToFString(SelectedAssetPaths);
		Result = DataBrowserCtrl->AssetView->SelectedAssets->Count;
	}

	return Result;
}

/**
 * Wrapper method for building a string containing a list of class name + path names delimited by tokens.
 *
 * @param	out_ResultString	receives the value of the tokenized string containing references to the selected assets.
 *
 * @return	the number of currently selected assets
 */
INT FDataBrowser::GenerateSelectedAssetString( FString* out_ResultString )
{
	INT Result = 0;

	MDataBrowserControl^ MyWindowControl = WindowControl;
	if ( MyWindowControl != nullptr && out_ResultString != NULL )
	{
		Result = MyWindowControl->GenerateSelectedAssetString(out_ResultString);
	}

	return Result;
}

/**
 * Returns whether saving the specified package is allowed
 */
UBOOL FDataBrowser::AllowPackageSave( UPackage* PackageToSave )
{
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl != nullptr )
	{
		return MyWindowControl->AllowPackageSave( PackageToSave );
	}

	return TRUE;
}

/**
 * Marshal a collection of asset items into a string (only marshals the Type and PathName)
 * 
 * @param InAssetItems A collection of AssetItems to marshal
 * 
 * @return A string representing the AssetItems
 */
String^ MDataBrowserControl::MarshalAssetItems( Generic::ICollection<ContentBrowser::DataAssetItem^>^ InAssetItems )
{
	Text::StringBuilder^ MarshalStringBuilder = gcnew Text::StringBuilder(); 

	for each ( ContentBrowser::DataAssetItem^ CurAssetItem in InAssetItems )
	{
		// Append an AssetDelimiter unless we're on the first item
		if ( MarshalStringBuilder->Length > 0 )
		{
			MarshalStringBuilder->Append( AssetMarshalDefs::AssetDelimiter );
		}
		MarshalStringBuilder->Append(CurAssetItem->AssetType);
		MarshalStringBuilder->Append(AssetMarshalDefs::NameTypeDelimiter);
		MarshalStringBuilder->Append(CurAssetItem->FullyQualifiedPath);
	}
	return MarshalStringBuilder->ToString();
}

/**
 * Extract asset full names from a marshaled string.
 * 
 * @param InAssetItems A collection of AssetItems to marshal
 * 
 * @return A list of asset full names
 */
List<String^>^ MDataBrowserControl::UnmarshalAssetFullNames( String^ MarshaledAssets )
{
	// Split the marshaled assets into an array of [type, name, type, name ...]
	array<TCHAR>^Delimiters = gcnew array<TCHAR>{ AssetMarshalDefs::AssetDelimiter, AssetMarshalDefs::NameTypeDelimiter };
	array<String^>^ TypesAndNames = MarshaledAssets->Split( Delimiters, StringSplitOptions::RemoveEmptyEntries );
	
	// Array must by of [type, name] pairs.
	check( (TypesAndNames->Length % 2) == 0 );

	List<String^>^ AssetFullNames = gcnew List<String^>(TypesAndNames->Length/2);
	for (int CurTokenIndex = 0; CurTokenIndex < TypesAndNames->Length; )
	{
		String^ ClassName = TypesAndNames[ CurTokenIndex++ ];
		String^ FullyQualifiedPath = TypesAndNames[CurTokenIndex++];

		AssetFullNames->Add( UnrealEd::Utils::MakeFullName( ClassName, FullyQualifiedPath ) );
	}
	return AssetFullNames;
}

/**
 * Generate a list of assets from a marshaled string.
 * 
 * @param	MarshaledAssetString		a string containing references one or more assets.
 * @param	out_SelectedAssets			receieves the list of assets parsed from the string.
 * 
 * @return	TRUE if at least one asset item was parsed from the string.
 */
UBOOL FDataBrowser::UnmarshalAssetItems( const FString& MarshaledAssetString, TArray<FSelectedAssetInfo>& out_SelectedAssets )
{
	UBOOL bResult = FALSE;

	TCHAR AssetDelimiter[2] = { AssetMarshalDefs::AssetDelimiter, '\0' };

	TArray<FString> TypesAndNames;
	MarshaledAssetString.ParseIntoArray(&TypesAndNames, AssetDelimiter, TRUE);
	for ( INT StrIndex = 0; StrIndex < TypesAndNames.Num(); StrIndex++ )
	{
		const FString& AssetIDString = TypesAndNames(StrIndex);

		FSelectedAssetInfo NewAssetInfo = FSelectedAssetInfo(AssetIDString);
		if ( NewAssetInfo.IsValid() )
		{
			out_SelectedAssets.AddItem(NewAssetInfo);
			bResult = TRUE;
		}
	}
	return bResult;
}

/**
 * Wrapper for determining whether an asset is eligible to be loaded on its own.
 * 
 * @param	AssetPathName	the fully qualified [Unreal] pathname of the asset to check
 * 
 * @return	true if the specified asset can be loaded on its own
 */
UBOOL FDataBrowser::IsAssetValidForLoading( const FString& AssetPathName )
{
	UBOOL bResult = CLRTools::IsAssetValidForLoading(CLRTools::ToString(AssetPathName));
	return bResult;
}

/**
 * Wrapper for determining whether an asset is eligible to be placed in a level.
 * 
 * @param	AssetPathName	the fully qualified [Unreal] pathname of the asset to check
 * 
 * @return	true if the specified asset can be placed in a level
 */
UBOOL FDataBrowser::IsAssetValidForPlacing( const FString& AssetPathName )
{
	UBOOL bResult = CLRTools::IsAssetValidForPlacing(CLRTools::ToString(AssetPathName));
	return bResult;
}


/** Static: List of currently-active Content Browser instances */
TArray< FDataBrowser* > FDataBrowser::DataBrowserInstances;


/** Static: Allocate and initialize content browser */
FDataBrowser* FDataBrowser::CreateDataBrowser( WxDataBrowserHost* InParentBrowser, const HWND InParentWindowHandle )
{

	/*
	WxPropertyWindowFrame* PropertiesWindow = new WxPropertyWindowFrame();
	PropertiesWindow->Create( GApp->EditorFrame, -1, GUnrealEd );
	PropertiesWindow->AllowClose();

	// Setup the property window
	PropertiesWindow->SetObjectArray(SelectedObjects, EPropertyWindowFlags::Sorted | EPropertyWindowFlags::ShouldShowCategories );

	// Display the properties!
	PropertiesWindow->Show();

	*/
// 	WxPropertyWindowFrame* PropertiesWindow2 = new WxPropertyWindowFrame();
// 	PropertiesWindow2->Create(InParentBrowser, -1, GUnrealEd );
// 	PropertiesWindow2->Show();
// 	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
// 
// 
// 	WxPropertyWindowHost* PropertyWindowHost = new WxPropertyWindowHost;
// 	check( PropertyWindowHost );
// 	PropertyWindowHost->Create( InParentBrowser, GUnrealEd, -1 );
// 	PropertyWindowHost->SetFlags(EPropertyWindowFlags::CanBeHiddenByPropertyWindowManager, TRUE);
// 	PropertyWindowHost->Show();
// 	topSizer->Add(PropertyWindowHost);


	FDataBrowser* NewDataBrowser = new FDataBrowser();

	if( !NewDataBrowser->InitDataBrowser( InParentBrowser, InParentWindowHandle ) )
	{
		delete NewDataBrowser;
		return NULL;
	}
// 	topSizer->Add(NewDataBrowser);
// 	InParentBrowser->SetSizer(topSizer);

	return NewDataBrowser;
}

/**
 * Initialize the content browser
 *
 * @param	InParentBrowser			Parent browser window (or NULL if we're not parented to a browser.)
 * @param	InParentWindowHandle	Parent window handle
 *
 * @return	TRUE if successful
 */
UBOOL FDataBrowser::InitDataBrowser( WxDataBrowserHost* InParentBrowser, const HWND InParentWindowHandle )
{
	WindowControl = gcnew MDataBrowserControl( this );
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl == nullptr )
	{
		return FALSE;
	}

	UBOOL bSuccess = MyWindowControl->InitDataBrowser( InParentBrowser, InParentWindowHandle );

	return bSuccess;
}



/** Constructor */
FDataBrowser::FDataBrowser()
{
	bIsActiveInstance = FALSE;

	// Register a callback for changes to objects which require the content browser to be refreshed
	GCallbackEvent->Register(CALLBACK_RefreshContentBrowser, this);

	// only subscribe to this callback if we're the first browser
	//GCallbackEvent->Register(CALLBACK_LoadSelectedAssetsIfNeeded, this);
	GCallbackEvent->Register(CALLBACK_LoadSelectedAssetsFromDataBrowser, this);

	// Register a callback for GC so we can clear up any soft references to UObjects that we're caching
	GCallbackEvent->Register( CALLBACK_PreGarbageCollection, this );

	// Register a callback for objects changing so that we can dirty their thumbnails and refresh visuals
	GCallbackEvent->Register( CALLBACK_ObjectPropertyChanged, this );

	GCallbackEvent->Register( CALLBACK_MapChange, this );
	GCallbackEvent->Register( CALLBACK_CleanseEditor, this );

	// Register to find out about other windows going modal
	GCallbackEvent->Register( CALLBACK_EditorPreModal, this );
	GCallbackEvent->Register( CALLBACK_EditorPostModal, this );

	// @todo cb: Remove this code altogether
	//GCallbackEvent->Register( CALLBACK_SelChange, this );

	// Add to list of instances
	DataBrowserInstances.AddItem( this );

	// A new content browser always becomes the active one.
	MakeActive();
}



/** Destructor */
FDataBrowser::~FDataBrowser()
{
	// Update singleton
	DataBrowserInstances.RemoveItem( this );

	// Unregister global callbacks
	GCallbackEvent->UnregisterAll( this );

	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl != nullptr )
	{
		// Save out the recently accessed browser objects.
		//This call may be extraneous because it's usually called whenever new objects are accessed, but it's here for safety
		MyWindowControl->WriteRecentObjectsToConfig();

		// Dispose of WindowControl
		delete MyWindowControl;
		WindowControl = NULL;
	}
}



/** Resize the window */
void FDataBrowser::Resize(HWND hWndParent, int x, int y, int Width, int Height)
{
	WindowControl->Resize(hWndParent, x, y, Width, Height);
}

/**
 * Propagates focus from the wxWidgets framework to the WPF framework.
 */
void FDataBrowser::SetFocus()
{
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if ( MyWindowControl != nullptr )
	{
		MyWindowControl->SetFocus();
	}

	MakeActive();
}

/** @return	the list of classes which render static shared thumbnail (unique per class) */
const TArray<UClass*>* FDataBrowser::GetSharedThumbnailClasses() const
{
	MDataBrowserControl^ MyWindowControl = WindowControl;
	check( MyWindowControl != nullptr );

	return MyWindowControl->GetSharedThumbnailClasses();
}



/** Returns the map of classes to browsable object types */
const TMap< UClass*, TArray< UGenericBrowserType* > >& FDataBrowser::GetBrowsableObjectTypeMap() const
{
	MDataBrowserControl^ MyWindowControl = WindowControl;
	check( MyWindowControl != nullptr );

	return MyWindowControl->GetBrowsableObjectTypeMap();
}




/** FSerializableObject: Serialize object references for garbage collector */
void FDataBrowser::Serialize( FArchive& Ar )
{
	// Ask the control to serialize it's objects
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl != nullptr )
	{
		TArray< UObject* > SerializableObjects;
		MyWindowControl->QuerySerializableObjects( SerializableObjects ); // Out
		
		for( INT CurObjectIndex = 0; CurObjectIndex < SerializableObjects.Num(); ++CurObjectIndex )
		{
			UObject* CurObject = SerializableObjects( CurObjectIndex );
			Ar << CurObject;
		}
	}
}



/**
 * Called from within UnLevTic.cpp after ticking all actors or from
 * the rendering thread (depending on bIsRenderingThreadObject)
 *
 * @param DeltaTime	Game time passed since the last call.
 */
void FDataBrowser::Tick( FLOAT DeltaTime )
{
	// Update the content browser control
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl != nullptr )
	{
		MyWindowControl->Tick();
	}
}

/**
 * Display the specified objects in the content browser
 *
 * @param	InObjects	One or more objects to display
 */
void FDataBrowser::SyncToObjects( TArray< UObject* >& InObjects )
{
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl != nullptr )
	{
		MyWindowControl->SyncToObjects( InObjects );
	}
}

/** 
 * Select the specified packages in the content browser
 * 
 * @param	InPackages	One or more packages to select
 */
void FDataBrowser::SyncToPackages( const TArray< UPackage* >& InPackages )
{
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl != nullptr )
	{
		MyWindowControl->SyncToPackages( InPackages );
	}
}

/** 
 *  Creates a local collection
 * 
 * @param	InCollectionName	Name of the collection to create
 */
void FDataBrowser::CreateLocalCollection( const FString& InCollectionName )
{
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl != nullptr )
	{
		MyWindowControl->CreateCollection( CLRTools::ToString(InCollectionName), ContentBrowser::EBrowserCollectionType::Local );
	}
}

/** 
 *  Destroys a local collection
 * 
 * @param	InCollectionName	Name of the collection to destroy
 */
void FDataBrowser::DestroyLocalCollection( const FString& InCollectionName )
{
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl != nullptr )
	{
		MyWindowControl->DestroyCollection( CLRTools::ToString(InCollectionName), ContentBrowser::EBrowserCollectionType::Local );
	}
}

/** 
 * Adds assets to a local collection
 * 
 * @param	InCollectionName	Name of the collection to add assets to
 * @param	AssetsToAdd			List of assets to add
 */
void FDataBrowser::AddAssetsToLocalCollection( const FString& InCollectionName, const TArray<UObject*>& AssetsToAdd )
{
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl != nullptr )
	{
		// Convert the object list to a list of full names
		List<System::String^>^ AssetFullNames = gcnew List<System::String^>();
		for( INT AssetIndex = 0; AssetIndex < AssetsToAdd.Num(); ++AssetIndex )
		{
			AssetFullNames->Add( CLRTools::ToString( AssetsToAdd(AssetIndex)->GetFullName() ) );
		}

		MyWindowControl->AddAssetsToCollection( AssetFullNames, gcnew ContentBrowser::Collection( CLRTools::ToString(InCollectionName), true), ContentBrowser::EBrowserCollectionType::Local );
	}
}

/** 
 * Removes assets to a local collection
 * 
 * @param	InCollectionName	Name of the collection to remove assets from
 * @param	AssetsToAdd			List of assets to remove
 */
void FDataBrowser::RemoveAssetsFromLocalCollection( const FString& InCollectionName, const TArray<UObject*>& AssetsToRemove )
{
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl != nullptr )
	{
		// Convert the object list to a list of full names
		List<System::String^>^ AssetFullNames = gcnew List<System::String^>();
		for( INT AssetIndex = 0; AssetIndex < AssetsToRemove.Num(); ++AssetIndex )
		{
			AssetFullNames->Add( CLRTools::ToString( AssetsToRemove(AssetIndex)->GetFullName() ) );
		}

		MyWindowControl->RemoveAssetsFromCollection( AssetFullNames, gcnew ContentBrowser::Collection( CLRTools::ToString(InCollectionName), true), ContentBrowser::EBrowserCollectionType::Local );
	}
}

/** 
 * Selects a collection
 * 
 * @param	InCollectionName	Name of the collection to select
 * @param	InCollectionType	Type of collection to select
 */
void FDataBrowser::SelectCollection( const FString& InCollectionToSelect, EGADCollection::Type InCollectionType )
{
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl != nullptr )
	{
		MyWindowControl->SelectCollection( CLRTools::ToString(InCollectionToSelect), (ContentBrowser::EBrowserCollectionType)InCollectionType );
	}
}
/**
 * Places focus straight into the Search filter
 */
void FDataBrowser::GoToSearch()
{
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl != nullptr )
	{
		MyWindowControl->GoToSearch();
	}
}

	
/** FCallbackEventDevice: Called when a parameterless global event we've registered for is fired */
void FDataBrowser::Send( ECallbackEventType Event )
{
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl != nullptr )
	{
		switch ( Event )
		{
			case CALLBACK_PreGarbageCollection:
				// Tell the content browser that a GC is about to occur
				MyWindowControl->OnPendingGarbageCollection();
				break;

			case CALLBACK_LoadSelectedAssetsFromDataBrowser: // oscar@snda
				if ( bIsActiveInstance )
				{
					MyWindowControl->LoadSelectedObjectsIfNeeded();
				}			
				break;

			case CALLBACK_CleanseEditor:
				MyWindowControl->RequestAssetListUpdate( AssetListRefreshMode::UIOnly, NULL, UINT(ContentBrowser::AssetStatusUpdateFlags::LoadedStatus) );
				MyWindowControl->RequestPackageListUpdate( false, NULL );
				if ( bIsActiveInstance )
				{
					MyWindowControl->SyncSelectedObjectsWithGlobalSelectionSet();
				}			
				break;

			case CALLBACK_EditorPreModal:
				MyWindowControl->EnableWindow( false );
				break;

			case CALLBACK_EditorPostModal:
				MyWindowControl->EnableWindow( true );
				break;
		}
	}
}

/**
 * Called for map change events.
 *
 * @param InType the event that was fired
 * @param InFlags the flags for this event
 */
void FDataBrowser::Send( ECallbackEventType InType, DWORD InFlags )
{
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl != nullptr )
	{
		if ( InType == CALLBACK_MapChange && (InFlags&MapChangeEventFlags::NewMap) != 0)
		{
			MyWindowControl->RequestAssetListUpdate(AssetListRefreshMode::UIOnly, NULL, UINT(ContentBrowser::AssetStatusUpdateFlags::LoadedStatus));
			MyWindowControl->RequestPackageListUpdate(false, NULL);
			MyWindowControl->RequestSCCStateUpdate();
		}
	}
}


/** FCallbackEventDevice: Called when a parameterless global event we've registered for is fired */
void FDataBrowser::Send( ECallbackEventType Event, UObject* InObject )
{
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl != nullptr )
	{
		switch ( Event )
		{
		case CALLBACK_ObjectPropertyChanged:
			// Tell the content browser that an object has been changed
			MyWindowControl->OnObjectModification( InObject );
			break;

		// @todo cb: Remove this code altogether?
		//case CALLBACK_SelChange:
		//	MyWindowControl->NotifySelectionChanged(CastChecked<USelection>(InObject));
		//	break;
		}
	}
}

#define DEBUG_CALLBACK_REQUESTS 0

/**
 * Called for content browser updates.
 */
void FDataBrowser::Send( const FCallbackEventParameters& Parms )
{
	if ( Parms.EventType == CALLBACK_RefreshContentBrowser
	&&	(Parms.Sender == NULL || Parms.Sender == this) )
	{
		MDataBrowserControl^ MyWindowControl = WindowControl;

		// disable content browser updates while in PIE
		if ( MyWindowControl != nullptr && !GIsPlayInEditorWorld )
		{
#if DEBUG_CALLBACK_REQUESTS
			debugf(TEXT("++++ START ++++"));
#endif
			if ( (Parms.EventFlag&(CBR_EmptySelection)) != 0 )
			{
				TArray< UObject* > EmptySelectionArray;
				MyWindowControl->SyncToObjects(EmptySelectionArray);
			}	
			if( (Parms.EventFlag&(CBR_ClearFilter)) != 0 )
			{
				MyWindowControl->ClearFilter();
			}
			if ( (Parms.EventFlag&(CBR_InternalPackageUpdate|CBR_UpdatePackageListUI)) != 0 )
			{
				MyWindowControl->RequestPackageListUpdate((Parms.EventFlag&CBR_InternalPackageUpdate) == 0, Parms.EventObject);
			}
			if ( (Parms.EventFlag&(CBR_InternalCollectionUpdate|CBR_UpdateCollectionListUI)) != 0 )
			{
				MyWindowControl->RequestCollectionListUpdate((Parms.EventFlag&CBR_InternalCollectionUpdate) == 0);
			}
			if ( bIsActiveInstance && (Parms.EventFlag&(CBR_InternalAssetUpdate|CBR_InternalQuickAssetUpdate|CBR_UpdateAssetListUI)) != 0 )
			{
				UINT UpdateMask = UINT(ContentBrowser::AssetStatusUpdateFlags::LoadedStatus);
				MyWindowControl->RequestAssetListUpdate( RefreshModeFromRefreshFlags(Parms.EventFlag), Parms.EventObject, UpdateMask);
			}
			if( (Parms.EventFlag & CBR_NewPackageSaved) != 0 )
			{
				UPackage* ToRemove = Cast<UPackage>(Parms.EventObject);

				if(ToRemove)
				{
					MyWindowControl->RemoveFromPackageList(ToRemove);
				}
			}
			if ( bIsActiveInstance && (Parms.EventFlag&(CBR_ObjectCreated|CBR_ObjectDeleted|CBR_ObjectRenamed)) != 0 )
			{
				MyWindowControl->RequestPackageListUpdate(false, Parms.EventObject);
				
				// Request a quick update to the view. It is possible that RequestSyncAssetView() below will override this.
				MyWindowControl->RequestAssetListUpdate(AssetListRefreshMode::QuickRepopulate, Parms.EventObject, 0);

				if ( Parms.EventObject != NULL )
				{
					if ( (Parms.EventFlag & (CBR_ObjectCreated|CBR_ObjectRenamed)) != 0 )
					{
						// Only sync to the event object if "no sync" wasn't requested
						if ( ( Parms.EventFlag & CBR_NoSync ) == 0 )
						{
							MyWindowControl->RequestSyncAssetView(Parms.EventObject);
						}

						TSet<FName> SystemTags;
						if ( Cast<UPackage>(Parms.EventObject) == NULL )
						{
							// Check to see if this object type can be displayed in the content browser
							const TSet<FName>& AllowedClassTypes = FGameAssetDatabase::GetAllowedClassList();
							
							// Check to see if this object is an archetype, as archetypes do not count as allowed classes
							const bool bIsArchetype =
								Parms.EventObject->HasAllFlags( RF_ArchetypeObject ) &&
								!Parms.EventObject->IsAPrefabArchetype();

							FName ObjClass = Parms.EventObject->GetClass()->GetFName();

							if (FGameAssetDatabase::IsInitialized() && bIsArchetype || AllowedClassTypes.Contains(ObjClass) ) // oscar@snda
							{
								// Update the journal server if we're adding a new object
								const bool bSendToJournalIfNeeded = true;

								// Create default tags for this object (locally only)
								String^ OutermostName = CLRTools::ToString(Parms.EventObject->GetOutermost()->GetName());
								String^ ClassName = CLRTools::ToString(ObjClass.ToString());
								
								FGameAssetDatabase::Get().SetDefaultTagsForAsset(
									CLRTools::ToString( *Parms.EventObject->GetFullName() ),
									ClassName,
									OutermostName,
									bIsArchetype,
									bSendToJournalIfNeeded );
							}
						}

						// For renamed objects, we also want to go and remove the old object from the GAD.  This
						// is just like the delete, below, except we're only given the path name string
						if(FGameAssetDatabase::IsInitialized() && ( Parms.EventFlag & CBR_ObjectRenamed ) != 0 ) // oscar@snda
						{
							// Event string arg must contain the path to the old object that was renamed!
							check( Parms.EventString.Len() > 0 );

							FName RenamedAssetFullNameFName = FName(*Parms.EventString);
							FGameAssetDatabase::Get().RemoveAssetTagMappings( RenamedAssetFullNameFName );
						}
					}
					else if (FGameAssetDatabase::IsInitialized() && (Parms.EventFlag&CBR_ObjectDeleted) != 0 ) // oscar@snda
					{
						if ( Cast<UPackage>(Parms.EventObject) == NULL )
						{
							FName DeletedAssetFullNameFName = FName(*Parms.EventObject->GetFullName());
							FGameAssetDatabase::Get().RemoveAssetTagMappings( DeletedAssetFullNameFName );
						}
					}
				}
			}
			
			if( bIsActiveInstance && ( Parms.EventFlag & CBR_ActivateObject ) != 0 )
			{
				TArray<UObject*> Assets;
				Assets.AddItem(Parms.EventObject);
				MyWindowControl->OpenObjectEditorFor( Assets );
			}
			
			if ( bIsActiveInstance && (Parms.EventFlag&CBR_SyncAssetView) != 0 && (Parms.EventFlag&CBR_NoSync) == 0 )
			{
				MyWindowControl->RequestSyncAssetView(Parms.EventObject);
			}

			if ( bIsActiveInstance && (Parms.EventFlag&CBR_UpdateSCCState) != 0 )
			{
				MyWindowControl->RequestSCCStateUpdate();
			}

			if ( bIsActiveInstance && (Parms.EventFlag&CBR_FocusBrowser) != 0  )
			{
				// Make sure the window is visible
				if( MyWindowControl->GetParentBrowserWindow() != NULL )
				{
					GUnrealEd->GetBrowserManager()->ShowWindow( MyWindowControl->GetParentBrowserWindow()->GetDockID(), TRUE );
				}
				MyWindowControl->SetFocus();
			}
#if DEBUG_CALLBACK_REQUESTS
			debugf(TEXT("++++ END ++++"));
#endif
		}
	}
}

#if HAVE_SCC
/**
 * Callback when a command is done executing (pass through to get to the window
 * @param InCommand - The Command passed from source control subsystem that has completed it's work
 */
void FDataBrowser::SourceControlCallback(FSourceControlCommand* InCommand)
{
	check(InCommand);
	WindowControl->SourceControlResultsProcess(InCommand);
}
#endif

/** Make this DataBrowser the sole active content browser */
void FDataBrowser::MakeActive()
{
	// Let all the other content browsers know that we will not be telling the editor which assets are selected.
	for ( INT DataBrowserIdx = 0; DataBrowserIdx < DataBrowserInstances.Num(); ++DataBrowserIdx )
	{
		FDataBrowser * CurCB = DataBrowserInstances(DataBrowserIdx);
		if (CurCB != this)
		{
			CurCB->OnYieldSelectionAuthority();
		}
	}

	// Become the Content Browser that tells the editor which assets are selected.
	this->OnBecameSelectionAuthority();
}

/** Find an appropriate content browser and make it the active one */
void FDataBrowser::MakeAppropriateDataBrowserActive()
{
	check(DataBrowserInstances.Num() > 0);
	DataBrowserInstances(0)->MakeActive();
}

/** Notifies an instance of the Content Browser that it is now responsible for the current selection */
void FDataBrowser::OnBecameSelectionAuthority()
{
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl != nullptr )
	{
		MyWindowControl->OnBecameSelectionAuthority();
	}
	bIsActiveInstance = true;
}

/** Notify an instance of the Content Browser that it is giving up being the selection authority */
void FDataBrowser::OnYieldSelectionAuthority()
{
	MDataBrowserControl^ MyWindowControl = WindowControl;
	if( MyWindowControl != nullptr )
	{
		MyWindowControl->OnYieldSelectionAuthority();
	}
	bIsActiveInstance = false;
}



void MDataBrowserControl::RequestPackageListUpdate( /*DefaultValueParmBoolFalse*/bool bUpdateUIOnly, UObject* SourceObject )
{
#if DEBUG_CALLBACK_REQUESTS
	debugf(TEXT("++++ RequestPackageListUpdate  bPackageListUpdateRequested:%i  bPackageListUpdateUIRequested:%i  bUpdateUIOnly:%i  SourceObject:%s"),
		(INT)bPackageListUpdateRequested, (INT)bPackageListUpdateUIRequested, (INT)bUpdateUIOnly, *SourceObject->GetFullName());
#endif
	bPackageListUpdateUIRequested = true;
	if ( bUpdateUIOnly && !bPackageListUpdateRequested )
	{
		UPackage* SourcePackage = Cast<UPackage>(SourceObject);
		if ( SourcePackage != NULL && !bPackageListUpdateRequested )
		{
			// try to update only the packages that were changed
			if ( !CallbackEventPackages )
			{
				CallbackEventPackages.Reset(new TArray<UPackage*>());
			}
			CallbackEventPackages->AddUniqueItem(SourcePackage);
		}
		
		bAssetViewUpdateUIRequested = true;
	}
	else
	{
		bPackageListUpdateRequested = true;
	}
}

void MDataBrowserControl::RequestCollectionListUpdate( /*DefaultValueParmBoolFalse*/bool bUpdateUIOnly )
{
#if DEBUG_CALLBACK_REQUESTS
	debugf(TEXT("++++ RequestCollectionListUpdate  bCollectionListUpdateRequested:%i  bCollectionListUpdateUIRequested:%i  bUpdateUIOnly:%i"),
		(INT)bCollectionListUpdateRequested, (INT)bCollectionListUpdateUIRequested, (INT)bUpdateUIOnly);
#endif
	bCollectionListUpdateUIRequested = true;
	if ( !bUpdateUIOnly )
	{
		bCollectionListUpdateRequested = true;
	}
}

void MDataBrowserControl::RequestAssetListUpdate( AssetListRefreshMode RefreshMode, UObject* SourceObject, UINT AssetUpdateFlagMask )
{
#if DEBUG_CALLBACK_REQUESTS
	debugf(TEXT("++++ RequestAssetListUpdate  bAssetViewUpdateRequested:%i  bAssetViewUpdateUIRequested:%i  bUpdateUIOnly:%i  SourceObject:%s   AssetUpdateFlagMask: 0x%08X"),
		(INT)bAssetViewUpdateRequested, (INT)bAssetViewUpdateUIRequested, (INT)RefreshMode, *SourceObject->GetFullName(), AssetUpdateFlagMask);
#endif
	// the asset viewer will completely repopulate its thumbnails, so no need to perform an additional UI update
	// if a full update was requested.
	if ( RefreshMode == AssetListRefreshMode::UIOnly )
	{
		bAssetViewUpdateUIRequested = true;
		if ( SourceObject != NULL )
		{
			if ( !CallbackEventObjects )
			{
				CallbackEventObjects.Reset(new TArray<UObject*>());
			}
			CallbackEventObjects->AddUniqueItem(SourceObject);
		}
	}
	else if ( RefreshMode == AssetListRefreshMode::Repopulate )
	{
		bAssetViewUpdateRequested = true;
		bDoFullAssetViewUpdate = true; // A full repopulate request overrides a quick repopulate request.
	}
	else // ( RefreshMode == AssetListRefreshMode::QuickRepopulate )
	{
		// Only allow a quick repopulate to be triggered if there isn't already a pending populate request.
		if ( ! bAssetViewUpdateRequested )
		{
			bAssetViewUpdateRequested = true;
			bDoFullAssetViewUpdate = bDoFullAssetViewUpdate || ! IsCapableOfQuickUpdate();
		}
	}

	AssetUpdateFlags |= AssetUpdateFlagMask;
}

void MDataBrowserControl::RequestSyncAssetView( UObject* SourceObject )
{
#if DEBUG_CALLBACK_REQUESTS
	debugf(TEXT("++++ RequestSyncAssetView  bAssetViewSyncRequested:%i  SourceObject:%s"),
		(INT)bAssetViewSyncRequested, *SourceObject->GetFullName());
#endif
	if ( SourceObject != NULL )
	{
		bAssetViewSyncRequested = true;
		if ( !CallbackEventSyncObjects )
		{
			CallbackEventSyncObjects.Reset(new TArray<UObject*>());
		}
		CallbackEventSyncObjects->AddUniqueItem(SourceObject);
	}
}


#if HAVE_SCC

/**
 * Helper functions to determine the results of a status update for a package
 * @param pkg - The content browser package to receive the new state information
 * @param InNewState - The new source control state (checked out, locked, etc)
 */
void  MDataBrowserControl::SetSourceControlState(ContentBrowser::Package^ pkg, ESourceControlState InNewState)
{
	UpdatePackageSCCState(pkg, InNewState);

	// now update the corresponding UPackage, if it's loaded
	GPackageFileCache->SetSourceControlState(*CLRTools::ToFString(pkg->Name), InNewState);
}

/**
 * Helper functions to determine the results of a status update for a package
 * @param pkg - The content browser package to receive the new state information
 * @param InResultsMap - String pairs generated from source control
 */
void  MDataBrowserControl::SetSourceControlState(ContentBrowser::Package^ pkg, const TMap<FString, FString>& InResultsMap)
{
	ESourceControlState NewState = FSourceControl::TranslateResultsToState(InResultsMap);

	SetSourceControlState(pkg, NewState);
}

/**
 * Callback when a command is done executing
 * @param InCommand - The Command passed from source control subsystem that has completed it's work
 */
void MDataBrowserControl::SourceControlResultsProcess(FSourceControlCommand* InCommand)
{
	check(InCommand);

	bPackageFilterUpdateRequested = true;

	// generate a list from all packages in the tree view
	List<ContentBrowser::Package^>^ PackageList = gcnew List<ContentBrowser::Package^>();
	DataBrowserCtrl->MySources->GetChildNodes<ContentBrowser::Package^>(PackageList);

	if (InCommand->bCommandSuccessful)
	{
		TSet<FString> InputPackageNames;
		UBOOL bIsAlreadyInSetPtr;
		for (INT i = 0; i < InCommand->Params.Num(); ++i)
		{
			FFilename FullPath = InCommand->Params(i);
			InputPackageNames.Add(FullPath.GetBaseFilename(), &bIsAlreadyInSetPtr);
		}

		// build an array of package names for passing to the SCC function
		for each ( ContentBrowser::Package^ pkg in PackageList )
		{
			FString PkgName = CLRTools::ToFString(pkg->Name);

			//try the results first, if there then we should update the state
			const TMap<FString, FString>* ResultsMap = InCommand->Results.Find(PkgName);
			if (ResultsMap != NULL)
			{
				//Found the proper package, now update the state!
				SetSourceControlState(pkg, *ResultsMap);
			}
			else if (InputPackageNames.Contains(PkgName))
			{
				//FAILED the request to perforce.  Must not be in the depot
				SetSourceControlState(pkg, SCC_NotInDepot);
			}
		}
	}
	else
	{
		//just go through the list of params, there will be no records
		for (INT i = 0; i < InCommand->Params.Num(); ++i)
		{
			FFilename FullPath = InCommand->Params(i);
			String^ TestPackageName = CLRTools::ToString(FullPath.GetBaseFilename());
			// build an array of package names for passing to the SCC function
			for each ( ContentBrowser::Package^ pkg in PackageList )
			{
				if (pkg->Name == TestPackageName)
				{
					//Found the proper pacakge, no update the state!
					SetSourceControlState(pkg, SCC_NotInDepot);
				}
			}
		}
	}
}
#endif

void MDataBrowserControl::ConditionalUpdateSCC( TArray<UPackage*>* PackageList )
{
	if ( bIsSCCStateDirty )
	{
		bIsSCCStateDirty = false;
#if HAVE_SCC
		TArray<FString> PackageNames;
		if ( PackageList == NULL )
		{
			// generate a list from all packages in the tree view
			List<ContentBrowser::Package^>^ PackageList = gcnew List<ContentBrowser::Package^>();
			DataBrowserCtrl->MySources->GetChildNodes<ContentBrowser::Package^>(PackageList);

			// build an array of package names for passing to the SCC function
			for each ( ContentBrowser::Package^ pkg in PackageList )
			{
				PackageNames.AddItem( CLRTools::ToFString(pkg->Name) );
			}
		}
		else
		{
			// update only these packages
			for (INT i = 0; i < PackageList->Num(); ++i)
			{
				PackageNames.AddItem((*PackageList)(i)->GetPathName());
			}
		}
		FSourceControl::ConvertPackageNamesToSourceControlPaths(PackageNames);
		FSourceControl::IssueUpdateState(NativeBrowserPtr, PackageNames);
#endif
	}
}

/** Clears the content browser filter panel */
void MDataBrowserControl::ClearFilter()
{
	DataBrowserCtrl->FilterPanel->ClearFilter();
}

/**
 * If the corresponding flags are set, triggers a rebuild of the package list, an update of the package list visual state,
 * or both.
 */
void MDataBrowserControl::ConditionalUpdatePackages()
{
	if ( bPackageListUpdateRequested )
	{
		bPackageListUpdateRequested = false;
		if ( CallbackEventPackages && CallbackEventPackages->Num() > 0 )
		{
			// if we're going to repopulate the packages tree, don't need to keep track
			// of individual packages because we're going to update them all
			CallbackEventPackages->Empty();
			bPackageListUpdateUIRequested = true;
		}

		UpdatePackagesTree( DataBrowserCtrl->MyPackagePanel->UsingFlatList );
	}
	
	if ( bPackageListUpdateUIRequested )
	{
		bPackageListUpdateUIRequested = false;
		if ( CallbackEventPackages && CallbackEventPackages->Num() > 0 )
		{
			// if individual packages
			TArray<UPackage*>& PackageList = *(CallbackEventPackages.Get());
			for ( INT PackageIdx = 0; PackageIdx < PackageList.Num(); PackageIdx++ )
			{
				UpdatePackagesTreeUI(PackageList(PackageIdx));
			}
		}
		else
		{
			UpdatePackagesTreeUI();
		}
	}

	if ( CallbackEventPackages && CallbackEventPackages->Num() > 0 )
	{
		CallbackEventPackages->Empty();
	}
}

void MDataBrowserControl::ConditionalUpdateCollections()
{
	//@todo
	if ( bCollectionListUpdateRequested )
	{
		bCollectionListUpdateRequested = false;
		UpdateCollectionsList();
		
		// Cause UpdateCollectionsListUI
		bCollectionListUpdateUIRequested = true;		
	}
	
	if ( bCollectionListUpdateUIRequested )
	{
		bCollectionListUpdateUIRequested = false;
		UpdateCollectionsListUI();
	}

	// If there are any collection syncs requested, select the collections now.
	if( bCollectionSyncRequested )
	{
		bCollectionSyncRequested = false;
		
		if ( CollectionSelectRequests != nullptr )
		{
			for each( CollectionSelectRequest^ Request in CollectionSelectRequests )
			{
				DataBrowserCtrl->MyPackagePanel->SelectCollection( Request->CollectionName, Request->CollectionType );
			}

			CollectionSelectRequests->Clear();
		}
	
	}
}

void MDataBrowserControl::ConditionalUpdateAssetView()
{
	if ( bAssetViewSyncRequested )
	{
		bAssetViewSyncRequested = false;
		bAssetViewUpdateRequested = true; // We should update the asset view since package selection changed.
		bAssetViewUpdateUIRequested = false;

		if ( CallbackEventSyncObjects && CallbackEventSyncObjects->Num() > 0 )
		{
			bool PackageSyncWasNecessary = SyncToObjects(*(CallbackEventSyncObjects.Get()));
			bDoFullAssetViewUpdate = bDoFullAssetViewUpdate || PackageSyncWasNecessary;

			CallbackEventSyncObjects->Empty();
		}
	}
	else if ( bAssetViewUpdateRequested )
	{
		bAssetViewUpdateRequested = false;
		bAssetViewUpdateUIRequested = false;
		
		// indicates that one of the following events occurred
		// 1. Objects were loaded/unloaded.
		// 2. Objects were deleted.
		// 3. Objects were renamed.
		StartAssetQuery();

		//@todo - update local copy of GAD
	}
	else if ( bAssetViewUpdateUIRequested )
	{
		// indicates that an object was modified and the thumbnail should be updated accordingly
		bAssetViewUpdateUIRequested = false;
		if ( CallbackEventObjects )
		{
			TArray<UObject*>* ObjectList = CallbackEventObjects.Get();

			UObject* NextObj;
			while ( ObjectList->Num() > 0 && (NextObj=ObjectList->Pop()) != NULL )
			{
				OnObjectModification(NextObj, ContentBrowser::AssetStatusUpdateFlags(AssetUpdateFlags));
			}
		}
		AssetUpdateFlags = 0;
	}
}

void MDataBrowserControl::ConditionalUpdatePackageFilter()
{
	if ( bPackageFilterUpdateRequested )
	{
		// Any changes to packages require a filter refresh.
		DataBrowserCtrl->MyPackagePanel->RefreshPackageFilter();

		bPackageFilterUpdateRequested = false;		
	}
}


//==============================================================================================================================
// The following methods had to be implemented outside the class declaration due to C++/CLI ICE in Array.h
//==============================================================================================================================

/**
 * Retrieves the names of the package and group which are currently selected in the package tree view
 */
void MDataBrowserControl::GetSelectedPackageAndGroupName( FString& SelectedPackageName, FString& SelectedGroupName )
{
	if( DataBrowserCtrl->MyPackagePanel->AnyNodesSelected() )
	{
		// Create a flat list of selected packages
		ReadOnlyCollection<ContentBrowser::ObjectContainerNode^>^ SelectedPackages = DataBrowserCtrl->MyPackagePanel->MakeSelectedPackageAndGroupList();
		if ( SelectedPackages->Count > 0 )
		{
			bool bFoundSelectedGroup = false;
			for ( INT PkgIndex = 0; PkgIndex < SelectedPackages->Count; PkgIndex++ )
			{
				ContentBrowser::ObjectContainerNode^ PackageItem = SelectedPackages[PkgIndex];
				if ( PackageItem->IsSelected && PackageItem->GetType() == ContentBrowser::GroupPackage::typeid )
				{
					SelectedGroupName = "";
					while ( PackageItem->Parent != nullptr && PackageItem->GetType() == ContentBrowser::GroupPackage::typeid )
					{
						const FString CurGroupName = CLRTools::ToFString( PackageItem->Name );
						if( SelectedGroupName.Len() == 0 )
						{
							SelectedGroupName = CurGroupName;
						}
						else
						{
							// Prepend parent group
							SelectedGroupName = CurGroupName + TEXT( "." ) + SelectedGroupName;
						}
						PackageItem = (ContentBrowser::ObjectContainerNode^)PackageItem->Parent;
					}

					if ( PackageItem != nullptr && PackageItem->GetType() == ContentBrowser::Package::typeid )
					{
						SelectedPackageName = CLRTools::ToFString(PackageItem->Name);
					}

					bFoundSelectedGroup = true;
					break;
				}
			}

			if ( !bFoundSelectedGroup )
			{
				for ( INT PkgIndex = 0; PkgIndex < SelectedPackages->Count; PkgIndex++ )
				{
					ContentBrowser::ObjectContainerNode^ PackageItem = SelectedPackages[PkgIndex];
					if ( PackageItem->GetType() == ContentBrowser::Package::typeid )
					{
						SelectedPackageName = CLRTools::ToFString(PackageItem->Name);
						break;
					}
				}
			}
		}
	}
}
/**
 * ExecutedRoutedEventHandler for the DataBrowser's custom commands
 * 
 * @param	Sender	the object that generated the event
 * @param	EvtArgs	details about the event that was generated
 */
void MDataBrowserControl::ExecutePackageCommand( System::Object^ Sender, System::Windows::Input::ExecutedRoutedEventArgs^ EvtArgs, const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames )
{
	//no one else should ever handle this event.
	EvtArgs->Handled = true;

	Input::RoutedCommand^ Command = (Input::RoutedCommand^)EvtArgs->Command;

	if ( Command == System::Windows::Input::ApplicationCommands::Save ||
		 Command == ContentBrowser::PackageCommands::SaveAsset )
	{
		EvtArgs->Handled = true;
		
		SavePackages( Packages, FALSE );
		RequestPackageListUpdate( false, NULL );
		DataBrowserCtrl->MyAssets->UpdateStatusForAllAssetsInView( ContentBrowser::AssetStatusUpdateFlags::LoadedStatus );
	}
	else if ( Command == ContentBrowser::PackageCommands::FullyLoadPackage )
	{
		EvtArgs->Handled = true;

		const UBOOL bBeginSlowTask = Packages.Num() > 0 || PackageNames.Num() > 0;
		if ( bBeginSlowTask )
		{
			GWarn->BeginSlowTask( *LocalizeUnrealEd(TEXT("FullyLoadingPackages")), TRUE );
		}

		if ( Packages.Num() > 0 )
		{
			for( INT PackageIndex = 0; PackageIndex < Packages.Num(); PackageIndex++ )
			{
				UPackage* Package = Packages(PackageIndex);

				// we might have null entries in the list if the user selected some packages that weren't loaded
				if ( Package != NULL )
				{
					Package->FullyLoad();
				}
			}
			// refresh is handled by UPackage::FullyLoad
		}
		
		if ( PackageNames.Num() > 0 )
		{
			for ( INT PkgIndex = 0; PkgIndex < PackageNames.Num(); PkgIndex++ )
			{
				const FString& PkgName = PackageNames(PkgIndex);
				FullyLoadPackage(CLRTools::ToString(PkgName));
			}
		}

		if ( bBeginSlowTask )
		{
			GWarn->EndSlowTask();
		}
	}
	else if ( Command == ContentBrowser::PackageCommands::UnloadPackage )
	{
		EvtArgs->Handled = true;
		if ( PackageTools::UnloadPackages(Packages) )
		{
			//@todo - anything else to do here?
		}
	}
	else if ( Command == ContentBrowser::PackageCommands::ImportAsset )
	{
		EvtArgs->Handled = true;

		FString FileTypes, AllExtensions;
		TArray<UFactory*> Factories;			// All the factories we can possibly use

		TMultiMap<INT, UFactory*> MapFactory;//zhaojun@snda ???
		ObjectTools::AssembleListOfImportFactories(Factories, FileTypes, AllExtensions, MapFactory);

		FileTypes = FString::Printf(TEXT("All Files (%s)|%s|%s"),*AllExtensions,*AllExtensions,*FileTypes);

		// Prompt the user for the filenames
		WxFileDialog OpenFileDialog(GApp->EditorFrame, 
			*LocalizeUnrealEd("Import"),
			*CLRTools::ToFString(LastImportPath),
			TEXT(""),
			*FileTypes,
			wxOPEN | wxFILE_MUST_EXIST | wxMULTIPLE,
			wxDefaultPosition
			);

		OpenFileDialog.SetFilterIndex(LastImportFilter);
		if( OpenFileDialog.ShowModal() == wxID_OK )
		{
			GWarn->BeginSlowTask( *LocalizeUnrealEd("Importing"), TRUE );

			LastImportFilter = OpenFileDialog.GetFilterIndex();

			wxArrayString OpenFilePaths;
			OpenFileDialog.GetPaths( OpenFilePaths );

			FString SelectedPackageName=TEXT("MyPackage"), SelectedGroupName;
			GetSelectedPackageAndGroupName(SelectedPackageName, SelectedGroupName);

			FString LastImportPathString;
			if( ObjectTools::ImportFiles( OpenFilePaths, Factories, &LastImportPathString, SelectedPackageName, SelectedGroupName ) )
			{
				LastImportPath = CLRTools::ToString(LastImportPathString);
			}

			GWarn->EndSlowTask();
		}
	}
	else if ( Command == ContentBrowser::PackageCommands::OpenPackage )
	{
		WxFileDialog OpenFileDialog(GApp->EditorFrame, 
									*LocalizeUnrealEd("Open"),
									*CLRTools::ToFString(LastOpenPath),
									TEXT(""),
									TEXT(""),
									wxOPEN | wxFILE_MUST_EXIST | wxMULTIPLE,
									wxDefaultPosition);
		
		if( OpenFileDialog.ShowModal() == wxID_OK )
		{
			const FScopedBusyCursor BusyCursor;
			GWarn->BeginSlowTask( *LocalizeUnrealEd( TEXT("LoadingPackage") ), TRUE );
			
			wxArrayString OpenFilePaths;
			OpenFileDialog.GetPaths( OpenFilePaths );

			TArray< FString > PackageFiles( GPackageFileCache->GetPackageFileList() );
		
			List< ContentBrowser::ObjectContainerNode^ >^ SyncList = gcnew List< ContentBrowser::ObjectContainerNode^ >();
			for(UINT fileIndex = 0; fileIndex < OpenFilePaths.GetCount(); ++fileIndex)
			{
				FFilename Filename = OpenFilePaths[fileIndex].c_str();
				const FString& BaseFilename = Filename.GetBaseFilename();
			
				// check to make sure the package name doesn't already exist.  Package names must be uniqie
				ContentBrowser::ObjectContainerNode^ Result = DataBrowserCtrl->MySources->FindPackage( CLRTools::ToString( BaseFilename ) );
				UBOOL bUniquePackageName = TRUE;
				if( Result != nullptr )
				{
					appMsgf( AMT_OK, LocalizeSecure( LocalizeUnrealEd("Error_PackageAlreadyExists"), *BaseFilename ) );
					//  add the found package to the list so we can sync to it
					SyncList->Add(Result);

					bUniquePackageName = FALSE;
				}
			
				if( bUniquePackageName )
				{
					GWarn->StatusUpdatef(0, 0, *FString::Printf( LocalizeSecure( LocalizeUnrealEd("LoadingPackagef"), *BaseFilename ) ) );
					UPackage* LoadedPackage = PackageTools::LoadPackage( Filename );
					if( LoadedPackage )
					{
						// add the package to a list of packages to sync to after we are done loading
						ContentBrowser::ObjectContainerNode^ PackageItem = dynamic_cast<ContentBrowser::ObjectContainerNode^>( UPackageToPackage( LoadedPackage ) );
						if ( PackageItem != nullptr )
						{
							SyncList->Add(PackageItem);
						}
					}
				}
				LastOpenPath = CLRTools::ToString( Filename.GetPath() );
			}

			DataBrowserCtrl->MyPackagePanel->SetSelectedPackages(SyncList);
			RequestPackageListUpdate( false, NULL );
			
			GWarn->EndSlowTask();
		}
	}
	else if ( Command == ContentBrowser::PackageCommands::BulkExport )
	{
		EvtArgs->Handled = true;
		
		// Gather all of the packages to be bulk exported
		TArray<UPackage*> PkgsToBulkExport;
		PkgsToBulkExport.Append( Packages );

		// Iterate over each of the provided package names, adding them to the list of packages to bulk export
		// if they're not already accounted for
		for ( TArray<FString>::TConstIterator PackageNameIter( PackageNames ); PackageNameIter; ++PackageNameIter )
		{
			UPackage* CurPackage = FindObject<UPackage>( NULL, **PackageNameIter );
			
			// If the package couldn't be found, try to load it
			if ( !CurPackage )
			{
				CurPackage = PackageTools::LoadPackage( FFilename( *PackageNameIter ) );
			}
			if ( CurPackage )
			{
				PkgsToBulkExport.AddUniqueItem( CurPackage );
			}
		}

		// Track which types the content browser is currently filtering by, if any. Filtered types will be used
		// if the user opts to export objects into their own files, at which point only objects matching the filtered
		// types will be exported.
		TSet<UClass*> FilteredClasses;
		List<String^>^ SelectedOptions = DataBrowserCtrl->FilterPanel->ObjectTypeFilterTier->GetSelectedOptions();
		if ( SelectedOptions->Count > 0 )
		{
			// Store each selected filter in a set for quick lookup
			TSet<FString> SelectedFilters;
			for each ( String^ CurFilter in SelectedOptions )
			{
				SelectedFilters.Add( CLRTools::ToFString( CurFilter ) );
			}

			// Iterate over each GB type, if the type is currently being used as a filter, add each of its corresponding classes
			// to the filtered classes set
			for ( GBTToClassMap::TConstIterator GBTMapIter( *( BrowsableObjectTypeToClassMap.Get() ) ); GBTMapIter; ++GBTMapIter )
			{
				const UGenericBrowserType* CurBrowserType = GBTMapIter.Key();
				if ( SelectedFilters.Contains( CurBrowserType->GetBrowserTypeDescription() ) )
				{
					FilteredClasses.Add( GBTMapIter.Value() );
					break;
				}
			}
		}
		LastExportPath = CLRTools::ToString(PackageTools::DoBulkExport(PkgsToBulkExport, CLRTools::ToFString(LastExportPath), ClassToBrowsableObjectTypeMap.Get(), FilteredClasses.Num() > 0 ? &FilteredClasses : NULL));
	}
	else if ( Command == ContentBrowser::PackageCommands::BulkImport )
	{
		EvtArgs->Handled = true;
		LastImportPath = CLRTools::ToString(PackageTools::DoBulkImport(CLRTools::ToFString(LastImportPath)));
	}
	else if ( Command == ContentBrowser::PackageCommands::LocExport )
	{
		EvtArgs->Handled = true;
		LastExportPath = CLRTools::ToString(PackageTools::ExportLocalization(Packages, CLRTools::ToFString(LastExportPath), ClassToBrowsableObjectTypeMap.Get()));
	}
	else if ( Command == ContentBrowser::PackageCommands::CheckErrors )
	{
		EvtArgs->Handled = true;

		// oscar@snda
		for (INT i = 0; i < Packages.Num(); ++i)
		{
			UPackage* Package = Packages(i);
			if( !Package )
			{				
				continue;;
			}

			if(!Package->IsFullyLoaded())
			{
				Package->FullyLoad();
			}
		}
		// oscar@snda

		// Retrieve list of selected packages.
		TArray<UObject*> Objects;
		TArray<UObject*> TrashcanObjects;
		GEditor->CheckForTrashcanReferences( Packages, Objects, TrashcanObjects );

		check( Objects.Num() == TrashcanObjects.Num() );

		// Output to dialog.
		FString OutputString;
		if ( Objects.Num() > 0 )
		{
			for ( INT ObjectIndex = 0 ; ObjectIndex < Objects.Num() ; ++ObjectIndex )
			{
				UObject* Object = Objects(ObjectIndex);
				const UObject* TrashcanObject = TrashcanObjects(ObjectIndex);

				Object->CheckForErrors();
				OutputString += FString::Printf( TEXT("%s -> %s  :  %s  ->  %s%s"),
					*Object->GetOutermost()->GetName(), *TrashcanObject->GetOutermost()->GetName(), *Object->GetFullName(), *TrashcanObject->GetFullName(), LINE_TERMINATOR );
			}
		}
		else
		{
			OutputString = TEXT("No package errors found.");
		}
		appMsgf( AMT_OK, *OutputString );
	}
	else if ( Command == ContentBrowser::PackageCommands::SyncPackageView )
	{
		EvtArgs->Handled = true;

		// get a list of Package items that correspond to the packages containing the objects in the set
		List< String^ >^ AssetPackageNames = gcnew List< String^ >();
		for each( ContentBrowser::DataAssetItem^ CurSelectedAsset in DataBrowserCtrl->AssetView->AssetListView->SelectedItems )
		{
			if ( !AssetPackageNames->Contains(CurSelectedAsset->PathOnly) )
			{
				AssetPackageNames->Add(CurSelectedAsset->PathOnly);
			}
		}

		List< ContentBrowser::ObjectContainerNode^ >^ PackagesList = gcnew List< ContentBrowser::ObjectContainerNode^ >();
		for each ( String^ PackagePathName in AssetPackageNames )
		{
			// don't require all packages to be matched - if the root package isn't loaded, we may not have groups yet
			ContentBrowser::ObjectContainerNode^ AssetNode = DataBrowserCtrl->MySources->FindPackage(PackagePathName, ContentBrowser::PackagePanelModel::EPackageSearchOptions::AllowIncompleteMatch);
			if ( AssetNode != nullptr )
			{
				if ( !PackagesList->Contains(AssetNode) )
				{
					PackagesList->Add(AssetNode);
				}
			}
		}

		// now sync the packages list to these packages
		DataBrowserCtrl->MyPackagePanel->SetSelectedPackages(PackagesList);

		// Also kick off a deferred selection
		DataBrowserCtrl->AssetView->StartDeferredAssetSelection( DataBrowserCtrl->AssetView->CloneSelectedAssetFullNames() );
	}
// 		else if ( Command == DataBrowser::PackageCommands::ResliceFracturedMeshes )
// 		{
// 			//@todo
// 		}
}

/**
 * CanExecuteRoutedEventHandler for the DataBrowser's custom commands
 * 
 * @param	Sender	the object that generated the event
 * @param	EvtArgs	details about the event that was generated
 */
void MDataBrowserControl::CanExecuteMenuCommand( System::Object^ Sender, System::Windows::Input::CanExecuteRoutedEventArgs^ EvtArgs )
{
	//no one else should ever handle this event.
	EvtArgs->Handled = true;

	// Reentrancy guard; sometimes commands trigger a WndProc which creates problems for us.  For example,
	// saving a package can trigger a window proc which will call CanExecuteMenuCommand which talks to 
	// UObjects that are in the middle of being serialized (not allowed.)
	if( bIsExecutingMenuCommand )
	{
		// We're already executing a command
		EvtArgs->CanExecute = false;
	}

	// Check to see if the window is disabled.  This usually means that a modal dialog is up and
	// we don't want to be processing commands which could be reentrant
	else if( !DataBrowserCtrl->IsEnabled )
	{
		// Modal dialog is up!
		EvtArgs->CanExecute = false;
	}
	else
	{
		Input::RoutedCommand^ Command = (Input::RoutedCommand^)EvtArgs->Command;

		if ( Command == ContentBrowser::PackageCommands::FindModifierKey )	//Bruce@snda 
		{
			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ Packages = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();
			if(Packages->Count > 0)
			{
				ContentBrowser::SourceTreeNode^ Node = Packages[0];
				FString RootPath;
				FString PathType = CLRTools::ToFString( Node->Name );
				GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString( Node->Name), PathType);
				bool cond1 = (Node->GetType() == ContentBrowser::Folder::typeid);
				FString testStr = *CLRTools::ToFString(Node->Name);
				bool cond2 = GPackageFileCache->FindDataRoot(testStr, RootPath) == TRUE;
				bool cond3 = RootPath.InStr(CLRTools::ToFString(Node->Name)) != -1;
				bool cond4 = (PathType.InStr("Modifier")) != -1;
				EvtArgs->CanExecute = cond1 && cond2 && cond3 && cond4;
			}
			else
				EvtArgs->CanExecute = false;
		}	//Bruce@snda 
		else if(Command == ContentBrowser::PackageCommands::AddPackage)
		{
			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ Packages = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();
			if(Packages->Count > 0)
			{
				ContentBrowser::SourceTreeNode^ Node = Packages[0];
				FString RootPath;
				EvtArgs->CanExecute = (Node->GetType() == ContentBrowser::Folder::typeid) &&
					GPackageFileCache->FindDataRoot(*CLRTools::ToFString(Node->Name), RootPath) &&
					(RootPath.InStr(CLRTools::ToFString(Node->Name)) != -1);
			}
			else
				EvtArgs->CanExecute = false;
		}
		else if(Command == ContentBrowser::PackageCommands::NewObject)
		{
			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ Packages = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();
			if(Packages->Count > 0)
			{
				ContentBrowser::SourceTreeNode^ Node = Packages[0];
				EvtArgs->CanExecute = Node->GetType() == ContentBrowser::Package::typeid;
			}
			else
				EvtArgs->CanExecute = false;
		}
		else if(Command == ContentBrowser::PackageCommands::SaveAsExcel ||
			Command == ContentBrowser::PackageCommands::ImportFromExcel)
		{
			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ Packages = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();
			if(Packages->Count > 0)
			{
				ContentBrowser::SourceTreeNode^ Node = Packages[0];
				FString RootPath;
				//int nIndex;
				FString value;
				bool cond1 = Node->GetType() == ContentBrowser::Folder::typeid;
				UBOOL cond2 = GPackageFileCache->FindDataRoot(*CLRTools::ToFString(Node->Name), RootPath);
				bool cond3 = RootPath.InStr(CLRTools::ToFString(Node->Name)) != -1;
				UBOOL cond4 = GConfig->GetString(TEXT("Excel.SaveAsExcel"), *CLRTools::ToFString(Node->Name), value, GEditorIni);
				EvtArgs->CanExecute = cond1 && cond2 && cond3 && cond4;
			}
			else
				EvtArgs->CanExecute = false;
		}
		else if(Command == ContentBrowser::PackageCommands::SaveAsALL)
		{
			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ Packages = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();
			if(Packages->Count > 0)
			{
				ContentBrowser::SourceTreeNode^ Node = Packages[0];
				FString RootPath;
				bool cond = CLRTools::ToFString(Node->Name).InStr(FString("MMOContent")) != -1;
				EvtArgs->CanExecute = cond;
			}
			else
				EvtArgs->CanExecute = false;
		}
		else if(Command == ContentBrowser::PackageCommands::SaveModifierTree)
		{
 			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ Packages = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();
 			if(Packages->Count > 0)
 			{
 				ContentBrowser::SourceTreeNode^ Node = Packages[0];
 				FString RootPath;
 				FString PathType = CLRTools::ToFString( Node->Name);// = "MMOAbility";
 				GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString( Node->Name), PathType);
				bool cond1 = (Node->GetType() == ContentBrowser::Folder::typeid);
				FString testStr = *CLRTools::ToFString(Node->Name);
				bool cond2 = GPackageFileCache->FindDataRoot(testStr, RootPath) == TRUE;
				bool cond3 = RootPath.InStr(CLRTools::ToFString(Node->Name)) != -1;
				bool cond4 = (PathType.InStr("Modifier")) != -1;
 				EvtArgs->CanExecute = cond1 && cond2 && cond3 && cond4;
 			}
 			else
 				EvtArgs->CanExecute = false;
		}
		else if(Command == ContentBrowser::PackageCommands::SaveSpellNode)
		{
			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ Packages = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();
			if(Packages->Count > 0)
			{
				ContentBrowser::SourceTreeNode^ Node = Packages[0];
				FString RootPath;
				FString PathType = CLRTools::ToFString( Node->Name);// = "MMOAbility";
				GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString( Node->Name), PathType);
				
				bool cond1 = (Node->GetType() == ContentBrowser::Folder::typeid);
				UBOOL cond2 = GPackageFileCache->FindDataRoot(*CLRTools::ToFString(Node->Name), RootPath);
				bool cond3 = RootPath.InStr(CLRTools::ToFString(Node->Name)) != -1;
				bool cond4 = (PathType.InStr("Ability")) != -1;

				EvtArgs->CanExecute = cond1 && cond2 && cond3 && cond4;
			}
			else
				EvtArgs->CanExecute = false;
		}
		else if(Command == ContentBrowser::PackageCommands::ExportAITable
			|| Command == ContentBrowser::PackageCommands::ImportAITable)
		{
			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ Packages = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();
			if(Packages->Count > 0)
			{
				ContentBrowser::SourceTreeNode^ Node = Packages[0];
				FString RootPath;
				FString PathType = CLRTools::ToFString( Node->Name);// = "AI_Affix_Table";

				GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString( Node->Name), PathType);

				bool cond1 = (Node->GetType() == ContentBrowser::Folder::typeid);
				UBOOL cond2 = GPackageFileCache->FindDataRoot(*CLRTools::ToFString(Node->Name), RootPath);
				bool cond3 = RootPath.InStr(CLRTools::ToFString(Node->Name)) != -1;
				bool cond4 = (PathType.InStr("AI_Affix_Table")) != -1;

				EvtArgs->CanExecute = cond1 && cond2 && cond3 && cond4;
			}
			else
				EvtArgs->CanExecute = false;
		}
		else if (Command == ContentBrowser::PackageCommands::OpenExplorer)
		{
			EvtArgs->CanExecute = DataBrowserCtrl->MyPackagePanel->AnyNodesSelected();
		}
		else if ( Command == ContentBrowser::PackageCommands::FullyLoadPackage )
		{
			// fully-load command also supports loading packages which are unloaded
			TArray<UPackage*> Packages;
			TArray<FString> PackageNames;
			GetSelectedRootPackages(Packages, &PackageNames, FALSE);

			OnCanExecutePackageCommand(Sender, EvtArgs, Packages, PackageNames);
		}
		else if(Command == ContentBrowser::PackageCommands::CheckDataErrors)
		{
			EvtArgs->CanExecute = true;
		}
		else if(Command == ContentBrowser::PackageCommands::CheckDataErrorsDeep)
		{
			EvtArgs->CanExecute = true;
		}
		else if (Command->OwnerType == ContentBrowser::PackageCommands::typeid
		||	Command == System::Windows::Input::ApplicationCommands::Save
		||	Command == ContentBrowser::PackageCommands::SaveAsset)
		{
			TArray<UPackage*> Packages;
			TArray<FString> PackageNames;
			GetSelectedRootPackages(Packages, &PackageNames, TRUE);

			OnCanExecutePackageCommand(Sender, EvtArgs, Packages, PackageNames);
		}
		else if ( Command->OwnerType == ContentBrowser::SourceControlCommands::typeid )
		{
			TArray<UPackage*> Packages;
			TArray<FString> PackageNames;
			GetSelectedRootPackages(Packages, &PackageNames, TRUE);

			OnCanExecuteSCCCommand(Sender, EvtArgs, Packages, PackageNames);
		}
		else if ( Command->OwnerType == ContentBrowser::ObjectFactoryCommands::typeid )
		{
			CanExecuteNewObjectCommand(Sender, EvtArgs);
		}
		else if ( Command->OwnerType == ContentBrowser::CollectionCommands::typeid )
		{
			CanExecuteCollectionCommand( Sender, EvtArgs );
		}
	}
}

/**
 * ExecutedRoutedEventHandler for the DataBrowser's custom commands
 * 
 * @param	Sender	the object that generated the event
 * @param	EvtArgs	details about the event that was generated
 */
void MDataBrowserControl::ExecuteMenuCommand( System::Object^ Sender, System::Windows::Input::ExecutedRoutedEventArgs^ EvtArgs )
{
	//no one else should ever handle this event.
	EvtArgs->Handled = true;

	// Reentrancy guard; sometimes commands trigger a WndProc which creates problems for us.  For example,
	// saving a package can trigger a window proc which will call CanExecuteMenuCommand which talks to 
	// UObjects that are in the middle of being serialized (not allowed.)
	if( !bIsExecutingMenuCommand )
	{
		bIsExecutingMenuCommand = true;

		Input::RoutedCommand^ Command = (Input::RoutedCommand^)EvtArgs->Command;

		if( Command == ContentBrowser::PackageCommands::FindModifierKey )   //Bruce@snda 
		{
			WxDlgModifierSettingValue dlg;

			TArray<FString> TypeList;
			TypeList.AddItem( LocalizeUnrealEdMMO( TEXT("FindModifierTypeName1") ) );
			TypeList.AddItem( LocalizeUnrealEdMMO( TEXT("FindModifierTypeName2") ) );
			TypeList.AddItem( LocalizeUnrealEdMMO( TEXT("FindModifierTypeName3") ) );

			dlg.SetTypeList(TypeList);
			int Result = dlg.ShowModal();
			if ( Result == wxID_CANCEL )
				return;

			INT Num = appAtoi( *dlg.GetText() );
			INT TypeSel = dlg.GetType();

			const FString CheckMapLocString(LocalizeUnrealEd(TEXT("CheckingMap")));
			GWarn->BeginSlowTask( *CheckMapLocString, FALSE);
			GWarn->MapCheck_BeginBulkAdd();
			GWarn->MapCheck_Clear();

			TArray<UPackage*> Packages;
			TArray<FString> PackageNames;
			TArray<FString> DirArray;
			GConfig->GetArray( TEXT("ModifierExport"), TEXT("TemplateDir"), DirArray, GGameIni );
			for (INT k = 0; k < DirArray.Num(); ++k)
			{
				FullLoadPackages(DirArray(k), Packages, PackageNames);
				FString PkgPath = "MMOAbility";
				GPackageFileCache->FindDataPathTypes(*DirArray(k), PkgPath);
				INT ClassIndex = GEditor->EditorClassHierarchy->Find(PkgPath);
				UClass* ClickedClass = GEditor->EditorClassHierarchy->GetClass(ClassIndex);
			}

			for ( INT k = 0; k < PackageNames.Num(); ++k )
			{
				debugIDf( TEXT("Package Name"), TEXT("=====  %s"), *PackageNames(k) );
			}

			TArray<USeqAct_Modifier*> ModifierList;
			TArray< UObject* > ObjectsInPackages;
			PackageTools::GetObjectsInPackages(&Packages, NULL, ObjectsInPackages);

			for(int i = 0; i < ObjectsInPackages.Num(); ++i)
			{
				UObject* Obj = ObjectsInPackages(i);
				if( Obj && Obj->IsA( USeqAct_Modifier::StaticClass() ) )
				{
					USeqAct_Modifier *Modifier = ( USeqAct_Modifier* )Obj;
					if ( Modifier->Modifiers.Num() )
					{
						for ( int i = 0; i < Modifier->Modifiers.Num(); ++i )
						{
							if ( ( ( Modifier->Modifiers(i)->Key == Num ) || ( Modifier->Key == Num ) ) && 
								 ( ( Modifier->Modifiers(i)->Type == TypeSel ) || ( Modifier->Type == TypeSel ) ) )
							{
								GWarn->MapCheck_Add( MCTYPE_WARNING, Obj, *FString::Printf( TEXT("%s"), *Modifier->GetPathName() ), NULL );
								break;
							}
						}
					}
				}
			}

			GWarn->MapCheck_EndBulkAdd();
			GWarn->EndSlowTask();
			GWarn->MapCheck_ShowConditionally();			
		}   //Bruce@snda 
		else if(Command == ContentBrowser::PackageCommands::AddPackage)
		{

			TArray<FString> PackageFiles;// = GPackageFileCache->GetDataFileList();

			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ Packages = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();
			for ( INT PackageIndex = 0; PackageIndex < Packages->Count; PackageIndex++ )
			{
				ContentBrowser::SourceTreeNode^ Node = Packages[PackageIndex];
				if ( Node->GetType() == ContentBrowser::Folder::typeid)
				{


					FString name = CLRTools::ToFString( Node->Name );
					//name.ReplaceInline( TEXT( "." ), TEXT( "_" ));
					PackageFiles.AddItem(*name);
				}

			}
			ContentBrowser::SourceTreeNode^ Node = Packages[0];
			FString ObjType = "MMOAbility";
			GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString( Node->Name), ObjType);
			UClass* ClickedClass;
 			INT ClassIndex = GEditor->EditorClassHierarchy->Find(ObjType);
 			ClickedClass = GEditor->EditorClassHierarchy->GetClass(ClassIndex);

			FString PackageName, FolderName, GroupName;
			CreatePackage( ObjType, FolderName, GroupName, PackageName);
			


		}
		else if(Command == ContentBrowser::PackageCommands::NewObject)
		{
			TArray<FString> PackageFiles;// = GPackageFileCache->GetDataFileList();

			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ Packages = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();
			for ( INT PackageIndex = 0; PackageIndex < Packages->Count; PackageIndex++ )
			{
				ContentBrowser::SourceTreeNode^ Node = Packages[PackageIndex];
				if ( Node->GetType() == ContentBrowser::Folder::typeid)
				{


					FString name = CLRTools::ToFString( Node->Name );
					//name.ReplaceInline( TEXT( "." ), TEXT( "_" ));
					PackageFiles.AddItem(*name);
				}

			}
			ContentBrowser::SourceTreeNode^ Node = Packages[0];
			FString PkgPath = "MMOAbility";
			GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString( Node->Name), PkgPath);
			UClass* ClickedClass;
			INT ClassIndex = GEditor->EditorClassHierarchy->Find(PkgPath);
			ClickedClass = GEditor->EditorClassHierarchy->GetClass(ClassIndex);

			FString ArchetypeName, PackageName, GroupName;

			if (ClickedClass->IsChildOf( AActor::StaticClass() ) )
			{
				// actor archetype
				AActor* const Actor =
					GWorld->SpawnActor(
					ClickedClass,
					NAME_None,
					FVector( 0.0f, 0.0f, 0.0f ),
					FRotator( 0, 0, 0 ),
					NULL,
					TRUE );		// Disallow placement failure due to collisions?
				if( Actor != NULL )
				{
					CreateFromObject( Actor, ArchetypeName, PackageName, GroupName );
					GWorld->EditorDestroyActor( Actor, FALSE );
				}
			}
			else
			{
				// creating archetype of a UObject
				// ok to leave this dangle, garbage collection will clean it up
				UObject* const Object = UObject::StaticConstructObject(ClickedClass);
				//CreatePackage(PkgPath, ArchetypeName, PackageName, GroupName);
				CreateFromObject( Object, ArchetypeName, PackageName, GroupName );
			}

		}
		else if(Command == ContentBrowser::PackageCommands::SaveAsExcel)
		{
			TArray<FString> PackageFiles;// = GPackageFileCache->GetDataFileList();

			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ PackagesPath = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();

			ContentBrowser::SourceTreeNode^ Node = PackagesPath[0];

			FString ServerDataPth, ExlFileName, Path;
			GConfig->GetString(TEXT("Configuration"), TEXT("ServerDataPath"), ServerDataPth, GEditorIni);
			UBOOL bRet = GConfig->GetString(TEXT("Configuration"), *CLRTools::ToFString(Node->Name), ExlFileName, GEditorIni);
			if (bRet)
			{
				Path = ServerDataPth + ExlFileName;
				Path = GFileManager->ConvertToAbsolutePath(*Path);

				FString value;
				GConfig->GetString(TEXT("Excel.SaveAsExcel"), *CLRTools::ToFString(Node->Name), value, GEditorIni);

				TArray<UPackage*> Packages;
				TArray<FString> PackageNames;
				FullLoadPackages(value, Packages, PackageNames);

				FString PkgPath;
				GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString( Node->Name), PkgPath);
				INT ClassIndex = GEditor->EditorClassHierarchy->Find(PkgPath);
				if(ClassIndex == -1)
				{
					//Class not found?
				}
				else
				{
					UClass* ClickedClass = GEditor->EditorClassHierarchy->GetClass(ClassIndex);

					SaveAsExcel(Packages, PackageNames, Path, ClassIndex, ExlFileName);	
				}
			}
					
		}
		else if(Command == ContentBrowser::PackageCommands::ImportFromExcel)
		{
			TArray<FString> PackageFiles;// = GPackageFileCache->GetDataFileList();

			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ PackagesPath = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();

			ContentBrowser::SourceTreeNode^ Node = PackagesPath[0];

			FString ServerDataPth, ExlFileName, Path;
			GConfig->GetString(TEXT("Configuration"), TEXT("ServerDataPath"), ServerDataPth, GEditorIni);
			GConfig->GetString(TEXT("Configuration"), *CLRTools::ToFString(Node->Name), ExlFileName, GEditorIni);
			GConfig->GetString(TEXT("Configuration"), *CLRTools::ToFString(Node->Name), ExlFileName, GEditorIni);
			Path = ServerDataPth + ExlFileName;
			Path = GFileManager->ConvertToAbsolutePath(*Path);

			FString value;
			GConfig->GetString(TEXT("Excel.SaveAsExcel"), *CLRTools::ToFString(Node->Name), value, GEditorIni);

			TArray<UPackage*> Packages;
			TArray<FString> PackageNames;
			FullLoadPackages(value, Packages, PackageNames);

			FString PkgPath = "MMOAbility";
			GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString( Node->Name), PkgPath);
			INT ClassIndex = GEditor->EditorClassHierarchy->Find(PkgPath);
			UClass* ClickedClass = GEditor->EditorClassHierarchy->GetClass(ClassIndex);

			ImportFromExcel(Packages, PackageNames, Path, ClassIndex, ClickedClass);			
		}
		else if(Command == ContentBrowser::PackageCommands::SaveAsALL)
		{
			//modifier
			{
				FString ServerDataPth, ExlFileName, Path;
				GConfig->GetString(TEXT("Configuration"), TEXT("ServerDataPath"), ServerDataPth, GEditorIni);
				GConfig->GetString(TEXT("Configuration"), TEXT("08_Modifier"), ExlFileName, GEditorIni);
				Path = ServerDataPth + ExlFileName;
				Path = GFileManager->ConvertToAbsolutePath(*Path);

				TArray<UPackage*> Packages;
				TArray<FString> PackageNames;
				TArray<FString> DirArray;
				GConfig->GetArray( TEXT("ModifierExport"), TEXT("TemplateDir"), DirArray, GGameIni );
				for (INT k = 0; k < DirArray.Num(); ++k)
				{
					FullLoadPackages(DirArray(k), Packages, PackageNames);
					FString PkgPath = "MMOAbility";
					GPackageFileCache->FindDataPathTypes(*DirArray(k), PkgPath);
					INT ClassIndex = GEditor->EditorClassHierarchy->Find(PkgPath);
					UClass* ClickedClass = GEditor->EditorClassHierarchy->GetClass(ClassIndex);
#if !SHIPPING_PC_GAME

					FCheckoutResult sccResult = FSourceControl::TryCheckOut(Path);
					if(sccResult.AddToSCC)
					{
						SaveModifier(Packages, PackageNames, Path, ClassIndex);
						sccResult.TryAddToDefaultChangelist();
					}
#endif // !SHIPPING_PC_GAME
				}

			}
			//spell_Node
			{
				FString ServerDataPth, ExlFileName, Path;
				GConfig->GetString(TEXT("Configuration"), TEXT("ServerDataPath"), ServerDataPth, GEditorIni);
				//GConfig->GetString(TEXT("Configuration"), TEXT("08_Modifier"), ExlFileName, GEditorIni);
				ExlFileName = "spell_node.xlsx";
				Path = ServerDataPth + ExlFileName;
				Path = GFileManager->ConvertToAbsolutePath(*Path);

				TArray<UPackage*> Packages;
				TArray<FString> PackageNames;
				TArray<FString> DirArray;
				GConfig->GetArray( TEXT("AbilityExport"), TEXT("TemplateDir"), DirArray, GGameIni );
				for (INT k = 0; k < DirArray.Num(); ++k)
				{
					FullLoadPackages(DirArray(k), Packages, PackageNames);
					FString PkgPath = "MMOAbility";
					GPackageFileCache->FindDataPathTypes(*DirArray(k), PkgPath);
					INT ClassIndex = GEditor->EditorClassHierarchy->Find(PkgPath);
					UClass* ClickedClass = GEditor->EditorClassHierarchy->GetClass(ClassIndex);

					SaveSpellNode(Packages, PackageNames, Path, ClassIndex);
				}

			}

			TArray<FString> NodeNames;
			TArray<FString> NodeConfigNames;

			FConfigFile* File = GConfig->Find(GEditorIni, 0 );
			if( !File )
				return;
			FConfigSection* Sec = File->Find(TEXT("Excel.SaveALL"));
			if( !Sec )
				return;
			for( FConfigSection::TIterator It(*Sec); It; ++It )
			{
				new(NodeNames) FString(FString::Printf( TEXT("%s"), *It.Key().ToString()));
				new(NodeConfigNames) FString(FString::Printf( TEXT("%s"), *It.Value() ));
			}
			
			for(int i = 0; i < NodeNames.Num(); ++i)
			{
				SaveAsExcelWithoutSelected(NodeNames(i), NodeConfigNames(i));
			}
		}
		else if (Command == ContentBrowser::PackageCommands::AnalyzeReference)
		{
			TArray<UPackage*> Packages;
			TArray<FString> PackageNames;
			GetSelectedRootPackages(Packages, &PackageNames, TRUE);

			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ PackagesPath = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();
			ContentBrowser::SourceTreeNode^ Node = PackagesPath[0];
			CLRAnalyzeReference(CLRTools::ToFString(Node->Name), PackageNames);
		}
		else if(Command == ContentBrowser::PackageCommands::SaveModifierTree)
		{
			TArray<FString> PackageFiles;// = GPackageFileCache->GetDataFileList();

			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ PackagesPath = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();

			ContentBrowser::SourceTreeNode^ Node = PackagesPath[0];
			FString ServerDataPth, ExlFileName, Path;
			GConfig->GetString(TEXT("Configuration"), TEXT("ServerDataPath"), ServerDataPth, GEditorIni);
			GConfig->GetString(TEXT("Configuration"), *CLRTools::ToFString(Node->Name), ExlFileName, GEditorIni);
			Path = ServerDataPth + ExlFileName;
			Path = GFileManager->ConvertToAbsolutePath(*Path);

			TArray<UPackage*> Packages;
			TArray<FString> PackageNames;
			GetSelectedRootPackages(Packages, &PackageNames, TRUE);
			if ( PackageNames.Num() > 0 )
			{
				for ( INT PkgIndex = 0; PkgIndex < PackageNames.Num(); PkgIndex++ )
				{
					const FString& PkgName = PackageNames(PkgIndex);
					FullyLoadPackage(CLRTools::ToString(PkgName));
				}
			}
			Packages.Empty();
			PackageNames.Empty();
			GetSelectedRootPackages(Packages, &PackageNames, TRUE);

			FString PkgPath = "MMOAbility";
			GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString( Node->Name), PkgPath);
			INT ClassIndex = GEditor->EditorClassHierarchy->Find(PkgPath);
			UClass* ClickedClass = GEditor->EditorClassHierarchy->GetClass(ClassIndex);

#if !SHIPPING_PC_GAME

			FCheckoutResult sccResult = FSourceControl::TryCheckOut(Path);
			if(sccResult.AddToSCC)
			{
				SaveModifier(Packages, PackageNames, Path, ClassIndex);
				sccResult.TryAddToDefaultChangelist();
			}
#endif // !SHIPPING_PC_GAME
		}
		else if(Command == ContentBrowser::PackageCommands::SaveSpellNode)
		{
			TArray<FString> PackageFiles;// = GPackageFileCache->GetDataFileList();

			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ PackagesPath = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();

			ContentBrowser::SourceTreeNode^ Node = PackagesPath[0];

			FString ServerDataPth, ExlFileName, Path;
			GConfig->GetString(TEXT("Configuration"), TEXT("ServerDataPath"), ServerDataPth, GEditorIni);
			//GConfig->GetString(TEXT("Configuration"), *CLRTools::ToFString(Node->Name), ExlFileName, GEditorIni);
			ExlFileName = "spell_node.xlsx";
			Path = ServerDataPth + ExlFileName;
			Path = GFileManager->ConvertToAbsolutePath(*Path);

			TArray<UPackage*> Packages;
			TArray<FString> PackageNames;
			GetSelectedRootPackages(Packages, &PackageNames, TRUE);
			if ( PackageNames.Num() > 0 )
			{
				for ( INT PkgIndex = 0; PkgIndex < PackageNames.Num(); PkgIndex++ )
				{
					const FString& PkgName = PackageNames(PkgIndex);
					FullyLoadPackage(CLRTools::ToString(PkgName));
				}
			}
			Packages.Empty();
			PackageNames.Empty();
			GetSelectedRootPackages(Packages, &PackageNames, TRUE);

			FString PkgPath = "MMOAbility";
			GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString( Node->Name), PkgPath);
			INT ClassIndex = GEditor->EditorClassHierarchy->Find(PkgPath);
			UClass* ClickedClass = GEditor->EditorClassHierarchy->GetClass(ClassIndex);

			SaveSpellNode(Packages, PackageNames, Path, ClassIndex);					
		}
		else if(Command == ContentBrowser::PackageCommands::ExportAITable)
		{
			TArray<FString> PackageFiles;// = GPackageFileCache->GetDataFileList();

			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ PackagesPath = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();

			ContentBrowser::SourceTreeNode^ Node = PackagesPath[0];

			FString ServerDataPth, ExlFileName, Path;
			GConfig->GetString(TEXT("Configuration"), TEXT("Affix_TablePath"), ServerDataPth, GEditorIni);
			//GConfig->GetString(TEXT("Configuration"), *CLRTools::ToFString(Node->Name), ExlFileName, GEditorIni);
			//ExlFileName = "spell_node.xlsx";
			Path = ServerDataPth;// + ExlFileName;
			Path = GFileManager->ConvertToAbsolutePath(*Path);

			TArray<UPackage*> Packages;
			TArray<FString> PackageNames;
			GetSelectedRootPackages(Packages, &PackageNames, TRUE);
			if ( PackageNames.Num() > 0 )
			{
				for ( INT PkgIndex = 0; PkgIndex < PackageNames.Num(); PkgIndex++ )
				{
					const FString& PkgName = PackageNames(PkgIndex);
					FullyLoadPackage(CLRTools::ToString(PkgName));
				}
			}
			Packages.Empty();
			PackageNames.Empty();
			GetSelectedRootPackages(Packages, &PackageNames, TRUE);

			FString PkgPath = "MMOAbility";
			GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString( Node->Name), PkgPath);
			INT ClassIndex = GEditor->EditorClassHierarchy->Find(PkgPath);
			UClass* ClickedClass = GEditor->EditorClassHierarchy->GetClass(ClassIndex);

			ExportTable(Packages, PackageNames, Path, ClassIndex);					
		}
		else if(Command == ContentBrowser::PackageCommands::ImportAITable)
		{
			TArray<FString> PackageFiles;// = GPackageFileCache->GetDataFileList();

			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ PackagesPath = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();

			ContentBrowser::SourceTreeNode^ Node = PackagesPath[0];

			FString ServerDataPth, ExlFileName, Path;
			GConfig->GetString(TEXT("Configuration"), TEXT("Affix_TablePath"), ServerDataPth, GEditorIni);
			//GConfig->GetString(TEXT("Configuration"), *CLRTools::ToFString(Node->Name), ExlFileName, GEditorIni);
			//ExlFileName = "spell_node.xlsx";
			Path = ServerDataPth;// + ExlFileName;
			Path = GFileManager->ConvertToAbsolutePath(*Path);

			TArray<UPackage*> Packages;
			TArray<FString> PackageNames;
			GetSelectedRootPackages(Packages, &PackageNames, TRUE);
			if ( PackageNames.Num() > 0 )
			{
				for ( INT PkgIndex = 0; PkgIndex < PackageNames.Num(); PkgIndex++ )
				{
					const FString& PkgName = PackageNames(PkgIndex);
					FullyLoadPackage(CLRTools::ToString(PkgName));
				}
			}
			Packages.Empty();
			PackageNames.Empty();
			GetSelectedRootPackages(Packages, &PackageNames, TRUE);

			FString PkgPath = "MMOAbility";
			GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString( Node->Name), PkgPath);
			INT ClassIndex = GEditor->EditorClassHierarchy->Find(PkgPath);
			UClass* ClickedClass = GEditor->EditorClassHierarchy->GetClass(ClassIndex);

			ImportTable(Packages, PackageNames, Path, ClassIndex);					
		}
		else if (Command == ContentBrowser::PackageCommands::OpenExplorer)
		{
			if( DataBrowserCtrl->MyPackagePanel->AnyNodesSelected() )
			{
				// Create a flat list of selected nodes
				ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ SelectedNodes = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();
				for ( INT PackageIndex = 0; PackageIndex < SelectedNodes->Count; PackageIndex++ )
				{
					ContentBrowser::SourceTreeNode^ Node = SelectedNodes[PackageIndex];
					if ( Node != nullptr )
					{
						// we have a node - open the explorer window here
						// first, get the "TreeView" path to the node, 
						Text::StringBuilder^ NodeTreeViewPath = gcnew Text::StringBuilder(CLRTools::ToString(appBaseDir()));
						NodeTreeViewPath->Append("..\\..");

						// if this is a group package, switch to the outermost package
						if ( Node->GetType() == ContentBrowser::GroupPackage::typeid )
						{
							ContentBrowser::GroupPackage^ GroupNode = safe_cast<ContentBrowser::GroupPackage^>(Node);
							Node = GroupNode->OutermostPackage;
						}

						// then convert the forward slashes from the TreeView path into backslashes so that explorer recognizes the path
						NodeTreeViewPath->Append(Node->FullTreeviewPath->Replace(ContentBrowser::SourceTreeNode::TreeViewNodeSeparator, System::IO::Path::DirectorySeparatorChar));

						FString Path = CLRTools::ToFString( NodeTreeViewPath->ToString() );
						if ( Node->GetType() != ContentBrowser::Folder::typeid )
						{
							GPackageFileCache->FindPackageFile( *Path, NULL, Path );
						}

						// now open explorer and select the package
						ProcessStartInfo^ runExplorer = gcnew ProcessStartInfo();

						runExplorer->FileName = "explorer.exe";
						runExplorer->Arguments = "/select," + CLRTools::ToString( Path );

						EvtArgs->Handled = true;
						Process::Start(runExplorer);
						break;
					}
				}
			}
		}
		else if(Command == ContentBrowser::PackageCommands::CheckDataErrors)
		{
			EvtArgs->Handled = true;

#if !SHIPPING_PC_GAME
			GWarn->BeginSlowTask(TEXT("Checking for data error..."), TRUE);
			GWarn->MapCheck_Clear();

			TArray<FString> PackageFiles;// = GPackageFileCache->GetDataFileList();

			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ PackagesPath = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();

			ContentBrowser::SourceTreeNode^ Node = PackagesPath[0];

			FString ServerDataPth, ExlFileName, Path;
			GConfig->GetString(TEXT("Configuration"), TEXT("ServerDataPath"), ServerDataPth, GEditorIni);
			GConfig->GetString(TEXT("Configuration"), *CLRTools::ToFString(Node->Name), ExlFileName, GEditorIni);
			Path = ServerDataPth + ExlFileName;
			Path = GFileManager->ConvertToAbsolutePath(*Path);

			FString value;
			GConfig->GetString(TEXT("Excel.SaveAsExcel"), *CLRTools::ToFString(Node->Name), value, GEditorIni);

			TArray<UPackage*> Packages;
			TArray<FString> PackageNames;
			FullLoadPackages(value, Packages, PackageNames);

			FString PkgPath = "MMOAbility";
			GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString( Node->Name), PkgPath);
			INT ClassIndex = GEditor->EditorClassHierarchy->Find(PkgPath);
			if(ClassIndex == -1)
			{
				//Class not found?
			}
			else
			{
				UClass* ClickedClass = GEditor->EditorClassHierarchy->GetClass(ClassIndex);

				CheckIDError(ClickedClass, Packages);
			}

			GWarn->EndSlowTask();
			GWarn->MapCheck_ShowConditionally();
#endif
		}
		else if(Command == ContentBrowser::PackageCommands::CheckDataErrorsDeep)
		{
			EvtArgs->Handled = true;

#if !SHIPPING_PC_GAME

			GWarn->BeginSlowTask(TEXT("Checking for data error recusive..."), TRUE);
			GWarn->MapCheck_Clear();

			TArray<FString> PackageFiles;// = GPackageFileCache->GetDataFileList();

			ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ PackagesPath = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();

			ContentBrowser::SourceTreeNode^ Node = PackagesPath[0];

			FString ServerDataPth, ExlFileName, Path;
			GConfig->GetString(TEXT("Configuration"), TEXT("ServerDataPath"), ServerDataPth, GEditorIni);
			GConfig->GetString(TEXT("Configuration"), *CLRTools::ToFString(Node->Name), ExlFileName, GEditorIni);
			Path = ServerDataPth + ExlFileName;
			Path = GFileManager->ConvertToAbsolutePath(*Path);

			FString value;
			GConfig->GetString(TEXT("Excel.SaveAsExcel"), *CLRTools::ToFString(Node->Name), value, GEditorIni);

			TArray<UPackage*> Packages;
			TArray<FString> PackageNames;
			FullLoadPackages(value, Packages, PackageNames);

			FString PkgPath = "MMOAbility";
			GPackageFileCache->FindDataPathTypes(*CLRTools::ToFString( Node->Name), PkgPath);
			INT ClassIndex = GEditor->EditorClassHierarchy->Find(PkgPath);
			if(ClassIndex == -1)
			{
				//Class not found?
			}
			else
			{
				UClass* classType = GEditor->EditorClassHierarchy->GetClass(ClassIndex);
				for( TObjectIterator<UObject> It ; It ; ++It )
				{
					UObject* Obj1 = *It;

					if((Obj1 && Obj1->IsA(classType)) && (Obj1->GetOutermost() != UObject::GetTransientPackage()))
					{
						Obj1->CheckForErrors();

						for( TFieldIterator<UField> Itf(Obj1->GetClass()); Itf; ++Itf )
						{
							UField* PreviewObj = *Itf;
							if (PreviewObj)
							{
								UProperty* ObjPro = Cast<UProperty>(PreviewObj);
								if (ObjPro)
								{
									RecursiveCheckForError(ObjPro, Obj1);
								}
							}
						}
					}
				}
			}

			GWarn->EndSlowTask();
			GWarn->MapCheck_ShowConditionally();
#endif
		}
		else
		if (Command->OwnerType == ContentBrowser::PackageCommands::typeid
		||	Command == System::Windows::Input::ApplicationCommands::Save
		||	Command == ContentBrowser::PackageCommands::SaveAsset)
		{
			TArray<UPackage*> Packages;
			TArray<FString> PackageNames;
			GetSelectedRootPackages(Packages, &PackageNames, TRUE);

			ExecutePackageCommand(Sender, EvtArgs, Packages, PackageNames);
		}
		else if ( Command->OwnerType == ContentBrowser::SourceControlCommands::typeid )
		{
			TArray<UPackage*> Packages;
			TArray<FString> PackageNames;
			GetSelectedRootPackages(Packages, &PackageNames, TRUE);

			ExecuteSCCCommand(Sender, EvtArgs, Packages, PackageNames);
		}
		else if ( Command->OwnerType == ContentBrowser::ObjectFactoryCommands::typeid )
		{
			ExecuteNewObjectCommand(Sender, EvtArgs);
		}
		else if ( Command->OwnerType == ContentBrowser::CollectionCommands::typeid )
		{
			ExecuteCollectionCommand( Sender, EvtArgs );
		}


		// Done executing command
		bIsExecutingMenuCommand = false;
	}	
}

/**
 * CanExecuteRoutedEventHandler for the DataBrowser's custom commands
 * 
 * @param	Sender	the object that generated the event
 * @param	EvtArgs	details about the event that was generated
 */
void MDataBrowserControl::CanExecuteAssetCommand( System::Object^ Sender, System::Windows::Input::CanExecuteRoutedEventArgs^ EvtArgs )
{
	//no one else should ever handle this event.
	EvtArgs->Handled = true;

	Input::RoutedCommand^ Command = (Input::RoutedCommand^)EvtArgs->Command;

	// Check to see if the window is disabled.  This usually means that a modal dialog is up and
	// we don't want to be processing commands which could be reentrant
	if( !DataBrowserCtrl->IsEnabled )
	{
		EvtArgs->CanExecute = false;
	}
	else if ( Command == ContentBrowser::PackageCommands::ImportAsset )
	{
		// for now, don't allow import when clicking on assets
		EvtArgs->CanExecute = false;
	}
	else if ( Command == ContentBrowser::PackageCommands::OpenPackage )
	{
		EvtArgs->CanExecute = false;
	}
	else if (Command == ContentBrowser::PackageCommands::OpenExplorer)
	{
		EvtArgs->CanExecute = DataBrowserCtrl->AssetView->AssetListView->SelectedItems->Count > 0;
	}
	else if (Command->OwnerType == ContentBrowser::PackageCommands::typeid
	||	Command == System::Windows::Input::ApplicationCommands::Save
	||	Command == ContentBrowser::PackageCommands::SaveAsset)
	{
		TArray<UPackage*> Packages;
		TArray<FString> PackageNames;
		GetPackagesForSelectedAssetItems(Packages, PackageNames);

		OnCanExecutePackageCommand(Sender, EvtArgs, Packages, PackageNames);
	}
	else if ( Command->OwnerType == ContentBrowser::SourceControlCommands::typeid )
	{
		TArray<UPackage*> Packages;
		TArray<FString> PackageNames;
		GetPackagesForSelectedAssetItems(Packages, PackageNames);

		OnCanExecuteSCCCommand(Sender, EvtArgs, Packages, PackageNames);
	}
}

/**
 * ExecutedRoutedEventHandler for the DataBrowser's custom commands
 * 
 * @param	Sender	the object that generated the event
 * @param	EvtArgs	details about the event that was generated
 */
void MDataBrowserControl::ExecuteAssetCommand( System::Object^ Sender, System::Windows::Input::ExecutedRoutedEventArgs^ EvtArgs )
{
	//no one else should ever handle this event.
	EvtArgs->Handled = true;

	Input::RoutedCommand^ Command = (Input::RoutedCommand^)EvtArgs->Command;

	if (Command == ContentBrowser::PackageCommands::OpenExplorer)
	{
		EvtArgs->Handled = true;

		List< String^ >^ AssetPackageNames = gcnew List< String^ >();
		for each( ContentBrowser::DataAssetItem^ CurSelectedAsset in DataBrowserCtrl->AssetView->AssetListView->SelectedItems )
		{
			if ( !AssetPackageNames->Contains(CurSelectedAsset->PackageName) )
			{
				AssetPackageNames->Add(CurSelectedAsset->PackageName);
			}
		}

		for each ( String^ PackageName in AssetPackageNames )
		{
			ContentBrowser::ObjectContainerNode^ AssetPackage = DataBrowserCtrl->MySources->RootFolder->FindPackage(PackageName);
			if ( AssetPackage != nullptr /*&& !PackagesList->Contains(AssetPackage) */)
			{
				// we have a node - open the explorer window here
				// first, get the source tree path, explorer command-line requires backslashes, not forward
				Text::StringBuilder^ NodeTreeViewPath = gcnew Text::StringBuilder(CLRTools::ToString(appBaseDir()));
				NodeTreeViewPath->Append("..\\..");
				NodeTreeViewPath->Append(AssetPackage->FullTreeviewPath->Replace(ContentBrowser::SourceTreeNode::TreeViewNodeSeparator, System::IO::Path::DirectorySeparatorChar));
				NodeTreeViewPath->Append(".upk");

				ProcessStartInfo^ runExplorer = gcnew ProcessStartInfo();

				runExplorer->FileName = "explorer.exe";
				runExplorer->Arguments = "/select," + NodeTreeViewPath->ToString();

				EvtArgs->Handled = true;
				Process::Start(runExplorer);
				break;
			}
		}
	}
	else
	if (Command != ContentBrowser::PackageCommands::ImportAsset
	&&(	Command->OwnerType == ContentBrowser::PackageCommands::typeid
	||	Command == System::Windows::Input::ApplicationCommands::Save
	||	Command == ContentBrowser::PackageCommands::SaveAsset) )
	{
		TArray<UPackage*> Packages;
		TArray<FString> PackageNames;
		GetPackagesForSelectedAssetItems(Packages, PackageNames);

		ExecutePackageCommand(Sender, EvtArgs, Packages, PackageNames);
	}
	else if ( Command->OwnerType == ContentBrowser::SourceControlCommands::typeid )
	{
		TArray<UPackage*> Packages;
		TArray<FString> PackageNames;
		GetPackagesForSelectedAssetItems(Packages, PackageNames);

		ExecuteSCCCommand(Sender, EvtArgs, Packages, PackageNames);
	}
}


UObject* MDataBrowserControl::CreateFromObject( UObject* ArchetypeBase, FString& ArchetypeName, FString& PackageName, FString& GroupName )
{
	if ( ArchetypeBase == NULL )
	{
		return NULL;
	}

#if HAVE_SCC
	if(!FSourceControl::ConnectToSource())
		return NULL;


	FString Path;
	FString SaveFileName;
	FCheckoutResult sccResult;
	// if no package or name were specified, prompt the user for this info
	if ( ArchetypeName.Len() == 0 || PackageName.Len() == 0 )
	{
		WxDlgNewObject dlg;
		FString TitleName = "NewObject+";
		TitleName += ArchetypeBase->GetClass()->GetName();
		dlg.SetTitle(*TitleName);
		TArray<FString> PackageFiles;// = GPackageFileCache->GetDataFileList();

		ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ Packages = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();
		for ( INT PackageIndex = 0; PackageIndex < Packages->Count; PackageIndex++ )
		{
			ContentBrowser::SourceTreeNode^ Node = Packages[PackageIndex];
			if ( Node->GetType() == ContentBrowser::Package::typeid)
			{

				
				FString name = CLRTools::ToFString( Node->Name );
				//name.ReplaceInline( TEXT( "." ), TEXT( "_" ));
				PackageFiles.AddItem(*name);
			}
			
		}
		ContentBrowser::SourceTreeNode^ Node = Packages[0];
		Text::StringBuilder^ NodeTreeViewPath = gcnew Text::StringBuilder(CLRTools::ToString(appBaseDir()));
		NodeTreeViewPath->Append("..\\..");
		NodeTreeViewPath->Append(Node->FullTreeviewPath->Replace(ContentBrowser::SourceTreeNode::TreeViewNodeSeparator, System::IO::Path::DirectorySeparatorChar));
		Path = CLRTools::ToFString( NodeTreeViewPath->ToString() );

		SaveFileName = Path +".upk";
		sccResult = FSourceControl::TryCheckOut(SaveFileName);
		if(!sccResult.AddToSCC)
			return NULL;


		dlg.SetPackageList(PackageFiles);
		int Result = dlg.ShowModal(CLRTools::ToFString(Node->Name),GroupName,ArchetypeName);
		if ( Result == wxID_CANCEL )
			return NULL;

		ArchetypeName = dlg.GetObjectName();
		GroupName = dlg.GetGroup();
		PackageName = dlg.GetPackage();
	}

	UObject* Result = NULL;

	if ( ArchetypeName.Len() > 0 && PackageName.Len() > 0 )
	{
		FString name = PackageName;
		// create or load the package specified
		UPackage* Package = GEngine->CreatePackage(NULL,*name);
		UPackage* Group = NULL;
		if( GroupName.Len() )
		{
			// first try loading the group, in case it already exists
			Group = LoadObject<UPackage>(Package, *GroupName, NULL, LOAD_NoWarn|LOAD_FindIfFail,NULL);
			if ( Group == NULL )
			{
				Group = GEngine->CreatePackage(Package,*GroupName);
			}	
		}

		Package->SetDirtyFlag(TRUE);

		// Group will be used as the archetype's outer, so if the group is invalid, set it to the package itself
		if ( Group == NULL )
		{
			Group = Package;
		}
		// create the archetype class
		Result = ArchetypeBase->CreateArchetype(*ArchetypeName, Group);

		TScriptInterface<IIExcelable> ExcelableObj = ScriptInterfaceCast<IIExcelable>(Result);
		if(ExcelableObj)
		{
			//SetDefaultValue(Result, NULL);
			//ExcelableObj->
		}


		sccResult.TryAddToDefaultChangelist();

		SaveFileName = Path + ".upk";
		FString ErrTxt;
		FEditorFileUtils::IsFilenameValidForSaving( SaveFileName, ErrTxt );

		// Inform the content browser of the newly created archetype
		GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_ObjectCreated, Result));
	}

	return Result;

#else
	
	return NULL;

#endif // #if HAVE_SCC
}

UObject* MDataBrowserControl::CreatePackage( FString& ObjType, FString& FolderName, FString& GroupName, FString& PackageName )
{
	if (ObjType.Len() == 0)
	{
		return NULL;
	}

#if HAVE_SCC

	if(!FSourceControl::ConnectToSource())
		return NULL;

	FString Path;
	// if no package or name were specified, prompt the user for this info
	if ( FolderName.Len() == 0 || PackageName.Len() == 0 )
	{
		WxDlgNewPackageGroupName dlg;
		TArray<FString> PackageFiles;// = GPackageFileCache->GetDataFileList();

		ReadOnlyCollection<ContentBrowser::SourceTreeNode^>^ Packages = DataBrowserCtrl->MyPackagePanel->MakeSelectedNodeList();
		for ( INT PackageIndex = 1; PackageIndex < Packages->Count; PackageIndex++ )
		{
			ContentBrowser::SourceTreeNode^ Node = Packages[PackageIndex];
			if ( Node->GetType() == ContentBrowser::Folder::typeid)
			{


				FString name = CLRTools::ToFString( Node->Name );
				//name.ReplaceInline( TEXT( "." ), TEXT( "_" ));
				PackageFiles.AddItem(*name);
			}

		}
		ContentBrowser::SourceTreeNode^ Node = Packages[0];
		Text::StringBuilder^ NodeTreeViewPath = gcnew Text::StringBuilder(CLRTools::ToString(appBaseDir()));
		NodeTreeViewPath->Append("..\\..");
		NodeTreeViewPath->Append(Node->FullTreeviewPath->Replace(ContentBrowser::SourceTreeNode::TreeViewNodeSeparator, System::IO::Path::DirectorySeparatorChar));
		Path = CLRTools::ToFString( NodeTreeViewPath->ToString() );



		dlg.SetPackageList(PackageFiles);
		int Result = dlg.ShowModal(FolderName,GroupName,PackageName);
		if ( Result == wxID_CANCEL )
			return NULL;

		PackageName = dlg.GetObjectName();
		GroupName = dlg.GetGroup();
		FolderName = dlg.GetPackage();
	}

	UObject* Result = NULL;

	if ( FolderName.Len() > 0 && PackageName.Len() > 0 )
	{
		FString name = PackageName;
		// create or load the package specified
		UPackage* Package = GEngine->CreatePackage(NULL,*name);
		UPackage* Group = NULL;
		if( GroupName.Len() )
		{
			// first try loading the group, in case it already exists
			Group = LoadObject<UPackage>(Package, *GroupName, NULL, LOAD_NoWarn|LOAD_FindIfFail,NULL);
			if ( Group == NULL )
			{
				Group = GEngine->CreatePackage(Package,*GroupName);
			}	
		}

		Package->SetDirtyFlag(TRUE);

		// Group will be used as the archetype's outer, so if the group is invalid, set it to the package itself
		if ( Group == NULL )
		{
			Group = Package;
		}

// 		// create the archetype class
// 		Result = ArchetypeBase->CreateArchetype(*ArchetypeName, Group);

		FString SaveFileName = Path + "\\"+  FolderName+"\\"+PackageName +".upk";


		FString ErrTxt;
		FEditorFileUtils::IsFilenameValidForSaving( SaveFileName, ErrTxt );

		GWarn->StatusUpdatef( 0, 1, *FString::Printf( LocalizeSecure(LocalizeUnrealEd("SavingPackagef"), *PackageName )) );
		const FScopedBusyCursor BusyCursor;
		debugf(TEXT("saving as %s"), *SaveFileName);
		if( GFileManager->IsReadOnly( *SaveFileName ) )
		{
			appMsgf( AMT_OK, *FString::Printf( LocalizeSecure(LocalizeUnrealEd("Error_CouldntWriteToFile_F"), *SaveFileName)) );
		}
		else if( !GUnrealEd->Exec( *FString::Printf(TEXT("OBJ SAVEPACKAGE PACKAGE=\"%s\" FILE=\"%s\""), *PackageName, *SaveFileName) ) )
		{
			// Couldn't save.
			appMsgf( AMT_OK, *LocalizeUnrealEd("Error_CouldntSavePackage") );
		}
		else
		{
		}
		FCheckoutResult sccResult = FSourceControl::TryCheckOut(SaveFileName);

		FString SaveFilePath = Path + "\\"+  FolderName;
		GPackageFileCache->CacheDataPath(*SaveFilePath, *ObjType);
		FullyLoadPackage(CLRTools::ToString(SaveFileName));

		sccResult.TryAddToDefaultChangelist();

		GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_UpdatePackageListUI));
	}

	return Result;

#else

	return NULL;

#endif // #if HAVE_SCC

}
UBOOL MDataBrowserControl::ShouldSerializeValue( QWORD PropertyFlags)
{
	UBOOL Skip = FALSE;
	if( (PropertyFlags & (CPF_Native|CPF_Transient|CPF_DuplicateTransient|CPF_ArchetypeProperty|CPF_NonTransactional|CPF_Deprecated)) != 0 )
	{
		Skip =	((PropertyFlags & CPF_Native) != 0)
			||	((PropertyFlags & CPF_Transient) != 0 )
			||	((PropertyFlags & CPF_DuplicateTransient) != 0 )
			||	((PropertyFlags & CPF_ArchetypeProperty) != 0)
			||	((PropertyFlags & CPF_Deprecated) != 0 );
	}
	return !Skip;
}



void MDataBrowserControl::ExportProperty(BYTE*Data, UProperty* ObjPro, PropertyNode& ParentNode, int nIndex)//TArray<FString> &Item, int nIndex, UObject* Obj)
{
	for(;ParentNode.ChildNode.Num() <= nIndex;)
	{
		ParentNode.ChildNode.AddItem(PropertyNode());
	}

	if( !(ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UArrayProperty)
		&& !(ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UStructProperty) )
	{
		//debugf(TEXT("	 [%d] %d ExportProperty:%s"), g_debugIndex, nIndex, *ObjPro->GetNameCPP());
	}

	UPropertyValue pv;
	ObjPro->GetPropertyValue(Data, pv);
	if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UStrProperty )
	{
		ParentNode.ChildNode(nIndex).StringValue = *pv.StringValue;
		ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
		ParentNode.ChildNode(nIndex).Data = Data;
		ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
		ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex;
		++g_debugIndex;
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UNameProperty )
	{
		ParentNode.ChildNode(nIndex).StringValue = *pv.NameValue->ToString();
		ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
		ParentNode.ChildNode(nIndex).Data = Data;
		ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
		ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex;
		++g_debugIndex;
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UIntProperty )
	{
		ParentNode.ChildNode(nIndex).StringValue = *FString::Printf(TEXT("%d"), pv.IntValue);
		ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
		ParentNode.ChildNode(nIndex).Data = Data;
		ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
		ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex;
		++g_debugIndex;
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UFloatProperty )
	{
		ParentNode.ChildNode(nIndex).StringValue = *FString::Printf(TEXT("%f"), pv.FloatValue);
		ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
		ParentNode.ChildNode(nIndex).Data = Data;
		FString value = ObjPro->GetMetaData(TEXT("Convert"));
		if(value.Len() > 0)
		{
			int nData = appAtoi(*value);
			int nValue = pv.FloatValue * nData;
			ParentNode.ChildNode(nIndex).StringValue = *FString::Printf(TEXT("%d"), nValue);
		}

		ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
		ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex;
		++g_debugIndex;
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UByteProperty )
	{
		ParentNode.ChildNode(nIndex).StringValue = *FString::Printf(TEXT("%d"), pv.ByteValue);
		ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
		ParentNode.ChildNode(nIndex).Data = Data;
		ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
		ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex;
		++g_debugIndex;
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UBoolProperty )
	{
		ParentNode.ChildNode(nIndex).StringValue = *FString::Printf(TEXT("%d"), pv.BoolValue);
		ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
		ParentNode.ChildNode(nIndex).Data = Data;
		ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
		ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex;
		++g_debugIndex;
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UArrayProperty )
	{
		UArrayProperty* ArrayProp = (UArrayProperty*)ObjPro;
		for (INT ArrayIndex = 0; ArrayIndex < pv.ArrayValue->Num(); ArrayIndex++)
		{
			ExportProperty(((BYTE*)pv.ArrayValue->GetData())+ArrayIndex*ArrayProp->Inner->ElementSize, ArrayProp->Inner, ParentNode.ChildNode(nIndex), ArrayIndex);//Item, nIndex+ArrayIndex,Obj);
		}
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UStructProperty )
	{
		if(((UStructProperty*)ObjPro)->Struct == NULL)
		{
			return;
		}
		UProperty* NextProperty = ((UStructProperty*)ObjPro)->Struct->PropertyLink;
		if(ObjPro->GetMetaData(TEXT("Merger")).Len() > 0)
		{
			int nMerger = 0;
			for (; NextProperty != NULL; NextProperty = NextProperty->PropertyLinkNext)
			{
				for(INT i = 0; i < NextProperty->ArrayDim; ++i)
				{
					if (!ShouldSerializeValue(NextProperty->GetPropertyFlags()))
					{
						continue;
					}
					FString MetaData = NextProperty->GetMetaData(TEXT("ExportExcel"));
					if(MetaData.Len() > 0)
					{
						INT nChildIndex = appAtoi(*MetaData);
						UPropertyValue pvBool;
						NextProperty->GetPropertyValue(Data+NextProperty->Offset + NextProperty->ElementSize*i, pvBool);
						if( NextProperty->GetClass()->ClassCastFlags & CASTCLASS_UBoolProperty )
						{
							nMerger += pvBool.BoolValue << nChildIndex;
						}
					}
				}
			}
			ParentNode.ChildNode(nIndex).StringValue = *FString::Printf(TEXT("%d"), nMerger);
			ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
			ParentNode.ChildNode(nIndex).Data = Data;

			ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
			ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex;
			++g_debugIndex;
		}
		else
		{
			for (; NextProperty != NULL; NextProperty = NextProperty->PropertyLinkNext)
			{
				for(INT i = 0; i < NextProperty->ArrayDim; ++i)
				{
					if (!ShouldSerializeValue(NextProperty->GetPropertyFlags()))
					{
						continue;
					}
					FString MetaData = NextProperty->GetMetaData(TEXT("ExportExcel"));
					if(MetaData.Len() > 0)
					{
						INT nChildIndex = appAtoi(*MetaData);
						ExportProperty(Data+NextProperty->Offset + NextProperty->ElementSize*i, NextProperty, ParentNode.ChildNode(nIndex), nChildIndex);//Item, nIndex++, Obj);
					}
				}
			}
		}
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UClassProperty )
	{		
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UObjectProperty )
	{
		UObject* ret = pv.ObjectValue;
		if(!ret)
		{
			return;
		}
		TScriptInterface<IIReferenceID> ReferenceObj = ScriptInterfaceCast<IIReferenceID>(ret);
		if(ReferenceObj)
		{
			ParentNode.ChildNode(nIndex).StringValue = *FString::Printf(TEXT("%d"), ReferenceObj->GetTemplateID());			
		}
		else
		{
			ParentNode.ChildNode(nIndex).StringValue = ret->GetFullName();
		}


		ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
		ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex;
		++g_debugIndex;
		//ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
		//ParentNode.ChildNode(nIndex).Data = Data;
	}
	else
	{
		ParentNode.ChildNode(nIndex).StringValue = *ObjPro->GetFName().ToString();

		ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
		ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex;
		++g_debugIndex;
		//ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
		//ParentNode.ChildNode(nIndex).Data = Data;
	}

}

void MDataBrowserControl::ExportPropertyToTextArray(PropertyNode &ParentNode, TArray<FString>& Items)
{
	if(ParentNode.ChildNode.Num() > 0)
	{
		for( INT i = 0; i < ParentNode.ChildNode.Num(); ++i )
		{
			ExportPropertyToTextArray(ParentNode.ChildNode(i), Items);
		}
	}
	else
	{
// 		debugf(TEXT("	-----%d g[%d] %s Value:%s"), 
// 			Items.Num(), ParentNode.DebugIndex, *ParentNode.DebugString,  
// 			*ParentNode.StringValue);
		Items.AddItem(ParentNode.StringValue);
	}
}

UBOOL MDataBrowserControl::ExportExcelHeader( const TCHAR* Section, TArray<FString>& Header, TArray<FString>& DefaultValue, const TCHAR* Filename )
{
	Header.Empty();
	DefaultValue.Empty();
	FConfigFile* File = GConfig->Find( Filename, 0 );
	if( !File )
		return 0;
	FConfigSection* Sec = File->Find( Section );
	if( !Sec )
		return 0;
	for( FConfigSection::TIterator It(*Sec); It; ++It )
	{
		new(Header) FString(FString::Printf( TEXT("%s"), *It.Key().ToString()));
		new(DefaultValue) FString(FString::Printf( TEXT("%s"), *It.Value() ));
	}
	return 1;
}

void MDataBrowserControl::ImportProperty(PropertyNode& ParentNode, int& nHeadIndex, TArray<FString>& DefaultValue)
{
	if(ParentNode.ChildNode.Num() > 0)
	{
		for( INT i = 0; i < ParentNode.ChildNode.Num(); ++i )
		{
			ImportProperty(ParentNode.ChildNode(i), nHeadIndex, DefaultValue);
		}

	}
	else
	{
		UProperty* ObjPro = ParentNode.ObjPro;
		BYTE* Data		= ParentNode.Data;
		if(!ObjPro || !Data)
		{
			nHeadIndex++;
			return;
		}
		UPropertyValue pv;
		ObjPro->GetPropertyValue(Data, pv);
		if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UStrProperty )
		{
			pv.StringValue = &DefaultValue(nHeadIndex);
			ObjPro->SetPropertyValue(Data, pv);
			nHeadIndex++;
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UNameProperty )
		{
			pv.NameValue = new FName(*DefaultValue(nHeadIndex));
			ObjPro->SetPropertyValue(Data, pv);
			nHeadIndex++;
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UIntProperty )
		{
			pv.IntValue = appAtoi(*DefaultValue(nHeadIndex));
			ObjPro->SetPropertyValue(Data, pv);
			nHeadIndex++;
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UFloatProperty )
		{
#if	0
			if(ObjPro->GetName() == TEXT("BaseStiffTime"))
			{
					debugf(TEXT("Debug Break"));
					for (INT j = 0; j < DefaultValue.Num(); ++j)
					{
						debugf(TEXT("\t%d=%s"), j, *DefaultValue(j));
					}
			}

			debugf(TEXT("importing float property:%s %s"), *ObjPro->GetName(), *ObjPro->GetNameCPP());

#endif

			FString value = ObjPro->GetMetaData(TEXT("Convert"));
			if(value.Len() > 0)
			{
				FLOAT nData = appAtof(*value);
				FString strVal = DefaultValue(nHeadIndex);
				pv.FloatValue = appAtof(*strVal);
				pv.FloatValue /= nData; 
			}
			else
			{
				pv.FloatValue = appAtof(*DefaultValue(nHeadIndex));
			}

			ObjPro->SetPropertyValue(Data, pv);
			nHeadIndex++;
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UByteProperty )
		{
			int nValue = appAtoi(*DefaultValue(nHeadIndex));
			if(nValue < 0)
				pv.ByteValue = 0;
			else
				pv.ByteValue = nValue;
			ObjPro->SetPropertyValue(Data, pv);
			nHeadIndex++;
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UBoolProperty )
		{
			pv.BoolValue = appAtoi(*DefaultValue(nHeadIndex));
			ObjPro->SetPropertyValue(Data, pv);
			nHeadIndex++;
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UArrayProperty )
		{
		//	UArrayProperty* ArrayProp = (UArrayProperty*)ObjPro;
		//	for (INT ArrayIndex = 0; ArrayIndex < pv.ArrayValue->Num(); ArrayIndex++)
		//	{
		////		ExportProperty2(ParentNode.ChildNode(nIndex), nHeadIndex, DefaultValue);//Item, nIndex+ArrayIndex,Obj);
		//	}
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UStructProperty )
		{
			if(((UStructProperty*)ObjPro)->Struct == NULL)
			{
				return;
			}
			UProperty* NextProperty = ((UStructProperty*)ObjPro)->Struct->PropertyLink;
			if(ObjPro->GetMetaData(TEXT("Merger")).Len() > 0)
			{
				int nMerger = appAtoi(*DefaultValue(nHeadIndex));;
				for (; NextProperty != NULL; NextProperty = NextProperty->PropertyLinkNext)
				{
					for(INT i = 0; i < NextProperty->ArrayDim; ++i)
					{
						if (!ShouldSerializeValue(NextProperty->GetPropertyFlags()))
						{
							continue;
						}
						FString MetaData = NextProperty->GetMetaData(TEXT("ExportExcel"));
						if(MetaData.Len() > 0)
						{
							INT nChildIndex = appAtoi(*MetaData);
							UPropertyValue pvBool;
							int n = 1;
							pvBool.BoolValue = nMerger&(n << nChildIndex );
							NextProperty->SetPropertyValue(Data+NextProperty->Offset + NextProperty->ElementSize*i, pvBool);
						}
					}
				}
				nHeadIndex++;
			}
			else
			{
				for (; NextProperty != NULL; NextProperty = NextProperty->PropertyLinkNext)
				{
					for(INT i = 0; i < NextProperty->ArrayDim; ++i)
					{
						if (!ShouldSerializeValue(NextProperty->GetPropertyFlags()))
						{
							continue;
						}
						FString MetaData = NextProperty->GetMetaData(TEXT("ExportExcel"));
						if(MetaData.Len() > 0)
						{
							INT nChildIndex = appAtoi(*MetaData);
							//ExportProperty2(ParentNode.ChildNode(nIndex), nHeadIndex, DefaultValue);//Item, nIndex++, Obj);
						}
					}
				}
			}
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UClassProperty )
		{
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UObjectProperty )
		{
			UObject* ret = pv.ObjectValue;
			if(!ret)
			{
				return;
			}
			TScriptInterface<IIReferenceID> ReferenceObj = ScriptInterfaceCast<IIReferenceID>(ret);
			if(ReferenceObj)
			{
				nHeadIndex++;
				return;
			}
			else
			{
				//pv.ObjectValue = UObject::StaticFindObject() (*DefaultValue(nHeadIndex));
				//ObjPro->SetPropertyValue(Data, pv);
				nHeadIndex++;
			}
		}
		else
		{
			nHeadIndex++;
		}

	}


}

void MDataBrowserControl::SetDefaultValue(UObject *Obj, const TCHAR* Section)
{
	FString KeyValue;
	GConfig->GetString(TEXT("Excel.Type"), *Obj->GetClass()->GetName(), KeyValue, GEditorIni);
	
	TArray<FString> Headers, DefaultValues;
	if(ExportExcelHeader(*FString::Printf(TEXT("Excel.%s"),*KeyValue), Headers, DefaultValues, GEditorIni ))
	{
		FString MetaData;
		UProperty* ObjPro;
		PropertyNode ParentNode;
		int nHeadIndex = 1;
		for( TFieldIterator<UField> It(Obj->GetClass()); It; ++It )
		{
			UField* PreviewObj = *It;
			if (PreviewObj)
			{
				ObjPro = Cast<UProperty>(PreviewObj);
				if (ObjPro)
				{
					MetaData = ObjPro->GetMetaData(TEXT("ExportExcel"));
					if(MetaData.Len() > 0)
					{
						BYTE* Data = (BYTE*)Obj + ObjPro->Offset;
						int nIndex = appAtoi(*MetaData);
						//Used for sorting
						ExportProperty(Data, ObjPro, ParentNode, nIndex);
					}
				}
			}
		}
		//Import default values
		ImportProperty(ParentNode, nHeadIndex, DefaultValues);

	}

}


void MDataBrowserControl::SaveAsExcel(const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames, const FString& Path, int nClassType, const FString& ExcelName)
{
#if HAVE_SCC

	GWarn->BeginSlowTask(TEXT("Exporting to Excel"), TRUE);
	GWarn->MapCheck_Clear();

	//List<List<String^>^> ^ItemList = gcnew List<List<String^>^>();
	String ^ExcelPath = CLRTools::ToString(Path);
	
	TArray< UObject* > ObjectsInPackages;
	PackageTools::GetObjectsInPackages(&Packages, NULL, ObjectsInPackages);

	TArray< TArray<FString> > data;
	extern void ExportToExcel(const TArray< UObject* >& ObjectsInPackages, TArray< TArray<FString> >& data, const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames, const FString& Path, int nClassType, const FString& ExcelName);
	ExportToExcel(ObjectsInPackages, data, Packages, PackageNames, Path, nClassType, ExcelName);


	array<String^, 2>^ ItemList = CLRTools::ToStringArray2D(data);

	INT idx = ExcelPath->LastIndexOf(".");
	String^ txtFile = ExcelPath->Substring(0, idx);
	txtFile += ".txt";
	FCheckoutResult sccResultTxt = FSourceControl::TryCheckOut(CLRTools::ToFString(txtFile));
	FCheckoutResult sccResult = FSourceControl::TryCheckOut(Path);

	if(sccResult.AddToSCC)
	{
		UnrealEdCSharp::Excel::ExcelUtil^ excelUtil = gcnew UnrealEdCSharp::Excel::ExcelUtil();
		excelUtil->SaveAsExcel(ItemList, ExcelPath);
		sccResult.TryAddToDefaultChangelist();
		//ImportFromExcel(Packages, PackageNames, Path, nClassType);	
	}

	UClass* ClickedClass;
	ClickedClass = GEditor->EditorClassHierarchy->GetClass(nClassType);
	CheckIDError(ClickedClass, Packages);

	GWarn->EndSlowTask();
	GWarn->MapCheck_ShowConditionally();


	if(sccResultTxt.AddToSCC)
	{
		sccResultTxt.TryAddToDefaultChangelist();
		//UGamePlayDataManager::GetSingleton()->SavePackage();
	}
#endif // HAVE_SCC
}

void MDataBrowserControl::CLRAnalyzeReference(FString FolderName, TArray<FString>& PackageNames)
{
	extern void AnalyzeReference(FString FolderName, TArray<FString>& PackageNames);
	AnalyzeReference(FolderName, PackageNames);
}

void MDataBrowserControl::ImportFromExcel(const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames,
	const FString& Path, int nClassType, UClass* classType)
{
#if HAVE_SCC
	GWarn->BeginSlowTask(TEXT("Import from Excel"), TRUE);
	GWarn->MapCheck_Clear();

	TArray<FString> loadList;
	GConfig->GetArray(TEXT("Excel.ImportLoadingDir"), TEXT("LoadingDir"), loadList, GEditorIni);

	TArray<UPackage*> tmpPackages;
	TArray<FString> tmpPackageNames;
	for(INT i = 0; i < loadList.Num(); ++i)
	{
		FullLoadPackages(loadList(i), tmpPackages, tmpPackageNames);
		GWarn->StatusUpdatef(i, loadList.Num(),*FString::Printf(TEXT("Loading all packages...")));
	}

	String^ ExcelPath = CLRTools::ToString(Path);
	UnrealEdCSharp::Excel::ExcelUtil^ excelUtil = gcnew UnrealEdCSharp::Excel::ExcelUtil();

	List< List<String^>^ > ^ ExcelData = nullptr;

	excelUtil->LoadFromExcel(ExcelPath, ExcelData);

	TArray< TArray<FString> > dataSource;
	CLRTools::ToFStringArray2D(ExcelData, dataSource);

	TMap<FString, UObject* > ObjectsInPackages;
	//for( INT PackageIndex = 0; PackageIndex < Packages.Num(); PackageIndex++ )
	//{
	//	UPackage* Package = Packages(PackageIndex);
	//	if(Package && !Package->IsFullyLoaded())
	//		Package->FullyLoad();
	//}
	PackageTools::GetObjectsInPackages(&Packages, NULL, ObjectsInPackages);

	extern void ImportFromExcel(TArray< TArray<FString> >& dataSource, TMap<FString, UObject* >& ObjectsInPackages,
		const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames, const FString& Path, UClass* classType);
	
	ImportFromExcel(dataSource, ObjectsInPackages, Packages, PackageNames, Path, classType);

	CheckIDError(classType, Packages);

	GWarn->EndSlowTask();
	GWarn->MapCheck_ShowConditionally();

	GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_UpdatePackageListUI));
#endif // HAV_SCC
}

void MDataBrowserControl::ExportTable(const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames, const FString& Path, int nClassType)
{
#if HAVE_SCC
	GWarn->BeginSlowTask(TEXT("Exporting to Excel..."), TRUE);
	GWarn->MapCheck_Clear();

	String ^ExcelPath = CLRTools::ToString(Path);

	int nIndex = 1;
	TArray< UObject* > ObjectsInPackages;

	PackageTools::GetObjectsInPackages(&Packages, NULL, ObjectsInPackages);
	array<String^, 2>^ ItemList;

	TArray< UAI_Affix_Table* > exportingObjs;

	for(int i = 0; i < ObjectsInPackages.Num(); ++i)
	{
		UObject* Obj = ObjectsInPackages(i);
		if(!Obj || !Obj->IsA(UAI_Affix_Table::StaticClass()))
		{
			continue;
		}

		UAI_Affix_Table* ExcelableObj = Cast<UAI_Affix_Table>(Obj);
		exportingObjs.AddItem(ExcelableObj);
	}

	INT curStep = 1;
	INT TotalSteps = exportingObjs.Num();

	GWarn->StatusUpdatef(curStep, TotalSteps, *FString::Printf(TEXT("Exporting to Excel...")));
	
	for(int i = 0; i < TotalSteps; ++i)
	{
		bool HeaderExported = false;
		
		UAI_Affix_Table* ExcelableObj = exportingObjs(i);

		// If the table header has not been exported yet, export it first
		if(!HeaderExported)
		{
			TArray<FString> Headers, DefaultValues;
			ExcelableObj->ExportExcelHeaders(Headers, DefaultValues);
			ItemList = gcnew array<String^, 2>(ExcelableObj->GetRowCount() + 1, Headers.Num());
			if(Headers.Num() > 0)
			{
				//ItemList->Add(CLRTools::ToStringArray(Headers));
				for (INT i = 0; i < Headers.Num(); ++i)
				{
					ItemList[0, i] = CLRTools::ToString(Headers(i));
				}

				//ItemList->Add(CLRTools::ToStringArray(DefaultValues));
			}

			HeaderExported = true;
		}
		
		for(int i = 0; i < ExcelableObj->GetRowCount(); ++i)
		{
			TArray<FString> Item;
			ExcelableObj->ExportRowData(Item, i);
			if(Item.Num() > 0)
			{					
				//ItemList->Add(CLRTools::ToStringArray(Item));
				for (INT j = 0; j < Item.Num(); ++j)
				{
					INT  idx = i + 1;
					ItemList[idx, j] = CLRTools::ToString(Item(j));
				}
			}
		}

		//int idx = ExcelPath->LastIndexOf(".");
		//String^ txtFile;// = ExcelPath->Substring(0, idx);
		String^ txtFile = ExcelPath;
		txtFile += CLRTools::ToString(ExcelableObj->GetTableName());
		txtFile += ".txt";
		FString PathFile = Path + ExcelableObj->GetTableName() + ".xlsx";
		String^ ExcelPathFile = CLRTools::ToString(PathFile);
		FCheckoutResult sccResultTxt = FSourceControl::TryCheckOut(CLRTools::ToFString(txtFile));
		FCheckoutResult sccResult = FSourceControl::TryCheckOut(PathFile);
		if(sccResult.AddToSCC)
		{
			UnrealEdCSharp::Excel::ExcelUtil^ excelUtil = gcnew UnrealEdCSharp::Excel::ExcelUtil();
			excelUtil->SaveAsExcel(ItemList, ExcelPathFile);

			sccResult.TryAddToDefaultChangelist();
			sccResultTxt.TryAddToDefaultChangelist();
		}

		GWarn->UpdateProgress( ++curStep, TotalSteps );
	}


	GWarn->EndSlowTask();
	GWarn->MapCheck_ShowConditionally();

	//DataBrowserCtrl->MyPackagePanel->SaveAsExcel(ItemList, ExcelPath);
#endif HAVE_SCC

}
void MDataBrowserControl::ImportTable(const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames, const FString& Path, int nClassType)
{
#if HAVE_SCC
	GWarn->BeginSlowTask(TEXT("Exporting to Excel"), TRUE);
	GWarn->MapCheck_Clear();

	String ^ExcelPath = CLRTools::ToString(Path);

	int nIndex = 1;
	TArray< UObject* > ObjectsInPackages;

	PackageTools::GetObjectsInPackages(&Packages, NULL, ObjectsInPackages);
	for(int i = 0; i < ObjectsInPackages.Num(); ++i)
	{
		UObject* Obj = ObjectsInPackages(i);
		if(!Obj || !Obj->IsA(UAI_Affix_Table::StaticClass()))
		{
			continue;
		}
		UAI_Affix_Table* ExcelableObj = Cast<UAI_Affix_Table>(Obj);
		
		UnrealEdCSharp::Excel::ExcelUtil^ excelUtil = gcnew UnrealEdCSharp::Excel::ExcelUtil();

		List< List<String^>^ > ^ ExcelData = nullptr;

		FString PathFile = Path + ExcelableObj->GetTableName() + ".xlsx";
		String^ ExcelPathFile = CLRTools::ToString(PathFile);

		excelUtil->LoadFromExcel(ExcelPathFile, ExcelData);

		for(int i = 0; i < (*ExcelData).Count; ++i)
		{
			TArray<FString> StrArr;
			CLRTools::ToFStringArray((*ExcelData)[i], StrArr);
			ExcelableObj->ImportRowData(StrArr);
		}
	}


	GWarn->EndSlowTask();
	GWarn->MapCheck_ShowConditionally();

	//DataBrowserCtrl->MyPackagePanel->SaveAsExcel(ItemList, ExcelPath);
#endif HAVE_SCC

}

void MDataBrowserControl::SaveModifier(const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames, const FString& Path, int nClassType)
{
	List<List<String^>^> ^ItemList = gcnew List<List<String^>^>();
	String ^ExcelPath = CLRTools::ToString(Path);

	UClass* ClickedClass;
	ClickedClass = GEditor->EditorClassHierarchy->GetClass(nClassType);

	bool bDataError = false;
	const FString CheckMapLocString(LocalizeUnrealEd(TEXT("CheckingMap")));
	GWarn->BeginSlowTask( *CheckMapLocString, FALSE);
	GWarn->MapCheck_BeginBulkAdd();
	GWarn->MapCheck_Clear();
	
	TArray<USeqAct_Modifier*> ModifierList;
	TArray< UObject* > ObjectsInPackages;
	PackageTools::GetObjectsInPackages(&Packages, NULL, ObjectsInPackages);

	//for(int i = 0; i < ObjectsInPackages.Num(); ++i)
	//{
	//	UObject* Obj = ObjectsInPackages(i);//Package->GetNetObjectAtIndex(i);
	//	if(Obj && Obj->IsA(USeqAct_Modifier::StaticClass()))
	//	{
	//		USeqAct_Modifier *Modifier = (USeqAct_Modifier *)Obj;

	//		const FString PackageName = Obj->GetOutermost()->GetName();
	//		UPackage* ResultPackage = FindObject<UPackage>(NULL, *PackageName);
	//		UObject* NewObject = GEngine->StaticConstructObject( USeqAct_Modifier1::StaticClass(), ResultPackage, NAME_None, RF_PerObjectLocalized);
	//		//UObject* DupObject = GEditor->StaticDuplicateObject( Obj, Obj, ResultPackage, *FString::Printf(TEXT("%s_1"), *Obj->GetName() ));
	//	}
	//}
	//return;


	for(int i = 0; i < ObjectsInPackages.Num(); ++i)
	{
		UObject* Obj = ObjectsInPackages(i);//Package->GetNetObjectAtIndex(i);
		if(Obj && Obj->IsA(USeqAct_Modifier::StaticClass()))
		{
			USeqAct_Modifier *Modifier = (USeqAct_Modifier *)Obj;
			if(Modifier->Modifiers.Num())
			{
				for(int i = 0; i < Modifier->Modifiers.Num(); ++i)
				{
					if(!Modifier->Modifiers(i))
					{
						bDataError = true;
						GWarn->MapCheck_Add( MCTYPE_ERROR, Obj, *FString::Printf(
							LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_ModifierNodeNull")),
							*Modifier->GetPathName(), i)), TEXT( "SeqAct_Modifier Checker" ) );
						continue;
					}

					Modifier->Modifiers(i)->Id = ModifierList.Num();

					if(Modifier->Modifiers(i)->Param1.ValueStr.IsEmpty())
					{
						Modifier->Modifiers(i)->Param1.ValueStr = "0";
						//if(!Modifier->Modifiers(i)->Param1.ValueX.IsEmpty())
						//	Modifier->Modifiers(i)->Param1.ValueStr = Modifier->Modifiers(i)->Param1.ValueX;
						//else
						//{
						//	TCHAR buff[64] = { 0 };
						//	_stprintf(buff, TEXT("%d"), Modifier->Modifiers(i)->Param1.Value1);
						//	Modifier->Modifiers(i)->Param1.ValueStr = buff;
						//}
					}

					if(Modifier->Modifiers(i)->Param2.ValueStr.IsEmpty())
					{
						Modifier->Modifiers(i)->Param2.ValueStr = "0";
						//if(!Modifier->Modifiers(i)->Param2.ValueX.IsEmpty())
						//	Modifier->Modifiers(i)->Param2.ValueStr = Modifier->Modifiers(i)->Param2.ValueX;
						//else
						//{
						//	TCHAR buff[64] = { 0 };
						//	_stprintf(buff, TEXT("%d"), Modifier->Modifiers(i)->Param2.Value1);
						//	Modifier->Modifiers(i)->Param2.ValueStr = buff;
						//}
					}

					if(Modifier->Modifiers(i)->Param3.ValueStr.IsEmpty())
					{
						Modifier->Modifiers(i)->Param3.ValueStr = "0";
						//if(!Modifier->Modifiers(i)->Param3.ValueX.IsEmpty())
						//	Modifier->Modifiers(i)->Param3.ValueStr = Modifier->Modifiers(i)->Param3.ValueX;
						//else
						//{
						//	TCHAR buff[64] = { 0 };
						//	_stprintf(buff, TEXT("%d"), Modifier->Modifiers(i)->Param3.Value1);
						//	Modifier->Modifiers(i)->Param3.ValueStr = buff;
						//}
					}
					
					ModifierList.AddItem(Modifier->Modifiers(i));
					if((Modifier->Modifiers(i)->OutputLinks(0).Links.Num() > 0)
						!= (Modifier->Modifiers(i)->TrueRetNode != nullptr))
					{
						bDataError = true;
						GWarn->MapCheck_Add( MCTYPE_WARNING, Obj, *FString::Printf(TEXT("%s TrueRetNode Error!"), *Modifier->Modifiers(i)->NodeName), TEXT( "SeqAct_Modifier Checker" ) );
					}
					if((Modifier->Modifiers(i)->OutputLinks(1).Links.Num() > 0)
						!= (Modifier->Modifiers(i)->NextNode != nullptr))
					{
						bDataError = true;
						GWarn->MapCheck_Add( MCTYPE_WARNING, Obj, *FString::Printf(TEXT("%s NextNode Error!"), *Modifier->Modifiers(i)->NodeName), TEXT( "SeqAct_Modifier Checker" ) );

					}
					if((Modifier->Modifiers(i)->OutputLinks(2).Links.Num() > 0)
						!= (Modifier->Modifiers(i)->FalseRetNode != nullptr))
					{
						bDataError = true;
						GWarn->MapCheck_Add( MCTYPE_WARNING, Obj, *FString::Printf(TEXT("%s FalseRetNode Error!"), *Modifier->Modifiers(i)->NodeName), TEXT( "SeqAct_Modifier Checker" ) );
					}
				}
			}
		}
	}




	GWarn->MapCheck_EndBulkAdd();
	GWarn->EndSlowTask();
	GWarn->MapCheck_ShowConditionally();
	if(bDataError)
	{
		return;
	}
	MODIFIER_FILE_HEADER ModifierHead;
	ModifierHead.dwTag		= MODIFIER_FILE_TAG;
	ModifierHead.dwVersion	= MODIFIER_FILE_VERSION;
	ModifierHead.nBlockCount= ModifierList.Num()+1;

	MODIFIER_FILE_NODE_NEW* ModifierNodes = new MODIFIER_FILE_NODE_NEW[ModifierList.Num()+1];
	memset(ModifierNodes, 0, sizeof(MODIFIER_FILE_NODE_NEW)*(ModifierList.Num()+1));

	for(int i = 0; i < ModifierList.Num(); ++i)
	{
		USeqAct_Modifier* Modifier = ModifierList(i);
		Modifier->Id = i+1;
		if(Modifier->bRoot && Modifier->Parent)
			Modifier->Parent->Id = Modifier->Id; 
	}
	for(int i = 0; i < ModifierList.Num(); ++i)
	{
		int Id = i+1;
		USeqAct_Modifier* Modifier = ModifierList(i);
		ModifierNodes[Id].nType = Modifier->Type;
		ModifierNodes[Id].nKey = Modifier->Key;

		if(Modifier->ParentNode)
			ModifierNodes[Id].nParentNode = Modifier->ParentNode->Id;

		if(Modifier->TrueRetNode)
			ModifierNodes[Id].nTrueRetNode = Modifier->TrueRetNode->Id;

		if(Modifier->FalseRetNode)
			ModifierNodes[Id].nFalseRetNode = Modifier->FalseRetNode->Id;

		if(Modifier->NextNode)
			ModifierNodes[Id].nNextNode	= Modifier->NextNode->Id;
		
#ifdef _DEBUG
		if(Modifier->BreakOnExport)
		{
			appMsgf(AMT_OK, TEXT("Breakpoint debugging of Modifier data was triggered during export. Please attach the Visual Studio debugger, continue and analyse: %s"), *Modifier->GetName());
		}
#endif

		TCHAR buff[64] = { 0 };
		
		_stprintf(buff, TEXT("%s"), *Modifier->ObjName);
		strncpy(ModifierNodes[Id].szObjName, TCHAR_TO_UTF8(buff), sizeof(ModifierNodes[Id].szObjName));

		_stprintf(buff, TEXT("%s"), *Modifier->Param1.ValueStr);
		strncpy(ModifierNodes[Id].Param[0].szParam, TCHAR_TO_UTF8(buff), sizeof(ModifierNodes[Id].Param[0].szParam));

		_stprintf(buff, TEXT("%s"), *Modifier->Param2.ValueStr);
		strncpy(ModifierNodes[Id].Param[1].szParam, TCHAR_TO_UTF8(buff), sizeof(ModifierNodes[Id].Param[1].szParam));

		_stprintf(buff, TEXT("%s"), *Modifier->Param3.ValueStr);
		strncpy(ModifierNodes[Id].Param[2].szParam, TCHAR_TO_UTF8(buff), sizeof(ModifierNodes[Id].Param[2].szParam));

		if(Modifier->Param1.Value2)
		{
			TCHAR buff[64] = { 0 };
			_stprintf(buff, TEXT("%d"), Modifier->Param1.Value2->Id);
			strncpy(ModifierNodes[Id].Param[0].szParam, TCHAR_TO_UTF8(buff), sizeof(ModifierNodes[Id].Param[0].szParam));
		}

		if(Modifier->Param2.Value2)
		{
			TCHAR buff[64] = { 0 };
			_stprintf(buff, TEXT("%d"), Modifier->Param2.Value2->Id);
			strncpy(ModifierNodes[Id].Param[1].szParam, TCHAR_TO_UTF8(buff), sizeof(ModifierNodes[Id].Param[1].szParam));
		}
		
		if(Modifier->Param3.Value2)
		{
			TCHAR buff[64] = { 0 };
			_stprintf(buff, TEXT("%d"), Modifier->Param3.Value2->Id);
			strncpy(ModifierNodes[Id].Param[2].szParam, TCHAR_TO_UTF8(buff), sizeof(ModifierNodes[Id].Param[2].szParam));
		}

		//add by JimZhang@snda.com
		FString pStr[3] = { Modifier->Param1.ValueStr, Modifier->Param2.ValueStr, Modifier->Param3.ValueStr };

		for(int j = 0; j < 3; j++)
		{
			ModifierNodes[Id].Param[j].nXType = mxtUseNone;

			if(pStr[j].Len()>1)
			{
				if(pStr[j] == _T("X1") || pStr[j] == _T("x1"))
				{
					ModifierNodes[Id].Param[j].nXType = mxtUseX1;
				}
				else if(pStr[j] == _T("X2") || pStr[j] == _T("x2"))
				{
					ModifierNodes[Id].Param[j].nXType = mxtUseX2;
				}
				else if(pStr[j] == _T("X3") || pStr[j] == _T("x3"))
				{
					ModifierNodes[Id].Param[j].nXType = mxtUseX3;
				}
				else if(pStr[j] == _T("X4") || pStr[j] == _T("x4"))
				{
					ModifierNodes[Id].Param[j].nXType = mxtUseX4;
				}
			}
		}
	}

	SaveModifierFile(*Path, &ModifierHead, ModifierNodes);

	delete[] ModifierNodes;
	//
}

BOOL MDataBrowserControl::SaveModifierFile(const TCHAR* file, MODIFIER_FILE_HEADER* Header, MODIFIER_FILE_NODE_NEW* Node)
{
	FArchive* ar = GFileManager->CreateFileWriter(file);
	if(ar != NULL)
	{
		ar->ByteOrderSerialize(Header, sizeof(MODIFIER_FILE_HEADER));
		ar->ByteOrderSerialize(Node, sizeof(MODIFIER_FILE_NODE_NEW)*Header->nBlockCount);
		ar->Flush();
		ar->Close();
	}
	return true;
}

void MDataBrowserControl::SaveSpellNode(const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames, const FString& Path, int nClassType)
{
#if HAVE_SCC
	GWarn->BeginSlowTask(TEXT("Export to Excel"), TRUE);
	GWarn->MapCheck_Clear();

	//List<List<String^>^> ^ItemList = gcnew List<List<String^>^>();
	String ^ExcelPath = CLRTools::ToString(Path);
	
	int nIndex = 1;
	bool HeaderExported = false;
	TArray< UObject* > ObjectsInPackages;
	//for( INT PackageIndex = 0; PackageIndex < Packages.Num(); PackageIndex++ )
	//{
	//	UPackage* Package = Packages(PackageIndex);
	//	if(Package && !Package->IsFullyLoaded())
	//		Package->FullyLoad();
	//}
	
	array<String^, 2>^ ItemList;
	
	PackageTools::GetObjectsInPackages(&Packages, NULL, ObjectsInPackages);

	TArray< UMMOAbility* > exportingObjs;

	for(int i = 0; i < ObjectsInPackages.Num(); ++i)
	{
		UObject* Obj = ObjectsInPackages(i);
		if(!Obj || !Obj->IsA(UMMOAbility::StaticClass()))
		{
			continue;
		}

		UMMOAbility* ExcelableObj = Cast<UMMOAbility>(Obj);
		exportingObjs.AddItem(ExcelableObj);
	}

	INT CurSaveStep = 0;
	INT TotalSteps = exportingObjs.Num();

	for(int i = 0; i < TotalSteps; ++i)
	{
		UMMOAbility* ExcelableObj = exportingObjs(i);
	
		// If the table header has not been exported yet, export it first
		if(!HeaderExported)
		{
			TArray<FString> Headers, DefaultValues;
			ExcelableObj->ExportExcelHeaders2(Headers, DefaultValues);

			if(Headers.Num() > 0)
			{
				ItemList = gcnew array<String^, 2>(TotalSteps + 2, Headers.Num());
				for (INT i = 0; i < Headers.Num(); ++i)
				{
					ItemList[0, i] = CLRTools::ToString(Headers(i));
				}

				for (INT i = 0; i < DefaultValues.Num(); ++i)
				{
					ItemList[1, i] = CLRTools::ToString(DefaultValues(i));
				}

				//ItemList->Add(CLRTools::ToStringArray(Headers));
				//ItemList->Add(CLRTools::ToStringArray(DefaultValues));
			}

			HeaderExported = true;
		}

		TArray<FString> Item;
		ExcelableObj->DataToExcel2(Item);
		if(Item.Num() > 0)
		{					
			//ItemList->Add(CLRTools::ToStringArray(Item));

			for (INT j = 0; j < Item.Num(); ++j)
			{
				INT  idx = i + 2;
				ItemList[idx, j] = CLRTools::ToString(Item(j));
			}
		}

		GWarn->UpdateProgress( ++CurSaveStep, TotalSteps );

	}

	int idx = ExcelPath->LastIndexOf(".");
	String^ txtFile = ExcelPath->Substring(0, idx);
	txtFile += ".txt";
	FCheckoutResult sccResultTxt = FSourceControl::TryCheckOut(CLRTools::ToFString(txtFile));
	FCheckoutResult sccResult = FSourceControl::TryCheckOut(Path);
	if(sccResult.AddToSCC)
	{
		UnrealEdCSharp::Excel::ExcelUtil^ excelUtil = gcnew UnrealEdCSharp::Excel::ExcelUtil();
		excelUtil->SaveAsExcel(ItemList, ExcelPath);

		sccResult.TryAddToDefaultChangelist();
		sccResultTxt.TryAddToDefaultChangelist();
	}

	GWarn->EndSlowTask();
	GWarn->MapCheck_ShowConditionally();

	//DataBrowserCtrl->MyPackagePanel->SaveAsExcel(ItemList, ExcelPath);
#endif HAVE_SCC
}

void MDataBrowserControl::SaveAsExcelWithoutSelected(FString NodeName, FString NodeConfigDir)
{
#if HAVE_SCC
	FString ServerDataPth, ExlFileName, Path;
	GConfig->GetString(TEXT("Configuration"), TEXT("ServerDataPath"), ServerDataPth, GEditorIni);
	GConfig->GetString(TEXT("Configuration"), *NodeName, ExlFileName, GEditorIni);
	Path = ServerDataPth + ExlFileName;
	Path = GFileManager->ConvertToAbsolutePath(*Path);

	TArray<UPackage*> Packages;
	TArray<FString> PackageNames;
	FullLoadPackages(NodeConfigDir, Packages, PackageNames);

	FString PkgPath = "MMOAbility";
	GPackageFileCache->FindDataPathTypes(*NodeConfigDir, PkgPath);
	INT ClassIndex = GEditor->EditorClassHierarchy->Find(PkgPath);
	UClass* ClickedClass = GEditor->EditorClassHierarchy->GetClass(ClassIndex);

	SaveAsExcel(Packages, PackageNames, Path, ClassIndex,ExlFileName);
//	}
#endif HAVE_SCC
}

void MDataBrowserControl::FullLoadPackages(FString ConfigDir, TArray<UPackage*> &Packages, TArray<FString> &PackageNames)
{
	const FString& cfgDir = ConfigDir;
	debugIDf(TEXT("TemplateData"), TEXT("===== Loading Ability template data form %s"), *cfgDir);
	TArray<FString> pkgs;
	appFindFilesInDirectory(pkgs, *cfgDir, TRUE, FALSE);
	for (INT i = 0; i < pkgs.Num(); ++i)
	{
		UPackage* Package = UObject::LoadPackage( NULL, *pkgs(i), 0 );
		if( !Package )
		{
			warnf( TEXT("Unable to load %s"), *pkgs(i) );
			continue;;
		}

		if(!Package->IsFullyLoaded())
		{
			Package->FullyLoad();
		}
		Packages.AddUniqueItem(Package);
		PackageNames.AddItem(*pkgs(i));
	}
}

/*
//@not used for now, can consider later
  //Read animation blend parameter table from Excel hemu@snda
void MDataBrowserControl::LoadExcelCLR(const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames, const FString& Path)
{
	List<List<String^>^> ^ItemList = gcnew List<List<String^>^>();
	//List<String^> ^ObjNames = gcnew List<String^>();
	String ^ExcelPath = CLRTools::ToString(Path);
	for( INT PackageIndex = 0; PackageIndex < Packages.Num(); PackageIndex++ )
	{
		UPackage* Package = Packages(PackageIndex);

		if ( Package != NULL )
		{
			int nNum = Package->GetNetObjectCount(0);
			//String^ objName;
			//String^ objClassName;
			//String^ AbilityName;
			//String^ ID;
			for(int i = 0; i < nNum; ++i)
			{
				UObject* Obj = Package->GetNetObjectAtIndex(i);
				if(Obj && ScriptInterfaceCast<IIExcelable>(Obj))
				{
					//					IIExcelable Iexcel = ScriptInterfaceCast<IIExcelable>(Obj);
					List<String^> ^Item = gcnew List<String^>();
					TArray<FString> Row;
					if(Obj->IsA(UMMOAbility::StaticClass()))
					{
						UMMOAbility *Ability = (UMMOAbility *)Obj;

						Ability->DataToExcel(Row);
						for(int i = 0; i < Row.Num(); ++i)
						{
							Item->Add(CLRTools::ToString(Row(i)));
						}
					}
					ItemList->Add(Item);
				}
			}
		}
	}
	DataBrowserCtrl->MyPackagePanel->LoadExcel(ItemList, ExcelPath);
}*/


// EOF



