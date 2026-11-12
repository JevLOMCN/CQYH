#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "TeamManager.h"
#include "MMOGuideManager.h"
#include "MMOMapDataManager.h"

IMPLEMENT_CLASS(UMMOUIFBEnter)

void UMMOUIFBEnter::TickPanel( FLOAT DeltaTime )
{
	if( IsVisible && Player != NULL ){
		INT xDis = Player->Location.X - CurEnterTeleportPos.X;
		INT yDis = Player->Location.Y - CurEnterTeleportPos.Y;
		INT curDisBetween = xDis*xDis + yDis*yDis;
		if( curDisBetween > Distance2ForVisible ){
			Show( FALSE );
		}
	}
}

void UMMOUIFBEnter::PrepareShow( const FString& mapName,INT scrTelepID )
{
	TArray<FTeleportLink*> teleLinkList;
	int LnkCnt = UGlobalMapInfo::GetSingleton()->FindLinkByOrigID( &teleLinkList, mapName, scrTelepID );
	if( LnkCnt <= 0 ){
		return;
	}
	//save date
	TelepOrigID = scrTelepID;
	CurMapName = mapName;
	for( int i = 0; i < 5; ++i ){
		FBBranchTelepLinkID[i] = (i < LnkCnt) ? teleLinkList(i)->srcLinkID : 0;
	}

	if( teleLinkList.Num() > 0 ){
		const FTeleportObject &origTeleport = teleLinkList(0)->From;
		INT dis = origTeleport.Radius - 100;
		Distance2ForVisible = dis * dis;
		CurEnterTeleportPos = origTeleport.Location;
	}

	////Initial FB branch button
	//for( int i = 0; i < 5; ++i ){
	//	//
	//	if( i < LnkCnt ){
	//		btn_FBBranch[i]->SetBool( "enabled", true );
	//		FExportMap targetFBInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoByName( teleLinkList(i)->ToMapName );
	//		btn_FBBranch[i]->GetObject("textField")->SetText( *targetFBInfo.MapDisplayName );
	//	}
	//	else{
	//		btn_FBBranch[i]->SetBool( "enabled", false );
	//		btn_FBBranch[i]->GetObject("textField")->SetText( "" );
	//	}
	//}
	//
	InitFBEnterData();
	//To get Server difficulty
	BranchChange( 0 );
	//
	//UpdateMemberStatus();
}

//
void UMMOUIFBEnter::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if( v ){
		//Remove dungeon difficulty
		//CurDiff = ENUM_SCENE_DIFFICULTY::esdNormal1;
		//MMOMapDataManager::getSingleton().SetCurFBDiff( CurDiff );
		//CurMaxDiff = MMOMapDataManager::getSingleton().GetCurFBMaxDiff();

		//
		UpdateCurFBDiff( CurDiff, CurMaxDiff );

		MMOGameEvent_Guide evtGuide(EGUET_ShowDungeonEnterPanel);
		MMOUI::PostGameEvent(&evtGuide);
	}
	else{

	}
}
//=======================================================================================================================
void UMMOUIFBEnter::BranchChange( INT iBranchIdx )
{
	if( iBranchIdx < 0 || iBranchIdx >= sizeof(FBBranchTelepLinkID) / sizeof(INT) ){
		return;
	}
	CurFBBranchIdx = iBranchIdx;

	////
	//for( int i = 0; i < 5; ++i ){
	//	//btn_FBBranch[i]->SetBool( "enabled", FBBranchTelepLinkID[0] > 0 );
	//	btn_FBBranch[i]->SetBool( "selected", i == CurFBBranchIdx );
	//}

	//Update FB description
	UpdateCurFBInfo( CurMapName, FBBranchTelepLinkID[CurFBBranchIdx] );
	//
	SendFBBranch( FBBranchTelepLinkID[CurFBBranchIdx] & svrTeleportIDMask );
}

