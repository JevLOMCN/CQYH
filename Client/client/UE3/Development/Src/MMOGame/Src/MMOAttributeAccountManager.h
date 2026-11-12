
#ifndef _MMOAttributeAccountManager_H_
#define _MMOAttributeAccountManager_H_

#include "MMOSingleton.h"
#include "MMOUtil.h"
#include "MMOGameEvent.h"
#include "MMOGameUIClasses.h"

#include "MMOAttrListener.h"

//special formula
enum ESpecialAttributeInPanel
{
	SAIP_PKValue = catTotal, //pk value is special
	SAIP_Fatigue, //fatigue is special
	SAIP_PhyDamageReduce, //physical damage reduction
	SAIP_MagDamageReduce,
	SAIP_CritRate, //crit rate
	SAIP_DodgeRate, //dodge rate
	SAIP_HitRate, //hit rate
	SAIP_AttSpeedAdd, //attack speed bonus
	SAIP_CritReduceRate, //crit resistance rate
	SAIP_MpRecover,
};

enum EValueShowType
{
	EVST_Int, //integer
	EVST_Att, //attack power is special
	EVST_Decimal, //2 decimal places
	EVST_Rate, //2 decimal places %
	EVST_HP,
	EVST_MP,
	EVST_Bag,
	EVST_Weap,
	EVST_Armor,
	EVST_ExpEtra,
};

struct StructAttributeItem
{
	//logically should be EPCT_Max,
	FString AttributeName[EPCT_MAX];
	FString TipsDesc[EPCT_MAX];

	//attribute value, 0 if it needs to be calculated
	INT AttributeValueType[EPCT_MAX];
	//special range attribute
	INT AttributeValueTypeB[EPCT_MAX];

	//related attribute
	INT AttributeTypeA[EPCT_MAX];
	INT AttributeTypeB[EPCT_MAX];

	//divide by 100
	FLOAT AttributeMulParaA[EPCT_MAX];
	FLOAT AttributeMulParaB[EPCT_MAX];

	UBOOL DifferName;
	UBOOL DifferValueType;
	UBOOL DifferTips;
	UBOOL DifferTipsValueType;
	UBOOL DifferTipsValuePara;
	EValueShowType ValueShowType;
	EValueShowType TipsValueShowType;
	
	StructAttributeItem()
	{
		Reset();
	}

	void Reset()
	{
		appMemzero( this , sizeof(StructAttributeItem) );
	}



};

struct StructAttributeGroup
{
	FString GroupName;
	TArray< StructAttributeItem > AttributeList;
};



typedef TArray< StructAttributeGroup > AttributeGroupArray;

class MMOAttributeAccountManager : public SingletonMeyers<MMOAttributeAccountManager>
{
public:

	MMOAttributeAccountManager();
	virtual ~MMOAttributeAccountManager();

	void Init();
	void Shutdown();
	AttributeGroupArray& GetAttributeGroupArray() { return AttributeGroupList; }

	const FString GetAttributeName( INT groupId , INT index ) const ;
	const FString GetAttributeValue( INT groupId , INT index ) const ;
	const FString GetAttributeDesc( INT groupId , INT index ) const ;

	void SetAttrOwnerMode( AttrListenerType type );

	//whether it is an attribute shown in the attribute UI
	UBOOL IsRelationAttribute( INT groupId , INT index , INT Attri );
	
	//whether it is an attribute used in combat/life calculation
	UBOOL IsBattleAttribute( INT Attri );


	FString GetDamagePoint();
	FString GetLifePoint();
	FString GetBattlePoint();

	void SetBattlePoint( INT bp ){ BattlePoint = bp; }

	INT GetOwnerAttribute( INT attriId , AttrListenerType type );

	INT GetAttributeById(INT attrId);

	void setExpEtra(INT exp);
	INT getExpEtra(){return expEtra;}

#define Attribute_GroupNum 4

protected:

	//
	UBOOL IsRelationSpecialAttribute( INT Special , INT Attri );

	void initBaseAttribute( INT groupId );
	void initAttAttribute( INT groupId );
	void initDefAttribute( INT groupId );
	void initMpAttribute( INT groupId );
	void initOtherAttribute( INT groupId );

	void initBaseAttributeByHighAndLow(CHAR_ATTR_TYPE typeHigh, CHAR_ATTR_TYPE typeLow,INT groupId = 0, INT pos = 0);

	//INT ClassId;
	INT BattlePoint;
	AttributeGroupArray AttributeGroupList;

	TArray<MMOAttrListener*> AttrOwnerTypeList;
	MMOAttrListener* CurAttrListener;

	INT expEtra;

};

#endif