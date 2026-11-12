#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMapDataManager.h"
#include "UITextParser.h"
//
IMPLEMENT_CLASS(UMMOUITeleportNet)

void UMMOUITeleportNet::Open( BYTE regionID, INT nodeID )
{
	//FTeleportNodeObj *pNode = MMOMapDataManager::getSingleton().GetTeleportNetNodes( (EWorldRegion)regionID, nodeID );
	MMOMapDataManager::getSingleton().SetCurOpenedNodeID( nodeID );

	_CurTab = regionID;
	eventUpdateTab( regionID );
	//show it
	Show( TRUE );
}
//
void UMMOUITeleportNet::UpdateTeleportNetList( BYTE regionID )
{
	MMOMapDataManager *pMapDataManager = &MMOMapDataManager::getSingleton();
	UGlobalMapInfo *pGlobalMapInfo = UGlobalMapInfo::GetSingleton();
	//sort
	//pMapDataManager->SortedByMap();
	if( regionID < 0 || regionID > EWR_MAX ){
		return;
	}
	_CurTab = regionID;

	FTeleportNodeObj *pCurShowNode = pMapDataManager->GetLastOpendNode();
	INT curShowNodeID = ( pCurShowNode != NULL ? pCurShowNode->ServerID : -1 );

	INT teleportNodeCnt;
	TArray<FTeleportNetNodeData*> nodesListInRegion;
	teleportNodeCnt = pMapDataManager->GetTeleportNetNodesIn( (EWorldRegion)_CurTab, 0, TRUE, &nodesListInRegion );

	//FTeleportNodeObj *pNode;
	FTeleportNetNodeData *pNode;
	FTeleportNodeObj *pNodeInitData;
	INT lastSceneID = -1;
	INT itemObjIdx = 0;
	INT curShowNodeIdx = 0;
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	for ( INT i = 0; i < teleportNodeCnt; ++i ){
		pNode = nodesListInRegion(i);
		pNodeInitData = pNode->_NodeTemplate;
		//if( !pNode->IsOpened ){
		//	//do not show not opened nodes
		//	continue;
		//}
		if( !pNode->_isPlayerOpened ){
			//do not show not opened nodes
			continue;
		}

		if( lastSceneID != pNodeInitData->InSceneID ){
			//First-level entry: map name
			lastSceneID = pNodeInitData->InSceneID;

			UGFxObject* itemMapObj = GFXPlayer->CreateObject( TEXT("UI_TeleportNet.TeleportNetAS3.TeleportMapData") );

			FString mapDisplayName;
			mapDisplayName = pGlobalMapInfo->FindMapInfo( lastSceneID ).MapDisplayName;
			
			itemMapObj->SetString( TEXT("label"), mapDisplayName );
			itemMapObj->SetInt( TEXT("index"), itemObjIdx );
			itemMapObj->SetInt( TEXT("type"), 2 );
			itemMapObj->SetInt( TEXT("deepth"), 0 );
			itemMapObj->SetBool( TEXT("visible"), TRUE );

			itemMapObj->SetString( TEXT("MapName"), mapDisplayName );

			dataProvider->SetElementObject( itemObjIdx, itemMapObj );
			++itemObjIdx;
		}
		//Second-level entry: teleport point info
		if( curShowNodeID == pNodeInitData->ServerID ){
			//current node add title
			UGFxObject* itemCurNodeTitleObj = GFXPlayer->CreateObject( TEXT("ActionScript3.TreeListData") );
			//UGFxObject* itemCurNodeTitleObj = GFXPlayer->CreateObject( TEXT("UI_TeleportNet.TeleportNetAS3.TreeListData") );
			itemCurNodeTitleObj->SetString( TEXT("label"), "" );
			itemCurNodeTitleObj->SetInt( TEXT("index"), itemObjIdx );
			itemCurNodeTitleObj->SetInt( TEXT("type"), 0 );
			itemCurNodeTitleObj->SetInt( TEXT("deepth"), 1 );
			itemCurNodeTitleObj->SetBool( TEXT("visible"), TRUE );

			dataProvider->SetElementObject( itemObjIdx, itemCurNodeTitleObj );
			++itemObjIdx;
		}

		UGFxObject* itemNodeObj = GFXPlayer->CreateObject( TEXT("UI_TeleportNet.TeleportNetAS3.TeleportNodesData") );

		itemNodeObj->SetString( TEXT("label"), pNodeInitData->DisplayName );
		itemNodeObj->SetInt( TEXT("index"), itemObjIdx );
		itemNodeObj->SetInt( TEXT("type"), 0 );
		itemNodeObj->SetInt( TEXT("deepth"), 1 );
		itemNodeObj->SetBool( TEXT("visible"), TRUE );

		if( curShowNodeID == pNodeInitData->ServerID ){
			//need show this
			curShowNodeIdx = itemObjIdx - 1;
			itemNodeObj->SetInt( TEXT("ShowState"), 1 );
		}
		else{
			itemNodeObj->SetInt( TEXT("ShowState"), 0 );
		}
		itemNodeObj->SetInt( TEXT("NodeID"), pNodeInitData->ServerID );
		itemNodeObj->SetString( TEXT("NodeName"), pNodeInitData->DisplayName );
		FString iconName = pNodeInitData->PicResName;
		if( iconName.IsEmpty() ){
			iconName = pGlobalMapInfo->FindMapInfo( lastSceneID ).MapName;
		}
		itemNodeObj->SetString( TEXT("PicName"), pNodeInitData->PicResName );
		itemNodeObj->SetInt( TEXT("CostType"), pNodeInitData->NeedMnyType );
		itemNodeObj->SetInt( TEXT("Cost"), pNodeInitData->NeedMoney );

		//
		FString moneyHtml;
		moneyHtml = UITextParser::HTMLMoneyIconString( pNodeInitData->NeedMnyType, pNodeInitData->NeedMoney );
		itemNodeObj->SetString( TEXT("CostString"), moneyHtml );

		dataProvider->SetElementObject( itemObjIdx, itemNodeObj );
		++itemObjIdx;
	}

	eventFlashTeleportNetList(dataProvider);
	//
	_TeleportNodesListMc->SetInt( TEXT("scrollPosition"), curShowNodeIdx );
	//TArray<struct FASValue> args;

	//FASValue arg(EC_EventParm);
	//arg.I = curShowNodeIdx;
	//arg.Type = AS_Int;
	//args.AddItem(arg);

	//_TeleportNodesListMc->Invoke( TEXT("scrollToIndex"), args );
}

