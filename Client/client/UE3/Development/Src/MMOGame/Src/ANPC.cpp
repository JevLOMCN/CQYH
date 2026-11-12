#include "stdafx.h"
//#include "MMOGameClasses.h"
#include "MMOFacade.h"
#include "EngineAudioDeviceClasses.h"
#include "MMOQuestManager.h"
#include "MMOGuildManager.h"
#include "MMOMapDataManager.h"

//#define MaxDistance 2500
//song@snda.com

FVector ANPC::GetInteractActorLocation()
{
	return Location;
}

FLOAT ANPC::GetMaxInteractRadius()
{
	return MMOFacade::getSingleton().GetMainController()->InteractDistance;
}

void ANPC::SwitchFightIdle(UBOOL bFight)
{
	AMMOActor::SwitchFightIdle(bFight);

	if (bFight)
	{
		if (Template)
		{
			PlayActions(Template->EnterFightEffects);
		}
	}
	else
	{
		if (Template)
		{
			PlayActions(Template->ExitFightEffects);
		}
	}
}

void ANPC::SetDead()
{
	AMMOActor::SetDead();

	if (bFightingState)
	{
		if (Template)
		{
			PlayActions(Template->ExitFightEffects);
		}
	}

#if 0	// Cannot be selected after death! See AMMOActor::GetPickPriority
	PickPriority = EPP_MAX;

	SET_WARN_COLOR_AND_BACKGROUND(COLOR_DARK_BLUE, COLOR_WHITE);
	warnf(NAME_Log, TEXT("NPC Die:%s %s"), *GetName(), Template ? *Template->GetDisplayName() : TEXT(""));
	CLEAR_WARN_COLOR();
#endif
}

FString ANPC::GetDebugInfo()
{
	FName AnimName;
	UBOOL bRet = this->GetCurrentAnimName(AnimName);
	if (!bRet)
	{
		AnimName = NAME_None;
	}

	UAvatarData* CurrentAvatar = UseingAvatar;
	if (MeshChangeAvatarData)
	{
		CurrentAvatar = MeshChangeAvatarData;
	}

	FString RetString = FString::Printf(
		TEXT("Id(%d), TemplateId(%d), CSpeed(%.1f), WSpeed(%.1f), RSpeed(%.1f), Anim(%s)"),
		ServerID,
		(Template ? Template->TemplateID : 0),
		this->Velocity.Size2D(),
		(CurrentAvatar ? CurrentAvatar->WalkSpeed : -1),
		(CurrentAvatar ? CurrentAvatar->RunSpeed : -1),
		*(AnimName.ToString())
		);
	return RetString;
}


void ANPC::InitNpcByTemplate(class UNPCTemplate* InTemplate)
{
	check(InTemplate);
	Template = InTemplate;

	if (SceneObjectAppearType == 3 || SceneObjectAppearType == 4)
	{
		PlayActions(Template->PreSpawnActions);
	}

	UseingAvatar = InTemplate->AvatarTemplate;

	NPCIcon = InTemplate->NPCIcon;
	HUDDisplayIcon = InTemplate->HUDDisplayIcon;
	IdentityIcon = InTemplate->IdentityIcon;
	IdentityName = InTemplate->IdentityName;

	ActorName = InTemplate->ActorName;

	PawnAnimInfo = InTemplate->PawnAnimInfo;

	DefaultDialogText = InTemplate->DefaultDialogText;

	if (OverrideActorType != EACT_None)
	{
		ActorType = OverrideActorType;
		OverrideActorType = EACT_None;
	}
	else
	{
		ActorType = InTemplate->ActorType;
	}

	DisplayName = InTemplate->GetDisplayName();
	QuestIconName= FString("");

	SpawnProcList = InTemplate->SpawnProcList;
	TeleportProcList = InTemplate->TeleportProcList;
	RebirthProcList = InTemplate->RebirthProcList;
	ActionProcList = InTemplate->ActionProcList;
	TickProcList = InTemplate->TickProcList;

	SpecialDyingActions = InTemplate->DeathComponents;

	LeftTimeToCheckAboutRestAnim = RandHelper(InTemplate->RandomRestAnimCycleTime);

	NPCID = InTemplate->GetTemplateID();
	ShopID = InTemplate->ShopID;
	ForceID = InTemplate->ForceID;
	m_bIsWarehouse = InTemplate->IsWarehouse;

	if(ActorAttributes)
	{
		Health = ActorAttributes->GetMaxHp();	
	}

	InitItems();

	RefreshAvatar();

	RebuildMaterialControllerList();

//	RefreshCollision();

	RefreshCollisonRate();

	eventInitAnimNodes();
	RefreshAnimSet();
	RefreshMovementSpeed();
	InitBodySize();

	if(SceneObjectAppearType == ESOAT_Spawn)
	{
		ProcessStatusProc(Template->SpawnProcList);
	}

	if (Template && Template->Sound_Spawn.Num() > 0)
	{
		PlayRandomSound(Template->Sound_Spawn);
	}

	// play spawn actions
	if(SceneObjectAppearType == 3 || SceneObjectAppearType == 4)
	{
		PlayActions(Template->SpawnActions);
	}


	// add interactive wind
	if( Template->WindStrength > 0.1f )
	{
		//`Log( "Add interactive wind for npc"@self );
		AddInteractiveWind( Template->WindStrength );
	}
	eventSetUsePlaneShadow( Template->UsePlaneShadow );
	eventCheckPlaneShadow();
}


