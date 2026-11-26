#include "StdAfx.h"
#include "attribute_def.h"

//------------------------------------------------------------------------
void UActorAttribute::Reset()
{
	HP = 0;
	MP = 0;
}
//------------------------------------------------------------------------
FLOAT UActorAttribute::GetHPPercent()
{
	if( GetMaxHp() <= 0 || GetCurHp() <= 0 )
	{
		return 0.001;
	}

	return (FLOAT)GetCurHp() / GetMaxHp();
}
//------------------------------------------------------------------------
FLOAT UActorAttribute::GetMPPercent()
{
	if( GetMaxMp() <= 0 || GetCurMp() <= 0 )
	{
		return 0.001;
	}

	return (FLOAT)GetCurMp() / GetMaxMp();
}
//------------------------------------------------------------------------
void UActorAttribute::SetHP( INT val )
{
	HP = val;
}
//------------------------------------------------------------------------
void UActorAttribute::SetMaxHP( INT val )
{
	MaxHP = val;
	if( AttributeList.Num() > catMaxHp )
	{
		AttributeList(catMaxHp) = val;
		if(GetCurHp()>val)
		{
			SetHP(val);
		}
	}
	
}
//------------------------------------------------------------------------
void UActorAttribute::SetMP( INT val )
{
	MP = val;
}
//------------------------------------------------------------------------
void UActorAttribute::SetMaxMP( INT val )
{
	MaxMP = val;

	if( AttributeList.Num() > catMaxMp )
	{
		AttributeList(catMaxMp) = val;
		if(GetCurMp()>val)
		{
			SetMP(val);
		}
	}

}
//------------------------------------------------------------------------
int UActorAttribute::GetCurMp()
{
	return MP;
}

int UActorAttribute::GetMaxMp()
{
	if(GetAttribute(catMaxMp) < 0)
		return MaxMP;
	return GetAttribute(catMaxMp);
}

int UActorAttribute::GetCurHp()
{
	return HP;
}

int UActorAttribute::GetMaxHp()
{
	if(GetAttribute(catMaxHp) <= 0)
		return MaxHP;
	return GetAttribute(catMaxHp);
}

void UActorAttribute::SetPkValue(int val)
{
	PkValue = val;
}

int  UActorAttribute::GetPkValue()
{
	return PkValue;
}

int UActorAttribute::GetAttribute(INT AttrID)
{
	if( AttrID > 0 && AttrID < AttributeList.Num()  )
		return AttributeList(AttrID);
	return 0;
}

void UActorAttribute::SetReputation(INT Index,INT ForceID,INT ReputationData)
{
	INT i;
	FReputation buf(EC_EventParm);
	UBOOL AddNew = TRUE;

	for(i = ReputationArray.Num() - 1; i >= 0; --i)
	{
		if(ReputationArray(i).ForceID == ForceID){
			ReputationArray(i).ReputationData = ReputationData; 
			AddNew = FALSE;
			break;
		}
	}

	if(AddNew){
		buf.ForceID = ForceID;
		buf.ReputationData = ReputationData; 
		ReputationArray.AddItem(buf);
	}
}

INT UActorAttribute::GetReputation(INT ForceID)
{
	for(INT i = ReputationArray.Num() - 1; i >= 0; --i)
	{
		if(ReputationArray(i).ForceID == ForceID){
			return ReputationArray(i).ReputationData; 
		}
	}

	return 0;
}


void UActorAttribute::SetActiveState(INT Index, INT ForceID, UBOOL ActiveState)
{
	for(INT i = ReputationArray.Num() - 1; i >= 0; --i)
	{
		if(ReputationArray(i).ForceID == ForceID){
			ReputationArray(i).ActiveState = ActiveState; 
			break;
		}
	}
}

UBOOL UActorAttribute::GetActiveState(INT ForceID)
{
	for(INT i = ReputationArray.Num() - 1; i >= 0; --i)
	{
		if(ReputationArray(i).ForceID == ForceID){
			return ReputationArray(i).ActiveState;
		}
	}
	return FALSE;
}

void UActorAttribute::SetAwardState(INT Index, INT ForceID, UBOOL AwardState)
{
	for(INT i = ReputationArray.Num() - 1; i >= 0; --i)
	{
		if(ReputationArray(i).ForceID == ForceID){
			ReputationArray(i).AwardState = AwardState; 
			break;
		}
	}
}

UBOOL UActorAttribute::GetAwardState(INT ForceID)
{
	for(INT i = ReputationArray.Num() - 1; i >= 0; --i)
	{
		if(ReputationArray(i).ForceID == ForceID){
			return ReputationArray(i).AwardState;
		}
	}
	return FALSE;
}