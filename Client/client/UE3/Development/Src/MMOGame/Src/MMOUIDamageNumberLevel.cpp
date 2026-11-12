#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "UITextParser.h"

IMPLEMENT_CLASS( UMMOUIDamageNumberLevel )


void UMMOUIDamageNumberLevel::Init()
{
	Super::Init();
	check(EventAdapter == NULL);

	if( GFXPlayer ){
		GFXPlayer->AddBeforeViewUpdateDelegate( FScriptDelegate(this, TEXT("BeforeViewUpdate")) );
	}
}

void UMMOUIDamageNumberLevel::UnInit()
{
	Super::UnInit();
	if( GFXPlayer ){
		GFXPlayer->RemoveBeforeViewUpdateDelegate( FScriptDelegate(EC_EventParm) );
	}
}

void UMMOUIDamageNumberLevel::BeforeViewUpdate(float DeltaTime)
{
	if(IsVisible==FALSE)return;

	if(MMO::GetCurrentGameState() == EGameState_InGame)
	{
		eventCallUpdatePos(DeltaTime);
	}
}

//-----------------------------------------------------------------------------------------------------
void UMMOUIDamageNumberLevel::SetDamageShowPara( const FString& FunctionName,INT PosX,INT PosY ,FVector Location)
{
	TArray<struct FASValue> Arguments;

	Arguments.AddZeroed( 5 ); //argument count
	Arguments(0).I = PosX;
	Arguments(0).Type = AS_Int;

	Arguments(1).I = PosY;
	Arguments(1).Type = AS_Int;

	Arguments(2).N = Location.X;
	Arguments(2).Type = AS_Number;

	Arguments(3).N = Location.Y;
	Arguments(3).Type = AS_Number;

	Arguments(4).N = Location.Z;
	Arguments(4).Type = AS_Number;

	Invoke( FunctionName , Arguments );
}
//-----------------------------------------------------------------------------------------------------
void UMMOUIDamageNumberLevel::SetValueDamageShowPara( const FString& FunctionName,INT PosX,INT PosY,INT Value ,FVector Location)
{
	TArray<struct FASValue> Arguments;

	Arguments.AddZeroed( 6 ); //number of arguments
	Arguments(0).I = PosX;
	Arguments(0).Type = AS_Int;

	Arguments(1).I = PosY;
	Arguments(1).Type = AS_Int;

	Arguments(2).I = Value;
	Arguments(2).Type = AS_Int;

	Arguments(3).N = Location.X;
	Arguments(3).Type = AS_Number;

	Arguments(4).N = Location.Y;
	Arguments(4).Type = AS_Number;

	Arguments(5).N = Location.Z;
	Arguments(5).Type = AS_Number;


	Invoke( FunctionName , Arguments );
}
//-----------------------------------------------------------------------------------------------------
void UMMOUIDamageNumberLevel::ShowAttackNumber(INT PosX,INT PosY,INT Number,FVector Location)
{
	SetValueDamageShowPara( TEXT("AddNewAttack") , PosX , PosY , Number ,Location);
}
//-----------------------------------------------------------------------------------------------------
void UMMOUIDamageNumberLevel::ShowAttackedNumber(INT PosX,INT PosY,INT Number,FVector Location)
{
	SetValueDamageShowPara( TEXT("AddNewAttacked") , PosX , PosY , Number ,Location);
}
//-----------------------------------------------------------------------------------------------------
void UMMOUIDamageNumberLevel::ShowDodge( int PosX , int PosY,INT Number,FVector Location)
{
	SetValueDamageShowPara( TEXT("AddNewDodge") , PosX , PosY ,Number, Location);
}
//-----------------------------------------------------------------------------------------------------
void UMMOUIDamageNumberLevel::ShowMiss( int PosX , int PosY,FVector Location)
{
	SetDamageShowPara(  TEXT("AddNewMiss") , PosX , PosY ,Location);
}
//-----------------------------------------------------------------------------------------------------
void UMMOUIDamageNumberLevel::ShowParry( int PosX , int PosY,FVector Location)
{
	SetDamageShowPara(  TEXT("AddNewParry") , PosX , PosY ,Location);
}
//-----------------------------------------------------------------------------------------------------
void UMMOUIDamageNumberLevel::ShowImmunity( int PosX , int PosY,FVector Location)
{
	SetDamageShowPara(  TEXT("AddNewImmunity") , PosX , PosY ,Location);
}
//-----------------------------------------------------------------------------------------------------
void UMMOUIDamageNumberLevel::ShowCriticalNumber( int PosX , int PosY , int Number,FVector Location)
{
	SetValueDamageShowPara( TEXT("AddNewCritical") , PosX , PosY , Number ,Location);
}
//-----------------------------------------------------------------------------------------------------
void UMMOUIDamageNumberLevel::ShowCriticaledNumber( int PosX , int PosY , int Number,FVector Location)
{
	SetValueDamageShowPara( TEXT("AddNewCriticaled") , PosX , PosY , Number ,Location);
}
//-----------------------------------------------------------------------------------------------------
void UMMOUIDamageNumberLevel::ShowBlockNumber( int PosX , int PosY , int Number,FVector Location)
{
	SetValueDamageShowPara( TEXT("AddNewBlock") , PosX , PosY , Number ,Location);
}

