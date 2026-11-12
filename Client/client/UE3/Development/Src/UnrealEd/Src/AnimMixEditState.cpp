/*=============================================================================
	hemu@snda 120416
	Animation blend parameter table, used for smooth return playback of animations such as combo actions
=============================================================================*/

#include "AnimMixEditState.h"
#include "../../Engine/Inc/EngineClasses.h"
#include "../../Engine/Inc/EngineMeshClasses.h"
#include "../../Engine/Inc/EngineAnimClasses.h"
#include "../../Engine/Inc/EngineAIClasses.h"
#include "../../GameFramework/Inc/GameFrameworkClasses.h"
#include "../../MMOGame/Inc/MMOGameClasses.h"

AnimMixEditState::AnimMixEditState( AnimMixMatrix* ed )
: AnimMixEditor(ed)
, BorderColor(0, 0, 0)
, BkgColor(112, 112, 112)
, BorderColorHighlight(255, 0, 0)
, BkgColorHighlight(255, 128, 128)
,YellowColor(255, 242, 0)
,GreenColor(0, 255, 64)
,WhiteColor(236, 236, 236)
,RedColor(255, 0, 0)
,CyanColor(255,0,255)
{
	EnumObjSource = FindObject<UEnum>(UTypeDefine::StaticClass(), TEXT("EAnimMixSource"), TRUE);
	EnumObjTarget = FindObject<UEnum>(UTypeDefine::StaticClass(), TEXT("EAnimMixTarget"), TRUE);
}
//----------- -------------------------------------------------------------
AnimMixEditState::~AnimMixEditState()
{

}
//------------------------------------------------------------------------
FString AnimMixEditState::GetEnumDisplayName( BYTE enumID,UBOOL bSource )
{
	UEnum* tEnumObj;
	
	if( bSource )
		tEnumObj = EnumObjSource;
	else
		tEnumObj = EnumObjTarget;
	FString EnumValueName = tEnumObj->GetEnum(enumID).ToString();
	const FString& AlternateEnumValueName = tEnumObj->GetMetaData( TEXT("DisplayName"), enumID );
	if ( AlternateEnumValueName.Len() == 0 )
	{
		FString LocalizedName = LocalizeProperty( *(tEnumObj->GetName()), *(EnumValueName) );	

		if(LocalizedName.Len() > 0)
		{
			return LocalizedName;
		}
		else
		{
			return EnumValueName;
		}
	}
	else
	{
		FString LocalizedName = LocalizeProperty( *(tEnumObj->GetName()), *(AlternateEnumValueName) );
		if(LocalizedName.Len() > 0)
		{
			return LocalizedName;
		}
		else
		{
			return AlternateEnumValueName;
		}
	}
}
