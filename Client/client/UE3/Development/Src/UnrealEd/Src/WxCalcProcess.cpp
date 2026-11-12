#include "WxCalcProcess.h"
#include "ScopedTransaction.h"
#include "ScopedObjectStateChange.h"
#include "PropertyWindow.h"



BEGIN_EVENT_TABLE(WxCalcProcess, WxLinkedObjEd )
	EVT_CLOSE( WxCalcProcess::OnClose )
	EVT_SIZE( WxCalcProcess::OnSize )

	//EVT_TOOL(IDM_SOUNDMATMATRIX_REFRESHALL, SoundMaterialMatrix::OnButtonRefreshAllData)	
	//EVT_TOOL(IDM_SOUNDMATMATRIX_SAVE, SoundMaterialMatrix::OnButtonSave)

	//EVT_TOOL(IDM_SOUNDMATMATRIX_HITSOUNDS, SoundMaterialMatrix::OnButtonHitSounds)	
	//EVT_TOOL(IDM_SOUNDMATMATRIX_FOOTSTEPSOUNDS, SoundMaterialMatrix::OnButtonFootstepSounds)

END_EVENT_TABLE()

WxCalcProcess::WxCalcProcess(wxWindow* InParent, wxWindowID InID, const TCHAR* Title)
	:WxLinkedObjEd(InParent, InID, Title)
{
}


WxCalcProcess::~WxCalcProcess(void)
{
	SaveProperties();
}

void WxCalcProcess::OpenEditor(UBOOL bCreateNewWindow, class wxWindow* ParentWindow, USeqAct_Modifier* SM)
{
	if(!SM->bInit)
	{
		TArray<FString> TypeList;
		UEnum* InEnum = FindObject<UEnum>(USeqAct_Modifier::StaticClass(), TEXT("EModifierTargetType"), TRUE);
		if (InEnum)
		{
			UBOOL bReplacedInValue=FALSE;

			// Names.Num() - 1, because the last item in an enum is the _MAX item
			for( INT i=0; i< InEnum->NumEnums() - 1; i++ )
			{
				// this is the name of the enum value (i.e. ROLE_Authority)
				FString EnumValueName = InEnum->GetEnum(i).ToString();

				// see if we specified an alternate name for this value using metadata
				const FString& AlternateEnumValueName = InEnum->GetMetaData( TEXT("DisplayName"), i );

				if ( AlternateEnumValueName.Len() == 0 )
				{
					FString LocalizedName = LocalizeProperty( *(InEnum->GetName()), *(EnumValueName) );	

					if(LocalizedName.Len() > 0)
					{
						TypeList.AddItem(LocalizedName);
					}
					else
					{
						TypeList.AddItem(EnumValueName);
					}
				}
				else
				{
					FString LocalizedName = LocalizeProperty( *(InEnum->GetName()), *(AlternateEnumValueName) );

					if(LocalizedName.Len() > 0)
					{
						TypeList.AddItem(LocalizedName);
					}
					else
					{
						TypeList.AddItem(EnumValueName);
					}
				}
			}
			WxDlgSelectType dlg;
			dlg.SetLabel(TEXT("SetType"));
			TArray<FString> PackageFiles;


			dlg.SetTypeList(TypeList);
			int Result = dlg.ShowModal();
			int nType = dlg.GetType();
			SM->Type = (BYTE)nType;//InEnum->GetEnum(nType).ToString();
			SM->bInit = true;
		}
	}
	WxCalcProcess* editor = new WxCalcProcess( ParentWindow, -1);
	editor->InitEditor();
	editor->LoadModifier(SM);
	editor->Show();
}

void WxCalcProcess::InitEditor()
{
	CreateControls(TRUE);
	LoadProperties();
}

void WxCalcProcess::CreateControls( UBOOL bTreeControl )
{
	GraphWindow = new WxLinkedObjVCHolder( this, -1, this );
	LinkedObjVC = GraphWindow->LinkedObjVC;
	
	// load the background texture
	BackgroundTexture = LoadObject<UTexture2D>(NULL, TEXT("EditorMaterials.KismetBackground"), NULL, LOAD_None, NULL);

	PropertyWindow = new WxPropertyWindowHost;
	PropertyWindow->Create(this, this);
	AddDockingWindow(PropertyWindow, FDockingParent::DH_Right, *LocalizeUnrealEd("Properties"));
	AddDockingWindow( GraphWindow, FDockingParent::DH_None, NULL );

	// Try to load a existing layout for the docking windows.
	LoadDockingLayout();
	ShowDockingWindow( PropertyWindow, TRUE );

}

