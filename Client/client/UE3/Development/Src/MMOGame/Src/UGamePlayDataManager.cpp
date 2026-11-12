#include "stdafx.h"
#include "FileHelpers.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"

IMPLEMENT_CLASS(UModelData);
IMPLEMENT_CLASS(UGamePlayDataManager);
IMPLEMENT_CLASS(UDamageProcessData);

UGamePlayDataManager* GGameplayDataMgr = 0;

void UGamePlayDataManager::InitGamePlayData()
{
	// init footstep sound/effect
	INT key = 0;
	for (INT i = 0; i < FootstepEffectList.Num(); ++i)
	{
		key = 1;	// footstep resource type is 1
		key = (key<<8) + INT(FootstepEffectList(i).ShapeType);
		key = (key<<8) + INT(FootstepEffectList(i).LandMatType);
		MMOFacade::getSingleton().AddSoundParticle(key, &(FootstepEffectList(i).EffectNode));
	}

	if(GIsEditor)
	{
		return;
	}

	for (INT i = 0; i < HitEffectList.Num(); ++i)
	{
		key = 2;	// hit sound effect resource type is 2
		key = (key<<8) + INT(HitEffectList(i).HitType);
		key = (key<<8) + INT(HitEffectList(i).TargetArmorMatType);

		MMOFacade::getSingleton().AddHitSoundGroup(key, HitEffectList(i).ToPlaySound);
	}
	for (INT i = 0; i < AnimMixList.Num(); ++i)
	{
		key = 3;	// hit sound effect resource type is 2
		key = (key<<8) + INT(AnimMixList(i).SourceAnim);
		key = (key<<8) + INT(AnimMixList(i).TargetAnim);

		MMOFacade::getSingleton().AddAnimMix(key, &(AnimMixList(i).Anim));
	}

	for (INT i = 0; DamageTemplateData && i < DamageTemplateData->DamageEffectList.Num(); ++i)
	{
		FDamageEffectPair& node = DamageTemplateData->DamageEffectList(i);
		UINT Key = node.ResultType<<24;
		Key += node.WeaponHitType<<16;
		key += node.DirectionType<<8;
		Key += node.ElementaryType;
		MMOFacade::getSingleton().AddDamgeEffectPair(node.ResultType, node.WeaponHitType, node.DirectionType, node.ElementaryType, &(DamageTemplateData->DamageEffectList(i)));
	}

	MMOFacade::getSingleton().ClearUISound();
	for (INT i = 0; i < UISoundList.Num(); ++i)
	{
		MMOFacade::getSingleton().AddUISound(UISoundList(i));
	}

	for (INT i = 0; i < WeaponLevelUpEffects.Num(); ++i)
	{
		for (INT j = 0; j < WeaponLevelUpEffects(i).WeaponEffects.Num(); ++j)
		{
			for (INT k = 0; k < WeaponLevelUpEffects(i).WeaponEffects(j).Effects.Num(); ++k)
			{
				if (WeaponLevelUpEffects(i).WeaponEffects(j).Effects(k))
				{
					WeaponLevelUpEffects(i).WeaponEffects(j).Effects(k)->bAddToRoot = TRUE;
				}
			}
		}
	}

	// oscar: see MMOFacade -> _LoadItemTemplates()
}

void UGamePlayDataManager::UnInitGamePlayData()
{
	MMOFacade::getSingleton().ClearSoundParticle();
}



class USoundCue* UGamePlayDataManager::FindHitSound(BYTE InHitType,BYTE TargetArmorMatType)
{
	INT key = 2;
	key = (key<<8) + INT(InHitType);
	key = (key<<8) + INT(TargetArmorMatType);

