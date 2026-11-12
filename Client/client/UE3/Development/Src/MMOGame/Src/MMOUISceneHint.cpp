#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "FSMIO.h"
#include "MMOGuildManager.h"
#include "MMOUtil.h"
#include "MMOQuestManager.h"
#include "CommonGameEvents.h"
#include "DebugMMO.h"
#include "MMOVipManager.h"
#include "MMOStats.h"
#include "TeamManager.h"
#include "FriendManager.h"
#include "MMOMsgBoxManager.h"
#include "MMOPerformance.h"
#include "MMOChatManager.h"
#include "UStallManager.h"
#include "MMOMapDataManager.h"

IMPLEMENT_CLASS(UMMOUISceneHint)

typedef std::map<INT,FActorHint, M_UE3_STL_ALLOC_MAP(INT, FActorHint) > INT2FActorHintMap;
INT2FActorHintMap mapHint;

typedef std::set<UGFxObject*> GFxObjectSet;
GFxObjectSet hintPool;

static const INT stackHintMax = 320;
UGFxObject* stackHintObject[stackHintMax];
INT stackHintNum;

UBOOL g_IsMoving = FALSE;
void UMMOUISceneHint::Init()
{
	Super::Init();
	check(EventAdapter == NULL);

	for(int i=0;i<stackHintMax;i++)
	{
		UGFxObject* HeadName= this->AttachMovie( TEXT("PlayerNameHintText") , FString::Printf( TEXT("PlayerNameHintTextEx%d"), i) ,1 );
		if(HeadName==NULL)
		{
			warnf(NAME_Log, TEXT("SceneHint initialization error!"));
		}
		HeadName->AddToRoot();
		HeadName->SetVisible(FALSE);
		stackHintObject[i] = HeadName;
	}
	stackHintNum = stackHintMax;


	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_MainPlayerDataChange);
	evts.AddItem(GameEvent_UpdatePlayerVipHint);
	evts.AddItem(GameEvent_TeamMemberUpdate);
	evts.AddItem(GameEvent_TeamRemoveMember);
	evts.AddItem(GameEvent_TeamLeave);
	evts.AddItem(GameEvent_TeamCommon);
	evts.AddItem(GameEvent_CharactorRemoved);
	EventAdapter = new TGameEventAdapter<UMMOUISceneHint>(this, evts);

	GConfig->GetString(TEXT("PlayerNameColor"), TEXT("PlayerName") , playerNameColor , GUIColorIni);

	MMOUI::GetUIManager()->SetUpdateHintTextDelegate(FScriptDelegate(this, TEXT("UpdateHintText")));
	MMOUI::GetUIManager()->SetUpdateHintTextByIDDelegate(FScriptDelegate(this, TEXT("UpdateHintTextByID")));

	MMOUI::GetUIManager()->SetShowHintBtnDelegate(FScriptDelegate(this, TEXT("ShowHintBtn")));

	if( GFXPlayer ){
		GFXPlayer->AddBeforeViewUpdateDelegate( FScriptDelegate(this, TEXT("BeforeViewUpdate")) );
	}

	bRefresh = TRUE;
}

void UMMOUISceneHint::UnInit()
{
	Super::UnInit();
	FScriptDelegate nullDelgate(EC_EventParm);
	MMOUI::GetUIManager()->SetUpdateHintTextDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetUpdateHintTextByIDDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetShowHintBtnDelegate(nullDelgate);
	if( GFXPlayer ){
		GFXPlayer->RemoveBeforeViewUpdateDelegate( FScriptDelegate(EC_EventParm) );
	}
}

void UMMOUISceneHint::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch(evt->GetEventType())
	{
	case GameEvent_MainPlayerDataChange:
		{
			MMOGameEvent_MainPlayerDataChange* e = (MMOGameEvent_MainPlayerDataChange*)evt;
			switch(e->DataChangeEventType)
			{
			case MMOGameEvent_MainPlayerDataChange::EMSET_HP:
				{
					PlayerHPChange(e->CurrentIntValue, e->MaxIntValue);
				}
				break;
			//case MMOGameEvent_MainPlayerDataChange::EMSET_MP:
			//	{
			//		PlayerMPChange(e->CurrentIntValue, e->MaxIntValue);
			//	}
			//	break;
			}
		}
		break;
	case GameEvent_UpdatePlayerVipHint:
		{
			UGFxObject* HeadName = NULL;
			INT2FActorHintMap::iterator iterHint = mapHint.find(MMOFacade::getSingleton().GetMainPlayer()->ServerID);
			if (iterHint != mapHint.end())
			{
				HeadName = iterHint->second.PlayerHeadText;
			}
			if (HeadName)
			{
			//	eventSetVipState(HeadName,MMOVipManager::getSingleton().GetMyCurrentVipType());
			}
		}
			
		break;
	case GameEvent_TeamMemberUpdate:
		//{
		//	TArray<MMOTeamMember>& teamMemberList = TeamManager::getSingleton().GetTeamMemberList();
		//	for(int i = 0; i < teamMemberList.Num(); i++)
		//	{
		//		INT2FActorHintMap::iterator iterHint = mapHint.find(teamMemberList(i).ActorServerId);
		//		if (iterHint != mapHint.end())
		//		{
		//			iterHint->second.ForceUpdate = TRUE;
		//		}
		//	}
		//}
		break;
	case GameEvent_TeamLeave:
	case GameEvent_TeamRemoveMember:
		//{
		//	INT2FActorHintMap::iterator iterHint = mapHint.begin();
		//	while (iterHint != mapHint.end())
		//	{
		//		iterHint->second.ForceUpdate = TRUE;
		//		iterHint++;
		//	}
		//}
		break;

	case GameEvent_CharactorRemoved:
		{
			bRefresh = TRUE;
		}
		break;

	case GameEvent_TeamCommon:
		{
			MMOGameEvent_TeamCommon* cb = (MMOGameEvent_TeamCommon*)(evt);
			if(cb&&cb->serverID > 0)
			{
				if (cb->EventType == MMOGameEvent_TeamCommon::ETeamEventType_AddEnemy)
				{
					AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
					if( !pl || (pl->ServerID == cb->serverID))
						return;

					if (!FriendManager::getSingleton().InMyEnemyList(cb->serverID))
					{
						CMessageHandler::GetInstance().SendAddEnemy(cb->serverID);
					}
				}
				else if (cb->EventType == MMOGameEvent_TeamCommon::ETeamEventType_RemoveEnemy)
				{
					if(confirmRemoveEnemy)
					{
						confirmRemoveEnemy->SetVisible(FALSE);
						confirmRemoveEnemy = NULL;
					}
					FScriptDelegate funOk;
					funOk.Object = this;
					funOk.FunctionName = "ClickRemoveEnemyConfirmOK";

					FScriptDelegate funCancel;
					funCancel.Object = this;
					funCancel.FunctionName = "ClickRemoveEnemyConfirmCancel";

					MMOFriendMember* pkMate = FriendManager::getSingleton().GetEnemyMemberByRoleId(cb->serverID);
					if(pkMate)
					{
						FString strHint = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FriendOperation.RemoveEnemy") ),*(pkMate->ActorName));
						confirmRemoveEnemy = MMOMsgBoxManager::getSingleton().AddModalMessageBox(strHint, 0, funOk, funCancel);
						if(confirmRemoveEnemy)
							confirmRemoveEnemy->SetInt(TEXT("serverID"),cb->serverID);
					}
				}
			}
			break;
		}
	}
}

void UMMOUISceneHint::ClickRemoveEnemyConfirmOK()
{
	if(confirmRemoveEnemy)
	{
		INT nServer = confirmRemoveEnemy->GetInt(TEXT("serverID"));
		CMessageHandler::GetInstance().SendDeleteEnemy(nServer);
	}
	confirmRemoveEnemy = NULL;
}
void UMMOUISceneHint::ClickRemoveEnemyConfirmCancel()
{
	confirmRemoveEnemy = NULL;
}

void UMMOUISceneHint::NPCHpChange(class ANPC* npc,INT hp)
{
	if(npc)RefleshHints(npc->ServerID,ESHU_HP);
}

void UMMOUISceneHint::OtherPlayerHpChange(class AMMOPlayer* pl,INT HP)
{
	if(pl)RefleshHints(pl->ServerID,ESHU_HP);
}
void UMMOUISceneHint::PlayerHPChange(INT NewHP, INT MaxHP)
{
	if(MMOFacade::getSingleton().GetMainPlayer())
	{
		RefleshHints(MMOFacade::getSingleton().GetMainPlayer()->ServerID,ESHU_HP);
	}
}
void UMMOUISceneHint::OtherPlayerTitleChange( class AMMOPlayer* pl,INT TitleID )
{
	if(pl)RefleshHints(pl->ServerID);
}
void UMMOUISceneHint::PlayerTitleChange( INT TitleID )
{
	if(MMOFacade::getSingleton().GetMainPlayer())
	{
		RefleshHints(MMOFacade::getSingleton().GetMainPlayer()->ServerID);
	}
}


void UMMOUISceneHint::OtherPlayerPkValueChange(class AMMOPlayer* pl, int PkValue )
{
	if(pl)RefleshHints(pl->ServerID,ESHU_NAME);
}

void UMMOUISceneHint::PlayerPkValueChange( int PkValue )
{
	if(MMOFacade::getSingleton().GetMainPlayer())
	{
		RefleshHints(MMOFacade::getSingleton().GetMainPlayer()->ServerID,ESHU_NAME);
	}
}

void UMMOUISceneHint::AddNewNpcData( class ANPC* npc )
{
	if(npc)RefleshHints(npc->ServerID);
}

void UMMOUISceneHint::OnShutDown()
{
	if(bShutDown)return;
	bShutDown = TRUE;

	//Smart delete
	INT2FActorHintMap::iterator HintItor = mapHint.begin();
	for( HintItor = mapHint.begin(); HintItor!=mapHint.end();	 )
	{
		//eventRemoveChild(HintItor->second.PlayerHeadText );
		HintItor->second.PlayerHeadText->RemoveFromRoot();
		mapHint.erase(HintItor++);
	}

	for(INT i=0;i<stackHintNum;i++)
	{
		stackHintObject[i]->RemoveFromRoot();
	}
	stackHintNum=0;
}


