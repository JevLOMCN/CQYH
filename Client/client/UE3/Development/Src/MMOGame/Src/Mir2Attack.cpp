#include "StdAfx.h"
#include "Mir2Attack.h"
#include "FSMState.h"
#include "FSMIO.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "AbilityProcessor.h"
#include "MMONavigationManager.h"
#include "ScaleformEngine.h"
#include "UITextParser.h"
#include "MMOSkillManager.h"
#include "MMOChatManager.h"
#include "UnPath.h"
#include "MMOAssistData.h"

using namespace FSM;
extern UBOOL g_PreMovetoPoint;
UBOOL g_bMineHitSound;

AttackClassX::AttackClassX()
{
	ClearAbilityFlag();
	_pMainPlayer=MMOFacade::getSingleton().GetMainPlayer();
	_fDistance = 0.0f;
	Init();
}

void AttackClassX::ClearAbilityFlag()
{
	for(INT i=0;i<EAT_FLAG_MAX;i++)
	{
		AbilityFlag[i] = FALSE;
	}
}

void AttackClassX::SetAbilityFlag(INT FlagType)
{
	if(FlagType>=0 && FlagType<EAT_FLAG_MAX)
	{
		AbilityFlag[FlagType] = TRUE;
	}
}

INT AttackClassX::GetBlockActorNumBetweenDesireMMOActor()
{
	if (FSM::Char.MainPlayer&&FSM::Target.DesireMMOActor)
	{
		FVector Src = FSM::Char.MainPlayer->ServerLocation;
		FVector Des = FSM::Target.DesireMMOActor->ServerLocation;
		FLOAT Dis = (Des - Src).Size2D();
		if (Dis > 210.0f)//there is a grid cell between target and self
		{
			ZONE_DATA* pSrcZone = NULL;
			GRID_DATA* pSrcGrid = NULL;
			INT		   BlockActorNum = 0;
			INT Index = gServerGridWorld.OptimizeDir_Mir2(_pMainPlayer->ServerLocation, Des);
			FVector pos1 = gServerGridWorld.GetNearbyGridPos(_pMainPlayer->ServerLocation, Index >= 0 ? Index : Index + 8, 1.0f);
			BlockActorNum = GetGridAttackbleActor(pos1);
			return BlockActorNum;
		}
	}
	return 0;
}

INT AttackClassX::GetGridAttackbleActor(FVector Des)
{
	//FVector pos1 = gServerGridWorld.GetNearbyGridPos(_pMainPlayer->ServerLocation,Index[k]);
	ZONE_DATA* pSrcZone = NULL;
	GRID_DATA* pSrcGrid = NULL;	
	INT		   AttackbleActorNum = 0;
	gServerGridWorld.GetZoneAndGrid(Des, pSrcZone, pSrcGrid);
	if(pSrcGrid && pSrcGrid->bOutOfPoly==FALSE)
	{
		AActor** ppActorList=NULL;
		INT nNum = 0;

		ppActorList = gServerGridWorld.GetActorList(pSrcGrid,nNum);
		if(ppActorList==NULL)
		{
			return 0;
		}

		for(INT i=0;i<nNum;i++)
		{
			AActor* pActor = *(ppActorList+i);
			
			//Half Moon and Assassinate checks, regardless of whether attack is possible
			if(pActor && pActor->IsA(AMMOPlayer::StaticClass()))
			{
				AttackbleActorNum++;
				continue;
			}
			else if(pActor && pActor->IsA(ANPC::StaticClass()))
			{
				AttackbleActorNum++;
				continue;
			}

			//AMMOActor* pMMOActor = Cast<AMMOActor>(pActor);
			//UBOOL isAttackble = MMOFacade::getSingleton().IsAttackable(pMMOActor);
			//if(pMMOActor&&isAttackble)
			//{
			//	AttackbleActorNum++;
			//	continue;
			//}
		}
	}
	return AttackbleActorNum;
}

void  AttackClassX::UpdateDynamicPos(FVector DesPos, UBOOL b3x3)
{
	_pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	_fDistance = 0.0f;
	_DesPos	= DesPos;
	if(_pMainPlayer)
	{
		FVector     D=_pMainPlayer->ServerLocation;
		ConvertXYToServer(D.X,D.Y);
		FVector     S=_DesPos;
		ConvertXYToServer(S.X,S.Y);
		FVector disVector = D-S;
		_fDistance_GridX = disVector.X;
		_fDistance_GridY = disVector.Y;
		//_fDistance = (_pMainPlayer->ServerLocation-_DesPos).Size2D();
		if(b3x3==FALSE)
		{
			_fDistance = max(abs(_fDistance_GridX),abs(_fDistance_GridY));
		}
		else
		{
			_fDistance = max(abs(_fDistance_GridX),abs(_fDistance_GridY))-128.0f;
		}

		if(_fDistance_GridX>-16.0f&&_fDistance_GridX<16.0f)
		{
			_in8Dir = TRUE;
		}
		else if(_fDistance_GridY>-16.0f&&_fDistance_GridY<16.0f)
		{
			_in8Dir = TRUE;
		}
		else{
			if(b3x3==FALSE)
			{
				FLOAT fBi = abs(_fDistance_GridX/_fDistance_GridY);
				if(fBi>0.9f&&fBi<1.1f)
				{
					_in8Dir = TRUE;
				}
				else
				{
					_in8Dir = FALSE;
				}
			}
			else
			{
				_in8Dir = TRUE;//if the target is a 3*3 area, it is considered mostly in the eight directions
			}
		}
	}
}

