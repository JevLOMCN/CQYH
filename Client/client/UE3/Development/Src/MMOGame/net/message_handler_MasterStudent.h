#ifndef __MESSAGE_HANDLER_MasterStudent_H__
#define __MESSAGE_HANDLER_MasterStudent_H__
public:
	/*
		Request list of players who can be master
		Request list of players who can be apprentice
		Master requests to enter apprentice recruiting state
		Master requests to exit apprentice recruiting state
	*/
	void Send_MasterListAck();
	void Send_SoloStudentListAck();
	void Send_ApplyRecruitStudent();
	void Send_CancelRecruitStudent();

	/*
		Apprentice requests to take a master
		Master agrees
		Master refuses
		Apprentice cancels master request by self
	*/
	void Send_SubmitApplyToMaster( INT RoleId  ); 
	void Send_AgreeStudentdApply( INT RoleId  ); 
	void Send_RefuseStudentApply( INT RoleId  ); 
	void Send_CancelStudentApplyBySelf( INT RoleId  ); 
	
	/*
		Master requests to recruit this apprentice
		Apprentice agrees
		Apprentice refuses
		Master cancels recruiting this apprentice by self
	*/
	void Send_SubmitApplyToStudent( INT RoleId ,UINT8 twitter = 0 ); 
	void Send_AgreeMasterdApply( INT RoleId  ); 
	void Send_RefuseMasterApply( INT RoleId  ); 
	void Send_CancelMasterApplyBySelf( INT RoleId  ); 
	
	/*
		Betray the sect
		Expel apprentice
		Request master info
		Send congratulation info
	*/
	void Send_LeaveMaster(); 
	void Send_FireStudent( INT RoleId  ); 
	void Send_GetMasterInfo();
	void Send_Congratulation(INT nRoleID,INT nTargetID,INT nConType,FString strCon,INT level);


	void Send_DesireGrad();
	void Send_AgreeGrad(INT nRoleID);
	void Send_ForceGrad();
	

	//Push related
	void Send_Ignore(UINT32 nRoleId);
	void Send_Twitter(UINT8 twitter);

	//Master/apprentice experience
	void Send_QueryMasterExp();
	void Send_ReciveMasterExp(UINT32 roleId);

private:
	M_DECL_PARSE_FUN(g2c_sync_master);
	M_DECL_PARSE_FUN(m2c_master_sync_lvl);
	M_DECL_PARSE_FUN(m2c_master_recruit_list);
	M_DECL_PARSE_FUN(m2c_master_get_solo_list_ack);
	//M_DECL_PARSE_FUN(m2c_master_recruit_ack);

	/* Apprentice master-request series message responses*/
	M_DECL_PARSE_FUN(m2c_master_stud_submit_app_ack);
	M_DECL_PARSE_FUN(m2c_master_stud_submit_app_ntf);
	M_DECL_PARSE_FUN(m2c_master_agree_stud_app_ack); 
	M_DECL_PARSE_FUN(m2c_master_refuse_stud_app_ack);
	M_DECL_PARSE_FUN(m2c_master_refuse_stud_app_ntf);
	M_DECL_PARSE_FUN(m2c_master_cancel_stud_app_ack);
	M_DECL_PARSE_FUN(m2c_master_cancel_stud_app_ntf);

	/* Master apprentice-recruit series message responses*/
	M_DECL_PARSE_FUN(m2c_master_teacher_submit_app_ack);
	M_DECL_PARSE_FUN(m2c_master_teacher_submit_app_ntf);
	M_DECL_PARSE_FUN(m2c_master_agree_teacher_app_ack);
	M_DECL_PARSE_FUN(m2c_master_refuse_teacher_app_ack);
	M_DECL_PARSE_FUN(m2c_master_refuse_teacher_app_ntf);
	M_DECL_PARSE_FUN(m2c_master_cancel_teacher_app_ack);
	M_DECL_PARSE_FUN(m2c_master_cancel_teacher_app_ntf);

	/*Sect internal operation message responses*/
	M_DECL_PARSE_FUN(m2c_master_leave_teacher_ack);
	M_DECL_PARSE_FUN(m2c_master_leave_teacher_ntf);
	M_DECL_PARSE_FUN(m2c_master_fire_student_ack);
	M_DECL_PARSE_FUN(m2c_master_fire_student_ntf);
	M_DECL_PARSE_FUN(m2c_master_add_student_ntf);
	M_DECL_PARSE_FUN(m2c_master_del_student_ntf);
	M_DECL_PARSE_FUN(m2c_master_clear_info);
	M_DECL_PARSE_FUN(m2c_master_info);
	M_DECL_PARSE_FUN(m2c_master_cz_level);
	M_DECL_PARSE_FUN(m2c_master_cz_ack);
	//M_DECL_PARSE_FUN(m2c_master_desire_grad_ack);
	//M_DECL_PARSE_FUN(m2c_master_desire_grad_ntf);
	M_DECL_PARSE_FUN(m2c_master_grad);

	M_DECL_PARSE_FUN(m2c_master_hello_teacher);
	M_DECL_PARSE_FUN(g2c_sync_master_reward_exp);
	M_DECL_PARSE_FUN(g2c_receive_master_reward_exp_ack);
	M_DECL_PARSE_FUN(m2c_master_twitter_ack);

