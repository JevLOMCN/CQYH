#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2m_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_manage_mentorship_students_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2m_master_stud_submit_app_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MASTER_STUD_SUBMIT_APP>(L, external_message_namespace, "C2M_MASTER_STUD_SUBMIT_APP", "ExternalMessage::C2M_MASTER_STUD_SUBMIT_APP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_master_stud_submit_app_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MASTER_STUD_SUBMIT_APP(); });
    external_message_c2m_master_stud_submit_app_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MASTER_STUD_SUBMIT_APP>(new ExternalMessage::C2M_MASTER_STUD_SUBMIT_APP()); });
    external_message_c2m_master_stud_submit_app_type["dwTeacherID"] = sol::property([](const ExternalMessage::C2M_MASTER_STUD_SUBMIT_APP& self) { return self.dwTeacherID; }, [](ExternalMessage::C2M_MASTER_STUD_SUBMIT_APP& self, decltype(ExternalMessage::C2M_MASTER_STUD_SUBMIT_APP::dwTeacherID) value) { self.dwTeacherID = value; });
    auto external_message_c2m_master_agree_stud_app_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MASTER_AGREE_STUD_APP>(L, external_message_namespace, "C2M_MASTER_AGREE_STUD_APP", "ExternalMessage::C2M_MASTER_AGREE_STUD_APP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_master_agree_stud_app_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MASTER_AGREE_STUD_APP(); });
    external_message_c2m_master_agree_stud_app_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MASTER_AGREE_STUD_APP>(new ExternalMessage::C2M_MASTER_AGREE_STUD_APP()); });
    external_message_c2m_master_agree_stud_app_type["dwStudentID"] = sol::property([](const ExternalMessage::C2M_MASTER_AGREE_STUD_APP& self) { return self.dwStudentID; }, [](ExternalMessage::C2M_MASTER_AGREE_STUD_APP& self, decltype(ExternalMessage::C2M_MASTER_AGREE_STUD_APP::dwStudentID) value) { self.dwStudentID = value; });
    auto external_message_c2m_master_refuse_stud_app_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MASTER_REFUSE_STUD_APP>(L, external_message_namespace, "C2M_MASTER_REFUSE_STUD_APP", "ExternalMessage::C2M_MASTER_REFUSE_STUD_APP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_master_refuse_stud_app_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MASTER_REFUSE_STUD_APP(); });
    external_message_c2m_master_refuse_stud_app_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MASTER_REFUSE_STUD_APP>(new ExternalMessage::C2M_MASTER_REFUSE_STUD_APP()); });
    external_message_c2m_master_refuse_stud_app_type["dwStudentID"] = sol::property([](const ExternalMessage::C2M_MASTER_REFUSE_STUD_APP& self) { return self.dwStudentID; }, [](ExternalMessage::C2M_MASTER_REFUSE_STUD_APP& self, decltype(ExternalMessage::C2M_MASTER_REFUSE_STUD_APP::dwStudentID) value) { self.dwStudentID = value; });
    auto external_message_c2m_master_cancel_stud_app_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MASTER_CANCEL_STUD_APP>(L, external_message_namespace, "C2M_MASTER_CANCEL_STUD_APP", "ExternalMessage::C2M_MASTER_CANCEL_STUD_APP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_master_cancel_stud_app_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MASTER_CANCEL_STUD_APP(); });
    external_message_c2m_master_cancel_stud_app_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MASTER_CANCEL_STUD_APP>(new ExternalMessage::C2M_MASTER_CANCEL_STUD_APP()); });
    external_message_c2m_master_cancel_stud_app_type["dwTeacherID"] = sol::property([](const ExternalMessage::C2M_MASTER_CANCEL_STUD_APP& self) { return self.dwTeacherID; }, [](ExternalMessage::C2M_MASTER_CANCEL_STUD_APP& self, decltype(ExternalMessage::C2M_MASTER_CANCEL_STUD_APP::dwTeacherID) value) { self.dwTeacherID = value; });
    auto external_message_c2m_master_fire_stud_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MASTER_FIRE_STUD>(L, external_message_namespace, "C2M_MASTER_FIRE_STUD", "ExternalMessage::C2M_MASTER_FIRE_STUD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_master_fire_stud_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MASTER_FIRE_STUD(); });
    external_message_c2m_master_fire_stud_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MASTER_FIRE_STUD>(new ExternalMessage::C2M_MASTER_FIRE_STUD()); });
    external_message_c2m_master_fire_stud_type["dwStudentID"] = sol::property([](const ExternalMessage::C2M_MASTER_FIRE_STUD& self) { return self.dwStudentID; }, [](ExternalMessage::C2M_MASTER_FIRE_STUD& self, decltype(ExternalMessage::C2M_MASTER_FIRE_STUD::dwStudentID) value) { self.dwStudentID = value; });
}

}
