#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"

IMPLEMENT_CLASS(UMMOUIMsgInfo)


//------------------------------------------------------------------------
void UMMOUIMsgInfo::TickWidget(FLOAT DeltaTime)
{
	

	if(InCdTime)
	{
		MsgCdTime += DeltaTime;
		if( MsgCdTime >= SameMsgCdTimeLimit )
		{
			MsgCdTime = 0.0;
			LastMsg = TEXT("");
			InCdTime = FALSE;
		}
	}

	for ( int i = 0 ; i < MsgInfoItemList.Num() ; i++ )
	{
		if( MsgInfoItemList(i).MsgShow )
		{
			float DeltaAlpha = DeltaTime/ShowTime; //DeltaTime*AlphaDeltaValue;
			if (MsgInfoItemList(i).deltaTime >= MsgDeltaTime)
				MsgInfoItemList(i).MsgAlpha -= DeltaAlpha;
			                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
			if( MsgInfoItemList(i).MsgAlpha > 0 && MsgInfoObjectList(i)  )
			{
				MsgInfoObjectList(i)->SetFloat( TEXT("alpha") , MsgInfoItemList(i).MsgAlpha );
			}	
			else
			{
				MsgInfoItemList(i).MsgShow = false;
				MsgInfoObjectList(i)->SetVisible( false );
				MsgInfoItemList(i).deltaTime = 0;
			}

			MsgInfoItemList(i).deltaTime += DeltaTime;
			
		}
	}
}

//------------------------------------------------------------------------
void UMMOUIMsgInfo::UpdateMsgText()
{
	
	for( INT i = 0 ; i < MsgInfoItemList.Num() ; i++)
	{
		if(!MsgInfoObjectList(i) )
			return;

		UGFxObject* text = MsgInfoObjectList(i)->GetObject(TEXT("Msg_TextField"));// textField"));
		if( !text )
		{
			return;
		}

		if( MsgInfoItemList(i).MsgShow )
		{			
			text->SetText( MsgInfoItemList(i).MsgText );

			switch ( MsgInfoItemList(i).Type )
			{
			case MsgInfoType_Warning:
				SetTextFieldFilter( text , MsgInfoType_Warning , MsgInfoTextColor(MsgInfoType_Warning) );
				//text->SetInt( TEXT("textColor") , 0xFF0000 );
				break;
			case MsgInfoType_Normal:
				SetTextFieldFilter( text , MsgInfoType_Normal , MsgInfoTextColor(MsgInfoType_Normal ) );
				//text->SetInt( TEXT("textColor") , 0xFFF9F2 );
				break;
			default:
				SetTextFieldFilter( text , MsgInfoType_Normal , MsgInfoTextColor(MsgInfoType_Normal ) );
				
				break;
			}
		}



		MsgInfoObjectList(i)->SetVisible( MsgInfoItemList(i).MsgShow );
	}
}

void UMMOUIMsgInfo::SetTextFieldFilter( UGFxObject* tf , INT type , FString textColor )
{
	if( tf && GFXPlayer )
	{
		UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
		data->SetObject( TEXT("textField") , tf );
		data->SetInt( TEXT("type") , type );
		data->SetString( TEXT("textColor") , textColor );

		eventCallSetFilter(data);
	}
}

