#include "stdafx.h"
#include "MasterStudentManager.h"
#include "MMOMsgButtonManager.h"
#include "MMOChatManager.h"
#include "MMOAchievementManager.h"
#include "FriendManager.h"

#define  MasterExpRatio 1000
MasterStudentManager::MasterStudentManager()
{
	selfState = 0;
	cdTime = 0;
	masterInfo.nRoleID = 0;
	curApplyPage = 0;
	curSystemPosterPage = 0;
	_MasterStudentStatus = E_MASTER_NONE;
	_MasterRecruitStatus = E_MST_CANCEL_RECRUIT_SUCC;
}
MasterStudentManager::~MasterStudentManager()
{

}
void MasterStudentManager::Init()
{
	selfState = 0;
	cdTime = 0;
	masterInfo.nRoleID = 0;
	curApplyPage = 0;
	curSystemPosterPage = 0;
	studentTwitterInfo.roleID = 0;
	studentTwitterInfo.bApply = TRUE;
	masterTwitterInfo.roleID = 0;
	_MasterStudentStatus = E_MASTER_NONE;
	_MasterRecruitStatus = E_MST_CANCEL_RECRUIT_SUCC;
	masterTwitter = 0;
	bShowTwitterMasterInfo = FALSE;
	bShowTwitterStudentInfo = FALSE;
	masterExp1 = 0;
	masterExp2 = 0;

	ClearAll();

	CongratulationGiftDataManager::getSingleton().InitFromFile(TEXT("master_gift.txt"), true);
	GiftDetailDataManager::getSingleton().InitFromFile(TEXT("gift_detail.txt"), true);
	MailTemplateDataManager::getSingleton().InitFromFile(TEXT("mail_template.txt"), true);

	isLeaveTeacher = FALSE;

	GConfig->GetInt( TEXT("MMOGame.MMOUIMasterExp"), TEXT("MasterMoney") , masterExp , GGameIni );
	GConfig->GetInt( TEXT("MMOGame.MMOUIMasterExp"), TEXT("MasterExp1") , masterExp1 , GGameIni );
	GConfig->GetInt( TEXT("MMOGame.MMOUIMasterExp"), TEXT("MasterExp2") , masterExp2 , GGameIni );

}
void MasterStudentManager::Shutdown()
{
	listGiftConfig.clear();
	ClearAll();
}
void MasterStudentManager::Clear()
{
	ClearAll();
}
BOOL MasterStudentManager::IsMasterRelation(INT nRoleID)
{
	if(nRoleID <= 0)
		return FALSE;
	if(masterInfo.nRoleID == nRoleID)
		return TRUE;
	if(GetStudentDataByID(nRoleID))
		return TRUE;
	return FALSE;
}
const ApplicantsListData* MasterStudentManager::GetApplicantsDataByID(INT nRoleID)
{
	return _GetApplicantsDataByID(nRoleID);
}
const StudentListData* MasterStudentManager::GetStudentDataByID(INT nRoleID)
{
	return _GetStudentDataByID(nRoleID);
}
const SystemPosterListData* MasterStudentManager::GetSystemPosterDataByID(INT nRoleID)
{
	return _GetSystemPosterDataByID(nRoleID);
}
ApplicantsListData* MasterStudentManager::_GetApplicantsDataByID(INT nRoleID)
{
	ApllicantsList::iterator itr = listApllicants.begin();
	for (;itr != listApllicants.end(); ++itr)
	{
		if((*itr).nRoleID == nRoleID)
			return &(*itr);
	}
	return NULL;
}
StudentListData* MasterStudentManager::_GetStudentDataByID(INT nRoleID)
{
	StudentList::iterator itr = listStudent.begin();
	for (;itr != listStudent.end(); ++itr)
	{
		if((*itr).nRoleID == nRoleID)
			return &(*itr);
	}
	return NULL;

	//StudentList::iterator itr = listStudent.find(nRoleID);
	//if(itr != listStudent.end())
	//{
	//	return &(itr->second);
	//}
	//return NULL;
}
SystemPosterListData* MasterStudentManager::_GetSystemPosterDataByID(INT nRoleID)
{
	SystemPosterList::iterator itr = listSystemPoster.begin();
	for (;itr != listSystemPoster.end(); ++itr)
	{
		if((*itr).nRoleID == nRoleID)
			return &(*itr);
	}
	return NULL;
}

const MasterDetailInfo& MasterStudentManager::GetMasterDetailInfo() const
{
	return masterInfo;
}
const StudentList& MasterStudentManager::GetStudentList()	  const
{
	return listStudent;
}
const ApllicantsList& MasterStudentManager::GetStudentApllyList() const
{
	return listApllicants;
}
const ApllicantsList& MasterStudentManager::GetMasterApllyList()  const
{
	return listApllicants;
}
const SystemPosterList& MasterStudentManager::GetMasterSystemList()	  const
{
	return listSystemPoster;
}
const SystemPosterList& MasterStudentManager::GetStudentSystemList()	  const
{
	return listSystemPoster;
}