void WxCalcProcess::LoadModifier(USeqAct_Modifier* SO)
{
	RootModifier = SO;
	this->SetLabel(*SO->GetName());
	if(RootModifier->Modifiers.Num() == 0)
	{
		UObject* ObjArchetype = NULL;
		UObject*		Parent				= RootModifier;
		UObject*		UseOuter			= Parent;
		EObjectFlags	MaskedOuterFlags	= UseOuter ? UseOuter->GetMaskedFlags(RF_PropagateToSubObjects) : 0;
		UObject*		NewObject			= GEngine->StaticConstructObject( USeqAct_Modifier::StaticClass(), UseOuter, NAME_None, MaskedOuterFlags | RF_PerObjectLocalized, ObjArchetype );

		USeqAct_Modifier* NewSeq = Cast<USeqAct_Modifier>(NewObject);
		check(NewSeq != NULL);

		NewSeq->Parent = RootModifier;
		NewSeq->bRoot = true;
		NewSeq->OnCreated();
		NewSeq->ObjName = Parent->GetName();
		NewSeq->NodeName = Parent->GetName();
		NewSeq->Type = SO->Type;
		NewSeq->ObjPosX = (LinkedObjVC->NewX - LinkedObjVC->Origin2D.X)/LinkedObjVC->Zoom2D;
		NewSeq->ObjPosY = (LinkedObjVC->NewY - LinkedObjVC->Origin2D.Y)/LinkedObjVC->Zoom2D;
		RootModifier->Modifiers.AddItem(NewSeq);
		//EditStates.AddItem(NewSeq);
	}

	for(int i = 0; i < RootModifier->Modifiers.Num(); ++i)
	{
		if(RootModifier->Modifiers(i)->bRoot && !RootModifier->Modifiers(i)->Parent)
			RootModifier->Modifiers(i)->Parent = RootModifier;
		RootModifier->Modifiers(i)->Type = SO->Type;
		ModifierDrawList.AddItem(RootModifier->Modifiers(i));
	}
	
}


void WxCalcProcess::OnClose( wxCloseEvent& In )
{

	this->Destroy();
}
//------------------------------------------------------------------------
void WxCalcProcess::OnSize(wxSizeEvent& In)
{
	RefreshViewport();
	In.Skip();
}

void WxCalcProcess::DrawObjects( FViewport* Viewport, FCanvas* Canvas )
{
	WxLinkedObjEd::DrawObjects( Viewport, Canvas );
	INT MouseOverIndex = INDEX_NONE;
	USequenceObject* MouseOverSeqObj = NULL;
	if(LinkedObjVC->MouseOverObject)
	{
		MouseOverSeqObj = Cast<USequenceObject>(LinkedObjVC->MouseOverObject);

		if (MouseOverSeqObj != NULL)
		{
			// See if its a SequenceOp.
			USequenceOp* MouseOverSeqOp = Cast<USequenceOp>(MouseOverSeqObj);
			if(MouseOverSeqOp)
			{
				// Need to convert from visible index to actual connector index.
				MouseOverIndex = MouseOverSeqOp->VisibleIndexToActualIndex(LinkedObjVC->MouseOverConnType, LinkedObjVC->MouseOverConnIndex);
			}
		}
	}
	TArray<UBOOL> SelectionStatus;
	for(INT i=0; i<ModifierDrawList.Num(); i++)
	{
		const UBOOL bSelected = SelectedSeqObjs.ContainsItem( ModifierDrawList(i) );
		SelectionStatus.AddItem( bSelected );
	}
	for(int i = 0; i < ModifierDrawList.Num(); ++i)
	{
		//if( EditStates(i)->bDrawFirst )
		{
			const UBOOL bSelected = SelectionStatus(i);
			const UBOOL bMouseOver = (ModifierDrawList(i) == MouseOverSeqObj);
			ModifierDrawList(i)->DrawSeqObj(Canvas, bSelected, bMouseOver, LinkedObjVC->MouseOverConnType, MouseOverIndex, appSeconds() - LinkedObjVC->MouseOverTime);
		//EditStates(i)->DrawSeqObj();//DrawObjects(Viewport, Canvas);
		}
	}
	// draw logic and variable Links
	for (INT i = 0; i < ModifierDrawList.Num(); i++)
	{
		ModifierDrawList(i)->DrawLogicLinks(Canvas, SelectedSeqObjs, MouseOverSeqObj, LinkedObjVC->MouseOverConnType, MouseOverIndex);
		ModifierDrawList(i)->DrawVariableLinks(Canvas, SelectedSeqObjs, MouseOverSeqObj, LinkedObjVC->MouseOverConnType, MouseOverIndex);

		if(ModifierDrawList(i)->bRoot)
		{
			FLOAT BreakpointImagePosX = ModifierDrawList(i)->ObjPosX - 20.f;
			FLOAT BreakpointImagePosY = ModifierDrawList(i)->ObjPosY;
			// Draw circle
			const FIntPoint CircleCenter(BreakpointImagePosX + 8.f, BreakpointImagePosY + 8.f);

			FLinkedObjDrawUtils::DrawNGon( Canvas, CircleCenter, FColor(255,0,0), 16, 8 );
		}
	}

}

void WxCalcProcess::MoveSelectedObjects( INT DeltaX, INT DeltaY )
{
	for(INT i=0; i<SelectedSeqObjs.Num(); i++)
	{
		USequenceObject* SeqObj = SelectedSeqObjs(i);

		SeqObj->ObjPosX += DeltaX;
		SeqObj->ObjPosY += DeltaY;
	}
}

void WxCalcProcess::UpdatePropertyWindow()
{
	PropertyWindow->SetObjectArray( SelectedSeqObjs, EPropertyWindowFlags::Sorted | EPropertyWindowFlags::ShouldShowCategories);
}

void WxCalcProcess::EmptySelection()
{
	//DuplicationCount = 0;
	SelectedSeqObjs.Empty();
	if (PropertyWindow != NULL)
	{
		PropertyWindow->SetObject(NULL,EPropertyWindowFlags::NoFlags);
	}
}

