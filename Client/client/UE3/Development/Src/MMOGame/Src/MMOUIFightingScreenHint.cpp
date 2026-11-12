#include "stdafx.h"
#include "MMOGameUIClasses.h"

IMPLEMENT_CLASS(UMMOUIFightingScreenHint)

void UMMOUIFightingScreenHint::PlayTenSecCountDownAfter( INT realCountDownSec/* = 10*/ )
{
	eventUCCallPlayTenSecCountDown( realCountDownSec );

	//TenSecCountDownMc->SetVisible( TRUE );
	//TenSecCountDownMc->GotoAndPlayI( 2 );
}

//========================================================================
void UMMOUIFightingScreenHint::PreOnShow(UBOOL v)
{
	Super::PreOnShow( v );

	if( v ){
		FVector2D scrSz = MMOFacade::GetScreenSize();

		//INT thisWidth = eventGetWidth();
		//INT thisHeight = eventGetHeight();

		//INT panelX = (scrSz.X - thisWidth) / 2;
		//INT panelY = (scrSz.Y - Max(leftH, rightH)) / 2;

		INT panelX = scrSz.X / 2;

		FLOAT curPosX;
		FLOAT curPosY;
		GetPosition( curPosX, curPosY );

		SetPosition( panelX, curPosY );
	}

}

//
void UMMOUIFightingScreenHint::PostOnShow(UBOOL v)
{
	//To avoid taking focus
}

//========================================================================
void UMMOUIFightingScreenHint::Init()
{
	Super::Init();

	InitMc();
}

void UMMOUIFightingScreenHint::InitMc()
{
	TenSecCountDownMc = GetObject( TEXT("TenSecCountDown_mc") );
}

//===============================================================================================================
//===============================================================================================================
void UMMOUIFightingScreenHint::ASBeforeFightTenSecCountDownEnd()
{
	Show( FALSE );
}

//===================================================================================
void UMMOUIFightingScreenHint::ASOnExitCountDownTimeOver()
{
	Show( FALSE );
}
//===============================================================================================================
//===============================================================================================================