StudentListData* MasterStudentManager::AddorUpdateStudent(const MASTER_STUDENT_INFO* pkInfo)
{
	if(pkInfo == NULL)
		return NULL;
	StudentListData* pkData = _GetStudentDataByID(pkInfo->dwStudentID);
	PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(pkInfo->dwStudentID, TRUE);
	if(pkData)
	{
		pkData->nRoleID		= pkInfo->dwStudentID;
		pkData->nCZLevel	= pkInfo->byCZCount;
		pkData->bLeaveMaster = FALSE;
		/*if (pkInfo->bDesireGrad)
		pkData->nState = Student_DesireGrad ;
		else
		pkData->nState = pkInfo->bDesireGrad ;*/

		if(playerInfo)
		{
			pkData->RoleName = playerInfo->_RoleName;
			pkData->nLevel   = pkInfo->byLevel;
			pkData->nClass   = playerInfo->_RoleClass;
		}
	}
	else
	{
		StudentListData data;
		data.nRoleID	= pkInfo->dwStudentID;
		data.nCZLevel	= pkInfo->byCZCount;
		data.bLeaveMaster = FALSE;
		/*if (pkInfo->bDesireGrad)
			data.nState = Student_DesireGrad ;
		else
			data.nState = pkInfo->bDesireGrad ;*/

		if(playerInfo)
		{
			data.RoleName = playerInfo->_RoleName;
			data.nLevel   =  pkInfo->byLevel;
			data.nClass   = playerInfo->_RoleClass;
		}
		else
		{
			data.RoleName = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UnKnowRole"));
			data.nLevel   = pkInfo->byLevel;
			data.nClass   = EPCT_PlayerClassA;
		}
		listStudent.push_back(data);
		pkData = &listStudent.back();
	}

	return pkData;
}
StudentListData* MasterStudentManager::AddorUpdateStudent(const M2C_MASTER_ADD_STUDENT_NTF* pkInfo)
{
	StudentListData* pkData = _GetStudentDataByID(pkInfo->dwStudentID);
	PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(pkInfo->dwStudentID, TRUE);
	if(pkData)
	{
		pkData->nRoleID		= pkInfo->dwStudentID;
		pkData->bLeaveMaster = FALSE;

		if(playerInfo)
		{
			pkData->RoleName = playerInfo->_RoleName;
			//pkData->nLevel   = playerInfo->_Level;
			pkData->nClass   = playerInfo->_RoleClass;
		}
	}
	else
	{
		StudentListData data;
		data.nRoleID	= pkInfo->dwStudentID;
		data.nCZLevel	= 0;
		data.bLeaveMaster = FALSE;

		if(playerInfo)
		{
			data.RoleName = playerInfo->_RoleName;
			//data.nLevel   = playerInfo->_Level;
			data.nClass   = playerInfo->_RoleClass;
		}
		else
		{
			data.RoleName = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UnKnowRole"));
			//data.nLevel   = 0;
			data.nClass   = EPCT_PlayerClassA;
		}
		listStudent.push_back(data);
		pkData = &listStudent.back();
	}

	return pkData;
}
UBOOL MasterStudentManager::UpdateAllStudentInfoForCompletion( UBOOL isForceRequire )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	UBOOL isUpdated = FALSE;

	StudentList::iterator itr = listStudent.begin();
	for( ; itr != listStudent.end(); ++itr){
		StudentListData &data = *itr;
		//if( data.nLevel == 0 ){
			PlayerBaseInfo *playerInfo = mmoFacade.RequestPlayerBaseInfo( data.nRoleID, isForceRequire );
			if( playerInfo != NULL ){
				isUpdated = isUpdated ? isUpdated : ( data.nLevel != playerInfo->_Level );

				data.RoleName = playerInfo->_RoleName;
				//data.nLevel   = playerInfo->_Level;
				data.nClass   = playerInfo->_RoleClass;
			}
		//}
	}

	return isUpdated;
}
//===========================================================================================================
void MasterStudentManager::AddorUpdateApplicants(const M2C_MASTER_STUD_SUBMIT_APP_NTF* pkInfo)
{
	if(pkInfo == NULL)
		return;
	ApplicantsListData* pkData = _GetApplicantsDataByID(pkInfo->dwRoleID);
	PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(pkInfo->dwRoleID, TRUE);
	if(pkData)
	{
		pkData->nRoleID		= pkInfo->dwRoleID;
		pkData->nMasterExp	= 0;
		if(playerInfo)
		{
			pkData->RoleName	= playerInfo->_RoleName;
			pkData->nLevel		= playerInfo->_Level;
			pkData->nClass		= playerInfo->_RoleClass;
		}
	}
	else
	{
		ApplicantsListData data;
		data.nRoleID	= pkInfo->dwRoleID;
		data.nMasterExp = 0;
		if(playerInfo)
		{
			data.RoleName = playerInfo->_RoleName;
			data.nLevel   = playerInfo->_Level;
			data.nClass   = playerInfo->_RoleClass;
		}
		else
		{
			data.RoleName = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UnKnowRole"));
			data.nLevel   = 0;
			data.nClass   = EPCT_PlayerClassA;
		}
		listApllicants.push_back(data);
	}
}
void MasterStudentManager::AddorUpdateApplicants(const M2C_MASTER_TEACHER_SUBMIT_APP_NTF* pkInfo)
{
	if(pkInfo == NULL)
		return;
	ApplicantsListData* pkData = _GetApplicantsDataByID(pkInfo->dwTeacherID);
	PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(pkInfo->dwTeacherID, TRUE);
	if(pkData)
	{
		pkData->nRoleID		= pkInfo->dwTeacherID;
		pkData->nMasterExp  = pkInfo->nMasterExp;
		if(playerInfo)
		{
			pkData->RoleName = playerInfo->_RoleName;
			pkData->nClass   = playerInfo->_RoleClass;
			pkData->nLevel   = playerInfo->_Level;
		}
	}
	else
	{
		ApplicantsListData data;
		data.nRoleID	= pkInfo->dwTeacherID;
		data.nMasterExp  = pkInfo->nMasterExp;

		if(playerInfo)
		{
			data.RoleName = playerInfo->_RoleName;
			data.nLevel   = playerInfo->_Level;
			data.nClass   = playerInfo->_RoleClass;
		}
		else
		{
			data.RoleName = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UnKnowRole"));
			data.nLevel   = 0;
			data.nClass   = EPCT_PlayerClassA;
		}
		listApllicants.push_back(data);
		
	}
}
UBOOL MasterStudentManager::UpdateAllApllicantsInfoForCompletion( UBOOL isForceRequire )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	UBOOL isUpdated = FALSE;

	ApllicantsList::iterator itr = listApllicants.begin();
	for( ; itr != listApllicants.end(); ++itr){
		ApplicantsListData &data = *itr;
		//if( data.nLevel == 0 ){
			PlayerBaseInfo *playerInfo = mmoFacade.RequestPlayerBaseInfo( data.nRoleID, isForceRequire );
			if( playerInfo != NULL ){
				isUpdated = isUpdated ? isUpdated : ( data.nLevel != playerInfo->_Level );

				data.RoleName = playerInfo->_RoleName;
				//data.nLevel   = playerInfo->_Level;
				data.nClass   = playerInfo->_RoleClass;
			}
		//}
	}

	return isUpdated;
}
//===========================================================================================================
void MasterStudentManager::AddorUpdateSystemPoster(const M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO* pkInfo)
{
	if(pkInfo == NULL)
		return;
	SystemPosterListData* pkData = _GetSystemPosterDataByID(pkInfo->dwRoleID);
	PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(pkInfo->dwRoleID, TRUE);
	if(pkData)
	{
		pkData->nRoleID = pkInfo->dwRoleID;
		pkData->nMasterExp = pkInfo->llMasterExp;
		pkData->bValid   = true;

		if(playerInfo)
		{
			pkData->RoleName = playerInfo->_RoleName;
			pkData->nClass   = playerInfo->_RoleClass;
			pkData->nLevel   = playerInfo->_Level;
		}
	}
	else
	{
		SystemPosterListData data;
		data.nRoleID = pkInfo->dwRoleID;
		data.nMasterExp = pkInfo->llMasterExp;
		data.bValid   = true;
		data.bCanApply   = true;

		if(playerInfo)
		{
			data.RoleName = playerInfo->_RoleName;
			data.nLevel   = playerInfo->_Level;
			data.nClass   = playerInfo->_RoleClass;
		}
		else
		{
			data.RoleName = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UnKnowRole"));
			data.nLevel   = 0;
			data.nClass   = EPCT_PlayerClassA;
		}
		listSystemPoster.push_back(data);
	}
}
void MasterStudentManager::AddorUpdateSystemPoster(const M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO* pkInfo)
{
	if(pkInfo == NULL)
		return;
	SystemPosterListData* pkData = _GetSystemPosterDataByID(pkInfo->dwRoleID);
	PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(pkInfo->dwRoleID, TRUE);
	if(pkData)
	{
		pkData->nRoleID = pkInfo->dwRoleID;
		pkData->bValid   = true;

		if (studentTwitterInfo.roleID == pkInfo->dwRoleID && studentTwitterInfo.bApply == FALSE)
		{
			pkData->bCanApply   = false;
		}

		if(playerInfo)
		{
			pkData->RoleName  = playerInfo->_RoleName;
			pkData->nClass    = playerInfo->_RoleClass;
			pkData->nLevel    = playerInfo->_Level;
		}
	}
	else
	{
		SystemPosterListData data;
		data.nRoleID = pkInfo->dwRoleID;
		data.bValid   = true;
		
		if (studentTwitterInfo.roleID == pkInfo->dwRoleID && studentTwitterInfo.bApply == FALSE)
		{
			data.bCanApply   = false;
		}
		else
		{
			data.bCanApply   = true;
		}


		if(playerInfo)
		{
			data.RoleName = playerInfo->_RoleName;
			data.nLevel   = playerInfo->_Level;
			data.nClass   = playerInfo->_RoleClass;
		}
		else
		{
			data.RoleName = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UnKnowRole"));
			data.nLevel   = 0;
			data.nClass   = EPCT_PlayerClassA;
		}
		listSystemPoster.push_back(data);
	}
}
UBOOL MasterStudentManager::UpdateAllSystemPosterInfoForCompletion( UBOOL isForceRequire )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	UBOOL isUpdated = FALSE;

	SystemPosterList::iterator itr = listSystemPoster.begin();
	for( ; itr != listSystemPoster.end(); ++itr){
		SystemPosterListData &data = *itr;
		//if( data.nLevel == 0 ){
			PlayerBaseInfo *playerInfo = mmoFacade.RequestPlayerBaseInfo( data.nRoleID, isForceRequire );
			if( playerInfo != NULL ){
				isUpdated = isUpdated ? isUpdated : ( data.nLevel != playerInfo->_Level );

				data.RoleName = playerInfo->_RoleName;
				//data.nLevel   = playerInfo->_Level;
				data.nClass   = playerInfo->_RoleClass;
			}
		//}
	}

	return isUpdated;
}
//===========================================================================================================
void MasterStudentManager::DeleteStudentByID(INT nRoleID)
{
	StudentList::iterator itr = listStudent.begin();
	for (; itr != listStudent.end(); ++itr)
	{
		if((*itr).nRoleID == nRoleID)
		{
			listStudent.erase(itr);
			break;
		}
	}
	
	
}
void MasterStudentManager::DeleteSystemPosterByID(INT nRoleID)
{
	SystemPosterList::iterator itr = listSystemPoster.begin();
	for (; itr != listSystemPoster.end(); ++itr)
	{
		if((*itr).nRoleID == nRoleID)
		{
			listSystemPoster.erase(itr);
			break;
		}
	}
}
void MasterStudentManager::DeleteApplicantsByID(INT nRoleID)
{
	ApllicantsList::iterator itr = listApllicants.begin();
	for (; itr != listApllicants.end(); ++itr)
	{
		if((*itr).nRoleID == nRoleID)
		{
			listApllicants.erase(itr);
			break;
		}
	}
}