//----------------------------------------------------------------------------------------------
void UMMOUIFBEnter::SendFBBranch( INT iTeleportID )
{
	CMessageHandler::GetInstance().Send_Teleport( iTeleportID );
}

//====================================================================================================
void UMMOUIFBEnter::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	switch( evt->GetEventType() )
	{
		//Remove dungeon difficulty
		//case GameEvent_ChangeFB_Difficulty:
		//	{
		//		MMOGameEvent_OneInt *fbDifficultyChangeEvt = (MMOGameEvent_OneInt*)evt;
		//		//MMOMapDataManager::getSingleton().GetCurFBMaxDiff();
		//		UpdateCurFBDiff( fbDifficultyChangeEvt->IntData, CurMaxDiff );
		//	}
		//	break;
		case GameEvent_TeamStateChaged:
			{
				if( IsVisible ){
					//UBOOL isInTeam = TeamManager::getSingleton().HasTeam();
					//UBOOL isTeamLeader = TeamManager::getSingleton().IsTeamLeader();
					//UBOOL bCanSelectFB =  isInTeam ? isTeamLeader : true;
					//if( !bCanSelectFB ){
						Show( FALSE );
					//}
					//else{
					//	BranchChange( 0 );
					//}
				}
			}
			break;
	//case GameEvent_TeamJoin:
	//case GameEvent_TeamAddMember:
	//case GameEvent_TeamLeave:
	//case GameEvent_TeamRemoveMember:
	//	{
	//		if( IsVisible ){
	//			UBOOL bCanSelectFB = TeamManager::getSingleton().HasTeam() ? TeamManager::getSingleton().IsTeamLeader() : true;
	//			if( bCanSelectFB ){
	//				CurDiff = -1;
	//				BranchChange(CurFBBranchIdx);
	//			}
	//			else{
	//				MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_FBEnter_FBEnter, FALSE );
	//			}
	//		}
	//	}
	//	break;
	}
}

//========================================================================================================================
void UMMOUIFBEnter::UpdateCurFBInfo( const FString& mapName, INT telepLinkID )
{
	FTeleportLink *pFBTelepLink = UGlobalMapInfo::GetSingleton()->FindLinkByMapAndID( mapName, telepLinkID );
	if( pFBTelepLink == NULL ){
		return;
	}
	//
	FBToMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoByName( pFBTelepLink->ToMapName );
	const FDungeonInfo *fbInfo = MMOMapDataManager::getSingleton().GetDungeonInfo( FBToMapInfo.ServerID, CurDiff );
	//
	SetUITitleName( FBToMapInfo.MapDisplayName );
	//FB backGround
	FString strImg;
	if( !FBToMapInfo.MapName.IsEmpty() ){
		strImg = FString::Printf( TEXT("img://MMO_UIImg.Ent_%s"), *( FBToMapInfo.MapName ) );
	}
	else{
		strImg = TEXT("img://MMO_UIIcon.icon.blank");
	}
	if( FBBackgroundMc != NULL ){
		FBBackgroundMc->SetString( TEXT("source"), strImg );
	}
	

	////Remove dungeon difficulty
	////UpdateFBAllDiff( FBToMapInfo.ServerID );
	//INT iMaxDiffMask = MMOMapDataManager::getSingleton().GetDungeonInfoDifftyMask( FBToMapInfo.ServerID );
	//for( int i = ENUM_SCENE_DIFFICULTY::esdNormal1; i < ENUM_SCENE_DIFFICULTY::esdTotal; ++i ){
	//	if( iMaxDiffMask & ( 1 << ( i - ENUM_SCENE_DIFFICULTY::esdNormal1 ) ) ){
	//		CurDiff = i;
	//	}
	//}
	////
	//eventCallSetFBDescription( FBToMapInfo.MapDescript );
}

