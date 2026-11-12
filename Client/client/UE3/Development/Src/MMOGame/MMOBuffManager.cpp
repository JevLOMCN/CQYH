#include "StdAfx.h"
#include "MMOBuffManager.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "AbilityProcessor.h"
#include "FSMIO.h"
#include "MMOSkillManager.h"
#include "Mir2Attack.h"
#include "scene_def.h"

using namespace std;
extern UBOOL g_bGMBuff;
MMOBuffManager::MMOBuffManager(void)
{
	for(int i=0;i<MAXBUFFSLOT;i++)
	{
		ShowBuffs[i] = NULL;
		ShowTargetBuffs[i] = NULL;	
	}
	for(int i=0;i<MAXBUFFSLOTEX;i++)
	{
		ShowBuffsEx[i] = NULL;
	}
	DisableUseNormalAbilityBuffType.Empty();
//	DisableUseAbilityBuffType.AddItem(EBE_Type1); //Lu Chenlei said no more turning red when hurt
//	DisableUseNormalAbilityBuffType.AddItem(E_auet_Dazzy); //stun
//	DisableUseNormalAbilityBuffType.AddItem(E_auet_Statued); //petrify
//	DisableUseNormalAbilityBuffType.AddItem(E_auet_Freeze); //freeze
//	DisableUseNormalAbilityBuffType.AddItem(E_auet_Sleep); //sleep

//	DisableUseAllAbilityBuffType.Empty();
//	DisableUseAllAbilityBuffType.AddItem(E_auet_Silence); //silence
//	DisableUseAllAbilityBuffType.AddItem(E_auet_Chaos); //confusion
//	DisableUseAllAbilityBuffType.AddItem(E_auet_KnockUp); //knock up
//	DisableUseAllAbilityBuffType.AddItem(E_auet_KnockDown); //knock down
//	DisableUseAllAbilityBuffType.AddItem(E_auet_Drag); //drag
}

UBOOL MMOBuffManager::CheckBuffEnableUseNormalAbility(UBuff* buff)
{
	if (buff)
	{
		for (INT i = 0 ; i < DisableUseNormalAbilityBuffType.Num() ; i++)
		{
			if (buff->Type == DisableUseNormalAbilityBuffType(i))
			{
				return FALSE;
				break;
			}
		}
	}
	return TRUE;
}

UBOOL MMOBuffManager::CheckBuffEnableUseAllAbility(UBuff* buff)
{
	if (buff)
	{
		for (INT i = 0; i < DisableUseAllAbilityBuffType.Num(); i ++)
		{
			if (buff->Type == DisableUseAllAbilityBuffType(i))
			{
				return FALSE;
				break;
			}
		}
	}
	return TRUE;
}

void MMOBuffManager::UpdateTargetBuff(INT ObjId)
{
	CTRL_TICK("MMOBuffManager::UpdateTargetBuff");

	int index =0;
	STLInt2BuffDataMapMap::iterator it = ActiveBuffsMap.find(ObjId);
	if (it != ActiveBuffsMap.end())
	{
		STLInt2BuffDataMap::iterator pos = it->second->Buffs.begin();
		for (; pos!=it->second->Buffs.end()&&index<MAXBUFFSLOT; pos ++)
		{
			if (pos->second->ObjId == ObjId)
			{
				if (pos->second->Buff->bShowIcon)
				{
					ShowTargetBuffs[index] = pos->second;	
					index++;
				}
			}		
		}
	}

	for (int i = index; i < MAXBUFFSLOT; i++)
	{
		ShowTargetBuffs[i] = NULL;
	}

	for(int i=0;i<MAXBUFFSLOT;i++)
	{
		if(ShowTargetBuffs[i])
		{
			FLOAT CurrentTime = max((ShowTargetBuffs[i]->Duration-ShowTargetBuffs[i]->CurrentTime)*1000.0f , 0.0f);
			MMOUI::GetUIManager()->delegateAddTargetBuffPanel(i,ShowTargetBuffs[i]->DisplayIconName /*>Buff->GetBuffIconName()*/ , (INT)(ShowTargetBuffs[i]->Duration*1000.0f) ,(INT)CurrentTime, (INT)ShowTargetBuffs[i]->StackCount,(INT)ShowTargetBuffs[i]->ObjId, (INT)ShowTargetBuffs[i]->Buff->TemplateID, (INT)ShowTargetBuffs[i]->Buff->bShowTimeUpTips, FALSE);

		}
		else
		{
			MMOUI::GetUIManager()->delegateDeleteTargetBuffPanel(i);
		}
	}
}

void MMOBuffManager::RefreshSlotBuff_Target(INT ObjId)
{
	
	UBOOL canUseAblity = TRUE;
	AMMOActor* TargetActor = FSMIO::getSingleton().Target.GetTargetMMOActor();
	AMMOActor* SelectActor = FSMIO::getSingleton().Target.GetSelectMMOActor();

//	AMMOActor* TargetActor = MMOTargetManager::getSingleton().GetTargetMMOActor();
//	AMMOActor* SelectActor = MMOTargetManager::getSingleton().GetSelectMMOActor();

	//if( TargetActor && TargetActor->ServerID == ObjId ){}
	//else 
	
	if	(SelectActor && SelectActor->ServerID == ObjId ){}
	else {
		return;
	}

	UpdateTargetBuff(ObjId);

	//STLInt2BuffDataMapMap::iterator it = ActiveBuffsMap.find(ObjId);
	//if (it != ActiveBuffsMap.end())
	//{

	//	STLInt2BuffDataMap::iterator pos = it->second->Buffs.begin();
	//	for(;pos!=it->second->Buffs.end()&&index<MAXBUFFSLOT;pos++)
	//	{
	//		if(pos->second->ObjId == ObjId)
	//		{
	//			if(pos->second->Buff->bShowIcon )
	//			{
	//				ShowTargetBuffs[index] = pos->second;	
	//				index++;
	//			}

	//			//canUseAblity = CheckBuffEnableUseAbility( pos->second->Buff );
	//		}		
	//	}
	//}
	////MMOUI::GetUIManager()->delegateBuffEnableAbility(canUseAblity);

	//for(int i=index;i<MAXBUFFSLOT;i++)
	//{
	//	ShowTargetBuffs[i] = NULL;
	//}

	//for(int i=0;i<MAXBUFFSLOT;i++)
	//{
	//	if(ShowTargetBuffs[i])
	//	{
	//		FLOAT CurrentTime = max((ShowTargetBuffs[i]->Duration-ShowTargetBuffs[i]->CurrentTime)*1000.0f , 0.0f);
	//		MMOUI::GetUIManager()->delegateAddTargetBuffPanel(i,ShowTargetBuffs[i]->DisplayIconName /*>Buff->GetBuffIconName()*/ , (INT)(ShowTargetBuffs[i]->Duration*1000.0f) ,(INT)CurrentTime, (INT)ShowTargetBuffs[i]->StackCount,(INT)ShowTargetBuffs[i]->ObjId, (INT)ShowTargetBuffs[i]->Buff->TemplateID, (INT)ShowTargetBuffs[i]->Buff->bShowTimeUpTips, FALSE);

	//	}
	//	else
	//	{
	//		MMOUI::GetUIManager()->delegateDeleteTargetBuffPanel(i);
	//	}
	//}



}