	USoundCue* ret = MMOFacade::getSingleton().GetHitSound(key);
	//if (ret == NULL)
	//{
	//	// try to get default shape sound
	//	key = 2;
	//	key = (key<<16) + INT(TargetArmorMatType);
	//	ret = MMOFacade::getSingleton().GetHitSoundGroup(key);
	//	if(ret == NULL)
	//	{
	//		// try to get defaut shape walk on default land sound
	//		key = 2;
	//		key = (key<<16);
	//		ret = MMOFacade::getSingleton().GetHitSoundGroup(key);
	//	}
	//}

	return ret;
}


UBOOL UGamePlayDataManager::FindFootstepEffect(BYTE ShapeType,BYTE LandMatType,class USoundCue*& Sound,class UParticleSystem*& Particle)
{
// 	if(GIsEditor)
// 	{
// 		return FALSE;
// 	}

	INT key = 1;
	key = (key<<8) + INT(ShapeType);
	key = (key<<8) + INT(LandMatType);

	FSoundParticleEffect* ret = MMOFacade::getSingleton().GetSoundParticle(key);
	if (ret == NULL)
	{
		// try to get default shape sound
		key = 1;
		key = (key<<16) + INT(LandMatType);
		ret = MMOFacade::getSingleton().GetSoundParticle(key);
		if(ret == NULL)
		{
			// try to get defaut shape walk on default land sound
			key = 1;
			key = (key<<16);
			ret = MMOFacade::getSingleton().GetSoundParticle(key);
		}
	}

	if (ret != NULL)
	{
		// TODO: check the sound play with random
		Sound = ret->Sound;
		Particle = ret->Particle;

#if !SHIPPING_PC_GAME
		if(!Sound)
		{
			UEnum* ShapeEnum = FindObject<UEnum>( UTypeDefine::StaticClass(), TEXT("EBodyType"), TRUE );
			UEnum* LandEnumObj = FindObject<UEnum>(UPhysicalMaterialProperty::StaticClass(), TEXT("EPhyMatType"), TRUE);
			SET_WARN_COLOR(COLOR_CYAN);
			warnf(NAME_Warning, TEXT("Footstep sound config is empty, body type: %s material: %s"), *ShapeEnum->GetEnum(ShapeType).ToString(),
				*LandEnumObj->GetEnum(LandMatType).ToString());
			CLEAR_WARN_COLOR();
		}
#endif

		return TRUE;
	}
	else
	{
#if !SHIPPING_PC_GAME
		UEnum* ShapeEnum = FindObject<UEnum>( UTypeDefine::StaticClass(), TEXT("EBodyType"), TRUE );
		UEnum* LandEnumObj = FindObject<UEnum>(UPhysicalMaterialProperty::StaticClass(), TEXT("EPhyMatType"), TRUE);
		SET_WARN_COLOR(COLOR_CYAN);
		warnf(NAME_Warning, TEXT("Footstep sound not found, body type: %s material: %s"), *ShapeEnum->GetEnum(ShapeType).ToString(),
			*LandEnumObj->GetEnum(LandMatType).ToString());
		CLEAR_WARN_COLOR();
#endif
		Sound = NULL;
		Particle = NULL;
		return FALSE;
	}
}
//hemu@snda 120416
//@todo later consider matching with Enum and string to solve the 2D array
//int i=GetRow(FName inComingName);
FName UGamePlayDataManager::FindAnimMix(FName SourceAnim,FName TargetAnim )
{
	FName name;
	//UMMOAnimMixEditObject *node;
/*
	for (INT i = 0; i < AnimMixList.Num(); ++i)
	{
		node=
		if( if(AnimMixList.))
	}*/
	return name;
}