//========================================================================================================================
void UMMOUIFBEnter::UpdateFBAllDiff( INT mapID )
{
	//const FDungeonInfo *fbInfo = MMOMapDataManager::getSingleton().GetDungeonInfo( mapID, E_esd_Hell );
	INT iMaxDiffMask = MMOMapDataManager::getSingleton().GetDungeonInfoDifftyMask( mapID );
	//
	for( int i = 0; i < UCONST_FB_SELECT_DIFF_CNT; ++i ){
		FBDiffBtn[i]->SetBool( TEXT("ShowState"), ( iMaxDiffMask & ( 1 << ( i ) ) ) );
	}
}

//========================================================================================================================
void UMMOUIFBEnter::SetUITitleName( const FString &fbName )
{
	FBNameTf->SetString( TEXT("text"), fbName );
	TArray<struct FASValue> asFunArgs;
	FASValue asTitleName(EC_EventParm);
	asTitleName.Type = AS_String;
	asTitleName.S = fbName;
	asFunArgs.AddItem( asTitleName );
	this->Invoke( TEXT("SetWindowTitle"), asFunArgs );
}

//------------------------------------------------------------------------------------------------------------------------
void UMMOUIFBEnter::UpdateCurFBDiff( INT newDiff, INT newMaxDiff )
{
	//get max difficulty
	if( newMaxDiff >= 0 ){
		CurMaxDiff = newMaxDiff;
		//MMOMapDataManager::getSingleton().SetCurFBMaxDiff( iLevel );
	}
	//get current difficulty
	if( newDiff > 0 ){
		CurDiff = newDiff;
		//MMOMapDataManager::getSingleton().SetCurFBDiff( iLevel );
	}

	/*
	//Remove dungeon difficulty
	INT iMaxDiffMask = MMOMapDataManager::getSingleton().GetDungeonInfoDifftyMask( FBToMapInfo.ServerID );
	//1 person FB
	INT fbOpened1PersonDiffMax = MMOMapDataManager::getSingleton().GetMaxDiffOfType( FBToMapInfo.ServerID, EFBDTyp_OnePerson, CurMaxDiff );
	INT fb1PersonDiffBtnMaxIdx = CvtToUiDiffIdx( esdHeroic1 );
	INT curMaxDiffOfFb1Person = CvtToUiDiffIdx( fbOpened1PersonDiffMax );

	for( INT i = CvtToUiDiffIdx( esdNormal1 ), diffEnum = 0; i <= fb1PersonDiffBtnMaxIdx; ++i, ++diffEnum ){
		UBOOL isShow = ( iMaxDiffMask & ( 1 << diffEnum ) );

		FBDiffBtn[i]->SetBool( TEXT("enabled"), isShow && i <= curMaxDiffOfFb1Person );
		FBDiffBtn[i]->SetBool( TEXT("selected"), false );
		if( FBDiffHintBtn[i] != NULL ){
			FBDiffHintBtn[i]->SetBool( TEXT("visible"), isShow && i > curMaxDiffOfFb1Person );
			FBDiffHintBtn[i]->SetBool( TEXT("enabled"), isShow && i > curMaxDiffOfFb1Person );
		}
	}

	//5 person FB
	INT fbOpened5PersonDiffMax = MMOMapDataManager::getSingleton().GetMaxDiffOfType( FBToMapInfo.ServerID, EFBDTyp_FivePerson, CurMaxDiff );
	INT fb5PersonDiffBtnMaxIdx = CvtToUiDiffIdx( esdHeroic5 );
	INT curMaxDiffOfFb5Person = CvtToUiDiffIdx( fbOpened5PersonDiffMax );

	for( INT i = CvtToUiDiffIdx( esdNormal5 ), diffEnum = esdNormal5 - esdNormal1; i <= fb5PersonDiffBtnMaxIdx; ++i, ++diffEnum ){
		UBOOL isShow = iMaxDiffMask & ( 1 << diffEnum );

		FBDiffBtn[i]->SetBool( TEXT("enabled"), isShow && i <= curMaxDiffOfFb5Person );
		FBDiffBtn[i]->SetBool( TEXT("selected"), false );
		if( FBDiffHintBtn[i] != NULL ){
			FBDiffHintBtn[i]->SetBool( TEXT("visible"), isShow && i > curMaxDiffOfFb5Person );
			FBDiffHintBtn[i]->SetBool( TEXT("enabled"), isShow && i > curMaxDiffOfFb5Person );
		}
	}

	//set difficulty seleted btn
	INT iUiIdx = CvtToUiDiffIdx( CurDiff );
	if( iUiIdx >= 0 && iUiIdx < UCONST_FB_SELECT_DIFF_CNT && ( CurDiff <= fbOpened1PersonDiffMax || CurDiff <= fbOpened5PersonDiffMax ) ){
		FBDiffBtn[iUiIdx]->SetBool( TEXT("selected"), true );
		FString difficultyTitleFormat = TEXT("MMOGame.FBEnter.DifficultyTitleFormat");
		difficultyTitleFormat += appItoa( iUiIdx );
		DiffcultyTitleTf->SetText( MMOUI::ParseLocalizedPropertyPath( difficultyTitleFormat ) );
	}
	*/
	
	const FDungeonInfo *fbInfo = MMOMapDataManager::getSingleton().GetDungeonInfo( FBToMapInfo.ServerID, CurDiff );
	if( fbInfo != NULL ){
		//Player count limit
		INT limitPeople = fbInfo->RoleLimit;
		FString peopleString;
		if( limitPeople == 1 ){
			peopleString = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBEnter.PeopleStringFormat3") );
		}
		else{
			peopleString = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBEnter.PeopleStringFormat2") ), limitPeople );
		}
		PeopleRequiredTf->SetText( peopleString );
		//Vitality
		FString HuoLiStringFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBEnter.HuoLiStringFormat") );
		UINT curHuoLi = Player->GetMoney( E_MONEYTYPE_FATIGUE );
		HuoLiRequiredTf->SetText( FString::Printf( *HuoLiStringFormat, fbInfo->Fatigue, curHuoLi ) );
		//Level limit
		FString levelStringFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBEnter.LevelStringFormat") );
		LevelRequiredTf->SetText( FString::Printf( *levelStringFormat, fbInfo->MinLevel, fbInfo->MaxLevel ) );
		//
		INT curEnterCnt = MMOMapDataManager::getSingleton().GetCurFBEnterFrequence();
		UINT curLeftEnterCnt = (UINT)(fbInfo->MaxEnterCount - curEnterCnt);
		FrequencyRequiredTf->SetText( FString::Printf( *HuoLiStringFormat, curLeftEnterCnt, fbInfo->MaxEnterCount ) );
		//
		OpenTimeRequiredTitleTf;
		OpenTimeRequiredTf;
	}
	else{
		PeopleRequiredTf->SetText( TEXT("No Data") );
		//Vitality
		HuoLiRequiredTf->SetText( TEXT("No Data") );
		//Level limit
		LevelRequiredTf->SetText( TEXT("No Data") );
		//
		FrequencyRequiredTf->SetText( TEXT("No Data") );
		//
		OpenTimeRequiredTf->SetText( TEXT("No Data") );
	}

	//
	CheckCanEnter();
}
//------------------------------------------------------------------------------------------------------------------------
void UMMOUIFBEnter::MapDiffChange( INT iDiffIdx )
{
	CurDiff = CvtToServerDiff( iDiffIdx );
	SendMapDiffChange( CurDiff );

	MMOGameEvent_Guide evtGuide(EGUET_ChooseDungeonLvPanel);
	MMOUI::PostGameEvent(&evtGuide);
}