void MMOBuffManager::RefreshSlotBuffEx(INT ObjId)
{
	RefreshSlotBuff(ObjId);
}

void MMOBuffManager::HideSlotBuffEx()
{
	for(int i=0;i<MAXBUFFSLOT;i++)
	{
		MMOUI::GetUIManager()->delegateDeleteBuffPanel(i);
	}

	//Add function buff
	for(int i=0;i<MAXBUFFSLOTEX;i++)
	{
		MMOUI::GetUIManager()->delegateDeleteFunctionBuffPanel(i);	
	}
}

void MMOBuffManager::RefreshSlotBuff(INT ObjId)
{
	int index =0;
	int indexEx =0;
	//UBOOL canUseNormalAblity = TRUE;
	//UBOOL canUseAllAblity = TRUE;
	if(MMOFacade::getSingleton().GetMainPlayer()==NULL)return;
	if(MMOFacade::getSingleton().GetMainPlayer()->ServerID != ObjId )return;

	UBOOL bStun = FALSE;
	g_bGMBuff = FALSE;
	Mir2Attack::getSingleton().ClearAbilityFlag();

	STLInt2BuffDataMapMap::iterator it = ActiveBuffsMap.find(ObjId);
	if (it != ActiveBuffsMap.end())
	{
		STLInt2BuffDataMap::iterator pos = it->second->Buffs.begin();
		for(;pos!=it->second->Buffs.end()&&index<MAXBUFFSLOT;pos++)
		{
			if(pos->second->ObjId == MMOFacade::getSingleton().GetMainPlayer()->ServerID)
			{
				if(pos->second->Buff->bShowIcon && pos->second->Buff->bBuffType == EBuff_Normal )
				{
					ShowBuffs[index] = pos->second;	
					index++;
				}

				if(pos->second->Buff->bShowIcon && pos->second->Buff->bBuffType == EBuff_Function )
				{
					ShowBuffsEx[indexEx] = pos->second;	
					indexEx++;
				}

				if(pos->second->Buff->Type==E_auet_Type01 ||
					pos->second->Buff->Type==E_auet_Type02 ||
				   pos->second->Buff->Type==E_auet_Type03 )//||
				   //pos->second->Buff->Type==E_auet_Type17 ||
				   //pos->second->Buff->Type==E_auet_Type20 )
				{
					bStun = TRUE;
				}

				if (pos->second->Buff->TemplateID==2516)
				{
					g_bGMBuff = TRUE;
				}

				switch(pos->second->Buff->Type)
				{
				case E_auet_Type18:
					Mir2Attack::getSingleton().SetAbilityFlag(EAT_FLAG_CISHA);
					break;
				case E_auet_Type19:
					Mir2Attack::getSingleton().SetAbilityFlag(EAT_FLAG_BANYUE);
					break;
				case E_auet_Type20:
					Mir2Attack::getSingleton().SetAbilityFlag(EAT_FLAG_LIEHUO);
					break;
				case E_auet_Type21:
					Mir2Attack::getSingleton().SetAbilityFlag(EAT_FLAG_ZHURI);
					break;
				case E_auet_Type09:
					Mir2Attack::getSingleton().SetAbilityFlag(EAT_FLAG_YINSHEN);
					break;
				case E_auet_Type15:
					Mir2Attack::getSingleton().SetAbilityFlag(EAT_FLAG_MOFADUN);
					break;
				}

				//if(pos->second->Buff->Type==E_auet_Type21)
				//{
				//	Mir2Attack::getSingleton().SetAbilityFlag(EAT_FLAG_ZHURI);
				//}
				//if(pos->second->Buff->Type==E_auet_Type20)
				//{
				//	Mir2Attack::getSingleton().SetAbilityFlag(EAT_FLAG_LIEHUO);
				//}
				//if(pos->second->Buff->Type==E_auet_Type19)
				//{
				//	Mir2Attack::getSingleton().SetAbilityFlag(EAT_FLAG_BANYUE);
				//}
				//if(pos->second->Buff->Type==E_auet_Type18)
				//{
				//	Mir2Attack::getSingleton().SetAbilityFlag(EAT_FLAG_CISHA);
				//}
				////canUseNormalAblity = CheckBuffEnableUseNormalAbility( pos->second->Buff );
				////canUseAllAblity = CheckBuffEnableUseAllAbility( pos->second->Buff );
			}		
		}

		for(;pos!=it->second->Buffs.end();pos++)
		{
			if(pos->second->Buff->Type==E_auet_Type01 ||
				pos->second->Buff->Type==E_auet_Type02 ||
				pos->second->Buff->Type==E_auet_Type03 )//||
				//pos->second->Buff->Type==E_auet_Type17 ||
				//pos->second->Buff->Type==E_auet_Type20 )
			{
				bStun = TRUE;
			}

			if (pos->second->Buff->TemplateID == 2516)
			{
				g_bGMBuff = TRUE;
			}

			if(pos->second->Buff->Type==E_auet_Type21)
			{
				Mir2Attack::getSingleton().SetAbilityFlag(EAT_FLAG_ZHURI);
				warnf(TEXT("Refresh buff----------add Zhuri (Chase the Sun)"));

			}
			if(pos->second->Buff->Type==E_auet_Type20)
			{
				Mir2Attack::getSingleton().SetAbilityFlag(EAT_FLAG_LIEHUO);
			}
			if(pos->second->Buff->Type==E_auet_Type19)
			{
				Mir2Attack::getSingleton().SetAbilityFlag(EAT_FLAG_BANYUE);
			}
			if(pos->second->Buff->Type==E_auet_Type18)
			{
				Mir2Attack::getSingleton().SetAbilityFlag(EAT_FLAG_CISHA);
			}
		}
	}


	MMOFacade::getSingleton().GetMainPlayer()->eventSetStun(bStun);

	//MMOUI::GetUIManager()->delegateBuffEnableAbility( !canUseNormalAblity , !canUseAllAblity );
	MMOUI::GetUIManager()->delegateBuffEnableAbility();

	for(int i=index;i<MAXBUFFSLOT;i++)
	{
		ShowBuffs[i] = NULL;
	}

	for(int i=indexEx;i<MAXBUFFSLOTEX;i++)
	{
		ShowBuffsEx[i] = NULL;
	}

	
	//Add normal buff
	for(int i=0;i<MAXBUFFSLOT;i++)
	{
		if(ShowBuffs[i])
		{
			FLOAT CurrentTime = max((ShowBuffs[i]->Duration-ShowBuffs[i]->CurrentTime)*1000.0f , 0.0f);
			UBOOL debuff = FALSE; 
			BYTE Category = E_ac_None;
			
			if(ShowBuffs[i]->Buff ){
			   Category = ShowBuffs[i]->Buff->Category;
			   if(Category==E_ac_Negative){
				   debuff = TRUE;
			   }
			}

			if(ShowBuffs[i]->Buff && ShowBuffs[i]->Buff->bShowStackCount ){
				MMOUI::GetUIManager()->delegateAddBuffPanel(i,ShowBuffs[i]->DisplayIconName /*>Buff->GetBuffIconName()*/ , (INT)(ShowBuffs[i]->Duration*1000.0f) ,(INT)CurrentTime, (INT)ShowBuffs[i]->StackCount, (INT)ShowBuffs[i]->ObjId, (INT)ShowBuffs[i]->Buff->TemplateID,(INT)ShowBuffs[i]->Buff->bShowTimeUpTips, debuff );
			}
			else
			{
				MMOUI::GetUIManager()->delegateAddBuffPanel(i,ShowBuffs[i]->DisplayIconName /*>Buff->GetBuffIconName()*/ , (INT)(ShowBuffs[i]->Duration*1000.0f) ,(INT)CurrentTime, 0, (INT)ShowBuffs[i]->ObjId, (INT)ShowBuffs[i]->Buff->TemplateID, (INT)ShowBuffs[i]->Buff->bShowTimeUpTips , debuff);
			}
		}
		else
		{
			MMOUI::GetUIManager()->delegateDeleteBuffPanel(i);
		}
	}

	//Add function buff
	for(int i=0;i<MAXBUFFSLOTEX;i++)
	{
		if(ShowBuffsEx[i])
		{
			FLOAT CurrentTime = max((ShowBuffsEx[i]->Duration-ShowBuffsEx[i]->CurrentTime)*1000.0f , 0.0f);
			UBOOL debuff = FALSE; 
			BYTE Category = E_ac_None;

			if(ShowBuffsEx[i]->Buff ){
				Category = ShowBuffsEx[i]->Buff->Category;
				if(Category==E_ac_Negative){
					debuff = TRUE;
				}
			}

			if(ShowBuffsEx[i]->Buff && ShowBuffsEx[i]->Buff->bShowStackCount ){
				MMOUI::GetUIManager()->delegateAddFunctionBuffPanel(i,ShowBuffsEx[i]->DisplayIconName /*>Buff->GetBuffIconName()*/ , (INT)(ShowBuffsEx[i]->Duration*1000.0f) ,(INT)CurrentTime, (INT)ShowBuffsEx[i]->StackCount, (INT)ShowBuffsEx[i]->ObjId, (INT)ShowBuffsEx[i]->Buff->TemplateID,(INT)ShowBuffsEx[i]->Buff->bShowTimeUpTips, debuff );
			}
			else
			{
				MMOUI::GetUIManager()->delegateAddFunctionBuffPanel(i,ShowBuffsEx[i]->DisplayIconName /*>Buff->GetBuffIconName()*/ , (INT)(ShowBuffsEx[i]->Duration*1000.0f) ,(INT)CurrentTime, 0, (INT)ShowBuffsEx[i]->ObjId, (INT)ShowBuffsEx[i]->Buff->TemplateID, (INT)ShowBuffsEx[i]->Buff->bShowTimeUpTips , debuff);
			}
		}
		else
		{
			MMOUI::GetUIManager()->delegateDeleteFunctionBuffPanel(i);
		}
	}
	

}

