#include "stdafx.h"
#include "MMOMsgButtonManager.h"
#include "MMOFacade.h"
#include "MMOAttributeAccountManager.h"
#include "UITextParser.h"

//#include "MMOHintButtonCommand.h"

//static MMOHintButtonCommand_ArenaQueue Command_ArenaQueue;

MMOAttributeAccountManager::MMOAttributeAccountManager()
{
	//Init();
	//ClassId = 0;
	BattlePoint = 0;
	CurAttrListener = NULL;
}

MMOAttributeAccountManager::~MMOAttributeAccountManager()
{

}

const FString MMOAttributeAccountManager::GetAttributeName( INT groupId , INT index ) const
{
	if( AttributeGroupList.Num() <= groupId || AttributeGroupList(groupId).AttributeList.Num() < index )
	{
		//out of bounds
		return TEXT("");
	}
	if( !CurAttrListener )
	{
		return TEXT("");
	}
	FString res;

	StructAttributeItem item = AttributeGroupList(groupId).AttributeList(index);
	if( !item.DifferName )
	{
		res = item.AttributeName[EPCT_PlayerClassA];
	}
	else
	{
		res = item.AttributeName[ CurAttrListener->GetClassId() ];
	}

	if(item.ValueShowType == EVST_Bag || item.ValueShowType == EVST_Weap || item.ValueShowType == EVST_Armor)
	{
		INT valueB =  CurAttrListener->GetAttribute( item.AttributeValueType[EPCT_PlayerClassA] ) ;
		INT valueA = 0;
		switch(item.ValueShowType)
		{
		case EVST_Bag:
			{
				valueA = MMOFacade::getSingleton().GetCurrentBagWgt();
			}
			break;
		case EVST_Weap:
			{
				valueA = MMOFacade::getSingleton().GetCurrentWeaponWgt();
			}
			break;
		case EVST_Armor:
			{
				valueA = MMOFacade::getSingleton().GetCurrentArmorWgt();
			}
			break;
		}
		if(valueA > valueB)
			res = UITextParser::HTMLBeginFont("#FF0000")+res+ UITextParser::HTMLEndFont();
		else
			res = UITextParser::HTMLBeginFont("#FFFFFF")+res+ UITextParser::HTMLEndFont();
	}
	return res;
}

//----------get attribute value
const FString MMOAttributeAccountManager::GetAttributeValue( INT groupId , INT index ) const
{
	if( AttributeGroupList.Num() <= groupId || AttributeGroupList(groupId).AttributeList.Num() < index )
	{
		//out of bounds
		return TEXT("");
	}
	const StructAttributeItem& item = AttributeGroupList(groupId).AttributeList(index);


	//AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( !CurAttrListener )
	{
		return TEXT("");
	}

	INT curClass = 0;

	if( !item.DifferValueType )
	{
		curClass = EPCT_PlayerClassA;		
	}
	else
	{
		curClass = CurAttrListener->GetClassId();
	}

	if( item.AttributeValueType[curClass] == 0 && item.ValueShowType != EVST_ExpEtra)
	{
		return TEXT("0");
	}

	//level constant
	/*INT levelPara = MMOFacade::getSingleton().GetAttributeConfigData( CurAttrListener->GetLevel() , CurAttrListener->GetClassId() , catValue );
	if( levelPara == 0 )
	{
	return TEXT("0");
	}*/

	//some attributes are very special
	switch( item.AttributeValueType[curClass] )
	{
	case SAIP_PKValue:
		return appItoa( CurAttrListener->GetPkValue() );
	case SAIP_Fatigue:
		return appItoa( CurAttrListener->GetFatigue() );
	case SAIP_AttSpeedAdd:
		return FString::Printf( TEXT("%.0f%%") , ( (FLOAT)CurAttrListener->GetAttribute( catAtkSpeedRate ) - 10000 )/100 );
	case SAIP_MpRecover:
		{
			/*switch( CurAttrListener->GetClassId() )
			{
			case EPCT_PlayerClassA:
				return appItoa( abs( CurAttrListener->GetAttribute(catMpRecovery) ) );
			case EPCT_PlayerClassB:
				return appItoa(  CurAttrListener->GetAttribute(catMpRecovery) );
			case EPCT_PlayerClassC:
				return appItoa( abs( CurAttrListener->GetAttribute(catMpRecovery) ) );
			case EPCT_PlayerClassD:
				{*/
// JO*(WJOSIJERg
//					return appItoa( (FLOAT)CurAttrListener->GetAttribute(catMpRecoveryRate)*CurAttrListener->GetAttribute(catMaxMp)/10000 );
// JO*(WJOSIJERg end
			/*	}
			}*/
			//case EPCT_PlayerClassE:
				return appItoa( abs( CurAttrListener->GetAttribute(catMpRecovery) ) );

			//return TEXT("0");
		}
	}
			
	FString res;
	if( item.ValueShowType == EVST_Rate )
	{
		res = FString::Printf( TEXT("%.0f%%") , (FLOAT)CurAttrListener->GetAttribute( item.AttributeValueType[curClass] )/100 );
		//range-value attribute
		if( item.AttributeValueTypeB[curClass] != 0 )
		{
			res += TEXT("-");
			res += FString::Printf( TEXT("%.0f%%") , (FLOAT)CurAttrListener->GetAttribute( item.AttributeValueTypeB[curClass] )/100 );
		}
	}
	else if( item.ValueShowType == EVST_Int )
	{
		res = appItoa( CurAttrListener->GetAttribute( item.AttributeValueType[curClass] ) );
		//range-value attribute
		if( item.AttributeValueTypeB[curClass] != 0 )
		{
			res += TEXT("-");
			res += appItoa( CurAttrListener->GetAttribute( item.AttributeValueTypeB[curClass] ) );
		}
	}
	else if( item.ValueShowType == EVST_Decimal )
	{
		
		res = FString::Printf( TEXT("%.02f") , (FLOAT)CurAttrListener->GetAttribute( item.AttributeValueType[curClass] )/10000 );
		
		if( item.AttributeValueTypeB[curClass] != 0 )
		{
			res += TEXT("-");
			res += FString::Printf( TEXT("%.02f") , (FLOAT)CurAttrListener->GetAttribute( item.AttributeValueTypeB[curClass] )/10000 );
		}
	}
	else if( item.ValueShowType == EVST_Att )
	{
		//basically only attack power uses this
		res = FString::Printf( TEXT("%.0f") , (FLOAT)CurAttrListener->GetAttribute( item.AttributeValueType[curClass] )/10000 );

		if( item.AttributeValueTypeB[curClass] != 0 )
		{
			res += TEXT("-");
			res += FString::Printf( TEXT("%.0f") , (FLOAT)CurAttrListener->GetAttribute( item.AttributeValueTypeB[curClass] )/10000 );
		}
	}
	else if(item.ValueShowType == EVST_HP)
	{
		res =  appItoa(MMOFacade::getSingleton().GetMainPlayer()->ActorAttributes->GetCurHp());
		res += TEXT("/");
		res += appItoa(CurAttrListener->GetAttribute( item.AttributeValueType[curClass]) );
	}
	else if(item.ValueShowType == EVST_MP)
	{
		res = appItoa(MMOFacade::getSingleton().GetMainPlayer()->ActorAttributes->GetCurMp());
		res += TEXT("/");
		res += appItoa(CurAttrListener->GetAttribute( item.AttributeValueType[curClass] )) ;
	}
	else if(item.ValueShowType == EVST_Bag || item.ValueShowType == EVST_Weap || item.ValueShowType == EVST_Armor)
	{
		INT valueA = 0;

		INT valueB = CurAttrListener->GetAttribute( item.AttributeValueType[curClass] ) ;

		switch(item.ValueShowType)
		{
		case EVST_Bag:
			{
				valueA = MMOFacade::getSingleton().GetCurrentBagWgt();
			}
			break;
		case EVST_Weap:
			{
				valueA = MMOFacade::getSingleton().GetCurrentWeaponWgt();
			}
			break;
		case EVST_Armor:
			{
				valueA =MMOFacade::getSingleton().GetCurrentArmorWgt();
			}
			break;
		}
		res = appItoa(valueA);
		res += TEXT("/");
		res += appItoa(valueB);

		if(valueA > valueB)
			res = UITextParser::HTMLBeginFont("#FF0000")+res+ UITextParser::HTMLEndFont();
		else
			res = UITextParser::HTMLBeginFont("#FFFFFF")+res+ UITextParser::HTMLEndFont();
	}
	else if (item.ValueShowType == EVST_ExpEtra)
	{
		res = appItoa(expEtra);
	}
	

	return res;

}