void WxCalcProcess::AddToSelection( UObject* Obj )
{
	checkSlow( Obj->IsA(USequenceObject::StaticClass()) );
	if ( Obj->IsA(USequenceObject::StaticClass()) )
	{
		SelectedSeqObjs.AddUniqueItem( (USequenceObject*)Obj );
		((USequenceObject*)Obj)->OnSelected();

		// If the debugger is active and running to the next selected node, we need to reset the "hidden" breakpoints
		//WxKismetDebugger* DebuggerWindow = GetDebuggerWindow();

		//if (DebuggerWindow)
		//{
		//	DebuggerWindow->Callstack->SetNodeListSelectionObject((USequenceObject*)Obj);
		//	if((DebuggerWindow->NextType == KDNT_NextSelected) && DebuggerWindow->bIsRunToNextMode)
		//	{		
		//		DebuggerWindow->ClearAllHiddenBreakpoints();
		//		if (DebuggerWindow->SetHiddenBreakpoints())
		//		{
		//			OpenKismetDebugger(GWorld->GetGameSequence(), NULL, NULL, FALSE);
		//		}
		//	}
		//}
	}
}

void WxCalcProcess::RemoveFromSelection( UObject* Obj )
{
	checkSlow( Obj->IsA(USequenceObject::StaticClass()) );
	if (Obj->IsA(USequenceObject::StaticClass()))
	{
		SelectedSeqObjs.RemoveItem( (USequenceObject*)Obj );
	}
}

UBOOL WxCalcProcess::IsInSelection( UObject* Obj ) const
{
	checkSlow( Obj->IsA(USequenceObject::StaticClass()) );
	return (Obj->IsA(USequenceObject::StaticClass()) && SelectedSeqObjs.ContainsItem( (USequenceObject*)Obj ));
}

INT WxCalcProcess::GetNumSelected() const
{
	return SelectedSeqObjs.Num();
}

void WxCalcProcess::SetSelectedConnector( FLinkedObjectConnector& Connector )
{
	checkSlow( Connector.ConnObj->IsA(USequenceOp::StaticClass()) );
	if (Connector.ConnObj->IsA(USequenceOp::StaticClass()))
	{
		ConnSeqOp = (USequenceOp*)Connector.ConnObj;
		ConnType = Connector.ConnType;
		ConnIndex = ConnSeqOp->VisibleIndexToActualIndex(Connector.ConnType, Connector.ConnIndex);
	}
	else
	{
		ConnSeqOp = NULL;
	}
}

FIntPoint WxCalcProcess::GetSelectedConnLocation(FCanvas* Canvas)
{
	if (ConnSeqOp == NULL)
	{
		return FIntPoint(0,0);
	}
	else
	{
		return ConnSeqOp->GetConnectionLocation(ConnType, ConnIndex);
	}
}

void WxCalcProcess::MakeConnectionToConnector( FLinkedObjectConnector& Connector )
{
	check( Connector.ConnObj->IsA(USequenceOp::StaticClass()) );
	USequenceOp* EndSeqOp = (USequenceOp*)Connector.ConnObj;
	INT ActualEndIndex = EndSeqOp->VisibleIndexToActualIndex(Connector.ConnType, Connector.ConnIndex);

	if(ConnType == LOC_INPUT && Connector.ConnType == LOC_OUTPUT)
		MakeLogicConnection((USequenceOp*)Connector.ConnObj, ActualEndIndex, ConnSeqOp, ConnIndex);
	else if(ConnType == LOC_OUTPUT && Connector.ConnType == LOC_INPUT)
		MakeLogicConnection(ConnSeqOp, ConnIndex, (USequenceOp*)Connector.ConnObj, ActualEndIndex);

	NotifyObjectsChanged();
}