void MMOBuffManager::AddBuffData(BuffData* InBuff)
{
	//if (!InBuff)
	//{
	//	return;
	//}
	//STLInt2BuffDataMap::iterator pos = ActiveBuffs.find(InBuff->Id);
	//if (pos == ActiveBuffs.end())
	//{
	//	ActiveBuffs.insert(make_pair(InBuff->Id, InBuff));
	//}

	
	//new-----------------------------------------------------------------------
	if( ActiveBuffsMap.find(InBuff->ObjId) == ActiveBuffsMap.end() )
	{
		BuffDataMap* newBuffMap = new BuffDataMap;
		ActiveBuffsMap.insert(make_pair(InBuff->ObjId, newBuffMap));
	}

	STLInt2BuffDataMapMap::iterator it = ActiveBuffsMap.find(InBuff->ObjId);
	if (it != ActiveBuffsMap.end())
	{
		BuffDataMap* pBuffDataMap = it->second;
		if(pBuffDataMap)
		{
			pBuffDataMap->Buffs.insert(make_pair(InBuff->Id, InBuff));
		}
	}


}

void MMOBuffManager::TickBuffData(float deltaTime)
{
	CTRL_TICK("MMOBuffManager::TickBuffData");

	PostDeleteBuffObjID.Empty();
	PostDeleteBuffBuffID.Empty();

	STLInt2BuffDataMapMap::iterator it = ActiveBuffsMap.begin();
	for(;it!=ActiveBuffsMap.end();it++)
	{
		AMMOActor* baseActor = MMOFacade::getSingleton().GetCharacter(it->first);
		if (baseActor == NULL)
		{
			continue;
		}

		if (baseActor->bResourceInitialized == FALSE && baseActor != MMOFacade::getSingleton().GetMainPlayer())
		{
			continue;
		}

		STLInt2BuffDataMap::iterator pos = it->second->Buffs.begin();
		for (;pos!=it->second->Buffs.end();pos++)
		{
			if (pos->second->CurrentTime < pos->second->Duration)
			{
				FLOAT NewTime = pos->second->CurrentTime + deltaTime;
				UBuff* Buff   = pos->second->Buff;
				INT    NUM	  = Buff->SingleComponentList.Num();
				for (INT i = 0; i < Buff->SingleComponentList.Num(); ++i)
				{
					UAbilityComponent* Comp = Buff->SingleComponentList(i).Component;
					if (Comp && pos->second->CurrentTime <= Buff->SingleComponentList(i).StartTime && NewTime > Buff->SingleComponentList(i).StartTime)
					{
						if (Comp->StopType != ACBT_StopWhenConstDurationPass)
						{
							Comp->Duration = pos->second->Duration - pos->second->CurrentTime;
							if (Comp->Duration < 0.f)
							{
								Comp->Duration = 0.1f;
							}
						}

						AActor* TargetActor = MMOFacade::getSingleton().GetActor(pos->second->ObjId);
						FVector TargetLocation;
						if (TargetActor)
						{
							TargetLocation = TargetActor->Location;
						}

						Comp->bAddToRoot = TRUE;
						BaseComponent* NewComponent = AbilityProcessor::CreateDynamicComponent(Comp, FALSE, pos->second->CreatorId, pos->second->ObjId, pos->second->ObjId, TargetLocation, 1.0);
						if (!NewComponent)
						{
							continue;
						}

						EffectProcessor::GetSingleton().AddActiveComponent(NewComponent);
						if (Comp->StopType != ACBT_StopWhenConstDurationPass)
						{
							pos->second->SingleEffectComponentIds.push_back(NewComponent->ComponentId);
						}
						NewComponent->OnAction(TRUE);
					}
				}

				for (INT i = 0; i < pos->second->Buff->OverlapComponentList.Num(); ++i)
				{
					UAbilityComponent* Comp = pos->second->Buff->OverlapComponentList(i).Component;
					if (Comp &&
						pos->second->CurrentTime <= pos->second->Buff->OverlapComponentList(i).StartTime &&
						NewTime > pos->second->Buff->OverlapComponentList(i).StartTime)
					{
						if (Comp->StopType != ACBT_StopWhenConstDurationPass)
						{
							Comp->Duration = pos->second->Duration - pos->second->CurrentTime;
							if (Comp->Duration < 0.f)
							{
								Comp->Duration = 0.1f;
							}
						}

						AActor* TargetActor = MMOFacade::getSingleton().GetActor(pos->second->ObjId);
						FVector TargetLocation;
						if (TargetActor)
						{
							TargetLocation = TargetActor->Location;
						}

						pos->second->OverlapEffectComponentIds.push_back(IdNode());
						for (INT j = 0; j < pos->second->Buff->MaxStackCount; ++j)
						{
							Comp->bAddToRoot = TRUE;
							BaseComponent* NewComponent = AbilityProcessor::CreateDynamicComponent(Comp, FALSE, pos->second->CreatorId, pos->second->ObjId, pos->second->ObjId, TargetLocation, 1.0);
							if (!NewComponent)
							{
								continue;
							}

							EffectProcessor::GetSingleton().AddActiveComponent(NewComponent);
							if (Comp->StopType != ACBT_StopWhenConstDurationPass)
							{
								pos->second->OverlapEffectComponentIds[i].Ids.push_back(NewComponent->ComponentId);
							}
							NewComponent->OnAction(TRUE);
						}
					}
				}

				pos->second->CurrentTime += deltaTime;

				if(pos->second->CurrentTime-deltaTime<pos->second->Duration  && pos->second->CurrentTime>=pos->second->Duration)
				{
					if(pos->second->bAutoClose)
					{
						PostDeleteBuffObjID.AddItem(pos->second->ObjId);
						PostDeleteBuffBuffID.AddItem(pos->second->Id);
					}
				}
			}

			// fix bug #22643
			if (baseActor->NeedRefreshMaterial)
			{
				for (size_t i = 0; i < pos->second->SingleEffectComponentIds.size(); ++i)
				{
					BaseComponent* Component = EffectProcessor::GetSingleton().GetActiveComponent(pos->second->SingleEffectComponentIds[i]);
					if (Component && Component->GetType() == ECPT_MaterialEffect)
					{
						Component->OnAction(TRUE);
					}
				}

				for (size_t i = 0; i < pos->second->OverlapEffectComponentIds.size(); ++i)
				{
					for (size_t j = 0; j < pos->second->OverlapEffectComponentIds[i].Ids.size(); ++j)
					{
						BaseComponent* Component = EffectProcessor::GetSingleton().GetActiveComponent(pos->second->OverlapEffectComponentIds[i].Ids[j]);
						if (Component && Component->GetType() == ECPT_MaterialEffect)
						{
							Component->OnAction(TRUE);
						}
					}
				}
			}
		}

		baseActor->NeedRefreshMaterial = FALSE;
	}

	//Self-closing buff
	for(INT i=0;i<PostDeleteBuffObjID.Num();i++)
	{
		DeleteBuff(PostDeleteBuffObjID(i),PostDeleteBuffBuffID(i));
	}
}