//------------------------------------------------------------------------
class UGamePlayDataManager* UGamePlayDataManager::Singleton()
{
	return GetSingleton();
}
//------------------------------------------------------------------------
void UGamePlayDataManager::SavePackage()
{
#if WITH_EDITOR
	UPackage* PackageToSave = GetGameplayDataMgrPackage();
	FString PackageName = PackageToSave->GetName();
	FString ExistingFile;
	FString SaveFileName;

	if( GPackageFileCache->FindPackageFile( *PackageName, NULL, ExistingFile ) )
	{
		// If the package already exists, save to the existing filename.
		SaveFileName = ExistingFile;
	}


	TArray<UPackage*> Packages;
	Packages.AddItem(PackageToSave);
	FEditorFileUtils::PromptForCheckoutAndSave(Packages, TRUE, TRUE);
#endif
}
//------------------------------------------------------------------------
UGamePlayDataManager* UGamePlayDataManager::GetSingleton()
{
	if(GGameplayDataMgr)
	{
		return GGameplayDataMgr;
	}
	else
	{
#if 1
		FString cnfgName;
		if(GConfig->GetString(TEXT("SingletonData"), TEXT("GamePlayDataManager"), cnfgName, GGameIni))
		{
			GGameplayDataMgr = Cast<UGamePlayDataManager>(UObject::StaticLoadObject(
				UGamePlayDataManager::StaticClass(), NULL, *cnfgName, NULL, LOAD_NoWarn | LOAD_Quiet, NULL));
			
			if (GGameplayDataMgr)
			{
				GGameplayDataMgr->AddToRoot();
			}
			else
			{
				appErrorf(TEXT("Data package error detected. Please run the full file repair (FixClient) and then start the game.\nGamePlayDataManager load failed!"));
			}
		}
		else
		{
			appErrorf(TEXT("Data package error detected. Please run the full file repair (FixClient) and then start the game.\nGamePlayDataManager not load! Check in MMOGame/Config/DefaultGame.ini"));
		}
#else
		UPackage* Package = GetGameplayDataMgrPackage();
		_crtBreakAlloc
		for (INT j = 0; j < Package->GetNetObjectCount(0); ++j)
		{
			GGameplayDataMgr = Cast<UGamePlayDataManager>(Package->GetNetObjectAtIndex(j));
			if(GGameplayDataMgr)
			{
				break;
			}
		}
#endif
		return GGameplayDataMgr;
	}
}
//------------------------------------------------------------------------
UPackage* UGamePlayDataManager::GetGameplayDataMgrPackage()
{
	FString PackageName = TEXT("GamePlayData.upk");
	FString ExistingFile, SaveFileName;

	if( GPackageFileCache->FindPackageFile( *PackageName, NULL, ExistingFile ) )
	{
		// If the package already exists, save to the existing filename.
		SaveFileName = ExistingFile;
	}

	UPackage* Package = LoadPackage( NULL, *PackageName, LOAD_NoWarn );
	if( !Package )
	{
		appErrorf( TEXT("Unable to load %s"), *PackageName );
	}

	if(!Package->IsFullyLoaded())
	{
		Package->FullyLoad();
	}

	return Package;
}
//------------------------------------------------------------------------

struct FDamageEffectPair UGamePlayDataManager::GetDamageEffect(BYTE ResultType,BYTE WeaponHitType,BYTE DirectionType,BYTE ElementaryType)
{
	FDamageEffectPair* Ret = NULL;
	Ret = MMOFacade::getSingleton().GetDamgeTemplateData(ResultType, WeaponHitType, DirectionType, ElementaryType);
	if (!Ret)
	{
		return FDamageEffectPair();
	}
	else
	{
		return *Ret;
	}
}

UBOOL UGamePlayDataManager::GetStateDeath(BYTE InType,TArray<class UAbilityComponent*>& OutData)
{
	if (!DamageTemplateData)
	{
		return FALSE;
	}

	for (INT i = 0; i < DamageTemplateData->StateDeaths.Num(); ++i)
	{
		if (DamageTemplateData->StateDeaths(i).BuffType == InType)
		{
			OutData = DamageTemplateData->StateDeaths(i).EffectList;
			return TRUE;
		}
	}

	return FALSE;
}

