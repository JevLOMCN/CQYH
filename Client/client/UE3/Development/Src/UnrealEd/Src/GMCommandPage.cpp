/*=============================================================================
	Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

#include "UnrealEd.h"
#include "GMCommandPage.h"
#include "GMGame.h"
#include "BusyCursor.h"

BEGIN_EVENT_TABLE(WxGMCommandPage, WxGMPage)
	EVT_TEXT_ENTER(ID_GM_COMMAND_COMBOBOX, WxGMCommandPage::OnEnterCommand)
END_EVENT_TABLE()

WxGMCommandPage::WxGMCommandPage(FGMGame *InGame, wxNotebook *InNotebook)
    :	WxGMPage(InGame)
{
	// Get the current display info.
	UINT CurWidth;
	UINT CurHeight;
	UBOOL bFullscreen;
	GetGame()->GetDisplayInfo( CurWidth, CurHeight, bFullscreen );
	const FString StrCurRes = FString::Printf(TEXT("%d x %d%s"), CurWidth, CurHeight, bFullscreen?TEXT(" Fullscreen"):TEXT(""));

	const bool bSuccess = wxXmlResource::Get()->LoadPanel(this, InNotebook, TEXT("ID_GM_COMMAND_PAGE"));
	check( bSuccess );

	GMCommandTitle = new wxStaticText( this, wxID_STATIC, TEXT("Enter GM command: "), wxPoint(10, 20), wxSize(80, wxDefaultSize.y), 0 );
	GMCommandComboBox = new wxComboBox( this, ID_GM_COMMAND_COMBOBOX, TEXT(""), wxPoint(100, 20), wxSize(300, wxDefaultSize.y), 0, NULL, wxCB_DROPDOWN | wxCB_SORT | wxTE_PROCESS_ENTER );

	// Size the window.
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
}

/**
 * Return's the page's title, displayed on the notebook tab.
 */
const TCHAR *WxGMCommandPage::GetPageTitle() const
{
	return TEXT("Command");
}

/**
 * Refreshes page contents.
 */
void WxGMCommandPage::RefreshPage(UBOOL bForce)
{
	// Get the current display info.
	UINT CurWidth, CurHeight;
	UBOOL bFullscreen;
	GetGame()->GetDisplayInfo( CurWidth, CurHeight, bFullscreen );
}

void WxGMCommandPage::ProcessCommand( wxCommandEvent& FxUnused(event) )
{
	
}

/**
 * Helper function for refreshing the value of Slomo text field.
 */
void WxGMCommandPage::UpdateCommandCtrl(const FString& commandStr)
{
	GMCommandComboBox->Append( *commandStr, (void*)NULL );
	GMCommandComboBox->SetValue( TEXT("") );
}

/**
 * Called when a user presses enter in the Slomo text field.
 */
void WxGMCommandPage::OnEnterCommand(wxCommandEvent &In)
{
	FString commandStr = (const TCHAR*)GMCommandComboBox->GetValue();
	GetGame()->ExecConsoleCommand( *commandStr );
	UpdateCommandCtrl(commandStr);
}
