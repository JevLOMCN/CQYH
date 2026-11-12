#include "LoginBase.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMsgBoxManager.h"
#include "MMOLoginManager.h"

LoginBase::LoginBase()
{
	//IsChinaJoy = TRUE;
	IsVersion2T = FALSE;
	SelectRoleIndex=-1;

	CreateRoleClass=0;
	CreateRoleSex=1;

	IsFromLogin=FALSE;

	MainPlayer = NULL;
	PC = NULL;


	SelectionCapture = NULL;
	CreationCapture = NULL;
	CreateShowCharacter = NULL;

	IsCreateOption = FALSE;

	CreateRoleHairColor = 0;
	CreateRoleHairModel = 0;
	CreateRoleSkinColor = 0;
	CreateRoleFaceModel = 0;
}

LoginBase::~LoginBase()
{

}

TArray<FString> LoginBase::GetLoginIPList()
{
	TArray<FString> ipNameList;
	TCHAR ipFile[1024];
	appSprintf(ipFile, TEXT("%sIP.ini"), *appGameConfigDir());
	GConfig->GetArray(TEXT("LoginIP"), TEXT("LoginServerIP"), ipNameList, ipFile);
	return ipNameList;
}

TArray<FString> LoginBase::GetLoginIPNameList()
{
	TArray<FString> ipNameList;
	TCHAR ipFile[1024];
	appSprintf(ipFile, TEXT("%sIP.ini"), *appGameConfigDir());
	GConfig->GetArray(TEXT("LoginIP"), TEXT("LoginServerName"), ipNameList, ipFile);
	return ipNameList;
}

void LoginBase::GetIPSelected( const FString& serverName,FString& ip )
{
	TArray<FString> ipList, ipNameList;

	ipList = GetLoginIPList();
	ipNameList = GetLoginIPNameList();
	for(INT i = 0; i < ipNameList.Num(); ++i)
	{
		if(ipNameList(i) == serverName)
		{
			ip = ipList(i);
			break;
		}
	}
}

INT LoginBase::GetLoginPort()
{
	TCHAR ipFile[1024];
	appSprintf(ipFile, TEXT("%sIP.ini"), *appGameConfigDir());
	INT port = 0;
	GConfig->GetInt(TEXT("LoginIP"), TEXT("LoginServerPort"), port, ipFile);
	return port;
}






//---------------------------------------------------------------------------
void LoginBase::SelectRole( INT id )
{
	if(PC ==NULL || MainPlayer==NULL)
		return;
	if(PC->RoleList.Num()<=id || id < 0)
		return;

	SelectRoleIndex = id;
	SelectedRoleName = FString(PC->RoleList(id).RoleName);

	// this must be set before PC->SetCharAvatart
	MainPlayer->WeaponUpgradePoint = PC->RoleList(id).WeaponUpgradePoint;
	//art proportions are off, special scaling needed
	//FLOAT scale = 1.3f;
	//MainPlayer->SetHidden( TRUE );
	PC->SetCharAvatart(id);
	//MainPlayer->WorldInfo->PlayerGlobalScale = scale;
	MainPlayer->RefreshCollision();
	//MainPlayer->SetHidden(FALSE);
	//MainPlayer->SetDrawScale( scale );
	//MainPlayer->eventRefreshCollisionForce( scale );
	PlayerAppearanceData data;
	data.FaceModel = PC->RoleList(id).nFaceType;
	data.HairColor = PC->RoleList(id).nHairColor;
	data.SkinColor = PC->RoleList(id).nSkinColor;
	data.HairModel = PC->RoleList(id).nHairType;
	data.Class = PC->RoleList(id).byCareer;
	data.Sex = PC->RoleList(id).bySex;

	MMOLoginManager::getSingleton().UpdateAllAppearance(data);

	SetHiddenInLogin(MainPlayer, FALSE);

	// reset location
	FVector NewLocation = PlayerStartPostion + FVector(0, 0, MainPlayer->GetCollisionHeight());
	MainPlayer->SetLocation(NewLocation);
}

void LoginBase::SetCreateApperance()
{
	if( PC ==NULL || MainPlayer==NULL )
		return;

	SetHiddenInLogin( MainPlayer, FALSE );

	MainPlayer->eventSetMesh( CreateRoleClass, CreateRoleSex );
	MainPlayer->RefreshCollision();

	FVector NewLocation = PlayerStartPostion + FVector(0, 0, MainPlayer->GetCollisionHeight());
	MainPlayer->SetLocation( NewLocation );
}

void LoginBase::UpdateCreateRole()
{
    HideAllCreateShowActors();

	ANPC* showActor = NULL;
	if( CreateRoleSex == EGT_Male )
		showActor = CreateShowActors( CreateRoleClass ) ;
	else
		showActor = CreateShowActors( CreateRoleClass + EPCT_None ) ;
	
	if( !showActor )
	    return;

	SetHiddenInLogin( showActor, FALSE );

	CreateShowCharacter = showActor;
	//Idle start
	showActor->PlayAnimation( CreateShowName );
}