UBOOL AttackClassX::GetBanYueCondition(FVector Des,INT nRange)
{
	if(_pMainPlayer)
	{	
		INT Index[5];
		Index[0] = gServerGridWorld.OptimizeDir_Mir2(_pMainPlayer->ServerLocation,Des);
		if(Index[0]<0)Index[0]+=8;
		Index[1] = (Index[0]+1)%8;
		Index[2] = (Index[0]+7)%8;
		Index[3] = (Index[0]+2)%8;
		Index[4] = (Index[0]+6)%8;
		INT NearbyAttackbleActor = 0;
		//Five adjacent grid cells
		for(INT k=0;k<5;k++){
			FVector pos1 = gServerGridWorld.GetNearbyGridPos(_pMainPlayer->ServerLocation,Index[k]);
			NearbyAttackbleActor += GetGridAttackbleActor(pos1);
			if(NearbyAttackbleActor>=2)
			{
				return TRUE;
			}
		}

		if(nRange==1)return FALSE;
		//Outer cells of the five adjacent grid cells
		FVector LinkPos[5];
		for(INT k=0;k<5;k++){
			FVector pos1 = LinkPos[k] = gServerGridWorld.GetNearbyGridPos(_pMainPlayer->ServerLocation,Index[k],2.0f);
			NearbyAttackbleActor += GetGridAttackbleActor(pos1);
			if(NearbyAttackbleActor>=2)
			{
				return TRUE;
			}
		}
		//Four outer gap cells
		INT Link[4][2] = {0,1,	0,2,	1,3,	2,4  };
		for(INT k=0;k<4;k++){
			FVector pos1 = LinkPos[Link[k][0]];
			FVector pos2 = LinkPos[Link[k][1]];
			NearbyAttackbleActor += GetGridAttackbleActor(0.5f*(pos1+pos2));
			if(NearbyAttackbleActor>=2)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

UBOOL AttackClassX::GetCiShaCondition(FVector Des,UBOOL bEx)
{
	if(_pMainPlayer)
	{	
		ZONE_DATA* pSrcZone = NULL;
		GRID_DATA* pSrcGrid = NULL;	
		INT		   NearbyAttackbleActor = 0;
		INT Index = gServerGridWorld.OptimizeDir_Mir2(_pMainPlayer->ServerLocation,Des);
		FVector pos1 = gServerGridWorld.GetNearbyGridPos(_pMainPlayer->ServerLocation,Index>=0?Index:Index+8 ,2.0f);
		NearbyAttackbleActor += GetGridAttackbleActor(pos1);
		if(NearbyAttackbleActor>0)
		{
			if(FSM::Target.DesireMMOActor)
			{
				FVector dis = _pMainPlayer->ServerLocation - FSM::Target.DesireMMOActor->ServerLocation;
				FLOAT fDis = dis.Size2D();
				if(fDis>362.0f+10.0f || (fDis>271.0f&&fDis<324.0f))
				{
					return FALSE;
				}
			}
			return TRUE;
		}
		
		if(bEx && FSM::Target.DesireMMOActor && FSM::Target.DesireMMOActor->isMoving && Index%2==0)	//takes effect along the axis direction
		{
			FVector pos2 = gServerGridWorld.GetNearbyGridPos(_pMainPlayer->ServerLocation,Index>=0?Index:Index+8,3.0f);
			INT NearbyAttackbleActor2 = GetGridAttackbleActor(pos2);
			FVector dis = _pMainPlayer->ServerLocation - FSM::Target.DesireMMOActor->Location;
			if(NearbyAttackbleActor2>0 && dis.Size2D()<256.0f+10.0f)
			{
				return TRUE;
			}
		}


	}
	return FALSE;
}

UBOOL AttackClassA::InPrimaryAbilityRange(FVector DesPos, UBOOL b3x3)
{
	UpdateDynamicPos(DesPos,b3x3);

	if(_fDistance<ONE_GRID_RADIUS)
	{
		return TRUE;
	}
	else{
		if(FSM::Target.DesireMMOActor && FSM::Target.DesireMMOActor->isMoving==FALSE)
		{
			return FALSE;
		}

		UMMOAbility* pResAbility = NULL;
		if(AbilityFlag[EAT_FLAG_ZHURI])
		{
			pResAbility = _CanUseZhuRi();
			if(pResAbility)
			{
				return TRUE;
			}
			return FALSE;
		}

		if(AbilityFlag[EAT_FLAG_LIEHUO])
		{
			pResAbility = _CanUseLieHuo();
			if(pResAbility)
			{
				return TRUE;
			}
			return FALSE;
		}

		if(AbilityFlag[EAT_FLAG_CISHA])
		{
			pResAbility = _CanUseCiSha(TRUE);
			if(pResAbility)
			{
				INT num = GetBlockActorNumBetweenDesireMMOActor();
				if (num == 0)
				{
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
		}

		return FALSE;
	}
}

UBOOL AttackClassA::DoAIAbility()
{
	if(	FSM::Char.MainPlayer		&&
		FSM::Char.Controller		&&
		FSM::CDTime.DoAIAbilityCD <=0.0f &&
		FSM::Char.IsDead==FALSE		&&
		FSM::CDTime.SwitchRespondTime<=0.0f )
	{
		if(FSM::Ctrl.ctrlFlag_ZDZR && AbilityFlag[EAT_FLAG_ZHURI]==FALSE)
		{
			// #21253
			//UMMOAbility *pZhuRiSwitch = MMOSkillManager::getSingleton().GetEquippedSkillBySkillID(1038);
			UMMOAbility *pZhuRiSwitch = MMOFacade::getSingleton().FindMyUsableSkill(1038);
			if(pZhuRiSwitch && pZhuRiSwitch->CanUseManaCheck(_pMainPlayer))
			{
				if(pZhuRiSwitch->bCoolDown==FALSE||pZhuRiSwitch->ClientCommonAttribute.ModifyCD>0.001f)
				{
					if(FSM::Char.Controller)
					{
						FSM::Char.Controller->RequestSwitchSpell(pZhuRiSwitch->SpellCommonAttribute.Id);
						FSM::CDTime.DoAIAbilityCD = 1.0f;
					}
				}
			}
		}

		if(FSM::Ctrl.ctrlFlag_ZDLH && AbilityFlag[EAT_FLAG_LIEHUO]==FALSE)
		{
			// #21253
			//UMMOAbility *pLieHuoSwitch = MMOSkillManager::getSingleton().GetEquippedSkillBySkillID(1036);
			UMMOAbility *pLieHuoSwitch = MMOFacade::getSingleton().FindMyUsableSkill(1036);
			if(pLieHuoSwitch && pLieHuoSwitch->CanUseManaCheck(_pMainPlayer))
			{
				if(pLieHuoSwitch->bCoolDown==FALSE||pLieHuoSwitch->ClientCommonAttribute.ModifyCD>0.001f)
				{
					if(FSM::Char.Controller)
					{
						FSM::Char.Controller->RequestSwitchSpell(pLieHuoSwitch->SpellCommonAttribute.Id);
						FSM::CDTime.DoAIAbilityCD = 1.0f;
					}
				}
			}
		}
	}
	return FALSE;
}

UMMOAbility* AttackClassA::_CanUseZhuRi()//Sun Chasing Swordplay
{
	UBOOL bZhuRi = AbilityFlag[EAT_FLAG_ZHURI];
	if(bZhuRi)//Sun Chasing Swordplay
	{
		UMMOAbility *pZhuRiSwitch = MMOFacade::getSingleton().FindMyUsableSkill(1038);
		if(pZhuRiSwitch)
		{
			UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( 1434, pZhuRiSwitch->SpellCommonAttribute.Level, pZhuRiSwitch->SpellCommonAttribute.RuneSlotID );
			UMMOAbility *pZhuRi = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
			if(pZhuRi && pZhuRi->CanUseManaCheck(_pMainPlayer))
			{
				if(FSM::Target.DesireMMOActor)
				{
					if(	_in8Dir ) 
					{
						if(	(_fDistance_GridX<ONE_GRID_WIDTH*4.0f+16.0f && _fDistance_GridX>ONE_GRID_WIDTH*-4.0f-16.0f) &&
							(_fDistance_GridY<ONE_GRID_WIDTH*4.0f+16.0f && _fDistance_GridY>ONE_GRID_WIDTH*-4.0f-16.0f) )
						{
							return pZhuRi;
						}
					}
					else
					{
						if(pZhuRi->SpellCommonAttribute.RuneSlotID==1)
						{
							if((_fDistance_GridX<ONE_GRID_WIDTH*3.0f+16.0f && _fDistance_GridX>ONE_GRID_WIDTH*-3.0f-16.0f)&&
								(_fDistance_GridY<ONE_GRID_WIDTH*3.0f+16.0f && _fDistance_GridY>ONE_GRID_WIDTH*-3.0f-16.0f))
							{
								return pZhuRi;
							}
						}
					}
				}
				else
				{
					return pZhuRi;
				}
			}
		}
	}
	return NULL;
}

UMMOAbility* AttackClassA::_CanUseLieHuo()//Flaming Swordplay
{
	UBOOL bLieHuo = AbilityFlag[EAT_FLAG_LIEHUO];
	if(bLieHuo)//Flaming Swordplay
	{
		if(FSM::Target.DesireMMOActor)
		{
			UMMOAbility *pLieHuoSwitch = MMOFacade::getSingleton().FindMyUsableSkill(1036);
			if(pLieHuoSwitch)
			{
				UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( 1433, pLieHuoSwitch->SpellCommonAttribute.Level, pLieHuoSwitch->SpellCommonAttribute.RuneSlotID );
				UMMOAbility *pLieHuo = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
				if(pLieHuo && pLieHuo->CanUseManaCheck(_pMainPlayer))
				{

					if(pLieHuo->SpellCommonAttribute.RuneSlotID==1)
					{
						if((_fDistance_GridX<ONE_GRID_WIDTH*3.0f+16.0f && _fDistance_GridX>ONE_GRID_WIDTH*-3.0f-16.0f)&&
							(_fDistance_GridY<ONE_GRID_WIDTH*3.0f+16.0f && _fDistance_GridY>ONE_GRID_WIDTH*-3.0f-16.0f))
						{
							return pLieHuo;
						}
					}
					else
					{
						if(_fDistance<ONE_GRID_RADIUS)
						{
							return pLieHuo;
						}
					}
				}
			}
		}
	}
	return NULL;
}

UMMOAbility* AttackClassA::_CanUseGongSha()//Slaying Swordplay
{
	UBOOL bGongSha = FALSE;
	if(FSM::Char.MainPlayer&&FSM::Target.DesireMMOActor)
	{
		if(FSM::Char.MainPlayer->GetActorAttributeValue(catPHitCountDown)-1 == FSM::Char.MainPlayer->GetActorAttributeValue(catPHitNum))
		{
			if(_fDistance<ONE_GRID_RADIUS)
			{
				bGongSha = TRUE;
			}
		}
	}

	if(bGongSha)//Slaying Swordplay
	{
		UMMOAbility *pGongShaSwitch = MMOFacade::getSingleton().FindMyUsableSkill(1032);
		if(pGongShaSwitch)
		{
			UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( 1430, pGongShaSwitch->SpellCommonAttribute.Level, pGongShaSwitch->SpellCommonAttribute.RuneSlotID );
			UMMOAbility *pGongSha = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
			if(pGongSha && pGongSha->CanUseManaCheck(_pMainPlayer))
			{
				return pGongSha;
			}
		}
	}
	return NULL;
}

UMMOAbility* AttackClassA::_CanUseBanYue()//Half Moon Swordplay
{
	UBOOL bBanYue = AbilityFlag[EAT_FLAG_BANYUE];
	UBOOL bBanYueUAttack = FALSE;
	if(bBanYue)//Half Moon Swordplay
	{
		//UMMOAbility* pBanYue = MMOFacade::getSingleton().FindMyUsableSkill(1432);
		UMMOAbility *pBanYueSwitch = MMOFacade::getSingleton().FindMyUsableSkill(1034);	//GetEquippedSkillBySkillID( 1034 );
		if(pBanYueSwitch)
		{
			UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( 1432, pBanYueSwitch->SpellCommonAttribute.Level, pBanYueSwitch->SpellCommonAttribute.RuneSlotID );
			UMMOAbility *pBanYue = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );

			if(pBanYue && pBanYue->CanUseManaCheck(_pMainPlayer))
			{
				FVector AttackPos = FSM::Char.Controller->MouseHitWorldLocation;
				if(FSM::Target.DesireMMOActor){
					AttackPos = FSM::Target.DesireMMOActor->ServerLocation;
					UBOOL bBanYueAi = FSM::Ctrl.ctrlFlag_ZNBY;
					if(_fDistance<ONE_GRID_RADIUS && bBanYueAi==FALSE)
					{
						return pBanYue;
					}
				}

				if(pBanYueSwitch->SpellCommonAttribute.RuneSlotID==1)
				{
					bBanYueUAttack = GetBanYueCondition(AttackPos,2.0f);
				}
				else
				{
					bBanYueUAttack = GetBanYueCondition(AttackPos,1.0f);
				}
				if(bBanYueUAttack)
				{
					return pBanYue;
				}
			}
		}
	}
	return NULL;
}

UMMOAbility* AttackClassA::_CanUseCiSha(UBOOL bEx)//Assassinate Swordplay
{
	UBOOL bCiSha = AbilityFlag[EAT_FLAG_CISHA];
	
	if(bCiSha)//Assassinate Swordplay
	{

		UMMOAbility *pCiShaSwitch = MMOFacade::getSingleton().FindMyUsableSkill(1033);//GetEquippedSkillBySkillID(1033);
		if(pCiShaSwitch)
		{
			UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( 1431, pCiShaSwitch->SpellCommonAttribute.Level, pCiShaSwitch->SpellCommonAttribute.RuneSlotID );
			UMMOAbility *pCiSha = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
			
			if(pCiSha && pCiSha->CanUseManaCheck(_pMainPlayer))
			{
				FVector AttackPos = FSM::Char.Controller->MouseHitWorldLocation;
				if(FSM::Target.DesireMMOActor){
					AttackPos = FSM::Target.DesireMMOActor->ServerLocation;
				}
				UBOOL bCiShattack = GetCiShaCondition(AttackPos,bEx);

				if(bCiShattack)
				{
					return pCiSha;
				}
			}
		}
	}

	UBOOL bDaoDaoCiSha = FSM::Ctrl.ctrlFlag_DDCS ;
	if(bDaoDaoCiSha)//Every-blade Assassinate
	{
		UMMOAbility *pCiShaSwitch = MMOFacade::getSingleton().FindMyUsableSkill(1033);//GetEquippedSkillBySkillID(1033);
		if(pCiShaSwitch)
		{
			UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( 1431, pCiShaSwitch->SpellCommonAttribute.Level, pCiShaSwitch->SpellCommonAttribute.RuneSlotID );
			UMMOAbility *pCiSha = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
			if(pCiSha && pCiSha->CanUseManaCheck(_pMainPlayer))
			{
				if(FSM::Target.DesireMMOActor)
				{
					if(_fDistance<ONE_GRID_RADIUS)
					{
						return pCiSha;
					}
				}
			}
		}
	}
	return NULL;
}

UMMOAbility* AttackClassA::_CanUsePuGong()
{
	if(FSM::Target.DesireMMOActor)
	{
		if(_fDistance<ONE_GRID_RADIUS)
		{
			UMMOAbility* pResAbility = MMOFacade::getSingleton().FindMyUsableSkill(1030);
			if(pResAbility && pResAbility->CanUseManaCheck(_pMainPlayer))
			{
				return pResAbility;
			}
		}
	}
	else
	{	
		UMMOAbility* pResAbility = MMOFacade::getSingleton().FindMyUsableSkill(1030);
		if(pResAbility && pResAbility->CanUseManaCheck(_pMainPlayer))
		{
			return pResAbility;
		}
	}
	return NULL;
}

UMMOAbility* AttackClassA::GetPrimaryAbility()
{
	if(FSM::Char.Controller==NULL)
	{
		return NULL;
	}
	FVector AttackPos = FSM::Char.Controller->MouseHitWorldLocation;
	if(FSM::Target.DesireMMOActor){
		AttackPos = FSM::Target.DesireMMOActor->ServerLocation;
		UpdateDynamicPos(AttackPos,FSM::Target.DesireMMOActor->BodySize>=E_body_3x3);
	}
	else{
		UpdateDynamicPos(AttackPos,FALSE);
	}

	UMMOAbility* pResAbility = NULL;
	pResAbility = _CanUseZhuRi();
	if(pResAbility)
	{
		return pResAbility;
	}


	pResAbility = _CanUseLieHuo();
	if(pResAbility)
	{
		return pResAbility;
	}
	pResAbility = _CanUseGongSha();
	if(pResAbility)
	{
		return pResAbility;
	}
	pResAbility = _CanUseBanYue();
	if(pResAbility)
	{
		return pResAbility;
	}
	pResAbility = _CanUseCiSha(TRUE);
	if(pResAbility)
	{
		return pResAbility;
	}
	pResAbility = _CanUsePuGong();
	if(pResAbility)
	{
		return pResAbility;
	}

	//Judge again using last
	if(FSM::Target.DesireMMOActor && FSM::Target.DesireMMOActor->isMoving)
	{
		AttackPos = FSM::Target.DesireMMOActor->LastServerLocation;
		UpdateDynamicPos(AttackPos,FSM::Target.DesireMMOActor->BodySize>=E_body_3x3);
	
		pResAbility = _CanUseZhuRi();
		if(pResAbility)
		{
			return pResAbility;
		}

		pResAbility = _CanUseLieHuo();
		if(pResAbility)
		{
			return pResAbility;
		}
		pResAbility = _CanUseGongSha();
		if(pResAbility)
		{
			return pResAbility;
		}
		pResAbility = _CanUseBanYue();
		if(pResAbility)
		{
			return pResAbility;
		}
		//Task #22162, Assassinate no longer checks LastServerLocation, everything else unchanged
		//pResAbility = _CanUseCiSha(TRUE);
		//if(pResAbility)
		//{
		//	return pResAbility;
		//}
	}

	//Normal attack
	pResAbility = MMOFacade::getSingleton().FindMyUsableSkill(1030);
	if(pResAbility && pResAbility->CanUseManaCheck(_pMainPlayer))
	{
		return pResAbility;
	}
	return NULL;
}

void AttackClassA::Update(FLOAT fDeltaTime)
{

}


UBOOL AttackClassB::InPrimaryAbilityRange(FVector DesPos, UBOOL b3x3)
{
	if(FSM::Char.Controller==NULL)
	{
		return NULL;
	}
	FVector AttackPos = FSM::Char.Controller->MouseHitWorldLocation;
	if(FSM::Target.DesireMMOActor){
		AttackPos = FSM::Target.DesireMMOActor->ServerLocation;
	}
	UpdateDynamicPos(AttackPos,b3x3);

	if(_fDistance<ONE_GRID_RADIUS)
	{
		return TRUE;
	}
	return FALSE;
}

UBOOL AttackClassB::DoAIAbility()
{
	if(	FSM::Char.MainPlayer		&&
		FSM::Char.Controller		&&
		FSM::Char.AbilityActionNum==0	&&	
		FSM::Char.MainPlayer->isMoving==FALSE &&
		FSM::CDTime.DoAIAbilityCD <=0.0f &&
		FSM::Char.IsDead==FALSE &&
		(FSM::Ability.UsableAbility==NULL ||FSM::Ability.AbilityEnable)&&
		FSM::Char.IsDead==FALSE	)
	{
		if(FSM::Ctrl.ctrlFlag_ZDKD && AbilityFlag[EAT_FLAG_MOFADUN]==FALSE)
		{
			// #21253
			//UMMOAbility *pMoFaDun = MMOSkillManager::getSingleton().GetEquippedSkillBySkillID(2535);
			UMMOAbility *pMoFaDun = MMOFacade::getSingleton().FindMyUsableSkill(2535);
			if(pMoFaDun)
			{
				if(pMoFaDun->bCoolDown==FALSE||pMoFaDun->ClientCommonAttribute.ModifyCD>0.001f)
				{
					if(pMoFaDun->CanUseManaCheck(FSM::Char.MainPlayer))
					{
						FSM::CDTime.DoAIAbilityCD = 1.0f;
						FSM::Char.Controller->eventClearAllCmd();
						if(FSM::Target.CurTargetMMOActor)
						{
							FSM::Char.Controller->eventAddCmdUseAbility(pMoFaDun,FSM::Target.CurTargetMMOActor,FSM::Target.CurTargetMMOActor->Location,FALSE);
						}
						else{
							FSM::Char.Controller->eventAddCmdUseAbility(pMoFaDun,		NULL,	FSM::Target.CurTargetPos);
						}
						return TRUE;
					}
					else
					{
						FSM::CDTime.DoAIAbilityCD = 0.5f;
					}
				}
			}
		}
	}
	return FALSE;
}

void AttackClassB::Update(FLOAT fDeltaTime)
{

}

UBOOL AttackClassE::DoAIAbility()
{
	if(	FSM::Char.MainPlayer		&&
		FSM::Char.Controller		&&
		FSM::Char.AbilityActionNum==0	&&	
		FSM::Char.MainPlayer->isMoving==FALSE &&
		FSM::CDTime.DoAIAbilityCD <=0.0f &&
		FSM::Char.IsDead==FALSE &&
		(FSM::Ability.UsableAbility==NULL ||FSM::Ability.AbilityEnable)&&
		FSM::Char.IsDead==FALSE	)
	{
		if(FSM::Ctrl.ctrlFlag_ZDYS && AbilityFlag[EAT_FLAG_YINSHEN]==FALSE)
		{
			// #21253
			//UMMOAbility *pYinShen = MMOSkillManager::getSingleton().GetEquippedSkillBySkillID(3009);
			UMMOAbility *pYinShen = MMOFacade::getSingleton().FindMyUsableSkill(3009);
			if(pYinShen)
			{
				if(pYinShen->bCoolDown==FALSE||pYinShen->ClientCommonAttribute.ModifyCD>0.001f)
				{
					FSM::CDTime.DoAIAbilityCD = 0.5f;

					UBOOL bRes = Mir2Attack::getSingleton().GetAbilityItemCostCondition(pYinShen);
					if(bRes==FALSE)
					{
						FSM::CDTime.DoAIAbilityCD = 2.0f;
						return FALSE;
					}

					if(pYinShen->CanUseManaCheck(FSM::Char.MainPlayer))
					{
						FSM::Char.Controller->eventClearAllCmd();
						if(FSM::Target.CurTargetMMOActor)
						{
							FSM::Char.Controller->eventAddCmdUseAbility(pYinShen,FSM::Target.CurTargetMMOActor,FSM::Target.CurTargetMMOActor->Location,FALSE);
						}
						else{
							FSM::Char.Controller->eventAddCmdUseAbility(pYinShen,		NULL,	FSM::Target.CurTargetPos);
						}
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}


UMMOAbility* AttackClassB::GetPrimaryAbility()
{
	if(FSM::Char.Controller==NULL)
	{
		return NULL;
	}
	FVector AttackPos = FSM::Char.Controller->MouseHitWorldLocation;
	if(FSM::Target.DesireMMOActor){
		AttackPos = FSM::Target.DesireMMOActor->ServerLocation;
		UpdateDynamicPos(AttackPos,FSM::Target.DesireMMOActor->BodySize>=E_body_3x3);
	}
	else{
		UpdateDynamicPos(AttackPos,FALSE);
	}


	UMMOAbility* pPuGong = MMOFacade::getSingleton().FindMyUsableSkill(2530);
	if(pPuGong && pPuGong->CanUseManaCheck(_pMainPlayer))
	{
		return pPuGong;
	}
	return NULL;
}


void  AttackClassE::Init()
{
	AttackClassX::Init();
	_Last_ShiDu_Ability = 3400;
}

UBOOL AttackClassE::InPrimaryAbilityRange(FVector DesPos, UBOOL b3x3)
{
	if(FSM::Char.Controller==NULL)
	{
		return NULL;
	}
	FVector AttackPos = FSM::Char.Controller->MouseHitWorldLocation;
	if(FSM::Target.DesireMMOActor){
		AttackPos = FSM::Target.DesireMMOActor->ServerLocation;
	}

	UpdateDynamicPos(AttackPos,b3x3);

	if(_fDistance<ONE_GRID_RADIUS)
	{
		return TRUE;
	}
	return FALSE;
}
UMMOAbility* AttackClassE::GetPrimaryAbility()
{
	if(FSM::Char.Controller==NULL)
	{
		return NULL;
	}
	FVector AttackPos = FSM::Char.Controller->MouseHitWorldLocation;
	if(FSM::Target.DesireMMOActor){
		AttackPos = FSM::Target.DesireMMOActor->ServerLocation;
		UpdateDynamicPos(AttackPos,FSM::Target.DesireMMOActor->BodySize>=E_body_3x3);
	}
	else{
		UpdateDynamicPos(AttackPos,FALSE);
	}


	UMMOAbility* pPuGong = MMOFacade::getSingleton().FindMyUsableSkill(3000);
	if(pPuGong && pPuGong->CanUseManaCheck(_pMainPlayer))
	{
		return pPuGong;
	}
	return NULL;
}

UMMOAbility* AttackClassE::OptimizeAbility(UMMOAbility* desireAbility,UMMOAbility* useAbility)
{
	if(desireAbility && desireAbility->SpellCommonAttribute.Id==3004)
	{
		if(_Last_ShiDu_Ability==3004)
		{
			UINT Key = MMOSkillManager::CombinateAbilityID( 3400, desireAbility->SpellCommonAttribute.Level, desireAbility->SpellCommonAttribute.RuneSlotID );
			UMMOAbility* p3400Ability = MMOSkillManager::getSingleton().FindAbilityTemplate( Key );
			if(p3400Ability)
			{
				UBOOL bRes = Mir2Attack::getSingleton().GetAbilityItemCostCondition(p3400Ability);
				if(bRes)
				{
					return p3400Ability;
				}
			}

			if(Mir2Attack::getSingleton().GetAbilityItemCostCondition(desireAbility))
			{
				return desireAbility;
			}
		}
		else
		{
			UBOOL bRes = Mir2Attack::getSingleton().GetAbilityItemCostCondition(desireAbility);
			if(bRes)
			{
				return desireAbility;
			}
			else
			{
				UINT Key = MMOSkillManager::CombinateAbilityID( 3400, desireAbility->SpellCommonAttribute.Level, desireAbility->SpellCommonAttribute.RuneSlotID );
				UMMOAbility* p3400Ability = MMOSkillManager::getSingleton().FindAbilityTemplate( Key );
				if(p3400Ability)
				{
					UBOOL bRes = Mir2Attack::getSingleton().GetAbilityItemCostCondition(p3400Ability);
					if(bRes)
					{
						return p3400Ability;
					}
				}
			}
		}







		//if(_Last_ShiDu_Ability!=3004)
		//{
		//	UBOOL bRes = Mir2Attack::getSingleton().GetAbilityItemCostCondition(desireAbility);
		//	if(bRes)
		//	{
		//		return desireAbility;
		//	}
		//	else
		//	{
		//		_Last_ShiDu_Ability=3004;
		//	}
		//}

		//if(_Last_ShiDu_Ability==3004)
		//{
		//	if(useAbility==NULL || useAbility->SpellCommonAttribute.Id!=3400){
		//		UINT Key = MMOSkillManager::CombinateAbilityID( 3400, desireAbility->SpellCommonAttribute.Level, desireAbility->SpellCommonAttribute.RuneSlotID );
		//		UMMOAbility* p3400Ability = MMOSkillManager::getSingleton().FindAbilityTemplate( Key );
		//		if(p3400Ability)
		//		{
		//			UBOOL bRes = Mir2Attack::getSingleton().GetAbilityItemCostCondition(p3400Ability);
		//			if(bRes)
		//			{
		//				return p3400Ability;
		//			}
		//			else
		//			{
		//				return desireAbility;
		//			}
		//		}
		//	}
		//	else
		//	{
		//		return useAbility;
		//	}
		//}
	}
	return desireAbility;
}		

void AttackClassE::SendUseAbility(INT AbilityId)
{
	if(AbilityId==3004)
	{
		_Last_ShiDu_Ability=3004;
	}
	else if(AbilityId==3400)
	{
		_Last_ShiDu_Ability=3400;
	}
}

void AttackClassE::Update(FLOAT fDeltaTime)
{
	if( FSM::Char.MainPlayer->isMoving )
	{
		FSM::CDTime.DoAIAbilityCD = 0.5f;
	}
}

//-------------------------------------------------------------------------
Mir2Attack::Mir2Attack()
{
	_AttackClassX = NULL;
	_ShowScreenMsgEnum = -1;
	_ShowScreenMsgCountDown = -1.0f;
	m_TryToMine = FALSE;
	m_PracticeDeltaTime = DEFAULT_PRACTISE_TIME;
}

Mir2Attack::~Mir2Attack()
{
	if(_AttackClassX)
	{
		delete _AttackClassX;
		_AttackClassX = NULL;
	}
}

// 1.05 backport: generic attack handler (see AttackClassGeneric in Mir2Attack.h)
UBOOL AttackClassGeneric::InPrimaryAbilityRange(FVector DesPos, UBOOL b3x3)
{
	if(FSM::Char.Controller==NULL)
	{
		return FALSE;
	}
	FVector AttackPos = FSM::Char.Controller->MouseHitWorldLocation;
	if(FSM::Target.DesireMMOActor){
		AttackPos = FSM::Target.DesireMMOActor->ServerLocation;
	}
	UpdateDynamicPos(AttackPos,b3x3);

	if(_fDistance<ONE_GRID_RADIUS)
	{
		return TRUE;
	}
	return FALSE;
}

UMMOAbility* AttackClassGeneric::GetPrimaryAbility()
{
	if(FSM::Char.Controller==NULL)
	{
		return NULL;
	}
	FVector AttackPos = FSM::Char.Controller->MouseHitWorldLocation;
	if(FSM::Target.DesireMMOActor){
		AttackPos = FSM::Target.DesireMMOActor->ServerLocation;
		UpdateDynamicPos(AttackPos,FSM::Target.DesireMMOActor->BodySize>=E_body_3x3);
	}
	else{
		UpdateDynamicPos(AttackPos,FALSE);
	}

	UMMOAbility* pPuGong = MMOFacade::getSingleton().FindMyUsableSkill(_NormalAttackId);
	if(pPuGong && pPuGong->CanUseManaCheck(_pMainPlayer))
	{
		return pPuGong;
	}
	return NULL;
}

UBOOL AttackClassGeneric::DoAIAbility()
{
	return FALSE;
}

void AttackClassGeneric::Update(FLOAT fDeltaTime)
{
}
void Mir2Attack::InitPlayerClassX()
{
	if(_AttackClassX)
	{
		delete _AttackClassX;
		_AttackClassX = NULL;
	}

	AMMOPlayer* pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(pMainPlayer){
		switch(pMainPlayer->GetClassId())
		{
		case EPCT_PlayerClassA:
			_AttackClassX = new AttackClassA();
			break;
		case EPCT_PlayerClassB:
			_AttackClassX = new AttackClassB();
			break;
		case EPCT_PlayerClassE:
			_AttackClassX = new AttackClassE();
			break;
		// 1.05 backport: Rogue, Archer, Lancer, Sorcerer use their normal attack spell
		case EPCT_PlayerClassC:
			_AttackClassX = new AttackClassGeneric(1530);
			break;
		case EPCT_PlayerClassD:
			_AttackClassX = new AttackClassGeneric(2040);
			break;
		case EPCT_PlayerClassG:
			_AttackClassX = new AttackClassGeneric(1200);
			break;
		case EPCT_PlayerClassH:
			_AttackClassX = new AttackClassGeneric(3200);
			break;
		}	
	}
}

void Mir2Attack::Update(FLOAT fDeltaTime)
{
	CTRL_TICK("Mir2Attack::Update");

	if(_AttackClassX)
	{
		_AttackClassX->Update(fDeltaTime);
	}

	if(_ShowScreenMsgCountDown>0.0f)
	{
		_ShowScreenMsgCountDown -= fDeltaTime;
	}

	if(m_bPracticeOpen)
	{
		if(m_PracticeTime>0.0f)
		{
			m_PracticeTime -= fDeltaTime;
		}

		static UBOOL switchFlag = FALSE;
		if(switchFlag)
		{
			switchFlag=FALSE;
			FSMIO::getSingleton().Target.ClearDesireAbility(NULL);
		}

		if(m_PracticeTime<0.0f)
		{
			if(	FSM::Char.AbilityActionNum==0 &&
				FSM::Char.nCannotInterruptLevel==0)
			{
				
				UMMOAbility *pPracticeAbility = MMOSkillManager::getSingleton().GetEquippedSkillBySkillID(m_PracticeAbilityID);
				if(pPracticeAbility && pPracticeAbility->SpellCommonAttribute.CastType!=E_spct_Switch)
				{
					FSMIO::getSingleton().Target.SetDesireAbility(pPracticeAbility);
					switchFlag = TRUE;
				}
			}
			m_PracticeTime = m_PracticeDeltaTime;
		}


	}

	if(m_TryToMine)
	{
		TryToMine(m_TryToMinePos);
	}
}


void Mir2Attack::ClearAbilityFlag()
{
	if(_AttackClassX)
	{
		_AttackClassX->ClearAbilityFlag();
	}
}

void Mir2Attack::SetAbilityFlag(INT FlagType)
{
	if(_AttackClassX)
	{
		_AttackClassX->SetAbilityFlag(FlagType);
	}
}

UBOOL Mir2Attack::InPrimaryAbility(INT AbilityID)
{
	switch(AbilityID)
	{
	case 1030:
	case 2530:
	case 3000:
	case 1430:
	case 1431:
	case 1432:
	case 1433:
	case 1434:
		return TRUE;
	default:
		return FALSE;
	}
	return FALSE;
}

UBOOL Mir2Attack::InPrimaryAbilityRange(FVector DesPos, UBOOL b3x3)
{
	if(_AttackClassX)
	{
		return _AttackClassX->InPrimaryAbilityRange(DesPos,b3x3);
	}
	return FALSE;
}

void Mir2Attack::TryToMine(const FVector& Des)
{
	AMMOPlayer* pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(pMainPlayer==NULL)
	{
		return;
	}

	if(pMainPlayer->IsInSafeZone)
	{
		return;
	}

	if(FSM::CDTime.MoveDelayTime>0.0f || FSM::CDTime.MoveDelayFrame>0)
	{
		return;
	}

	if(pMainPlayer->IsUsingAbility()==TRUE)
	{
		return;
	}

	UEquip* WeaponEquip = pMainPlayer->GetWeaponEquip();
	if (WeaponEquip == NULL)
	{
		return;
	}

	INT TemplateID = WeaponEquip->GetTemplateID();

	if (TemplateID != 99900007 &&
		TemplateID != 99900041 &&
		TemplateID != 99900042 &&
		TemplateID != 99900043)
	{
		return;
	}


	if(pMainPlayer->CanUseAnyAbility()==FALSE)
	{
		return;
	}

	if(pMainPlayer->isMoving==TRUE)
	{
		return;
	}



	FLOAT Dis = (pMainPlayer->ServerLocation-Des).Size2D();
	if(Dis>64.0f&&Dis<400.0f)
	{
		ZONE_DATA* pBufZone = NULL;
		GRID_DATA* pBufGrid = NULL;
		UBOOL bRes= gServerGridWorld.GetZoneAndGrid(Des, pBufZone, pBufGrid);
		if(bRes && pBufGrid && (pBufGrid->bOutOfPoly||pBufGrid->bInServerPointRange) )
		{
			g_bMineHitSound = pBufGrid->uMineLevel > 0;
			m_TryToMine = FALSE;
			FSM::CDTime.DoAIAbilityCD = 2.0f;
			CMessageHandler::GetInstance().SendRoleMine(Des);
			//UMMOAbility* pMineAbility = GetMineAbility();
			//if(pMineAbility){
			//	MMOFacade::getSingleton().GetAbilityProcessor()->PlayClientAbility(pMainPlayer->ServerID, pMineAbility);
			//	FSMIO::getSingleton().CDTime.ResetCD_WalkAndRunDelay(pMineAbility->SpellCommonAttribute.WalkDelayTime,pMineAbility->SpellCommonAttribute.RunDelayTime);
			//}
		}
	}
}

UMMOAbility* Mir2Attack::GetMineAbility(INT nMineLevel)
{
	if(nMineLevel==2){
		UINT Key = MMOSkillManager::CombinateAbilityID( 4003, 0, 0 );
		UMMOAbility* pMineAbility = MMOSkillManager::getSingleton().FindAbilityTemplate( Key );
		return pMineAbility;
	}
	else
	{
		UINT Key = MMOSkillManager::CombinateAbilityID( 4002, 0, 0 );
		UMMOAbility* pMineAbility = MMOSkillManager::getSingleton().FindAbilityTemplate( Key );
		return pMineAbility;
	}
}

UMMOAbility* Mir2Attack::GetPrimaryAbility()
{
	if(_AttackClassX)
	{
		return _AttackClassX->GetPrimaryAbility();
	}
	else
	{
		InitPlayerClassX();
		if(_AttackClassX)
		{
			return _AttackClassX->GetPrimaryAbility();
		}
	}
	return NULL;
}

void Mir2Attack::UpdateItemSubType(FClientItemInfo* newInfo,UItem* item)
{
	if(newInfo&&item)
	{
		//Purely a speed-up behavior, this code section can be removed------------------------------------------
		switch (newInfo->m_nTemplateID)
		{
		case 40011:	case 40012:	case 40013:	case 40014:
		case 40001:	case 40002:	case 40003:	case 40004:
		case 40101:	case 40102:	case 40103:	case 40104:
		case 40201:	case 40202:
			break;
		default:
			return;
		};

		//add by JimZhang 2015.4.10 currently only---------------------------------------
		if (item->ItemType == TEXT("ITEMTYPE_USABLE"))
		{
			if (item->ItemSubType == TEXT("ITEMSUBTYPE_USABLE_YELLOW_POWDER"))
			{ //yellow powder
				newInfo->ItemSubTypeNum = ITEMSUBTYPE_USABLE_YELLOW_POWDER;
			}
			else if (item->ItemSubType == TEXT("ITEMSUBTYPE_USABLE_GREY_POWDER"))
			{ //gray powder
				newInfo->ItemSubTypeNum = ITEMSUBTYPE_USABLE_GREY_POWDER;
			}
			else if (item->ItemSubType == TEXT("ITEMSUBTYPE_USABLE_RUNE"))
			{	//amulet
				newInfo->ItemSubTypeNum = ITEMSUBTYPE_USABLE_RUNE;
			}
			else if (item->ItemSubType == TEXT("ITEMSUBTYPE_USABLE_QUIVER"))
			{	//quiver (archer arrows)
				newInfo->ItemSubTypeNum = ITEMSUBTYPE_USABLE_QUIVER;
			}
			else
			{
				newInfo->ItemSubTypeNum = ITEMSUBTYPE_INVALID;
			}
		}
		else
		{
			newInfo->ItemSubTypeNum = ITEMSUBTYPE_INVALID;
		}
	}
}

UBOOL Mir2Attack::GetAbilityItemCostCondition(UMMOAbility* pUseAbility,UBOOL bSHowMsg)
{
	if(pUseAbility)
	{ 
		if( pUseAbility->SpellCommonAttribute.ItemCostCount>0 )
		{
			UBOOL bRes = FALSE;
			if(pUseAbility->SpellCommonAttribute.ItemCostSubType  == TEXT("ITEMSUBTYPE_USABLE_YELLOW_POWDER"))
			{
				bRes = MMOFacade::getSingleton().GetItemDurInBag(0,ITEMSUBTYPE_USABLE_YELLOW_POWDER,pUseAbility->SpellCommonAttribute.ItemCostCount);
				if(!bRes&&bSHowMsg)_ShowAbilityScreenMsg(ITEMSUBTYPE_USABLE_YELLOW_POWDER);
			}
			else if(pUseAbility->SpellCommonAttribute.ItemCostSubType  == TEXT("ITEMSUBTYPE_USABLE_GREY_POWDER"))
			{
				bRes = MMOFacade::getSingleton().GetItemDurInBag(0,ITEMSUBTYPE_USABLE_GREY_POWDER,pUseAbility->SpellCommonAttribute.ItemCostCount);
				if(!bRes&&bSHowMsg)_ShowAbilityScreenMsg(ITEMSUBTYPE_USABLE_GREY_POWDER);
			}
			else if(pUseAbility->SpellCommonAttribute.ItemCostSubType  == TEXT("ITEMSUBTYPE_USABLE_RUNE"))
			{
				bRes = MMOFacade::getSingleton().GetItemDurInBag(0,ITEMSUBTYPE_USABLE_RUNE,pUseAbility->SpellCommonAttribute.ItemCostCount);
				if(!bRes&&bSHowMsg)_ShowAbilityScreenMsg(ITEMSUBTYPE_USABLE_RUNE);
			}
			else if(pUseAbility->SpellCommonAttribute.ItemCostSubType  == TEXT("ITEMSUBTYPE_USABLE_QUIVER"))
			{
				bRes = MMOFacade::getSingleton().GetItemDurInBag(0,ITEMSUBTYPE_USABLE_QUIVER,pUseAbility->SpellCommonAttribute.ItemCostCount);
				if(!bRes&&bSHowMsg)_ShowAbilityScreenMsg(ITEMSUBTYPE_USABLE_QUIVER);
			}
			if(bRes==0)
			{
				if(pUseAbility==FSM::Ability.CacheAbility)
				{
					FSMIO::getSingleton().Target.SetCacheAbility(NULL);
				}
			}
			return bRes;
		}
	}
	return TRUE;
}

void Mir2Attack::_ShowAbilityScreenMsg(INT enumType)
{
	if(_ShowScreenMsgEnum!=enumType || _ShowScreenMsgCountDown<=0.0f)
	{
		FString msg;
		switch(enumType)
		{
		case ITEMSUBTYPE_USABLE_YELLOW_POWDER:
			msg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Mir2Attack.ITEMSUBTYPE_USABLE_YELLOW_POWDER"));
			break;
		case ITEMSUBTYPE_USABLE_GREY_POWDER:
			msg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Mir2Attack.ITEMSUBTYPE_USABLE_GREY_POWDER"));
			break;
		case ITEMSUBTYPE_USABLE_RUNE:
			msg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Mir2Attack.ITEMSUBTYPE_USABLE_RUNE"));
			break;
		case ITEMSUBTYPE_USABLE_QUIVER:
			msg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Mir2Attack.ITEMSUBTYPE_USABLE_QUIVER"));
			break;
		}
		MMOUI::GetUIManager()->delegateAddScreenMessage( msg,MsgInfoType_Table,TEXT("Mir2Attack::ShowAbilityScreenMsg") );
		_ShowScreenMsgCountDown = 0.5f;
		_ShowScreenMsgEnum=enumType;
	}
}

UMMOAbility* Mir2Attack::OptimizeAbility(UMMOAbility* desireAbility,UMMOAbility* useAbility)
{
	if(_AttackClassX && desireAbility)
	{ 
		AMMOPlayer* pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		AMMOPlayerController* pController =	MMOFacade::getSingleton().GetMainController();
		if(InPrimaryAbility(desireAbility->SpellCommonAttribute.Id))
		{
			//if(pMainPlayer && pMainPlayer->isMoving==FALSE && pMainPlayer->IsUsingAbility()==FALSE )
			//{
				return _AttackClassX->GetPrimaryAbility();
			//}
		}
		else
		{
			UMMOAbility* pOptimizeAbility = _AttackClassX->OptimizeAbility( desireAbility, useAbility);
			if(pController && pController->CanUseAnyAbility(pOptimizeAbility)==FALSE)
			{
				return _AttackClassX->GetPrimaryAbility();
			}
			else
			{
				return pOptimizeAbility;
			}
			//return _AttackClassX->OptimizeAbility( desireAbility, useAbility);
		}
	}
	return desireAbility;
}

void Mir2Attack::SendUseAbility(INT AbilityId)
{
	if(_AttackClassX){
		return _AttackClassX->SendUseAbility(AbilityId);
	}
}

UBOOL Mir2Attack::DoAIAbility()
{
	if(_AttackClassX){
		return _AttackClassX->DoAIAbility();
	}	
	return FALSE;
}

void Mir2Attack::PreRotate(FVector DesPos)
{
	if(FSM::Char.MainPlayer)
	{
		INT DirIndex = gServerGridWorld.OptimizeDir_Mir2(FSM::Char.MainPlayer->ServerLocation,DesPos);
		if(DirIndex>=0)
		{
			INT nYaw = gServerGridWorld.GetYawFromDirIndex((DirIndex+7)%8);
			FRotator NewRotation = FSM::Char.MainPlayer->Rotation;
			NewRotation.Yaw = nYaw;
			FSM::Char.MainPlayer->LockDesiredRotation(FALSE, TRUE);
			FSM::Char.MainPlayer->SetDesiredRotation(NewRotation, FALSE, TRUE, -1.f, TRUE);
			FSM::Char.MainPlayer->SetRotation(NewRotation);
		}
	}
}

FLOAT Mir2Attack::GetNpcMoveSpeedTime(AMMOActor* actor,UBOOL bRun,FLOAT defaultTime)
{
	if(actor==NULL)return defaultTime;

	
	if(actor->GetActorType()==EACT_Pet)
	{
		ANPC* npc = Cast<ANPC>(actor);
		if(npc && npc->ActorAttributes)
		{
			INT nTemplateID = npc->GetTemplateID();
			INT nPetLevel = npc->ActorAttributes->PetLevel;
			if(nTemplateID==9291 || nTemplateID==9298)//divine beast
			{
				INT nMTime = 500 - (nPetLevel<=3 ? nPetLevel*50 : 150);
				return FLOAT(nMTime)/1000.0f;
			}
			else if(nTemplateID==9290 || nTemplateID==9292)//skeleton
			{
				INT nMTime = 1200 - (nPetLevel<=3 ? nPetLevel*250 : 750);
				return FLOAT(nMTime)/1000.0f;
			}
			else
			{
				INT nMTime = 1500-(nPetLevel<=3? nPetLevel*200 : 600);
				return Min( defaultTime, FLOAT(nMTime)/1000.0f);
			}
		}
	}
	return defaultTime;
}

void Mir2Attack::AutoPractice(UINT16 AbilityID, UINT8 deltaTime, UBOOL bOpen)
{
	m_PracticeAbilityID = AbilityID;
	m_PracticeDeltaTime = deltaTime < MIN_PRACTISE_TIME ? MIN_PRACTISE_TIME : deltaTime;
	m_bPracticeOpen = bOpen;
	if(m_bPracticeOpen)
	{
		m_PracticeTime = m_PracticeDeltaTime;
	}
	else
	{
		FSMIO::getSingleton().Target.ClearDesireAbility(NULL);
		m_PracticeTime = -1.0f;
	}
}

void Mir2Attack::OnEnterGame(UBOOL bEnter)
{
	if(bEnter){
		FSM::CDTime.DoAIAbilityCD = 1.0f;
	}
	else
	{

	}
}





