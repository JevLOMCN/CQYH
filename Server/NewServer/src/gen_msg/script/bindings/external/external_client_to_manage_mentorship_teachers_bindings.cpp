#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2m_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_manage_mentorship_teachers_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2m_master_teacher_ignore_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MASTER_TEACHER_IGNORE>(L, external_message_namespace, "C2M_MASTER_TEACHER_IGNORE", "ExternalMessage::C2M_MASTER_TEACHER_IGNORE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_master_teacher_ignore_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MASTER_TEACHER_IGNORE(); });
    external_message_c2m_master_teacher_ignore_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MASTER_TEACHER_IGNORE>(new ExternalMessage::C2M_MASTER_TEACHER_IGNORE()); });
    external_message_c2m_master_teacher_ignore_type["dwRoleID"] = sol::property([](const ExternalMessage::C2M_MASTER_TEACHER_IGNORE& self) { return self.dwRoleID; }, [](ExternalMessage::C2M_MASTER_TEACHER_IGNORE& self, decltype(ExternalMessage::C2M_MASTER_TEACHER_IGNORE::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2m_master_teacher_submit_app_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MASTER_TEACHER_SUBMIT_APP>(L, external_message_namespace, "C2M_MASTER_TEACHER_SUBMIT_APP", "ExternalMessage::C2M_MASTER_TEACHER_SUBMIT_APP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_master_teacher_submit_app_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MASTER_TEACHER_SUBMIT_APP(); });
    external_message_c2m_master_teacher_submit_app_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MASTER_TEACHER_SUBMIT_APP>(new ExternalMessage::C2M_MASTER_TEACHER_SUBMIT_APP()); });
    external_message_c2m_master_teacher_submit_app_type["bySourceType"] = sol::property([](const ExternalMessage::C2M_MASTER_TEACHER_SUBMIT_APP& self) { return self.bySourceType; }, [](ExternalMessage::C2M_MASTER_TEACHER_SUBMIT_APP& self, decltype(ExternalMessage::C2M_MASTER_TEACHER_SUBMIT_APP::bySourceType) value) { self.bySourceType = value; });
    external_message_c2m_master_teacher_submit_app_type["dwStudentID"] = sol::property([](const ExternalMessage::C2M_MASTER_TEACHER_SUBMIT_APP& self) { return self.dwStudentID; }, [](ExternalMessage::C2M_MASTER_TEACHER_SUBMIT_APP& self, decltype(ExternalMessage::C2M_MASTER_TEACHER_SUBMIT_APP::dwStudentID) value) { self.dwStudentID = value; });
    auto external_message_c2m_master_agree_teacher_app_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MASTER_AGREE_TEACHER_APP>(L, external_message_namespace, "C2M_MASTER_AGREE_TEACHER_APP", "ExternalMessage::C2M_MASTER_AGREE_TEACHER_APP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_master_agree_teacher_app_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MASTER_AGREE_TEACHER_APP(); });
    external_message_c2m_master_agree_teacher_app_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MASTER_AGREE_TEACHER_APP>(new ExternalMessage::C2M_MASTER_AGREE_TEACHER_APP()); });
    external_message_c2m_master_agree_teacher_app_type["dwTeacherID"] = sol::property([](const ExternalMessage::C2M_MASTER_AGREE_TEACHER_APP& self) { return self.dwTeacherID; }, [](ExternalMessage::C2M_MASTER_AGREE_TEACHER_APP& self, decltype(ExternalMessage::C2M_MASTER_AGREE_TEACHER_APP::dwTeacherID) value) { self.dwTeacherID = value; });
    auto external_message_c2m_master_refuse_teacher_app_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MASTER_REFUSE_TEACHER_APP>(L, external_message_namespace, "C2M_MASTER_REFUSE_TEACHER_APP", "ExternalMessage::C2M_MASTER_REFUSE_TEACHER_APP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_master_refuse_teacher_app_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MASTER_REFUSE_TEACHER_APP(); });
    external_message_c2m_master_refuse_teacher_app_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MASTER_REFUSE_TEACHER_APP>(new ExternalMessage::C2M_MASTER_REFUSE_TEACHER_APP()); });
    external_message_c2m_master_refuse_teacher_app_type["dwTeacherID"] = sol::property([](const ExternalMessage::C2M_MASTER_REFUSE_TEACHER_APP& self) { return self.dwTeacherID; }, [](ExternalMessage::C2M_MASTER_REFUSE_TEACHER_APP& self, decltype(ExternalMessage::C2M_MASTER_REFUSE_TEACHER_APP::dwTeacherID) value) { self.dwTeacherID = value; });
    auto external_message_c2m_master_cancel_teacher_app_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MASTER_CANCEL_TEACHER_APP>(L, external_message_namespace, "C2M_MASTER_CANCEL_TEACHER_APP", "ExternalMessage::C2M_MASTER_CANCEL_TEACHER_APP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_master_cancel_teacher_app_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MASTER_CANCEL_TEACHER_APP(); });
    external_message_c2m_master_cancel_teacher_app_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MASTER_CANCEL_TEACHER_APP>(new ExternalMessage::C2M_MASTER_CANCEL_TEACHER_APP()); });
    external_message_c2m_master_cancel_teacher_app_type["dwStudentID"] = sol::property([](const ExternalMessage::C2M_MASTER_CANCEL_TEACHER_APP& self) { return self.dwStudentID; }, [](ExternalMessage::C2M_MASTER_CANCEL_TEACHER_APP& self, decltype(ExternalMessage::C2M_MASTER_CANCEL_TEACHER_APP::dwStudentID) value) { self.dwStudentID = value; });
    auto external_message_c2m_master_leave_teacher_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MASTER_LEAVE_TEACHER>(L, external_message_namespace, "C2M_MASTER_LEAVE_TEACHER", "ExternalMessage::C2M_MASTER_LEAVE_TEACHER", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_master_leave_teacher_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MASTER_LEAVE_TEACHER(); });
    external_message_c2m_master_leave_teacher_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MASTER_LEAVE_TEACHER>(new ExternalMessage::C2M_MASTER_LEAVE_TEACHER()); });
}

}