UBOOL MMOAttributeAccountManager::IsRelationAttribute( INT groupId , INT index , INT Attri )
{
	if( AttributeGroupList.Num() <= groupId || AttributeGroupList(groupId).AttributeList.Num() < index )
	{
		//out of bounds
		return FALSE;
	}

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( !player )
	{
		return FALSE;
	}

	StructAttributeItem& item = AttributeGroupList(groupId).AttributeList(index);

	INT curClass = 0;
	if( item.DifferValueType )
	{
		curClass = player->GetClassId();		
	}
	else
	{
		curClass = EPCT_PlayerClassA;
	}

// JO*(WJOSIJERg
	//if( item.AttributeValueType[curClass] >= catEndExAttr )
		//return IsRelationSpecialAttribute( item.AttributeValueType[curClass] , Attri );
// JO*(WJOSIJERg end

	return item.AttributeValueType[curClass] == Attri || item.AttributeValueTypeB[curClass] == Attri ;
	
}

UBOOL MMOAttributeAccountManager::IsRelationSpecialAttribute( INT Special , INT Attri )
{
	switch( Special )
	{
	//case SAIP_CritRate:
	//	return ( Attri == catCrit ) || ( Attri == catPhyCritRate ) || ( Attri == catMagCritRate );
	case SAIP_AttSpeedAdd:
		return Attri == catAtkSpeedRate;
// JO*(WJOSIJERg
	//case SAIP_MpRecover:
	//	return ( Attri == catMaxMp ) || ( Attri == catMpRecovery ) || ( Attri == catMpRecoveryRate );
	case SAIP_HitRate:
		return Attri == catAccurate;
	//case SAIP_DodgeRate:
	//	return ( Attri == catDodge ) || ( Attri == catDodgeRate );
	//case SAIP_CritRate:
	//	return ( Attri == catMagCritRate ) || ( Attri == catPhyCritRate );
// JO*(WJOSIJERg end
	case SAIP_PhyDamageReduce:
		return Attri == catPhyDef;
	case SAIP_MagDamageReduce:
		return Attri == catMagDef;
	}
	return FALSE;
}

UBOOL MMOAttributeAccountManager::IsBattleAttribute( INT Attri )
{
	return TRUE;
}

FString MMOAttributeAccountManager::GetBattlePoint()
{ 
	if( !CurAttrListener )
	{
		return TEXT("");
	}
	return appItoa( CurAttrListener->GetPower() ); 
}

INT MMOAttributeAccountManager::GetOwnerAttribute( INT attrId , AttrListenerType type )
{
	if( AttrOwnerTypeList.Num() > (INT)type )
	{
		return AttrOwnerTypeList(type)->GetAttribute( attrId );		
	}
	return 0;
}