BuffData* MMOBuffManager::GetActiveBuff(INT ObjId,INT InId)
{
	//STLInt2BuffDataMap::iterator pos = ActiveBuffs.find(InId);
	//if (pos == ActiveBuffs.end())
	//{
	//	return NULL;
	//}
	//else
	//{
	//	return pos->second;
	//}

	//new-----------------------------------------------------------------------
	STLInt2BuffDataMapMap::iterator it = ActiveBuffsMap.find(ObjId);
	if (it != ActiveBuffsMap.end())
	{
		BuffDataMap* pBuffDataMap = it->second;
		if(pBuffDataMap)
		{
			STLInt2BuffDataMap::iterator itBuff = pBuffDataMap->Buffs.find(InId);
			if(itBuff != pBuffDataMap->Buffs.end())
			{
				return itBuff->second;
			}
		}
	}
	return NULL;

}


void MMOBuffManager::DeleteBuffDataMap(BuffDataMap* pBuffDataMap, INT CharactorID)
{
	if(pBuffDataMap==NULL)return;
	AMMOActor* pCharactor =  MMOFacade::getSingleton().GetCharacter(CharactorID);


	STLInt2BuffDataMap::iterator pos = pBuffDataMap->Buffs.begin();
	for(;pos!=pBuffDataMap->Buffs.end(); )
	{
		for (size_t i = 0; i < pos->second->SingleEffectComponentIds.size(); ++i)
		{
			EffectProcessor::GetSingleton().DeleteActiveComponent(pos->second->SingleEffectComponentIds[i]);
		}
		pos->second->SingleEffectComponentIds.clear();

		for (size_t i = 0; i < pos->second->OverlapEffectComponentIds.size(); ++i)
		{
			for (size_t j = 0; j < pos->second->OverlapEffectComponentIds[i].Ids.size(); ++j)
			{
				EffectProcessor::GetSingleton().DeleteActiveComponent(pos->second->OverlapEffectComponentIds[i].Ids[j]);
			}
			pos->second->OverlapEffectComponentIds[i].Ids.clear();
		}
		pos->second->OverlapEffectComponentIds.clear();



		if(pos->second){
			pos->second->Buff->eventDoEnd(pCharactor);
			if (pCharactor)
			{
				pCharactor->RemoveElementary(pos->second->Id);
			}
			delete pos->second;
			pos->second = NULL;
		}



		pos = pBuffDataMap->Buffs.erase(pos);
	}

	if(pBuffDataMap->Buffs.size()==0)
	{
		delete pBuffDataMap;
	}

}