#define M_INIT_MSGHANDLER_MASTER\
	MREG_PARSE_FUN(g2c_sync_master);\
	MREG_PARSE_FUN(m2c_master_sync_lvl);\
	MREG_PARSE_FUN(m2c_master_recruit_list);\
	MREG_PARSE_FUN(m2c_master_get_solo_list_ack);\
	MREG_PARSE_FUN(m2c_master_stud_submit_app_ack);\
	MREG_PARSE_FUN(m2c_master_stud_submit_app_ntf);\
	MREG_PARSE_FUN(m2c_master_agree_stud_app_ack);\
	MREG_PARSE_FUN(m2c_master_refuse_stud_app_ack);\
	MREG_PARSE_FUN(m2c_master_refuse_stud_app_ntf);\
	MREG_PARSE_FUN(m2c_master_cancel_stud_app_ack);\
	MREG_PARSE_FUN(m2c_master_cancel_stud_app_ntf);\
	MREG_PARSE_FUN(m2c_master_teacher_submit_app_ack);\
	MREG_PARSE_FUN(m2c_master_teacher_submit_app_ntf);\
	MREG_PARSE_FUN(m2c_master_agree_teacher_app_ack);\
	MREG_PARSE_FUN(m2c_master_refuse_teacher_app_ack);\
	MREG_PARSE_FUN(m2c_master_refuse_teacher_app_ntf);\
	MREG_PARSE_FUN(m2c_master_cancel_teacher_app_ack);\
	MREG_PARSE_FUN(m2c_master_cancel_teacher_app_ntf);\
	MREG_PARSE_FUN(m2c_master_leave_teacher_ack);\
	MREG_PARSE_FUN(m2c_master_leave_teacher_ntf);\
	MREG_PARSE_FUN(m2c_master_fire_student_ack);\
	MREG_PARSE_FUN(m2c_master_fire_student_ntf);\
	MREG_PARSE_FUN(m2c_master_add_student_ntf);\
	MREG_PARSE_FUN(m2c_master_del_student_ntf);\
	MREG_PARSE_FUN(m2c_master_clear_info);\
	MREG_PARSE_FUN(m2c_master_info);\
	MREG_PARSE_FUN(m2c_master_cz_ack);\
	MREG_PARSE_FUN(m2c_master_grad);\
	MREG_PARSE_FUN(m2c_master_cz_level);\
	MREG_PARSE_FUN(m2c_master_hello_teacher);\
	MREG_PARSE_FUN(g2c_sync_master_reward_exp);\
	MREG_PARSE_FUN(g2c_receive_master_reward_exp_ack);\
	MREG_PARSE_FUN(m2c_master_twitter_ack);
#endif
 