//-----------------------------------------------------------------------------------------------------
void UMMOUIDamageNumberLevel::ShowExpNumber(INT PosX,INT PosY,INT Number,FVector Location)
{
	SetValueDamageShowPara( TEXT("AddNewExp") , PosX , PosY , Number ,Location);
}

//-----------------------------------------------------------------------------------------------------
void UMMOUIDamageNumberLevel::ShowHPRestore(INT PosX,INT PosY,INT Number,FVector Location)
{
	SetValueDamageShowPara( TEXT("AddNewRestore") , PosX , PosY , Number ,Location);
}

//-----------------------------------------------------------------------------------------------------
void UMMOUIDamageNumberLevel::ShowGetMoney(INT PosX,INT PosY,INT Number, INT MoneyType ,FVector Location)
{
	//FString moneyName = TEXT("Properties.EMONEY_TYPE.EMoneyTypeName_");
	//moneyName += appItoa(MoneyType);
	//moneyName = MMOUI::ParseLocalizedPropertyPath(moneyName);
	FString moneyName = UITextParser::HTMLGetCurrencyName((MONEY_TYPE)MoneyType);

	TArray<struct FASValue> Arguments;

	Arguments.AddZeroed( 8 ); //argument count
	Arguments(0).I = MoneyType;
	Arguments(0).Type = AS_Int;

	Arguments(1).I = PosX;
	Arguments(1).Type = AS_Int;

	Arguments(2).I = PosY;
	Arguments(2).Type = AS_Int;

	Arguments(3).I = Number;
	Arguments(3).Type = AS_Int;

	Arguments(4).S = moneyName;
	Arguments(4).Type = AS_String;

	Arguments(5).N = Location.X;
	Arguments(5).Type = AS_Number;

	Arguments(6).N = Location.Y;
	Arguments(6).Type = AS_Number;

	Arguments(7).N = Location.Z;
	Arguments(7).Type = AS_Number;

	
	Invoke( TEXT("AddMoneyEx") , Arguments );
	//Invoke( TEXT("AddNewMoney") , Arguments );

}

//-----------------------------------------------------------------------------------------------------
void UMMOUIDamageNumberLevel::PlayerExpChange(INT CurrentExp,INT MaxExp,INT ChangeExp)
{
	INT ExpChange = 0;
	if( ChangeExp>0)
	{
		ExpChange = ChangeExp;
	}
	else if( MaxExp > LastMaxExp)
	{
		//Upgrade
		ExpChange = LastMaxExp - LastExp + CurrentExp;
	}
	else if( MaxExp == LastMaxExp )
	{
		ExpChange = CurrentExp - LastExp;
	}

	AMMOHud* Hud = MMOFacade::getSingleton().GetMyHud();
	if( Hud && ExpChange != 0 && Player)
	{
		Hud->AddGainInfo( EGainType_Exp ,  ExpChange , Player->Location );
	}

	LastExp = CurrentExp;
	LastMaxExp = MaxExp;
}

//-----------------------------------------------------------------------------------------------------
void UMMOUIDamageNumberLevel::InitInfo()
{
	IsMoneyVisible.Add( E_MONEYTYPE_MAX );
	for(INT i=0;i<E_MONEYTYPE_MAX;i++)
	{
		TweenData* pTweenData = MMOFacade::getSingleton().GetTweenData(		FString::Printf(TEXT("MoneyEx%d"),i)	);
		if(pTweenData){
			IsMoneyVisible(i) =  pTweenData->isVisible;
		}
		else
		{
			IsMoneyVisible(i) = 1;
		}
	}
	LastExp = MMOFacade::getSingleton().GetPlayerCurExp();
	LastMaxExp = MMOFacade::getSingleton().GetPlayerMaxExp();
	//LastMoney = MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_01);
}