void WxCalcProcess::MakeLogicConnection(USequenceOp* OutOp, INT OutConnIndex, USequenceOp* InOp, INT InConnIndex)
{
	// Check connector indices are within range before doing anything else.
	if( OutConnIndex >= OutOp->OutputLinks.Num() ||
		InConnIndex >= InOp->InputLinks.Num() )
	{
		return;
	}

	const FScopedTransaction Transaction( *LocalizeUnrealEd("MakeLogicConnection") );
	//OutOp->Modify();
	FScopedObjectStateChange Notifier(OutOp);

	// grab the output link
	FSeqOpOutputLink &outLink = OutOp->OutputLinks(OutConnIndex);
	// make sure the link doesn't already exist

	//Remove already connected modifiers
	{
		//OutOp;
		FSeqOpOutputLink &outLink = OutOp->OutputLinks(OutConnIndex);
		if(outLink.Links.Num() > 0)
		{
			USequenceOp* outLinkOp = outLink.Links(0).LinkedOp;
			if(outLinkOp)
			{
				((USeqAct_Modifier*)outLinkOp)->ParentNode = NULL;
			}
			switch(OutConnIndex)
			{
			case 0:
				((USeqAct_Modifier*)OutOp)->TrueRetNode = NULL;
				break;
			case 1:
				((USeqAct_Modifier*)OutOp)->NextNode = NULL;
				break;
			case 2:
				((USeqAct_Modifier*)OutOp)->FalseRetNode = NULL;
				break;
			default:
				break;
			}
			outLink.Links.Empty();

		}
		//InOp;
		USeqAct_Modifier* ParentOp = ((USeqAct_Modifier*)InOp)->ParentNode;
		if(ParentOp)
		{
			for (INT linkIdx = 0; linkIdx < ParentOp->OutputLinks.Num() ; linkIdx++)
			{
				// if it's an empty entry save the index
				if (ParentOp->OutputLinks(linkIdx).Links.Num() > 0)
				{
					if (ParentOp->OutputLinks(linkIdx).Links(0).LinkedOp == InOp)
					{
						ParentOp->OutputLinks(linkIdx).Links.Empty();
						switch(linkIdx)
						{
						case 0:
							((USeqAct_Modifier*)ParentOp)->TrueRetNode = NULL;
							break;
						case 1:
							((USeqAct_Modifier*)ParentOp)->NextNode = NULL;
							break;
						case 2:
							((USeqAct_Modifier*)ParentOp)->FalseRetNode = NULL;
							break;
						default:
							break;
						}
					}
				}
			}
			((USeqAct_Modifier*)InOp)->ParentNode = NULL;
		}
	}
	


	INT newIdx = -1;
	UBOOL bFoundEntry = 0;
	for (INT linkIdx = 0; linkIdx < outLink.Links.Num() && !bFoundEntry; linkIdx++)
	{
		// if it's an empty entry save the index
		if (outLink.Links(linkIdx).LinkedOp == NULL)
		{
			newIdx = linkIdx;
		}
		else
			// check for a match
			if (outLink.Links(linkIdx).LinkedOp == InOp &&
				outLink.Links(linkIdx).InputLinkIdx == InConnIndex)
			{
				bFoundEntry = 1;
			}
	}
	// if we didn't find an entry, add one
	if (!bFoundEntry)
	{
		// if we didn't find an empty entry, create a new one
		if (newIdx == -1)
		{
			newIdx = outLink.Links.AddZeroed();
		}
		// add the entry
		outLink.Links(newIdx).LinkedOp = InOp;
		outLink.Links(newIdx).InputLinkIdx = InConnIndex;
		// notify the op that it has connected to something

		OutOp->OnConnect(InOp,OutConnIndex);
		switch(OutConnIndex)
		{
		case 0:
			{
				((USeqAct_Modifier*)OutOp)->TrueRetNode = (USeqAct_Modifier*)InOp;
				((USeqAct_Modifier*)InOp)->ParentNode = (USeqAct_Modifier*)OutOp;
			}
			break;
		case 1:
			{
				((USeqAct_Modifier*)OutOp)->NextNode = (USeqAct_Modifier*)InOp;
				((USeqAct_Modifier*)InOp)->ParentNode = (USeqAct_Modifier*)OutOp;
			}
			break;
		case 2:
			{
				((USeqAct_Modifier*)OutOp)->FalseRetNode = (USeqAct_Modifier*)InOp;
				((USeqAct_Modifier*)InOp)->ParentNode = (USeqAct_Modifier*)OutOp;
			}
			break;
		default:
			break;
		}
	}
	else
	{
		Notifier.CancelEdit();
	}
}

void WxCalcProcess::ClickedLine(FLinkedObjectConnector &Src, FLinkedObjectConnector &Dest)
{
	UBOOL bCtrlDown = (LinkedObjVC->Viewport->KeyState(KEY_LeftControl) || LinkedObjVC->Viewport->KeyState(KEY_RightControl));	
	if (bCtrlDown)
	{
		// disable ctrl-dragging lines due to LD request
		return;
	}
	UBOOL bAltDown = (LinkedObjVC->Viewport->KeyState(KEY_LeftAlt) || LinkedObjVC->Viewport->KeyState(KEY_RightAlt));
	UBOOL bShiftDown = LinkedObjVC->Viewport->KeyState(KEY_LeftShift) || LinkedObjVC->Viewport->KeyState(KEY_RightShift);
	INT HitX = LinkedObjVC->Viewport->GetMouseX();
	INT HitY = LinkedObjVC->Viewport->GetMouseY();
	// drag this line
	if (bCtrlDown || bShiftDown || bAltDown)
	{
		FIntPoint MousePos(HitX,HitY), SrcPos, DestPos;
		USequenceOp *SrcOp = Cast<USequenceOp>(Src.ConnObj),
			*DestOp = Cast<USequenceOp>(Dest.ConnObj);
		if (SrcOp != NULL &&
			DestOp != NULL)
		{
			// figure out which end we should pick up
			SrcPos = SrcOp->GetConnectionLocation(LOC_OUTPUT,Src.ConnIndex);
			DestPos = DestOp->GetConnectionLocation(LOC_INPUT,Dest.ConnIndex);
			if ((SrcPos - MousePos).Size() < (DestPos - MousePos).Size())
			{
				// closer to source
				SetSelectedConnector(Dest);
			}
			else
			{
				// closer to end
				SetSelectedConnector(Src);
			}
			if (!bCtrlDown)
			{
				FScopedTransaction Transaction(TEXT("Remove Link"));	//@todo localize
				FScopedObjectStateChange Notifier(SrcOp);
				UBOOL bModified = FALSE;

				// break the connection to src
				for (INT Idx = 0; Idx < SrcOp->OutputLinks(Src.ConnIndex).Links.Num(); Idx++)
				{
					if (SrcOp->OutputLinks(Src.ConnIndex).Links(Idx).LinkedOp == DestOp &&
						SrcOp->OutputLinks(Src.ConnIndex).Links(Idx).InputLinkIdx == Dest.ConnIndex)
					{
						bModified = TRUE;
						SrcOp->OutputLinks(Src.ConnIndex).Links.Remove(Idx--,1);
						break;
					}
				}
				switch(Src.ConnIndex)
				{
				case 0:
					((USeqAct_Modifier*)SrcOp)->TrueRetNode = NULL;
					break;
				case 1:
					((USeqAct_Modifier*)SrcOp)->NextNode = NULL;
					break;
				case 2:
					((USeqAct_Modifier*)SrcOp)->FalseRetNode = NULL;
					break;
				default:
					break;
				}

				if ( !bModified )
				{
					Notifier.CancelEdit();
					Transaction.Cancel();
				}
			}
			if (bShiftDown || bCtrlDown)
			{
				// start making the line and update the position
				LinkedObjVC->bMakingLine = TRUE;
				LinkedObjVC->NewX = HitX;
				LinkedObjVC->NewY = HitY;
			}
		}
	}
	else
	{
		// select the two
		EmptySelection();
		AddToSelection(Src.ConnObj);
		AddToSelection(Dest.ConnObj);
	}
}


