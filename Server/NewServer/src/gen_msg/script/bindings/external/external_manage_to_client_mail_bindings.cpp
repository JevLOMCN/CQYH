#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/m2c_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/guild_def.h"
#include "../../../../../../Common/define/ranklist_def.h"
#include "../../../../../../Common/define/string_def.h"
#include "../../../../../../Common/define/team_def.h"
#include "../../../../../../Common/define/variable_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_manage_to_client_mail_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_m2c_mail_write_success_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_MAIL_WRITE_SUCCESS>(L, external_message_namespace, "M2C_MAIL_WRITE_SUCCESS", "ExternalMessage::M2C_MAIL_WRITE_SUCCESS", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_mail_write_success_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_MAIL_WRITE_SUCCESS(); });
    external_message_m2c_mail_write_success_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_MAIL_WRITE_SUCCESS>(new ExternalMessage::M2C_MAIL_WRITE_SUCCESS()); });
    auto external_message_brief_mail_info_type = cqyh::gen_msg::usertype<ExternalMessage::BriefMailInfo>(L, external_message_namespace, "BriefMailInfo", "ExternalMessage::BriefMailInfo");
    external_message_brief_mail_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::BriefMailInfo(); });
    external_message_brief_mail_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::BriefMailInfo>(new ExternalMessage::BriefMailInfo()); });
    external_message_brief_mail_info_type["dwMailID"] = sol::property([](const ExternalMessage::BriefMailInfo& self) { return self.dwMailID; }, [](ExternalMessage::BriefMailInfo& self, decltype(ExternalMessage::BriefMailInfo::dwMailID) value) { self.dwMailID = value; });
    external_message_brief_mail_info_type["dwMailTime"] = sol::property([](const ExternalMessage::BriefMailInfo& self) { return self.dwMailTime; }, [](ExternalMessage::BriefMailInfo& self, decltype(ExternalMessage::BriefMailInfo::dwMailTime) value) { self.dwMailTime = value; });
    external_message_brief_mail_info_type["byMailType"] = sol::property([](const ExternalMessage::BriefMailInfo& self) { return self.byMailType; }, [](ExternalMessage::BriefMailInfo& self, decltype(ExternalMessage::BriefMailInfo::byMailType) value) { self.byMailType = value; });
    external_message_brief_mail_info_type["byNew"] = sol::property([](const ExternalMessage::BriefMailInfo& self) { return self.byNew; }, [](ExternalMessage::BriefMailInfo& self, decltype(ExternalMessage::BriefMailInfo::byNew) value) { self.byNew = value; });
    external_message_brief_mail_info_type["byAttachment"] = sol::property([](const ExternalMessage::BriefMailInfo& self) { return self.byAttachment; }, [](ExternalMessage::BriefMailInfo& self, decltype(ExternalMessage::BriefMailInfo::byAttachment) value) { self.byAttachment = value; });
    external_message_brief_mail_info_type["szSenderRoleName"] = cqyh::lua::string_property(&ExternalMessage::BriefMailInfo::szSenderRoleName);
    external_message_brief_mail_info_type["szMailSubject"] = cqyh::lua::string_property(&ExternalMessage::BriefMailInfo::szMailSubject);
    auto external_message_m2c_mail_list_recv_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_MAIL_LIST_RECV>(L, external_message_namespace, "M2C_MAIL_LIST_RECV", "ExternalMessage::M2C_MAIL_LIST_RECV", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_mail_list_recv_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_MAIL_LIST_RECV(); });
    external_message_m2c_mail_list_recv_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_MAIL_LIST_RECV>(new ExternalMessage::M2C_MAIL_LIST_RECV()); });
    external_message_m2c_mail_list_recv_type["wMailCount"] = sol::property([](const ExternalMessage::M2C_MAIL_LIST_RECV& self) { return self.wMailCount; }, [](ExternalMessage::M2C_MAIL_LIST_RECV& self, decltype(ExternalMessage::M2C_MAIL_LIST_RECV::wMailCount) value) { self.wMailCount = value; });
    external_message_m2c_mail_list_recv_type["stBriefMailInfo"] = cqyh::lua::array_property(&ExternalMessage::M2C_MAIL_LIST_RECV::stBriefMailInfo);
    auto external_message_m2c_mail_read_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_MAIL_READ>(L, external_message_namespace, "M2C_MAIL_READ", "ExternalMessage::M2C_MAIL_READ", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_mail_read_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_MAIL_READ(); });
    external_message_m2c_mail_read_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_MAIL_READ>(new ExternalMessage::M2C_MAIL_READ()); });
    external_message_m2c_mail_read_type["dwMailID"] = sol::property([](const ExternalMessage::M2C_MAIL_READ& self) { return self.dwMailID; }, [](ExternalMessage::M2C_MAIL_READ& self, decltype(ExternalMessage::M2C_MAIL_READ::dwMailID) value) { self.dwMailID = value; });
    external_message_m2c_mail_read_type["dwMailTime"] = sol::property([](const ExternalMessage::M2C_MAIL_READ& self) { return self.dwMailTime; }, [](ExternalMessage::M2C_MAIL_READ& self, decltype(ExternalMessage::M2C_MAIL_READ::dwMailTime) value) { self.dwMailTime = value; });
    external_message_m2c_mail_read_type["byMailType"] = sol::property([](const ExternalMessage::M2C_MAIL_READ& self) { return self.byMailType; }, [](ExternalMessage::M2C_MAIL_READ& self, decltype(ExternalMessage::M2C_MAIL_READ::byMailType) value) { self.byMailType = value; });
    external_message_m2c_mail_read_type["nAttachmentItemID"] = sol::property([](const ExternalMessage::M2C_MAIL_READ& self) { return self.nAttachmentItemID; }, [](ExternalMessage::M2C_MAIL_READ& self, decltype(ExternalMessage::M2C_MAIL_READ::nAttachmentItemID) value) { self.nAttachmentItemID = value; });
    external_message_m2c_mail_read_type["nAttachmentItemCount"] = sol::property([](const ExternalMessage::M2C_MAIL_READ& self) { return self.nAttachmentItemCount; }, [](ExternalMessage::M2C_MAIL_READ& self, decltype(ExternalMessage::M2C_MAIL_READ::nAttachmentItemCount) value) { self.nAttachmentItemCount = value; });
    external_message_m2c_mail_read_type["szSenderRoleName"] = cqyh::lua::string_property(&ExternalMessage::M2C_MAIL_READ::szSenderRoleName);
    external_message_m2c_mail_read_type["szMailSubject"] = cqyh::lua::string_property(&ExternalMessage::M2C_MAIL_READ::szMailSubject);
    external_message_m2c_mail_read_type["szMailContent"] = cqyh::lua::string_property(&ExternalMessage::M2C_MAIL_READ::szMailContent);
    external_message_m2c_mail_read_type["nAttachCDTime"] = sol::property([](const ExternalMessage::M2C_MAIL_READ& self) { return self.nAttachCDTime; }, [](ExternalMessage::M2C_MAIL_READ& self, decltype(ExternalMessage::M2C_MAIL_READ::nAttachCDTime) value) { self.nAttachCDTime = value; });
    external_message_m2c_mail_read_type["byBufferAttach"] = sol::property([](const ExternalMessage::M2C_MAIL_READ& self) { return self.byBufferAttach; }, [](ExternalMessage::M2C_MAIL_READ& self, decltype(ExternalMessage::M2C_MAIL_READ::byBufferAttach) value) { self.byBufferAttach = value; });
    external_message_m2c_mail_read_type["acAttachItem"] = cqyh::lua::string_property(&ExternalMessage::M2C_MAIL_READ::acAttachItem);
    auto external_message_m2c_mail_delete_success_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_MAIL_DELETE_SUCCESS>(L, external_message_namespace, "M2C_MAIL_DELETE_SUCCESS", "ExternalMessage::M2C_MAIL_DELETE_SUCCESS", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_mail_delete_success_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_MAIL_DELETE_SUCCESS(); });
    external_message_m2c_mail_delete_success_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_MAIL_DELETE_SUCCESS>(new ExternalMessage::M2C_MAIL_DELETE_SUCCESS()); });
    external_message_m2c_mail_delete_success_type["dwMailID"] = sol::property([](const ExternalMessage::M2C_MAIL_DELETE_SUCCESS& self) { return self.dwMailID; }, [](ExternalMessage::M2C_MAIL_DELETE_SUCCESS& self, decltype(ExternalMessage::M2C_MAIL_DELETE_SUCCESS::dwMailID) value) { self.dwMailID = value; });
    auto external_message_m2c_mail_new_count_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_MAIL_NEW_COUNT>(L, external_message_namespace, "M2C_MAIL_NEW_COUNT", "ExternalMessage::M2C_MAIL_NEW_COUNT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_mail_new_count_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_MAIL_NEW_COUNT(); });
    external_message_m2c_mail_new_count_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_MAIL_NEW_COUNT>(new ExternalMessage::M2C_MAIL_NEW_COUNT()); });
    external_message_m2c_mail_new_count_type["dwMailNewCount"] = sol::property([](const ExternalMessage::M2C_MAIL_NEW_COUNT& self) { return self.dwMailNewCount; }, [](ExternalMessage::M2C_MAIL_NEW_COUNT& self, decltype(ExternalMessage::M2C_MAIL_NEW_COUNT::dwMailNewCount) value) { self.dwMailNewCount = value; });
    auto external_message_m2c_mail_get_attach_success_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_MAIL_GET_ATTACH_SUCCESS>(L, external_message_namespace, "M2C_MAIL_GET_ATTACH_SUCCESS", "ExternalMessage::M2C_MAIL_GET_ATTACH_SUCCESS", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_mail_get_attach_success_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_MAIL_GET_ATTACH_SUCCESS(); });
    external_message_m2c_mail_get_attach_success_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_MAIL_GET_ATTACH_SUCCESS>(new ExternalMessage::M2C_MAIL_GET_ATTACH_SUCCESS()); });
    external_message_m2c_mail_get_attach_success_type["dwMailID"] = sol::property([](const ExternalMessage::M2C_MAIL_GET_ATTACH_SUCCESS& self) { return self.dwMailID; }, [](ExternalMessage::M2C_MAIL_GET_ATTACH_SUCCESS& self, decltype(ExternalMessage::M2C_MAIL_GET_ATTACH_SUCCESS::dwMailID) value) { self.dwMailID = value; });
}

}