void MMOBuffManager::ClearAllBuff()
{
	STLInt2BuffDataMapMap::iterator it = ActiveBuffsMap.begin();// .find(ObjId);
	while( it != ActiveBuffsMap.end())
	{
		BuffDataMap* pBuffDataMap = it->second;
		INT CharactorID = it->first;
		if(pBuffDataMap)
		{
			DeleteBuffDataMap(pBuffDataMap,CharactorID);
		}
		it = ActiveBuffsMap.erase(it);
	}

	if(MMOFacade::getSingleton().GetMainPlayer()){
		RefreshSlotBuff(MMOFacade::getSingleton().GetMainPlayer()->ServerID);
	}
}


void MMOBuffManager::DeleteActiveBuff(INT ObjId)
{
	STLInt2BuffDataMapMap::iterator it = ActiveBuffsMap.find(ObjId);
	if (it == ActiveBuffsMap.end())return;

	BuffDataMap* pBuffDataMap = it->second;
	if(pBuffDataMap==NULL){
		ActiveBuffsMap.erase(it);
		return;
	}

	DeleteBuffDataMap(pBuffDataMap,ObjId);
	ActiveBuffsMap.erase(it);


	//STLInt2BuffDataMap::iterator pos = pBuffDataMap->Buffs.begin();
	//for(;pos!=pBuffDataMap->Buffs.end(); )
	//{
	//	for (size_t i = 0; i < pos->second->SingleEffectComponentIds.size(); ++i)
	//	{
	//		EffectProcessor::GetSingleton().DeleteActiveComponent(pos->second->SingleEffectComponentIds[i]);
	//	}
	//	pos->second->SingleEffectComponentIds.clear();

	//	for (size_t i = 0; i < pos->second->OverlapEffectComponentIds.size(); ++i)
	//	{
	//		for (size_t j = 0; j < pos->second->OverlapEffectComponentIds[i].Ids.size(); ++j)
	//		{
	//			EffectProcessor::GetSingleton().DeleteActiveComponent(pos->second->OverlapEffectComponentIds[i].Ids[j]);
	//		}
	//		pos->second->OverlapEffectComponentIds[i].Ids.clear();
	//	}
	//	pos->second->OverlapEffectComponentIds.clear();

	//	if(pos->second){
	//		delete pos->second;
	//		pos->second = NULL;
	//	}

	//	pos = pBuffDataMap->Buffs.erase(pos);
	//}

	//if(pBuffDataMap->Buffs.size()==0)
	//{
	//	delete pBuffDataMap;
	//	ActiveBuffsMap.erase(it);
	//}

}