void WxCalcProcess::OpenNewObjectMenu()
{
//	WxMBKismetNewObject menu( this );
	//FTrackPopupMenu tpm( this, &menu );
//	tpm.Show();
	UObject* ObjArchetype = NULL;
	UObject*		Parent				= RootModifier;
	//BYTE*			Addr				= PropertyNode->GetValueBaseAddress( (BYTE*) Parent );
	UObject*		UseOuter			= Parent;//( NewClass->IsChildOf( UClass::StaticClass() ) ? Cast<UClass>(Parent)->GetDefaultObject() : Parent );
	EObjectFlags	MaskedOuterFlags	= UseOuter ? UseOuter->GetMaskedFlags(RF_PropagateToSubObjects) : 0;
	UObject*		NewObject			= GEngine->StaticConstructObject( USeqAct_Modifier::StaticClass(), UseOuter, NAME_None, MaskedOuterFlags | RF_PerObjectLocalized, ObjArchetype );


	USeqAct_Modifier* NewSeq = (USeqAct_Modifier*)NewObject;

	//USeqAct_Modifier* NewSeq;
	//NewSeq = new USeqAct_Modifier();
	NewSeq->OnCreated();
	NewSeq->ObjName = NewSeq->GetName();
	NewSeq->NodeName = NewSeq->GetName();
	NewSeq->Type = RootModifier->Type;
	NewSeq->ObjPosX = (LinkedObjVC->NewX - LinkedObjVC->Origin2D.X)/LinkedObjVC->Zoom2D;
	NewSeq->ObjPosY = (LinkedObjVC->NewY - LinkedObjVC->Origin2D.Y)/LinkedObjVC->Zoom2D;
	RootModifier->Modifiers.AddItem(NewSeq);
	ModifierDrawList.AddItem(NewSeq);
}

/*
USequenceObject* WxKismet::NewSequenceObject(UClass* NewSeqObjClass, INT NewPosX, INT NewPosY, UBOOL bTransient)
{
	if(!SeqObjPosIsValid(NewPosX, NewPosY))
	{
		appMsgf(AMT_OK, *LocalizeUnrealEd("NewObjectInvalidLocation"));
		return NULL;
	}

	// figure out which Outer to use
	UObject* DesiredOuter;
	if (bTransient) 
	{
		DesiredOuter = UObject::GetTransientPackage();
	}
	else
	{
		DesiredOuter = Sequence;
	}

	FScopedTransaction Transaction( *LocalizeUnrealEd("NewObject") );
	//Sequence->Modify();
	FScopedObjectStateChange Notifier(DesiredOuter);

	UBOOL bDoubleCommentBorderOffset = TRUE;

	// if creating a comment ask for the initial text
	FString CommentText;
	if (NewSeqObjClass == USequenceFrame::StaticClass())
	{
		WxDlgGenericStringEntry dlg;
		INT Result = dlg.ShowModal( TEXT("NewComment"), TEXT("CommentText"), TEXT("Comment") );
		if (Result != wxID_OK)
		{
			// abort!
			Transaction.Cancel();
			Notifier.CancelEdit();
			return NULL;
		}
		else
		{
			CommentText = dlg.GetEnteredString();
			bDoubleCommentBorderOffset = FALSE;
		}
	}

	USequenceObject* NewSeqObj = ConstructObject<USequenceObject>( NewSeqObjClass, DesiredOuter, NAME_None, RF_Transactional);

	// move the sequence object to the transient package prior to calling Modify for the first time. this way, if we undo the operation,
	// then immediately copy the parent sequence, we don't end up with a bunch of dead objects in our copied text
	if ( !bTransient )
	{
		NewSeqObj->MarkPendingKill();
		NewSeqObj->Modify();
		NewSeqObj->ClearFlags(RF_PendingKill);
	}
	NewSeqObj->SetFlags(Sequence->GetMaskedFlags(RF_ArchetypeObject|RF_Public));

	debugf(TEXT("Created new %s in sequence %s"),*NewSeqObj->GetName(),*Sequence->GetName());

	USequenceFrame* SeqFrame = Cast<USequenceFrame>(NewSeqObj);
	if (SeqFrame != NULL)
	{
		SeqFrame->ObjComment = CommentText;
	}
	// If creating a SequenceFrame (comment) object, and we have things selected, create frame around selection.
	if (SeqFrame != NULL && SelectedSeqObjs.Num() > 0)
	{
		FIntRect SelectedRect = CalcBoundingBoxOfSelected();
		SeqFrame->ObjPosX = SelectedRect.Min.X - (bDoubleCommentBorderOffset ? FitCommentToSelectedBorder * 2 : FitCommentToSelectedBorder);
		SeqFrame->ObjPosY = SelectedRect.Min.Y - (bDoubleCommentBorderOffset ? FitCommentToSelectedBorder * 2 : FitCommentToSelectedBorder);
		SeqFrame->SnapPosition(KISMET_GRIDSIZE, MaxSequenceSize);
		SeqFrame->SizeX = (SelectedRect.Max.X - SelectedRect.Min.X) + (2 * (bDoubleCommentBorderOffset ? FitCommentToSelectedBorder * 2 : FitCommentToSelectedBorder));
		SeqFrame->SizeY = (SelectedRect.Max.Y - SelectedRect.Min.Y) + (2 * (bDoubleCommentBorderOffset ? FitCommentToSelectedBorder * 2 : FitCommentToSelectedBorder));
		SeqFrame->bDrawBox = TRUE;
	}
	else
	{
		NewSeqObj->ObjPosX = NewPosX;
		NewSeqObj->ObjPosY = NewPosY;
		NewSeqObj->SnapPosition(KISMET_GRIDSIZE, MaxSequenceSize);
	}

	if ( Sequence->AddSequenceObject(NewSeqObj) )
	{
		NewSeqObj->OnCreated();

		RebuildTreeControl();

		EmptySelection();	
		AddToSelection(NewSeqObj);

		RefreshViewport();
		UpdatePropertyWindow();
		NotifyObjectsChanged();
	}
	else
	{
		Notifier.CancelEdit();
		Transaction.Cancel();
		NewSeqObj = NULL;
	}

	return NewSeqObj;
}
*/