void UMMOUISceneHint::ClearUI()
{

}

bool g_updateMainPlayerHudName = true;


void UMMOUISceneHint::RefleshHintsByTemp(INT nTempleteID)
{
	//Batch refresh by template ID
	if (nTempleteID < 0)
	{
		INT2FActorHintMap::iterator HintItor = mapHint.begin();
		for( HintItor = mapHint.begin(); HintItor!=mapHint.end();HintItor++	 )
		{
			HintItor->second.ForceUpdate = TRUE;
			HintItor->second.UpdateType = ESHU_ALL;
		}
	}
	else if (nTempleteID > 0)
	{
		INT2FActorHintMap::iterator HintItor = mapHint.begin();
		for( HintItor = mapHint.begin(); HintItor!=mapHint.end();HintItor++	 )
		{
			if (HintItor->second.TempleteID == nTempleteID)
			{
				HintItor->second.ForceUpdate = TRUE;
				HintItor->second.UpdateType = ESHU_ALL;
			}
		}
	}
}

void UMMOUISceneHint::RefleshAllHints()
{
	if(bShutDown)return;

	INT2FActorHintMap::iterator HintItor = mapHint.begin();
	for( HintItor = mapHint.begin(); HintItor!=mapHint.end();HintItor++	 )
	{
		HintItor->second.ForceUpdate = TRUE;
		HintItor->second.UpdateType = ESHU_ALL;
	}
	g_updateMainPlayerHudName = TRUE;
}

void UMMOUISceneHint::RefleshAllNPCHints()
{
	if(bShutDown)return;

	INT2FActorHintMap::iterator HintItor = mapHint.begin();
	for( HintItor = mapHint.begin(); HintItor!=mapHint.end();HintItor++	 )
	{
		if(HintItor->second.ActorType == EACT_NPC || HintItor->second.ActorType == EACT_Monster || HintItor->second.ActorType == EACT_Boss )
		{
			HintItor->second.ForceUpdate = TRUE;
			HintItor->second.UpdateType = ESHU_ALL;
		}
	}
}

void UMMOUISceneHint::RefleshHints(INT nServerID,BYTE updateType)
{
	if(bShutDown)return;
	INT2FActorHintMap::iterator pos = mapHint.find(nServerID);
	if (pos != mapHint.end())
	{
		if(updateType==ESHU_HP ||updateType==ESHU_NAME)
		{
			if (pos->second.ForceUpdate == FALSE)
			{
				pos->second.UpdateType = updateType;
			}
		}
		else
		{
			pos->second.ForceUpdate = TRUE;
			pos->second.UpdateType = ESHU_ALL;		
		}
	}

	AMMOPlayer* self = MMOFacade::getSingleton().GetMainPlayer();
	if(self && nServerID == self->ServerID ){
		g_updateMainPlayerHudName = TRUE;
	}
}

void UMMOUISceneHint::UpdateHints_Pos(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_UI_SceneHint);
	CTRL_TICK("HUD_MMOUISceneHint::UpdateHints_Pos");

	if(bShutDown)
		return;

	if( !IsVisible  )
		return;
	
	ULocalPlayer* lp = Cast<ULocalPlayer>(MMOFacade::getSingleton().GetMainController()->Player);
	if(!lp)
	{
		return;
	}

	FLOAT HudNameDisplayRangeSquared2D = 0.0f;
	GConfig->GetFloat(TEXT("HUD"), TEXT("HudNameDisplayRangeSquared2D"), HudNameDisplayRangeSquared2D, GUIIni);

	

	//Self
	AMMOPlayer* self = MMOFacade::getSingleton().GetMainPlayer();
	if(self)
	{
		g_IsMoving = self->isMoving;

		//static UBOOL bIsMoving = FALSE;
		//static INT   nAlphaHoldFrame = 2;
		//if(g_IsMoving != bIsMoving)
		//{
		//	if(nAlphaHoldFrame>0)
		//	{
		//		nAlphaHoldFrame--;
		//	}
		//	else{
		//		bIsMoving = g_IsMoving;
		//		this->SetFloat(TEXT("alpha"), bIsMoving?0.8f:1.0f );
		//	}
		//}
		//else
		//{
		//	nAlphaHoldFrame=2;
		//}


		//if(lp->Actor && lp->Actor->PlayerCamera)
		//{
		//	AMMOPlayerCamera* plcam = Cast<AMMOPlayerCamera>(lp->Actor->PlayerCamera);
		//if(g_updateMainPlayerHudName)
		{				
			MMOFacade::getSingleton().UpdateScreenPixelFromWorld();
			UpdateHints_Player(self,lp, FALSE, HudNameDisplayRangeSquared2D);
			g_updateMainPlayerHudName = false;
		}
		//}
	}

	////player traversal
	//STLInt2IntMap* PlayerMap = MMOFacade::getSingleton().GetPlayerMap();
	//STLInt2IntMap::iterator PlayerMapItor ;
	//for ( PlayerMapItor= PlayerMap->begin();  PlayerMapItor !=PlayerMap->end(); PlayerMapItor++ )
	//{
	//	AMMOPlayer* pl = Cast<AMMOPlayer>(MMOFacade::getSingleton().GetCharacter(PlayerMapItor->first) );
	//	if(pl)
	//	{
	//		UpdateHints_Player(pl,lp,FALSE, HudNameDisplayRangeSquared2D);
	//		MMOFacade::getSingleton().RunBlockCheck(pl->ServerLocation);
	//	}
	//}

	//npc traversal player traversal
	STLInt2IntMap* ActorMap = MMOFacade::getSingleton().GetCharacterMap();
	STLInt2IntMap::iterator ActorMapItor ;
	UBOOL bBlockCheck = MMOFacade::getSingleton().GetBlockCheck();
	MMOFacade::getSingleton().StartObjectsOrder();
	for ( ActorMapItor= ActorMap->begin();  ActorMapItor !=ActorMap->end(); ActorMapItor++ )
	{
		AMMOActor* pAMMOActor = Cast<AMMOActor>(UObject::GetIndexedObject(ActorMapItor->second));
		if (pAMMOActor)
		{
			if(pAMMOActor->GetActorType()==EACT_OtherPlayer)
			{
				AMMOPlayer* pl = Cast<AMMOPlayer>(pAMMOActor);
				if(pl)
				{
					UpdateHints_Player(pl,lp,FALSE, HudNameDisplayRangeSquared2D);
				}
			}
			else
			{
				ANPC* npc = Cast<ANPC>(pAMMOActor);
				if(npc)
				{
					UpdateHints_NPC(npc,lp,FALSE, HudNameDisplayRangeSquared2D);
				}
			}

			MMOFacade::getSingleton().OperateObjectsOrder(pAMMOActor);

			if(bBlockCheck )
			{
				if (pAMMOActor->CurBlockType>0 || pAMMOActor->GetActorType() == EACT_Boss)
				{
					MMOFacade::getSingleton().RunBlockCheck(pAMMOActor);
				}
			}
		}
	}

	MMOFacade::getSingleton().DoBlockCheck();

	//Smart delete
	if(bRefresh)
	{
		bRefresh = FALSE;

		INT2FActorHintMap::iterator HintItor = mapHint.begin();
		for( HintItor = mapHint.begin(); HintItor!=mapHint.end();	 )
		{
			AActor* pA = MMOFacade::getSingleton().GetActor(  HintItor->second.PlayerNo );
			if(pA==NULL)
			{
				if(stackHintNum<stackHintMax)
				{
					HintItor->second.PlayerHeadText->SetVisible(FALSE);
					HintItor->second.IsVisible = FALSE;
					stackHintObject[stackHintNum] = HintItor->second.PlayerHeadText;
					stackHintNum++;
					mapHint.erase(HintItor++);
				}
				else
				{
					eventRemoveChild(HintItor->second.PlayerHeadText );
					HintItor->second.PlayerHeadText->RemoveFromRoot();
					mapHint.erase(HintItor++);
				}
			}
			else
			{
				HintItor++;
			}
		}
	}

	// Sabak statue
	MMOGuildManager::getSingleton().UpdateStatueHint(TRUE);
}

void UMMOUISceneHint::EnterScene()
{
	if(bShutDown)return;
	INT2FActorHintMap::iterator pos = mapHint.begin();// .find(nServerID);
	for(;pos != mapHint.end();pos++)
	{
		pos->second.ForceUpdate = TRUE;
	}
}


void UMMOUISceneHint::UpdateGreyName_Player(UGFxObject* HeadName, class AMMOPlayer* pl)
{
	if(HeadName==NULL||pl==NULL)return;
	FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	UBOOL HideMapRoleName = curMapInfo.HideRoleName;
	UBOOL HintHideNameOther = MMOUI::GetUIManager()->HintHideNameOther;
	UBOOL HintHideNameOtherTitle = MMOUI::GetUIManager()->HintHideNameOtherTitle;
	UBOOL HintHideNameFamilyOther = MMOUI::GetUIManager()->HintHideNameFamilyOther;
	if(HideMapRoleName)
	{
		//HintHideNameOther = TRUE;
		HintHideNameOtherTitle = TRUE;
		HintHideNameFamilyOther = TRUE;	
	}

	FString ActorName = pl->GetActorName();
	if (pl->IsMainPlayer() && MMOUI::GetUIManager()->HintHideNameSelf)
		ActorName.Empty();
	if (!pl->IsMainPlayer() && HintHideNameOther)
		ActorName.Empty();

	FString NameColor = GetPlayerNameColor(pl);
	eventSetPlayerTxt_Name(HeadName, GetPlayerNameFrame(pl)	,ActorName,NameColor);
}