FString MMOAttributeAccountManager::GetDamagePoint()
{
	//AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( !CurAttrListener )
	{
		return TEXT("0");
	}
	//level constant
	INT levelPara = MMOFacade::getSingleton().GetAttributeConfigData( CurAttrListener->GetLevel() , CurAttrListener->GetClassId() , catDefault );
	if( levelPara == 0 || MMOUI::GetUIManager()->DodgeConstPara == 0 )
	{
		return TEXT("0");
	}
	//mage special
	INT res = 0 ;
	FLOAT aveDamage = 0;
	FLOAT critRate = 0;
	if( CurAttrListener->GetClassId() == EPCT_PlayerClassB )
	{
// JO*(WJOSIJERg
//		aveDamage = FLOAT( CurAttrListener->GetAttribute( catMagAtkHi ) + CurAttrListener->GetAttribute( catMagAtkLo ) )/20000;	
//		critRate = CurAttrListener->GetAttribute( catMagCritRate );
// JO*(WJOSIJERg end
	}
	else
	{
// JO*(WJOSIJERg
//		aveDamage = FLOAT( CurAttrListener->GetAttribute( catPhyAtkHi ) + CurAttrListener->GetAttribute( catPhyAtkLo ) )/20000;
//		critRate = CurAttrListener->GetAttribute( catPhyCritRate );
// JO*(WJOSIJERg end
	}

	if( CurAttrListener->GetAttribute( catAtkSpeedRate ) != 0 )
	{
		aveDamage = aveDamage*( (FLOAT)CurAttrListener->GetAttribute( catAtkSpeedRate )/10000 );
	}


	//crit factor
	// JO*(WJOSIJERg
	//FLOAT crt = (FLOAT)CurAttrListener->GetAttribute( catCrit ) / levelPara*MMOUI::GetUIManager()->CrtConstPara + critRate/10000;	
	//crt = 1 + crt * CurAttrListener->GetAttribute( catCritDamageModificationRate )/10000;
	////hit factor
	//FLOAT acc = CurAttrListener->GetAttribute( catAccurate );
	//acc =  acc / ( levelPara*MMOUI::GetUIManager()->DodgeConstPara ) + 4.0/3.0 ;
	//acc =  0.75 - 1/acc ;
	//acc = 1.25 * Min<FLOAT>( 1 , 0.8 * ( 1 + acc ) );
	////
	//FLOAT damageAddRate = 1 + (FLOAT)CurAttrListener->GetAttribute( catDamageEnhanceRate )/10000;

	//res = aveDamage * crt * damageAddRate * acc;
	// JO*(WJOSIJERg end

	//damage ability=(average physical damage/attack speed)*(1+crit rate*(crit damage multiplier-1))* (1+damage bonus)*1.25*min(1,(0.8*accuracy rate))
	return appItoa(res);
}

FString MMOAttributeAccountManager::GetLifePoint()
{
	//effective life=max life*1/(1-average damage reduction)*1/(1-dodge rate-parry rate-block rate*0.5)*1/min(1.2,(1+tenacity rate))+ max(0,block value*(block rate-0.33))
	
	//AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( !CurAttrListener )
	{
		return TEXT("0");
	}
	INT levelPara = MMOFacade::getSingleton().GetAttributeConfigData( CurAttrListener->GetLevel() , CurAttrListener->GetClassId() , catDefault );
	if( levelPara == 0 || MMOUI::GetUIManager()->CrtConstPara == 0 )
	{
		return TEXT("0");
	}

	FLOAT res = 0 ;
	//	damage reduction part: average damage reduction =1-2/( 1/(1-physical damage reduction) + 1/(1-magic damage reduction))
	INT phyDef = CurAttrListener->GetAttribute( catPhyDef );
	FLOAT phyDamageReduce = (FLOAT)phyDef/( phyDef + levelPara*MMOUI::GetUIManager()->PhyDefConstPara );
	INT magDef = CurAttrListener->GetAttribute( catMagDef );
	FLOAT magDamageReduce = (FLOAT)magDef/( magDef + levelPara*MMOUI::GetUIManager()->MagDefConstPara );
	
	FLOAT aveDamageReduce = 1 - 2/( 1/( 1 - phyDamageReduce ) + 1/( 1 - magDamageReduce ) ) ;
	FLOAT damageReduce = 1 - aveDamageReduce;	
	// dodge and parry part

	//0.75 - 1/( dodge/(level constant * dodge coefficient) + 4/3 )

	// JO*(WJOSIJERg
	//FLOAT dodge = CurAttrListener->GetAttribute( catDodge );
	//dodge = dodge/levelPara*MMOUI::GetUIManager()->DodgeConstPara + 4.0/3.0;
	//FLOAT dodgeRate = ( 0.75 - 1/dodge ) + CurAttrListener->GetAttribute(catDodgeRate)/10000;
	//FLOAT parry = 1 - dodgeRate - (FLOAT)CurAttrListener->GetAttribute( catParryRate )/10000 - (FLOAT)CurAttrListener->GetAttribute( catBlockRate )/10000*0.5;
	//parry = Max<FLOAT>(0.2,parry);
	////crit resistance part

	//FLOAT crtReduce = CurAttrListener->GetAttribute( catCritResistance );
	//crtReduce = crtReduce/ ( levelPara*MMOUI::GetUIManager()->CrtConstPara );
	//crtReduce = Min<FLOAT>( 1.2 , 1 + crtReduce );
	//crtReduce += Max<FLOAT>( 0 , (FLOAT)CurAttrListener->GetAttribute( catBlock )*((FLOAT)CurAttrListener->GetAttribute( catBlockRate )-0.33) );
	// JO*(WJOSIJERg end

	res = (FLOAT)CurAttrListener->GetAttribute( catMaxHp );
	if( damageReduce != 0 )
		res = res/damageReduce;
// JO*(WJOSIJERg
	//if( parry != 0 )
	//	res = res/parry;
	//if( crtReduce != 0 )
	//	res = res/crtReduce;
// JO*(WJOSIJERg end

	res = abs(res);
	return appItoa( INT(res) );
}

