#include "..\stdafx.h"
#include "MMOFacade.h"
#include "FSMIO.h"
#include "MMOQuestManager.h"

IMPLEMENT_CLASS(USeqAct_PlayerMatineeBegin)
IMPLEMENT_CLASS(USeqAct_PlayerMatineeEnd)
IMPLEMENT_CLASS(USeqAct_SwitchPlayerClassGender)
IMPLEMENT_CLASS(USeqAct_PlayBik)
IMPLEMENT_CLASS(USeqAct_LevelLoaded)


struct FSeqAct_PlayerMatinee_Backup
{
    USkeletalMesh* SceneObjectMainSkeletal;
    USkeletalMeshComponent* MergedComp;
    TArray< UPrimitiveComponent* > AttachObject;
    TArray< UPrimitiveComponent* > AttachSocket;
};


typedef TMap<USeqVar_Object*, FSeqAct_PlayerMatinee_Backup> VarObjectMap;

VarObjectMap g_varObjInfo;

void SetVarObjectInfo(USeqVar_Object* k, const FSeqAct_PlayerMatinee_Backup& val)
{
    if (k)
	    g_varObjInfo.Set(k, val);
}

FSeqAct_PlayerMatinee_Backup* GetOrigVarObject(USeqVar_Object* k)
{
	FSeqAct_PlayerMatinee_Backup* objPtr = g_varObjInfo.Find(k);
	if(objPtr)
		return objPtr;
	return NULL;
}

void DelOrigVarObject(USeqVar_Object* k)
{
    if (k)
        g_varObjInfo.Remove(k);
}