void UMMOUIMsgInfo::Init()
{
	
	Super::Init();

	if( GFXPlayer ){
		GFXPlayer->AddPostAdvanceDelegate( FScriptDelegate(this, TEXT("PostAdvance")) );
	}

	MsgInfoItemList.Remove( 0 , MsgInfoItemList.Num() );
	MsgInfoItemList.AddZeroed( MsgInfoItemNum );
	MsgInfoObjectList.Remove( 0 , MsgInfoObjectList.Num() );
	MsgInfoObjectList.AddZeroed( MsgInfoItemNum );

	for( INT i = 0 ; i < MsgInfoObjectList.Num() ; i++ )
	{
		MsgInfoObjectList(i) = GetObject( FString( TEXT("message") )+appItoa(i) );
	}

	//颜色
	MsgInfoTextColor.AddZeroed(MsgInfoType_Num);
	MsgInfoFilterColor.AddZeroed(MsgInfoType_Num);

	GConfig->GetString( TEXT("MsgInfoColor"), TEXT("WarningText") , MsgInfoTextColor(MsgInfoType_Warning) , GUIColorIni );
	GConfig->GetString( TEXT("MsgInfoColor"), TEXT("NormalText") , MsgInfoTextColor(MsgInfoType_Normal) , GUIColorIni );

	GConfig->GetString( TEXT("MsgInfoColor"), TEXT("WarningFilter") , MsgInfoFilterColor(MsgInfoType_Warning) , GUIColorIni );
	GConfig->GetString( TEXT("MsgInfoColor"), TEXT("NormalFilter") , MsgInfoFilterColor(MsgInfoType_Normal) , GUIColorIni );

	GConfig->GetString( TEXT("MsgInfoColor"), TEXT("Ex1") , MsgInfoFilterColor(MsgInfoType_Ex1) , GUIColorIni );

	GConfig->GetFloat( TEXT("MsgInfoTime"), TEXT("ShowTime") , ShowTime , GDefaultGameIni );
	GConfig->GetFloat( TEXT("MsgInfoTime"), TEXT("MsgDeltaTime") , MsgDeltaTime , GDefaultGameIni );
	
	//
	FScriptDelegate fun;
	fun.Object = this;
	fun.FunctionName = TEXT("AddMsgInfo");
	MMOUI::GetUIManager()->SetScreenMessageDelegate(fun);

	for( INT i = 0 ; i < MsgInfoItemList.Num() ; i++ )
	{
		MsgInfoItemList(i).MsgAlpha = 1;
		MsgInfoItemList(i).MsgShow = FALSE;
		MsgInfoItemList(i).MsgText = TEXT("");
		MsgInfoItemList(i).Type = MsgInfoType_Table;
		MsgInfoItemList(i).deltaTime = 0;
	}
	UpdateMsgText();
}

void UMMOUIMsgInfo::UnInit()
{
	Super::UnInit();
	if( GFXPlayer ){
		GFXPlayer->RemovePostAdvanceDelegate( FScriptDelegate(EC_EventParm) );
	}
}

void UMMOUIMsgInfo::PostAdvance(FLOAT DeltaTime)
{
	TickWidget(DeltaTime);
}

void UMMOUIMsgInfo::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	//if(v)
	//{
	//	warnf(TEXT("UMMOUIMsgInfo::PostOnShow(UBOOL true)"));
	//}
	//else
	//{
	//	warnf(TEXT("UMMOUIMsgInfo::PostOnShow(UBOOL false)"));
	//}
}

FString UMMOUIMsgInfo::TryGetString(const FString& id_str)
{
    UBOOL ret = FALSE;
    FString str = MMOUI::ParseLocalizedPropertyPath(id_str, ret);
    if (ret)
        return str;
    return _T("");
}