//
void UMMOUIFBEnter::CheckCanEnter()
{
	//bool isDiffOK = FALSE;
	//if( CurDiff >= esdNormal1 && CurDiff <= esdHeroic1 ){
	//	//1 person FB
	//	INT fbOpened1PersonDiffMax = MMOMapDataManager::getSingleton().GetMaxDiffOfType( FBToMapInfo.ServerID, EFBDTyp_OnePerson, CurMaxDiff );
	//	isDiffOK = ( CurDiff <= fbOpened1PersonDiffMax );
	//}
	//else if( CurDiff >= esdNormal5 && CurDiff <= esdHeroic5 ){
	//	//5 person FB
	//	INT fbOpened5PersonDiffMax = MMOMapDataManager::getSingleton().GetMaxDiffOfType( FBToMapInfo.ServerID, EFBDTyp_FivePerson, CurMaxDiff );
	//	isDiffOK = ( CurDiff <= fbOpened5PersonDiffMax );
	//}

	////
	//EnterFBBtn->SetBool( TEXT("enabled"), isDiffOK );
	EnterFBBtn->SetBool( TEXT("enabled"), TRUE );
}

//
INT UMMOUIFBEnter::CvtToServerDiff( INT i )
{
	//INT rst;

	//rst = i + ENUM_SCENE_DIFFICULTY::esdNormal1;
	//return rst;
	return 0;
}
INT UMMOUIFBEnter::CvtToUiDiffIdx( INT i )
{
	//INT rst;

	//rst = i - ENUM_SCENE_DIFFICULTY::esdNormal1;
	//return rst;
	return 0;
}