void USeqAct_PlayerMatineeBegin::Activated()
{
	GSystemSettings.SetForceUseLodZeroLevel( TRUE );
    AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
    AMMOPlayer* player = Cast<AMMOPlayer>(ctrl->Pawn);

	if (player->GetMasterComponent() == NULL)
	{
		return;
	}

    //MMOTargetManager::getSingleton().Clear();
	FSMIO::getSingleton().State.ClearAllState();

// 	if (ctrl && ctrl->MyDecalManager)
// 	{
// 		ctrl->StopMove(TRUE);
// 		ctrl->MyDecalManager->eventSelectActor(NULL);
// 	}

    player->IsInMatinee = TRUE;
	//MMOFacade::getSingleton().PauseBackSound(TRUE);

	if(this->InputLinks(0).bHasImpulse)
	{
        for(INT i = 0; i < VariableLinks(0).LinkedVariables.Num(); ++i)
        {
            USeqVar_Object* objVar = Cast<USeqVar_Object>(VariableLinks(0).LinkedVariables(i));
            if(objVar)
            {
                if (objVar->ObjValue->IsA(ASkeletalMeshActorMAT::StaticClass()))
                {
                    ASkeletalMeshActorMAT* matineeActor = (ASkeletalMeshActorMAT*)objVar->ObjValue;

                    FSeqAct_PlayerMatinee_Backup BackMatineeObject;
                    BackMatineeObject.SceneObjectMainSkeletal = matineeActor->SkeletalMeshComponent->SkeletalMesh;

                    matineeActor->SkeletalMeshComponent->SetSkeletalMesh(player->GetMasterComponent()->SkeletalMesh);

					if( !player->UseingAvatar->UseMergedMesh )
					{
						// only old type avatar needs to deal with the merged mesh
						USkeletalMeshComponent* MergedComp = ConstructObject<USkeletalMeshComponent>(USkeletalMeshComponent::StaticClass());
						MergedComp->SetSkeletalMesh(player->ItemManager->MergedMesh);
						matineeActor->AttachComponent(MergedComp);
						MergedComp->SetParentAnimComponent(matineeActor->SkeletalMeshComponent);

						MergedComp->SetLightingChannels( player->GetMasterComponent()->LightingChannels );
						MergedComp->SetLightEnvironment( player->GetMasterComponent()->LightEnvironment );

						// skin color
						for( INT j = 0; j < player->ItemManager->MergedComp->Materials.Num(); ++j )
						{
							UMaterialInterface* psrcMaterial = player->ItemManager->MergedComp->Materials(j);
							if( psrcMaterial == NULL )
								break;

							UMaterialInstance* pMaterial = NULL;

							if ( psrcMaterial->IsA( UMaterialInstanceConstant::StaticClass() ) )
							{
								pMaterial = ConstructObject<UMaterialInstanceConstant>(UMaterialInstanceConstant::StaticClass()
									, INVALID_OBJECT, NAME_None /**(psrcMaterial->GetName())*/, 0, psrcMaterial );
							}
							else if ( psrcMaterial->IsA( UMaterialInstanceTimeVarying::StaticClass() ) )
							{
								pMaterial = ConstructObject<UMaterialInstanceTimeVarying>(UMaterialInstanceTimeVarying::StaticClass()
									, INVALID_OBJECT, NAME_None /**(psrcMaterial->GetName())*/, 0, psrcMaterial );
							}

							if ( pMaterial )
							{
								UAvatarData* pAvatar = player->GetAvatarData();
								// todo: change skin color
								//if( pAvatar)
								//{
								//	pMaterial->SetVectorParameterValue( TEXT("SkinColor"), pAvatar->SkinColorSet( player->ItemManager->CurrentSkinColorIdx ) );
								//}
								pMaterial->SetVectorParameterValue( TEXT("SilhouetteColor"), FLinearColor( 0, 0, 0 ) );
								pMaterial->SetVectorParameterValue( TEXT("SkinSpecular"), FLinearColor( 0, 0, 0 ) );
							}
						
							MergedComp->SetMaterial( j, pMaterial/*player->ItemManager->MergedComp->Materials(j)*/ );

							BackMatineeObject.MergedComp = MergedComp;
						}
					}

					for (INT i = 0; i < player->EquipList.Num(); ++i)
					{
						if (player->EquipList(i).ComponentOrActor == NULL)
						{
							continue;
						}

						UObject* o = player->EquipList(i).ComponentOrActor;
						if (o->IsA(AArmorActor::StaticClass()))
						{
							AArmorActor* armor = (AArmorActor*)o;
							UPrimitiveComponent* ToUseComponent = NULL;
							if (armor->MainMesh == NULL)
							{
								continue;
							}

							if (armor->Template && (armor->Template->MeshType == EBMT_Weapon || armor->Template->MeshType == EBMT_Weapon2))
							{
								continue;
							}

							if (armor->MainMesh->IsA(UStaticMeshComponent::StaticClass()))
							{
								UStaticMeshComponent* usmc = (UStaticMeshComponent*)(armor->MainMesh);
								UStaticMeshComponent* TempComponent = ConstructObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass(), matineeActor);
								TempComponent->SetStaticMesh(usmc->StaticMesh);
								ToUseComponent = TempComponent;
							}
							else if (armor->MainMesh->IsA(USkeletalMeshComponent::StaticClass()))
							{
								USkeletalMeshComponent* usmc = (USkeletalMeshComponent*)(armor->MainMesh);
								USkeletalMeshComponent* TempComponent = ConstructObject<USkeletalMeshComponent>(USkeletalMeshComponent::StaticClass(), matineeActor);
								TempComponent->SetSkeletalMesh(usmc->SkeletalMesh);
								ToUseComponent = TempComponent;
							}
							else if (armor->MainMesh->IsA(UParticleSystemComponent::StaticClass()))
							{
								UParticleSystemComponent* upsc = (UParticleSystemComponent*)(armor->MainMesh);
								UParticleSystemComponent* TempComponent = ConstructObject<UParticleSystemComponent>(UParticleSystemComponent::StaticClass(), matineeActor);
								TempComponent->SetTemplate(upsc->Template);
								ToUseComponent = TempComponent;
							}

							check(ToUseComponent);
							USkeletalMeshComponent* BaseMesh = matineeActor->SkeletalMeshComponent;
							if (BaseMesh)
							{
								ToUseComponent->SetLightEnvironment(player->GetMasterComponent()->LightEnvironment);
								ToUseComponent->SetLightingChannels(player->GetMasterComponent()->LightingChannels);

								BaseMesh->AttachComponentToSocket(ToUseComponent, armor->CurrentAttachedSocketName);
								BackMatineeObject.AttachSocket.AddItem(ToUseComponent);
							}
						}
						else if (o->IsA(USkeletalMeshComponent::StaticClass()))
						{
							USkeletalMeshComponent* usmc = (USkeletalMeshComponent*)(o);

							// Using usmc as the template here makes the parent actor wrong, and it crashes on detach -ming
							USkeletalMeshComponent* NewComponent = ConstructObject<USkeletalMeshComponent>(USkeletalMeshComponent::StaticClass(), matineeActor, NAME_None, 0);
							NewComponent->SetSkeletalMesh( usmc->SkeletalMesh );
							matineeActor->AttachComponent(NewComponent);
							BackMatineeObject.AttachObject.AddItem(NewComponent);
							NewComponent->SetParentAnimComponent(matineeActor->SkeletalMeshComponent);

							NewComponent->SetLightingChannels( player->GetMasterComponent()->LightingChannels );
							NewComponent->SetLightEnvironment( player->GetMasterComponent()->LightEnvironment );

							if( player->EquipList(i).BodyMeshType == EBMT_Hair )
							{
								for( INT matIdx = 0; matIdx < usmc->Materials.Num(); ++matIdx )
								{
									UMaterialInterface* pMat = usmc->Materials( matIdx );
									if( pMat == NULL )
										continue;

									UMaterialInstance* pNewMat = NULL;

									if ( pMat->IsA( UMaterialInstanceConstant::StaticClass() ) )
									{
										pNewMat = ConstructObject<UMaterialInstanceConstant>(UMaterialInstanceConstant::StaticClass()
											, INVALID_OBJECT, NAME_None /**(psrcMaterial->GetName())*/, 0, pMat );
									}
									else if ( pMat->IsA( UMaterialInstanceTimeVarying::StaticClass() ) )
									{
										pNewMat = ConstructObject<UMaterialInstanceTimeVarying>(UMaterialInstanceTimeVarying::StaticClass()
											, INVALID_OBJECT, NAME_None /**(psrcMaterial->GetName())*/, 0, pMat );
									}
									if ( pNewMat )
									{
										UAvatarData* pAvatar = player->GetAvatarData();
										if( pAvatar)
										{
											pNewMat->SetVectorParameterValue( TEXT("HairColor"), pAvatar->HairColorSet( player->ItemManager->CurrentHairColorIdx ) );
										}
										// disable silhouette specular
										pNewMat->SetVectorParameterValue( TEXT("SilhouetteColor"), FLinearColor( 0, 0, 0 ) );
									}
											
									NewComponent->SetMaterial( matIdx, pNewMat );
								}
							}
							else if( player->EquipList(i).BodyMeshType == EBMT_Face )
							{
								for( INT matIdx = 0; matIdx < usmc->Materials.Num(); ++matIdx )
								{
									UMaterialInterface* pMat = usmc->Materials( matIdx );
									if( pMat == NULL )
										continue;

									UMaterialInstance* pNewMat = NULL;

									if ( pMat->IsA( UMaterialInstanceConstant::StaticClass() ) )
									{
										pNewMat = ConstructObject<UMaterialInstanceConstant>(UMaterialInstanceConstant::StaticClass()
											, INVALID_OBJECT, NAME_None /**(psrcMaterial->GetName())*/, 0, pMat );
									}
									else if ( pMat->IsA( UMaterialInstanceTimeVarying::StaticClass() ) )
									{
										pNewMat = ConstructObject<UMaterialInstanceTimeVarying>(UMaterialInstanceTimeVarying::StaticClass()
											, INVALID_OBJECT, NAME_None /**(psrcMaterial->GetName())*/, 0, pMat );
									}
									if ( pNewMat )
									{
										UAvatarData* pAvatar = player->GetAvatarData();
										if( pAvatar)
										{
											FLinearColor temp;
											if( pNewMat->GetVectorParameterValue( TEXT("BeardColor"), temp ) )
												pNewMat->SetVectorParameterValue( TEXT("BeardColor"), pAvatar->HairColorSet( player->ItemManager->CurrentHairColorIdx ) );
										}
									}
											
									NewComponent->SetMaterial( matIdx, pNewMat );
								}
							}
						}
					}

                    SetVarObjectInfo(objVar, BackMatineeObject);
                }
            }
        }
		
		ctrl->GotoState(TEXT("PlayerMatinee"));
	}

	Super::Activated();
}