//void UMMOUITeleportNet::ToThisTeleportNode( INT destNodeID )
//{
//	INT scrNodeID = MMOMapDataManager::getSingleton().GetLastOpendNodeID();
//	CMessageHandler::GetInstance().Send_TeleportToNetNodes( (UINT8)scrNodeID, (UINT8)destNodeID );
//	//
//	Show( FALSE );
//}

void UMMOUITeleportNet::Init()
{
	Super::Init();

	//
	TArray<struct FASValue> asFunArgs;
	FASValue asTitleName(EC_EventParm);
	asTitleName.Type = AS_String;
	asTitleName.S = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.TeleportNet.UITitleName") );
	asFunArgs.AddItem( asTitleName );
	this->Invoke( TEXT("SetWindowTitle"), asFunArgs );

	for( INT i = 0 ; i < 4; ++i )
	{
		FString objName = TEXT("tab_mc");
		objName += appItoa(i);

		_TeleportReginTab[i] = GetObject( objName );
		if( _TeleportReginTab[i] )
		{
			FString txtPath = TEXT("MMOGame.TeleportNet.TeleportNetRegin");
			txtPath += appItoa(i);
			//_TeleportReginTab[i]->SetText( MMOUI::ParseLocalizedPropertyPath( txtPath ) );
			_TeleportReginTab[i]->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( txtPath ) );
			//Temporarily hidden
			_TeleportReginTab[i]->SetVisible( FALSE );
		}
	}

	_TeleportNodesListMc = GetObject( TEXT("TeleportNodesList_mc") );
}

void UMMOUITeleportNet::PreOnShow(UBOOL show)
{
	Super::PreOnShow(show);

	if(show)
	{
		UpdateTeleportNetList( _CurTab );
	}
	else
	{
	}
}

//===============================================================================================================
//===============================================================================================================
void UMMOUITeleportNet::ASToThisTeleportNode( INT destNodeID )
{
	FTeleportNodeObj *pCurShowNode = MMOMapDataManager::getSingleton().GetLastOpendNode();
	if( pCurShowNode == NULL ){
		return;
	}

	INT scrNodeID = pCurShowNode->ServerID;
	//
	if( scrNodeID == destNodeID ){
		return;
	}
	//check money
	INT nowMny = MMOFacade::getSingleton().GetMainPlayer()->GetMoney( pCurShowNode->NeedMnyType );
	if( pCurShowNode->NeedMoney > nowMny ){
		//not enough money
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( "MMOGame.TradeP2P.ERROR25"),MsgInfoType_Table,TEXT("MMOGame.TradeP2P.ERROR25") );
		return;
	}
	//
	CMessageHandler::GetInstance().Send_TeleportToNetNodes( (UINT8)scrNodeID, (UINT8)destNodeID );
	//
	Show( FALSE );
}

//test
void UMMOUITeleportNet::TestToTeleportNode( INT srcNodeID, INT destNodeID )
{
	CMessageHandler::GetInstance().Send_TeleportToNetNodes( (UINT8)srcNodeID, (UINT8)destNodeID );
}

//===============================================================================================================
//===============================================================================================================