void WxCalcProcess::DeleteSelected(UBOOL bSilent)
{
	// Make sure any pending property item changes are applied before we go and potentially delete
	// the object out from underneath the property window.
	PropertyWindow->FlushLastFocused();
	PropertyWindow->ClearLastFocused();

	UBOOL bDeletingSequence = FALSE;
	TArray<USequenceObject*> DeletingSeqs;

	// Loop through all selected objects and unselect anything that isn't deletable.
	// make a copy of the array in case there happen to be multiple copies of the same object
	// in the array for some reason.
	TArray<USequenceObject*> ArrayCopy = SelectedSeqObjs;
	for( INT ObjIdx = 0; ObjIdx < ArrayCopy.Num(); ObjIdx++ )
	{
		USequenceObject* DelObj = ArrayCopy(ObjIdx);
		if ( !DelObj->bDeletable )
		{
			RemoveFromSelection(DelObj);
		}

		USeqAct_Modifier* SeqOp = Cast<USeqAct_Modifier>(DelObj);
		if (SeqOp != NULL)
		{
			if(SeqOp->bRoot)
			{
				appMsgf(AMT_OK, *LocalizeUnrealEdMMO(TEXT("Modifier_CanNotDelRootNode")));
				return;
			}
			// iterate thraough all other objects, looking for output links that point to this op
			for (INT chkIdx = 0; chkIdx < ModifierDrawList.Num(); chkIdx++)
			{
				// if it is a sequence op,
				USeqAct_Modifier *ChkOp = ModifierDrawList(chkIdx);
				if (ChkOp != NULL && ChkOp != SeqOp)
				{
					// iterate through this op's output links
					for (INT linkIdx = 0; linkIdx < ChkOp->OutputLinks.Num(); linkIdx++)
					{
						// iterate through all the inputs linked to this output
						for (INT inputIdx = 0; inputIdx < ChkOp->OutputLinks(linkIdx).Links.Num(); inputIdx++)
						{
							if (ChkOp->OutputLinks(linkIdx).Links(inputIdx).LinkedOp == SeqOp)
							{
								ChkOp->Modify(TRUE);

								// remove the entry
								ChkOp->OutputLinks(linkIdx).Links.Remove(inputIdx--,1);
								switch(linkIdx)
								{
								case 0:
									((USeqAct_Modifier*)ChkOp)->TrueRetNode = NULL;
									break;
								case 1:
									((USeqAct_Modifier*)ChkOp)->NextNode = NULL;
									break;
								case 2:
									((USeqAct_Modifier*)ChkOp)->FalseRetNode = NULL;
									break;
								default:
									break;
								}
							}
						}
					}
				}
			}
			// clear the refs in this op, so that none are left dangling
			SeqOp->InputLinks.Empty();
			SeqOp->OutputLinks.Empty();
			SeqOp->VariableLinks.Empty();
		}

		ModifierDrawList.RemoveItem(SeqOp);
		USeqAct_Modifier* DelModifier = Cast<USeqAct_Modifier>(DelObj);
		if(DelModifier)
		{
			RootModifier->Modifiers.RemoveItem(DelModifier);
			DelModifier->Modify(TRUE);
			DelModifier->MarkPendingKill();
		}
	}

	bool hasRoot = false;
	for (INT i = 0; i < ModifierDrawList.Num(); ++i)
	{
		USeqAct_Modifier* modifier = ModifierDrawList(i);
		if(modifier->bRoot)
		{
			hasRoot = TRUE;
			if(!modifier->Parent)
			{
				modifier->Parent = RootModifier;
				modifier->Modify();
			}
			break;
		}
	}

	if(!hasRoot)
	{
		for (INT i = 0; i < ModifierDrawList.Num(); ++i)
		{
			USeqAct_Modifier* modifier = ModifierDrawList(i);
			if(modifier && !modifier->Parent)
			{
				modifier->ParentNode = NULL;
				modifier->Parent = RootModifier;
				modifier->bRoot = TRUE;
				modifier->Modify();
				break;
			}
		}
	}

	for(INT ObjIdx=0; ObjIdx<SelectedSeqObjs.Num(); ObjIdx++)
	{
		USequence* DelSeq = Cast<USequence>( SelectedSeqObjs(ObjIdx) );
		if(DelSeq)
		{
			bDeletingSequence = TRUE;
			DeletingSeqs.AddItem(DelSeq);
			DelSeq->FindSeqObjectsByClass(USequence::StaticClass(), DeletingSeqs, TRUE);
		}
	}

	// pop up a confirm when deleting sequences
	if (!bSilent && bDeletingSequence && !appMsgf(AMT_YesNo,*LocalizeUnrealEd("DeleteSequenceConfirm")))
	{
		// abort! abort!
		return;
	}

	if (LinkedObjVC != NULL && Cast<USequenceObject>(LinkedObjVC->MouseOverObject) != NULL &&
		SelectedSeqObjs.ContainsItem((USequenceObject*)(LinkedObjVC->MouseOverObject)))
	{
		LinkedObjVC->MouseOverObject = NULL;
	}

	{
	//	const FScopedTransaction Transaction( *LocalizeUnrealEd("DeleteSelected") );
	//	FScopedObjectStateChange Notifier(Sequence);

	//	Sequence->RemoveObjects(SelectedSeqObjs);
	}

	//// Ensure the SearchWindow (if up) doesn't contain references to deleted SequenceObjects.
	//if(SearchWindow)
	//{
	//	SearchWindow->ClearResultsList();
	//}

	SelectedSeqObjs.Empty();
	UpdatePropertyWindow();

	// May have deleted a named variable, so update their global status.
	//Sequence->UpdateNamedVarStatus();
	//GetRootSequence()->UpdateInterpActionConnectors();

	// We see if there are any PrefabInstances referring to any Sequences we are removing.
	// If they do - set the pointer to NULL.
	//for(TObjectIterator<APrefabInstance> It; It; ++It)
	//{
	//	APrefabInstance* PrefabInst = *It;
	//	if( !PrefabInst->bDeleteMe && 
	//		!PrefabInst->IsPendingKill() &&
	//		PrefabInst->SequenceInstance &&
	//		DeletingSeqs.ContainsItem(PrefabInst->SequenceInstance) )
	//	{
	//		PrefabInst->SequenceInstance = NULL;
	//	}
	//}

	// Reset connector 'copy' buffer.
	//ClearCopiedConnections();

	//RebuildTreeControl();
	UObject::CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);

	RefreshViewport();
	NotifyObjectsChanged();
}