const FString MMOAttributeAccountManager::GetAttributeDesc( INT groupId , INT index ) const
{
	if( !AttributeGroupList.IsValidIndex(groupId) || !AttributeGroupList(groupId).AttributeList.IsValidIndex(index) )
	{
		//out of bounds
		return TEXT("");
	}

	const StructAttributeItem& item = AttributeGroupList(groupId).AttributeList(index);
	//AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( !CurAttrListener )
	{
		return TEXT("");
	}
	

	INT curClassTipsText = 0;
	INT curClassTipsType = 0;
	INT curClassTipsValue = 0;
	//whether tips text differs by class
	if( item.DifferTips )
	{
		curClassTipsText = CurAttrListener->GetClassId();
	}
	else
	{		
		curClassTipsText = EPCT_PlayerClassA;	
	}
	//whether tips attribute type differs by class
	if( item.DifferTipsValueType )
	{
		curClassTipsType = CurAttrListener->GetClassId();
	}
	else
	{
		curClassTipsType = EPCT_PlayerClassA;
	}
	//whether tips attribute value parameters differ by class
	if( item.DifferTipsValuePara )
	{
		curClassTipsValue = CurAttrListener->GetClassId();
	}
	else
	{
		curClassTipsValue = EPCT_PlayerClassA;
	}

	FLOAT paraA,paraB;
	paraA = paraB = 0.0f;
	FString strParaA,strParaB;

// JO*(WJOSIJERg
	//if( item.AttributeTypeA[curClassTipsType] >= catEndExAttr )
	//{
	//	//special formula
	//	//level constant
	//	INT levelPara = MMOFacade::getSingleton().GetAttributeConfigData( CurAttrListener->GetLevel() , CurAttrListener->GetClassId() , catValue );
	//	if( levelPara == 0 )
	//	{
	//		return TEXT("");
	//	}

	//	switch ( item.AttributeTypeA[curClassTipsType] )
	//	{
	//	case SAIP_PhyDamageReduce:
	//		{
	//			//physical defense/(physical defense + level constant*physical defense coefficient)
	//			INT phyDef = CurAttrListener->GetAttribute( catPhyDef );
	//			paraA = (FLOAT)phyDef/( phyDef + levelPara*MMOUI::GetUIManager()->PhyDefConstPara )*100;
	//		}
	//		break;
	//	case SAIP_MagDamageReduce:
	//		{
	//			INT magDef = CurAttrListener->GetAttribute( catMagDef );
	//			paraA = (FLOAT)magDef/( magDef + levelPara*MMOUI::GetUIManager()->MagDefConstPara )*100;
	//		}
	//		break;
	//	case SAIP_CritRate:
	//		{
	//			//crit/(level constant * crit coefficient) + system bonus value
	//			INT crt = CurAttrListener->GetAttribute( catCrit );
	//			FLOAT crtAdd = 0;
	//			switch( CurAttrListener->GetClassId() )
	//			{
	//			case EPCT_PlayerClassB:
	//				crtAdd =  CurAttrListener->GetAttribute(catMagCritRate) ;
	//				break;
	//			case EPCT_PlayerClassA:
	//			case EPCT_PlayerClassC:
	//			case EPCT_PlayerClassD:
	//			default:
	//				crtAdd =  CurAttrListener->GetAttribute(catPhyCritRate) ;
	//				break;
	//			}	

	//			FLOAT phyCrtRate = CurAttrListener->GetAttribute( catPhyCritRate );
	//			FLOAT magCrtRate = CurAttrListener->GetAttribute( catMagCritRate );
	//			paraA = ((FLOAT)crt/ (levelPara*MMOUI::GetUIManager()->CrtConstPara)) *100;
	//			paraB = paraA + crtAdd/100;
	//		}
	//		break;
	//	case SAIP_DodgeRate:
	//		{
	//			//0.75 - 1/( dodge/(level constant * dodge coefficient) + 4/3 )
	//			FLOAT dodge = CurAttrListener->GetAttribute( catDodge );
	//			dodge = dodge/levelPara*MMOUI::GetUIManager()->DodgeConstPara + 4.0/3.0;
	//			paraA = ( 0.75 - 1/dodge )*100;
	//			paraB = paraA + CurAttrListener->GetAttribute(catDodgeRate)/100 ;
	//		}
	//		break;
	//	case SAIP_HitRate:
	//		{
	//			//A: 0.75 - 1/(accuracy/(level constant * dodge coefficient) + 4/3)
	//			FLOAT acc = CurAttrListener->GetAttribute( catAccurate );
	//			acc =  acc / ( levelPara*MMOUI::GetUIManager()->DodgeConstPara ) + 4.0/3.0 ;
	//			paraA = ( 0.75 - 1/acc )*100;
	//		}
	//		break;
	//	case SAIP_CritReduceRate:
	//		{
	//			//tenacity/(level constant * crit coefficient) + system bonus value
	//			FLOAT crtReduce = CurAttrListener->GetAttribute( catCritResistance );
	//			paraA = crtReduce/ ( levelPara*MMOUI::GetUIManager()->CrtConstPara )*100;
	//		}
	//		break;
	//	case SAIP_AttSpeedAdd:
	//		{
	//			paraA = ( (FLOAT)CurAttrListener->GetAttribute( catAtkSpeedRate ) - 10000 )/100 ;
	//			
	//		}
	//		break;
	//	case SAIP_MpRecover:
	//		{
	//			switch( CurAttrListener->GetClassId() )
	//			{
	//			case EPCT_PlayerClassA:
	//				paraA = abs( CurAttrListener->GetAttribute(catMpRecovery) )/*/CurAttrListener->GetAttribute(catMpRecoveryPeriod)*/ ;
	//				break;
	//			case EPCT_PlayerClassB:
	//				paraA =  CurAttrListener->GetAttribute(catMpRecovery) ;
	//				break;
	//			case EPCT_PlayerClassC:
	//				paraA = abs( CurAttrListener->GetAttribute(catMpRecovery) ) ;
	//				break;
	//			case EPCT_PlayerClassD:
	//				paraA = (FLOAT)CurAttrListener->GetAttribute(catMpRecoveryRate) * CurAttrListener->GetAttribute(catMaxMp) / 10000 ;
	//				break;
	//			}				
	//		}
	//	}

	//}
	//else 
	//{
	//paraA = (FLOAT)CurAttrListener->GetAttribute( item.AttributeTypeA[curClassTipsType] ) * item.AttributeMulParaA[curClassTipsValue];
	//paraB = (FLOAT)CurAttrListener->GetAttribute( item.AttributeTypeB[curClassTipsType] ) * item.AttributeMulParaB[curClassTipsValue];
	//}
// JO*(WJOSIJERg end
	
	paraA = (FLOAT)CurAttrListener->GetAttribute( item.AttributeValueType[curClassTipsValue] );
	paraB = (FLOAT)CurAttrListener->GetAttribute( item.AttributeValueTypeB[curClassTipsValue] );

	if( item.TipsValueShowType == EVST_Int )
	{
		strParaA = FString::Printf(TEXT("%.0f") , paraA );
		strParaB = FString::Printf(TEXT("%.0f") , paraB );
	}
	else if( item.TipsValueShowType == EVST_Decimal )
	{
		strParaA = FString::Printf(TEXT("%.02f") , ( paraA /10000 ) );
		strParaB = FString::Printf(TEXT("%.02f") , ( paraB /10000 ) );
	}
	else if( item.TipsValueShowType == EVST_Rate )
	{
		strParaA = FString::Printf(TEXT("%.02f%%") , ( paraA / 100 ) );
		strParaB = FString::Printf(TEXT("%.02f%%") , ( paraB / 100 ) );
	}
	else if (item.TipsValueShowType == EVST_Att)
	{
		strParaA = FString::Printf(TEXT("%.00f") , ( paraA /10000 ) );
		strParaB = FString::Printf(TEXT("%.00f") , ( paraB /10000 ) );
	}
	else if (item.TipsValueShowType == EVST_HP)
	{
		strParaA = appItoa(MMOFacade::getSingleton().GetMainPlayer()->ActorAttributes->GetCurHp());
		strParaB = FString::Printf(TEXT("%.0f") , paraA );
	}
	else if (item.TipsValueShowType == EVST_MP)
	{
		strParaA = appItoa(MMOFacade::getSingleton().GetMainPlayer()->ActorAttributes->GetCurMp());
		strParaB = FString::Printf(TEXT("%.0f") , paraA );
	}
	else if(item.ValueShowType == EVST_Bag || item.ValueShowType == EVST_Weap || item.ValueShowType == EVST_Armor)
	{
		switch(item.ValueShowType)
		{
		case EVST_Bag:
			{
				strParaA = appItoa(MMOFacade::getSingleton().GetCurrentBagWgt());
			}
			break;
		case EVST_Weap:
			{
				strParaA = appItoa(MMOFacade::getSingleton().GetCurrentWeaponWgt());
			}
			break;
		case EVST_Armor:
			{
				strParaA = appItoa(MMOFacade::getSingleton().GetCurrentArmorWgt());
			}
			break;
		}
		strParaB = FString::FormatAsNumber(paraA);
	}
	else if(item.ValueShowType == EVST_ExpEtra)
	{
		strParaA = appItoa(expEtra);
		strParaB = appItoa(expEtra);
	}
	return FString::Printf( *item.TipsDesc[curClassTipsText] , *strParaA , *strParaB ); 
}

