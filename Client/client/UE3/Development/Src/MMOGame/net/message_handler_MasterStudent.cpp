#include "stdafx.h"
#include "message_handler.h"
#include "MasterStudentManager.h"
#include "MMOFacade.h"
#include "MMOAchievementManager.h"
#include "MMOQuestManager.h"
#include "MMOChatManager.h"
#include "MMOClientVariableManager.h"

using namespace ExternalMessage;


void CMessageHandler::Send_MasterListAck()
{
	C2M_MASTER_GET_RECRUIT_LIST msg;
	msg.wMsg = c2m_master_get_recruit_list; 
	_Send(&msg, sizeof(msg));  
}
void CMessageHandler::Send_SoloStudentListAck()
{
	C2M_MASTER_GET_SOLO_LIST msg;
	msg.wMsg = c2m_master_get_solo_list; 
	_Send(&msg, sizeof(msg));  
}
void CMessageHandler::Send_ApplyRecruitStudent()
{
	/*C2M_MASTER_APPLY_RECRUIT msg; 
	msg.wMsg = c2m_master_apply_recruit; 
	_Send(&msg, sizeof(msg));  */
}
void CMessageHandler::Send_CancelRecruitStudent()
{
	/*C2M_MASTER_CANCEL_RECRUIT msg; 
	msg.wMsg = c2m_master_cancel_recruit; 
	_Send(&msg, sizeof(msg)); */ 
}

void CMessageHandler::Send_SubmitApplyToMaster( INT RoleId)
{
	C2M_MASTER_STUD_SUBMIT_APP msg; 
	msg.wMsg = c2m_master_stud_submit_app;
	msg.dwTeacherID = RoleId; 
	_Send(&msg, sizeof(msg));  

	// Guide quest
	MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_Master);
}
void CMessageHandler::Send_AgreeStudentdApply( INT RoleId)
{
	C2M_MASTER_AGREE_STUD_APP msg; 
	msg.wMsg = c2m_master_agree_stud_app;
	msg.dwStudentID = RoleId; 
	_Send(&msg, sizeof(msg));  
}
void CMessageHandler::Send_RefuseStudentApply( INT RoleId)
{
	C2M_MASTER_REFUSE_STUD_APP msg; 
	msg.wMsg = c2m_master_refuse_stud_app;
	msg.dwStudentID = RoleId; 
	_Send(&msg, sizeof(msg));  
}
void CMessageHandler::Send_CancelStudentApplyBySelf( INT RoleId)
{
	C2M_MASTER_CANCEL_STUD_APP msg; 
	msg.wMsg = c2m_master_cancel_stud_app;
	msg.dwTeacherID = RoleId; 
	_Send(&msg, sizeof(msg));  
}

void CMessageHandler::Send_SubmitApplyToStudent( INT RoleId ,UINT8 twitter )
{
	C2M_MASTER_TEACHER_SUBMIT_APP msg; 
	msg.wMsg = c2m_master_teacher_submit_app;
	msg.dwStudentID = RoleId; 
	msg.bySourceType = twitter;
	_Send(&msg, sizeof(msg)); 
}
void CMessageHandler::Send_AgreeMasterdApply( INT RoleId)
{
	C2M_MASTER_AGREE_TEACHER_APP msg; 
	msg.wMsg = c2m_master_agree_teacher_app;
	msg.dwTeacherID = RoleId; 
	_Send(&msg, sizeof(msg)); 
}
void CMessageHandler::Send_RefuseMasterApply( INT RoleId)
{
	C2M_MASTER_REFUSE_TEACHER_APP msg; 
	msg.wMsg = c2m_master_refuse_teacher_app;
	msg.dwTeacherID = RoleId; 
	_Send(&msg, sizeof(msg)); 
}
void CMessageHandler::Send_CancelMasterApplyBySelf( INT RoleId)
{
	C2M_MASTER_CANCEL_TEACHER_APP msg; 
	msg.wMsg = c2m_master_cancel_teacher_app;
	msg.dwStudentID = RoleId; 
	_Send(&msg, sizeof(msg)); 
}

