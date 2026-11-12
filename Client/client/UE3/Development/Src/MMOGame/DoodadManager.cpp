#include "stdafx.h"
#include "DoodadManager.h"
#include "CommonGameEvents.h"
#include "MMOQuestManager.h"
#include "MMOGameClasses.h"
#include "MMOFacade.h"
#include "MMOTemplateObjectFinder.h"
#include "MMODropManager.h"
#include "MMOGuideManager.h"

DoodadManager::DoodadManager()
{
#if defined(_DEBUG) && 1
	INT tmpID = 0xefff, lv = 0xef, diff = 0xee;
	INT id = FDoodadDropData::CombineID(tmpID, lv, diff);

	INT tmpID2, lv2, diff2;
	FDoodadDropData::DecomposeID(id, tmpID2, lv2, diff2);

	check(tmpID == tmpID2);
	check(lv == lv2);
	check(diff == diff2);
#endif
}
//------------------------------------------------------------------------
DoodadManager::~DoodadManager()
{

}
//------------------------------------------------------------------------
void DoodadManager::Init()
{
	static bool g_loadData = false;
	if(g_loadData)
	{
		return;
	}

	g_loadData = true;

	SCOPE_LOG_TIME(___INIT__, TEXT("DoodadManager::Init"), 0);

	MMOUI::RegisterEventHandler(this, GameEvent_ActorAppear);
	MMOUI::RegisterEventHandler(this, GameEvent_ActorDestroyed);
	MMOUI::RegisterEventHandler(this, GameEVent_SceneLoadFinish);
	MMOUI::RegisterEventHandler(this, GameEvent_LeaveGame);

	LoadDoodadData();

	if(M_MMO_HD)
	{
		LoadAllDoodadTemplate();
	}
}
//------------------------------------------------------------------------
void DoodadManager::AddDoodadDropTemplate( INT k, struct FDoodadDropData* dat )
{
	DoodadDropTemplateMap::const_iterator i = _DoodadDropMap.find(k);
	if(i != _DoodadDropMap.end())
	{
		warnf(NAME_Error, TEXT("[DoodadManager::AddDoodadDropTemplate] Doodad drop already exists, please check for a config conflict! ID:%d[%s]->[%s]"),
			k, *dat->ToString(), *i->second->ToString());	
	}
	else
	{
		_DoodadDropMap.insert(std::make_pair(k, dat));
	}

}


void DoodadManager::LoadDoodadData()
{
	M_BeginLoadData(TEXT("doodad.txt"));

#if 1
	MMODoodadTemplateDataManager::getSingleton().InitFromFile(TEXT("doodad.txt"));
#else
	TArray<FString> DirArray;
	GConfig->GetArray( TEXT("DoodadTemplates"), TEXT("TemplateDir"), DirArray, GGameIni );

	MMO::FormatTempleteDataLoadingDir(DirArray);

	for (INT k = 0; k < DirArray.Num(); ++k)
	{
		const FString& cfgDir = DirArray(k);

		debugIDf(TEXT("TemplateData"), TEXT("===== Loading Doodad template data form %s"), *cfgDir);

		TArray<FString> pkgs;
		appFindFilesInDirectory(pkgs, *cfgDir, TRUE, FALSE);
		for (INT i = 0; i < pkgs.Num(); ++i)
		{
			UPackage* Package = UObject::LoadPackage( NULL, *pkgs(i), LOAD_Verify );
			if( !Package )
			{
				M_AddDataError(TEXT("doodad.txt"),
					FString::Printf(TEXT("Cannot load DOODAD config package %s, the file may be corrupted!"), *pkgs(i)));
				continue;;
			}

			if(!Package->IsFullyLoaded())
			{
				Package->FullyLoad();
			}

			for (INT j = 0; j < Package->GetNetObjectCount(0); ++j)
			{
				UDoodad* Doodad = Cast<UDoodad>(Package->GetNetObjectAtIndex(j));
				if(Doodad)
				{
					Doodad->AddToRoot();
					AddDoodadTemplate(Doodad->ID, Doodad);
				}
			}
		}
	}
#endif
}