//-----------------------------------------------------------------------------------------------------
void UMMOUIDamageNumberLevel::PlayerMoneyChange(BYTE MoneyType,INT DeltaMoney)
{
	INT MoneyChange = 0;

	//ShowGetMoney( , ,Money-OldMoney,MoneyType,);

	//ShowGetMoney (  screenPos.X, screenPos.y , Abs( GainInfoList[0].Value ) , GainInfoList[0].SubType ,GainInfoList[0].Location);

	if( IsMoneyVisible(MoneyType)==0 )return;
	//if( MoneyType == MONEYTYPE_02  || MoneyType == MONEYTYPE_03 || MoneyType == MONEYTYPE_04 || MoneyType == MONEYTYPE_SOUL_PTS)
	//{	
	//	return;
	//}

	MoneyChange = DeltaMoney;
	//LastMoney = MMOFacade::getSingleton().GetPlayerMoney((EMONEY_TYPE)MoneyType);

	if(MoneyType ==  MONEYTYPE_MORAL || MoneyType == MONEYTYPE_MASTER_EXP)
	{
		MoneyChange = MoneyChange / 1000;
	}

	if( MoneyChange > 0 )
	{
		AMMOHud* Hud = MMOFacade::getSingleton().GetMyHud();
		if( Hud && MoneyChange != 0 && Player)
		{
			Hud->AddGainInfo( EGainType_Money ,  MoneyChange , Player->Location , MoneyType );
		}
	}
	
}

//----------------------------------------------------------------------------------------------------

void UMMOUIDamageNumberLevel::ASSetPos3DTo2D(class UGFxObject* mc, FLOAT PosX, FLOAT PosY, FLOAT PosZ )
{
	if(mc){
		FVector2D ScreenPos;
		UBOOL suc = MMOFacade::getSingleton().GetScreenPixelFromWorld( FVector(PosX,PosY,PosZ) , ScreenPos );
		if(suc){
			//FLOAT dynamicX = mc->GetFloat(TEXT("dynamicX"));//  >SetInt(TEXT("x"),INT(ScreenPos.X+0.5));

			mc->SetInt(TEXT("x"),INT(ScreenPos.X+0.5));
			mc->SetInt(TEXT("y"),INT(ScreenPos.Y+0.5));
		}
	}
}


void UMMOUIDamageNumberLevel::FormatTweenData(class UGFxObject* DataObj, struct TweenData* pTweenData )
{
	if(DataObj && pTweenData){

		DataObj->SetBool("isEnable",TRUE);

		DataObj->SetFloat("sx",pTweenData->sx);
		DataObj->SetFloat("sy",pTweenData->sy);
		DataObj->SetFloat("sz",pTweenData->sz);

		DataObj->SetFloat("vx",pTweenData->vx);
		DataObj->SetFloat("vy",pTweenData->vy);
		DataObj->SetFloat("vz",pTweenData->vz);

		DataObj->SetFloat("ax",pTweenData->ax);
		DataObj->SetFloat("ay",pTweenData->ay);
		DataObj->SetFloat("az",pTweenData->az);

		DataObj->SetFloat("sxRange",pTweenData->sxRange);
		DataObj->SetFloat("syRange",pTweenData->syRange);
		DataObj->SetFloat("szRange",pTweenData->szRange);

		DataObj->SetFloat("vxRange",pTweenData->vxRange);
		DataObj->SetFloat("vyRange",pTweenData->vyRange);
		DataObj->SetFloat("vzRange",pTweenData->vzRange);

		DataObj->SetFloat("axRange",pTweenData->axRange);
		DataObj->SetFloat("ayRange",pTweenData->ayRange);
		DataObj->SetFloat("azRange",pTweenData->azRange);

		DataObj->SetFloat("scaleStart"	,pTweenData->scaleStart);
		DataObj->SetFloat("scaleBomb"	,pTweenData->scaleBomb);
		DataObj->SetFloat("scaleEnd"	,pTweenData->scaleEnd);

		DataObj->SetFloat("scaleStartRange"	,pTweenData->scaleStartRange);
		DataObj->SetFloat("scaleBombRange"	,pTweenData->scaleBombRange);
		DataObj->SetFloat("scaleEndRange"	,pTweenData->scaleEndRange);

		DataObj->SetFloat("durTime"		,pTweenData->durTime);
		DataObj->SetFloat("bombTime"	,pTweenData->bombTime);

		DataObj->SetFloat("dumping"		,pTweenData->dumping);

		DataObj->SetFloat("alphaStart"	,pTweenData->alphaStart);
		DataObj->SetFloat("alphaBomb"	,pTweenData->alphaBomb);
		DataObj->SetFloat("alphaEnd"	,pTweenData->alphaEnd);
		DataObj->SetFloat("alphaPow"	,pTweenData->alphaPow);
		DataObj->SetFloat("isVisible"	,pTweenData->isVisible);

		TArray<	FASValue> val;
		DataObj->Invoke("InitData", val);
	}
}
void UMMOUIDamageNumberLevel::ASSetTweenData(class UGFxObject* DataObj, const FString& TweenName )
{
	if(DataObj){
		TweenData* pTweenData = MMOFacade::getSingleton().GetTweenData(TweenName);
		FormatTweenData(DataObj,pTweenData);
	}
}