void USeqAct_PlayerMatineeEnd::Activated()
{
	GSystemSettings.SetForceUseLodZeroLevel( FALSE );
    AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
    AMMOPlayer* player = Cast<AMMOPlayer>(ctrl->Pawn);

	if(this->InputLinks(0).bHasImpulse)
	{
		for(INT i = 0; i < VariableLinks(0).LinkedVariables.Num(); ++i)
		{
			USeqVar_Object* objVar = Cast<USeqVar_Object>(VariableLinks(0).LinkedVariables(i));
			if(objVar)
			{
                FSeqAct_PlayerMatinee_Backup* backup = GetOrigVarObject(objVar);
                if (backup && objVar->ObjValue->IsA(ASkeletalMeshActorMAT::StaticClass()))
                {
                    ASkeletalMeshActorMAT* SceneObject = (ASkeletalMeshActorMAT*)objVar->ObjValue;

                    for (INT i = 0; i < backup->AttachObject.Num(); ++i)
                        SceneObject->DetachComponent(backup->AttachObject(i));
                    for (INT i = 0; i < backup->AttachSocket.Num(); ++i)
                        SceneObject->SkeletalMeshComponent->DetachComponent(backup->AttachSocket(i));

                    SceneObject->SkeletalMeshComponent->SetSkeletalMesh(backup->SceneObjectMainSkeletal);
					if( backup->MergedComp )
	                    SceneObject->DetachComponent(backup->MergedComp);
                }
			}
            DelOrigVarObject(objVar);
		}
	}

	
    player->IsInMatinee = FALSE;
	MMOFacade::getSingleton().PauseBackSound(FALSE);
    ctrl->GotoState(TEXT(""));

	Super::Activated();
}