//------------------------------------------------------------------------
void DoodadManager::Clear()
{
	_DoodadDropMap.clear();
// 	_doodadTemplateMap.clear();	
	MMODoodadTemplateDataManager::getSingleton().Clear();
}

void DoodadManager::Tick()
{
    //if (_doodadRuning)
    //{
    //    DWORD nt = GetTickCount();
    //    if (nt >= _doodadRuning)
    //    {
    //        DoodadRoleAnimPlayStep();
    //    }
    //}
}

//------------------------------------------------------------------------
struct FDoodadDropData* DoodadManager::FindDoodadDropTemplate( INT templateID )
{
	DoodadDropTemplateMap::iterator it = _DoodadDropMap.find(templateID);
	if(it != _DoodadDropMap.end())
	{
		return it->second;
	}

	INT sceneLv = 0,  sceneDiff = 0, tmpID = 0;
	FDoodadDropData::DecomposeID(templateID, tmpID, sceneLv, sceneDiff);
	warnf(NAME_Error, TEXT("Doodad drop not found, please check the drop config table, ID:%d SceneLv:%d SceneDiff:%d"),
		tmpID, sceneLv * 5, sceneDiff + 1);

	return 0;
}
//------------------------------------------------------------------------
// void DoodadManager::AddDoodadTemplate( INT k, class UDoodad* node)
// {
// 	DoodadTemplateMap::const_iterator i = _doodadTemplateMap.find(k);
// 	if(i != _doodadTemplateMap.end())
// 	{
// 		M_AddDataError(TEXT("doodad.txt"), FString::Printf(TEXT("Doodad template duplicate:%d[%s]->[%s]"),
// 			k, *node->DoodadName.ToString(), *i->second->DoodadName.ToString()));
// 	}
// 	else
// 	{
// 		_doodadTemplateMap.insert(std::make_pair(k, node));
// 	}
// }

void DoodadManager::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	switch(evt->GetEventType())
	{
	case GameEVent_SceneLoadFinish:	
		{
			RefreshDoodadEffects();
		}
		break;

	case GameEvent_LeaveGame:
		{
			DestroyAllDoodadActor();
		}
		break;

	case GameEvent_ActorAppear:
		{
			MMOGameEvent_ActorAppear* actorEvt = (MMOGameEvent_ActorAppear*)evt;
			if(actorEvt->Actor && actorEvt->Actor->IsA(AMMODoodadActor::StaticClass()))
			{
				AMMODoodadActor* doodad = Cast<AMMODoodadActor>(actorEvt->Actor);
				if(doodad)
				{
					SetupQuestDoodadEffect(doodad);
				}
			}
		}
		break;
	}
}