void MMOAttributeAccountManager::initBaseAttribute( INT groupId )
{
	INT itemNum = 8;
	
	FString strPath;
	StructAttributeGroup& groupAttribute = AttributeGroupList(groupId);
	
	for ( INT i = 0 ; i < itemNum ; i++ )
	{
		StructAttributeItem attributeItem;

		for ( INT j = 0 ; j < EPCT_MAX - 1 ; j++ )
		{
			strPath = FString::Printf( TEXT("MMOGame.Player.AttriGroup%d_ItemName%d") , groupId , i , j );
			attributeItem.AttributeName[j] = MMOUI::ParseLocalizedPropertyPath( strPath );
			strPath = FString::Printf( TEXT("MMOGame.Player.AttriGroup%d_ItemTipsDesc%d") , groupId , i , j );
			attributeItem.TipsDesc[j] = MMOUI::ParseLocalizedPropertyPath( strPath );
		}

		groupAttribute.AttributeList.AddItem( attributeItem );
	}

	//attack power
	INT pos = 0;
	initBaseAttributeByHighAndLow(catMaxPhyAtk,catPhyAtk,0,pos);

	//magic
	pos = 1;
	initBaseAttributeByHighAndLow(catMaxMagAtk,catMagAtk,0,pos);

	//taoism
	pos = 2;
	initBaseAttributeByHighAndLow(catMaxTaoAtk,catTaoAtk,0,pos);

	//defense
	pos = 3;
	initBaseAttributeByHighAndLow(catMaxPhyDef,catPhyDef,0,pos);

	//magic defense
	pos = 4;
	initBaseAttributeByHighAndLow(catMaxMagDef,catMagDef,0,pos);

	//life
	pos = 5;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_HP;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_HP;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catMaxHp;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catMaxHp;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 1;

	//magic
	pos = 6;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_MP;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_MP;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catMaxMp;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catMaxMp;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 1;

	//luck
	pos = 7;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catLuck;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catLuck;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 1;

// JO*(WJOSIJERg
	////strength
	//INT pos = 0;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catStrength;
	//groupAttribute.AttributeList(pos).DifferTips = TRUE;	
	//groupAttribute.AttributeList(pos).DifferTipsValuePara = TRUE;
	//groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Decimal;
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catStrength;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassA , catPhyAtkHi ).catStrength/100000000;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassB] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassB , catPhyDef ).catStrength/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassC] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassC , catPhyAtkHi ).catStrength/100000000;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassD] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassD , catPhyAtkHi ).catStrength/100000000;
	//groupAttribute.AttributeList(pos).AttributeTypeB[EPCT_PlayerClassA] = catStrength;
	//groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassD] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassD , catPhyDef ).catStrength/10000;
	//
	////agility
	//pos = 1;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catAgility;
	//groupAttribute.AttributeList(pos).DifferTips = TRUE;	
	//groupAttribute.AttributeList(pos).DifferTipsValuePara = TRUE;
	//groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Decimal;
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catAgility;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassA , catCrit ).catAgility/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassB] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassB , catDodge ).catAgility/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassC] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassC , catCrit ).catAgility/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassD] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassD , catPhyAtkHi ).catAgility/100000000;
	//groupAttribute.AttributeList(pos).AttributeTypeB[EPCT_PlayerClassA] = catAgility;
	//groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassA] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassA , catDodge ).catAgility/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassB] = 0;
	//groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassC] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassC , catDodge ).catAgility/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassD] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassD , catCrit ).catAgility/10000;

	////intelligence
	//pos = 2;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catIntellect;
	//groupAttribute.AttributeList(pos).DifferTips = TRUE;	
	//groupAttribute.AttributeList(pos).DifferTipsValuePara = TRUE;
	//groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Decimal;
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catIntellect;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassA , catAccurate ).catIntellect/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassB] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassB , catMagAtkHi ).catIntellect/100000000;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassC] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassC , catAccurate ).catIntellect/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassD] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassD , catAccurate ).catIntellect/10000;
	//groupAttribute.AttributeList(pos).AttributeTypeB[EPCT_PlayerClassA] = catIntellect;
	//groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassA] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassA , catCritResistance ).catIntellect/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassB] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassB , catAccurate ).catIntellect/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassC] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassC , catCritResistance ).catIntellect/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassD] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassD , catCritResistance ).catIntellect/10000;

	////constitution
	//pos = 3;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catStamina;
	//groupAttribute.AttributeList(pos).DifferTips = TRUE;	
	//groupAttribute.AttributeList(pos).DifferTipsValuePara = TRUE;
	//groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Decimal;
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catStamina;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassA , catMaxHp ).catStamina/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassB] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassB , catMaxHp ).catStamina/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassC] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassC , catMaxHp ).catStamina/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassD] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassD , catMaxHp ).catStamina/10000;
	//groupAttribute.AttributeList(pos).AttributeTypeB[EPCT_PlayerClassA] = catStamina;
	//groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassA] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassA , catHpRecovery ).catStamina/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassB] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassB , catHpRecovery ).catStamina/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassC] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassC , catHpRecovery ).catStamina/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassD] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassD , catHpRecovery ).catStamina/10000;

	////spirit
	//pos = 4;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catSpirit;
	//groupAttribute.AttributeList(pos).DifferTips = TRUE;	
	//groupAttribute.AttributeList(pos).DifferTipsValuePara = TRUE;
	//groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Decimal;
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catSpirit;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassA , catPhyDef ).catSpirit/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassB] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassB , catCrit ).catSpirit/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassC] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassC , catPhyDef ).catSpirit/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassD] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassD , catPhyDef ).catSpirit/10000;
	//groupAttribute.AttributeList(pos).AttributeTypeB[EPCT_PlayerClassA] = catSpirit;
	//groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassA] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassA , catMagDef ).catSpirit/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassB] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassB , catMagDef ).catSpirit/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassC] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassC , catMagDef ).catSpirit/10000;
	//groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassD] = (FLOAT)MMOFacade::getSingleton().GetAttribute2ConfigStructData( EPCT_PlayerClassD , catMagDef ).catSpirit/10000;