void LoginBase::EnterSelectRole()
{
	HideAllCreateShowActors();
	CreateShowCharacter = NULL;

	AMMOActor* MainActor = MMOFacade::getSingleton().GetMainActor();
	if(MainActor){
		MainActor->SetRotation(PlayerStartRotation);
	}
	IsCreateOption = FALSE;

	CreateRoleName = TEXT("");
	CreateRoleClass = EPCT_PlayerClassA;
	CreateRoleSex = EGT_Male;
}

void LoginBase::EnterCreateRole()
{
	if( PC ==NULL || MainPlayer==NULL )
		return;

	SetHiddenInLogin( MainPlayer, TRUE );
	CreateRoleName = TEXT("");
	//CreateRoleClass = EPCT_PlayerClassA;
	//CreateRoleSex = EGT_Male;
	ResetApperance();
	UpdateCreateRole();
}

void LoginBase::EnterCreateRoleAppearance()
{
	AMMOActor* MainActor = MMOFacade::getSingleton().GetMainActor();
	if(MainActor){
		MainActor->SetRotation(PlayerStartRotation);
	}
	SetCreateApperance();
}
//----------------------------------------------------------------------------
void LoginBase::HideAllCreateShowActors()
{
	for ( INT i= 0; i < CreateShowActors.Num(); ++i )
	{
		if( CreateShowActors(i) )
		{
			SetHiddenInLogin( CreateShowActors(i), TRUE );
		}
	}
}

void LoginBase::SetHiddenInLogin( class AMMOActor* actor,UBOOL hiddden )
{	
	if( !actor )
		return;
	if( hiddden )
	{
		actor->StopAnimation();

		actor->SetHidden( TRUE );
		actor->SetLocation( HiddenPostion );

		// added by zhaojun@snda
		// stop the associate audio
		for (int i = 0; i < actor->AllComponents.Num(); ++i)
		{
			UActorComponent* pComponent = actor->AllComponents(i);

			UAudioComponent* pAudioComponent = Cast<UAudioComponent>(pComponent);

			if (pAudioComponent)
			{
				pAudioComponent->Stop();
			}
		}
		// added by zhaojun@snda

	}
	else
	{
		//due to outfit change, prevent the model from suddenly appearing
		actor->SetHidden( FALSE );
		FVector NewLocation = PlayerStartPostion + FVector(0, 0, actor->GetCollisionHeight());
		actor->SetLocation( NewLocation );
		actor->SetDesiredRotation( PlayerStartRotation );
	}
}

//----------------------------------------------------------------------------
void LoginBase::CreateListRoleIni()
{
    PC = MMOFacade::getSingleton().GetMainController();
    MainPlayer = MMOFacade::getSingleton().GetMainPlayer();

	CreateShowName = TEXT("star_show");

	if( PC ==NULL || MainPlayer==NULL )
		return;

    //these must be ensured to be reloaded once after the scene reloads
    TActorInstanceIterator< APlayerStart > tempIter;
    while ( *tempIter )
    {
        APlayerStart* plStart = (*tempIter);
        if( plStart )
        {
            PlayerStartPostion = plStart->Location;
            HiddenPostion = PlayerStartPostion;
            HiddenPostion.Y -=  1000;
            PlayerStartRotation = plStart->Rotation;
            break;
        }
        ++tempIter;
    }

    CreateShowActors.Remove( 0, CreateShowActors.Num() );
    CreateShowActors.AddZeroed( EPCT_None*2 );

    TActorInstanceIterator< ANPC > tempNPCIter;
    while ( *tempNPCIter )
    {
        ANPC* showActor = (*tempNPCIter);
        showActor->WorldInfo  = MainPlayer->WorldInfo;
        //turn off own LightEnvironment
        //showActor->LightEnvironment->SetEnabled( FALSE );
        static_cast<AActor*>( showActor )->setPhysics( PHYS_Walking );
        static_cast<AActor*>( showActor )->SetCollision( true, false, true );

        if( showActor->ActorAttributes  )
        {
            int classType = showActor->ActorAttributes->ClassType;
            int genderType = showActor->ActorAttributes->GenderType;
            if( classType >= 0 && classType < EPCT_None && genderType != EGT_None )
            {
                if( genderType == EGT_Male )
                    CreateShowActors( classType ) = showActor;
                else
                    CreateShowActors( classType + EPCT_None ) = showActor;
            }
        }

        ++tempNPCIter;
    }

    TActorInstanceIterator< ASceneCapture2DActor > tempSCAIter;
    while ( *tempSCAIter ) 
    {
        ASceneCapture2DActor* sca = (*tempSCAIter);
        if( sca->Tag == TEXT("CreationCapture") )
        {
            CreationCapture = sca;
        }
        else if( sca->Tag == TEXT("SelectionCapture") )
        {
            SelectionCapture = sca;
            SelectCameraNormalPostion = SelectionCapture->Location;
        }
        ++tempSCAIter;
    }

    if( CreationCapture && CreationCapture->SceneCapture )
    {
        CreationCapture->SceneCapture->bEnabled = false;
    }

    if( SelectionCapture && SelectionCapture->SceneCapture )
    {
        SelectionCapture->SceneCapture->bEnabled = false;
    }
}

void LoginBase::ResetApperance()
{
	CreateRoleHairColor = 0;
	CreateRoleHairModel = 0;
	CreateRoleSkinColor = 0;
	CreateRoleFaceModel = 0;
}