void CMessageHandler::Send_LeaveMaster()
{
	MasterStudentManager::getSingleton().SetLeaveTeacher();

	C2M_MASTER_LEAVE_TEACHER msg; 
	msg.wMsg = c2m_master_leave_teacher; 
	_Send(&msg, sizeof(msg));  

}
void CMessageHandler::Send_FireStudent( INT RoleId  )
{
	C2M_MASTER_FIRE_STUD msg; 
	msg.wMsg = c2m_master_fire_stud;
	msg.dwStudentID = RoleId; 
	_Send(&msg, sizeof(msg));  
}
void CMessageHandler::Send_GetMasterInfo()
{
	C2M_MASTER_INFO msg; 
	msg.wMsg = c2m_master_info;
	_Send(&msg, sizeof(msg));  
}
void CMessageHandler::Send_Congratulation(INT nRoleID,INT nTargetID,INT nConType,FString strCon,INT level)
{
	
	FTCHARToUTF8 szContent( *strCon);

	INT contentLen = szContent.Length()+1;
	if( contentLen >  MAX_CZ_TEXT_LENGTH )
		return;

	C2M_MASTER_CZ* msg = (C2M_MASTER_CZ*)ms_MsgBuffer;
	msg->wMsg = c2m_master_cz;
	msg->byCZType	 = nConType;
	msg->dwTargetID  = nTargetID;
	msg->dwRoleID    = nRoleID;
	msg->byCZLevel = level;
	strxcpy(msg->szText, szContent, contentLen);
	msg->wSize = sizeof(C2M_MASTER_CZ) + contentLen;

	_Send(msg, msg->wSize);
}

void CMessageHandler::Send_DesireGrad()
{
	/*C2M_MASTER_DESIRE_GRAD msg;
	msg.wMsg = c2m_master_desire_grad;
	_Send(&msg, sizeof(msg)); */ 
}
void CMessageHandler::Send_AgreeGrad(INT nRoleID)
{
	/*C2M_MASTER_AGREE_GRAD msg;
	msg.wMsg = c2m_master_agree_grad;
	msg.dwStudentID = nRoleID;
	_Send(&msg, sizeof(msg));  */
}
void CMessageHandler::Send_ForceGrad()
{
	MasterStudentManager::getSingleton().SetLeaveTeacher();

	C2M_MASTER_GRAD msg;
	msg.wMsg = c2m_master_grad;
	_Send(&msg, sizeof(msg));  
}

void CMessageHandler::Send_Ignore(UINT32 nRoleId)
{
	C2M_MASTER_TEACHER_IGNORE msg;
	msg.wMsg = c2m_master_teacher_ignore;
	msg.dwRoleID = nRoleId;
	_Send(&msg,sizeof(msg));
}

void CMessageHandler::Send_Twitter(UINT8 twitter)
{
	C2G_MASTER_TWITTER msg;
	msg.wMsg = c2g_master_twitter;
	msg.byTwitter = twitter;
	_Send(&msg,sizeof(msg));
}

//Experience claim
void CMessageHandler::Send_QueryMasterExp()
{
	C2G_QUERY_MASTER_REWARD_EXP msg;
	msg.wMsg =c2g_query_master_reward_exp;
	_Send(&msg,sizeof(msg));
}
void CMessageHandler::Send_ReciveMasterExp(UINT32 roleId)
{
	C2G_RECEIVE_MASTER_REWARD_EXP msg;
	msg.wMsg = c2g_receive_master_reward_exp;
	msg.dwRoleID = roleId;
	_Send(&msg,sizeof(msg));
}

