# net - translated from net.vcxproj (Release|x64 / Debug|x64 settings).
# The source list is explicit on purpose; add new files here.

# Compile once, then archive the packet bindings separately. In Debug, the
# combined object archive exceeds MSVC's 4 GB library limit.
# Consumers link cqyh::net; net is the internal object compilation target.
add_library(net OBJECT
    ../../../Common/message/external_message_size.cpp
    intranet.cpp
    net.cpp
    packet_allocator.cpp
    script/sol_common.cpp
    script/sol_external_message.cpp
    script/sol_internal_message.cpp
    share_mem_net.cpp
    simple_net.cpp
    script/bindings/external/external_balance_to_client_bindings.cpp
    script/bindings/external/external_client_to_game_achievements_bindings.cpp
    script/bindings/external/external_client_to_game_activities_bindings.cpp
    script/bindings/external/external_client_to_game_appearance_bindings.cpp
    script/bindings/external/external_client_to_game_attributes_bindings.cpp
    script/bindings/external/external_client_to_game_audit_bindings.cpp
    script/bindings/external/external_client_to_game_auras_bindings.cpp
    script/bindings/external/external_client_to_game_characters_bindings.cpp
    script/bindings/external/external_client_to_game_chat_bindings.cpp
    script/bindings/external/external_client_to_game_combat_bindings.cpp
    script/bindings/external/external_client_to_game_common_bindings.cpp
    script/bindings/external/external_client_to_game_companions_bindings.cpp
    script/bindings/external/external_client_to_game_connection_bindings.cpp
    script/bindings/external/external_client_to_game_crafting_bindings.cpp
    script/bindings/external/external_client_to_game_dungeons_bindings.cpp
    script/bindings/external/external_client_to_game_economy_bindings.cpp
    script/bindings/external/external_client_to_game_enhancement_bindings.cpp
    script/bindings/external/external_client_to_game_friends_bindings.cpp
    script/bindings/external/external_client_to_game_gambling_bindings.cpp
    script/bindings/external/external_client_to_game_guild_bindings.cpp
    script/bindings/external/external_client_to_game_guild_buildings_bindings.cpp
    script/bindings/external/external_client_to_game_guild_members_bindings.cpp
    script/bindings/external/external_client_to_game_guild_storage_bindings.cpp
    script/bindings/external/external_client_to_game_guild_war_bindings.cpp
    script/bindings/external/external_client_to_game_ids_bindings.cpp
    script/bindings/external/external_client_to_game_inventory_bindings.cpp
    script/bindings/external/external_client_to_game_mentorship_bindings.cpp
    script/bindings/external/external_client_to_game_movement_bindings.cpp
    script/bindings/external/external_client_to_game_progression_bindings.cpp
    script/bindings/external/external_client_to_game_pvp_bindings.cpp
    script/bindings/external/external_client_to_game_quests_bindings.cpp
    script/bindings/external/external_client_to_game_reputation_bindings.cpp
    script/bindings/external/external_client_to_game_session_bindings.cpp
    script/bindings/external/external_client_to_game_shops_bindings.cpp
    script/bindings/external/external_client_to_game_spells_bindings.cpp
    script/bindings/external/external_client_to_game_stalls_bindings.cpp
    script/bindings/external/external_client_to_game_team_auction_bindings.cpp
    script/bindings/external/external_client_to_game_teams_bindings.cpp
    script/bindings/external/external_client_to_game_titles_bindings.cpp
    script/bindings/external/external_client_to_game_trade_bindings.cpp
    script/bindings/external/external_client_to_game_variables_bindings.cpp
    script/bindings/external/external_client_to_game_world_bindings.cpp
    script/bindings/external/external_client_to_game_world_objects_bindings.cpp
    script/bindings/external/external_client_to_gate_bindings.cpp
    script/bindings/external/external_client_to_manage_activities_bindings.cpp
    script/bindings/external/external_client_to_manage_administration_bindings.cpp
    script/bindings/external/external_client_to_manage_announcements_bindings.cpp
    script/bindings/external/external_client_to_manage_auction_bindings.cpp
    script/bindings/external/external_client_to_manage_audit_bindings.cpp
    script/bindings/external/external_client_to_manage_characters_bindings.cpp
    script/bindings/external/external_client_to_manage_chat_bindings.cpp
    script/bindings/external/external_client_to_manage_common_bindings.cpp
    script/bindings/external/external_client_to_manage_dungeons_bindings.cpp
    script/bindings/external/external_client_to_manage_economy_bindings.cpp
    script/bindings/external/external_client_to_manage_friends_bindings.cpp
    script/bindings/external/external_client_to_manage_guild_bindings.cpp
    script/bindings/external/external_client_to_manage_guild_buildings_bindings.cpp
    script/bindings/external/external_client_to_manage_guild_diplomacy_bindings.cpp
    script/bindings/external/external_client_to_manage_guild_members_bindings.cpp
    script/bindings/external/external_client_to_manage_guild_storage_bindings.cpp
    script/bindings/external/external_client_to_manage_guild_war_bindings.cpp
    script/bindings/external/external_client_to_manage_ids_bindings.cpp
    script/bindings/external/external_client_to_manage_mail_bindings.cpp
    script/bindings/external/external_client_to_manage_mentorship_bindings.cpp
    script/bindings/external/external_client_to_manage_mentorship_recruitment_bindings.cpp
    script/bindings/external/external_client_to_manage_mentorship_students_bindings.cpp
    script/bindings/external/external_client_to_manage_mentorship_teachers_bindings.cpp
    script/bindings/external/external_client_to_manage_pvp_bindings.cpp
    script/bindings/external/external_client_to_manage_session_bindings.cpp
    script/bindings/external/external_client_to_manage_teams_bindings.cpp
    script/bindings/external/external_client_to_manage_world_bindings.cpp
    script/bindings/external/external_game_to_client_achievements_bindings.cpp
    script/bindings/external/external_game_to_client_activities_bindings.cpp
    script/bindings/external/external_game_to_client_appearance_bindings.cpp
    script/bindings/external/external_game_to_client_attributes_bindings.cpp
    script/bindings/external/external_game_to_client_auras_bindings.cpp
    script/bindings/external/external_game_to_client_characters_bindings.cpp
    script/bindings/external/external_game_to_client_chat_bindings.cpp
    script/bindings/external/external_game_to_client_combat_bindings.cpp
    script/bindings/external/external_game_to_client_common_bindings.cpp
    script/bindings/external/external_game_to_client_companions_bindings.cpp
    script/bindings/external/external_game_to_client_connection_bindings.cpp
    script/bindings/external/external_game_to_client_crafting_bindings.cpp
    script/bindings/external/external_game_to_client_dungeons_bindings.cpp
    script/bindings/external/external_game_to_client_economy_bindings.cpp
    script/bindings/external/external_game_to_client_enhancement_bindings.cpp
    script/bindings/external/external_game_to_client_friends_bindings.cpp
    script/bindings/external/external_game_to_client_gambling_bindings.cpp
    script/bindings/external/external_game_to_client_guild_bindings.cpp
    script/bindings/external/external_game_to_client_guild_members_bindings.cpp
    script/bindings/external/external_game_to_client_ids_bindings.cpp
    script/bindings/external/external_game_to_client_inventory_bindings.cpp
    script/bindings/external/external_game_to_client_loot_bindings.cpp
    script/bindings/external/external_game_to_client_mentorship_bindings.cpp
    script/bindings/external/external_game_to_client_mentorship_teachers_bindings.cpp
    script/bindings/external/external_game_to_client_movement_bindings.cpp
    script/bindings/external/external_game_to_client_progression_bindings.cpp
    script/bindings/external/external_game_to_client_pvp_bindings.cpp
    script/bindings/external/external_game_to_client_quests_bindings.cpp
    script/bindings/external/external_game_to_client_reputation_bindings.cpp
    script/bindings/external/external_game_to_client_session_bindings.cpp
    script/bindings/external/external_game_to_client_shops_bindings.cpp
    script/bindings/external/external_game_to_client_siege_bindings.cpp
    script/bindings/external/external_game_to_client_spells_bindings.cpp
    script/bindings/external/external_game_to_client_stalls_bindings.cpp
    script/bindings/external/external_game_to_client_synchronization_bindings.cpp
    script/bindings/external/external_game_to_client_team_auction_bindings.cpp
    script/bindings/external/external_game_to_client_teams_bindings.cpp
    script/bindings/external/external_game_to_client_titles_bindings.cpp
    script/bindings/external/external_game_to_client_trade_bindings.cpp
    script/bindings/external/external_game_to_client_variables_bindings.cpp
    script/bindings/external/external_game_to_client_world_bindings.cpp
    script/bindings/external/external_game_to_client_world_objects_bindings.cpp
    script/bindings/external/external_gate_to_client_bindings.cpp
    script/bindings/external/external_headers_bindings.cpp
    script/bindings/external/external_manage_to_client_activities_bindings.cpp
    script/bindings/external/external_manage_to_client_attributes_bindings.cpp
    script/bindings/external/external_manage_to_client_auction_bindings.cpp
    script/bindings/external/external_manage_to_client_audit_bindings.cpp
    script/bindings/external/external_manage_to_client_characters_bindings.cpp
    script/bindings/external/external_manage_to_client_chat_bindings.cpp
    script/bindings/external/external_manage_to_client_common_bindings.cpp
    script/bindings/external/external_manage_to_client_connection_bindings.cpp
    script/bindings/external/external_manage_to_client_dungeons_bindings.cpp
    script/bindings/external/external_manage_to_client_economy_bindings.cpp
    script/bindings/external/external_manage_to_client_friends_bindings.cpp
    script/bindings/external/external_manage_to_client_guild_bindings.cpp
    script/bindings/external/external_manage_to_client_guild_buildings_bindings.cpp
    script/bindings/external/external_manage_to_client_guild_diplomacy_bindings.cpp
    script/bindings/external/external_manage_to_client_guild_members_bindings.cpp
    script/bindings/external/external_manage_to_client_guild_storage_bindings.cpp
    script/bindings/external/external_manage_to_client_guild_war_bindings.cpp
    script/bindings/external/external_manage_to_client_ids_bindings.cpp
    script/bindings/external/external_manage_to_client_mail_bindings.cpp
    script/bindings/external/external_manage_to_client_mentorship_bindings.cpp
    script/bindings/external/external_manage_to_client_mentorship_recruitment_bindings.cpp
    script/bindings/external/external_manage_to_client_mentorship_students_bindings.cpp
    script/bindings/external/external_manage_to_client_mentorship_teachers_bindings.cpp
    script/bindings/external/external_manage_to_client_pvp_bindings.cpp
    script/bindings/external/external_manage_to_client_quests_bindings.cpp
    script/bindings/external/external_manage_to_client_siege_bindings.cpp
    script/bindings/external/external_manage_to_client_teams_bindings.cpp
    script/bindings/external/external_manage_to_client_world_bindings.cpp
    script/bindings/internal/internal_certification_bindings.cpp
    script/bindings/internal/internal_common_bindings.cpp
    script/bindings/internal/internal_database_to_game_bindings.cpp
    script/bindings/internal/internal_database_to_gate_bindings.cpp
    script/bindings/internal/internal_database_to_manage_bindings.cpp
    script/bindings/internal/internal_game_to_database_bindings.cpp
    script/bindings/internal/internal_game_to_gate_bindings.cpp
    script/bindings/internal/internal_game_to_manage_activities_bindings.cpp
    script/bindings/internal/internal_game_to_manage_administration_bindings.cpp
    script/bindings/internal/internal_game_to_manage_auction_bindings.cpp
    script/bindings/internal/internal_game_to_manage_audit_bindings.cpp
    script/bindings/internal/internal_game_to_manage_characters_bindings.cpp
    script/bindings/internal/internal_game_to_manage_chat_bindings.cpp
    script/bindings/internal/internal_game_to_manage_common_bindings.cpp
    script/bindings/internal/internal_game_to_manage_connection_bindings.cpp
    script/bindings/internal/internal_game_to_manage_dungeons_bindings.cpp
    script/bindings/internal/internal_game_to_manage_economy_bindings.cpp
    script/bindings/internal/internal_game_to_manage_guild_bindings.cpp
    script/bindings/internal/internal_game_to_manage_guild_buildings_bindings.cpp
    script/bindings/internal/internal_game_to_manage_guild_members_bindings.cpp
    script/bindings/internal/internal_game_to_manage_guild_storage_bindings.cpp
    script/bindings/internal/internal_game_to_manage_guild_war_bindings.cpp
    script/bindings/internal/internal_game_to_manage_ids_bindings.cpp
    script/bindings/internal/internal_game_to_manage_loot_bindings.cpp
    script/bindings/internal/internal_game_to_manage_mail_bindings.cpp
    script/bindings/internal/internal_game_to_manage_mentorship_bindings.cpp
    script/bindings/internal/internal_game_to_manage_mentorship_students_bindings.cpp
    script/bindings/internal/internal_game_to_manage_mentorship_teachers_bindings.cpp
    script/bindings/internal/internal_game_to_manage_progression_bindings.cpp
    script/bindings/internal/internal_game_to_manage_pvp_bindings.cpp
    script/bindings/internal/internal_game_to_manage_quests_bindings.cpp
    script/bindings/internal/internal_game_to_manage_session_bindings.cpp
    script/bindings/internal/internal_game_to_manage_shops_bindings.cpp
    script/bindings/internal/internal_game_to_manage_siege_bindings.cpp
    script/bindings/internal/internal_game_to_manage_spells_bindings.cpp
    script/bindings/internal/internal_game_to_manage_teams_bindings.cpp
    script/bindings/internal/internal_game_to_manage_variables_bindings.cpp
    script/bindings/internal/internal_game_to_manage_world_bindings.cpp
    script/bindings/internal/internal_gate_to_database_bindings.cpp
    script/bindings/internal/internal_gate_to_game_bindings.cpp
    script/bindings/internal/internal_gate_to_login_bindings.cpp
    script/bindings/internal/internal_gate_to_manage_bindings.cpp
    script/bindings/internal/internal_gm_server_administration_bindings.cpp
    script/bindings/internal/internal_gm_server_announcements_bindings.cpp
    script/bindings/internal/internal_gm_server_audit_bindings.cpp
    script/bindings/internal/internal_gm_server_characters_bindings.cpp
    script/bindings/internal/internal_gm_server_chat_bindings.cpp
    script/bindings/internal/internal_gm_server_connection_bindings.cpp
    script/bindings/internal/internal_gm_server_economy_bindings.cpp
    script/bindings/internal/internal_gm_server_ids_bindings.cpp
    script/bindings/internal/internal_gm_server_mail_bindings.cpp
    script/bindings/internal/internal_gm_tool_administration_bindings.cpp
    script/bindings/internal/internal_gm_tool_announcements_bindings.cpp
    script/bindings/internal/internal_gm_tool_audit_bindings.cpp
    script/bindings/internal/internal_gm_tool_characters_bindings.cpp
    script/bindings/internal/internal_gm_tool_chat_bindings.cpp
    script/bindings/internal/internal_gm_tool_common_bindings.cpp
    script/bindings/internal/internal_gm_tool_connection_bindings.cpp
    script/bindings/internal/internal_gm_tool_economy_bindings.cpp
    script/bindings/internal/internal_gm_tool_ids_bindings.cpp
    script/bindings/internal/internal_gm_tool_inventory_bindings.cpp
    script/bindings/internal/internal_gm_tool_lookup_bindings.cpp
    script/bindings/internal/internal_gm_tool_mail_bindings.cpp
    script/bindings/internal/internal_gm_tool_session_bindings.cpp
    script/bindings/internal/internal_headers_bindings.cpp
    script/bindings/internal/internal_login_to_gate_bindings.cpp
    script/bindings/internal/internal_login_to_manage_bindings.cpp
    script/bindings/internal/internal_manage_to_database_bindings.cpp
    script/bindings/internal/internal_manage_to_game_achievements_bindings.cpp
    script/bindings/internal/internal_manage_to_game_activities_bindings.cpp
    script/bindings/internal/internal_manage_to_game_administration_bindings.cpp
    script/bindings/internal/internal_manage_to_game_auction_bindings.cpp
    script/bindings/internal/internal_manage_to_game_characters_bindings.cpp
    script/bindings/internal/internal_manage_to_game_common_bindings.cpp
    script/bindings/internal/internal_manage_to_game_connection_bindings.cpp
    script/bindings/internal/internal_manage_to_game_dungeons_bindings.cpp
    script/bindings/internal/internal_manage_to_game_economy_bindings.cpp
    script/bindings/internal/internal_manage_to_game_friends_bindings.cpp
    script/bindings/internal/internal_manage_to_game_guild_bindings.cpp
    script/bindings/internal/internal_manage_to_game_guild_buildings_bindings.cpp
    script/bindings/internal/internal_manage_to_game_guild_diplomacy_bindings.cpp
    script/bindings/internal/internal_manage_to_game_guild_members_bindings.cpp
    script/bindings/internal/internal_manage_to_game_guild_storage_bindings.cpp
    script/bindings/internal/internal_manage_to_game_guild_war_bindings.cpp
    script/bindings/internal/internal_manage_to_game_ids_bindings.cpp
    script/bindings/internal/internal_manage_to_game_loot_bindings.cpp
    script/bindings/internal/internal_manage_to_game_mail_bindings.cpp
    script/bindings/internal/internal_manage_to_game_mentorship_bindings.cpp
    script/bindings/internal/internal_manage_to_game_mentorship_students_bindings.cpp
    script/bindings/internal/internal_manage_to_game_mentorship_teachers_bindings.cpp
    script/bindings/internal/internal_manage_to_game_movement_bindings.cpp
    script/bindings/internal/internal_manage_to_game_progression_bindings.cpp
    script/bindings/internal/internal_manage_to_game_pvp_bindings.cpp
    script/bindings/internal/internal_manage_to_game_shops_bindings.cpp
    script/bindings/internal/internal_manage_to_game_siege_bindings.cpp
    script/bindings/internal/internal_manage_to_game_synchronization_bindings.cpp
    script/bindings/internal/internal_manage_to_game_teams_bindings.cpp
    script/bindings/internal/internal_manage_to_game_variables_bindings.cpp
    script/bindings/internal/internal_manage_to_game_world_bindings.cpp
    script/bindings/internal/internal_manage_to_game_world_objects_bindings.cpp
    script/bindings/internal/internal_manage_to_gate_bindings.cpp
    script/bindings/internal/internal_manage_to_login_bindings.cpp
    script/bindings/registration.h
    stdafx.cpp
)