// JO*(WJOSIJERg end

}

void MMOAttributeAccountManager::initBaseAttributeByHighAndLow(CHAR_ATTR_TYPE typeHigh, CHAR_ATTR_TYPE typeLow,INT groupId, INT pos)
{
	StructAttributeGroup& groupAttribute = AttributeGroupList(groupId);

	groupAttribute.AttributeList(pos).DifferName = TRUE;
	groupAttribute.AttributeList(pos).DifferValueType = TRUE;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = typeLow;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassB] = typeLow;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassC] = typeLow;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassD] = typeLow;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassE] = typeLow;

	groupAttribute.AttributeList(pos).AttributeValueTypeB[EPCT_PlayerClassA] = typeHigh;
	groupAttribute.AttributeList(pos).AttributeValueTypeB[EPCT_PlayerClassB] = typeHigh;
	groupAttribute.AttributeList(pos).AttributeValueTypeB[EPCT_PlayerClassC] = typeHigh;
	groupAttribute.AttributeList(pos).AttributeValueTypeB[EPCT_PlayerClassD] = typeHigh;
	groupAttribute.AttributeList(pos).AttributeValueTypeB[EPCT_PlayerClassE] = typeHigh;

	groupAttribute.AttributeList(pos).DifferTips = TRUE;	
	groupAttribute.AttributeList(pos).DifferTipsValueType = TRUE;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Int;

	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = typeLow;
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassB] = typeLow;
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassC] = typeLow;
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassD] = typeLow;
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassE] = typeLow;

	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0.0001;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassB] = 0.0001;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassC] = 0.0001;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassD] = 0.0001;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassE] = 0.0001;


	groupAttribute.AttributeList(pos).AttributeTypeB[EPCT_PlayerClassA] = typeHigh;
	groupAttribute.AttributeList(pos).AttributeTypeB[EPCT_PlayerClassB] = typeHigh;
	groupAttribute.AttributeList(pos).AttributeTypeB[EPCT_PlayerClassC] = typeHigh;
	groupAttribute.AttributeList(pos).AttributeTypeB[EPCT_PlayerClassD] = typeHigh;
	groupAttribute.AttributeList(pos).AttributeTypeB[EPCT_PlayerClassE] = typeHigh;

	groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassA] = 0.0001;
	groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassB] = 0.0001;
	groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassC] = 0.0001;
	groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassD] = 0.0001;
	groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassE] = 0.0001;

}

void MMOAttributeAccountManager::initAttAttribute( INT groupId )
{
	INT itemNum = 7;

	FString strPath;
	StructAttributeGroup& groupAttribute = AttributeGroupList(groupId);

	for ( INT i = 0 ; i < itemNum ; i++ )
	{
		StructAttributeItem attributeItem;

		for ( INT j = 0 ; j < EPCT_MAX -1; j++ )
		{
			strPath = FString::Printf( TEXT("MMOGame.Player.AttriGroup%d_ItemName%d") , groupId , i , j );
			attributeItem.AttributeName[j] = MMOUI::ParseLocalizedPropertyPath( strPath );
			strPath = FString::Printf( TEXT("MMOGame.Player.AttriGroup%d_ItemTipsDesc%d") , groupId , i , j );
			attributeItem.TipsDesc[j] = MMOUI::ParseLocalizedPropertyPath( strPath );
		}

		groupAttribute.AttributeList.AddItem( attributeItem );
	}

	//defense penetration
	INT pos = 0;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catAntiPhyDef;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catAntiPhyDef;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;

	//magic defense penetration
	pos = 1;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catAntiMagDef;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catAntiMagDef;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;

	//accuracy
	pos = 2;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catAccurate;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catAccurate;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;

	//crit rate
	pos = 3;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catCrit;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catCrit;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;

	//crit damage
	pos = 4;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catCritDmg;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catCritDmg;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;

	//damage bonus
	pos = 5;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catDmgEnhance;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catDmgEnhance;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;

	//attack speed
	pos = 6;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catAtkSpeed;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catAtkSpeed;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;

	//attack power
	/*INT pos = 0;
	groupAttribute.AttributeList(pos).DifferName = TRUE;
	groupAttribute.AttributeList(pos).DifferValueType = TRUE;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Att;*/
	// JO*(WJOSIJERg
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catPhyAtkLo;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassB] = catMagAtkLo;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassC] = catPhyAtkLo;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassD] = catPhyAtkLo;
	//groupAttribute.AttributeList(pos).AttributeValueTypeB[EPCT_PlayerClassA] = catPhyAtkHi;
	//groupAttribute.AttributeList(pos).AttributeValueTypeB[EPCT_PlayerClassB] = catMagAtkHi;
	//groupAttribute.AttributeList(pos).AttributeValueTypeB[EPCT_PlayerClassC] = catPhyAtkHi;
	//groupAttribute.AttributeList(pos).AttributeValueTypeB[EPCT_PlayerClassD] = catPhyAtkHi;
	// JO*(WJOSIJERg end
	/*groupAttribute.AttributeList(pos).DifferTips = TRUE;	
	groupAttribute.AttributeList(pos).DifferTipsValueType = TRUE;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Decimal;*/
	// JO*(WJOSIJERg
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catPhyAtkLo;
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassB] = catMagAtkLo;
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassC] = catPhyAtkLo;
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassD] = catPhyAtkLo;
	// JO*(WJOSIJERg end
	/*groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0.0001;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassB] = 0.0001;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassC] = 0.0001;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassD] = 0.0001;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassE] = 0.0001;*/
	// JO*(WJOSIJERg
	//groupAttribute.AttributeList(pos).AttributeTypeB[EPCT_PlayerClassA] = catPhyAtkHi;
	//groupAttribute.AttributeList(pos).AttributeTypeB[EPCT_PlayerClassB] = catMagAtkHi;
	//groupAttribute.AttributeList(pos).AttributeTypeB[EPCT_PlayerClassC] = catPhyAtkHi;
	//groupAttribute.AttributeList(pos).AttributeTypeB[EPCT_PlayerClassD] = catPhyAtkHi;
	// JO*(WJOSIJERg end
	/*groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassA] = 0.0001;
	groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassB] = 0.0001;
	groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassC] = 0.0001;
	groupAttribute.AttributeList(pos).AttributeMulParaB[EPCT_PlayerClassD] = 0.0001;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassE] = 0.0001;*/

	//attack speed bonus
	/*pos = 1;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = SAIP_AttSpeedAdd;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = SAIP_AttSpeedAdd;*/
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0.01;

	//accuracy
	/*pos = 2;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catAccurate;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = SAIP_HitRate;*/
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 1;
	//crit
	/*pos = 3;
	groupAttribute.AttributeList(pos).DifferName = TRUE;	
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catCrit;
	groupAttribute.AttributeList(pos).DifferTipsValueType = FALSE;
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = SAIP_CritRate;*/


	//crit damage multiplier
	//pos = 4;
	//groupAttribute.AttributeList(pos).ValueShowType = EVST_Rate;
	//groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	// JO*(WJOSIJERg