INT GetOutputIndexByClassGender(BYTE playerClass, BYTE playerGender)
{
	switch(playerClass)
	{
	case E_cls_Warrior:
		{
			if(playerGender == EGT_Male)
			{
				return 0;
			}

			return 1;
		}
		break;

	case E_cls_Mage:
		{
			if(playerGender == EGT_Male)
			{
				return 2;
			}

			return 3;
		}
		break;

	case E_cls_Rogue:
		{
			if(playerGender == EGT_Male)
			{
				return 4;
			}

			return 5;
		}
		break;

	case E_cls_Archer:
		{
			if(playerGender == EGT_Male)
			{
				return 6;
			}

			return 7;
		}
		break;
	case E_cls_Taoist:
		{
			if (playerGender == EGT_Male)
			{
				return 9;
			}
			else
			{
				return 10;
			}
		}
		break;
	}

	warnf(NAME_Error, TEXT("[USeqAct_SwitchPlayerClassGender::Activated] with error class:%d gender:%d"), playerClass, playerGender);
	return 8;
}

void USeqAct_SwitchPlayerClassGender::Activated()
{
	for (INT i = 0; i < OutputLinks.Num(); ++i)
	{
		OutputLinks(i).bDisabled = TRUE;
	}

	if(GIsEditor)
	{
		INT outputIdx = GetOutputIndexByClassGender(PIETestClassType, PIETestGender);
		OutputLinks(outputIdx).bHasImpulse = TRUE;
		//ForceActivateOutput(outputIdx);
        AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
        if (player)
            player->eventSetMesh(PIETestClassType, PIETestGender);
	}
	else
	{
		AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
		if(player)
		{
			EGenderType myGender = (EGenderType)player->ActorAttributes->GenderType;
			EPlayerClass myClass = (EPlayerClass)player->ActorAttributes->ClassType;

			INT outputIdx = GetOutputIndexByClassGender(myClass, myGender);

			OutputLinks(outputIdx).bHasImpulse = TRUE;
		}
	}

	//Super::Activated();
}