void MasterStudentManager::SetApplyCurPage(INT nPage)
{
	curApplyPage = nPage;
}
void MasterStudentManager::SetSystemPosterPage(INT nPage)
{
	curSystemPosterPage = nPage;
}
INT MasterStudentManager::GetApplyCurPage()
{
	return curApplyPage;
}
INT MasterStudentManager::GetSystemPosterCurPage()
{
	return curSystemPosterPage;
}
INT MasterStudentManager::GetApplyMaxPage()
{
	INT nSize = (INT)listApllicants.size();
	if ( nSize > 0)
		return (nSize % MAX_ROW_MASTERLIST == 0) ? (nSize / MAX_ROW_MASTERLIST - 1) : (nSize / MAX_ROW_MASTERLIST);
	return nSize;
}
INT MasterStudentManager::GetSystemPosterMaxPage()
{
	INT nSize = (INT)listSystemPoster.size();
	if ( nSize > 0)
		return (nSize % MAX_ROW_MASTERLIST == 0) ? (nSize / MAX_ROW_MASTERLIST - 1) : (nSize / MAX_ROW_MASTERLIST);
	return nSize;
}
INT MasterStudentManager::GetSudentNum()
{
	INT num = 0;
	StudentList::iterator itr = listStudent.begin();
	for( ; itr != listStudent.end(); ++itr)
	{
		if ((*itr).bLeaveMaster == FALSE)
		{
			num++;
		}
	}
	return num;
}
INT MasterStudentManager::GetSelfState()
{
	return selfState;
}
void MasterStudentManager::SortStudentMembers(INT type)
{

}
void MasterStudentManager::SortMasterMembers(INT type)
{

}
void MasterStudentManager::MarkSystemPosterList()
{
	SystemPosterList::iterator itr = listSystemPoster.begin();
	for (; itr != listSystemPoster.end(); ++itr)
	{
		(*itr).bValid = false;
	}
}
void MasterStudentManager::RemoveInvalidSystemPoster()
{
	SystemPosterList::iterator itr = listSystemPoster.begin();
	for (; itr != listSystemPoster.end(); )
	{
		if((*itr).bValid == false)
			itr = listSystemPoster.erase(itr);
		else
			++itr;
	}
}
void MasterStudentManager::ClearAll()
{
	ClearMasterInfo();
	ClearApplicants();
	ClearSystemPoster();
	cdTime = 0;
	
}
void MasterStudentManager::ClearMasterInfo()
{
	masterInfo.masterName = TEXT("");
	masterInfo.nClass     = 0;
	masterInfo.nCZLevel   = 0;
	masterInfo.nLevel     = 0;
	masterInfo.nRoleID    = 0;
	masterInfo.nStudentNum= 0;
	listStudent.clear();
}
void MasterStudentManager::ClearApplicants()
{
	listApllicants.clear();
	curApplyPage = 0;
}
void MasterStudentManager::ClearSystemPoster()
{
	listSystemPoster.clear();
	curSystemPosterPage = 0;
}
INT MasterStudentManager::GetCDTime()					
{
	return cdTime;
}

void MasterStudentManager::SetMasterRecruitStatus(EMST_SUCCESS_RSP_TYPE bValue)
{
	_MasterRecruitStatus = bValue; 
}
EMST_SUCCESS_RSP_TYPE  MasterStudentManager::GetMasterRecruitStatus() 
{
	return _MasterRecruitStatus; 
}
EMASTER_STATE  MasterStudentManager::GetMasterStudentState()
{
	return _MasterStudentStatus; 
}
void MasterStudentManager::SetMasterStudentState(EMASTER_STATE val)
{
	_MasterStudentStatus = val; 

	if(val == E_MASTER_STUDENT)
	{
		const AMMOPlayer* pkPlayer   =   MMOFacade::getSingleton().GetMainPlayer();
		if(pkPlayer&&pkPlayer->ActorAttributes&&pkPlayer->ActorAttributes->LevelAttributes.Level >= GRADUATE_LEVEL)
			selfState = 1;
		else
			selfState = 0;
	}
	else
		selfState = -1;

	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterStateChange);
	MMOUI::PostGameEvent(&evt);
}
void MasterStudentManager::SetOpenMasterStudentPanelDelegate(FScriptDelegate fun)
{
	_OpenMasterStudentPanelDelegate = fun;
}
void MasterStudentManager::SetOpenCongUpLevelPanelDelegate(FScriptDelegate fun)
{
	_OpenCongUpLevelPanelDelegate = fun;
}