//	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catCritDamageModificationRate;	
//	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catCritDamageModificationRate;
	// JO*(WJOSIJERg end

	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0.01;
	
}

void MMOAttributeAccountManager::initDefAttribute( INT groupId )
{
	INT itemNum = 6;

	FString strPath;
	StructAttributeGroup& groupAttribute = AttributeGroupList(groupId);

	for ( INT i = 0 ; i < itemNum ; i++ )
	{
		StructAttributeItem attributeItem;

		for ( INT j = 0 ; j < EPCT_MAX -1; j++ )
		{
			strPath = FString::Printf( TEXT("MMOGame.Player.AttriGroup%d_ItemName%d") , groupId , i , j );
			attributeItem.AttributeName[j] = MMOUI::ParseLocalizedPropertyPath( strPath );
			strPath = FString::Printf( TEXT("MMOGame.Player.AttriGroup%d_ItemTipsDesc%d") , groupId , i , j );
			attributeItem.TipsDesc[j] = MMOUI::ParseLocalizedPropertyPath( strPath );
		}

		groupAttribute.AttributeList.AddItem( attributeItem );
	}

	//agility
	INT pos = 0;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catAgility;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catAgility;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 1;

	//magic dodge
	pos = 1;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catMagDodge;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catMagDodge;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0.01;

	//crit reduction
	pos = 2;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catCritResist;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catCritResist;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0.01;

	//crit damage reduction
	pos = 3;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catCritDmgResist;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catCritDmgResist;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;

	//damage reduction
	pos = 4;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catDmgResist;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catDmgResist;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;

	//poison evasion
	pos = 5;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catPoisonResist;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catPoisonResist;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;

	//max life
	//INT pos = 0;
	//groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Decimal;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catMaxHp;	
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catMaxHp;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 1;
	//life regen
	//pos = 1;
	//groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Decimal;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catHpRecovery;	
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catHpRecovery;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 1;
	//physical defense
	//pos = 2;
	//groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catPhyDef;	
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = SAIP_PhyDamageReduce;
	
	//magic defense
	//pos = 3;
	//groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catMagDef;	
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = SAIP_MagDamageReduce;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 1;
	//block value
	// JO*(WJOSIJERg
	//pos = 4;
	//groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Int;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catBlockValue;	
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catBlockValue;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 1;
	////block rate
	//pos = 5;
	//groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catBlockRate;	
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catBlockRate;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0.01;
	//groupAttribute.AttributeList(pos).ValueShowType = EVST_Rate;
	////parry rate
	//pos = 6;
	//groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catParryRate;	
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catParryRate;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0.01;
	//groupAttribute.AttributeList(pos).ValueShowType = EVST_Rate;
	////dodge
	//pos = 7;
	//groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catDodge;	
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = SAIP_DodgeRate;
	////groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 1;
	//
	////tenacity
	//pos = 8;
	//groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catCritResistance;	
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = SAIP_CritReduceRate;
	////groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 1;
	// JO*(WJOSIJERg end
}

void MMOAttributeAccountManager::initMpAttribute( INT groupId )
{
	INT itemNum = 2;

	FString strPath;
	StructAttributeGroup& groupAttribute = AttributeGroupList(groupId);

	for ( INT i = 0 ; i < itemNum ; i++ )
	{
		StructAttributeItem attributeItem;

		for ( INT j = 0 ; j < EPCT_MAX -1; j++ )
		{
			strPath = FString::Printf( TEXT("MMOGame.Charactor.AttriGroup%d_ItemName%d_Class%d") , groupId , i , j );
			attributeItem.AttributeName[j] = MMOUI::ParseLocalizedPropertyPath( strPath );
			strPath = FString::Printf( TEXT("MMOGame.Charactor.AttriGroup%d_ItemTipsDesc%d_Class%d") , groupId , i , j );
			attributeItem.TipsDesc[j] = MMOUI::ParseLocalizedPropertyPath( strPath );
		}

		groupAttribute.AttributeList.AddItem( attributeItem );
	}

	//max mana
	INT pos = 0;
	groupAttribute.AttributeList(pos).DifferName = TRUE;
	groupAttribute.AttributeList(pos).DifferTips = TRUE;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catMaxMp;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = catMaxMp;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 1;
	//mana regen
	pos = 1;
	groupAttribute.AttributeList(pos).DifferName = TRUE;
	groupAttribute.AttributeList(pos).DifferTips = TRUE;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = SAIP_MpRecover;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = SAIP_MpRecover;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 1;
}