void UMMOUISceneHint::UpdateHp_Player(UGFxObject* HeadName, class AMMOPlayer* pl)
{
	if(HeadName==NULL||pl==NULL)return;

	if(pl->MoveType!=EMT_InStall && pl->MoveType!=EMT_PrepareStall)
	{
		bool set_hp = true;
		if (pl->ActorType == EACT_OtherPlayer)
			set_hp = MMOUI::GetUIManager()->BloodDrawPlayer;
		if (pl->ActorType == EACT_PlayerSelf )
			set_hp = MMOUI::GetUIManager()->BloodDrawSelf;
		if (pl->ActorType == EACT_Monster || pl->ActorType == EACT_Boss || pl->ActorType == EACT_MonsterBody)
			set_hp = MMOUI::GetUIManager()->BloodDrawMonster;

		if(set_hp)
		{
			if( pl->IsMainPlayer())
			{
				if(pl->ActorAttributes->HP>0 && pl->bDead==FALSE)
				{
					if(pl->ActorAttributes->HP*4<=pl->ActorAttributes->GetMaxHp())
						eventSetHpInfo(  HeadName, pl->ActorAttributes->HP  , pl->ActorAttributes->GetMaxHp() , 2,  1 );// 2);
					else
						eventSetHpInfo(  HeadName, pl->ActorAttributes->HP  , pl->ActorAttributes->GetMaxHp() , 1,  1 );// 2);
				}
				else
				{
					eventSetHpInfo(  HeadName, pl->ActorAttributes->HP  , pl->ActorAttributes->GetMaxHp() , 1,  0 );// 2);
				}
			}
			else if( MMOFacade::getSingleton().IsAttackable(pl) )
			{
				if(pl->ActorAttributes->HP>0 && pl->bDead==FALSE)
				{
					if(pl->ShowHpNumber)
					{
						eventSetHpInfo(  HeadName, pl->ActorAttributes->HP  , pl->ActorAttributes->GetMaxHp() , 2,  1);
					}
					else
					{
						eventSetHpInfo(  HeadName, pl->ActorAttributes->HP  , pl->ActorAttributes->GetMaxHp() , 2,  2);
					}
				}
				else
				{
					eventSetHpInfo(  HeadName, pl->ActorAttributes->HP  , pl->ActorAttributes->GetMaxHp() , 2,  0);// 2);
				}
			}
			else
			{
				if(pl->ActorAttributes->HP>0 && pl->bDead==FALSE)
				{
					if(pl->ShowHpNumber)
					{
						eventSetHpInfo(  HeadName, pl->ActorAttributes->HP  , pl->ActorAttributes->GetMaxHp() , 1,  1);
					}
					else
					{
						eventSetHpInfo(  HeadName, pl->ActorAttributes->HP  , pl->ActorAttributes->GetMaxHp() , 1,  2);// 2);
					}
				}
				else
				{
					eventSetHpInfo(  HeadName, pl->ActorAttributes->HP  , pl->ActorAttributes->GetMaxHp() , 1,  0);// 2);
				}
			}
		}
		else
		{
			eventSetHpInfo(  HeadName, 100, 100, 1,  0);
		}
	}
}

void UMMOUISceneHint::UpdateHints_Player(class AMMOPlayer* pl,class ULocalPlayer* lp,UBOOL Force, FLOAT maxViewDist)
{
	SCOPE_CYCLE_COUNTER(STAT_UI_SceneHint_Player);
	CTRL_TICK("HUD_MMOUISceneHint::UpdateHints_Player");

	static INT  nTest = 0;
	if(nTest==2)return;

	if(pl==NULL)
	{
		debugf(NAME_Error, TEXT("------------------UpdateHints_Player input is NULL"));
		return;
	}

	UGFxObject* HeadName = NULL;
	FActorHint* pActorHint=NULL;

	INT2FActorHintMap::iterator iterHint = mapHint.find(pl->ServerID);
	if (iterHint != mapHint.end())
	{
		pActorHint = &(iterHint->second);
		HeadName = iterHint->second.PlayerHeadText;
	}

	//if(pl && pl->ShowHeadName==FALSE)
	//{
	//	return;
	//}


	//--Distance check, beyond the distance do nothing but hide------------------------------------------------------------------------------
	FLOAT dist = (pl->Location - Player->Location).SizeSquared2D();
	if(dist > maxViewDist)
	{
		if(HeadName&&pActorHint->IsVisible)
		{
			pActorHint->IsVisible = FALSE;
			HeadName->SetVisible(FALSE);
		}
		return;
	}

	UBOOL bUpdatePos=FALSE;
	//Add headName UI
	if(HeadName==NULL)
	{
		if(stackHintNum>0)
		{
			HeadName = stackHintObject[stackHintNum-1];
			HeadName->SetVisible(FALSE);
			eventSetHpInfo(  HeadName, 100, 100, 1,  0);
			stackHintObject[stackHintNum-1]=NULL;
			stackHintNum -= 1;
		}
		else
		{
			HeadName= this->AttachMovie( TEXT("PlayerNameHintText") , FString::Printf( TEXT("PlayerNameHintText%d"), pl->ServerID) ,1 );
			HeadName->AddToRoot();
			HeadName->SetVisible(FALSE);
			eventSetHpInfo(  HeadName, 100, 100, 1,  0);
		}

		//Add to local table
		FActorHint PH(EC_EventParm);
		PH.PlayerHeadText = HeadName;	
		PH.PlayerNo = pl->ServerID;
		PH.PlayerString = pl->GetActorName();
		PH.IconName		= TEXT("");
		PH.ForceUpdate = FALSE;
		PH.UpdateType = ESHU_ALL;
		PH.ForceUpdateCountDown = rand()%3;//-1; 
		PH.ActorType = pl->GetActorType();
		PH.IsVisible = FALSE;
		pActorHint = &PH;

		mapHint.insert(std::make_pair(pl->ServerID, PH));
	}
	else
	{

		if(pActorHint->ForceUpdate)
		{
			pActorHint->ForceUpdate = FALSE;
			pActorHint->ForceUpdateCountDown = rand()%10+2;
		}
		else if(pActorHint->ForceUpdateCountDown>=0)
		{
			pActorHint->ForceUpdateCountDown--;
			if(pActorHint->ForceUpdateCountDown<0)
			{
				Force = TRUE;
				bUpdatePos = TRUE;
				pActorHint->UpdateType = ESHU_NONE;
			}
		}
		else if(pActorHint->UpdateType==ESHU_HP )
		{
			pActorHint->UpdateType=ESHU_NONE;
			bUpdatePos = TRUE;
			UpdateHp_Player(HeadName,pl);
		}
		else if(pActorHint->UpdateType==ESHU_NAME )
		{
			pActorHint->UpdateType=ESHU_NONE;
			bUpdatePos = TRUE;
			UpdateGreyName_Player(HeadName,pl);
		}

		

	}
		
	//--Confirm components are complete--------------------------------------------------------------------------
	if(HeadName==NULL||pActorHint==NULL)
	{
		return;
	}

	////--visible refresh---------------------------------------------------------------------------
	if(pActorHint->IsVisible==FALSE && pActorHint->ForceUpdate==FALSE && pActorHint->ForceUpdateCountDown<0)
	{
		pActorHint->IsVisible = TRUE;
		HeadName->SetVisible(TRUE);
	}


	//--alpha fade----------------------------------------------------------------------------

	FLOAT InAlpha = 1.0;
	if(dist <= maxViewDist && dist > maxViewDist/2)
	{
		InAlpha = (maxViewDist-dist)/(maxViewDist/2);
	}

	if(pActorHint->CurAlpha>InAlpha+0.01 || pActorHint->CurAlpha<InAlpha-0.01)
	{
		HeadName->SetFloat(TEXT("alpha"), InAlpha );
		pActorHint->CurAlpha=InAlpha;
	}
		
		

	//--Position binding, projecting from 3d to 2d-------------------------------------------------------------------------
	FVector2D ScreenPos;
	FLOAT  h = 0;
	if(pl->CylinderComponent)
	{
		h=pl->CylinderComponent->CollisionHeight*1.2;
	}
	if(pl->UseingAvatar)
	{
		h+=pl->UseingAvatar->HintNameOffsetZ;
	}

	UBOOL suc = MMOFacade::getSingleton().GetScreenPixelFromWorld( pl->Location + FVector(0,0,h) , ScreenPos );
	if(suc)
	{
		INT dx = pActorHint->CurScreenX-ScreenPos.X;
		INT dy = pActorHint->CurScreenY-ScreenPos.Y;

		if(dx*dx+dy*dy>0 || bUpdatePos)
		{
			HeadName->SetPosition( ScreenPos.X ,ScreenPos.Y );//image center
			pActorHint->CurScreenX=ScreenPos.X;
			pActorHint->CurScreenY=ScreenPos.Y;
		}
	}

	//--Detail parsing, component representation-------------------------------------------------------------------------------------
	if(Force)
	{
		//Detail parsing, component representation
		if( pl->MoveType == EMT_InStall  || pl->MoveType == EMT_PrepareStall )
		{

			FString ActorName = pl->GetActorName();
			UGFxObject* btSelected = HeadName->GetObject(TEXT("btn_Selected"));
			if (btSelected)
			{
				btSelected->SetString(TEXT("label"), *ActorName);
			}

			//eventSetPlayerTxt_Title(HeadName, TEXT("Title"), TEXT(""), TEXT(""));
			eventUCSetAllVisible(HeadName, FALSE);
			eventSetIconInfo(HeadName,TEXT(""),TEXT(""));
			eventSetStallTxtInfo(HeadName, ActorName, pl->StallName, UStallManager::getSingleton().IsAttentionStall(pl->StallName));
			CloseActorTalkBubble(pl->ServerID);
			eventUpdateDisplay(HeadName);
			HeadName->SetInt(TEXT("serverID"), pl->ServerID);
		}
		else
		{
			FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
			UBOOL HideMapRoleName = curMapInfo.HideRoleName;
			UBOOL HintHideNameOther = MMOUI::GetUIManager()->HintHideNameOther;
			UBOOL HintHideNameOtherTitle = MMOUI::GetUIManager()->HintHideNameOtherTitle;
			UBOOL HintHideNameFamilyOther = MMOUI::GetUIManager()->HintHideNameFamilyOther;
			if(HideMapRoleName)
			{
				//HintHideNameOther = TRUE;
				HintHideNameOtherTitle = TRUE;
				HintHideNameFamilyOther = TRUE;	
			}

			eventUCSetAllVisible(HeadName, FALSE);
			eventSetIconInfo(HeadName,TEXT(""),TEXT(""));
			//eventSetStallTxtInfo(HeadName,TEXT(""),TEXT(""),FALSE);
			//eventSetIconInfo(HeadName,TEXT(""),TEXT(""));

			TitleData* ValupTitle = MMOFacade::getSingleton().GetTitleData(pl->ActorAttributes->TitleID);
			FString TitleName = TEXT("");
			FString TitleIcon = TEXT("");
			INT		HoverShow = 0;
			if(ValupTitle)
			{
				TitleIcon = FString::Printf(TEXT("img://MMO_UITitle.Title_%d"),ValupTitle->ID);
				TitleName = ValupTitle->TitleName;
				HoverShow = ValupTitle->HoverShow;
			}
				

			UpdateHp_Player(HeadName, pl);



			FString ActorName = pl->GetActorName();
			if (pl->IsMainPlayer() && MMOUI::GetUIManager()->HintHideNameSelf)
				ActorName.Empty();
			if (!pl->IsMainPlayer() && HintHideNameOther) // MMOUI::GetUIManager()->HintHideNameOther)
				ActorName.Empty();

				
			FString NameColor = GetPlayerNameColor(pl);
				
			if (pl->IsMainPlayer())
			{
				/*
					Name is called first, then vip, then enemy marker
				*/
				eventSetPlayerTxt_Name(HeadName, GetPlayerNameFrame(pl)	,ActorName,NameColor);
//				eventSetVipState(HeadName,MMOVipManager::getSingleton().GetMyCurrentVipType());
			}
			else
			{
				/*
					Name is called first, then vip, then enemy marker
				*/
				eventSetPlayerTxt_Name(HeadName, GetPlayerNameFrame(pl)	,ActorName,NameColor);
//				eventSetVipState(HeadName,pl->GetVipType());
				if (HideMapRoleName)
				{
					eventsetEnemy(HeadName, FALSE);
				}
				else
				{
					eventsetEnemy(HeadName, FriendManager::getSingleton().InMyEnemyList(pl->ServerID));
				}
			}


			if (pl->IsMainPlayer() && MMOUI::GetUIManager()->HintHideNameSelfTitle )
			{
				eventSetPlayerTxt_Title(HeadName, TEXT("Title"),TEXT(""),  TEXT(""));
			}
			else if(pl->IsMainPlayer()==FALSE && HintHideNameOtherTitle) // MMOUI::GetUIManager()->HintHideNameOtherTitle)
			{
				eventSetPlayerTxt_Title(HeadName, TEXT("Title"),TEXT(""),  TEXT(""));
			}
			else
			{
				if (pl->IsMainPlayer())
				{
					eventSetPlayerTxt_Title(HeadName, TEXT("Title")	,TitleName,  TitleIcon);
				}
				else if(HoverShow && pl!=FSM::Target.CurTargetMMOPlayer)
				{
					eventSetPlayerTxt_Title(HeadName, TEXT("Title"),TEXT(""),  TEXT(""));
				}
				else
				{
					eventSetPlayerTxt_Title(HeadName, TEXT("Title")	,TitleName,  TitleIcon);
				}
			}

			// guild
			FString guildName = TEXT("");
			if (!pl->GuildName.IsEmpty())
			{
				guildName += TEXT("<");
				guildName += pl->GuildName;
				guildName += TEXT(">");
			}

			if (pl->IsMainPlayer() && MMOUI::GetUIManager()->HintHideNameFamilySelf)
			{
				guildName.Empty();
			}

			if (!pl->IsMainPlayer() && HintHideNameFamilyOther) // MMOUI::GetUIManager()->HintHideNameFamilyOther)
			{
				guildName.Empty();
			}

			eventSetPlayerTxt_Guild(HeadName, GetPlayerGuildFrame(pl->GuildName), guildName);
			eventSetSabacState(HeadName, guildName.IsEmpty() ? FALSE : MMOGuildManager::getSingleton().IsSabacOccupyGuild(pl->GuildName));


			if (pl->IsMainPlayer()==FALSE)
			{
				eventSetSelectButton(HeadName,bShowHintBtn);
			}
			eventUpdateDisplay(HeadName);
								
			HeadName->SetInt(TEXT("serverID"),pl->ServerID);

		}
	}
	
}