void MasterStudentManager::NotifyUpdateMasterList(const M2C_MASTER_RECRUIT_LIST*msg)
{
	if(msg == NULL)
		return;
	MarkSystemPosterList();
	for (UINT32 i = 0; i < msg->byCount; ++i)
	{
		M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO* pMaster = ( M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO* )&msg->szListData[i];
		AddorUpdateSystemPoster(pMaster);
	}
	RemoveInvalidSystemPoster();

	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterSystemPosterList);
	MMOUI::PostGameEvent(&evt);
}
void MasterStudentManager::NotifyUpdateMasterSoloListAck(const M2C_MASTER_GET_SOLO_LIST_ACK* msg)
{
	if(msg == NULL)
		return;
	MarkSystemPosterList();
	for (UINT32 i = 0; i < msg->byCount; ++i)
	{
		M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO* pMaster = ( M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO* )&msg->szListData[i];
		AddorUpdateSystemPoster(pMaster);
	}
	RemoveInvalidSystemPoster();
	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_StudentSystemPosterList);
	MMOUI::PostGameEvent(&evt);
}
void  MasterStudentManager::NotifyUpdateRecruitAck(INT nState)
{
	SetMasterRecruitStatus((EMST_SUCCESS_RSP_TYPE)nState);
	if (E_MST_APPLY_RECRUIT_SUCC == (EMST_SUCCESS_RSP_TYPE)nState)
	{
		FString strTips = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MasterStudent.ApplyStudentSuccess"));
		MMOChatManager::getSingleton().AddSystemMessage(strTips); 
		MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,TEXT("MMOGame.MasterStudent.ApplyStudentSuccess")); 
	}
	else if(E_MST_CANCEL_RECRUIT_SUCC == (EMST_SUCCESS_RSP_TYPE)nState)
	{
		FString strTips = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MasterStudent.CancelStudentSuccess"));
		MMOChatManager::getSingleton().AddSystemMessage(strTips); 
		MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,TEXT("MMOGame.MasterStudent.CancelStudentSuccess")); 
	}
	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_RecruitStatus);
	MMOUI::PostGameEvent(&evt);
}
void  MasterStudentManager::NotifySubmitStudentApplyAck(INT nRoleID)
{
	SystemPosterListData*pkMaster = _GetSystemPosterDataByID(nRoleID);
	if(pkMaster)
	{
		pkMaster->bCanApply = false;
		MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterSystemPosterList, nRoleID);
		MMOUI::PostGameEvent(&evt);

		FString strTips = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MasterStudent.ApplyMasterSuccess"));
		MMOChatManager::getSingleton().AddSystemMessage(strTips); 
		MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,TEXT("MMOGame.MasterStudent.ApplyMasterSuccess")); 
	}
}
void  MasterStudentManager::NotifySubmitStudentApplyNtf(const M2C_MASTER_STUD_SUBMIT_APP_NTF* msg)
{
	if(msg == NULL)
		return;

	if(MMOUI::GetUIManager()->DeclineInviteMaster)
	{
		CMessageHandler::GetInstance().Send_RefuseStudentApply(msg->dwRoleID);
	}
	else
	{
		AddorUpdateApplicants(msg);
		MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_StudentApplicantsList);
		MMOUI::PostGameEvent(&evt);

		/*FString strTips = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MasterStudent.ApplyMaster")), UTF8_TO_TCHAR(msg->szRoleName));
		MMOChatManager::getSingleton().AddSystemMessage(strTips); 
		MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,TEXT("MMOGame.MasterStudent.ApplyMaster")); 
		*/
		MMOMsgButtonManager_Parms Parms(EC_EventParm);
		Parms.Parm1 = -1;
		Parms.Parm2 = 4;
		Parms.Parm3 = msg->dwRoleID;
		MMOMsgButtonManager::getSingleton().AddNewButton(_OpenMasterStudentPanelDelegate, Parms, E_MsgBtnShow_Master );
	}
}
void  MasterStudentManager::NotifyAgreeStudentApplyAck(INT nRoleID)
{
	DeleteApplicantsByID(nRoleID);
	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_StudentApplicantsList);
	MMOUI::PostGameEvent(&evt);

	FString strTips = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MasterStudent.AdoptStudentSuccess"));
	MMOChatManager::getSingleton().AddSystemMessage(strTips); 
	MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,TEXT("MMOGame.MasterStudent.AdoptStudentSuccess")); 
}
void  MasterStudentManager::NotifyRefuseStudentApplyAck(INT nRoleID)
{
	DeleteApplicantsByID(nRoleID);
	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_StudentApplicantsList);
	MMOUI::PostGameEvent(&evt);
}
void  MasterStudentManager::NotifyRefuseStudentApplyNtf(INT nRoleID)
{
	SystemPosterListData* pkMaster = _GetSystemPosterDataByID(nRoleID);
	if(pkMaster)
	{
		pkMaster->bCanApply = true;

		MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterSystemPosterList, nRoleID);
		MMOUI::PostGameEvent(&evt);
	}
	FString msg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Master.MasterApplyNtf") );
	MMOChatManager::getSingleton().AddSystemMessage(msg);
	MMOUI::GetUIManager()->delegateAddScreenMessage( msg , MsgInfoType_Table , msg );
}
void  MasterStudentManager::NotifyCacelStudentApplyAck(INT nRoleID)
{
	SystemPosterListData* pkMaster = _GetSystemPosterDataByID(nRoleID);
	if(pkMaster)
	{
		pkMaster->bCanApply = true;

		MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterSystemPosterList, nRoleID);
		MMOUI::PostGameEvent(&evt);
	}
}
void  MasterStudentManager::NotifyCacelStudentApplyNtf(INT nRoleID)
{
	DeleteApplicantsByID(nRoleID);
	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_StudentApplicantsList);
	MMOUI::PostGameEvent(&evt);
}
void  MasterStudentManager::NotifySubmitMasterApplyAck(INT nRoleID)
{
	SystemPosterListData* pkApplicants =  _GetSystemPosterDataByID(nRoleID);
	if(pkApplicants)
	{
		pkApplicants->bCanApply = false;
		//notify downward
		MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_StudentSystemPosterList);
		MMOUI::PostGameEvent(&evt);
	}
}
void  MasterStudentManager::NotifySubmitMasterApplyNtf(const M2C_MASTER_TEACHER_SUBMIT_APP_NTF* msg)
{
	if(msg == NULL)
		return;
	if(MMOUI::GetUIManager()->DeclineInviteDisciple)
	{
		CMessageHandler::GetInstance().Send_RefuseMasterApply(msg->dwTeacherID);
		return;
	}
	switch(msg->bySourceType)
	{
	case 0:
		{
			AddorUpdateApplicants(msg);
			//notify downward
			MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterApplicantsList);
			MMOUI::PostGameEvent(&evt);

			MMOMsgButtonManager_Parms Parms(EC_EventParm);
			Parms.Parm1 = -1;
			Parms.Parm2 = 2;
			Parms.Parm3 = msg->dwTeacherID;
			MMOMsgButtonManager::getSingleton().AddNewButton(_OpenMasterStudentPanelDelegate, Parms, E_MsgBtnShow_Master );
		}
		break;
	case 1:
		{
			bShowTwitterMasterInfo = TRUE;

			//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MasterInvite,TRUE,"");

			masterTwitterInfo.roleID = msg->dwTeacherID;
			masterTwitterInfo.level = msg->byLevel;
			masterTwitterInfo.masterExp = msg->nMasterExp;

			//MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_HelloStudent);
			//MMOUI::PostGameEvent(&evt);

			if(MMO::GetCurrentGameState() == EGameState_InGame)
			{
				AddHelloStudentBtnBall();
			}
			
		}
		break;
	}
	
}

