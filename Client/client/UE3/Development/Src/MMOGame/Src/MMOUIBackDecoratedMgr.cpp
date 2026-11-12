#include "stdafx.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOUIBackDecoratedMgr.h"
#include "MMOFacade.h"
#include "tabfile.h"

//=================================================================================================================================
BackDecoratedMakeUpCfg::BackDecoratedMakeUpCfg():
_MakedItemID(-1),
_ConsumptionItemID(-1),
_ConsumptionItemCnt(0),
_MakeUpMnyCosts(0)
{
}
//=================================================================================================================================
BackDecoratedUpgradeCfg::BackDecoratedUpgradeCfg():
_ToUpgradeItemID(-1),
_ToUpgradeItemLv(0),
_ConsumptionItemID(-1),
_ConsumptionItemCnt(0),
_UpgradeMnyCosts(0),
_UpgradedItemID(-1)
{
}

//=================================================================================================================================
MMOUIBackDecoratedMgr::MMOUIBackDecoratedMgr()
{
#if 0
	INT testID = 114003;
	BackDecoratedMakeUpCfg *data = AddBackDecoratedMakeUpCfg( testID );
	if( data != NULL ){
		testID = 99950001;
		data->_MakedItemID = testID;
		testID = 114003;
		data->_ConsumptionItemID = testID;
		testID = 2;
		data->_ConsumptionItemCnt = testID;
		testID = 3000;
		data->_MakeUpMnyCosts = testID;
	}

	//
	testID = 99950001;
	BackDecoratedUpgradeCfg *upUpgradeData = AddBackUpgradeCfg( testID, 0 );
	if( upUpgradeData != NULL ){
		upUpgradeData->_ToUpgradeItemID = testID = 99950001;
		testID = 1;
		upUpgradeData->_ToUpgradeItemLv = testID;
		testID = 114003;
		upUpgradeData->_ConsumptionItemID = testID;
		testID = 5;
		upUpgradeData->_ConsumptionItemCnt = testID;
		testID = 99;
		upUpgradeData->_SuccessRate = testID;
		testID = 5000;
		upUpgradeData->_UpgradeMnyCosts = testID;
		testID = 99950002;
		upUpgradeData->_UpgradedItemID = testID;
	}

#endif
}
MMOUIBackDecoratedMgr::~MMOUIBackDecoratedMgr()
{

}

//=================================================================================================================================
void MMOUIBackDecoratedMgr::Initialize()
{
	ReadBackDecoratedMakeUpCfg();
}

//=================================================================================================================================
void MMOUIBackDecoratedMgr::Shutdown()
{

}

//=================================================================================================================================
void MMOUIBackDecoratedMgr::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	//if(evt == NULL){
	//	return;
	//}
}

//=================================================================================================================================
UBOOL MMOUIBackDecoratedMgr::ReadBackDecoratedMakeUpCfg()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOUIBackDecoratedMgr::ReadBackDecoratedMakeUpCfg"), 0);

	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	CTabFile* tabFile = NULL;
	if( !mmoFacade.ReadConfigString( tabFile, TEXT("equip_levelup.txt"), TRUE ) ){
		return FALSE;
	}

	_BackDecoratedCfgs.Empty();

	UINT rowCnt = tabFile->GetRowCount();
	UINT startIdx = 2;
	//Actual data starts from row 3; header row, N/A row
	//_BackDecoratedCfgs.Add( rowCnt > startIdx ? (rowCnt - startIdx) : 0 );

	for( UINT i = startIdx ; i < rowCnt; ++i )
	{
		INT comsuptionItemID = appAtoi( tabFile->GetData( TEXT("MakeUpItemID"), i ) );
		//INT backDecoratedItemID = appAtoi( tabFile->GetData( TEXT("backDecoratedItemID"), i ) );
		BackDecoratedMakeUpCfg *data = AddBackDecoratedMakeUpCfg( comsuptionItemID );
		if( data == NULL ){
			warnf( NAME_Error, TEXT("ReadEquipLevelUpData at %d alloc BackDecoratedMakeUpCfg Failed."), comsuptionItemID );
			continue;
		}

		data->_MakedItemID = appAtoi( tabFile->GetData( TEXT("MakedItemID"), i ) );
		data->_ConsumptionItemID = comsuptionItemID;//appAtoi( tabFile->GetData( TEXT("MakeUpItemID"), i ) );
		data->_ConsumptionItemCnt = appAtoi( tabFile->GetData( TEXT("ItemCnt"), i ) );
		data->_MakeUpMnyCosts = appAtoi( tabFile->GetData( TEXT("MakeUpMnyCosts"), i ) );
	}

	delete tabFile;
	return TRUE;
}