FString UMMOUISceneHint::GetPlayerGuildFrame(const FString &strGuildName)
{
	GuildRelation re = MMOGuildData::getSingleton().GetGuildRelation(strGuildName);
	if(re==GR_FRIEND)
	{
		return TEXT("Guild");
	}
	else if(re==GR_ENEMY)
	{
		return TEXT("Guild1");
	}
	else
	{
		const GuildBaseInfo& guildBaseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
		if(guildBaseInfo.Name == strGuildName)
		{
			return TEXT("Guild");
		}
		else
		{
			return TEXT("Guild2");
		}
	}
}

FString UMMOUISceneHint::GetPlayerNameColor(AMMOPlayer* pl)
{
	AMMOPlayerController* playerCtrl = MMOFacade::getSingleton().GetMainController();
	if(playerCtrl==NULL||pl==NULL)return TEXT("Player3");

	//1. Sabak, attacker/defender----------------------------------------------------------------------------------
	MMOTeamMember *pTeammate = TeamManager::getSingleton().GetMemberById( pl->ServerID );
	if(pl->inSabacZoneCount > 0)
	{
		GuildSabacState state = MMOGuildData::getSingleton().GetGuildSabacState(pl);
		if(state == GSS_ORANGE)
		{
			return TEXT("");
		}
		else if(state == GSS_BLUE)
		{
			return TEXT("");
		}
		else if(state == GSS_GREEN)
		{
			return TEXT("");
		}
	}

	//2. Hostile faction-----------------------------------------------------------------------------------------
	FExportMap *pMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( playerCtrl->CurMapID );
	INT mapType = E_est_Invalid;
	if( pMapInfo != NULL )
	{
		mapType = pMapInfo->ServerMapType;
	}
	if( mapType == E_est_BattleField || mapType == E_est_Arena)
	{
		if(MMOFacade::getSingleton().IsAttackable(pl))
		{
			return TEXT("");
		}
		else
		{
			return TEXT("");
		}
	}

	//3. Gray name----------------------------------------------------------------------------------------------
	if(pl->ActorAttributes && pl->ActorAttributes->IsGreyName )
	{
		return TEXT("");
	}

	//4. Special name--------------------------------------------------------------------------------------------
	if(pl->ActorAttributes )
	{
		if(pl->ActorAttributes->FrameEx1.Len()>0){
			return TEXT("");
		}
		else if(pl->ActorAttributes->FrameEx2.Len())
		{
			return TEXT("");
		}
	}

	//5. Natural display
	if (pl->ActorAttributes->PkValue >= 100)
	{
		return TEXT("");
	}

	if(pl->ActorAttributes->ColorEx2.Len()>0){
		return pl->ActorAttributes->ColorEx2;
	}
	else if(pl->ActorAttributes->ColorEx1.Len()>0)
	{
		return pl->ActorAttributes->ColorEx1;
	}
	return TEXT("");
}