//=================================================================================================
void UMMOUIFBEnter::SendMapDiffChange(INT iDifficulty)
{
	CMessageHandler::GetInstance().Send_SelMapDifficulty(iDifficulty);
}

//=================================================================================================
void UMMOUIFBEnter::EnterMap()
{
	SendEnterMap( CurDiff );
	//close it
	Show( FALSE );

	//
	MMOGameEvent_Guide evtGuide(EGUET_EnterDungeon);
	MMOUI::PostGameEvent(&evtGuide);
}

//------------------------------------------------------------------------
void UMMOUIFBEnter::SendEnterMap(INT iDifficulty)
{
	CMessageHandler::GetInstance().Send_EnterDungeon(iDifficulty);
}

//=================================================================================================
void UMMOUIFBEnter::Init()
{
	Super::Init();
	if( GFXPlayer ){
		GFXPlayer->AddBeforeViewUpdateDelegate( FScriptDelegate(this, TEXT("BeforeViewUpdate")) );
	}

	//
	InitMc();
	//
	InitFBEnterData();

	//
	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	//evts.AddItem(GameEvent_ChangeFB_Difficulty);
	evts.AddItem(GameEvent_TeamStateChaged);
	//evts.AddItem(GameEvent_TeamAddMember);
	//evts.AddItem(GameEvent_TeamLeave);
	//evts.AddItem(GameEvent_TeamRemoveMember);
	EventAdapter = new TGameEventAdapter<UMMOUIFBEnter>(this, evts);
}

void UMMOUIFBEnter::UnInit()
{
	Super::UnInit();
	if( GFXPlayer ){
		GFXPlayer->RemoveBeforeViewUpdateDelegate( FScriptDelegate(EC_EventParm) );
	}
}