void MasterStudentManager::AddHelloStudentBtnBall()
{
	MMOMsgButtonManager_Parms Parms(EC_EventParm);
	Parms.Parm1 = 0;
	MMOMsgButtonManager::getSingleton().AddNewButton(_OpenMasterStudentPanelDelegate, Parms, E_MsgBtnShow_Master );
}
void  MasterStudentManager::NotifyAgreeMasterApplyAck(INT nRoleID)
{
	DeleteApplicantsByID(nRoleID);

	FString strTips = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MasterStudent.BeMasterSuccess"));
	MMOChatManager::getSingleton().AddSystemMessage(strTips); 
	MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,TEXT("MMOGame.MasterStudent.BeMasterSuccess")); 
	//notify downward
	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterApplicantsList);
	MMOUI::PostGameEvent(&evt);
}
void  MasterStudentManager::NotifyRefuseMasterApplyAck(INT nRoleID)
{
	DeleteApplicantsByID(nRoleID);
	//notify downward
	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterApplicantsList);
	MMOUI::PostGameEvent(&evt);
}
void  MasterStudentManager::NotifyRefuseMasterApplyNtf(INT nRoleID)
{
	SystemPosterListData* pkApplicants =  _GetSystemPosterDataByID(nRoleID);
	if(pkApplicants)
	{
		pkApplicants->bCanApply = true;
		//notify downward
		MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_StudentSystemPosterList);
		MMOUI::PostGameEvent(&evt);
	}
	FString msg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Master.StudentApplyNtf") );
	MMOChatManager::getSingleton().AddSystemMessage(msg);
	MMOUI::GetUIManager()->delegateAddScreenMessage( msg , MsgInfoType_Table , msg );
}
void  MasterStudentManager::NotifyCacelMasterApplyAck(INT nRoleID)
{
	SystemPosterListData* pkApplicants =  _GetSystemPosterDataByID(nRoleID);
	if(pkApplicants)
	{
		pkApplicants->bCanApply = true;

		//notify downward
		MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_StudentSystemPosterList);
		MMOUI::PostGameEvent(&evt);
	}
}
void  MasterStudentManager::NotifyCacelMasterApplyNtf(INT nRoleID)
{
	DeleteApplicantsByID(nRoleID);
	//notify downward
	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_StudentSystemPosterList);
	MMOUI::PostGameEvent(&evt);
}
void  MasterStudentManager::NotifyLeaveMasterAck()
{
	//clear master info
	ClearMasterInfo();
	FString strTips = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MasterStudent.LeaveMasterForStudent"));
	MMOChatManager::getSingleton().AddSystemMessage(strTips); 
	MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,TEXT("MMOGame.MasterStudent.LeaveMasterForStudent")); 
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MasterCongMain,FALSE,"");
	//MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterInfo);
	//MMOUI::PostGameEvent(&evt);

	//query master/apprentice reward experience
	if (_MasterStudentStatus == E_MASTER_NONE)
	{
		CMessageHandler::GetInstance().Send_QueryMasterExp();
	}
	else
	{
		CMessageHandler::GetInstance().Send_GetMasterInfo();
	}
}
void  MasterStudentManager::NotifyLeaveMasterNtf(INT nRoleID)
{
	const StudentListData* pkData = GetStudentDataByID(nRoleID);
	if(pkData && GetMasterStudentState()== E_MASTER_TEACHER)
	{
		FString	strTips = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MasterStudent.LeaveMasterForMaster")), *(pkData->RoleName));
		FString	strString = TEXT("MMOGame.MasterStudent.LeaveMasterForMaster");
		MMOChatManager::getSingleton().AddSystemMessage(strTips); 
		MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,strString); 

		DeleteStudentByID(nRoleID);
		MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterInfo);
		MMOUI::PostGameEvent(&evt);

		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MasterCongMain,FALSE,"");

		//apprentice betrays master, query master/apprentice team reward
		CMessageHandler::GetInstance().Send_QueryMasterExp();
	}
}
void  MasterStudentManager::NotifyFireStudentAck(INT nRoleID)
{
	const StudentListData* pkData = GetStudentDataByID(nRoleID);
	if(pkData)
	{
		FString strTips =FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MasterStudent.FireStudent")),*(pkData->RoleName));
		MMOChatManager::getSingleton().AddSystemMessage(strTips); 
		MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,TEXT("MMOGame.MasterStudent.FireStudent")); 

	}
	UMMOUIMasterCongratulationMain* panel = Cast<UMMOUIMasterCongratulationMain>(MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_MasterCongMain));
	if (panel && panel->currentRoleId == nRoleID)
	{
		panel->SetVisible(FALSE);
	}
	
	DeleteStudentByID(nRoleID);
	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterInfo);
	MMOUI::PostGameEvent(&evt);
}
void  MasterStudentManager::NotifyFireStudentNtf(INT nRoleID)
{
	ClearMasterInfo();
	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterInfo);
	MMOUI::PostGameEvent(&evt);
	
}
void  MasterStudentManager::NotifyAddStudentNtf(const M2C_MASTER_ADD_STUDENT_NTF* msg)
{

	//AddorUpdateStudent(msg);

	////notify downward
	//MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterInfo);
	//MMOUI::PostGameEvent(&evt);
	CMessageHandler::GetInstance().Send_GetMasterInfo();
	//add to follow
	FriendManager::getSingleton().RequestAddAttention( msg->dwStudentID , TEXT("") );
}
void  MasterStudentManager::NotifyDelStudentNtf(INT nRoleID)
{
	DeleteStudentByID(nRoleID);
	//notify downward
	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterInfo);
	MMOUI::PostGameEvent(&evt);
}
void  MasterStudentManager::NotifyClearInfo()
{
	ClearMasterInfo();
	//notify downward
	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterInfo);
	MMOUI::PostGameEvent(&evt);

	//prompt
	if (isLeaveTeacher)
	{
		isLeaveTeacher = FALSE;
	}
	else
	{
		
		FString strTips = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MasterStudent.FiredForStudent"));
		MMOChatManager::getSingleton().AddSystemMessage(strTips); 
		MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,TEXT("MMOGame.MasterStudent.FiredForStudent")); 
	}
	
}
void  MasterStudentManager::NotifyMasterInfo(const M2C_MASTER_INFO* msg)
{
	if(msg == NULL)
		return;

	PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(msg->dwTeacherID, TRUE);

	masterInfo.nRoleID			=		msg->dwTeacherID;
	masterInfo.nLevel			=		msg->byTeacherLevel;
	//masterInfo.nClass			=		msg->byTeacherClass;
	//masterInfo.nCZCount			=		msg->byCZCount;
	masterInfo.nStudentNum		=		msg->byStudentNum;
	//masterInfo.masterName       =       UTF8_TO_TCHAR( msg->szTeacherName);

	//if (msg->byTeacherCZLevel <= GRADUATE_LEVEL)
	//{
		masterInfo.nCZLevel = GRADUATE_LEVEL;
	//}
	//else
	//{
		//masterInfo.nCZLevel  =	msg->byTeacherCZLevel;
	//}
	
	if(playerInfo)
	{
		masterInfo.masterName = playerInfo->_RoleName;
		//masterInfo.nLevel     = playerInfo->_Level;
		masterInfo.nClass     = playerInfo->_RoleClass;
	}
	else
	{
		masterInfo.masterName = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UnKnowRole"));
		masterInfo.nLevel   = msg->byTeacherLevel;
		masterInfo.nClass   = EPCT_PlayerClassA;
	}
	
	/*std::map< INT, UBOOL > stdnMap;
	const StudentList& stdnList = GetStudentList();
	for (StudentList::const_iterator itr = stdnList.begin(); itr != stdnList.end(); ++itr)
	{
		stdnMap.insert( std::make_pair(itr->first, FALSE ) );
	}*/

	listStudent.clear();
	for (INT n = 0; n < msg->byStudentNum;++n)
	{
		AddorUpdateStudent(&(msg->arrInfo[n]));
		/*StudentListData *newData = AddorUpdateStudent(&(msg->arrInfo[n]));*/
		/*if (newData != NULL){
			std::map< INT, UBOOL >::iterator itor = stdnMap.find( newData->nRoleID );
			if (itor != stdnMap.end())
			{
				itor->second = TRUE;
			}
		}*/
	}

	/*for (std::map< INT, UBOOL >::iterator itr = stdnMap.begin(); itr != stdnMap.end(); ++itr)
	{
		if (itr->second == FALSE)
		{
			DeleteStudentByID( itr->first );
		}
	}*/

	//notify downward
	//MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterInfo);
	//MMOUI::PostGameEvent(&evt);

	//request reward data
	CMessageHandler::GetInstance().Send_QueryMasterExp();
}
UBOOL MasterStudentManager::UpdateMasterInfoForCompletion( UBOOL isForceRequire )
{
	UBOOL isUpdated = FALSE;
	PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(masterInfo.nRoleID, isForceRequire);
	if(playerInfo)
	{
		masterInfo.masterName = playerInfo->_RoleName;
		//masterInfo.nLevel     = playerInfo->_Level;
		masterInfo.nClass     = playerInfo->_RoleClass;

		isUpdated = TRUE;
	}

	return isUpdated;
}
void  MasterStudentManager::NotifyCongratulationCount(INT nRoleID,INT nCount)
{
	StudentListData* pkData = _GetStudentDataByID(nRoleID);
	if(pkData)
	{
		pkData->nCZLevel = nCount;
	}
	else if (masterInfo.nRoleID == nRoleID)
	{
		if (nCount <= GRADUATE_LEVEL)
		{
			masterInfo.nCZLevel = GRADUATE_LEVEL;
		}
		else
		{
			masterInfo.nCZLevel = nCount;
		}
	}

	//notify downward
	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterInfo);
	MMOUI::PostGameEvent(&evt);
}
void  MasterStudentManager::NotifyCongratulationTarget(INT nRoleID)
{
	//congratulate a fellow disciple on success
	FString strTips =  MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.CongratulationOK"));
	StudentListData* pkData = _GetStudentDataByID(nRoleID);
	if(pkData)
	{
		//pkData->nCZCount -= 1;
		if(pkData->nCZLevel < 0)
			pkData->nCZLevel = 0;
	}
	else if (masterInfo.nRoleID == nRoleID)
	{
		//masterInfo.nCZCount -= 1;
		if(masterInfo.nCZLevel < 0)
			masterInfo.nCZLevel = 0;
	}
	MMOChatManager::getSingleton().AddSystemMessage(strTips); 
	MMOUI::GetUIManager()->delegateAddScreenMessage(strTips); 
}