void MMOAttributeAccountManager::initOtherAttribute( INT groupId )
{
	
	INT itemNum = 7;

	FString strPath;
	StructAttributeGroup& groupAttribute = AttributeGroupList(groupId);

	for ( INT i = 0 ; i < itemNum ; i++ )
	{
		StructAttributeItem attributeItem;

		for ( INT j = 0 ; j < EPCT_MAX -1; j++ )
		{
			strPath = FString::Printf( TEXT("MMOGame.Player.AttriGroup%d_ItemName%d") , groupId , i , j );
			attributeItem.AttributeName[j] = MMOUI::ParseLocalizedPropertyPath( strPath );
			strPath = FString::Printf( TEXT("MMOGame.Player.AttriGroup%d_ItemTipsDesc%d") , groupId , i , j );
			attributeItem.TipsDesc[j] = MMOUI::ParseLocalizedPropertyPath( strPath );
		}
		groupAttribute.AttributeList.AddItem( attributeItem );
	}

	//potion life recovery
	INT pos = 0;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catDrugHpRecovery;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = 0;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;

	//potion mana recovery
	pos = 1;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Rate;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catDrugMpRecovery;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = 0;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;

	//pk value
	pos = 2;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Int;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = SAIP_PKValue;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = 0;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;

	//exp bonus
	//pos = 3;
	//groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Int;
	//groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catExpLootModRate;	
	//groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = 0;
	//groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;

	//fatigue
	/*pos = 1;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = SAIP_Fatigue;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = 0;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;*/

	//max bag weight
	pos = 3;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Bag;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Bag;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catBagWeightLmt;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = 0;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;

	//max equipment weight
	pos = 4;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Armor;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Armor;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catArmorWeightLmt;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = 0;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;

	//max wrist strength
	pos = 5;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_Weap;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_Weap;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = catWeaponWeightLmt;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = 0;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;

	//Double experience
	pos = 6;
	groupAttribute.AttributeList(pos).TipsValueShowType = EVST_ExpEtra;
	groupAttribute.AttributeList(pos).ValueShowType = EVST_ExpEtra;
	groupAttribute.AttributeList(pos).AttributeValueType[EPCT_PlayerClassA] = 0;	
	groupAttribute.AttributeList(pos).AttributeTypeA[EPCT_PlayerClassA] = 0;
	groupAttribute.AttributeList(pos).AttributeMulParaA[EPCT_PlayerClassA] = 0;
	
}

void MMOAttributeAccountManager::Init()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOAttributeAccountManager::Init"), 0);

	warnf(NAME_Log, TEXT("MMOAttributeAccountManager::Init >>>"));

	//attribute owner
	static MMOAttrListener_Mine attrMine;
	static MMOAttrListener_OtherPlayer attrOther;

	AttrOwnerTypeList.Empty();
	AttrOwnerTypeList.AddZeroed(AttrListenerType_Count);
	AttrOwnerTypeList(AttrListenerType_Mine) = &attrMine;
	AttrOwnerTypeList(AttrListenerType_OtherPlayer) = &attrOther;

	MMOAttrListener* CurAttrListener = NULL;

	//attribute bar
	AttributeGroupList.AddZeroed( Attribute_GroupNum );
	for ( INT i = 0 ; i < Attribute_GroupNum ; i++ )
	{
		FString stringPath = TEXT("MMOGame.Player.AttriGroup");
		stringPath += appItoa(i);
		AttributeGroupList(i).GroupName = MMOUI::ParseLocalizedPropertyPath( stringPath );
	}

	initBaseAttribute(0);
	initAttAttribute(1);
	initDefAttribute(2);
	//initMpAttribute(3);
	initOtherAttribute(3);
	//warrior, mage, assassin, hunter

	// 1.05 classes: the per-class columns above are only filled for A..E.
	// Lancer (G) shows the Warrior attribute layout, Sorcerer (H) the Mage layout.
	for ( INT g = 0 ; g < AttributeGroupList.Num() ; g++ )
	{
		for ( INT it = 0 ; it < AttributeGroupList(g).AttributeList.Num() ; it++ )
		{
			StructAttributeItem& attrItem = AttributeGroupList(g).AttributeList(it);
			const INT srcCls[2] = { EPCT_PlayerClassA, EPCT_PlayerClassB };
			const INT dstCls[2] = { EPCT_PlayerClassG, EPCT_PlayerClassH };
			for ( INT c = 0 ; c < 2 ; c++ )
			{
				attrItem.AttributeName[dstCls[c]]		= attrItem.AttributeName[srcCls[c]];
				attrItem.TipsDesc[dstCls[c]]			= attrItem.TipsDesc[srcCls[c]];
				attrItem.AttributeValueType[dstCls[c]]	= attrItem.AttributeValueType[srcCls[c]];
				attrItem.AttributeValueTypeB[dstCls[c]]	= attrItem.AttributeValueTypeB[srcCls[c]];
				attrItem.AttributeTypeA[dstCls[c]]		= attrItem.AttributeTypeA[srcCls[c]];
				attrItem.AttributeTypeB[dstCls[c]]		= attrItem.AttributeTypeB[srcCls[c]];
				attrItem.AttributeMulParaA[dstCls[c]]	= attrItem.AttributeMulParaA[srcCls[c]];
				attrItem.AttributeMulParaB[dstCls[c]]	= attrItem.AttributeMulParaB[srcCls[c]];
			}
		}
	}

	
	warnf(NAME_Log, TEXT("MMOAttributeAccountManager::Init <<<"));
}

void MMOAttributeAccountManager::SetAttrOwnerMode( AttrListenerType type )
{
	//for ( INT i = 0 ; i < AttrOwnerTypeList.Num() ; i++ )
	//{
	//	if( AttrOwnerTypeList(i) && AttrOwnerTypeList(i)->GetType() == type )
	//	{
			CurAttrListener = AttrOwnerTypeList(type);
	//	}
	//}
				
}

//void MMOAttributeAccountManager::ClearAll()
//{
//	
//}

INT MMOAttributeAccountManager::GetAttributeById(INT attrId)
{
	if (CurAttrListener)

		return CurAttrListener->GetAttribute(attrId);
	else
		return 0;
}


void MMOAttributeAccountManager::setExpEtra(INT exp)
{
	if (exp > 0)
		expEtra = exp;
	else
		expEtra = 0;
}