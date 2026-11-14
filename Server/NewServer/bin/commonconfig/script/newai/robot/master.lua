
LoadPackage("role.pkg")
LoadPackage("asimov_user.pkg")
LoadPackage("master.pkg")
LoadPackage("character.pkg")


function BeMaster(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetObjID() % 9 ~= 0 then
		return;
	end

	local nRand = math.random(0,10000);
	if nRand > 3000 then
		return;
	end

	ret = user:GMCommand("Level 25");

	-- jump to Bichon Province
	--local ret = user:GMCommand("Jump Bichon Province");

	--if role:GetLevel() > GRADUATE_LEVEL and role:GetMaster():IsTeacher() == false and role:GetMaster():IsStudent() == false then
	--	local ret = user:GMCommand("AcceptQuest(30)");
	--	ret = user:GMCommand("CompleteQuest(30)");
	--end
end



function GetMasterInfo(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetMaster():GetState() ~= MASTER_NONE then
		local ret = user:GetMasterInfo();
	end
end



function GetMasterRecruitList(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetLevel() >= GRADUATE_LEVEL then
		return ;
	end

	if role:GetMaster():IsStudent() == true or role:GetMaster():IsTeacher() == true then
		return;
	end

	if role:GetMaster():GetCD() > 0 then
		return ;
	end


	local ret = user:GetMasterRecruitList();
end


function GetMasterSoloList(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetMaster():IsTeacher() == false then
		return;
	end

	local ret = user:GetMasterSoloList();
end


function ApplyToTeacher(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetMaster():GetState() == MASTER_STUDENT or role:GetMaster():GetState() == MASTER_TEACHER then
		return;
	end

	local RecruitNum = user:LuaMasterGetRecruitNum();
	if RecruitNum == 0 then
		return ;
	end

	local ret = user:LuaMasterIsSendListFull();
	if ret then
		return ;
	end

	local index = math.random(0, RecruitNum);
	local RecruitRoleID = user:LuaMasterGetRecruitRoleByIndex(index);
	if RecruitRoleID == 0 then
		return ;
	end

	ret = user:ApplyTeacher(RecruitRoleID);
end


function TeacherAgreeApp(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetMaster():GetState() ~= MASTER_TEACHER then
		return;
	end

	--local StudNum = role:GetMaster():GetStudentCount();

	local ret = role:GetMaster():IsStudentFull();
	if ret then
		return ;
	end

	local AppNum = user:LuaMasterGetRecvAppNum();
	if AppNum == 0 then
		return ;
	end

	local index = math.random(0, AppNum);
	local StudID = user:LuaMasterGetRecvRoleIDByIndex(index);
	if StudID == 0 then
		return ;
	end

	ret = user:TeacherAgreeApp(StudID);
end


function TeacherRefuseApp(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetMaster():GetState() ~= MASTER_TEACHER then
		return;
	end

	local AppNum = user:LuaMasterGetRecvAppNum();
	if AppNum == 0 then
		return ;
	end

	local index = math.random(0, AppNum);
	local StudID = user:LuaMasterGetRecvRoleIDByIndex(index);
	if StudID == 0 then
		return ;
	end

	local ret = user:TeacherRefuseApp(StudID);
end


function SoloCancelApp(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetMaster():GetState() == MASTER_STUDENT or role:GetMaster():GetState() == MASTER_TEACHER then
		return;
	end

	local AppNum = user:LuaMasterGetSendListNum();
	if AppNum == 0 then
		return ;
	end

	local index = math.random(0, AppNum);
	local TeacherID = user:LuaMasterGetSendRoleIDByIndex(index);
	if TeacherID == 0 then
		return ;
	end

	local ret = user:SoloCancelApp(TeacherID);
end


function ApplyToStudent(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetMaster():GetState() ~= MASTER_TEACHER then
		return;
	end

	local SoloNum = user:LuaMasterGetSoloNum();
	if SoloNum == 0 then
		return ;
	end

	local ret = user:LuaMasterIsSendListFull();
	if ret then
		return ;
	end

	local index = math.random(0, SoloNum);
	local SoloRoleID = user:LuaMasterGetSoloRoleByIndex(index);
	if SoloRoleID == 0 then
		return ;
	end

	ret = user:ApplyToStudent(SoloRoleID);
end


function StudentAgreeApp(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetMaster():GetState() == MASTER_STUDENT or role:GetMaster():GetState() == MASTER_TEACHER then
		return;
	end

	local AppNum = user:LuaMasterGetRecvAppNum();
	if AppNum == 0 then
		return ;
	end

	local index = 0;
	if AppNum == 1 then
		index = 0;
	else
		index = math.random(0, AppNum-1);
	end

	--local index = math.random(0, AppNum-1);

	local TeacherID = user:LuaMasterGetRecvRoleIDByIndex(index);
	if TeacherID == 0 then
		return ;
	end

	local ret = user:SoloAgreeApp(TeacherID);
end


function StudentRefuseApp(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetMaster():GetState() == MASTER_STUDENT or role:GetMaster():GetState() == MASTER_TEACHER then
		return;
	end

	local AppNum = user:LuaMasterGetRecvAppNum();
	if AppNum == 0 then
		return ;
	end

	local index = 0;
	if AppNum == 1 then
		index = 0;
	else
		index = math.random(0, AppNum-1);
	end

	--local index = math.random(0, AppNum);
	local TeacherID = user:LuaMasterGetRecvRoleIDByIndex(index);
	if TeacherID == 0 then
		return ;
	end

	local ret = user:SoloRefuseApp(TeacherID);
end


function TeacherCancelApp(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetMaster():GetState() ~= MASTER_TEACHER then
		return;
	end

	local AppNum = user:LuaMasterGetSendListNum();
	if AppNum == 0 then
		return ;
	end

	local index = 0;
	if AppNum == 1 then
		index = 0;
	else
		index = math.random(0, AppNum-1);
	end

	--local index = math.random(0, AppNum);
	local SoloID = user:LuaMasterGetSendRoleIDByIndex(index);
	if SoloID == 0 then
		return ;
	end

	local ret = user:TeacherCancelApp(SoloID);
end


function LeaveTeacher(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetMaster():GetState() ~= MASTER_STUDENT then
		return;
	end

	local ret = user:LeaveTeacher();
end


function FireStudent(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetMaster():GetState() ~= MASTER_TEACHER then
		return;
	end

	local StudentNum = role:GetMaster():GetStudentCount();
	if StudentNum == 0 then
		return;
	end

	local index = 0;
	if StudentNum == 1 then
		index = 0;
	else
		index = math.random(0, StudentNum - 1);
	end

	local StudentID = role:GetMaster():GetStudentByIndex(index);
	if StudentID == 0 then
		return 0;
	end

	local ret = user:FireStudent(StudentID);
end


function MasterCZ(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetMaster():GetState() == MASTER_NONE then
		return;
	end

	local StudentNum = role:GetMaster():GetStudentCount();
	if StudentNum == 0 then
		return;
	end

	local index  = 0;
	if StudentNum == 1 then
		index = 0;
	else
		index = math.random(0, StudentNum - 1);
	end

	local 	TempLevel = role:GetLevel() + 1;
	user:GMCommand("Level "..TempLevel);

	local CZRoleID = 0;

	local bRet = false;

	if role:GetMaster():IsTeacher() then

		--if index > 0 then
		--	index = index - 1;
		--end

		CZRoleID = role:GetMaster():GetStudentByIndex(index);
		if CZRoleID == 0 or CZRoleID == role:GetObjID() then
			return 0;
		end

		bRet = user:LuaMasterCanCZStudent(CZRoleID);
		if bRet == false then
			return 0;
		end

	else
		--local index = math.random(0, StudentNum);
		--if index > 0 then
		--	index = index - 1;
		--end

		if index == StudentNum then
			CZRoleID = role:GetMaster():GetTeacherID();

			bRet = user:LuaMasterCanCZTeacher();
			if bRet == false then
				return 0;
			end
		else
			CZRoleID = role:GetMaster():GetStudentByIndex(index);
			if CZRoleID == 0 or CZRoleID == role:GetObjID() then
				return 0;
			end

			bRet = user:LuaMasterCanCZStudent(CZRoleID);
			if bRet == false then
				return 0;
			end
		end
	end

	local CZType = math.random(MASTER_CONG_BY_FREE, MASTER_CONG_BY_GOLD);

	local ret = user:MasterCZ(role:GetObjID(), CZRoleID, CZType, "Hello world!");

end


function DesireGraduate(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetMaster():GetState() ~= MASTER_STUDENT then
		return;
	end

	--local ret = user:GMCommand("level "..GRADUATE_LEVEL.." ");

	if role:GetLevel() ~= GRADUATE_LEVEL then
		return;
	end

	local ret = user:DesireGraduate();
end


function AgreeGraduate(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetMaster():GetState() ~= MASTER_TEACHER then
		return;
	end

	local GraduateRoleID = user:LuaMasterGetGraduateRoleID();
	if GraduateRoleID == 0 then
		return ;
	end

	local ret = user:AgreeGraduate(GraduateRoleID);
end


function ForceGraduate(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	if role:GetMaster():GetState() ~= MASTER_STUDENT then
		return;
	end

	--local ret = user:GMCommand("level "..GRADUATE_LEVEL.." ");
	if role:GetLevel() ~= GRADUATE_LEVEL then
		return;
	end

	local ret = user:ForceGraduate();
end










