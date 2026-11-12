/********************************************************************
	Created:	2013/05/20
	Filename: 	DlgInterpEditorAutoShakeCamera
	file ext:	cpp
	Author:		hemu@snda
	Purpose:	Matinee auto-add screen shake animation
*********************************************************************/

#include "UnrealEd.h"
#include "DlgInterpEditorAutoShakeCamera.h"

BEGIN_EVENT_TABLE(WxInterpEditorAutoShakeCamera, wxDialog)
	EVT_BUTTON(wxID_OK, WxInterpEditorAutoShakeCamera::OnOkay)
	EVT_BUTTON(wxID_CANCEL, WxInterpEditorAutoShakeCamera::OnCancel)
END_EVENT_TABLE()


WxInterpEditorAutoShakeCamera::WxInterpEditorAutoShakeCamera(wxWindow* InParent, FLOAT IntervalMinimum, FLOAT IntervalMaximum) :
	wxDialog(InParent, wxID_ANY, TEXT("DlgInterpEditorAutoShakeCameraTitle"), wxDefaultPosition, wxSize(320, 480), wxCAPTION),
	Tolerance(5.0f), FullInterval(TRUE), IntervalStart(IntervalMinimum), IntervalEnd(IntervalMaximum),
	StartDirLabel(NULL), StartDirRadioBox(NULL), FrequencyLabel(NULL), FrequencyText(NULL),
	SwingLabel(NULL), SwingText(NULL), SwingSpin(NULL), IntervalLabel(NULL), IntervalText(NULL),
	CountLabel(NULL), CountText(NULL), OkayButton(NULL), CancelButton(NULL)
{
	static const UINT LabelHeight = 22;
    static const UINT LabelWidth = 80;
    static const UINT InputWidth = 100;

    UINT ctrlPosY     = 3;
    UINT ctrlPosLeft  = 3;
    UINT ctrlPosRight = ctrlPosLeft + LabelWidth + 3;

	StartDirLabel       = new wxStaticText(this, wxID_ANY, TEXT("DlgInterpEditorStartDir"), wxPoint( ctrlPosLeft, 3), wxSize( LabelWidth, LabelHeight));
    ctrlPosY += LabelHeight;
    wxString groupMethodChoices[] = {_("+X"), _("-X"),_("+Y"), _("-Y"),_("+Z"), _("-Z"),};
    StartDirRadioBox    = new wxRadioBox(this, wxID_ANY, TEXT(""), wxPoint( ctrlPosLeft, ctrlPosY ), wxSize( LabelHeight + InputWidth, LabelHeight ), 2, groupMethodChoices, 0, wxRA_HORIZONTAL);
    ctrlPosY += LabelHeight;

	FrequencyLabel      = new wxStaticText(this, wxID_ANY, TEXT("DlgInterpEditorFrequency"), wxPoint( ctrlPosLeft, ctrlPosY ), wxSize( LabelWidth, LabelHeight));
    FrequencyText       = new wxTextCtrl(this, wxID_ANY, TEXT(""), wxPoint( ctrlPosRight, ctrlPosY ), wxSize( InputWidth, LabelHeight));
    ctrlPosY += LabelHeight;

    SwingLabel          = new wxStaticText(this, wxID_ANY, TEXT("DlgInterpEditorSwing"), wxPoint( ctrlPosLeft, ctrlPosY ), wxSize( LabelWidth, LabelHeight));
    SwingText           = new wxTextCtrl(this, wxID_ANY, TEXT(""), wxPoint( ctrlPosRight, ctrlPosY ), wxSize( InputWidth, LabelHeight));
    ctrlPosY += LabelHeight;
    SwingSpin           = new wxSpinCtrl(this, wxID_ANY, TEXT("5"), wxPoint( ctrlPosRight, ctrlPosY ), wxSize( InputWidth, LabelHeight), 0x100, 0, 100, 5);
    ctrlPosY += LabelHeight;

    CountLabel          = new wxStaticText(this, wxID_ANY, TEXT("DlgInterpEditorCount"), wxPoint( ctrlPosLeft, ctrlPosY ), wxSize( LabelWidth, LabelHeight));
    CountText           = new wxTextCtrl(this, wxID_ANY, TEXT(""), wxPoint( ctrlPosRight, ctrlPosY ), wxSize( InputWidth, LabelHeight));
    ctrlPosY += LabelHeight;

    IntervalLabel       = new wxStaticText(this, wxID_ANY, TEXT("DlgInterpEditorInterval"), wxPoint( ctrlPosLeft, ctrlPosY ), wxSize( LabelWidth, LabelHeight));
    IntervalText        = new wxTextCtrl(this, wxID_ANY, TEXT(""), wxPoint( ctrlPosRight, ctrlPosY ), wxSize( InputWidth, LabelHeight));
    ctrlPosY += LabelHeight;


	OkayButton          = new wxButton(this, wxID_OK, TEXT("OK"), wxPoint( ctrlPosLeft, ctrlPosY ), wxSize(48, 25));
	CancelButton        = new wxButton(this, wxID_CANCEL, TEXT("Cancel"), wxPoint( ctrlPosRight, ctrlPosY ), wxSize(64, 25));

	FLocalizeWindow( this );
	FWindowUtil::LoadPosSize( TEXT("DlgInterpEditorAutoShakeCameraTitle"), this );
	Layout();
	OkayButton->SetDefault();
	SetDefaultItem(OkayButton);

	StartDirLabel->SetWindowStyle(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	FrequencyLabel->SetWindowStyle(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	SwingLabel->SetWindowStyle(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	IntervalLabel->SetWindowStyle(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

	TCHAR StringValue[MAX_SPRINTF]=TEXT("");
	appSprintf(StringValue, TEXT("%.2f"), IntervalMinimum);
	//IntervalStartControl->SetValue(StringValue);

	appSprintf(StringValue, TEXT("%.2f"), IntervalMaximum);
	//IntervalEndControl->SetValue(StringValue);
	//FullIntervalCheckBox->SetValue(true);

}

WxInterpEditorAutoShakeCamera::~WxInterpEditorAutoShakeCamera()
{

}

void WxInterpEditorAutoShakeCamera::OnOkay(wxCommandEvent& In)
{
	/*Tolerance = (FLOAT) ToleranceControl->GetValue();
	FullInterval = (UBOOL) FullIntervalCheckBox->GetValue();
	IntervalStart = appAtof(IntervalStartControl->GetValue().c_str());
	IntervalEnd = appAtof(IntervalEndControl->GetValue().c_str());
	*/wxDialog::EndDialog(TRUE);
}

void WxInterpEditorAutoShakeCamera::OnCancel(wxCommandEvent& In)
{
	wxDialog::EndDialog(FALSE);
}