void MMOBuffManager::DeleteActiveBuff(INT ObjId,INT InId)
{
	STLInt2BuffDataMapMap::iterator it = ActiveBuffsMap.find(ObjId);
	if (it == ActiveBuffsMap.end())return;

	BuffDataMap* pBuffDataMap = it->second;
	if(pBuffDataMap==NULL){
		ActiveBuffsMap.erase(it);
		return;
	}

	STLInt2BuffDataMap::iterator pos = pBuffDataMap->Buffs.find(InId);
	if (pos != pBuffDataMap->Buffs.end())
	{
		for (size_t i = 0; i < pos->second->SingleEffectComponentIds.size(); ++i)
		{
			EffectProcessor::GetSingleton().DeleteActiveComponent(pos->second->SingleEffectComponentIds[i]);
		}
		pos->second->SingleEffectComponentIds.clear();

		for (size_t i = 0; i < pos->second->OverlapEffectComponentIds.size(); ++i)
		{
			for (size_t j = 0; j < pos->second->OverlapEffectComponentIds[i].Ids.size(); ++j)
			{
				EffectProcessor::GetSingleton().DeleteActiveComponent(pos->second->OverlapEffectComponentIds[i].Ids[j]);
			}
			pos->second->OverlapEffectComponentIds[i].Ids.clear();
		}
		pos->second->OverlapEffectComponentIds.clear();

		////UI attached to auto-judged qte buff
		//if(pos->second->Buff && pos->second->Buff->bQTEBuff)
		//{
		//	if(MMOFacade::getSingleton().GetMainPlayer()->ServerID == ObjId )
		//	{
		//		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_HintCombat_BossQTE,FALSE);
		//	}
		//}
		delete pos->second;

		pBuffDataMap->Buffs.erase(pos);
	}
	

	if(pBuffDataMap->Buffs.size()==0)
	{
		delete pBuffDataMap;
		ActiveBuffsMap.erase(it);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BuffData* MMOBuffManager::GetBuffBySlot(INT nSlot)
{
	if(nSlot>=0 && nSlot<MAXBUFFSLOT)
	{
		return ShowBuffs[nSlot];	
	}
	else
	{
		return NULL;
	}
}

BuffData* MMOBuffManager::GetBuffExBySlot(INT nSlot)
{
	if(nSlot>=0 && nSlot<MAXBUFFSLOTEX)
	{
		return ShowBuffsEx[nSlot];	
	}
	else
	{
		return NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BuffData* MMOBuffManager::GetTargetBuffBySlot(INT nSlot)
{
	if(nSlot>=0 && nSlot<MAXBUFFSLOT)
	{
		return ShowTargetBuffs[nSlot];	
	}
	else
	{
		return NULL;
	}
}

void MMOBuffManager::AddOrChangeBuff(INT CreatorId, INT ObjId, INT Id, INT TemplateId, INT Level, FLOAT Duration, FLOAT MaxDuration, INT StackCount, UBOOL bAutoClose)
{
	BuffData* buff = GetActiveBuff(ObjId, Id);
	if (buff == NULL )
	{
		AddNewBuff(CreatorId, ObjId, Id, TemplateId, Level, Duration, MaxDuration, StackCount, bAutoClose);
	}
	else
	{
		ChangeBuff(CreatorId, ObjId, Id, TemplateId, Level, Duration, MaxDuration, StackCount, bAutoClose);
	}
}


void MMOBuffManager::ModifyBuff(BuffData* newBuff, INT CreatorId, INT ObjId, INT Id, INT TemplateId, INT Level, FLOAT Duration, FLOAT MaxDuration, INT StackCount, UBOOL bAutoClose)
{
	if (newBuff && newBuff->Buff)
	{
		newBuff->BuffName		 = newBuff->Buff->BuffName.GetNameString();
		newBuff->DisplayIconName = newBuff->Buff->GetBuffIconName();
		newBuff->BuffDescription = newBuff->Buff->BuffDescription;
	}
	else
	{
		return;
	}

	//Special handling for mount replacement-----------------------------------------------------------------------
	if (TemplateId==2501)
	{
		MountData* pMountData = MMOFacade::getSingleton().GetMountData(CreatorId);
		if (pMountData)
		{
			newBuff->BuffName		 = pMountData->Name;			//   newBuff->buff->BuffName.GetNameString();
			newBuff->DisplayIconName = pMountData->IconName;	//   newBuff->buff->GetBuffIconName();
			newBuff->BuffDescription = pMountData->Script;		// newBuff->buff->BuffDescription;
			newBuff->BuffColor		 = FString::Printf(TEXT("lv%d"), pMountData->Quality );
		}
	}
}

void MMOBuffManager::AddNewBuff(INT CreatorId, INT ObjId, INT Id, INT TemplateId, INT Level, FLOAT Duration, FLOAT MaxDuration, INT StackCount, UBOOL bAutoClose)
{
#if SHOW_BUFF_LOG
	warnf(NAME_Log, TEXT("MMOBuffManager::AddNewBuff(CreatorId = %d, ObjId = %d, Id = %d, TemplateId = %d, Level = %d, Duration = %f, MaxDuration = %f, StackCount = %d)"),
		CreatorId, ObjId, Id, TemplateId, Level, Duration, MaxDuration, StackCount);
#endif

	if(TemplateId==0)
	{
		return;
	}

	BuffData* bd = GetActiveBuff(ObjId,Id);
	if (bd)
	{
 		return;
	}

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(ObjId);
	if (!actor)
	{
		return;
	}

	UBuff* buff = MMOFacade::getSingleton().FindBuffTemplate(TemplateId, Level);
	if (!buff)
	{
		warnf(L"can not find buff template data by id=%d and level=%d", TemplateId, Level);
		return;
	}

	if(buff->bQTEBuff)
	{
		if(actor->IsMainPlayer()){
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_HintCombat_BossQTE,TRUE,"");
			MMOUI::GetUIManager()->delegateUpdateBossQTE(StackCount,INT((Duration+1.0)/1000.0)	);
		}
	}

	BuffData* newBuff = new BuffData;
	if (!newBuff)
	{
		return;
	}
	newBuff->Id = Id;
	newBuff->CreatorId = CreatorId;
	newBuff->ObjId = ObjId;
	newBuff->Level = Level;
	newBuff->Buff = buff;

	if (MaxDuration < -0.0)
	{
		MaxDuration = Duration = buff->Duration * 1000.f;
	}

	ModifyBuff(newBuff, CreatorId,  ObjId,  Id,  TemplateId,  Level,  Duration,  MaxDuration,  StackCount,  bAutoClose);

	newBuff->CurrentTime = (MaxDuration - Duration) / 1000.f;
	// fix bug #23809
	// For Buffs that need saving, the Duration and MaxDuration sent by the server have a 1-frame error (saving on logout is one frame late),
	// If the Component's StartTime is less than 0.0625 seconds, the mechanism considers the Component already created
	if (newBuff->CurrentTime <= (1.0F / (FLOAT)GAME_FPS + 0.1f))
	{
		newBuff->CurrentTime = 0.f;
	}
	newBuff->Duration = MaxDuration / 1000.f;
	newBuff->StackCount = StackCount;
	newBuff->bAutoClose = bAutoClose;

	if (newBuff->Buff->OverriddenElementaryType != 0)
	{
		actor->SetNewElementary(newBuff->Id, newBuff->Buff->OverriddenElementaryType);
	}

	for (INT i = 0; i < buff->SingleEffectList.Num(); ++i)
	{
		if (buff->SingleEffectList(i) == NULL)
		{
			continue;
		}
		buff->SingleEffectList(i)->StopType = ACBT_StopWhenConstDurationPass;
		buff->SingleEffectList(i)->Duration = Duration / 1000.f;

		buff->SingleEffectList(i)->bAddToRoot = TRUE;
		BaseComponent* NewComponent = AbilityProcessor::CreateDynamicComponent(buff->SingleEffectList(i), FALSE, CreatorId, ObjId, ObjId, actor->Location, 1.0);
		if (!NewComponent)
		{
			continue;
		}

		EffectProcessor::GetSingleton().AddActiveComponent(NewComponent);
		newBuff->SingleEffectComponentIds.push_back(NewComponent->ComponentId);

		NewComponent->OnAction(TRUE);
	}

	for (INT i = 0; i < buff->OverlapEffectList.Num(); ++i)
	{
		if (buff->OverlapEffectList(i) == NULL)
		{
			continue;
		}
		buff->OverlapEffectList(i)->StopType = ACBT_StopWhenConstDurationPass;
		buff->OverlapEffectList(i)->Duration = Duration / 1000.f;

		newBuff->OverlapEffectComponentIds.push_back(IdNode());
		for (INT j = 0; j < StackCount; ++j)
		{
			buff->OverlapEffectList(i)->bAddToRoot = TRUE;
			BaseComponent* NewComponent = AbilityProcessor::CreateDynamicComponent(buff->OverlapEffectList(i), FALSE,CreatorId, ObjId, ObjId, actor->Location, 1.0);
			if (!NewComponent)
			{
				continue;
			}

			EffectProcessor::GetSingleton().AddActiveComponent(NewComponent);
			newBuff->OverlapEffectComponentIds[i].Ids.push_back(NewComponent->ComponentId);
			NewComponent->OnAction(TRUE);
		}
	}

	AddBuffData(newBuff);
	RefreshSlotBuff(ObjId);
	RefreshSlotBuff_Target(ObjId);
	//EffectProcessor::GetSingleton().AddBuffData(newBuff);

	//debugf(TEXT( "------------------------------------------------------------------AddNewBuff buffid = %d" ), TemplateId  );


	//add by jimzhang 2012.7.18, add buff start and end state trigger behaviors
	buff->eventDoStart(actor);

	AMMOPlayerController* controller = MMOFacade::getSingleton().GetMainController();
	check(controller);
	INT selfID = controller->GetPlayer()->ServerID;
	if (buff->bQTEBuff)
	{
		if(newBuff->ObjId == selfID )
		{
			controller->OnQTEStart(newBuff->Buff,newBuff->CreatorId ,newBuff->Duration);
		}
	}

	//
	if( ObjId == selfID && !controller->IsTransformed() ){
		UBOOL isTransformed = MMOSkillManager::getSingleton().EquipTransformSkillGroupAndCachOldSkills( TemplateId );
		controller->SetTransformState( isTransformed );
	}

}

void MMOBuffManager::ChangeBuff(INT CreatorId, INT ObjId, INT Id, INT TemplateId, INT Level, FLOAT Duration, FLOAT MaxDuration, INT StackCount, UBOOL bAutoClose)
{
#if SHOW_BUFF_LOG
	warnf(NAME_Log, TEXT("MMOBuffManager::ChangeBuff(CreatorId = %d, ObjId = %d, Id = %d, TemplateId = %d, Level = %d, Duration = %f, MaxDuration = %f, StackCount = %d)"), 
		CreatorId, ObjId, Id, TemplateId, Level, Duration, MaxDuration, StackCount);
#endif

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(ObjId);
	if (!actor)
	{
		return;
	}

	BuffData* buffData = GetActiveBuff(ObjId,Id);
	if (buffData == NULL || buffData->Buff == NULL)
	{
		return;
	}

	ModifyBuff(buffData, CreatorId,  ObjId,  Id,  TemplateId,  Level,  Duration,  MaxDuration,  StackCount,  bAutoClose);

	if (buffData->Buff->bQTEBuff)
	{
		if (actor->IsMainPlayer())
		{
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_HintCombat_BossQTE, TRUE, "");
			MMOUI::GetUIManager()->delegateUpdateBossQTE(StackCount, -1);
		}
	}

	//debugf(TEXT("Buff Change Id=%d, Duration=%f, StackCount=%d"), Id, Duration, StackCount);
	FLOAT NewCurrentTime = (MaxDuration - Duration) / 1000.f;
	if (NewCurrentTime < 0.f)
	{
		NewCurrentTime = 0.f;
	}

	buffData->CurrentTime = NewCurrentTime;
	// fix bug  #23688
	FLOAT MaxStartTime = 0.0f;
	for (INT i = 0; i < buffData->Buff->SingleComponentList.Num(); ++i)
	{
		FComponentNode ComponentNode = buffData->Buff->SingleComponentList(i);
		if (MaxStartTime < ComponentNode.StartTime)
		{
			MaxStartTime = ComponentNode.StartTime;
		}
	}

	for (INT i = 0; i < buffData->Buff->OverlapComponentList.Num(); ++i)
	{
		FComponentNode ComponentNode = buffData->Buff->OverlapComponentList(i);
		if (MaxStartTime < ComponentNode.StartTime)
		{
			MaxStartTime = ComponentNode.StartTime;
		}
	}
	buffData->CurrentTime += MaxStartTime + 0.5f;

	buffData->Duration = MaxDuration / 1000.f;
	INT oldStackCount = buffData->StackCount;
	buffData->StackCount = StackCount;

	for (size_t i = 0; i < buffData->SingleEffectComponentIds.size(); ++i)
	{
		BaseComponent* Component = EffectProcessor::GetSingleton().GetActiveComponent(buffData->SingleEffectComponentIds[i]);
		if(Component)
		{
			Component->ResetLifeSpan(0.f, Duration / 1000.f);
		}
	}

	for (size_t i = 0; i < buffData->OverlapEffectComponentIds.size(); ++i)
	{
		for (size_t j = 0; j < buffData->OverlapEffectComponentIds[i].Ids.size(); ++j)
		{
			BaseComponent* Component = EffectProcessor::GetSingleton().GetActiveComponent(buffData->OverlapEffectComponentIds[i].Ids[j]);
			if (Component)
			{
				Component->ResetLifeSpan(0.f, Duration / 1000.f);
			}
		}
	}

	if (StackCount < oldStackCount)
	{
		for (size_t i = 0; i < buffData->OverlapEffectComponentIds.size(); ++i)
		{
			for (INT j = StackCount; j < oldStackCount - StackCount; ++j)
			{
				if (buffData->OverlapEffectComponentIds[i].Ids.size() > 0)
				{
					INT IdCom = buffData->OverlapEffectComponentIds[i].Ids.size();
					IdCom = buffData->OverlapEffectComponentIds[i].Ids[IdCom -1];
					EffectProcessor::GetSingleton().DeleteActiveComponent(IdCom);
				}
			}
		}
	}
	else if (StackCount > oldStackCount)
	{// Add stack count
		UBuff* buffTemplate = MMOFacade::getSingleton().FindBuffTemplate(buffData->Buff->TemplateID,  buffData->Level);
		if (buffTemplate)
		{
			for (INT i = 0; i < buffTemplate->OverlapEffectList.Num(); ++i)
			{
				if (buffTemplate->OverlapEffectList(i) == NULL)
				{
					continue;
				}
				buffTemplate->OverlapEffectList(i)->StopType = ACBT_StopWhenConstDurationPass;
				buffTemplate->OverlapEffectList(i)->Duration = Duration / 1000.f;

				//buff->OverlapEffectComponentIds.push_back(IdNode());
				for (INT j = oldStackCount; j < StackCount; ++j)
				{
					buffTemplate->OverlapEffectList(i)->bAddToRoot = TRUE;
					BaseComponent* NewComponent = AbilityProcessor::CreateDynamicComponent(buffTemplate->OverlapEffectList(i), FALSE,buffData->CreatorId, ObjId, ObjId, actor->Location, 1.0);
					if (!NewComponent)
					{
						continue;
					}

					EffectProcessor::GetSingleton().AddActiveComponent(NewComponent);
					buffData->OverlapEffectComponentIds[i].Ids.push_back(NewComponent->ComponentId);
					NewComponent->OnAction(TRUE);
				}
			}
		}
	}

	RefreshSlotBuff(ObjId);
	RefreshSlotBuff_Target(ObjId);
}

void MMOBuffManager::DeleteBuffByCharacterID(INT ObjId)
{
	DeleteActiveBuff(ObjId);
	RefreshSlotBuff(ObjId);
	RefreshSlotBuff_Target(ObjId);

	//
	AMMOPlayerController *mainCtrl = MMOFacade::getSingleton().GetMainController();
	AMMOActor* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( ( mainPlayer != NULL && mainPlayer->ServerID == ObjId) && ( mainCtrl != NULL && mainCtrl->IsTransformed() ) ){
		mainCtrl->SetTransformState( FALSE );
		//SetTransformState comes first
		MMOSkillManager::getSingleton().RecoverCachedSkillsInTransform();
	}
}

void MMOBuffManager::DeleteBuff(INT ObjId, INT Id)
{
	BuffData* data = GetActiveBuff(ObjId,Id);
	if (!data)
	{
		return;
	}

#if SHOW_BUFF_LOG
	if (data && data->Buff)
	{
		warnf(NAME_Log, TEXT("MMOBuffManager::DeleteBuff(ObjId = %d, Id = %d, BuffTemp = %d)"), ObjId, Id, data->Buff->GetTemplateID());
	}
#endif

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(ObjId);

	if (actor)
	{
		actor->RemoveElementary(Id);
	}

	AMMOPlayerController* controller = MMOFacade::getSingleton().GetMainController();
	check(controller);
	if (data->Buff)
	{
		if (actor)
		{
			actor->PlayActions(data->Buff->BuffEndComponentList);
		}

		data->Buff->eventDoEnd(actor);
		if (data->Buff->bQTEBuff){
			MMOUI::GetUIManager()->delegateUpdateBossQTE(0,-1);
		}

		//
		if( actor && actor->IsMainPlayer() && controller->IsTransformed() ){
			INT buffTemplateID = data->Buff->GetTemplateID();
			const TransformSkillGroup *skillGroup = MMOSkillManager::getSingleton().FindTransformSkillGroupByBuffID( buffTemplateID );
			if( skillGroup != NULL ){
				//Is a transform BUFF
				controller->SetTransformState( FALSE );
				//SetTransformState comes first
				warnf(NAME_Warning, TEXT("======!!!!==================================== [RecoverCachedSkillsInTransform AT %d]"), buffTemplateID );
				MMOSkillManager::getSingleton().RecoverCachedSkillsInTransform();
			}
		}
	}



	DeleteActiveBuff(ObjId,Id);
	RefreshSlotBuff(ObjId);
	RefreshSlotBuff_Target(ObjId);


}

BuffDataMap* MMOBuffManager::GetBuffMap(INT ObjectId)
{
	STLInt2BuffDataMapMap::iterator pos = ActiveBuffsMap.find(ObjectId);
	if (pos == ActiveBuffsMap.end())
	{
		return NULL;
	}
	else
	{
		return pos->second;
	}
}


void BuffTemplateIndexDataProcessor::ParseData( BuffTemplateDataIndex* obj, class CTabFile* dataRow, int rowIdx )
{
	obj->TemplateID = TReadDataValue_Int<BuffTemplateDataIndexManager>(TEXT("TemplateID"), dataRow, rowIdx);
	obj->ObjectPath = TReadDataValue_FString<BuffTemplateDataIndexManager>(TEXT("Index"), dataRow, rowIdx);

	INT id = obj->TemplateID;
	BuffTemplateDataIndexManager::getSingleton().AddData(id, obj);
}

#if WITH_EDITOR
void BuffTemplateIndexDataProcessor::CollectReferencedResourceFromDataTable( BuffTemplateDataIndex* obj, TSet<FString>& outResList )
{
	UObject* dat = UObject::StaticLoadObject(UBuff::StaticClass(), NULL,
		*obj->ObjectPath, NULL, LOAD_NoWarn | LOAD_Quiet, NULL );
	if(dat)
	{
		_CollectReferencedResource(dat, outResList);
	}
}
#endif