void ANPC::DoInteract()
{
	//// PICK disabled also means no interaction
	//if(Template && Template->DisablePick)
	//{
	//	return;
	//}

	//// Interaction disabled while cutscene plays
	//AMMOPlayerController* mainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
	//if (mainPlayerCtrl && mainPlayerCtrl->IsPlayingMatinee())
	//{
	//	return;
	//}

	//UBOOL isAttackAble = MMOFacade::getSingleton().IsAttackable(this);
	//// Cannot interact if attackable
	//if(isAttackAble)
	//{
	//	return;
	//}	
	//else if(!isAttackAble && ((ActorType == EACT_Pet)  || (ActorType == EACT_PlayerSelf ))) // Friendly summons cannot be interacted with
	//{ 
	//	return;
	//}
	//
	//// Only non-monsters can be interacted with
	//if(!IsMonster())
	//{
	//	CMessageHandler::GetInstance().Send_InteractNpc(ServerID);
	//}

	if (CanInteract())
	{
		CMessageHandler::GetInstance().Send_InteractNpc(ServerID);
	}
}

UBOOL ANPC::CanGuildBuildingInteract() const
{
	// fix bug #23046
	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	INT curTerritoryId = MMOGuildManager::getSingleton().GetCurTerritorySceneId();

	if (baseInfo.Id > 0)
	{// Has guild
		if (baseInfo.Id != curTerritoryId)
		{// In another's territory
			if (MMOGuildData::getSingleton().IsGuildAlly(curTerritoryId))
			{// Is allied
				if (Template)
				{
					if (Template->BuildingInteractType == EGBIT_ONLY_SELF ||
						Template->BuildingInteractType == EGBIT_ONLY_ENEMY)
					{
						return FALSE;
					}
				}
			}
			else
			{// Not allied
				if (Template)
				{
					if (Template->BuildingInteractType == EGBIT_ONLY_SELF ||
						Template->BuildingInteractType == EGBIT_ONLY_ALLY)
					{
						return FALSE;
					}
				}
			}
		}
	}
	else
	{// No guild, cannot interact with territory NPC
		return FALSE;
	}

	return TRUE;
}

UBOOL ANPC::CanInteract() const
{
	//if(Template && Template->DisablePick)
	//{
	//	return FALSE;
	//}

	//return TRUE;
	// PICK disabled also means no interaction
	if(Template && Template->DisablePick)
	{
		return FALSE;
	}

	UBOOL isAttackAble = MMOFacade::getSingleton().IsAttackable(this);
	// Cannot interact if attackable
	if(isAttackAble)
	{
		return FALSE;
	}	
	else if(!isAttackAble && ((ActorType == EACT_Pet)  || (ActorType == EACT_PlayerSelf ))) // Friendly summons cannot be interacted with
	{ 
		return FALSE;
	}

	if (IsMonster())
	{
		return FALSE;
	}

	////Cannot interact beyond valid interaction distance
	//INT CurDistance = (this->Location - mainPlayerCtrl->Location).Size2D();

	//if (CurDistance > MaxDistance)
	//	return FALSE;

	//If interacting with a guild territory NPC
	INT curMapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	if (curMapId == GUILD_TERRITORY_MAP_ID)
	{
		if (!CanGuildBuildingInteract())
		{
			return FALSE;
		}
	}

	return TRUE;
}

UBOOL ANPC::IsMonster() const
{
	return ActorType == EACT_Boss
		|| ActorType == EACT_Monster;
}

INT32 ANPC::CalGridBlockRadius() // zhaojun@snda
{
	if (bDead)
	{
		return -1;
	}
	else
	{
		if (Template != NULL)
		{
			const FString& bt = Template->BodyType;

			check(*((*bt) + 4) >= TEXT('0') && *((*bt) + 4) <= TEXT('9'));

			CurBlockType = *((*bt) + 4) - TEXT('0');
			if (ActorType == EACT_Boss)
			{
				return (CurBlockType == 0 ? (0) : (CurBlockType / 2));
			}
			else
			{
				return (CurBlockType == 0 ? (-1) : (CurBlockType / 2));
			}
		}
		else
		{
			CurBlockType=1;
			return CurBlockType / 2;
		}
	}
}