void UMMOUIMsgInfo::PlayMsgSound(const FString& snd_name,INT idx)
{/*
    TArray<FString> snd_lst;
    GConfig->GetArray(_T("MMOErrorSound"), *snd_name, snd_lst, GUIIni);
    if (snd_lst.Num() <= 0)
        return;
    if (snd_lst.Num() <= 1)
    {
        MMOFacade::getSingleton().PlayUISound(*snd_lst(0));
        return;
    }

    AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
    // 指定了播放索引
    if (idx >= 0 && idx < snd_lst.Num())
        MMOFacade::getSingleton().PlayUISound(*snd_lst(idx));
    // 没指定则默认按职业播放
    else if (actor->GetClassId() >= 0 && actor->GetClassId() < snd_lst.Num())
        MMOFacade::getSingleton().PlayUISound(*snd_lst(actor->GetClassId()));
    // 否则随机
    else
    {
        INT idx = (INT)RandRange(0, snd_lst.Num() - 1);
        MMOFacade::getSingleton().PlayUISound(*snd_lst(idx));
    }*/

	static time_t ttLastTime = 0;
	static FLOAT nLastSpan = 0;

	FString strSound = MMOUI::FindSoundNameByLocalString(snd_name);
	if(strSound.IsEmpty())
		return;
	time_t ttN = time(NULL);
	if(ttN - ttLastTime > nLastSpan)
	{
		AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
		if(actor == NULL)
			return;
		INT nWhole = strSound.InStr("SoundCue");

		if(nWhole < 0)
		{
			FString soundTemp;
			FString soundPackage;
			UBOOL bOK = strSound.Split(".",&soundPackage,&soundTemp);
			if(!bOK)
				return;

			if(actor->ActorAttributes->GenderType == EGT_Male)
				soundTemp += "_Male";
			else
				soundTemp += "_Female";

			if(actor->GetClassId() == EPCT_PlayerClassA || actor->GetClassId() == EPCT_PlayerClassG) // Lancer reuses Warrior voice
			{
				soundPackage +=  "_Warrior";
				soundTemp += "_Warrior";
			}
			else if(actor->GetClassId() == EPCT_PlayerClassB || actor->GetClassId() == EPCT_PlayerClassH) // Sorcerer reuses Mage voice
			{
				soundPackage +=  "_Mage";
				soundTemp += "_Mage";
			}
			else if(actor->GetClassId() == EPCT_PlayerClassC)
			{
				soundPackage +=  "_Rogue";
				soundTemp += "_Rogue";
			}
			else if(actor->GetClassId() == EPCT_PlayerClassD)
			{
				soundPackage +=  "_Archer";
				soundTemp += "_Archer";
			}
			else if(actor->GetClassId() == EPCT_PlayerClassE)
			{
				soundPackage +=  "_Taoist";
				soundTemp += "_Taoist";
			}
			strSound = FString::Printf(TEXT("SoundCue'%s.%s'"),*soundPackage,*soundTemp);
		}
		USoundCue* hintSound = Cast<USoundCue>(StaticLoadObject( USoundCue::StaticClass(),
			NULL, *strSound/*TEXT("SoundCue'Bgm_Login.Bgm_Login'")*/, NULL, LOAD_NoWarn | LOAD_Quiet, NULL ));
		if(hintSound)
		{
			ttLastTime = ttN;
			nLastSpan  = hintSound->Duration;
			actor->PlaySound(hintSound);	 
		}
	}
}

void UMMOUIMsgInfo::AddMsgInfo( const FString& Text , BYTE type , const FString& SndName, INT idx )
{
	if(MsgInfoItemList.Num() <= 0)
	{
		return;
	}

	if( InCdTime && Text == LastMsg )
	{
		return;
	}

	if(Text==TEXT("  "))
	{
		return;
	}

	for( int i = MsgInfoItemList.Num() - 1 ; i >= 1  ; i--)
	{
		MsgInfoItemList(i) = MsgInfoItemList(i - 1);		
	}

	if( type == MsgInfoType_Table )
	{
		MsgInfoItemList(0).Type = MMOUI::FindMsgTypeByLocalString(SndName);
	}
	else
	{
		MsgInfoItemList(0).Type = type;
	}

	MsgInfoItemList(0).MsgAlpha = 1;
	MsgInfoItemList(0).MsgShow = true;
	MsgInfoItemList(0).MsgText = Text;
	MsgInfoItemList(0).deltaTime = 0;
	if (!SndName.IsEmpty())
	{
		PlayMsgSound(SndName, idx);
	}
// 	else
// 	{
// 		PlayMsgSound(Text, idx);
// 	}

	UpdateMsgText();
	TickWidget ( 0 );
	
	LastMsg = Text;
	InCdTime = TRUE;
	MsgCdTime = 0;
}

UBOOL UMMOUIMsgInfo::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	if( IsVisible )
	{
		if(	strHoverControllerName==TEXT("Msg_TextField")||
			strHoverControllerName==TEXT("message0") ||
			strHoverControllerName==TEXT("message1") ||
			strHoverControllerName==TEXT("message2") ||
			strHoverControllerName==TEXT("message3") )
		{
			return TRUE;
		}
	}
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace );
}