//=======================================================================================================================
void UMMOUIFBEnter::InitMc()
{
	FBBackgroundMc = GetObject( TEXT("FBBackgroundMc") );
	EnterFBBtn = GetObject( TEXT("EnterFBBtn") );
	//
	DiffcultyTitleTf = GetObject( TEXT("DiffcultyTitleTf") );
	LevelRequiredTf = GetObject( TEXT("LevelRequiredTf") );
	FrequencyRequiredTf = GetObject( TEXT("FrequencyRequiredTf") );
	FrequencyRequiredTitleTf = GetObject( TEXT("FrequencyRequiredTitleTf") );

	PeopleRequiredTf = GetObject( TEXT("PeopleRequiredTf") );
	HuoLiRequiredTf = GetObject( TEXT("HuoLiRequiredTf") );

	OpenTimeRequiredTf = GetObject( TEXT("OpenTimeRequiredTf") );
	if( OpenTimeRequiredTf != NULL ){
		OpenTimeRequiredTf->SetVisible( FALSE );
	}
	OpenTimeRequiredTitleTf = GetObject( TEXT("OpenTimeRequiredTitleTf") );
	if( OpenTimeRequiredTitleTf != NULL ){
		OpenTimeRequiredTitleTf->SetVisible( FALSE );
	}
	//
	FBNameTf = GetObject( TEXT("FBNameTf") );
	FBDescribleTf = GetObject( TEXT("FBVerticalDescribleMc") );

	//
	for( INT i = 0 ;i < UCONST_FB_SELECT_DIFF_CNT ; ++i ){
		FString objName = TEXT("FBDiffBtn");
		objName += appItoa(i);
		FBDiffBtn[i] = GetObject( objName );
		//if( FBDiffBtn[i] != NULL ){
		//	FBDiffBtn[i]->SetVisible( FALSE );
		//}
		//
		objName = TEXT("FBDiffMc");
		objName += appItoa(i);
		FBDiffMc[i] = GetObject( objName );
		//if( FBDiffMc[i] != NULL ){
		//	FBDiffMc[i]->SetVisible( FALSE );
		//}
		//When FBDiffBtn is disabled and grayed out, FBDiffHintBtn provides the hover hint function
		objName = TEXT("FBDiffHintBtn");
		objName += appItoa(i);
		FBDiffHintBtn[i] = GetObject( objName );
		//if( FBDiffHintBtn[i] != NULL ){
		//	FBDiffHintBtn[i]->SetVisible( FALSE );
		//}
	}
}

//---------------------------------------------------------------------------------------------
void UMMOUIFBEnter::InitFBEnterData()
{
	//CurDiff = ENUM_SCENE_DIFFICULTY::esdNormal1;
	//CurFBBranchIdx = 0;
	//CurMaxDiff = ( 1 << ENUM_SCENE_DIFFICULTY::esdNormal1 ) | ( 1 << ENUM_SCENE_DIFFICULTY::esdHeroic1 );
	//CurMaxDiff >>= ENUM_SCENE_DIFFICULTY::esdStatic;
}

//
////=======================================================================================================================
//void UMMOUIFBEnter::UpdateTeamMate( INT UIPos, INT Pos )
//{
//	if( IsVisible ){
//		UpdateMemberStatus();
//	}
//}
////=======================================================================================================================
//void UMMOUIFBEnter::UpdateMemberStatus()
//{
//	bool bAllPrepared = true;
//	INT iMemCnt = TeamManager::getSingleton().GetTeamMemberCount();
//	INT showMemberIdx = 0;
//	AMMOPlayer *pMe = MMOFacade::getSingleton().GetMainPlayer();
//	if( pMe == NULL ){
//		return;
//	}
//
//	for( int i = 0; i < iMemCnt && showMemberIdx < UCONST_FB_MEMBER_CNT; ++i )
//	{
//		MMOTeamMember* mem = TeamManager::getSingleton().GetTeamMember(i);
//		if( mem && mem->ActorServerId != pMe->ServerID )
//		{
//			mc_FBMember[showMemberIdx]->SetVisible( true );
//			mc_FBMember[showMemberIdx]->GetObject( "tf_name" )->SetText( mem->ActorName );
//			//
//			INT iPortrait = mem->ClassId;
//			mc_FBMember[showMemberIdx]->GetObject( "mc_portriat" )->GotoAndStopI( iPortrait );
//
//			++showMemberIdx;
//		}
//		
//	}
//	for( int i = showMemberIdx; i < UCONST_FB_MEMBER_CNT; ++i ){
//		mc_FBMember[i]->SetVisible( false );
//	}
//	//
//	//btn_Enter->SetBool( "enabled",  bAllPrepared );
//}

//===============================================================================================================
//===============================================================================================================
void UMMOUIFBEnter::BeforeViewUpdate(FLOAT DeltaTime)
{
	if( IsVisible ){
		TickPanel(DeltaTime);
	}
}
//===============================================================================================================
//===============================================================================================================