FString UMMOUISceneHint::GetPlayerNameFrame(AMMOPlayer* pl)
{
	AMMOPlayerController* playerCtrl = MMOFacade::getSingleton().GetMainController();
	if(playerCtrl==NULL||pl==NULL)return TEXT("Player3");

	//1. Sabak, attacker/defender----------------------------------------------------------------------------------
	MMOTeamMember *pTeammate = TeamManager::getSingleton().GetMemberById( pl->ServerID );
	if(pl->inSabacZoneCount > 0)
	{
		GuildSabacState state = MMOGuildData::getSingleton().GetGuildSabacState(pl);
		if(state == GSS_ORANGE)
		{
			return TEXT("sbk3");
		}
		else if(state == GSS_BLUE)
		{
			return TEXT("sbk1");
		}
		else if(state == GSS_GREEN)
		{
			return TEXT("sbk2");
		}
	}

	//2. Hostile faction-----------------------------------------------------------------------------------------
	FExportMap *pMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( playerCtrl->CurMapID );
	INT mapType = E_est_Invalid;
	if( pMapInfo != NULL )
	{
		mapType = pMapInfo->ServerMapType;
	}
	if( mapType == E_est_BattleField || mapType == E_est_Arena)
	{
		if(MMOFacade::getSingleton().IsAttackable(pl))
		{
			return TEXT("Group4");
		}
		else
		{
			return TEXT("Group3");
		}
	}

	//3. Gray name----------------------------------------------------------------------------------------------
	if(pl->ActorAttributes && pl->ActorAttributes->IsGreyName )
	{
		return TEXT("Grey");
	}

	//4. Special name--------------------------------------------------------------------------------------------
	if(pl->ActorAttributes )
	{
		if(pl->ActorAttributes->FrameEx1.Len()>0){
			return pl->ActorAttributes->FrameEx1;
		}
		else if(pl->ActorAttributes->FrameEx2.Len())
		{
			return pl->ActorAttributes->FrameEx2;
		}
	}


	//5. Natural display
	if(pl->IsMainPlayer() && pl->ActorAttributes->PkValue<100)return TEXT("Player1");
	else if(pTeammate && pl->ActorAttributes->PkValue<100)return TEXT("Group1");
	else if(pl->ActorAttributes->PkValue<100)return TEXT("Player3");
	else if(pl->ActorAttributes->PkValue<300)return TEXT("Player4");
	else if(pl->ActorAttributes->PkValue<500)return TEXT("Player5");
	else if(pl->ActorAttributes->PkValue<800)return TEXT("Player6");
	else return TEXT("Player7");



	//if(pl && pl->ActorAttributes && pl->ActorAttributes->IsGreyName )
	//{
	//	return TEXT("Grey");
	//}

	//if(pl && pl->ActorAttributes )
	//{
	//	if(pl->ActorAttributes->FrameEx1.Len()>0){
	//		return pl->ActorAttributes->FrameEx1;
	//	}
	//	else if(pl->ActorAttributes->FrameEx2.Len())
	//	{
	//		return pl->ActorAttributes->FrameEx2;
	//	}
	//}

	//FExportMap *pMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( playerCtrl->CurMapID );
	//INT mapType = E_est_Invalid;
	//if( pMapInfo != NULL )
	//{
	//	mapType = pMapInfo->ServerMapType;
	//}

	//MMOTeamMember *pTeammate = TeamManager::getSingleton().GetMemberById( pl->ServerID );

	//if(pl && pl->inSabacZoneCount > 0)
	//{
	//	GuildSabacState state = MMOGuildData::getSingleton().GetGuildSabacState(pl);
	//	if(state == GSS_ORANGE)
	//	{
	//		return TEXT("sbk3");
	//	}
	//	else if(state == GSS_BLUE)
	//	{
	//		return TEXT("sbk1");
	//	}
	//	else if(state == GSS_GREEN)
	//	{
	//		return TEXT("sbk2");
	//	}
	//	else{

	//		if(pl->IsMainPlayer() && pl->ActorAttributes->PkValue<100)return TEXT("Player1");
	//		else if(pTeammate && pl->ActorAttributes->PkValue<100)return TEXT("Group1");
	//		else if(pl->ActorAttributes->PkValue<100)return TEXT("Player3");
	//		else if(pl->ActorAttributes->PkValue<300)return TEXT("Player4");
	//		else if(pl->ActorAttributes->PkValue<500)return TEXT("Player5");
	//		else if(pl->ActorAttributes->PkValue<800)return TEXT("Player6");
	//		else return TEXT("Player7");
	//	}
	//	
	//}


	//if( mapType == E_est_BattleField || mapType == E_est_Guild  || mapType == E_est_Arena)
	//{
	//	if(MMOFacade::getSingleton().IsAttackable(pl))
	//	{
	//		return TEXT("Group4");
	//	}
	//	else
	//	{
	//		return TEXT("Group3");
	//	}
	//}
	//else
	//{

	//	if(pl->IsMainPlayer() && pl->ActorAttributes->PkValue<100)return TEXT("Player1");
	//	else if(pTeammate && pl->ActorAttributes->PkValue<100)return TEXT("Group1");
	//	else if(pl->ActorAttributes->PkValue<100)return TEXT("Player3");
	//	else if(pl->ActorAttributes->PkValue<300)return TEXT("Player4");
	//	else if(pl->ActorAttributes->PkValue<500)return TEXT("Player5");
	//	else if(pl->ActorAttributes->PkValue<800)return TEXT("Player6");
	//	else return TEXT("Player7");

	//}



}

//INT UMMOUISceneHint::GetCroupId(class AMMOActor* actor)
//{
//	AMMOPlayerController* playerCtrl = MMOFacade::getSingleton().GetMainController();
//	if(playerCtrl==NULL)return 0;
//
//	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
//	if(player==0)return 0;
//
//	FExportMap *pMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( playerCtrl->CurMapID );
//	INT mapType = E_est_Invalid;// _None;
//	if( pMapInfo != NULL )
//	{
//		mapType = pMapInfo->ServerMapType;
//	}
//
//	if(actor && actor->bInSabacZone )
//	{
//		GuildSabacState state = MMOGuildData::getSingleton().GetGuildSabacState(actor->GuildName);
//		if(state==GSS_ATTACK)
//		{
//			return 4;
//		}
//		else if(state==GSS_DEFEND)
//		{
//			return 3;
//		}
//		else{
//			return 0;
//		}
//	}
//
//	if( mapType == E_est_BattleField )
//	{
//		if(MMOFacade::getSingleton().IsAttackable(player,actor))
//		{
//			return 4;
//		}
//		else
//		{
//			return 3;
//		}
//	}
//	else
//	{
//		return 0;
//	}
//}

UBOOL UMMOUISceneHint::CreateNpcHint(class ANPC* npc)
{
	if(npc==NULL)return FALSE;

	INT2FActorHintMap::iterator iterHint = mapHint.find(npc->ServerID);
	if(iterHint != mapHint.end())return FALSE;

	UGFxObject* HeadName=NULL;
	if(stackHintNum>0)
	{
		HeadName = stackHintObject[stackHintNum-1];
		HeadName->SetVisible(FALSE);
		stackHintObject[stackHintNum-1]=NULL;
		stackHintNum -= 1;
	}
	else
	{
		HeadName= this->AttachMovie( TEXT("PlayerNameHintText") , FString::Printf( TEXT("PlayerNameHintText%d"), npc->ServerID) ,1 );
		HeadName->SetVisible(FALSE);
		HeadName->AddToRoot();
	}

	//UGFxObject* HeadName = this->AttachMovie( TEXT("PlayerNameHintText") , FString::Printf( TEXT("PlayerNameHintText%d"), npc->ServerID),1  );
	//HeadName->AddToRoot();

	//Add to local table
	FActorHint PH(EC_EventParm);
	PH.PlayerHeadText = HeadName;
	PH.PlayerNo = npc->ServerID;
	PH.PlayerString = npc->DisplayName;
	PH.TempleteID = npc->NPCID;
	PH.IconName		= TEXT("");
	PH.ForceUpdate = TRUE;
	PH.IsVisible = FALSE;
	PH.ActorType = EACT_None;

	mapHint.insert(std::make_pair(npc->ServerID, PH));
	return TRUE;
}

void UMMOUISceneHint::ShowHintBtn(UBOOL bShow)
{
	bShowHintBtn = bShow;
	//FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	//if(curMapInfo.HideRoleName)
	//{
	//	bShowHintBtn = FALSE;
	//}
	UGFxObject* HeadName = NULL;
	INT2FActorHintMap::iterator HintItor = mapHint.begin();
	for( ; HintItor!=mapHint.end();	HintItor++ )
	{
		if(HintItor->second.ActorType == EACT_OtherPlayer )
		{
			HeadName = HintItor->second.PlayerHeadText;
			eventSetSelectButton(HeadName,bShowHintBtn);
		}
	}
}

void UMMOUISceneHint::UpdateHp_NPC(UGFxObject* HeadName, class ANPC* pl)
{
	if(HeadName==NULL||pl==NULL)return;
	
	INT nShowHpNumber = pl->ShowHpNumber>0?1:2;

	if(pl->GetActorType()==EACT_Pet)
	{
		INT HP		= pl->ActorAttributes->GetCurHp();
		INT MaxHP	= pl->ActorAttributes->GetMaxHp();
		AMMOPlayer* pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		BOOL isAttackeble = MMOFacade::getSingleton().IsAttackable(pl);

		if(pMainPlayer && pl->dwHostID==pMainPlayer->ServerID && HP > 0 && MaxHP > 0 )
		{
			if(MMOUI::GetUIManager()->BloodDrawSelf)
			{
				if(isAttackeble)
				{
					eventSetHpInfo(  HeadName, HP  , MaxHP , 2,  1 );
				}
				else
				{
					eventSetHpInfo(  HeadName, HP  , MaxHP , 1,  1 );
				}
			}
			else
			{
				eventSetHpInfo(  HeadName, HP  , MaxHP , 1,  0 );
			}
		}
		else if(MMOUI::GetUIManager()->BloodDrawPlayer && HP > 0 && MaxHP > 0)
		{
			if(isAttackeble){
				eventSetHpInfo(  HeadName, HP  , MaxHP , 2,  nShowHpNumber );
			}
			else
			{
				eventSetHpInfo(  HeadName, HP  , MaxHP , 1,  nShowHpNumber );
			}
		}
		else
		{
			eventSetHpInfo(  HeadName, HP  , MaxHP , 2,  0 );
		}
	}
	else if(pl->GetActorType()==EACT_Monster || pl->GetActorType()==EACT_Boss)
	{
		INT HP		= pl->ActorAttributes->GetCurHp();
		INT MaxHP	= pl->ActorAttributes->GetMaxHp();

		if(MMOUI::GetUIManager()->BloodDrawMonster && HP >= 0 && MaxHP > 0)
		{
			eventSetHpInfo(  HeadName, HP  , MaxHP , 2,  nShowHpNumber );
		}
		else
		{
			eventSetHpInfo(  HeadName, HP  , MaxHP , 2,  0 );
		}
	}

}