cqyh_target_defaults(net)
cqyh_use_pch(net ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(net PRIVATE
    ${CQYH_SERVER_DIR}/NewServer/inc
    ${CQYH_SERVER_DIR}/NewServer/inc/baselib
    ${CQYH_SERVER_DIR}/NewServer/inc/net
    ${CQYH_SERVER_DIR}/NewServer/3rd/lua/src
    ${CQYH_SERVER_DIR}/Common/define
    ${CQYH_SERVER_DIR}/Common/message
)

# asio transport (netAsio) includes its socket headers before Windows headers.
# Keep it separate from the legacy stdafx.h include order.
target_sources(net PRIVATE asio_net.cpp)
set_source_files_properties(asio_net.cpp PROPERTIES
    SKIP_PRECOMPILE_HEADERS ON
    COMPILE_DEFINITIONS NOMINMAX)
target_link_libraries(net PUBLIC asio::asio)



target_link_libraries(net PUBLIC cqyh::sol2)

# Visual Studio also emits a dummy .lib for an OBJECT target. Keep packet
# objects out of that dummy archive; the real archives below own them.
if(MSVC)
    get_target_property(_net_objects net SOURCES)
    foreach(source IN LISTS _net_objects)
        if(source MATCHES "script/sol_(external|internal)_message|script/bindings/(external|internal)/")
            set_property(SOURCE ${source} APPEND PROPERTY VS_SETTINGS "LibCompiled=false")
        endif()
    endforeach()
endif()

set(_net_external_bindings "sol_external_message|external_.*_bindings")
set(_net_external_responses "external_(game|manage)_to_client_.*_bindings")
set(_net_internal_bindings "sol_internal_message|internal_.*_bindings")
add_library(net_core STATIC "$<FILTER:$<TARGET_OBJECTS:net>,EXCLUDE,${_net_external_bindings}|${_net_internal_bindings}>")
# C++23's Debug objects push the complete external archive past MSVC's 4 GB
# limit. Keep the large game/manage-to-client registrations in their own archive.
add_library(net_lua_external STATIC "$<FILTER:$<FILTER:$<TARGET_OBJECTS:net>,INCLUDE,${_net_external_bindings}>,EXCLUDE,${_net_external_responses}>")
add_library(net_lua_external_responses STATIC "$<FILTER:$<TARGET_OBJECTS:net>,INCLUDE,${_net_external_responses}>")
add_library(net_lua_internal STATIC "$<FILTER:$<TARGET_OBJECTS:net>,INCLUDE,${_net_internal_bindings}>")
foreach(archive net_core net_lua_external net_lua_external_responses net_lua_internal)
    cqyh_target_defaults(${archive})
    set_target_properties(${archive} PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(${archive} PUBLIC cqyh::sol2)
endforeach()
set_target_properties(net_core PROPERTIES OUTPUT_NAME net)
# Preserve the complete external binding interface for existing consumers.
target_link_libraries(net_lua_external PUBLIC net_lua_external_responses)
target_link_libraries(net_core PUBLIC asio::asio net_lua_external net_lua_internal)
add_library(cqyh::net ALIAS net_core)

get_target_property(_binding_sources net SOURCES)
list(FILTER _binding_sources INCLUDE REGEX "^script/bindings/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/script/bindings"
    PREFIX "Lua bindings" FILES ${_binding_sources})