void DoodadManager::RefreshDoodadEffects()
{
	DoodadActorMap::iterator it = _doodadActoMap.begin();
	for (; it != _doodadActoMap.end(); ++it)
	{
		if(!it->second->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
		{
			SetupQuestDoodadEffect(it->second);
		}
	}
	/*
	TObjectIterator<ADropItemActor> It;
	for(; It; ++It)
	{
	ADropItemActor* actor = *It;
	if (actor->ItemType == EItemActorType_Doodad)
	{
	SetupQuestDoodadEffect(actor, quests, questEffect, effectRadius);
	}
	}*/
}

void DoodadManager::SetupQuestDoodadEffect(AMMODoodadActor* actor)
{
	if (!actor)
	{
		return;
	}

	actor->RemoveParticleEffects();
	actor->IsQuestActive = FALSE;

	UDoodad* doodad = actor->DoodadTemplate;
	if (!doodad)
	{
		warnf(NAME_Error, TEXT("Doodad template is empty DebugName:%s"), *actor->GetName());
		return;
	}

	if (!doodad->DoodadAvatar)
	{
		warnf(NAME_Error, TEXT("Doodad DoodadAvatar not configured: %s ID[%d]"), *doodad->DoodadName.ToString(), doodad->DoodadID);
		return;
	}

	INT sceneDifficulty = MMOFacade::getSingleton().GetSceneDifficulty();
	INT sceneLevel = MMOFacade::getSingleton().GetSceneLevel() / 5;
	INT doodadDropId = FDoodadDropData::CombineID(doodad->GetTemplateID(), sceneLevel, sceneDifficulty);
	
	// Whether it is a doodad needed by an accepted quest
	if (MMOQuestData::getSingleton().IsAcceptedQuestDoodad(doodadDropId))
	{
		actor->IsQuestActive = TRUE;

		FString questEffect;
		GConfig->GetString(TEXT("Quest"), TEXT("DoodadEffect"), questEffect, GGameIni);

		FLOAT effectColRadius = 0.0f;
		GConfig->GetFloat(TEXT("Quest"), TEXT("DoodadEffectCollisionRadius"), effectColRadius, GGameIni);

		FLOAT effectScale = actor->DoodadTemplate->DoodadAvatar->CollisionRadius / effectColRadius;

		UParticleSystem* particle = (UParticleSystem*)UObject::StaticLoadObject(UParticleSystem::StaticClass(),
			NULL, *questEffect, NULL, LOAD_NoWarn | LOAD_Quiet, NULL);

		if (particle)
		{
			actor->AddParticleEffects(particle, effectScale);
		}
		else
		{
			warnf(NAME_Error, TEXT("Doodad quest effect particle resource not found: %s. Please check the [Quest] DoodadEffect setting in DefaultGame.ini!"), *questEffect);
		}
	}
}

//------------------------------------------------------------------------
class UDoodad* DoodadManager::FindDoodadTemplate( INT templateID )
{
	FDoodadDataHolder* helper = MMODoodadTemplateDataManager::getSingleton().FindAndModifyData(templateID);
	if(helper)
	{
		return helper->GetDoodadTemplate();
	}
// 	DoodadTemplateMap::iterator it = _doodadTemplateMap.find(templateID);
// 	if(it != _doodadTemplateMap.end())
// 	{
// 		return it->second;
// 	}

	M_AddDataError(TEXT("doodad.txt"),
		FString::Printf(TEXT("Doodad template not found ID:%d"), templateID));
	return 0;
}


void DoodadManager::LoadAllDoodadTemplate()
{
	MMODoodadTemplateDataManager::DataTypeMap& dat = MMODoodadTemplateDataManager::getSingleton().GetDataMap();
	
	SET_WARN_COLOR_AND_BACKGROUND(COLOR_WHITE, COLOR_BLUE);
	warnf(NAME_Log, TEXT("DoodadManager::Init LoadAllDoodadTemplate %d"), dat.size());	
	CLEAR_WARN_COLOR();

	for(MMODoodadTemplateDataManager::DataTypeMap::iterator it = dat.begin();
		it != dat.end(); ++it)
	{
		it->second->GetDoodadTemplate();
	}
}


void DoodadManager::UpdateDoodadOperateCount( INT doodadID, BYTE newCount, UINT32 dwCharID )
{
	AMMODoodadActor* doodad = FindDoodadActor(doodadID);
	if(doodad)
	{
		MMOGameEvent_Doodad evt(doodad, newCount);
		AMMOActor* instigateActor = MMOFacade::getSingleton().GetCharacter(dwCharID);
		doodad->SetRemainOpenCount(newCount, instigateActor);

		RefreshDoodadEffects();

		MMOUI::PostGameEvent(&evt);
	}
}



class AMMODoodadActor* DoodadManager::AddDoodadActor(const FVector& inLocation, const class UDoodad* doodadTemplate, const FRotator& rot, INT doodadActorID, BYTE remainOpenCount)
{
//#if !SHIPPING_PC_GAME
	AMMODoodadActor* findActor = FindDoodadActor(doodadActorID);
	if(findActor)
	{
		SET_WARN_COLOR(COLOR_CYAN);
		debugf(TEXT("@@@@@@@@@@@ AMMODoodadActor::Duplicate doodad actor id %s"), *findActor->GetDebugDescription());
		CLEAR_WARN_COLOR();
		return findActor;
	}
//#endif

	class AMMODoodadActor* obj = NULL;
	FName debugName;

#if 0 // oscar@snda names that may be duplicated cannot be specified like this
#if !SHIPPING_PC_GAME
	debugName = FName(*FString::Printf(TEXT("DoodadActor.DoodadTemplate[%d]_ID[%d]"),
		doodadTemplate->GetTemplateID(), doodadActorID));
#else
	debugName = NAME_None;
#endif
#endif

	debugName = NAME_None;

	obj = Cast<AMMODoodadActor>(GWorld->SpawnActor(AMMODoodadActor::StaticClass(), debugName, inLocation, FRotator(0, 0, 0), NULL, TRUE));
	if(obj)
	{
		obj->DoodadTemplate = const_cast<UDoodad*>(doodadTemplate);
		obj->Rotation = rot;
		//obj->SetRotation(rot);
		obj->SetRemainOpenCount(remainOpenCount, NULL);
		obj->ServerID = doodadActorID;		
		obj->Init();
		_doodadActoMap.insert(std::make_pair(doodadActorID, obj));
		
		SET_WARN_COLOR(COLOR_CYAN);
		debugf(TEXT("@@@@@@@@@@@ AMMODoodadActor::Add %s"), *obj->GetDebugDescription());
		CLEAR_WARN_COLOR();

		MMOFacade::getSingleton().AddToCharacterMap(obj, doodadActorID);

		if (obj->DoodadTemplate)
		{
			MMOGameEvent_Guide evtGuide(EGUET_AddDrop, obj->DoodadTemplate->DoodadType);
			MMOUI::PostGameEvent(&evtGuide);
		}

		obj->ReCalGridBlockRadius();
	}

	return obj;
}

void DoodadManager::DestroyDoodadActor( INT doodadActorID, UBOOL removeFromWrold, INT disappearType )
{
	DoodadActorMap::iterator i = _doodadActoMap.find(doodadActorID);
	if(i != _doodadActoMap.end())
	{
		if(removeFromWrold)
		{
			i->second->DestroyDoodadActor(disappearType);
			//GWorld->DestroyActor(i->second);
		}

		MMOFacade::getSingleton().RemoveFromCharacterMap(doodadActorID);
		_doodadActoMap.erase(i);
	}
}

void DoodadManager::DestroyAllDoodadActor()
{
	DoodadActorMap::iterator i = _doodadActoMap.begin();
	for (; i != _doodadActoMap.end(); ++i)
	{
		i->second->DestroyDoodadActor(sodtLeaveScene);
		MMOFacade::getSingleton().RemoveFromCharacterMap(i->second->ServerID);
	}

	_doodadActoMap.clear();
}

void DoodadManager::OnDoodadBeginOperate(INT InDoodadId, INT OperatorId, FLOAT InDuration)
{
	AMMODoodadActor* findActor = FindDoodadActor(InDoodadId);
	if (findActor)
	{
		findActor->BeginOperate(OperatorId, InDuration);
	}
}

void DoodadManager::OnDoodadEndOperate(INT InDoodadId, UBOOL bBreak)
{
	AMMODoodadActor* findActor = FindDoodadActor(InDoodadId);
	if (findActor)
	{
		findActor->EndOperate(bBreak);
	}
}



//void DoodadManager::EnableRoleAnimByDoodad(AMMODoodadActor* doodadActor, UDoodad* dd, FVector& pos)
//{
//    StopRoleAnimByDoodad(FALSE);
//
//    // Forced test code
//    //name = "Default";
//    _doodadWroking = dd;
//	_doodadActorWorking = doodadActor;
//    _doodadPosition = pos;
//
//    if (dd->RoleAnimGroup.IsEmpty())
//        return;
//
//    AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
//    if (actor && !actor->ActorAT)
//        return;
//
//    UDoodad_RoleAnimGroup** drg = _doodadAnimMap.Find(dd->RoleAnimGroup);
//    if (!drg || !*drg)
//        return;
//
//    UAnimSequence* as_anim;
//    if (!(*drg)->AnimSetBegin.IsEmpty())
//    {
//        as_anim = actor->ActorAT->Mesh->FindAnimSequence(*(*drg)->AnimSetBegin);
//        if (as_anim)
//            _doodadAnimPlaying.push_back(RoleAnimPlayNode(0, false, (*drg)->AnimSetBegin, (DWORD)(as_anim->SequenceLength * 1000.0f)));
//    }
//
//    if (!(*drg)->AnimSetLoop.IsEmpty())
//    {
//        as_anim = actor->ActorAT->Mesh->FindAnimSequence(*(*drg)->AnimSetLoop);
//        if (as_anim)
//            _doodadAnimPlaying.push_back(RoleAnimPlayNode(1, true, (*drg)->AnimSetLoop, (DWORD)(as_anim->SequenceLength * 1000.0f)));
//    }
//
//    if (!(*drg)->AnimSetEnd.IsEmpty())
//    {
//        as_anim = actor->ActorAT->Mesh->FindAnimSequence(*(*drg)->AnimSetEnd);
//        if (as_anim)
//            _doodadAnimPlaying.push_back(RoleAnimPlayNode(2, false, (*drg)->AnimSetEnd, (DWORD)(as_anim->SequenceLength * 1000.0f)));
//    }
//}

//void DoodadManager::PlayRoleAnimByDoodad(DWORD tk)
//{
//    //for (size_t i = 0; i < _doodadAnimPlaying.size(); ++i)
//    //{
//    //    if (_doodadAnimPlaying[i].loop)
//    //        _doodadAnimPlaying[i].tk = tk;
//    //}
//
//    //DoodadRoleAnimPlayStep();
//}

//void DoodadManager::StopRoleAnimByDoodad(UBOOL is_normal)
//{
//    if (is_normal)
//    {
//        if (!_doodadAnimPlaying.empty())
//        {
//            while (_doodadAnimPlaying.size() > 1)
//                _doodadAnimPlaying.erase(_doodadAnimPlaying.begin());
//        }
//
//        if (_doodadWroking)
//        {
//            if (_doodadWroking->DoodadEndEffect)
//            {
//                AActor* actor = GWorld->SpawnActor(AMMOEmitter::StaticClass(), NAME_None, _doodadPosition);
//                AMMOEmitter* emit = Cast<AMMOEmitter>(actor);
//                if (emit)
//                    emit->eventSetTemplate(_doodadWroking->DoodadEndEffect, TRUE);
//            }
//        }
//        DoodadRoleAnimPlayStep();
//    }
//
//	_doodadActorWorking = 0;
//    _doodadWroking = 0;
//    _doodadRuning = 0;
//    _doodadAnimPlaying.clear();
//}

//void DoodadManager::TryPlayDoodadLoopSound()
//{
//	if (_doodadRuning && _doodadWroking && _doodadWroking->DoodadAvatar && _doodadActorWorking)
//	{
//		UGamePlayDataManager::GetSingleton()->PlayDoodadLoopSound(
//			_doodadActorWorking,
//			_doodadWroking->DoodadAvatar->TargetArmorMatType);
//	}
//}

//UBOOL DoodadManager::DoodadRoleAnimPlayStep()
//{
//    _doodadRuning = 0;
//    if (_doodadAnimPlaying.empty())
//        return FALSE;
//
//    AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
//    RoleAnimPlayNode& rapn = _doodadAnimPlaying.front();
//    actor->PlayAnimation(*rapn.name, 0, 1, rapn.loop);
//    
//	switch (rapn.knode)
//    {
//    case 0:
//		{
//			if(_doodadWroking && _doodadWroking->DoodadAvatar && _doodadActorWorking)
//			{
//				UGamePlayDataManager::GetSingleton()->PlayDoodadBeginSound(
//					_doodadActorWorking,
//					_doodadWroking->DoodadAvatar->TargetArmorMatType);
//			}
//			break;
//		}
//
//    case 1:
//		break;// Driven by animation, see interface TryPlayDoodadLoopSound.
//
//    case 2:
//		{
//			if(_doodadWroking && _doodadWroking->DoodadAvatar && _doodadActorWorking)
//			{
//				UGamePlayDataManager::GetSingleton()->PlayDoodadEndSound(
//					_doodadActorWorking,
//					_doodadWroking->DoodadAvatar->TargetArmorMatType);
//			}
//			break;
//		}
//    }
//
//    _doodadRuning = GetTickCount() + rapn.tk - 50;
//    _doodadAnimPlaying.pop_front();
//    return !_doodadAnimPlaying.empty();
//}

class AMMODoodadActor* DoodadManager::FindDoodadActor( INT doodadActorID )
{
	DoodadActorMap::iterator i = _doodadActoMap.find(doodadActorID);
	if(i != _doodadActoMap.end())
	{
		if(!i->second->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
		{
			return i->second;
		}
	}

	return NULL;
}

#if !SHIPPING_PC_GAME

// Test whether quest-related drops are normal
#define TEST_QUEST_DOODAD_ITEM 1

void DoodadManager::TestDoodadEffect()
{

#if TEST_QUEST_DOODAD_ITEM 
	// quest id: 246
	// doodad id: 
	MMOQuestManager::getSingleton().NotifyAcceptQuest(246);
#endif

	FVector curPos = MMOFacade::getSingleton().GetMainPlayer()->GetFootLocation();

	INT idx = 0;
	static INT idOffset = 0;
// 	DoodadTemplateMap::const_iterator it = _doodadTemplateMap.begin();
// 	for (; it != _doodadTemplateMap.end(); ++idx, ++it, ++idOffset )
// 	{
// 		
// 		FVector loc;
// 		loc.X = idx % 20 * 100;
// 		loc.Y = idx / 20 * 100;
// 		loc.Z = 0;
// 
// 		loc += curPos;
// 
// 		AddDoodadActor(loc, it->second, FRotator(0, 0, 0), idOffset, it->second->MaxOperateCount);
// 
// 		++idx;
// 	}

#if TEST_QUEST_DOODAD_ITEM
	DoodadActorMap::iterator dit = _doodadActoMap.begin();
	for (; dit != _doodadActoMap.end(); ++dit)
	{
		MMOGameEvent_ActorAppear evt(dit->second);
		MMOUI::PostGameEvent(&evt);
	}
#endif

#if !TEST_QUEST_DOODAD_ITEM && 0
	FString questEffect;
	GConfig->GetString(TEXT("Quest"), TEXT("DoodadEffect"), questEffect, GGameIni);

	UParticleSystem* parTmp = 
		(UParticleSystem*)UObject::StaticLoadObject( UParticleSystem::StaticClass(), NULL, *questEffect, NULL, LOAD_NoWarn | LOAD_Quiet, NULL );

	FLOAT effectRadius = 0.0f;
	GConfig->GetFloat(TEXT("Quest"), TEXT("DoodadEffectCollisionRadius"), effectRadius, GGameIni);

	static bool removEffect = false;

	DoodadActorMap::iterator i = _doodadActoMap.begin();
	for (; i != _doodadActoMap.end(); ++i)
	{
		if(parTmp)
		{
			FLOAT effScale = i->second->DoodadTemplate->DoodadAvatar->CollisionRadius / effectRadius;
			if(removEffect)
			{
				i->second->RemoveParticleEffects();
			}
			else
			{
				i->second->AddParticleEffects(parTmp, effScale);
			}
		}
	}

	removEffect = !removEffect;
#endif
}

#endif

INT DoodadManager::SearchBody(const FVector& Location, FLOAT Radius) const
{
	INT DoodadActorID = 0;
	FLOAT Distance = Radius * Radius;

	FVector playerPos = Location;

	DoodadActorMap::const_iterator it = _doodadActoMap.begin();
	for (; it != _doodadActoMap.end(); ++it)
	{
		if(it->second
			&& !it->second->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed)
			&& it->second->DoodadTemplate )//&& it->second->DoodadTemplate->DoodadType == E_dt_Corpse)
		{
			FLOAT sz = (it->second->Location - playerPos).SizeSquared2D();
			if(sz < Distance)
			{
				Distance = sz;
				DoodadActorID = it->first;
			}
		}
	}

	return DoodadActorID;
}

UINT FDoodadDropData::CombineID( INT templateID, INT lv, INT sceneDiff )
{
	UINT ret = templateID;
	ret = ret<<8;
	ret += lv;
	ret = ret<<8;
	ret += sceneDiff;
	return ret;
}

void FDoodadDropData::DecomposeID( UINT id, INT& templateID, INT& lv, INT& sceneDiff )
{
	templateID = id>>16;
	lv = ( ( id & 0x0000ff00 ) >> 8 );
	sceneDiff = id & 0x000000ff;
}

class UDoodad* FDoodadDropData::GetDoodadTempalte() const
{
	return DoodadManager::getSingleton().FindDoodadTemplate(DoodadTemplateID);
}

void DoodadDropDataProcessor::ParseData( FDoodadDropData* obj, class CTabFile* dataRow, int rowIdx )
{
	//obj->Doodad = TReadDataValue_TemplateIDObject<UDoodad, MMODoodadDropDataManager, TemplateObjectFinder_Doodad>(TEXT("DoodadTemplateID"), dataRow, rowIdx);

	//fix bug #23519
	//obj->SetDoodadLevel(TReadDataValue_Int<MMODoodadDropDataManager>(TEXT("DoodadLevel"), dataRow, rowIdx));
	obj->SetDoodadLevel(0);

	obj->SetSceneDifficulty(TReadDataValue_Int<MMODoodadDropDataManager>(TEXT("SceneDifficultyClient"), dataRow, rowIdx));
	//obj->DoodadQuality = TReadDataValue_Int<MMODoodadDropDataManager>(TEXT("DoodadQuality"), dataRow, rowIdx);

	for (INT i = 0; i < FDoodadDropData::C_MaxDropSetCount; ++i)
	{
		FString headerDropChance = FString(TEXT("DropChance")) + appItoa(i + 1);
		FString headerDropGroupID = FString(TEXT("DropGroupID")) + appItoa(i + 1);

		FDoodadDropSet dropSet;
		dropSet.DropChance = TReadDataValue_Int<MMODoodadDropDataManager>(headerDropChance, dataRow, rowIdx);		
		dropSet.DropGroupID = TReadDataValue_Int<MMODoodadDropDataManager>(headerDropGroupID, dataRow, rowIdx);
		obj->SetDropSet(i, dropSet);
		//TReadDataValue_TemplateIDObject<UDropGroup, MMODoodadDropDataManager, TemplateObjectFinder_DropGroup>(headerDropGroupID, dataRow, rowIdx);
	}

	MMODoodadDropDataManager::getSingleton().AddData(obj->GetTemplateID(), obj);
}

void DoodadDropDataProcessor::PostInitData()
{
	MMODoodadDropDataManager& itemMgr = MMODoodadDropDataManager::getSingleton();
	MMODoodadDropDataManager::DataTypeMap::const_iterator it = itemMgr._dataMap.begin();
	for (; it != itemMgr._dataMap.end(); ++it)
	{
		DoodadManager::getSingleton().AddDoodadDropTemplate(it->first, it->second);
	}
}

const FDropGroup* FDoodadDropSet::GetDropGroup() const
{
	return MMODropGroupDataManager::getSingleton().FindData(DropGroupID);
}

#if WITH_EDITOR
void DoodadDropDataProcessor::CollectReferencedResourceFromDataTable( FDoodadDropData* obj, TSet<FString>& outResList )
{
	UObject* dat = obj->GetDoodadTempalte();
	if(dat)
	{
		_CollectReferencedResource(dat, outResList);
	}
}

void DoodadDataHolderDataProcessor::CollectReferencedResourceFromDataTable( FDoodadDataHolder* obj, TSet<FString>& outResList )
{
	UDoodad* dat = obj->GetDoodadTemplate();
	if(dat)
	{
		_CollectReferencedResource(dat, outResList);
	}
}
#endif

UDoodad* FDoodadDataHolder::GetDoodadTemplate()
{
	if(DoodadTemplate)
	{
		return DoodadTemplate;
	}

	DoodadTemplate = LoadUEObject<UDoodad>(DoodadObjectPath, TEXT("doodad.txt"));

	return DoodadTemplate;
}


void DoodadDataHolderDataProcessor::ParseData( FDoodadDataHolder* obj, class CTabFile* dataRow, int rowIdx )
{
	obj->DoodadObjectPath = TReadDataValue_FString<MMODoodadTemplateDataManager>(TEXT("Index"), dataRow, rowIdx);	
}

void DoodadDataHolderDataProcessor::PostInitData()
{
}