UBOOL UGamePlayDataManager::GetCommonDeath(BYTE InStyle,BYTE InElementaryType,TArray<class UAbilityComponent*>& OutData)
{
	if (!DamageTemplateData)
	{
		return FALSE;
	}

	for (INT i = 0; i < DamageTemplateData->CommonDeaths.Num(); ++i)
	{
		if (DamageTemplateData->CommonDeaths(i).DeathType == InStyle && DamageTemplateData->CommonDeaths(i).ElementaryType == InElementaryType)
		{
			OutData = DamageTemplateData->CommonDeaths(i).EffectList;
			return TRUE;
		}
	}

	return FALSE;
}

INT UGamePlayDataManager::GetDamageEffectListIndex(BYTE ResultType,BYTE WeaponHitType,BYTE DirectionType,BYTE ElementaryType)
{
	if (!DamageTemplateData)
	{
		return -1;
	}

	INT Ret = -1;

	for (INT i = 0; i < DamageTemplateData->DamageEffectList.Num(); ++i)
	{
		if (DamageTemplateData->DamageEffectList(i).ResultType == ResultType
			&& DamageTemplateData->DamageEffectList(i).WeaponHitType == WeaponHitType
			&& DamageTemplateData->DamageEffectList(i).DirectionType == DirectionType
			&& DamageTemplateData->DamageEffectList(i).ElementaryType == ElementaryType)
		{
			Ret = i;
			break;
		}
	}

	return Ret;
}

//------------------------------------------------------------------------
FItemActionSound UGamePlayDataManager::GetItemActionSounds(BYTE Type)
{
	for (INT i = 0; i < ItemActionSounds.Num(); ++i)
	{
		if(ItemActionSounds(i).ItemType == Type)
		{
			return ItemActionSounds(i);
		}
	}

	return FItemActionSound();
}

void UGamePlayDataManager::PlayItemActionSounds(AActor* Actor,BYTE Type,BYTE eas)
{
	if(!Actor)
	{
		return;
	}

	TArrayNoInit<class USoundCue*> *soundPtr=NULL;
	for (INT i = 0; i < ItemActionSounds.Num(); ++i)
	{
		if(ItemActionSounds(i).ItemType == Type)
		{
			switch( eas )
			{
				// 				case EActionSound_Use:
				// 					soundPtr = &ItemActionSounds(i).Use;
				// 					break;
				// 				case EActionSound_Get:
				// 					soundPtr = &ItemActionSounds(i).Get;
				// 					break;
			case EActionSound_Drop:
				soundPtr = &ItemActionSounds(i).Drop;
				break;
				// 				case EActionSound_Land:
				// 					soundPtr = &ItemActionSounds(i).Land;
				// 					break;
			case EActionSound_MousePickup:
				soundPtr = &ItemActionSounds(i).MousePickup;
				break;

			case EActionSound_MousePutdown:
				soundPtr = &ItemActionSounds(i).MousePutdown;
				break;

			case EActionSound_Use:
				soundPtr = &ItemActionSounds(i).Use;
				break;

			case EActionSound_Get:
				soundPtr = &ItemActionSounds(i).Get;
				break;
			}

			if(soundPtr != NULL && soundPtr->Num() > 0)
			{
				INT idx = (INT)RandRange(0, soundPtr->Num() - 1);

				Actor->PlaySound((*soundPtr)(idx));
				//Actor->PlaySound( soundPtr( RandHelper( soundPtr->Num() ) )  );
			}
		}
	}
	return;
}

void UGamePlayDataManager::PlayDoodadBeginSound(class AActor* actor, BYTE type)
{
    if (/*type < 0 || */type >= EIMAT_MAX)
        return;

    if (DoodadSndList[type].AnimBegin)
        actor->PlaySound(DoodadSndList[type].AnimBegin);
}

void UGamePlayDataManager::PlayDoodadHitSound(class AActor* actor, BYTE type)
{
    if (/*type < 0 || */type >= EIMAT_MAX)
        return;

    if (DoodadSndList[type].HitSound)
        actor->PlaySound(DoodadSndList[type].HitSound);
}