UBOOL ANPC::IsNeedArchetype()
{
	return false;
}

BYTE ANPC::GetPickPriority()
{
	if(bDead || (Template && Template->DisablePick))
	{
		return EPP_MAX;
	}

	//if (Master == MMOFacade::getSingleton().GetMainActor())
	//{
	//	return EPP_Low;
	//}

	UBOOL isAttackAble = MMOFacade::getSingleton().IsAttackable(this);
	// If attackable, return as NPC directly
	if(isAttackAble)
	{
		//return EPP_NPC;
		return EPP_Normal;
	}
	//else if((ActorType == EACT_Pet) || (ActorType == EACT_PlayerSelf))  // Friendly summons cannot be interacted with
	//{
	//	return EPP_MAX;
	//}

	if (CanInteract())
	{
		return EPP_High;
	}

	// If no default dialog is configured, do not select
// 	if(Template && Template->DefaultDialogText.IsEmpty())
// 	{
// 		return EPP_MAX;
// 	}

	return EPP_Normal;
}


// oscar@snda
BYTE ANPC::GetMouseCursor()
{
	if(Template && Template->DisablePick)
	{
		return EMC_Arrow;
	}

	UBOOL isAttackAble = MMOFacade::getSingleton().IsAttackable(this);

	if(isAttackAble)
	{
		if(eventIsDead()==FALSE)
		{
			return EMC_Attack;
		}
		else
		{
			return EMC_Arrow;
		}
	}
	else if((ActorType == EACT_Pet) || (ActorType == EACT_PlayerSelf) || (ActorType == EACT_Monster) )  // Friendly summons cannot be interacted with
	{
		return EMC_Arrow;
	}

	switch(ActorType)
	{
	case EACT_None:
		return EMC_Arrow;
	case EACT_SceneObject:
		return EMC_Pick;
	}

	return EMC_Talk;
}
// oscar@snda

//Bruce@snda
void ANPC::RefreshCollision()
{
	float NPCGlobalScale = GWorld->GetWorldInfo()->NpcGlobalScale;
	UAvatarData* CurrentAV = UseingAvatar;
	if ( MeshChangeAvatarData )
	{
		CurrentAV = MeshChangeAvatarData;
	}

	if ( CurrentAV )
	{
		SetDrawScale( CurrentAV->MeshScale * CurrentAV->AvatarDrawScale * NPCGlobalScale );
		SetCollisionSize( CurrentAV->CollisionRadius * CurrentAV->AvatarDrawScale * NPCGlobalScale, 
			CurrentAV->CollisionHeight * CurrentAV->AvatarDrawScale * NPCGlobalScale );
		UpdateLocationZ();
	}
}
//Bruce@snda

void ANPC::DetachHeadUp()
{
	HeadUpType = EQHUDS_None;
	DetachComponent( HeadUpComponent );
}

void ANPC::ProcessStatusProc(const TArray<struct FStatusProcData>& procs)
{
	FLOAT RandNumb = RandRange(0, 1);
	for (INT i = 0; i < procs.Num(); ++i)
	{
		if(RandNumb <= procs(i).Rate)
		{
			for (INT j = 0; j < procs(i).Proc.Num(); ++j)
			{
				procs(i).Proc(j)->eventOnExecute(this);
			}
			break;
		}

		RandNumb -= procs(i).Rate;
	}
}

void ANPC::SetSilhouette( UBOOL show, FVector sc )
{
	if(Template && Template->DisablePick)
	{
		return;
	}

	if(bDead)
	{
		show = FALSE;
	}

	AMMOActor::SetSilhouetteEx(show, sc);

	for(INT i = 0; i < AttachedDestructableParts.Num(); ++i)
	{
		if(AttachedDestructableParts(i)->IsStaticMeshComponent)
		{
			AttachedDestructableParts(i)->MeshComponent->UpdateSilhouetteState( show, sc );
			//AttachedDestructableParts(i)->MeshComponent->ShowSilhouette = show;
			//AttachedDestructableParts(i)->MeshComponent->SilhouetteColor = sc;
		}
		else
		{
			AttachedDestructableParts(i)->SkelMeshComponent->UpdateSilhouetteState( show, sc );
			//AttachedDestructableParts(i)->SkelMeshComponent->ShowSilhouette = show;
			//AttachedDestructableParts(i)->SkelMeshComponent->SilhouetteColor = sc;
		}
	}
}