IMPLEMENT_CLASS(USeqAct_SetPlayerInMatinee);

void USeqAct_SetPlayerInMatinee::Activated()
{
	//MMOTargetManager::getSingleton().Clear();
	FSMIO::getSingleton().State.ClearAllState();
	//ctrl->StopMove(TRUE);

	UBOOL isInMatinee = FALSE;
	if(this->InputLinks(0).bHasImpulse)
	{
		isInMatinee = TRUE;
	}
	else if(this->InputLinks(1).bHasImpulse)
	{
		isInMatinee = FALSE;
	}

	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if(ctrl)
	{
		AMMOPlayer* player = Cast<AMMOPlayer>(ctrl->Pawn);

		if(player)
		{
			if(player->IsInMatinee!=isInMatinee)
			{
				player->IsInMatinee = isInMatinee;
				//MMOFacade::getSingleton().PauseBackSound(player->IsInMatinee);
			}
		}
	}


	Super::Activated();
}

FString GetBinkMoviePath(const FString& fileName)
{
	FString MoviePath = appBaseDir();
	MoviePath += appGameDir();
	FString FullPath = MoviePath + TEXT( "Movies\\" ) + fileName + TEXT( ".bik" );
	return FullPath;
}

#define ENCRY_BUF_LEN_OPEN_BINK 1212
const unsigned char BinkBitMask = 0xFF;
#define READ_BUF_LEN (4096)

void USeqAct_PlayBik::Activated()
{
	Super::Activated();

	RealBinkMovieName = BikName.ToString();

#if BETA_7_7 && SHIPPING_PC_GAME // enable again before the big push
	if(RealBinkMovieName == TEXT("FirstLogin.bik"))
	{
		if(GFileManager->FileSize(*GetBinkMoviePath("open_ok")) <= 0)
		{
			FILE* oldFile = NULL;
			FString srcFileName = GetBinkMoviePath("open");
			errno_t errNo = _tfopen_s(&oldFile, *srcFileName, TEXT("rb"));
			if(oldFile == NULL)
			{
				warnf(NAME_Error, TEXT("[BINK] Read bink file failed: [%d] %s"), errNo, *srcFileName);
				return;
			}

			FILE* newFile = NULL;
			FString destFileName = GetBinkMoviePath("open_ok");
			errNo = _tfopen_s(&newFile, *destFileName, TEXT("wb"));
			if(newFile == NULL)
			{
				warnf(NAME_Error, TEXT("[BINK] Create bink file failed: [%d] %s"), errNo, *destFileName);
				fclose(oldFile);
				return;
			}

			unsigned char readBuf[READ_BUF_LEN] = {0};
			size_t rd = 0;
			do 
			{
				rd = fread(readBuf, 1, READ_BUF_LEN, oldFile);

#if SHIPPING_PC_GAME
				FArchiveEncode::Decode(readBuf, (INT)rd);
#else
				for ( size_t i = 0; i < rd; ++i )
				{
					*( readBuf + i ) = FArchiveEncode::GetInstance()->offsetDeCode[*( readBuf + i )];
				}
#endif

				size_t sz = fwrite(readBuf, 1, rd, newFile);

				if(rd != sz)
				{
					warnf(NAME_Error, TEXT("[BINK]### Error Read :%d Write:%d \n"), rd, sz);
					appMsgf(AMT_OK, TEXT("Not enough disk space to create the cache file for the latest intro animation. Please leave 500 MB free. The old intro animation will play instead."));
					fclose(oldFile);
					fclose(newFile);
					GFileManager->Delete(*GetBinkMoviePath("open_ok"));
					return;
				}

			} while (rd > 0);


			fclose(oldFile);
			fclose(newFile);
		}

		RealBinkMovieName = TEXT("open_ok.bik");
	}
#endif
}

UBOOL USeqAct_PlayBik::UpdateOp(FLOAT deltaTime)
{
	UBOOL result = Super::UpdateOp( deltaTime );
	MMO::ActiveKismetPlayBik( RealBinkMovieName, bPlayBGM );
	//Stop();
	return TRUE;
}

void USeqAct_LevelLoaded::Activated()
{
	Super::Activated();
}