void UMMOUISceneHint::UpdateHints_NPC(class ANPC* pl,class ULocalPlayer* lp,UBOOL Force, FLOAT maxViewDist)
{
	SCOPE_CYCLE_COUNTER(STAT_UI_SceneHint_NPC);
	CTRL_TICK("HUD_MMOUISceneHint::UpdateHints_NPC");

	static INT  nTest = 0;
	if(nTest==2)return;

	if(pl==NULL)
	{
		debugf(NAME_Error, TEXT("------------------UpdateHints_NPC input is NULL"));
		return;
	}


	UGFxObject* HeadName = NULL;
	FActorHint* pActorHint=NULL;
	INT2FActorHintMap::iterator iterHint = mapHint.find(pl->ServerID);
	if (iterHint != mapHint.end())
	{
		pActorHint = &(iterHint->second);
		HeadName = iterHint->second.PlayerHeadText;
	}

	if(pl->eventIsDead())
	{
		if(HeadName && pActorHint->IsVisible != FALSE)
		{
			pActorHint->IsVisible = FALSE;
			HeadName->SetVisible(FALSE);
		}
		return;
	}

	UBOOL IsQuestTrace = MMOQuestManager::getSingleton().IsMonsterBeTraced(pl->NPCID);
	if(pl->Template && pl->Template->HideHUDName && IsQuestTrace==FALSE)
	{
		if(HeadName&&pActorHint->IsVisible != FALSE)
		{
			pActorHint->IsVisible = FALSE;
			HeadName->SetVisible(FALSE);
		}
		return;
	}

	if(pl && pl->ShowHeadName==FALSE && IsQuestTrace==FALSE)
	{
		if(HeadName&&pActorHint->IsVisible != FALSE)
		{
			pActorHint->IsVisible = FALSE;
			HeadName->SetVisible(FALSE);
		}
		return;
	}

	//--Distance check, beyond the distance do nothing but hide------------------------------------------------------------------------------
	FLOAT dist = (pl->Location - Player->Location).SizeSquared2D();
	if(dist > maxViewDist)
	{
		if(HeadName&&pActorHint->IsVisible != FALSE)
		{
			pActorHint->IsVisible = FALSE;
			HeadName->SetVisible(FALSE);
		}			
		return;
	}


	//--Add headName UI-------------------------------------------------------------------------------------------------
	if(HeadName==NULL)
	{
				
		if(stackHintNum>0)
		{
			HeadName = stackHintObject[stackHintNum-1];
			HeadName->SetVisible(FALSE);
			stackHintObject[stackHintNum-1]=NULL;
			stackHintNum -= 1;
		}
		else
		{
			HeadName= this->AttachMovie( TEXT("PlayerNameHintText") , FString::Printf( TEXT("PlayerNameHintText%d"), pl->ServerID) ,1 );
			HeadName->AddToRoot();
			HeadName->SetVisible(FALSE);
		}

		//Add to local table
		FActorHint PH(EC_EventParm);
		PH.PlayerHeadText = HeadName;
		PH.PlayerNo = pl->ServerID;
		PH.TempleteID = pl->NPCID;
		PH.PlayerString = pl->DisplayName;
		PH.UpdateType = ESHU_ALL;
		PH.IconName		= pl->QuestIconName;
		PH.ForceUpdate = TRUE;
		PH.ForceUpdateCountDown = -1; 
		PH.ActorType = pl->GetActorType();
		PH.IsVisible = FALSE;
		PH.IsQuestTrace = FALSE;
		pActorHint = &PH;

		mapHint.insert(std::make_pair(pl->ServerID, PH));
	}
	else
	{
		if(pActorHint->ForceUpdate)
		{
			pActorHint->ForceUpdate = FALSE;
			pActorHint->ForceUpdateCountDown = rand()%10+2;
		}
		else if(pActorHint->ForceUpdateCountDown>=0)
		{
			pActorHint->ForceUpdateCountDown--;
			if(pActorHint->ForceUpdateCountDown<0)
			{
				Force = TRUE;
			}
		}
		else if(pActorHint->UpdateType==ESHU_HP)
		{
			pActorHint->UpdateType=ESHU_NONE;
			UpdateHp_NPC(HeadName,pl);
		}
	}

	//--Confirm components are complete--------------------------------------------------------------------------
	if(HeadName==NULL||pActorHint==NULL)
	{
		return;
	}

	//--visible refresh---------------------------------------------------------------------------
	if(pActorHint->IsVisible==FALSE && pActorHint->ForceUpdate==FALSE && pActorHint->ForceUpdateCountDown<0)
	{
		pActorHint->IsVisible = TRUE;
		HeadName->SetVisible(TRUE);
	}

	//--alpha fade----------------------------------------------------------------------------
	FLOAT InAlpha = 1.0;
	if(dist <= maxViewDist && dist > maxViewDist/2)
	{
		InAlpha = (maxViewDist-dist)/(maxViewDist/2);
	}

	if(pActorHint->CurAlpha>InAlpha+0.01 || pActorHint->CurAlpha<InAlpha-0.01)
	{
		HeadName->SetFloat(TEXT("alpha"), InAlpha );
		pActorHint->CurAlpha=InAlpha;
	}
			

	//--Position binding, projecting from 3d to 2d-------------------------------------------------------------------------
	FVector2D ScreenPos;
	FLOAT  h = 0;
	if(pl->UseingAvatar)
	{
		h=pl->UseingAvatar->CollisionHeight*1.2+pl->UseingAvatar->HintNameOffsetZ;
	}
		
	UBOOL suc = MMOFacade::getSingleton().GetScreenPixelFromWorld( pl->Location + FVector(0,0,h) , ScreenPos );
	if(suc)
	{
		static FVector2D lastPos = ScreenPos;
		//if(g_IsMoving)
		//{
		//	warnf(TEXT("ScreenPos.x=%f, ScreenPos.y=%f, dif=%f,%f"), ScreenPos.X, ScreenPos.Y,ScreenPos.X-lastPos.X, ScreenPos.Y-lastPos.Y);
		//}
		lastPos = ScreenPos;

		INT dx = pActorHint->CurScreenX-ScreenPos.X;
		INT dy = pActorHint->CurScreenY-ScreenPos.Y;
		if(dx*dx+dy*dy>0)
		{
			HeadName->SetPosition( ScreenPos.X ,ScreenPos.Y );//image center
			pActorHint->CurScreenX=ScreenPos.X;
			pActorHint->CurScreenY=ScreenPos.Y;
		}
	}

	// fix bug #23679
	// Quest monster head marker changes continuously with quest state and entering/leaving the quest area, so it needs updating when it changes
	if (IsQuestTrace != pActorHint->IsQuestTrace)
	{
		pActorHint->IsQuestTrace = IsQuestTrace;
		Force = TRUE;
	}

	//--Detail parsing, component representation-------------------------------------------------------------------------------------
	if(Force)
	{

		FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
		UBOOL HideMapRoleName = curMapInfo.HideRoleName;
		UBOOL HintHideNameOther = MMOUI::GetUIManager()->HintHideNameOther;
		if(HideMapRoleName)
		{
			HintHideNameOther = TRUE;
		}

		eventSetPlayerTxt_Title(HeadName, TEXT("Title"),TEXT(""),  TEXT(""));
		eventSetStallTxtInfo(HeadName,TEXT(""),TEXT(""),FALSE);


		FString DisplayName		= pl->DisplayName;
		FString IdentityName	= pl->IdentityName;
		AMMOPlayer* pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();

		if(pl->MasterName.Len()>0)
		{
			if( pMainPlayer && pMainPlayer->ServerID==pl->dwHostID && MMOUI::GetUIManager()->HintHideNameSelf ==TRUE)
			{
				DisplayName = TEXT("");
			}
			else if( pMainPlayer && pMainPlayer->ServerID!=pl->dwHostID &&	HintHideNameOther ) // MMOUI::GetUIManager()->HintHideNameOther ==TRUE )
			{
				DisplayName = TEXT("");
			}
			else{
				DisplayName = DisplayName + TEXT("(")+pl->MasterName+TEXT(")");
			}
		}

		INT Reputation = MMOFacade::getSingleton().GetPlayerReputation( Player , pl->ForceID );
		BOOL isAttackeble = MMOFacade::getSingleton().IsAttackable(pl);

		HintConfig *pHint = MMOFacade::getSingleton().GetHintInfo(EActorClassType(pl->GetActorType()),ENPC_QUALITY(pl->Quality) );
		if(pHint){
			if(pHint->NameVisible==0 && IsQuestTrace==FALSE)
			{
				DisplayName = TEXT("");
			}
			if(pHint->StatusVisible==0 && IsQuestTrace==FALSE)
			{
				IdentityName = TEXT("");
			}
		}

		if (pl->IsMonster() && MMOUI::GetUIManager()->HintHideNameMonster)
		{
			DisplayName = TEXT("");
		}
			

		FString FrameExName = TEXT("");
		if(pl && pl->ActorAttributes )
		{
			if(pl->ActorAttributes->FrameEx1.Len()>0){
				FrameExName = pl->ActorAttributes->FrameEx1;
			}
			else if(pl->ActorAttributes->FrameEx2.Len()>0)
			{
				FrameExName = pl->ActorAttributes->FrameEx2;
			}
		}

		FString NameColor = TEXT("");
		if(pl && pl->ActorAttributes )
		{
			if(pl->ActorAttributes->ColorEx2.Len()>0){
				NameColor=pl->ActorAttributes->ColorEx2;
			}
			else if(pl->ActorAttributes->ColorEx1.Len()>0)
			{
				NameColor=pl->ActorAttributes->ColorEx1;
			}
		}

		INT nShowHpNumber = 2;
		if(pl->ShowHpNumber )
		{
			nShowHpNumber = 1;
		}

		INT size = FString::Printf(TEXT("img://")).Len();
		if(pl->GetActorType()==EACT_Toy)
		{
			eventSetHpInfo(  HeadName, 100, 100, 1,  0);
			eventSetNpcTxt( HeadName, 1  , isAttackeble, DisplayName, IdentityName,FrameExName,NameColor );
			eventSetIconInfo(HeadName,TEXT(""),TEXT(""));
		}
		else if(pl->GetActorType()==EACT_Pet)
		{
			UpdateHp_NPC(HeadName, pl);
			//INT HP		= pl->ActorAttributes->GetCurHp();
			//INT MaxHP	= pl->ActorAttributes->GetMaxHp();
			////warnf( NAME_Error, TEXT("=========eventSetHpInfo: %s, HP=%d:%d"), HeadName, HP  , MaxHP  );

			//if(pMainPlayer && pl->dwHostID==pMainPlayer->ServerID && HP > 0 && MaxHP > 0 )
			//{
			//	if(MMOUI::GetUIManager()->BloodDrawSelf){
			//		if(isAttackeble){
			//			eventSetHpInfo(  HeadName, HP  , MaxHP , 2,  1 );
			//		}
			//		else
			//		{
			//			eventSetHpInfo(  HeadName, HP  , MaxHP , 1,  1 );
			//		}
			//	}
			//	else
			//	{
			//		eventSetHpInfo(  HeadName, HP  , MaxHP , 1,  0 );
			//	}
			//}
			//else if(MMOUI::GetUIManager()->BloodDrawPlayer && HP > 0 && MaxHP > 0)
			//{
			//	if(isAttackeble){
			//		eventSetHpInfo(  HeadName, HP  , MaxHP , 2,  nShowHpNumber );
			//	}
			//	else
			//	{
			//		eventSetHpInfo(  HeadName, HP  , MaxHP , 1,  nShowHpNumber );
			//	}
			//}
			//else
			//{
			//	eventSetHpInfo(  HeadName, HP  , MaxHP , 2,  0 );
			//}
			FrameExName = TEXT("");
			eventSetNpcTxt( HeadName, 4  , isAttackeble, DisplayName, IdentityName,FrameExName,NameColor );
			eventSetIconInfo(HeadName,TEXT(""),TEXT(""));
		}
		else if(pl->GetActorType()==EACT_NPC)
		{
			eventSetHpInfo(  HeadName, 100, 100, 1,  0);
			eventSetNpcTxt( HeadName, 2  , isAttackeble, DisplayName, IdentityName,FrameExName,NameColor );

			FString npcIcon = TEXT("");
			if (pl->IdentityIcon)
			{
				npcIcon = FString(TEXT("img://")) + pl->IdentityIcon->GetPathName();
			}

			if(iterHint!=mapHint.end() && iterHint->second.IconName.Len()>size)
			{
				debugf(NAME_Error, TEXT("#########################eventSetIconInfo:%s  %s"), *DisplayName, *(iterHint->second.IconName) );
					
				eventSetIconInfo(HeadName,iterHint->second.IconName, npcIcon);
			}
			else{
				eventSetIconInfo(HeadName,TEXT(""),npcIcon);
			}
		}
		else if(pl->GetActorType()==EACT_Function_NPC)
		{
			eventSetHpInfo(  HeadName, 100, 100, 1,  0);
			eventSetNpcTxt( HeadName, 3  ,isAttackeble, DisplayName, IdentityName,FrameExName,NameColor );

			FString npcIcon = TEXT("");
			if (pl->IdentityIcon)
			{
				npcIcon = FString(TEXT("img://")) + pl->IdentityIcon->GetPathName();
			}

			if(iterHint!=mapHint.end() && iterHint->second.IconName.Len()>size)
			{
				FString npcIcon = TEXT("");
				if (pl->IdentityIcon)
				{
					npcIcon = FString(TEXT("img://")) + pl->IdentityIcon->GetPathName();
				}
				eventSetIconInfo(HeadName,iterHint->second.IconName,npcIcon);
			}
			else{
				eventSetIconInfo(HeadName,TEXT(""),npcIcon);
			}
		}
		else if(pl->GetActorType()==EACT_Monster || pl->GetActorType()==EACT_Boss)
		{
			UpdateHp_NPC(HeadName, pl);
			//INT HP		= pl->ActorAttributes->GetCurHp();
			//INT MaxHP	= pl->ActorAttributes->GetMaxHp();
			////warnf( NAME_Error, TEXT("=========eventSetHpInfo: %s, HP=%d:%d"), HeadName, HP  , MaxHP  );
			//if(MMOUI::GetUIManager()->BloodDrawMonster && HP >= 0 && MaxHP > 0){
			//	eventSetHpInfo(  HeadName, HP  , MaxHP , 2,  nShowHpNumber );
			//}
			//else
			//{
			//	eventSetHpInfo(  HeadName, HP  , MaxHP , 2,  0 );
			//}

			FString icon = TEXT("");
			if (IsQuestTrace)
			{
				MMOQuestManager::getSingleton().GetQuestStateIcon(E_qt_Trunk, EQHUDS_Acceptable, TRUE, icon);
			}
				
			eventSetMonsterTxt( HeadName, (pl->Quality-E_nq_Invalid), DisplayName, IdentityName, icon ,FrameExName,NameColor);
			eventSetIconInfo(HeadName,TEXT(""),TEXT(""));
		}
		else
		{
			eventSetHpInfo(  HeadName, 100, 100, 1,  0);
			eventSetNpcTxt( HeadName, 1  , isAttackeble, DisplayName, IdentityName,FrameExName,NameColor );
			eventSetIconInfo(HeadName,TEXT(""),TEXT(""));
		}
		eventUpdateDisplay(HeadName);
		HeadName->SetInt(TEXT("serverID"),pl->ServerID);
	}
	
}