//------------------------------------------------------------------------
void ANPC::DataToExcel(TArray<FString>& Row)
{
	////Row.AddItem(TEXT("_IMPORT_"));
	//Row.AddItem(FString::Printf(TEXT("%d"), NPCID));
	//Row.AddItem(NameID.ToString());
	//Row.AddItem(DisplayName);

	//Row.AddItem(FString::Printf(TEXT("%d"), Level));
	//Row.AddItem(FString::Printf(TEXT("%d"), MaxHP));
	//Row.AddItem(FString::Printf(TEXT("%d"), MaxMP));
	//Row.AddItem(FString::Printf(TEXT("%d"), Speed));
	//Row.AddItem(FString::Printf(TEXT("%d"), Radius));
	//Row.AddItem(FString::Printf(TEXT("%d"), ServerSpawnTime));
	//Row.AddItem(FString::Printf(TEXT("%d"), ReviveTime));
	//Row.AddItem(FString::Printf(TEXT("%d"), CorpseTime));
	//Row.AddItem(FString::Printf(TEXT("%d"), RespawnTime));
	//if(AIData != NULL)
	//{
	//	Row.AddItem(FString::Printf(TEXT("%d"), AIData->TemplateID));	
	//}
	//else	
	//{
	//	Row.AddItem(TEXT(""));
	//}

	//Row.AddItem(FString::Printf(TEXT("%d"), Exp));
	//Row.AddItem(FString::Printf(TEXT("%d"), ForceID));
	//Row.AddItem(NPCScriptName);
	//Row.AddItem(AIScriptName);

	//for(INT i = 0; i < DropItemSetList.Num(); ++i)
	//{
	//	if(DropItemSetList(i).DropGroup != NULL)
	//	{
	//		Row.AddItem( FString::Printf(TEXT("%d"), DropItemSetList(i).Weight) );
	//		Row.AddItem( FString::Printf(TEXT("%d"), DropItemSetList(i).DropGroup->TemplateID) );
	//	}
	//	else
	//	{
	//		Row.AddItem(TEXT(""));
	//		Row.AddItem(TEXT(""));
	//	}
	//}
	//Row.AddItem(FString::Printf(TEXT("%d"), WeaponPhyAtkHi));
	//Row.AddItem(FString::Printf(TEXT("%d"), WeaponPhyAtkLo));
	//Row.AddItem(FString::Printf(TEXT("%d"), WeaponMagAtkHi));
	//Row.AddItem(FString::Printf(TEXT("%d"), WeaponMagAtkLo));
	//Row.AddItem(FString::Printf(TEXT("%d"), ShopID));
	//Row.AddItem(FString::Printf(TEXT("%d"), ImmunePassiveMove));
	//Row.AddItem(FString::Printf(TEXT("%d"), Invincible));
	//Row.AddItem(FString::Printf(TEXT("%d"), AllwaysRun));

}
//------------------------------------------------------------------------
void ANPC::ExportExcelHeaders(TArray<FString>& Header, TArray<FString>& DefaultValues)
{
	Header.AddItem(TEXT("TemplateID"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("NameID"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("DisplayName"));
	DefaultValues.AddItem(TEXT("N/A"));

	Header.AddItem(TEXT("Level"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("MaxHp"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("MaxMp"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("Speed"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("Radius"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("SpawnTime"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("ReviveTime"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("CorpseTime"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("RespawnTimeMin"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("RespawnTimeMax"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("AIID"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("Exp"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("ForceID"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("NPCScriptName"));
	DefaultValues.AddItem(TEXT(""));

	//Header.AddItem(TEXT("AIScriptName"));
	//DefaultValues.AddItem(TEXT(""));

	for(INT i = 0; i < DropItemSetList.Num(); ++i)
	{
		Header.AddItem(FString::Printf(TEXT("DropChance%d"), i + 1));
		DefaultValues.AddItem(TEXT("0"));

		Header.AddItem(FString::Printf(TEXT("DropGroupID%d"), i + 1));
		DefaultValues.AddItem(TEXT("0"));
	}

	Header.AddItem(TEXT("WeaponPhyAtkHi"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("WeaponPhyAtkLo"));
	DefaultValues.AddItem(TEXT("0"));
	
	Header.AddItem(TEXT("WeaponMagAtkHi"));
	DefaultValues.AddItem(TEXT("0"));
	
	Header.AddItem(TEXT("WeaponMagAtkLo"));
	DefaultValues.AddItem(TEXT("0"));
	
	Header.AddItem(TEXT("ShopID"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("ImmunePassiveMove"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("Invincible"));
	DefaultValues.AddItem(TEXT("0"));

	Header.AddItem(TEXT("AllwaysRun"));
	DefaultValues.AddItem(TEXT("0"));

}
void ANPC::InitDataToExcel()
{
}
int ANPC::GetTemplateID() const
{
	return NPCID;
}
//------------------------------------------------------------------------
void ANPC::ImportFromExcel(const TArray<FString>& Values)
{	
	NPCID = appAtoi(*Values(0));
}
//------------------------------------------------------------------------
void ANPC::DebugNativePostBeginPlay()
{
#ifdef _DEBUG
	int i = 0;
	++i;

#endif
}

void ANPC::TickStatusProc(const TArray<FStatusProcData>& procs )
{
//	int i, j;
	float randNumb;
	
	//`log("********************* ProcessStatusProc" @self);
	//Random///////////////////////////////////////////////////////////////////////////////////
	randNumb = RandRange(0, 1);
	//Get the randomly selected index
	for(int i = 0; i < procs.Num(); ++i)
	{
		if(randNumb <= procs(i).Rate)
		{ 
			for(int j = 0; j < procs(i).Proc.Num(); ++j)
			{
				procs(i).Proc(j)->Update(this);
			}
			break;
		}
		randNumb -= procs(i).Rate;
	}

	if(randNumb <= 0.0)
	{
		//`log("********************* ProcessStatusProc error: randNumb<=0.0 " @self);
	}
}

//-----------------------------------------------------------------------------------
FString ANPC::GetIdentityFullPath()
{
	if(!IdentityIcon)
	{
		return TEXT("");
	}

	FString FullName;

	UObject* Obj = IdentityIcon;
	FullName = FString(Obj->GetPureName().ToString() );
	while( Obj && Obj->GetOuter() )
	{
		FullName = FString(Obj->GetOuter()->GetPureName().ToString()) + TEXT(".") + FullName;
		Obj= Obj->GetOuter();
	}
	//int i = 0;
	//FString str = FString( DisplayIcon->GetOutermost() ->GetOut());// ->TextureFileCacheName.ToString() );
	return FullName;
}

void ANPC:: InitBodySize()
{
	if (Template != NULL)
	{
		const FString& bt = Template->BodyType;

		check(*((*bt) + 4) >= TEXT('0') && *((*bt) + 4) <= TEXT('9'));

		INT32 n = *((*bt) + 4) - TEXT('0');

		if(n>=3)
		{
			BodySize = E_body_3x3;
		}
		else
		{
			BodySize = E_body_1x1;
		}
	}
	else
	{
		BodySize = E_body_1x1;
	}
}

void ANPC::PlayRandomAnim()
{
	if (!Template)
	{
		return;
	}

	if (IsPlayingAnyAnimation())
	{
		return;
	}

	if (isMoving)
	{
		return;
	}

	TArray<FName>* SeqList = NULL;
	if (bFightingState)
	{
		SeqList = &(Template->PawnAnimInfo.FightIdleRestAnims);
	}
	else
	{
		SeqList = &(Template->PawnAnimInfo.IdleRestAnims);
	}

	if (SeqList->Num() == 0)
	{
		return;
	}

	FLOAT p = appFrand();
	FLOAT TemplateRate = bFightingState ? Template->FightRandomRestAnimPlayRate : Template->RandomRestAnimPlayRate;
	if (p > TemplateRate)
	{
		return;
	}

	INT idx = RandHelper(SeqList->Num());
	if ((*SeqList)(idx) == NAME_None)
	{
		return;
	}

	// Play the corresponding animation sound effect
	if(Template && Template->AvatarTemplate && CanPlayAnimSoundNow())
	{
		FString aniName = (*SeqList)(idx).ToString();
		if(aniName.Len() > 1)
		{
			const TCHAR* numberChar = &(aniName[aniName.Len() - 1]);
			if(numberChar)
			{
				INT soundIdx = appAtoi(numberChar);
				if(soundIdx < Template->AvatarTemplate->Sound_Idle.Num())
				{
					if(Template->AvatarTemplate->Sound_Idle(soundIdx))
					{
						StartAnimSoundPlayCD();
						PlaySound(Template->AvatarTemplate->Sound_Idle(soundIdx));
					}
				}
			}
		}
	}

	
	PlayAnimation((*SeqList)(idx));
}

void ANPC::ResetIdleTimer()
{
	if (Template)
	{
		LeftTimeToCheckAboutRestAnim = bFightingState ? Template->FightRandomRestAnimCycleTime : Template->RandomRestAnimCycleTime;
	}
}


bool ANPC::CanPlayOpenSoundNow() const
{
	return CurrentOpenSoundCDTime <= 0.0f;
}

bool ANPC::CanPlayCloseSoundNow() const
{
	return CurrentCloseSoundCDTime <= 0.0f;
}

bool ANPC::CanPlayAnimSoundNow() const
{
	return CurrentAnimSoundCDTime <= 0.0f;
}

void ANPC::StartOpenSoundPlayCD()
{
	CurrentOpenSoundCDTime = SoundCDTime;
}

void ANPC::StartCloseSoundPlayCD()
{
	CurrentCloseSoundCDTime = SoundCDTime;
}

void ANPC::StartAnimSoundPlayCD()
{
	CurrentAnimSoundCDTime = SoundCDTime;
}

void ANPC::TickSpecial(FLOAT Delta)
{
	AMMOActor::TickSpecial(Delta);

	if (Template)
	{
		if(CurrentOpenSoundCDTime > 0.0f)
		{
			CurrentOpenSoundCDTime -= Delta;
		}

		if(CurrentCloseSoundCDTime > 0.0f)
		{
			CurrentCloseSoundCDTime -= Delta;
		}

		if(CurrentAnimSoundCDTime > 0.0f)
		{
			CurrentAnimSoundCDTime -= Delta;
		}

		if (!bDead)
		{
			LeftTimeToCheckAboutRestAnim -= Delta;
			if (LeftTimeToCheckAboutRestAnim <= 0)
			{
				LeftTimeToCheckAboutRestAnim = bFightingState ? Template->FightRandomRestAnimCycleTime : Template->RandomRestAnimCycleTime;
				PlayRandomAnim();
			}
		}

	}

	//// to do : check whick npc id is -1
	//if (!IsPendingKill() && ServerID != -1)
	//{
	//	AMMOActor* mainActor = MMOFacade::getSingleton().GetMainPlayer();
	//	if (mainActor)
	//	{
	//		INT MainActorX = mainActor->Location.X / 2048.f;
	//		INT MainActorY = mainActor->Location.Y / 2048.f;

	//		INT GridX = Location.X / 2048.f;
	//		INT GridY = Location.Y / 2048.f;

	//		UBOOL bKill = FALSE;
	//		if (abs(MainActorX - GridX) >= 3)
	//		{
	//			bKill = TRUE;
	//		}

	//		if (abs(MainActorY - GridY) >= 3)
	//		{
	//			bKill = TRUE;
	//		}

	//		if (bKill)
	//		{
	//			MMOFacade::getSingleton().DestroyObject(ServerID, TRUE);
	//		}
	//	}


	//}
}

void ANPC::PlayRandomSound(const TArray<class USoundCue*>& soundList)
{
    //DWORD nt = GetTickCount(); // This time function is not very precise
    //if (nt < (DWORD)NextSoundTime)
    //    return;
    //// Designer requires NPC to play only one sound regardless of when or what type
    //if (soundList.Num() > 0)
    //{
    //    // By default Actor handles randomness, but here to know this sound's length, randomize once actively and pass only the result
    //    TArray<class USoundCue*> tmp;
    //    INT idx = (INT)RandRange(0, soundList.Num() - 1);
    //    tmp.AddItem(soundList(idx));
    //    (DWORD&)NextSoundTime = nt + (DWORD)((soundList(idx)->Duration - 0.1f) * 1000.0f);
    //    __super::PlayRandomSound(tmp);
    //}
    //else
        __super::PlayRandomSound(soundList);
}

// add by zhangrui05@snda
void ANPC::PlayTalkSound()
{
	if (Template && Template->AvatarTemplate && Template->AvatarTemplate->Sound_Talk.Num() > 0)
	{
		INT talkAnimIdx = RandHelper(Template->AvatarTemplate->Sound_Talk.Num());
		if (talkAnimIdx < Template->AvatarTemplate->Sound_Talk.Num())
		{
			USoundCue* sound = Template->AvatarTemplate->Sound_Talk(talkAnimIdx);
			if (sound)
			{
				MMOFacade::getSingleton().StopAllVoiceSound(TRUE, 0.0, 0.0);
				PlaySound(sound);
			}
		}

		FString talkAnim = FString(TEXT("Talk"))/*NPC->Template->PawnAnimInfo.AnimTalkKey.ToString()*/ + appItoa(talkAnimIdx);
		FLOAT playedTime = PlayAnimation(FName(*talkAnim));

		SET_WARN_COLOR(COLOR_CYAN);
		warnf(TEXT("NPC Diag Rand:%d TalkSounds:%d Anim:%s AnimLen:%f"),
			talkAnimIdx, Template->AvatarTemplate->Sound_Talk.Num(),
			*talkAnim, playedTime);
		CLEAR_WARN_COLOR();
	}
}

void ANPC::PlayEventSound(USoundCue* sound)
{
	MMOFacade::getSingleton().StopAllVoiceSound(FALSE, 0.5, 0.0);

	UAudioComponent* audioComponent = UAudioDevice::CreateComponent(sound, GWorld->Scene, this, 0, TRUE);
	if (audioComponent)
	{
		audioComponent->Location = this->Location;
		audioComponent->bUseOwnerLocation = TRUE;
		audioComponent->bStopWhenOwnerDestroyed = TRUE;
		audioComponent->bAllowSpatialization = TRUE;
		audioComponent->bIsUISound	= TRUE;
		audioComponent->bAutoDestroy = 1;
		audioComponent->SubtitlePriority = 10000;
		audioComponent->Play();
	}
}

void ANPC::ProcessDefaultDie(BYTE DieCause)
{
	AMMOActor::ProcessDefaultDie(DieCause);
	if (Template && Template->DefaultDeathComponents.Num() > 0)
	{
		PlayActions(Template->DefaultDeathComponents);
	}

	//PickPriority = EPP_UnTouchBody;

	eventTriggerEvent(0);
}

void ANPC::PlayActionById(INT ActionId)
{
	if (Template)
	{
		for (INT i = 0; i < Template->Actions.Num(); ++i)
		{
			if (ActionId == Template->Actions(i).ActionId)
			{
				PlayActions(Template->Actions(i).Actions);
				return;
			}
		}
	}
}

void ANPC::DrawHUDPostGFx( AHUD* H )
{
#if M_DEBUG_HUD_NAME
	FVector screenPos;
	FVector objectIconWorldPos;


	objectIconWorldPos = Location;//(HealthBarPositionOffset>>Rotation);//actual position
	objectIconWorldPos.Z += 120;
	screenPos = H->Canvas->Project(objectIconWorldPos);

	H->Canvas->SetDrawColor(255, 0, 0, 255);
	//H->Canvas->SetPos(drawX, drawY);
	FVector2D drawPos(screenPos.X, screenPos.Y);
	DrawBox2D(H->Canvas->Canvas, drawPos, drawPos + FVector2D(100, 10), FLinearColor(1, 0, 0));

#endif
}

UBOOL ANPC::DrawMyHUD(class AMMOHud* H)
{
	if (MainController && MainController->bDrawAllNpcInfo)
	{
		FString DebugInfo = GetDebugInfo();
		FVector BaseLocation = Location + FVector(0, 0, this->GetCollisionHeight());
		FVector ScreenPos = H->Canvas->Project(BaseLocation);
		H->Canvas->SetPos(ScreenPos.X, ScreenPos.Y);
		H->Canvas->SetDrawColor(255, 0, 0);
		H->Canvas->DrawText(DebugInfo);
	}
#if M_DEBUG_HUD_NAME
	FVector screenPos;
	FLOAT healthPercent;
	FLinearColor healthColor;
	FVector healthColorVec;
	FVector objectIconWorldPos;
	FLOAT cameraDistFromPos;
	FLOAT iconWidth, iconHeight;
	FLOAT IconDistanceScale;
	INT drawX, drawY;

	objectIconWorldPos = Location;//actual position
	objectIconWorldPos.Z += 120;
	screenPos = H->Canvas->Project(objectIconWorldPos);

	screenPos.X = appRound(screenPos.X);
	screenPos.Y = appRound(screenPos.Y);

	cameraDistFromPos = objectIconWorldPos.Distance( H->PlayerOwner->PlayerCamera->ViewTarget.POV.Location );
	IconDistanceScale = Clamp<FLOAT>( 1.0 - cameraDistFromPos/4000 , 0.25 , 1 ) * ( H->Canvas->ClipY / 768.0f) * HealthBarScale;

	iconWidth = 166*IconDistanceScale;
	iconHeight = 18*IconDistanceScale;

	AMMOGame* game = Cast<AMMOGame>( GWorld->GetGameInfo() );
	if( game && game->DrawDebugActorState )
	{
		H->Canvas->SetDrawColor(255, 0, 0, 255);

		drawY = screenPos.Y - iconHeight/2;
		drawX = screenPos.X - iconWidth/2;
		H->Canvas->SetPos(drawX, drawY);
		FString drawTxt = GetPureName().ToString() + TEXT(" Ctrl:[") + Controller->GetStateName().ToString() + TEXT("] Actor:[") + GetStateName().ToString() + TEXT("]");
		H->Canvas->DrawText( drawTxt );

		drawY -= 20;
		H->Canvas->SetPos(drawX, drawY);
		drawTxt = TEXT("[");
		drawTxt += GetPhysicsName() + TEXT("][") + appItoa( CollisionType ) + TEXT("][") + appItoa(Mesh->RBChannel) + TEXT("]");
		H->Canvas->DrawText( drawTxt );

		drawY -= 40;
		H->Canvas->SetPos(drawX, drawY);
		drawTxt = FString::Printf( TEXT("Server Dest: %f %f %f Current Loc:%f %f %f Anim:") , ServerMoveDest.X , ServerMoveDest.Y , ServerMoveDest.Z , Location.X , Location.Y , Location.Z ) ;
		drawTxt += PlayingAnim.ToString();
		H->Canvas->DrawText( drawTxt );

		//drawY -= 20;
		//H.Canvas.SetPos(drawX, drawY);
		//H.Canvas.DrawText("BlockingVolume = [" $ Mesh.RBCollideWithChannels.BlockingVolume $ "] Cloth = [" $ Mesh.RBCollideWithChannels.Cloth $ "]");

		//drawY -= 20;
		//H.Canvas.SetPos(drawX, drawY);
		//H.Canvas.DrawText("Clothing = [" $ Mesh.RBCollideWithChannels.Clothing $ "] ClothingCollision = [" $ Mesh.RBCollideWithChannels.ClothingCollision $ "]");

		//drawY -= 20;
		//H.Canvas.SetPos(drawX, drawY);
		//H.Canvas.DrawText("DeadPawn = [" $ Mesh.RBCollideWithChannels.DeadPawn $ "] Default = [" $ Mesh.RBCollideWithChannels.Default $ "]");

		//drawY -= 20;
		//H.Canvas.SetPos(drawX, drawY);
		//H.Canvas.DrawText("EffectPhysics = [" $ Mesh.RBCollideWithChannels.EffectPhysics $ "] FluidDrain = [" $ Mesh.RBCollideWithChannels.FluidDrain $ "]");

		//drawY -= 20;
		//H.Canvas.SetPos(drawX, drawY);
		//H.Canvas.DrawText("FracturedMeshPart = [" $ Mesh.RBCollideWithChannels.FracturedMeshPart $ "] FluidDrain = [" $ Mesh.RBCollideWithChannels.GameplayPhysics $ "]");

		//drawY -= 20;
		//H.Canvas.SetPos(drawX, drawY);
		//H.Canvas.DrawText("Nothing = [" $ Mesh.RBCollideWithChannels.Nothing $ "] Pawn = [" $ Mesh.RBCollideWithChannels.Pawn $ "]");

		//drawY -= 20;
		//H.Canvas.SetPos(drawX, drawY);
		//H.Canvas.DrawText("Untitled1 = [" $ Mesh.RBCollideWithChannels.Untitled1 $ "] Untitled2 = [" $ Mesh.RBCollideWithChannels.Untitled2 $ "]");

		//drawY -= 20;
		//H.Canvas.SetPos(drawX, drawY);
		//H.Canvas.DrawText("Untitled3 = [" $ Mesh.RBCollideWithChannels.Untitled3 $ "] Untitled4 = [" $ Mesh.RBCollideWithChannels.Untitled4 $ "]");

		//drawY -= 20;
		//H.Canvas.SetPos(drawX, drawY);
		//H.Canvas.DrawText("Vehicle = [" $ Mesh.RBCollideWithChannels.Vehicle $ "] Water = [" $ Mesh.RBCollideWithChannels.Water $ "]");

	}

// 	if(!EnableDrawHealthBar || ( ActorAttributes->GetCurHp() <= 0) ||
// 		!IsMonster() || ( game && !game->ShowNPCHP ) )
// 		return false;
// 
// 
// 	if( !H->IsOnCanvas(screenPos,iconWidth,iconHeight) )
// 		return false;

	healthPercent = GetHealthPercent();
	healthColorVec = Lerp(UnhealthyColor,HealthyColor,healthPercent);

	if( TimeSince(LastTakeDamageTime) < 2 )
		healthColorVec *= 1 + 13.0 * abs( sin( GWorld->GetTimeSeconds()*7 ) );

	healthColor.R = healthColorVec.X;
	healthColor.G = healthColorVec.Y;
	healthColor.B = healthColorVec.Z;

	MyHealthHUDMatInstance->SetScalarParameterValue( "Percent" , healthPercent );
	MyHealthHUDMatInstance->SetVectorParameterValue( "BarColor" , healthColor );

	//H->Canvas->SetPos( screenPos.x-iconWidth/2 , screenPos.y-iconHeight/2 );
	//H->Canvas->DrawMaterialTile(MyHealthHUDMatInstance, iconWidth,iconHeight);
	H->Canvas->DrawMaterialTile( MyHealthHUDMatInstance , screenPos.X-iconWidth/2 , screenPos.Y-iconHeight/2 , 1 , iconWidth , iconHeight , 0 , 0 , iconWidth , iconHeight );
#endif
	return true;
}


FString ANPC::GetDetailedDescription( INT InIndex )
{
	//return eventGetDetailedDescription(InIndex);
	switch( InIndex )
	{
	case 0:
		return FString::Printf(TEXT("%d"), NPCID);
		break;
	case 1:
		return NameID.ToString();
		break;
	case 2:
		return DisplayName;
		break;
	}

	return TEXT("");
}