void MasterStudentManager::NotifyMasterValueChange(INT Money , INT OldMoney)
{
	static bool FirstTime = true;
	INT diffMoney = Money - OldMoney;
	if(!FirstTime && diffMoney > 0)
	{
		diffMoney = GetMasterExpByRatio(diffMoney);
		MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_ChangeValue, 0);
		MMOUI::PostGameEvent(&evt);

		FString strTips = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MasterStudent.CongGetMasterValue")), diffMoney);
		MMOChatManager::getSingleton().AddSystemMessage(strTips); 
		MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,TEXT("MMOGame.MasterStudent.CongGetMasterValue")); 
	}
	if(FirstTime)
		FirstTime = false;
}
void MasterStudentManager::NotifyCDTimeChange(INT nCDTime)
{
	cdTime = nCDTime;
	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_CDTime);
	MMOUI::PostGameEvent(&evt);
}
void MasterStudentManager::NotifyLevelUp(INT nRoleID,INT nLevel)
{
	StudentListData* pkData = _GetStudentDataByID(nRoleID);
	if(pkData)
	{
		pkData->nLevel = nLevel;
	}
	else if (nRoleID == masterInfo.nRoleID)
	{
		masterInfo.nLevel = nLevel;
	}

	//update level
	PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(nRoleID);
	if( playerInfo != NULL ){
		playerInfo->_Level = nLevel;
	}

	//MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterInfo);
	//MMOUI::PostGameEvent(&evt);

	//CMessageHandler::GetInstance().Send_GetMasterInfo();
}
void MasterStudentManager::NotifyDesireGradAck()
{
	if(selfState < 2)
	{
		selfState = 2;
		MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterStateChange);
		MMOUI::PostGameEvent(&evt);
	}
}
void MasterStudentManager::NotifyDesireGradNtf(INT nRoleID)
{
	StudentListData* pkData = _GetStudentDataByID(nRoleID);
	if(pkData)
	{
		pkData->nState = 2;
		MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterInfo);
		MMOUI::PostGameEvent(&evt);

		MMOMsgButtonManager_Parms Parms(EC_EventParm);
		Parms.Parm1 = -1;
		Parms.Parm2 = 2;
		MMOMsgButtonManager::getSingleton().AddNewButton(_OpenMasterStudentPanelDelegate, Parms, E_MsgBtnShow_Master );
	}
}
void MasterStudentManager::NotifyStudentGrad(INT nRoleID)
{
	//StudentListData* pkData = _GetStudentDataByID(nRoleID);
	//if(pkData)
	//{
		MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_StudentGrad);
		MMOUI::PostGameEvent(&evt);
	//}

}