void UMMOUISceneHint::ChangeScene()
{

}

void UMMOUISceneHint::AddActorTalkBubble( INT nServerID, const FString &strWords )
{
    if (!MMOUI::GetUIManager()->DrawChatBubble)
        return;

	INT2FActorHintMap::iterator pos = mapHint.find(nServerID);
	if (pos != mapHint.end())
	{
		if(pos->second.PlayerHeadText && pos->second.IsVisible)
		{
			eventSetBubbleText(pos->second.PlayerHeadText, strWords );
		}
	}
}

void UMMOUISceneHint::CloseActorTalkBubble(INT nServerId)
{
	if (!MMOUI::GetUIManager()->DrawChatBubble)
		return;

	INT2FActorHintMap::iterator pos = mapHint.find(nServerId);
	if (pos != mapHint.end())
	{
		if(pos->second.PlayerHeadText && pos->second.IsVisible)
		{
			eventCloseBubbleText(pos->second.PlayerHeadText);
		}
	}
}

void UMMOUISceneHint::CloseAllTalkBubble()
{
    STLInt2IntMap* PlayerMap = MMOFacade::getSingleton().GetPlayerMap();
    STLInt2IntMap::iterator PlayerMapItor ;
    for (PlayerMapItor = PlayerMap->begin();  PlayerMapItor != PlayerMap->end(); PlayerMapItor++)
    {
		AMMOPlayer* pl = Cast<AMMOPlayer>(MMOFacade::getSingleton().GetCharacter(PlayerMapItor->first) );
		if( !pl){
			continue;
		}
        //AMMOPlayer* pl = Cast<AMMOPlayer>(UObject::GetIndexedObject(PlayerMapItor->second));
        //if (!pl)
        //    continue;
        INT2FActorHintMap::iterator itr = mapHint.find(pl->ServerID);
        if (itr != mapHint.end())
        {
            if(itr->second.PlayerHeadText)
            {
                eventSetBubbleText(itr->second.PlayerHeadText, _T("") );
                itr->second.ForceUpdate = TRUE;
            }
        }
    }
}

void UMMOUISceneHint::TestNpcTalk( const FString &strWords )
{
	FString showWords = strWords;
	if( showWords.IsEmpty() ){
		showWords = TEXT("Hello! I am just a test bubble.");
	}

	//AMMOActor* targetActor =    Cast<AMMOActor>(MMOTargetManager::getSingleton().GetDesireActor());//    .GetCurrentTarget();
	AMMOActor* targetActor =    FSMIO::getSingleton().Target.GetDesireMMOActor();

	AMMOPlayer* pl	= Cast<AMMOPlayer>(targetActor);
	ANPC*		npc = Cast<ANPC>(targetActor);

	if(pl)			AddActorTalkBubble( pl->ServerID, showWords );
	else if(npc)	AddActorTalkBubble( npc->ServerID, showWords );
}

void UMMOUISceneHint::SetIconName( class ANPC* npc, const FString &strName )
{
	if(npc==NULL)return;

	npc->QuestIconName = strName;
	INT2FActorHintMap::iterator pos = mapHint.find(npc->ServerID);
	//if(pos==mapHint.end())
 //       CreateNpcHint(npc);


	//pos = mapHint.find(npc->ServerID);
	if (pos != mapHint.end())
	{

		UGFxObject* HeadName = pos->second.PlayerHeadText;

		HintConfig *pHint = MMOFacade::getSingleton().GetHintInfo(EActorClassType(npc->GetActorType()),ENPC_QUALITY(npc->Quality) );
		if( pHint&&pHint->IconVisible>0 )
		{
			pos->second.IconName = npc->QuestIconName;
		}
		else
		{
			pos->second.IconName = TEXT("");
		}

		pos->second.ForceUpdate = TRUE;

	}

}

void UMMOUISceneHint::UpdatePlayerTitle(UBOOL main_actor)
{
	CTRL_TICK("HUD_MMOUISceneHint::UpdatePlayerTitle");

    INT2FActorHintMap::iterator itr = mapHint.end();
    if (main_actor)
    {
        AMMOPlayer* self = MMOFacade::getSingleton().GetMainPlayer();
        if (self)
            itr = mapHint.find(self->ServerID);
        if (itr != mapHint.end())
            itr->second.ForceUpdate = TRUE;
    }
    else
    {
        STLInt2IntMap* PlayerMap = MMOFacade::getSingleton().GetPlayerMap();
        STLInt2IntMap::iterator PlayerMapItor ;
        for (PlayerMapItor = PlayerMap->begin();  PlayerMapItor != PlayerMap->end(); PlayerMapItor++)
        {
			AMMOPlayer* pl = Cast<AMMOPlayer>(MMOFacade::getSingleton().GetCharacter(PlayerMapItor->first) );
			if( !pl){
				continue;
			}
            //AMMOPlayer* pl = Cast<AMMOPlayer>(UObject::GetIndexedObject(PlayerMapItor->second));
            //if (!pl)
            //    continue;
            itr = mapHint.find(pl->ServerID);
            if (itr != mapHint.end())
                itr->second.ForceUpdate = TRUE;
        }
    }
}

void UMMOUISceneHint::ASRightClickTargetInfo(INT x,INT y,INT serverID)
{
	AActor* Target = MMOFacade::getSingleton().GetActor(serverID);
	AMMOActor* targetActor =	 Cast<AMMOActor>(Target);
	//MMOTargetManager::getSingleton().SetSelectMMOActor(targetActor);
	FSMIO::getSingleton().Target.SetSelectMMOActor(targetActor);
	if( targetActor )
	{
		MMOUI::GetUIManager()->delegateOpenRClickMenu(
			ERCMType_TargetInfo , x , y, targetActor->ServerID, targetActor->GetActorName(), 0 );
	}
}