/*
	List refresh data
*/
void CMessageHandler::Parse_g2c_sync_master(const char* buf, UINT32 size)
{
	G2C_SYNC_MASTER* msg = (G2C_SYNC_MASTER*)buf;
	if(msg == NULL)
		return;
	//if(msg->dwCD > 0)
	MasterStudentManager::getSingleton().NotifyCDTimeChange(msg->dwCD);
	MasterStudentManager::getSingleton().SetMasterStudentState((EMASTER_STATE)msg->byState);
	MasterStudentManager::getSingleton().NotifyUpdateMasterTwitter(msg->byTwitter);
}
void CMessageHandler::Parse_m2c_master_recruit_list(const char* buf, UINT32 size)
{
	M2C_MASTER_RECRUIT_LIST* msg = (M2C_MASTER_RECRUIT_LIST*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyUpdateMasterList(msg);
}
void CMessageHandler::Parse_m2c_master_get_solo_list_ack(const char* buf, UINT32 size)
{
	M2C_MASTER_GET_SOLO_LIST_ACK* msg = (M2C_MASTER_GET_SOLO_LIST_ACK*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyUpdateMasterSoloListAck(msg);
}/*
void CMessageHandler::Parse_m2c_master_recruit_ack(const char* buf, UINT32 size)
{
	M2C_MASTER_RECRUIT_ACK* msg =(M2C_MASTER_RECRUIT_ACK*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyUpdateRecruitAck(msg->dwType);
}*/

void CMessageHandler::Parse_m2c_master_sync_lvl(const char* buf, UINT32 size)
{
	M2C_MASTER_SYNC_LVL* msg =(M2C_MASTER_SYNC_LVL*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyLevelUp(msg->dwRoleID,msg->byLevel);
}

//void CMessageHandler::Parse_m2c_master_desire_grad_ack(const char* buf, UINT32 size)
//{
//	M2C_MASTER_DESIRE_GRAD_ACK* msg =(M2C_MASTER_DESIRE_GRAD_ACK*)buf;
//	if(msg == NULL)
//		return;
//	MasterStudentManager::getSingleton().NotifyDesireGradAck();
//}
//void CMessageHandler::Parse_m2c_master_desire_grad_ntf(const char* buf, UINT32 size)
//{
//	M2C_MASTER_DESIRE_GRAD_NTF* msg =(M2C_MASTER_DESIRE_GRAD_NTF*)buf;
//	if(msg == NULL)
//		return;
//	MasterStudentManager::getSingleton().NotifyDesireGradNtf(msg->dwStudentID);
//}
void CMessageHandler::Parse_m2c_master_grad(const char* buf, UINT32 size)
{
	M2C_MASTER_GRAD* msg =(M2C_MASTER_GRAD*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyStudentGrad(msg->dwStudentID);
}
/*
	Apprentice-initiated master request related messages
*/
void CMessageHandler::Parse_m2c_master_stud_submit_app_ack(const char* buf, UINT32 size)
{
	M2C_MASTER_STUD_SUBMIT_APP_ACK* msg =(M2C_MASTER_STUD_SUBMIT_APP_ACK*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifySubmitStudentApplyAck(msg->dwMasterID);
}
void CMessageHandler::Parse_m2c_master_stud_submit_app_ntf(const char* buf, UINT32 size)
{
	M2C_MASTER_STUD_SUBMIT_APP_NTF* msg =(M2C_MASTER_STUD_SUBMIT_APP_NTF*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifySubmitStudentApplyNtf(msg);
}
void CMessageHandler::Parse_m2c_master_agree_stud_app_ack(const char* buf, UINT32 size)
{
	M2C_MASTER_AGREE_STUD_APP_ACK* msg =(M2C_MASTER_AGREE_STUD_APP_ACK*)buf; 
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyAgreeStudentApplyAck(msg->dwStudentID);
}
void CMessageHandler::Parse_m2c_master_refuse_stud_app_ack(const char* buf, UINT32 size)
{
	M2C_MASTER_REFUSE_STUD_APP_ACK* msg =(M2C_MASTER_REFUSE_STUD_APP_ACK*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyRefuseStudentApplyAck(msg->dwStudentID);
}
void CMessageHandler::Parse_m2c_master_refuse_stud_app_ntf(const char* buf, UINT32 size)
{
	M2C_MASTER_REFUSE_STUD_APP_NTF* msg =(M2C_MASTER_REFUSE_STUD_APP_NTF*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyRefuseStudentApplyNtf(msg->dwTeacherID);
}
void CMessageHandler::Parse_m2c_master_cancel_stud_app_ack(const char* buf, UINT32 size)
{
	M2C_MASTER_CANCEL_STUD_APP_ACK* msg =(M2C_MASTER_CANCEL_STUD_APP_ACK*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyCacelStudentApplyAck(msg->dwTeacherID);
}
void CMessageHandler::Parse_m2c_master_cancel_stud_app_ntf(const char* buf, UINT32 size)
{
	M2C_MASTER_CANCEL_STUD_APP_NTF* msg =(M2C_MASTER_CANCEL_STUD_APP_NTF*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyCacelStudentApplyNtf(msg->dwStudentID);
}

/*
	Master-initiated apprentice recruitment related messages
*/
void CMessageHandler::Parse_m2c_master_teacher_submit_app_ack(const char* buf, UINT32 size)
{
	M2C_MASTER_TEACHER_SUBMIT_APP_ACK* msg =(M2C_MASTER_TEACHER_SUBMIT_APP_ACK*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifySubmitMasterApplyAck(msg->dwStudentID);
}
void CMessageHandler::Parse_m2c_master_teacher_submit_app_ntf(const char* buf, UINT32 size)
{
	M2C_MASTER_TEACHER_SUBMIT_APP_NTF* msg =(M2C_MASTER_TEACHER_SUBMIT_APP_NTF*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifySubmitMasterApplyNtf(msg);
}
void CMessageHandler::Parse_m2c_master_agree_teacher_app_ack(const char* buf, UINT32 size)
{
	M2C_MASTER_AGREE_TEACHER_APP_ACK* msg =(M2C_MASTER_AGREE_TEACHER_APP_ACK*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyAgreeMasterApplyAck(msg->dwTeacherID);
}
void CMessageHandler::Parse_m2c_master_refuse_teacher_app_ack(const char* buf, UINT32 size)
{
	M2C_MASTER_REFUSE_TEACHER_APP_ACK* msg =(M2C_MASTER_REFUSE_TEACHER_APP_ACK*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyRefuseMasterApplyAck(msg->dwTeacherID);
}
void CMessageHandler::Parse_m2c_master_refuse_teacher_app_ntf(const char* buf, UINT32 size)
{
	M2C_MASTER_REFUSE_TEACHER_APP_NTF* msg =(M2C_MASTER_REFUSE_TEACHER_APP_NTF*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyRefuseMasterApplyNtf(msg->dwStudentID);
}
void CMessageHandler::Parse_m2c_master_cancel_teacher_app_ack(const char* buf, UINT32 size)
{
	M2C_MASTER_CANCEL_TEACHER_APP_ACK* msg =(M2C_MASTER_CANCEL_TEACHER_APP_ACK*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyCacelMasterApplyAck(msg->dwStudentID);
}
void CMessageHandler::Parse_m2c_master_cancel_teacher_app_ntf(const char* buf, UINT32 size)
{
	M2C_MASTER_CANCEL_TEACHER_APP_NTF* msg =(M2C_MASTER_CANCEL_TEACHER_APP_NTF*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyCacelMasterApplyNtf(msg->dwTeacherID);
}

/*
	Master/apprentice sect internal related messages
*/
void CMessageHandler::Parse_m2c_master_leave_teacher_ack(const char* buf, UINT32 size)
{
	M2C_MASTER_LEAVE_TEACHER_ACK* msg =(M2C_MASTER_LEAVE_TEACHER_ACK*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyLeaveMasterAck();
}
void CMessageHandler::Parse_m2c_master_leave_teacher_ntf(const char* buf, UINT32 size)
{
	M2C_MASTER_LEAVE_TEACHER_NTF* msg =(M2C_MASTER_LEAVE_TEACHER_NTF*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyLeaveMasterNtf(msg->dwStudentID);
}
void CMessageHandler::Parse_m2c_master_fire_student_ack(const char* buf, UINT32 size)
{
	M2C_MASTER_FIRE_STUDENT_ACK* msg =(M2C_MASTER_FIRE_STUDENT_ACK*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyFireStudentAck(msg->dwStudentID);
}
void CMessageHandler::Parse_m2c_master_fire_student_ntf(const char* buf, UINT32 size)
{
	M2C_MASTER_FIRE_STUDENT_NTF* msg =(M2C_MASTER_FIRE_STUDENT_NTF*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyFireStudentNtf(msg->dwStudentID);
}
void CMessageHandler::Parse_m2c_master_add_student_ntf(const char* buf, UINT32 size)
{
	M2C_MASTER_ADD_STUDENT_NTF* msg =(M2C_MASTER_ADD_STUDENT_NTF*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyAddStudentNtf(msg);
}
void CMessageHandler::Parse_m2c_master_del_student_ntf(const char* buf, UINT32 size)
{
	M2C_MASTER_DEL_STUDENT_NTF* msg =(M2C_MASTER_DEL_STUDENT_NTF*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyDelStudentNtf(msg->dwStudentID);
}
void CMessageHandler::Parse_m2c_master_clear_info(const char* buf, UINT32 size)
{
	M2C_MASTER_CLEAR_INFO* msg =(M2C_MASTER_CLEAR_INFO*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyClearInfo();
}
void CMessageHandler::Parse_m2c_master_info(const char* buf, UINT32 size)
{
	M2C_MASTER_INFO* msg =(M2C_MASTER_INFO*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyMasterInfo(msg);
}
void CMessageHandler::Parse_m2c_master_cz_level(const char* buf, UINT32 size)
{
	M2C_MASTER_CZ_LEVEL* msg =(M2C_MASTER_CZ_LEVEL*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyCongratulationCount(msg->dwRoleID,msg->byCZLevel);
}
void CMessageHandler::Parse_m2c_master_cz_ack(const char* buf, UINT32 size)
{
	M2C_MASTER_CZ_ACK* msg =(M2C_MASTER_CZ_ACK*)buf;
	if(msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyCongratulationTarget(msg->dwTargetRoleID);
}

void CMessageHandler::Parse_m2c_master_hello_teacher(const char* buf, UINT32 size)
{
	M2C_MASTER_HELLO_TEACHER* msg  = (M2C_MASTER_HELLO_TEACHER*)buf;
	if (msg == NULL)
		return;
	MasterStudentManager::getSingleton().SetIsShowTwitterStudentInfo(TRUE);
	MasterStudentManager::getSingleton().NotifyHelloTeacher(msg->dwRoleID,msg->byLevel);
}

void CMessageHandler::Parse_g2c_sync_master_reward_exp(const char* buf, UINT32 size)
{
	G2C_SYNC_MASTER_REWARD_EXP* msg = (G2C_SYNC_MASTER_REWARD_EXP*)buf;
	if (msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyUpdateMasterReward(msg);
}

void CMessageHandler::Parse_m2c_master_twitter_ack(const char* buf, UINT32 size)
{
	M2C_MASTER_TWITTER_ACK* msg = (M2C_MASTER_TWITTER_ACK*)buf;
	if (msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyUpdateMasterTwitter(msg->byTwitter);
}
void CMessageHandler::Parse_g2c_receive_master_reward_exp_ack(const char* buf, UINT32 size)
{
	G2C_RECEIVE_MASTER_REWARD_EXP_ACK* msg = (G2C_RECEIVE_MASTER_REWARD_EXP_ACK*)buf;
	if (msg == NULL)
		return;
	MasterStudentManager::getSingleton().NotifyRewardAck(msg->bySuccess);
}