void WxCalcProcess::EdHandleKeyInput(FViewport* Viewport, FName Key, EInputEvent Event)
{
	UBOOL bCtrlDown = Viewport->KeyState(KEY_LeftControl) || Viewport->KeyState(KEY_RightControl);
	UBOOL bShiftDown = Viewport->KeyState(KEY_LeftShift) || Viewport->KeyState(KEY_RightShift);
	UBOOL bAltDown = Viewport->KeyState(KEY_LeftAlt) || Viewport->KeyState(KEY_RightAlt);

	const INT HitX = Viewport->GetMouseX();
	const INT HitY = Viewport->GetMouseY();
	HHitProxy* HitResult = Viewport->GetHitProxy(HitX,HitY);

	if( Event == IE_Pressed )
	{		
		if( Key == KEY_Delete )
		{
			DeleteSelected();
		}
		//else if( (bCtrlDown && Key == KEY_W) || (bShiftDown && Key == KEY_D) )
		//{
		//	// preserve the duplication count despite the selection change
		//	// (apologies for the crappiness of this code, such a nightmare to fix w/o breaking normal pasting, grr)
		//	INT OldDuplicationCount = DuplicationCount;
		//	KismetCopy();
		//	PasteCount = OldDuplicationCount;
		//	KismetPaste();
		//	DuplicationCount = ++OldDuplicationCount;
		//}
		//else if( Key == KEY_BackSpace )
		//{
		//	CenterViewOnSeqObj(Sequence);
		//}
		//else if( bCtrlDown && Key == KEY_C )
		//{
		//	KismetCopy();
		//}
		//else if( bCtrlDown && Key == KEY_V )
		//{
		//	KismetPaste();
		//}
		//else if( bCtrlDown && Key == KEY_X )
		//{
		//	KismetCopy();
		//	DeleteSelected(TRUE);
		//}
		//else if( bCtrlDown && Key == KEY_Z )
		//{
		//	KismetUndo();
		//}
		//else if( bCtrlDown && Key == KEY_Y )
		//{
		//	KismetRedo();
		//}
		//else if( bCtrlDown && Key == KEY_N )
		//{
		//	DoOpenClassSearch();
		//}
		//else if( Key == KEY_C )
		//{
		//	if(SelectedSeqObjs.Num() > 0)
		//	{
		//		NewSequenceObject( USequenceFrame::StaticClass(), 0, 0 );
		//	}
		//}
		//else if( Key == KEY_A )
		//{
		//	if (bCtrlDown && bShiftDown)
		//	{
		//		SelectAllMatching();
		//	}
		//	else if (bCtrlDown)
		//	{
		//		SelectAll();
		//	}
		//	else
		//	{
		//		wxCommandEvent DummyEvent;
		//		OnButtonZoomToFit( DummyEvent );
		//		RefreshViewport();
		//	}
		//}
		//else if( bCtrlDown && Key == KEY_Tab )
		//{
		//	if(OldSequence)
		//	{
		//		ChangeActiveSequence(OldSequence);
		//	}
		//}
		//else if (Key == KEY_Equals || Key == KEY_Add)
		//{
		//	if (SelectedSeqObjs.Num() > 0)
		//	{
		//		ShowConnectors(SelectedSeqObjs);
		//	}
		//	else
		//	{
		//		ShowConnectors(Sequence->SequenceObjects);
		//	}
		//}
		//else if (Key == KEY_Underscore || Key == KEY_Subtract)
		//{
		//	if (SelectedSeqObjs.Num() > 0)
		//	{
		//		HideConnectors(Sequence,SelectedSeqObjs);
		//	}
		//	else
		//	{
		//		HideConnectors(Sequence,Sequence->SequenceObjects);
		//	}
		//}
		//else if ( Key == KEY_PageUp || Key == KEY_PageDown )
		//{
		//	// PageUp is bring to front, PageDown is send to back.
		//	if ( SelectedSeqObjs.Num() == 1 )
		//	{
		//		USequenceObject* SeqObj	= SelectedSeqObjs(0);
		//		const INT Index			= Sequence->SequenceObjects.FindItemIndex( SeqObj );
		//		if ( Index != INDEX_NONE )
		//		{
		//			const INT TargetIndex = (Key == KEY_PageUp) ? Sequence->SequenceObjects.Num()-1 : 0;
		//			const FScopedTransaction Transaction( *LocalizeUnrealEd("LinkedObjectModify") );
		//			Sequence->Modify();
		//			Sequence->SequenceObjects.SwapItems( Index, TargetIndex );
		//			RefreshViewport();
		//		}
		//	}
		//}
	}
	else if ( Event == IE_Released )
	{
		//// LMBRelease + hotkey performs certain actions.
		//if( Key == KEY_LeftMouseButton )
		//{
		//	// Toggle breakpoints on selected nodes
		//	if(bAltDown && HitResult && HitResult->IsA(HLinkedObjProxy::StaticGetType()))
		//	{
		//		if(SelectedSeqObjs.Num() == 1)
		//		{
		//			USequenceObject* Object = SelectedSeqObjs(0);
		//			USequenceOp* Op = Cast<USequenceOp>(Object);
		//			if(Op)
		//			{
		//				Op->SetBreakpoint(!Op->bIsBreakpointSet);
		//			}
		//		}
		//	}
		//	if( Viewport->KeyState(KEY_R) )
		//	{
		//		// Create a remote event and "Activate Remote Event" action pair.
		//		WxDlgGenericStringEntry dlg;
		//		const INT Result = dlg.ShowModal( TEXT("NewRemoteEvent"), TEXT("EventName"), TEXT("NewEventName") );
		//		if ( Result == wxID_OK )
		//		{
		//			const FString EventNameString( dlg.GetEnteredString() );
		//			if( !EventNameString.Len() )
		//			{
		//				appMsgf(AMT_OK,*LocalizeUnrealEd("Error_InvalidInput"));
		//			}
		//			else
		//			{
		//				const FName EventName( *EventNameString );
		//				const INT NewPosX = (LinkedObjVC->NewX - LinkedObjVC->Origin2D.X)/LinkedObjVC->Zoom2D;
		//				const INT NewPosY = (LinkedObjVC->NewY - LinkedObjVC->Origin2D.Y)/LinkedObjVC->Zoom2D;

		//				const FScopedTransaction Transaction( *LocalizeUnrealEd("NewRemoteEvent") );
		//				USeqEvent_RemoteEvent* RemoteEvent = static_cast<USeqEvent_RemoteEvent*>( NewSequenceObject( USeqEvent_RemoteEvent::StaticClass(), NewPosX, NewPosY ) );
		//				RemoteEvent->EventName = EventName;
		//				RemoteEvent->ObjComment = EventNameString;
		//				RemoteEvent->PostEditChange();
		//				USeqAct_ActivateRemoteEvent* ActivateRemoteEvent = static_cast<USeqAct_ActivateRemoteEvent*>( NewSequenceObject( USeqAct_ActivateRemoteEvent::StaticClass(), NewPosX+150, NewPosY ) );
		//				ActivateRemoteEvent->EventName = EventName;
		//				ActivateRemoteEvent->ObjComment = EventNameString;
		//				ActivateRemoteEvent->PostEditChange();
		//				Sequence->UpdateNamedVarStatus();
		//			}
		//		}
		//	}

		//	if( Viewport->KeyState(KEY_E) && !bShiftDown )
		//	{
		//		// Create a console event
		//		WxDlgGenericStringEntry dlg;
		//		const INT Result = dlg.ShowModal( TEXT("NewConsoleEvent"), TEXT("EventName"), TEXT("NewEventName") );
		//		if ( Result == wxID_OK )
		//		{
		//			const FString EventNameString( dlg.GetEnteredString() );
		//			if( !EventNameString.Len() )
		//			{
		//				appMsgf(AMT_OK,*LocalizeUnrealEd("Error_InvalidInput"));
		//			}
		//			else
		//			{
		//				const FName EventName( *EventNameString );
		//				const INT NewPosX = (LinkedObjVC->NewX - LinkedObjVC->Origin2D.X)/LinkedObjVC->Zoom2D;
		//				const INT NewPosY = (LinkedObjVC->NewY - LinkedObjVC->Origin2D.Y)/LinkedObjVC->Zoom2D;

		//				const FScopedTransaction Transaction( *LocalizeUnrealEd("NewConsoleEvent") );
		//				USeqEvent_Console* ConsoleEvent = static_cast<USeqEvent_Console*>( NewSequenceObject( USeqEvent_Console::StaticClass(), NewPosX, NewPosY ) );
		//				ConsoleEvent->ConsoleEventName = EventName;
		//				ConsoleEvent->ObjComment = EventNameString;
		//				ConsoleEvent->PostEditChange();
		//				Sequence->UpdateNamedVarStatus();
		//			}
		//		}
		//	}
		//
		//}
	}
}