void UMMOUISceneHint::ASRightClickNpcInfo(INT x,INT y,INT serverID)
{
	AActor* actor = MMOFacade::getSingleton().GetActor(serverID);
	if (!actor)
	{
		return;
	}

	ANPC* npc =	 Cast<ANPC>(actor);
	if (!npc)
	{
		return;
	}

	FSMIO::getSingleton().Target.SetSelectMMOActor(npc);
	FSMIO::getSingleton().Target.SetDesireActor(npc);
	npc->DoInteract();
}

void UMMOUISceneHint::UpdateHintPos(float DeltaTime)
{

	UpdateHints_Pos(DeltaTime);

}

void UMMOUISceneHint::UpdateHintText(class AMMOActor* pl ,BYTE type)
{
	if(pl==NULL)return;
	RefleshHints(pl->ServerID,type);
}

void UMMOUISceneHint::UpdateHintTextByID(int ServerID ,BYTE type)
{
	RefleshHints(ServerID,type);
}

extern FLOAT GAverageFPS;
FLOAT g_lastCheckTime = 0.0f;
FLOAT s_averageFPS = 0;


void UMMOUISceneHint::BeforeViewUpdate(float DeltaTime)
{
	UpdateHintPos(DeltaTime);

	if(MMO::GetCurrentGameState() != EGameState_InGame)
	{
		return;
	}
	/*
	Performance tuning hint
	*/

	if( GSystemSettings.bAutoAdjustPerformance )
	{
		if(g_lastCheckTime > GSystemSettings.AutoPerformanceCheckTime)
		{
			// Do not check hints during cutscenes
			if(!MMOFacade::getSingleton().GetMainController()->IsPlayingMatinee())
			{
				//FLOAT fInGameStateFps = MMOFacade::getSingleton().GetGameSateFps();				
				s_averageFPS /= g_lastCheckTime;
				//debugf(TEXT("=== AvgFPS:%.2f"), s_averageFPS);

				if(s_averageFPS < GSystemSettings.LowerCheckFPS)
				{		
					// Only hint when the new tuning level is lower than the current level
					INT newLv = GetFPSLevel(GAverageFPS);
					if(GSystemSettings.bHintBeforeAutoAdjust && (newLv < GSystemSettings.AutoFPSLevel))
					{
						MMOMsgBoxManager::getSingleton().AutoAdjustSystemSettingMessageBox();
					}
					else
					{
						AutoLowScaleLevel(true);
					}
				}
				else if(s_averageFPS > (GSystemSettings.LowerCheckFPS + 5))
				{
					AutoLowScaleLevel(false);
				}
			}

			g_lastCheckTime = 0.0f;
			s_averageFPS = 0;

			MMOFacade::getSingleton().ResetGameSateFps();
		}
		else
		{
			g_lastCheckTime += DeltaTime;
			++s_averageFPS;

			//FLOAT fInGameStateFps = MMOFacade::getSingleton().GetGameSateFps();
			//s_averageFPS += fInGameStateFps * DeltaTime;
		}
	}
	else
	{
			g_lastCheckTime = 0.0f;
	}
}

void UMMOUISceneHint::HideNearPlayer(FLOAT DeltaTime)
{
	CTRL_TICK("HUD_MMOUISceneHint::HideNearPlayer");

	static INT nNearPlayerHideHint = -1;
	if(nNearPlayerHideHint < 0)
		GConfig->GetInt( TEXT("SettingHint") , TEXT("NearPlayerHideHint") , nNearPlayerHideHint , GDefaultGameIni );

	if(nNearPlayerHideHint > 0)
	{
		UMMOKeyManager* ukm = UMMOKeyManager::getSingleton();

		if(ukm == NULL || !( MMOUI::GetUIManager()->HintHideNearPlayer))
			return;

		static FLOAT fTimeDetailPass = 0.0f;
		static INT nNearPlayerHintPerSeconds = 0;
		static INT nNearPlayerFps = 0;
		static INT nNearPlayerCount = 0;
		if(nNearPlayerHintPerSeconds == 0)
			GConfig->GetInt( TEXT("SettingHint") , TEXT("NearPlayerHintPerSeconds") , nNearPlayerHintPerSeconds , GDefaultGameIni );
		if(nNearPlayerFps == 0)
			GConfig->GetInt( TEXT("SettingHint") , TEXT("NearPlayerFps") , nNearPlayerFps , GDefaultGameIni );
		if(nNearPlayerCount == 0)
			GConfig->GetInt( TEXT("SettingHint") , TEXT("NearPlayerCount") , nNearPlayerCount , GDefaultGameIni );

		fTimeDetailPass += DeltaTime;
		if (fTimeDetailPass >= nNearPlayerHintPerSeconds)
		{
			fTimeDetailPass = 0.0f;

			FLOAT fInGameStateFps = MMOFacade::getSingleton().GetGameSateFps();
			if(fInGameStateFps < nNearPlayerFps)
			{
				if(nNearPlayerCount <= MMOFacade::getSingleton().GetPlayerCount())
				{
					FString strKey;
					UMMOKeyManager::KKeyworkd* pKKey = ukm->FindKeyworld(UMMOKeyManager::KF_ShowHidePlayer, FALSE );
					if(pKKey)
						ukm->KeyToString(pKKey,strKey);

					if(!strKey.IsEmpty())
					{
						FString text = FString::Printf( TEXT("Press %s to hide nearby players") , *(strKey) );
						MMOChatManager::getSingleton().AddSystemMessage( text );
						MMOUI::GetUIManager()->delegateAddScreenMessage( text ,MsgInfoType_Normal);
					}
				}
			}
		}
	}
}

UBOOL UMMOUISceneHint::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	if(	strHoverControllerName==TEXT("chatbubble_mc")||
		strHoverControllerName==TEXT("tf_TalkContent") ||
		strHoverControllerName==TEXT("bg_mc") ||
		strHoverControllerName==TEXT("Title_BG") ||
		strHoverControllerName==TEXT("TitleLoader_mc") ||


		strHoverControllerName==TEXT("mc_enemy") ||
		strHoverControllerName==TEXT("HP_mc") ||
		strHoverControllerName==TEXT("progressBar_txt") ||
		strHoverControllerName==TEXT("progressBar_mc") ||
//		strHoverControllerName==TEXT("stall_mc") ||
//		strHoverControllerName==TEXT("tf_stallName") ||

		strHoverControllerName==TEXT("tF_name") ||
		strHoverControllerName==TEXT("mc_Name") ||
		strHoverControllerName==TEXT("mc_Guild") ||

		strHoverControllerName==TEXT("mc_Monster_Status") ||
		strHoverControllerName==TEXT("mc_Npc_Status") ||

		strHoverControllerName==TEXT("mc_shabake")  ||
		
		strHoverControllerName==TEXT("mc_shabake")  ||
		strHoverControllerName==TEXT("mc_guildleader") ) 

		
	{
		return TRUE;
	}
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace );
}

void UMMOUISceneHint::ASClickStallInfo(const FString& playerName, const FString& stallName)
{
	if (stallName == TEXT(""))
	{
		return;
	}

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	if (playerName == mainPlayer->GetActorName() && stallName == mainPlayer->StallName)
	{// If it is self
		if (mainPlayer->MoveType >= EMT_PrepareStall)
		{
			UMMOUIBag* bag = (UMMOUIBag*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Bag);
			if (bag)
			{
				bag->Show(TRUE);
			}

			UMMOUINewStallSettingPanel* panel = (UMMOUINewStallSettingPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Stall_StallSetting);
			if (panel)
			{
				panel->Show(TRUE);
			}
			return;
		}
	}

	// Others
	STLInt2IntMap* playerMap = MMOFacade::getSingleton().GetPlayerMap();
	STLInt2IntMap::iterator playerMapItor;
	for (playerMapItor = playerMap->begin(); playerMapItor != playerMap->end(); playerMapItor ++)
	{
		AMMOPlayer* player = Cast<AMMOPlayer>(MMOFacade::getSingleton().GetCharacter(playerMapItor->first));
		if (player && player->GetActorName() == playerName && player->StallName == stallName && player->MoveType == EMT_InStall)
		{
			CMessageHandler::GetInstance().Send_ViewStallItem(player->ServerID);
			return;
		}
	}
}



/************************************************************************/
/*     Take over name UI creation			                                        */
/************************************************************************/
class UGFxObject* UMMOUISceneHint::CreateHintFromPool()
{
	if(hintPool.empty())
	{
		//If pool 2 is empty, recreate
		static int autoGenIndex = 0;
		UGFxObject* HeadName = this->AttachMovie( TEXT("PlayerNameHintText") , FString::Printf( TEXT("PlayerNameHintText%d"), ++autoGenIndex),1  );
		HeadName->AddToRoot();
		return HeadName; 
	}
	else
	{
		//If the pool has spares, use those
		GFxObjectSet::iterator it = hintPool.begin();
		UGFxObject* fx = *it;
		hintPool.erase(it);
		fx->SetVisible(TRUE);
		return fx;
	}
	return NULL;
}


/************************************************************************/
/*     Take over name UI destruction			                                        */
/************************************************************************/
void UMMOUISceneHint::RecycleHintToPool(class UGFxObject* HeadName)
{
	//Hide directly and add to the spare pool
	HeadName->SetVisible(FALSE);
	hintPool.insert(HeadName);
}

void UMMOUISceneHint::InitHintPool()
{

}


/************************************************************************/
/*     When SceneHint is destroyed, destroy all UI in the pools to free memory                        */
/************************************************************************/
void UMMOUISceneHint::UnintHintPool()
{
	for (GFxObjectSet::iterator it = hintPool.begin(); it!=hintPool.end();)
	{
		UGFxObject* fx = *it;
		eventRemoveChild(fx);
		fx->RemoveFromRoot();

		it = hintPool.erase(it);
	}

}