void MasterStudentManager::NotifyHelloTeacher(UINT32 nRoleId, UINT8 level)
{
	//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MasterTips,TRUE,"");

	studentTwitterInfo.roleID = nRoleId;
	studentTwitterInfo.level = level;

	//MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_HelloTeacher);
	//MMOUI::PostGameEvent(&evt);

	MMOMsgButtonManager_Parms Parms(EC_EventParm);
	Parms.Parm1 = 1;
	MMOMsgButtonManager::getSingleton().AddNewButton(_OpenMasterStudentPanelDelegate, Parms, E_MsgBtnShow_Master );

	//UpdateSTudentAppleState(nRoleId);
}
void MasterStudentManager::NotifyEnterGame()
{
	if(MMO::GetCurrentGameState() == EGameState_InGame)
	{
		//CMessageHandler::GetInstance().Send_GetMasterInfo();
	}
}
void MasterStudentManager::NotifyLevelChange(INT nLevel)
{
	if(GetMasterStudentState() == E_MASTER_STUDENT)
	{
		if(nLevel >= GRADUATE_LEVEL&&selfState < 1)
		{
			selfState = 1;
			MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterStateChange);
			MMOUI::PostGameEvent(&evt);
		}
	}
}
void MasterStudentManager::AddGiftConfigData(const CongratulationGiftData& data)
{
	listGiftConfig.insert(std::make_pair(data.nTargetLevel,data));
}
void MasterStudentManager::AddGiftDetailConfigData(const GiftDetailData& data)
{
	listGiftDetailConfig.push_back(data);
}
void MasterStudentManager::AddMailTemplateConfigData(const MailTemplateData& data)
{
	listMailTemplateConfig.push_back(data);
}
const GiftDetailData* MasterStudentManager:: GetGiftDeatilData(INT nGiftID) const
{
	GiftDetailDataList::const_iterator itr = listGiftDetailConfig.begin();
	for (;itr != listGiftDetailConfig.end(); ++itr)
	{
		if((*itr).nGiftID == nGiftID)
			return &(*itr);
	}
	return NULL;
}
const MailTemplateData*	MasterStudentManager::GetMailTemplateData(INT nMailID) const
{
	MailTemplateDataList::const_iterator itr = listMailTemplateConfig.begin();
	for (;itr != listMailTemplateConfig.end(); ++itr)
	{
		if((*itr).nMailID == nMailID)
			return &(*itr);
	}
	return NULL;
}
const CongratulationGiftData* MasterStudentManager::GetGiftConfigData(INT nLevel) const
{
	GiftConfigDataMap::const_iterator itr = listGiftConfig.find(nLevel);

	if(itr != listGiftConfig.end())
		return &(itr->second);

	return NULL;
}

CongratulationGiftData* CongratulationGiftDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void CongratulationGiftDataProcessor::DestroyDataObject(CongratulationGiftData* obj)
{
	if(obj)
	{
		delete obj;
		obj = NULL;
	}
}

void CongratulationGiftDataProcessor::ParseData(CongratulationGiftData* obj, class CTabFile* dataRow, int rowIdx)
{
	CongratulationGiftData data;
	data.nTargetLevel							= TReadDataValue_Int<CongratulationGiftDataManager>(TEXT("ID"), dataRow, rowIdx);	
	if(data.nTargetLevel <= 0)
		return;
	data.nType0Consume							= TReadDataValue_Int<CongratulationGiftDataManager>(TEXT("arrCZMoneyConsume0"), dataRow, rowIdx);
	data.nType1Consume							= TReadDataValue_Int<CongratulationGiftDataManager>(TEXT("arrCZMoneyConsume1"), dataRow, rowIdx);
	data.nType2Consume							= TReadDataValue_Int<CongratulationGiftDataManager>(TEXT("arrCZMoneyConsume2"), dataRow, rowIdx);
	data.nType0MailID1							= TReadDataValue_Int<CongratulationGiftDataManager>(TEXT("arrCZT2SStudentMailID0"), dataRow, rowIdx);
	data.nType0MailID2							= TReadDataValue_Int<CongratulationGiftDataManager>(TEXT("arrCZT2SStudentMailID1"), dataRow, rowIdx);
	data.nType0MailID3							= TReadDataValue_Int<CongratulationGiftDataManager>(TEXT("arrCZT2SStudentMailID2"), dataRow, rowIdx);
	data.nType1MailID1							= TReadDataValue_Int<CongratulationGiftDataManager>(TEXT("arrCZS2TTeacherMailID0"), dataRow, rowIdx);
	data.nType1MailID2							= TReadDataValue_Int<CongratulationGiftDataManager>(TEXT("arrCZS2TTeacherMailID1"), dataRow, rowIdx);
	data.nType1MailID3							= TReadDataValue_Int<CongratulationGiftDataManager>(TEXT("arrCZS2TTeacherMailID2"), dataRow, rowIdx);
	data.nType2MailID1							= TReadDataValue_Int<CongratulationGiftDataManager>(TEXT("arrCZC2CTargetMailID0"), dataRow, rowIdx);
	data.nType2MailID2							= TReadDataValue_Int<CongratulationGiftDataManager>(TEXT("arrCZC2CTargetMailID1"), dataRow, rowIdx);
	data.nType2MailID3							= TReadDataValue_Int<CongratulationGiftDataManager>(TEXT("arrCZC2CTargetMailID2"), dataRow, rowIdx);

	MasterStudentManager::getSingleton().AddGiftConfigData(data);
}

void CongratulationGiftDataProcessor::PostInitData()
{
}

GiftDetailData* GiftDetailDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void GiftDetailDataProcessor::DestroyDataObject(GiftDetailData* obj)
{

}

void GiftDetailDataProcessor::ParseData(GiftDetailData* obj, class CTabFile* dataRow, int rowIdx)
{
	GiftDetailData data;
	data.nGiftID = TReadDataValue_Int<GiftDetailDataManager>(TEXT("GiftID"), dataRow, rowIdx);	

	data.nItemID[0]= TReadDataValue_Int<GiftDetailDataManager>(TEXT("ItemID1"), dataRow, rowIdx);	
	data.nItemCount[0]= TReadDataValue_Int<GiftDetailDataManager>(TEXT("ItemCount1"), dataRow, rowIdx);	
	data.nItemID[1]= TReadDataValue_Int<GiftDetailDataManager>(TEXT("ItemID2"), dataRow, rowIdx);	
	data.nItemCount[1]= TReadDataValue_Int<GiftDetailDataManager>(TEXT("ItemCount2"), dataRow, rowIdx);	
	data.nItemID[2]= TReadDataValue_Int<GiftDetailDataManager>(TEXT("ItemID3"), dataRow, rowIdx);	
	data.nItemCount[2]= TReadDataValue_Int<GiftDetailDataManager>(TEXT("ItemCount3"), dataRow, rowIdx);	
	data.nMoneyType[0]= TReadDataValue_Int<GiftDetailDataManager>(TEXT("MoneyType1"), dataRow, rowIdx);	
	data.nMoneyNum[0]= TReadDataValue_Int<GiftDetailDataManager>(TEXT("MoneyNum1"), dataRow, rowIdx);	
	data.nMoneyType[1]= TReadDataValue_Int<GiftDetailDataManager>(TEXT("MoneyType2"), dataRow, rowIdx);	
	data.nMoneyNum[1]= TReadDataValue_Int<GiftDetailDataManager>(TEXT("MoneyNum2"), dataRow, rowIdx);	
	data.nAddExp= TReadDataValue_Int<GiftDetailDataManager>(TEXT("AddExp"), dataRow, rowIdx);	
	data.strGiftName= TReadDataValue_FString<GiftDetailDataManager>(TEXT("GiftName"), dataRow, rowIdx);	
	data.strGiftTips = TReadDataValue_FString<GiftDetailDataManager>(TEXT("GiftTips"), dataRow, rowIdx);	

	MasterStudentManager::getSingleton().AddGiftDetailConfigData(data);
}

void GiftDetailDataProcessor::PostInitData()
{

}

MailTemplateData* MailTemplateDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void MailTemplateDataProcessor::DestroyDataObject(MailTemplateData* obj)
{
	if(obj)
	{
		delete obj;
		obj = NULL;
	}
}

