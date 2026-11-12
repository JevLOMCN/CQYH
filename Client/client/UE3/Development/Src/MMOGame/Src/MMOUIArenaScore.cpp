#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOArenaManager.h"

IMPLEMENT_CLASS(UMMOUIArenaScore)


void UMMOUIArenaScore::OpenArenaScoreSingle( )
{
	//var player2 = {IsMale:false,PlayerName:"dd",ClassId:1,ClassName:"Wudang",Alive:"alive",Lv:21,Kill:2,Score:"100",Damage:7777};
	//SetScoreData( { Title:"t" , WinTeam:"w" , LoseTeam:"L" , WinList:[player,player2,player2] , LoseList:[player,player] } );

	//if( !GFXPlayer )
	//{
	//	return;
	//}

	//ArenaPlayerData* winner = MMOArenaManager::getSingleton().GetWinnerPlayer();
	//ArenaPlayerData*  loser = MMOArenaManager::getSingleton().GetLoserPlayer();
	//if( !winner || !loser )
	//{
	//	return;
	//}

	//UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
	//if(data)
	//{
	//	data->SetString( TEXT("Title") , winner->roleName );
	//	data->SetString( TEXT("WinTeam") , winner->roleName );
	//	data->SetString( TEXT("LoseTeam") , loser->roleName );

	//	UGFxObject* winList = GFXPlayer->CreateArray();
	//	if(winList)
	//	{
	//		UGFxObject* winData = GFXPlayer->CreateObject( TEXT("Object") );
	//		if( winData )
	//		{
	//			winData->SetBool( TEXT("IsMale") , ( winner->sex == EGT_Male ) );
	//			winData->SetString( TEXT("PlayerName") , winner->roleName );
	//			winData->SetInt( TEXT("ClassId") , winner->classId );
	//			winData->SetString( TEXT("ClassName") , MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(winner->classId) ) );
	//			winData->SetString( TEXT("Alive") , TEXT("") );
	//			winData->SetInt( TEXT("Lv") , winner->level );
	//			winData->SetInt( TEXT("Kill") , winner->kill );
	//			winData->SetInt( TEXT("Damage") , winner->damage );

	//			if( winner->addScore >= 0 )
	//			{
	//				winData->SetString( TEXT("Score") , appItoa(winner->currentScore) + TEXT("(+") + appItoa(abs(winner->addScore)) + TEXT(")") );
	//			}
	//			//else
	//			//{
	//			//	winData->SetString( TEXT("Score") , appItoa(winner->currentScore) + TEXT("(-") + appItoa(abs(winner->addScore)) + TEXT(")") );
	//			//	
	//			//}
	//			
	//		}
	//		winList->SetElementObject( 0 , winData );
	//	}
	//	data->SetObject( TEXT("WinList") , winList );

	//	UGFxObject* loseList = GFXPlayer->CreateArray();
	//	if(loseList)
	//	{
	//		UGFxObject* loseData = GFXPlayer->CreateObject( TEXT("Object") );
	//		if( loseData )
	//		{
	//			loseData->SetBool( TEXT("IsMale") , ( loser->sex == EGT_Male ) );
	//			loseData->SetString( TEXT("PlayerName") , loser->roleName );
	//			loseData->SetInt( TEXT("ClassId") , loser->classId );
	//			loseData->SetString( TEXT("ClassName") , MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(loser->classId) ) );
	//			loseData->SetString( TEXT("Alive") , TEXT("") );
	//			loseData->SetInt( TEXT("Lv") , loser->level );
	//			loseData->SetInt( TEXT("Kill") , loser->kill );
	//			loseData->SetInt( TEXT("Damage") , loser->damage );

	//			if( loser->addScore >= 0 )
	//			{
	//				loseData->SetString( TEXT("Score") , appItoa(loser->currentScore) + TEXT("(-") + appItoa(abs(loser->addScore)) + TEXT(")") );
	//			}
	//			//else
	//			//{
	//			//	loseData->SetString( TEXT("Score") , appItoa(loser->currentScore) + TEXT("(-") + appItoa(abs(loser->addScore)) + TEXT(")") );

	//			//}

	//		}
	//		loseList->SetElementObject( 0 , loseData );
	//	}
	//	data->SetObject( TEXT("LoseList") , loseList );

	//	eventCallSetScoreData(data);
	//}

	//if(winner)
	//{
	//	SetMemberInfo( TRUE , 0 , winner->sex , winner->roleName , winner->sex , TRUE , winner->level
	//		, 1 , winner->currentScore , winner->addScore );
	//}

	//if(loser)
	//{
	//	SetMemberInfo( FALSE , 0 , loser->sex , loser->roleName , loser->sex , FALSE , loser->level
	//		, 0 , loser->currentScore , -loser->addScore );
	//}

}

void UMMOUIArenaScore::PostOnShow( UBOOL v )
{
	Super::PostOnShow(v);
	if( v )
	{
		//if( MMOArenaManager::getSingleton().GetSize() == ArenaSize_1vs1 )
		//{
		//	OpenArenaScoreSingle();
		//}
	}
}

void UMMOUIArenaScore::NativeInitOnce()
{
	UGFxObject* obj = GetObject( TEXT("LeaveButton") );
	if(obj)
		obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.ArenaLeave") ) );
}

void UMMOUIArenaScore::CloseArenaScore()
{
	Show(FALSE);
}

//void UMMOUIArenaScore::SetMemberNumber(INT num,UBOOL winner)
//{
//	TArray<struct FASValue> args;
//	args.AddZeroed(1);
//
//	args(0).I = num;
//	args(0).Type = AS_Int;
//
//	if(winner)
//	{
//		Invoke( TEXT("SetWinnerNum") , args );
//	}
//	else
//	{
//		Invoke( TEXT("SetLoserNum") , args );
//	}
//}

//void UMMOUIArenaScore::SetMemberInfo(UBOOL winner,INT index,INT sex,const FString& roleName,INT classId,UBOOL alive,INT lv,INT kill,INT score,INT getScore)
//{
//	TArray<struct FASValue> args;
//	args.AddZeroed(9);
//
//	args(0).I = index;
//	args(0).Type = AS_Int; //index:int , male:Boolean , playername:String , classId:int , className:String , alive:Boolean , lv:int , kill:int , score:String
//
//	args(1).B = ( sex == EGT_Male );
//	args(1).Type = AS_Boolean;
//
//	args(2).S = roleName;
//	args(2).Type = AS_String;
//
//	args(3).I = classId;
//	args(3).Type = AS_Int;
//
//	args(4).S = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(classId) );
//	args(4).Type = AS_String;
//
//	args(5).B = alive;
//	args(5).Type = AS_Boolean;
//
//	args(6).I = lv;
//	args(6).Type = AS_Int;
//
//	args(7).I = kill;
//	args(7).Type = AS_Int;
//
//	if( getScore >= 0 )
//	{
//		args(8).S = appItoa(score) + TEXT("(+") + appItoa(abs(getScore)) + TEXT(")");
//	}
//	else
//	{
//		args(8).S = appItoa(score) + TEXT("(-") + appItoa(abs(getScore)) + TEXT(")");
//	}
//	args(8).Type = AS_String;
//
//	if(winner)
//	{
//		Invoke( TEXT("SetWinnerData") , args );
//	}
//	else
//	{
//		Invoke( TEXT("SetLoserData") , args );
//	}
//}