void UGamePlayDataManager::PlayDoodadDestructSound(class AActor* actor, BYTE type)
{
	if (/*type < 0 || */type >= EIMAT_MAX)
		return;

	if (DoodadSndList[type].DestructSound)
		actor->PlaySound(DoodadSndList[type].DestructSound);
}


void UGamePlayDataManager::PlayDoodadLoopSound(class AActor* actor, BYTE type)
{
    if (/*type < 0 || */type >= EIMAT_MAX)
        return;

    if (DoodadSndList[type].AnimLoop)
        actor->PlaySound(DoodadSndList[type].AnimLoop);
}

void UGamePlayDataManager::PlayDoodadEndSound(class AActor* actor, BYTE type)
{
    if (/*type < 0 || */type >= EIMAT_MAX)
        return;

    if (DoodadSndList[type].AnimEnd)
		actor->PlaySound(DoodadSndList[type].AnimEnd);

//     switch (type)
//     {
//     case EDST_WOOD:
//         break;
//     case EDST_VEGETATION:
//         break;
//     case EDST_METAL:
//         break;
//     }
}

FLOAT UGamePlayDataManager::GetWeaponSpeedFactor(BYTE InWeaponType)
{
	for (INT i = 0; i < WeaponTypeSpeeds.Num(); ++i)
	{
		if (InWeaponType == WeaponTypeSpeeds(i).WeaponType)
		{
			return WeaponTypeSpeeds(i).SpeedFactor;
		}
	}
	return 1.f;
}

class USoundCue* UGamePlayDataManager::GetWeaponUseSound(BYTE InWeaponType,INT Index)
{
    if (Index < 0)
        return NULL;
	for (INT i = 0; i < CommonWeaponSoundList.Num(); ++i)
	{
		if (CommonWeaponSoundList(i).WeaponAppearenceType == InWeaponType)
		{
			if (Index < CommonWeaponSoundList(i).SoundToPlay.Num())
			{
				return CommonWeaponSoundList(i).SoundToPlay(Index);
			}
			break;
		}
	}
	return NULL;
}

class UMaterialInstanceTimeVaryingEffectNode* UGamePlayDataManager::GetMaterialEffectData(INT InEffectId)
{
	if (MaterialTimeVaryingEffectData == NULL)
	{
		return NULL;
	}

	for (INT i = 0; i < MaterialTimeVaryingEffectData->TimeVaryingEffectList.Num(); ++i)
	{
		if (MaterialTimeVaryingEffectData->TimeVaryingEffectList(i) && InEffectId == MaterialTimeVaryingEffectData->TimeVaryingEffectList(i)->EffectTemplateId)
		{
			return MaterialTimeVaryingEffectData->TimeVaryingEffectList(i);
		}
	}

	return NULL;
}


UBOOL UGamePlayDataManager::GetNpcLevelColorData(INT InType,struct FNpcLevelColorData& OutData)
{
	for (INT i = 0; i < NpcLevelColorList.Num(); ++i)
	{
		if (NpcLevelColorList(i).Quarity == InType)
		{
			OutData.Quarity = InType;
			OutData.ParameterName = NpcLevelColorList(i).ParameterName;
			OutData.ColorValue = NpcLevelColorList(i).ColorValue;
			return TRUE;
		}
	}

	return FALSE;
}


class USoundCue* UGamePlayDataManager::GetClothNoise(BYTE GenderType,BYTE ClothType,BYTE ActionType)
{
	for (INT i = 0; i < ClothNoiseList.Num(); ++i)
	{
		if (ClothNoiseList(i).GenderType == GenderType && ClothNoiseList(i).ClothType == ClothType && ClothNoiseList(i).ActionType == ActionType)
		{
			return ClothNoiseList(i).SoundToPlay;
		}
	}

	return NULL;
}