void MailTemplateDataProcessor::ParseData(MailTemplateData* obj, class CTabFile* dataRow, int rowIdx)
{
	MailTemplateData data;
	data.nMailID							= TReadDataValue_Int<MailTemplateDataManager>(TEXT("TemplateID"), dataRow, rowIdx);	
	if(data.nMailID <= 0)
		return;
	data.nItemID							= TReadDataValue_Int<MailTemplateDataManager>(TEXT("ItemID"), dataRow, rowIdx);

	MasterStudentManager::getSingleton().AddMailTemplateConfigData(data);
}

void MailTemplateDataProcessor::PostInitData()
{
}

INT MasterStudentManager::GetCongratulationLevel(INT lastLeve, INT currentLevel)
{
	for(INT i = lastLeve+1; i <= currentLevel; i++)
	{
		if(GetGiftConfigData(i) != NULL)
			return i;
	}

	return -1;
}

INT MasterStudentManager::GetCongratulationCount(INT lastLeve, INT currentLevel)
{
	INT count = 0;

	for(INT i = lastLeve+1; i <= currentLevel; i++)
	{
		if(GetGiftConfigData(i) != NULL)
			count++;
	}

	return count;
}
void MasterStudentManager::NotifyUpdateMasterTwitter(UINT8 twitter)
{
	masterTwitter = twitter;
	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterTwitterType);
	MMOUI::PostGameEvent(&evt);
}

void MasterStudentManager::NotifyUpdateMasterReward(G2C_SYNC_MASTER_REWARD_EXP* msg)
{
	if(msg == NULL)
		return;
	if(GetMasterStudentState() != E_MASTER_STUDENT && GetMasterStudentState() != E_MASTER_NONE)
	{
		for(INT i = 0; i < MAX_STUDENT_NUM; i++)
		{
			Teacher_RewardExp reward;
			reward.nRoleID = msg->TeacherRewardExp[i].dwRoleID;
			reward.nExp = msg->TeacherRewardExp[i].nExp;
			reward.nMasterExp = msg->TeacherRewardExp[i].nMasterExp;
			reward.nMoney = msg->TeacherRewardExp[i].nMoney;
			reward.bSelfReward = FALSE;

			if (reward.nRoleID <= 0)
				continue;

			AddOrUpdateMasterRewardExp(reward);
		}
	}

	if (msg->dwMasterID != 0)
	{
		Teacher_RewardExp rewardMaster;
		rewardMaster.nExp = msg->nStudentRewardExp;
		rewardMaster.nMoney = msg->nStudentRewardMoney;
		rewardMaster.nRoleID = msg->dwMasterID;
		rewardMaster.bSelfReward = TRUE;

		if (GetMasterStudentState() == E_MASTER_STUDENT && msg->dwMasterID == masterInfo.nRoleID)
		{
			AddOrUpdateMasterRewardExp(rewardMaster,E_MASTER_TEACHER);
		}
		else if(rewardMaster.nExp > 0 || rewardMaster.nMoney > 0)
		{
			if (GetMasterStudentState() == E_MASTER_NONE)
			{
				AddOrUpdateMasterRewardExp(rewardMaster,E_MASTER_NONE);
			}
			else
			{
				AddOrUpdateMasterRewardExp(rewardMaster);
			}
			
		}
		
	}

	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterInfo);
	MMOUI::PostGameEvent(&evt);
	
}

void MasterStudentManager::AddOrUpdateMasterRewardExp(Teacher_RewardExp& reward,EMASTER_STATE state)
{
	StudentListData* pkData = _GetStudentDataByID(reward.nRoleID);
	PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(reward.nRoleID, TRUE);

	if(state == E_MASTER_STUDENT || state == E_MASTER_NONE)
	{
		if(pkData)
		{
			pkData->reward.nExp = reward.nExp;
			pkData->reward.nMoney = reward.nMoney;
			pkData->reward.nMasterExp = reward.nMasterExp;
			if (pkData->bLeaveMaster == FALSE)
			{
				pkData->bLeaveMaster = state == E_MASTER_NONE ? TRUE : FALSE;
			}
			
			if (reward.bSelfReward)
			{
				pkData->bLeaveMaster = TRUE;
			}
			pkData->reward.bSelfReward = reward.bSelfReward;

		}
		else
		{
			StudentListData data;
			data.nRoleID = reward.nRoleID;
			data.reward.nExp = reward.nExp;
			data.reward.nMoney = reward.nMoney;
			data.reward.nMasterExp = reward.nMasterExp;
			data.nLevel = 0;
			data.bLeaveMaster = TRUE;
			data.reward.bSelfReward = reward.bSelfReward;

			if(playerInfo)
			{
				data.RoleName = playerInfo->_RoleName;
				//data.nLevel   =  pkInfo->byLevel;
				data.nClass   = playerInfo->_RoleClass;
			}
			else
			{
				data.RoleName = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UnKnowRole"));
				//data.nLevel   = pkInfo->byLevel;
				data.nClass   = EPCT_PlayerClassA;
			}
			listStudent.push_back(data);
		}
	}
	else
	{
		masterInfo.reward.nExp = reward.nExp;
		masterInfo.reward.nMoney = reward.nMoney;
	}
}

INT32 MasterStudentManager::GetMasterExpByRatio(INT32 masterExp)
{
	INT32 nMasterExp = masterExp;
	INT ratio = GetMasterExpRatio();

	if(nMasterExp > 0 && ratio > 0)
	{
		nMasterExp = nMasterExp / ratio;
	}
	return nMasterExp;
}

void  MasterStudentManager::NotifyRewardAck(UINT8 success)
{
	if(GetMasterStudentState() == E_MASTER_NONE )
	{
		ClearStudentList();
		MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_MasterInfo);
		MMOUI::PostGameEvent(&evt);

	}
	else
	{
		CMessageHandler::GetInstance().Send_GetMasterInfo();
	}
}

void MasterStudentManager::OpenHelloStudentPanel()
{
	//MasterStudentManager::getSingleton().SetIsShowTwitterMasterInfo(TRUE);
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MasterInvite,TRUE,"");
	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_HelloStudent);
	MMOUI::PostGameEvent(&evt);
}

void MasterStudentManager::OpenHelloTeacherPanel()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MasterTips,TRUE,"");
	MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_HelloTeacher);
	MMOUI::PostGameEvent(&evt);
}

INT MasterStudentManager::GetMasterStudentNum(INT32 masterExp )
{
	INT nMaxStudent = 3;

	if(masterExp < masterExp1)
		nMaxStudent = 3;
	else if(masterExp < masterExp2)
		nMaxStudent = 4;
	else
		nMaxStudent = 5;

	return nMaxStudent;
}

void MasterStudentManager::ClearStudentList()
{
	listStudent.clear();
}

void MasterStudentManager::ClearStudentTwitterInfo(UINT32 id)
{
	if(id == studentTwitterInfo.roleID)
	{
		studentTwitterInfo.roleID = 0;
		studentTwitterInfo.level = 0;
		studentTwitterInfo.masterExp = 0;
		studentTwitterInfo.bApply = TRUE;;
	}
	
}

void MasterStudentManager::UpdateSTudentAppleState(UINT32 id)
{
	SystemPosterListData* pkData = _GetSystemPosterDataByID(id);
	if (pkData)
	{
		pkData->bCanApply = false;

		MMOGameEvent_MasterStudent evt(EMasterStudentType::EMST_StudentSystemPosterList);
		MMOUI::PostGameEvent(&evt);
	}
	
	studentTwitterInfo.bApply = FALSE;
}
