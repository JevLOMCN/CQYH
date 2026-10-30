add_test(NAME tool_version_label
    COMMAND "${CMAKE_COMMAND}"
        "-DLABEL_TOOL=$<TARGET_FILE:gmtool_label_version>"
        "-DSERVER=${CQYH_SERVER_DIR}"
        "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/version label"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_version_label.cmake")
set_tests_properties(tool_version_label PROPERTIES TIMEOUT 60)
add_test(NAME tool_string_map
    COMMAND "${CMAKE_COMMAND}"
        "-DMAP_TOOL=$<TARGET_FILE:update_string_map>"
        "-DSERVER=${CQYH_SERVER_DIR}"
        "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/string map"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_string_map.cmake")
set_tests_properties(tool_string_map PROPERTIES TIMEOUT 60)

add_executable(test_tool_pak test_pak.cpp ../../tools/pak/pak_file.cpp)
cqyh_target_defaults(test_tool_pak)
target_include_directories(test_tool_pak PRIVATE
    ../../tools/pak ../../inc ../../inc/baselib ../../3rd/zlib)
target_compile_definitions(test_tool_pak PRIVATE NOMINMAX _CRT_SECURE_NO_WARNINGS)
target_link_libraries(test_tool_pak PRIVATE baselib cqyh::zlib cqyh::syslibs)
cqyh_copy_runtime_dlls(test_tool_pak)
add_test(NAME tool_pak_formats
    COMMAND "${CMAKE_COMMAND}"
        "-DPAK_TEST=$<TARGET_FILE:test_tool_pak>"
        "-DPAK_TOOL=$<TARGET_FILE:pak>"
        "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/pak formats"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_pak.cmake")
set_tests_properties(tool_pak_formats PROPERTIES TIMEOUT 60)

add_executable(test_tool_common_log test_common_log.cpp
    ../../tools/common_log/common_log.cpp ../../tools/common_log/anticheat_log.cpp)
cqyh_target_defaults(test_tool_common_log)
target_include_directories(test_tool_common_log PRIVATE ../../tools/common_log)
add_test(NAME tool_common_log
    COMMAND "${CMAKE_COMMAND}"
        "-DCOMMON_LOG=$<TARGET_FILE:common_log>"
        "-DCOMMON_TEST=$<TARGET_FILE:test_tool_common_log>"
        "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/common log"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_common_log.cmake")
set_tests_properties(tool_common_log PROPERTIES TIMEOUT 60)

add_executable(test_tool_analysis_log test_analysis_log.cpp
    ../../tools/analysis_log/analysis_records.cpp ../../tools/analysis_log/iocp_mgr.cpp)
cqyh_target_defaults(test_tool_analysis_log)
target_include_directories(test_tool_analysis_log PRIVATE ../../tools/analysis_log)
target_compile_definitions(test_tool_analysis_log PRIVATE NOMINMAX)
add_test(NAME tool_analysis_log
    COMMAND "${CMAKE_COMMAND}"
        "-DANALYSIS_TOOL=$<TARGET_FILE:analysis_log>"
        "-DANALYSIS_TEST=$<TARGET_FILE:test_tool_analysis_log>"
        "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/analysis log"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_analysis_log.cmake")
set_tests_properties(tool_analysis_log PROPERTIES TIMEOUT 90)

add_executable(test_tool_role_log test_role_log.cpp
    ../../tools/RoleLogCheck/RoleLogCheck/rolelogcheck.cpp)
cqyh_target_defaults(test_tool_role_log)
target_include_directories(test_tool_role_log PRIVATE
    ../../tools/RoleLogCheck/RoleLogCheck ../../inc ../../inc/baselib)
target_compile_definitions(test_tool_role_log PRIVATE NOMINMAX _CRT_SECURE_NO_WARNINGS)
target_link_libraries(test_tool_role_log PRIVATE baselib cqyh::lua cqyh::zlib cqyh::syslibs)
cqyh_copy_runtime_dlls(test_tool_role_log)
add_test(NAME tool_role_log
    COMMAND "${CMAKE_COMMAND}"
        "-DROLE_TOOL=$<TARGET_FILE:RoleLogCheck>"
        "-DROLE_TEST=$<TARGET_FILE:test_tool_role_log>"
        "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/role log"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_role_log.cmake")
set_tests_properties(tool_role_log PROPERTIES TIMEOUT 120)

add_executable(test_tool_cash_accounting test_cash_accounting.cpp
    ../baselib/fake_mysql_client.cpp
    ../../tools/cash_accounting/cash_accounting/cash_accounting.cpp
    ../../tools/cash_accounting/cash_accounting/cash_accounting_stmt.list.cpp)
cqyh_target_defaults(test_tool_cash_accounting)
target_include_directories(test_tool_cash_accounting PRIVATE
    ../../tools/cash_accounting/cash_accounting ../baselib ../../inc ../../inc/baselib)
target_compile_definitions(test_tool_cash_accounting PRIVATE NOMINMAX _CRT_SECURE_NO_WARNINGS)
target_link_libraries(test_tool_cash_accounting PRIVATE baselib fusion cqyh::syslibs)
cqyh_copy_runtime_dlls(test_tool_cash_accounting)
add_test(NAME tool_cash_accounting
    COMMAND "${CMAKE_COMMAND}"
        "-DCASH_TEST=$<TARGET_FILE:test_tool_cash_accounting>"
        "-DCASH_TOOL=$<TARGET_FILE:cash_accounting>"
        "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/cash accounting"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_cash_accounting.cmake")
set_tests_properties(tool_cash_accounting PROPERTIES TIMEOUT 90)

add_executable(test_tool_exportdb test_exportdb.cpp ../baselib/fake_mysql_client.cpp
    ../../tools/exportdb/export_sql.cpp ../../tools/exportdb/guild.cpp)
cqyh_target_defaults(test_tool_exportdb)
target_include_directories(test_tool_exportdb PRIVATE ../../tools/exportdb ../baselib
    ../../inc ../../inc/baselib "${CQYH_SERVER_DIR}/Common/define")
target_compile_definitions(test_tool_exportdb PRIVATE NOMINMAX _CRT_SECURE_NO_WARNINGS)
target_link_libraries(test_tool_exportdb PRIVATE baselib fusion cqyh::zlib cqyh::syslibs)
cqyh_copy_runtime_dlls(test_tool_exportdb)
add_test(NAME tool_exportdb
    COMMAND "${CMAKE_COMMAND}"
        "-DEXPORT_TEST=$<TARGET_FILE:test_tool_exportdb>"
        "-DEXPORT_TOOL=$<TARGET_FILE:exportdb>"
        "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/exportdb"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_exportdb.cmake")
set_tests_properties(tool_exportdb PROPERTIES TIMEOUT 90)

add_executable(test_tool_export_roles test_export_roles.cpp ../baselib/fake_mysql_client.cpp
    ../../tools/exportdb/export_sql.cpp ../../tools/exportdb/role_records.cpp)
cqyh_target_defaults(test_tool_export_roles)
target_include_directories(test_tool_export_roles PRIVATE ../../tools/exportdb ../baselib
    ../../inc ../../inc/baselib "${CQYH_SERVER_DIR}/Common/define")
target_compile_definitions(test_tool_export_roles PRIVATE NOMINMAX _CRT_SECURE_NO_WARNINGS)
target_link_libraries(test_tool_export_roles PRIVATE baselib fusion cqyh::lua cqyh::zlib cqyh::syslibs)
cqyh_copy_runtime_dlls(test_tool_export_roles)
add_test(NAME tool_export_roles COMMAND test_tool_export_roles
    "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/export roles" "${CMAKE_CURRENT_SOURCE_DIR}/fixtures")
set_tests_properties(tool_export_roles PROPERTIES TIMEOUT 90)

add_executable(test_tool_guard_sessions ../../tools/guardserver/guard_files.cpp test_guard_sessions.cpp ../../tools/guardserver/gd_session_mgr.cpp)
cqyh_target_defaults(test_tool_guard_sessions)
target_include_directories(test_tool_guard_sessions PRIVATE ../../tools/guardserver ../../inc ../../inc/baselib ../../inc/net
    "${CQYH_SERVER_DIR}/Common/define" "${CQYH_SERVER_DIR}/Common/fusion/inc" "${CQYH_SERVER_DIR}/Common/message")
target_compile_definitions(test_tool_guard_sessions PRIVATE NOMINMAX)
target_link_libraries(test_tool_guard_sessions PRIVATE baselib fusion cqyh::syslibs)
cqyh_copy_runtime_dlls(test_tool_guard_sessions)
add_test(NAME tool_guard_sessions COMMAND test_tool_guard_sessions)
set_tests_properties(tool_guard_sessions PROPERTIES TIMEOUT 30)

add_executable(test_tool_guard_commands test_guard_commands.cpp)
cqyh_target_defaults(test_tool_guard_commands)
target_include_directories(test_tool_guard_commands PRIVATE ../../tools/guardserver)
add_test(NAME tool_guard_commands COMMAND test_tool_guard_commands)
set_tests_properties(tool_guard_commands PROPERTIES TIMEOUT 30)

add_executable(test_tool_guard_dispatch test_guard_dispatch.cpp ../../tools/guardserver/cmd_mgr.cpp
    ../../tools/guardserver/guard_files.cpp
    ../../tools/guardserver/gd_session_mgr.cpp ../../src/net/packet_allocator.cpp)
cqyh_target_defaults(test_tool_guard_dispatch)
target_include_directories(test_tool_guard_dispatch PRIVATE ../../tools/guardserver ../../inc ../../inc/baselib ../../inc/net
    "${CQYH_SERVER_DIR}/Common/define" "${CQYH_SERVER_DIR}/Common/fusion/inc" "${CQYH_SERVER_DIR}/Common/message")
target_compile_definitions(test_tool_guard_dispatch PRIVATE NOMINMAX)
target_link_libraries(test_tool_guard_dispatch PRIVATE baselib fusion cqyh::syslibs)
cqyh_copy_runtime_dlls(test_tool_guard_dispatch)
add_test(NAME tool_guard_dispatch COMMAND test_tool_guard_dispatch)
set_tests_properties(tool_guard_dispatch PROPERTIES TIMEOUT 30)

add_executable(test_tool_guard_processes ../../tools/guardserver/guard_files.cpp test_guard_processes.cpp ../../tools/guardserver/process_mgr.cpp
    ../../tools/guardserver/gd_session_mgr.cpp)
cqyh_target_defaults(test_tool_guard_processes)
target_include_directories(test_tool_guard_processes PRIVATE ../../tools/guardserver ../../inc ../../inc/baselib ../../inc/net
    "${CQYH_SERVER_DIR}/Common/define" "${CQYH_SERVER_DIR}/Common/fusion/inc" "${CQYH_SERVER_DIR}/Common/message")
target_compile_definitions(test_tool_guard_processes PRIVATE NOMINMAX)
# Only this fixture target receives aliases; the production guard uses real APIs.
if(MSVC)
    target_compile_options(test_tool_guard_processes PRIVATE "/FI${CMAKE_CURRENT_SOURCE_DIR}/fake_guard_process_api.h")
endif()
target_link_libraries(test_tool_guard_processes PRIVATE baselib fusion cqyh::syslibs)
cqyh_copy_runtime_dlls(test_tool_guard_processes)
add_test(NAME tool_guard_processes COMMAND test_tool_guard_processes)
set_tests_properties(tool_guard_processes PROPERTIES TIMEOUT 30)

add_executable(test_tool_guard_native_handles test_guard_native_handles.cpp)
cqyh_target_defaults(test_tool_guard_native_handles)
target_include_directories(test_tool_guard_native_handles PRIVATE ../../tools/guardserver)
target_compile_definitions(test_tool_guard_native_handles PRIVATE NOMINMAX)
target_link_libraries(test_tool_guard_native_handles PRIVATE psapi)
add_test(NAME tool_guard_native_handles COMMAND test_tool_guard_native_handles)
set_tests_properties(tool_guard_native_handles PROPERTIES TIMEOUT 30)

add_executable(test_tool_guard_files test_guard_files.cpp ../../tools/guardserver/guard_files.cpp)
cqyh_target_defaults(test_tool_guard_files)
target_include_directories(test_tool_guard_files PRIVATE ../../tools/guardserver)
target_compile_definitions(test_tool_guard_files PRIVATE NOMINMAX)
add_test(NAME tool_guard_files COMMAND test_tool_guard_files
    "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/guard files")
set_tests_properties(tool_guard_files PROPERTIES TIMEOUT 30)

add_executable(test_tool_guard_results ../../tools/guardserver/guard_deploy.cpp test_guard_results.cpp
    ../../tools/guardserver/guard_files.cpp ../../tools/guardserver/guardserver.cpp
    ../../tools/guardserver/gd_message_handler.cpp ../../tools/guardserver/gd_session_mgr.cpp
    ../../tools/guardserver/process_mgr.cpp ../../tools/guardserver/cmd_mgr.cpp)
cqyh_target_defaults(test_tool_guard_results)
target_include_directories(test_tool_guard_results PRIVATE ../../tools/guardserver
    ../../inc ../../inc/baselib ../../inc/net "${CQYH_SERVER_DIR}/Common/define"
    "${CQYH_SERVER_DIR}/Common/fusion/inc" "${CQYH_SERVER_DIR}/Common/message")
target_compile_definitions(test_tool_guard_results PRIVATE NOMINMAX)
target_link_libraries(test_tool_guard_results PRIVATE baselib cqyh::net cqyh::lua cqyh::zlib fusion cqyh::syslibs shlwapi)
cqyh_copy_runtime_dlls(test_tool_guard_results)
add_test(NAME tool_guard_results COMMAND test_tool_guard_results
    "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/guard results")
set_tests_properties(tool_guard_results PROPERTIES TIMEOUT 30)

add_executable(guard_deploy_child guard_deploy_child.cpp)
cqyh_target_defaults(guard_deploy_child)
add_executable(test_tool_guard_deploy test_guard_deploy.cpp
    ../../tools/guardserver/guard_files.cpp ../../tools/guardserver/guard_deploy.cpp)
cqyh_target_defaults(test_tool_guard_deploy)
target_include_directories(test_tool_guard_deploy PRIVATE ../../tools/guardserver)
target_compile_definitions(test_tool_guard_deploy PRIVATE NOMINMAX)
add_dependencies(test_tool_guard_deploy guard_deploy_child)
add_test(NAME tool_guard_deploy COMMAND test_tool_guard_deploy
    "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/guard deployment"
    "$<TARGET_FILE:guard_deploy_child>" "${CQYH_SERVER_DIR}/NewServer/bin/_GUARD/7z.exe")
set_tests_properties(tool_guard_deploy PROPERTIES TIMEOUT 90)

add_executable(test_tool_guard_runtime test_guard_runtime.cpp
    ../../tools/guardserver/guard_files.cpp ../../src/net/packet_allocator.cpp)
cqyh_target_defaults(test_tool_guard_runtime)
target_include_directories(test_tool_guard_runtime PRIVATE ../../tools/guardserver
    ../../inc ../../inc/baselib ../../inc/net "${CQYH_SERVER_DIR}/Common/define"
    "${CQYH_SERVER_DIR}/Common/fusion/inc" "${CQYH_SERVER_DIR}/Common/message")
target_compile_definitions(test_tool_guard_runtime PRIVATE NOMINMAX)
target_link_libraries(test_tool_guard_runtime PRIVATE baselib fusion cqyh::syslibs)
cqyh_copy_runtime_dlls(test_tool_guard_runtime)
add_test(NAME tool_guard_runtime COMMAND test_tool_guard_runtime
    "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/guard runtime")
set_tests_properties(tool_guard_runtime PROPERTIES TIMEOUT 30)

add_executable(test_tool_guard_console test_guard_console.cpp)
cqyh_target_defaults(test_tool_guard_console)
target_include_directories(test_tool_guard_console PRIVATE ../../tools/guardserver)
add_test(NAME tool_guard_console COMMAND test_tool_guard_console)
set_tests_properties(tool_guard_console PROPERTIES TIMEOUT 30)

add_executable(test_tool_cert_packets test_cert_packets.cpp ../../tools/certserver/cert_message_handler.cpp)
cqyh_target_defaults(test_tool_cert_packets)
target_include_directories(test_tool_cert_packets PRIVATE ../../tools/certserver ../../inc ../../inc/baselib ../../inc/net ../../3rd/zlib
    "${CQYH_SERVER_DIR}/Common/define" "${CQYH_SERVER_DIR}/Common/message" "${CQYH_SERVER_DIR}/Common/fusion/inc")
target_compile_definitions(test_tool_cert_packets PRIVATE NOMINMAX)
target_link_libraries(test_tool_cert_packets PRIVATE baselib cqyh::zlib cqyh::syslibs)
cqyh_copy_runtime_dlls(test_tool_cert_packets)
add_test(NAME tool_cert_packets COMMAND test_tool_cert_packets)
set_tests_properties(tool_cert_packets PROPERTIES TIMEOUT 30)

add_executable(test_tool_cert_math test_cert_math.cpp ../../tools/certserver/cert_math.cpp)
cqyh_target_defaults(test_tool_cert_math)
target_include_directories(test_tool_cert_math PRIVATE ../../tools/certserver ../../inc ../../inc/baselib ../../inc/net)
target_compile_definitions(test_tool_cert_math PRIVATE NOMINMAX)
add_test(NAME tool_cert_math COMMAND test_tool_cert_math)
set_tests_properties(tool_cert_math PROPERTIES TIMEOUT 30)

add_executable(test_tool_cert_runtime test_cert_runtime.cpp ../../tools/certserver/cert_math.cpp
    ../../tools/certserver/cert_message_handler.cpp)
cqyh_target_defaults(test_tool_cert_runtime)
target_include_directories(test_tool_cert_runtime PRIVATE ../../tools/certserver ../../inc ../../inc/baselib ../../inc/net ../../3rd/zlib
    "${CQYH_SERVER_DIR}/Common/define" "${CQYH_SERVER_DIR}/Common/message" "${CQYH_SERVER_DIR}/Common/fusion/inc")
target_compile_definitions(test_tool_cert_runtime PRIVATE NOMINMAX)
target_link_libraries(test_tool_cert_runtime PRIVATE baselib cqyh::zlib cqyh::syslibs)
cqyh_copy_runtime_dlls(test_tool_cert_runtime)
add_test(NAME tool_cert_runtime COMMAND test_tool_cert_runtime "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/cert runtime")
set_tests_properties(tool_cert_runtime PROPERTIES TIMEOUT 60)

add_executable(test_tool_account_logout test_account_logout.cpp
    ../../tools/account_logout/account_logout/account_logout/account_logout.cpp)
cqyh_target_defaults(test_tool_account_logout)
target_include_directories(test_tool_account_logout PRIVATE ../../tools/account_logout/account_logout/account_logout)
add_test(NAME tool_account_logout COMMAND "${CMAKE_COMMAND}"
    "-DACCOUNT_TEST=$<TARGET_FILE:test_tool_account_logout>"
    "-DACCOUNT_TOOL=$<TARGET_FILE:account_logout>"
    "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/account logout"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/test_account_logout.cmake")
set_tests_properties(tool_account_logout PROPERTIES TIMEOUT 60)

add_executable(test_tool_maintain_sessions test_maintain_sessions.cpp ../../tools/maintaintool/src/session_mgr.cpp)
cqyh_target_defaults(test_tool_maintain_sessions)
target_include_directories(test_tool_maintain_sessions PRIVATE ../../tools/maintaintool/src ../../inc ../../inc/baselib ../../inc/net
    "${CQYH_SERVER_DIR}/Common/define" "${CQYH_SERVER_DIR}/Common/fusion/inc")
target_compile_definitions(test_tool_maintain_sessions PRIVATE NOMINMAX)
target_link_libraries(test_tool_maintain_sessions PRIVATE baselib cqyh::syslibs)
cqyh_copy_runtime_dlls(test_tool_maintain_sessions)
add_test(NAME tool_maintain_sessions COMMAND test_tool_maintain_sessions "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/maintain sessions")
set_tests_properties(tool_maintain_sessions PROPERTIES TIMEOUT 30)

add_executable(test_tool_maintain_files test_maintain_files.cpp ../../tools/maintaintool/src/maintain_files.cpp)
cqyh_target_defaults(test_tool_maintain_files)
target_include_directories(test_tool_maintain_files PRIVATE ../../tools/maintaintool/src)
add_test(NAME tool_maintain_files COMMAND test_tool_maintain_files "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/maintain files")
set_tests_properties(tool_maintain_files PROPERTIES TIMEOUT 30)

add_executable(test_tool_maintain_stats test_maintain_stats.cpp ../../tools/maintaintool/src/maintain_stats.cpp)
cqyh_target_defaults(test_tool_maintain_stats)
target_include_directories(test_tool_maintain_stats PRIVATE ../../tools/maintaintool/src ../../inc/baselib
    "${CQYH_SERVER_DIR}/Common/define")
target_compile_definitions(test_tool_maintain_stats PRIVATE NOMINMAX)
add_test(NAME tool_maintain_stats COMMAND test_tool_maintain_stats)
set_tests_properties(tool_maintain_stats PROPERTIES TIMEOUT 30)

add_executable(test_tool_gm_file_transform test_gm_file_transform.cpp
    ../../tools/gmtool-encrypt/Encrypt/encrypt/base.cpp)
cqyh_target_defaults(test_tool_gm_file_transform STRICT)
target_include_directories(test_tool_gm_file_transform PRIVATE ../../tools/gmtool-encrypt/Encrypt/encrypt)
add_test(NAME tool_gm_file_transform COMMAND test_tool_gm_file_transform)
set_tests_properties(tool_gm_file_transform PROPERTIES TIMEOUT 30)

add_executable(test_tool_gm_configuration test_gm_configuration.cpp ../../tools/gmtool/gm_configuration.cpp)
cqyh_target_defaults(test_tool_gm_configuration STRICT)
target_include_directories(test_tool_gm_configuration PRIVATE ../../tools/gmtool ../../inc/baselib)
target_compile_definitions(test_tool_gm_configuration PRIVATE NOMINMAX)
target_link_libraries(test_tool_gm_configuration PRIVATE baselib cqyh::syslibs)
add_test(NAME tool_gm_configuration COMMAND test_tool_gm_configuration "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/gm configuration"
    "${CQYH_SERVER_DIR}/NewServer/tools/gmtool/bin/bin/config/gmtool_config.lua")
set_tests_properties(tool_gm_configuration PROPERTIES TIMEOUT 30)

add_executable(test_tool_gm_data test_gm_data.cpp ../../tools/gmtool/data_mgr.cpp ../../tools/gmtool/gm_data_packets.cpp)
cqyh_target_defaults(test_tool_gm_data STRICT)
target_include_directories(test_tool_gm_data PRIVATE ../../tools/gmtool ../../inc/baselib ../../inc/gm ../../inc/net
    "${CQYH_SERVER_DIR}/Common/define")
target_compile_definitions(test_tool_gm_data PRIVATE NOMINMAX)
add_test(NAME tool_gm_data COMMAND test_tool_gm_data)
set_tests_properties(tool_gm_data PROPERTIES TIMEOUT 30)

add_executable(test_tool_gm_role_data test_gm_role_data.cpp ../../tools/gmtool/data_mgr.cpp ../../tools/gmtool/gm_role_data.cpp)
cqyh_target_defaults(test_tool_gm_role_data STRICT)
target_include_directories(test_tool_gm_role_data PRIVATE ../../tools/gmtool ../../inc/baselib ../../inc/gm ../../inc/net
    "${CQYH_SERVER_DIR}/Common/define")
target_compile_definitions(test_tool_gm_role_data PRIVATE NOMINMAX)
add_test(NAME tool_gm_role_data COMMAND test_tool_gm_role_data)
set_tests_properties(tool_gm_role_data PROPERTIES TIMEOUT 30)

add_executable(test_tool_gm_commands test_gm_commands.cpp ../../tools/gmtool/cmd_manager.cpp)
cqyh_target_defaults(test_tool_gm_commands STRICT)
target_include_directories(test_tool_gm_commands PRIVATE ../../tools/gmtool ../../inc/baselib ../../inc/gm ../../inc/net
    "${CQYH_SERVER_DIR}/Common/define")
target_compile_definitions(test_tool_gm_commands PRIVATE NOMINMAX)
target_link_libraries(test_tool_gm_commands PRIVATE baselib cqyh::syslibs)
add_test(NAME tool_gm_commands COMMAND test_tool_gm_commands)
set_tests_properties(tool_gm_commands PROPERTIES TIMEOUT 30)

if(CQYH_BUILD_QT_TOOLS)
    find_package(Qt5 COMPONENTS Core Gui Widgets QUIET)
    if(Qt5_FOUND)
        add_executable(test_tool_gm_exchange_widget test_gm_exchange_widget.cpp
            ../../tools/gmtool/gm_log_data.cpp ../../tools/gmtool/gm_log_view.cpp
            ../../tools/gmtool/gm_receive.cpp ../../tools/gmtool/gm_exchange_replies.cpp
            ../../tools/gmtool/ld_widget.cpp ../../tools/gmtool/ld_widget.h
            ../../tools/gmtool/detail.cpp ../../tools/gmtool/detail.h
            ../../tools/gmtool/exchange_limit.cpp ../../tools/gmtool/exchange_limit.h
            ../../tools/gmtool/ui_exchange_history.cpp ../../tools/gmtool/my_tree_widget_item.cpp
            ../../tools/gmtool/gm_item_info.cpp ../../tools/gmtool/gm_item_info.h
            ../../tools/gmtool/gm_exchange_stats.cpp ../../tools/gmtool/gm_reports.cpp)
        cqyh_target_defaults(test_tool_gm_exchange_widget STRICT)
        set_target_properties(test_tool_gm_exchange_widget PROPERTIES AUTOMOC ON AUTOUIC ON)
        target_include_directories(test_tool_gm_exchange_widget PRIVATE ../../tools/gmtool ../../inc/baselib ../../inc/gm ../../inc/net
            "${CQYH_SERVER_DIR}/Common/define" "${CQYH_SERVER_DIR}/Common/fusion/inc")
        target_compile_definitions(test_tool_gm_exchange_widget PRIVATE NOMINMAX)
        target_link_libraries(test_tool_gm_exchange_widget PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets baselib cqyh::syslibs)
        cqyh_copy_runtime_dlls(test_tool_gm_exchange_widget)
        add_test(NAME tool_gm_exchange_widget COMMAND test_tool_gm_exchange_widget "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/gm-exchange-validation.png")
        get_target_property(_gm_exchange_qt_core Qt5::Core IMPORTED_LOCATION_RELEASE)
        get_filename_component(_gm_exchange_qt_bin "${_gm_exchange_qt_core}" DIRECTORY)
        set_tests_properties(tool_gm_exchange_widget PROPERTIES TIMEOUT 30
            ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QPA_PLATFORM_PLUGIN_PATH=${_gm_exchange_qt_bin}/../plugins/platforms")
        add_executable(test_tool_gm_role_widget test_gm_role_widget.cpp
            ../../tools/gmtool/role_info.cpp ../../tools/gmtool/role_info.h
            ../../tools/gmtool/gm_item_info.cpp ../../tools/gmtool/gm_item_info.h
            ../../tools/gmtool/my_tree_widget_item.cpp ../../tools/gmtool/logic.cpp
            ../../tools/gmtool/data_mgr.cpp ../../tools/gmtool/gm_role_data.cpp)
        cqyh_target_defaults(test_tool_gm_role_widget STRICT)
        set_target_properties(test_tool_gm_role_widget PROPERTIES AUTOMOC ON AUTOUIC ON)
        target_include_directories(test_tool_gm_role_widget PRIVATE ../../tools/gmtool ../../inc/baselib ../../inc/gm ../../inc/net
            "${CQYH_SERVER_DIR}/Common/define" "${CQYH_SERVER_DIR}/Common/message" "${CQYH_SERVER_DIR}/Common/fusion/inc")
        target_link_libraries(test_tool_gm_role_widget PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets baselib cqyh::net cqyh::syslibs)
        cqyh_copy_runtime_dlls(test_tool_gm_role_widget)
        add_test(NAME tool_gm_role_widget COMMAND test_tool_gm_role_widget "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/gm-role-validation.png")
        get_target_property(_gm_role_qt_core Qt5::Core IMPORTED_LOCATION_RELEASE)
        get_filename_component(_gm_role_qt_bin "${_gm_role_qt_core}" DIRECTORY)
        set_tests_properties(tool_gm_role_widget PROPERTIES TIMEOUT 30
            ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QPA_PLATFORM_PLUGIN_PATH=${_gm_role_qt_bin}/../plugins/platforms")
        add_executable(test_tool_gm_item_dialog test_gm_item_dialog.cpp
            ../../tools/gmtool/gm_item_info.cpp ../../tools/gmtool/gm_item_info.h)
        cqyh_target_defaults(test_tool_gm_item_dialog STRICT)
        set_target_properties(test_tool_gm_item_dialog PROPERTIES AUTOMOC ON AUTOUIC ON)
        target_include_directories(test_tool_gm_item_dialog PRIVATE ../../tools/gmtool
            ../../inc/baselib "${CQYH_SERVER_DIR}/Common/define")
        target_link_libraries(test_tool_gm_item_dialog PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)
        cqyh_copy_runtime_dlls(test_tool_gm_item_dialog)
        add_test(NAME tool_gm_item_dialog COMMAND test_tool_gm_item_dialog)
        get_target_property(_gm_qt_core Qt5::Core IMPORTED_LOCATION_RELEASE)
        get_filename_component(_gm_qt_bin "${_gm_qt_core}" DIRECTORY)
        set_tests_properties(tool_gm_item_dialog PROPERTIES TIMEOUT 30
            ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QPA_PLATFORM_PLUGIN_PATH=${_gm_qt_bin}/../plugins/platforms")
        add_executable(test_tool_maintain_model test_maintain_model.cpp test_maintain_url_handler.h
            ../../tools/maintaintool/src/maintain_files.cpp
            ../../tools/maintaintool/src/maintain_stats.cpp
            ../../tools/maintaintool/src/maintaintool.cpp ../../tools/maintaintool/src/message_handler.cpp
            ../../tools/maintaintool/src/session_mgr.cpp ../../tools/maintaintool/src/session_ui.cpp
            ../../tools/maintaintool/src/transmit_file_mgr.cpp)
        cqyh_target_defaults(test_tool_maintain_model)
        set_target_properties(test_tool_maintain_model PROPERTIES AUTOMOC ON AUTOUIC ON)
        target_include_directories(test_tool_maintain_model PRIVATE ../../tools/maintaintool/src ../../inc ../../inc/baselib ../../inc/net
            "${CQYH_SERVER_DIR}/Common/define" "${CQYH_SERVER_DIR}/Common/message" "${CQYH_SERVER_DIR}/Common/fusion/inc")
        target_compile_definitions(test_tool_maintain_model PRIVATE NOMINMAX)
        target_link_libraries(test_tool_maintain_model PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets baselib cqyh::net cqyh::lua cqyh::zlib fusion cqyh::syslibs)
        cqyh_copy_runtime_dlls(test_tool_maintain_model)
        get_target_property(_maintain_qt_core Qt5::Core IMPORTED_LOCATION_RELEASE)
        get_filename_component(_maintain_qt_bin "${_maintain_qt_core}" DIRECTORY)
        add_test(NAME tool_maintain_model COMMAND test_tool_maintain_model "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/maintain model")
        set_tests_properties(tool_maintain_model PROPERTIES TIMEOUT 60
            ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QPA_PLATFORM_PLUGIN_PATH=${_maintain_qt_bin}/../plugins/platforms")
        add_executable(test_tool_maintain_runtime test_maintain_runtime.cpp)
        cqyh_target_defaults(test_tool_maintain_runtime)
        target_include_directories(test_tool_maintain_runtime PRIVATE ../../tools/maintaintool/src ../../inc ../../inc/baselib ../../inc/net
            ../../3rd/lua/include "${CQYH_SERVER_DIR}/Common/define" "${CQYH_SERVER_DIR}/Common/message" "${CQYH_SERVER_DIR}/Common/fusion/inc")
        target_compile_definitions(test_tool_maintain_runtime PRIVATE NOMINMAX)
        target_link_libraries(test_tool_maintain_runtime PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets cqyh::syslibs)
        cqyh_copy_runtime_dlls(test_tool_maintain_runtime)
        add_test(NAME tool_maintain_runtime COMMAND test_tool_maintain_runtime)
        set_tests_properties(tool_maintain_runtime PROPERTIES TIMEOUT 30)
    endif()
endif()

add_executable(test_tool_gm_receive test_gm_receive.cpp ../../tools/gmtool/gm_receive.cpp)
cqyh_target_defaults(test_tool_gm_receive STRICT)
target_include_directories(test_tool_gm_receive PRIVATE ../../tools/gmtool ../../inc/baselib ../../inc/net ../../inc/gm
    "${CQYH_SERVER_DIR}/Common/define" "${CQYH_SERVER_DIR}/Common/message")
target_compile_definitions(test_tool_gm_receive PRIVATE NOMINMAX)
target_link_libraries(test_tool_gm_receive PRIVATE baselib cqyh::syslibs)
add_test(NAME tool_gm_receive COMMAND test_tool_gm_receive)
set_tests_properties(tool_gm_receive PROPERTIES TIMEOUT 30)

add_executable(test_tool_gm_receive_packets test_gm_receive_packets.cpp)
cqyh_target_defaults(test_tool_gm_receive_packets STRICT)
target_include_directories(test_tool_gm_receive_packets PRIVATE ../../tools/gmtool ../../inc/baselib ../../inc/net ../../inc/gm
    "${CQYH_SERVER_DIR}/Common/define")
target_compile_definitions(test_tool_gm_receive_packets PRIVATE NOMINMAX)
add_test(NAME tool_gm_receive_packets COMMAND test_tool_gm_receive_packets)
set_tests_properties(tool_gm_receive_packets PROPERTIES TIMEOUT 30)

add_executable(test_tool_gm_send test_gm_send.cpp ../../tools/gmtool/gm_send.cpp
    ../../tools/gmtool/gm_log_receive.cpp
    ../../tools/gmtool/gm_bulk_queries.cpp ../../tools/gmtool/gm_role_replies.cpp
    ../../tools/gmtool/gm_name_queries.cpp ../../tools/gmtool/gm_reports.cpp
    ../../tools/gmtool/gm_receive.cpp ../../tools/gmtool/cmd_manager.cpp)
cqyh_target_defaults(test_tool_gm_send STRICT)
target_include_directories(test_tool_gm_send PRIVATE ../../tools/gmtool ../../inc/baselib ../../inc/net ../../inc/gm
    "${CQYH_SERVER_DIR}/Common/define")
target_compile_definitions(test_tool_gm_send PRIVATE NOMINMAX)
target_link_libraries(test_tool_gm_send PRIVATE baselib cqyh::syslibs)
add_test(NAME tool_gm_send COMMAND test_tool_gm_send "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/gm name queries")
set_tests_properties(tool_gm_send PROPERTIES TIMEOUT 30)

add_executable(test_tool_gm_reports test_gm_reports.cpp ../../tools/gmtool/gm_reports.cpp)
cqyh_target_defaults(test_tool_gm_reports STRICT)
target_include_directories(test_tool_gm_reports PRIVATE ../../tools/gmtool ../../inc/baselib ../../inc/gm
    "${CQYH_SERVER_DIR}/Common/define")
target_compile_definitions(test_tool_gm_reports PRIVATE NOMINMAX)
add_test(NAME tool_gm_reports COMMAND test_tool_gm_reports "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/gm reports")
set_tests_properties(tool_gm_reports PROPERTIES TIMEOUT 30)

add_executable(test_tool_gm_role_files test_gm_role_files.cpp ../../tools/gmtool/gm_role_files.cpp)
cqyh_target_defaults(test_tool_gm_role_files STRICT)
target_include_directories(test_tool_gm_role_files PRIVATE ../../tools/gmtool ../../inc/baselib ../../inc/gm
    "${CQYH_SERVER_DIR}/Common/define")
target_compile_definitions(test_tool_gm_role_files PRIVATE NOMINMAX)
add_test(NAME tool_gm_role_files COMMAND test_tool_gm_role_files "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/gm role files")
set_tests_properties(tool_gm_role_files PROPERTIES TIMEOUT 30)

add_executable(test_tool_gm_exchange_stats test_gm_exchange_stats.cpp ../../tools/gmtool/gm_exchange_stats.cpp ../../tools/gmtool/gm_reports.cpp)
add_executable(test_tool_gm_logging test_gm_logging.cpp ../../tools/gmtool/gm_log.cpp ../../tools/gmtool/gm_log_data.cpp ../../tools/gmtool/gm_reports.cpp)
cqyh_target_defaults(test_tool_gm_logging STRICT)
target_compile_definitions(test_tool_gm_logging PRIVATE NOMINMAX)
target_include_directories(test_tool_gm_logging PRIVATE ../../tools/gmtool ../../inc/baselib ../../inc/gm "${CQYH_SERVER_DIR}/Common/define")
target_link_libraries(test_tool_gm_logging PRIVATE baselib cqyh::syslibs)
add_test(NAME tool_gm_logging COMMAND test_tool_gm_logging "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/gm logging")
set_tests_properties(tool_gm_logging PROPERTIES TIMEOUT 30)
add_executable(test_tool_gm_sessions test_gm_sessions.cpp ../../tools/gmtool/session_mgr.cpp ../../tools/gmtool/gm_session_events.cpp)
cqyh_target_defaults(test_tool_gm_sessions STRICT)
target_compile_definitions(test_tool_gm_sessions PRIVATE NOMINMAX)
target_include_directories(test_tool_gm_sessions PRIVATE ../../tools/gmtool ../../inc/baselib ../../inc/gm ../../inc/net "${CQYH_SERVER_DIR}/Common/define")
target_link_libraries(test_tool_gm_sessions PRIVATE baselib cqyh::syslibs)
add_test(NAME tool_gm_sessions COMMAND test_tool_gm_sessions)
set_tests_properties(tool_gm_sessions PROPERTIES TIMEOUT 30)
add_executable(test_tool_gm_runtime test_gm_runtime.cpp ../../tools/gmtool/gm_manager.cpp ../../tools/gmtool/gm_runtime.cpp
    ../../tools/gmtool/gm_receive.cpp ../../tools/gmtool/gm_send.cpp ../../tools/gmtool/cmd_manager.cpp
    ../../tools/gmtool/session_mgr.cpp ../../tools/gmtool/gm_session_events.cpp)
cqyh_target_defaults(test_tool_gm_runtime STRICT)
target_compile_definitions(test_tool_gm_runtime PRIVATE NOMINMAX)
target_include_directories(test_tool_gm_runtime PRIVATE ../../tools/gmtool ../../inc/baselib ../../inc/gm ../../inc/net "${CQYH_SERVER_DIR}/Common/define")
target_link_libraries(test_tool_gm_runtime PRIVATE baselib cqyh::syslibs)
add_test(NAME tool_gm_runtime COMMAND test_tool_gm_runtime)
set_tests_properties(tool_gm_runtime PROPERTIES TIMEOUT 30)
add_executable(test_tool_gm_native_idle test_gm_native_idle.cpp ../../tools/gmtool/gm_manager.cpp ../../tools/gmtool/gm_runtime.cpp
    ../../tools/gmtool/gm_runtime_native.cpp ../../tools/gmtool/gm_configuration.cpp
    ../../tools/gmtool/gm_receive.cpp ../../tools/gmtool/gm_send.cpp ../../tools/gmtool/cmd_manager.cpp
    ../../tools/gmtool/session_mgr.cpp ../../tools/gmtool/gm_session_events.cpp ../../tools/gmtool/data_mgr.cpp ../../tools/gmtool/gm_role_data.cpp)
cqyh_target_defaults(test_tool_gm_native_idle STRICT)
target_compile_definitions(test_tool_gm_native_idle PRIVATE NOMINMAX)
target_include_directories(test_tool_gm_native_idle PRIVATE ../../tools/gmtool ../../inc/baselib ../../inc/gm ../../inc/net "${CQYH_SERVER_DIR}/Common/define")
target_link_libraries(test_tool_gm_native_idle PRIVATE baselib cqyh::net cqyh::syslibs)
add_test(NAME tool_gm_native_idle COMMAND test_tool_gm_native_idle "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/gm native idle")
set_tests_properties(tool_gm_native_idle PROPERTIES TIMEOUT 30)
if(TARGET gmtool AND TARGET Qt5::Widgets)
    # Compile the actual application, replacing only WinMain with the local lifecycle fixture.
    get_target_property(_gm_application_sources gmtool SOURCES)
    get_target_property(_gm_application_directory gmtool SOURCE_DIR)
    set(_gm_application_test_sources)
    foreach(_source IN LISTS _gm_application_sources)
        if(_source MATCHES "(^|/)main\\.cpp$" OR _source MATCHES "\\.rc$")
            continue()
        endif()
        get_filename_component(_absolute_source "${_source}" ABSOLUTE BASE_DIR "${_gm_application_directory}")
        list(APPEND _gm_application_test_sources "${_absolute_source}")
    endforeach()
    add_executable(test_tool_gm_application test_gm_application.cpp ${_gm_application_test_sources})
    cqyh_target_defaults(test_tool_gm_application STRICT)
    foreach(_property INCLUDE_DIRECTORIES COMPILE_DEFINITIONS LINK_LIBRARIES)
        get_target_property(_value gmtool ${_property})
        set_property(TARGET test_tool_gm_application PROPERTY ${_property} "${_value}")
    endforeach()
    set_target_properties(test_tool_gm_application PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON
        AUTOUIC_SEARCH_PATHS "${_gm_application_directory}/gmtool")
    cqyh_use_pch(test_tool_gm_application "${_gm_application_directory}/gmtool")
    cqyh_copy_runtime_dlls(test_tool_gm_application)
    add_test(NAME tool_gm_application COMMAND test_tool_gm_application "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/gm application")
    set_tests_properties(tool_gm_application PROPERTIES TIMEOUT 30 ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
    set(_gm_auth_test_sources ${_gm_application_test_sources})
    list(FILTER _gm_auth_test_sources EXCLUDE REGEX "/gm_runtime_native\\.cpp$")
    add_executable(test_tool_gm_authentication test_gm_authentication.cpp ${_gm_auth_test_sources})
    cqyh_target_defaults(test_tool_gm_authentication STRICT)
    foreach(_property INCLUDE_DIRECTORIES COMPILE_DEFINITIONS LINK_LIBRARIES)
        get_target_property(_value gmtool ${_property})
        set_property(TARGET test_tool_gm_authentication PROPERTY ${_property} "${_value}")
    endforeach()
    set_target_properties(test_tool_gm_authentication PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON
        AUTOUIC_SEARCH_PATHS "${_gm_application_directory}/gmtool")
    cqyh_use_pch(test_tool_gm_authentication "${_gm_application_directory}/gmtool")
    cqyh_copy_runtime_dlls(test_tool_gm_authentication)
    add_test(NAME tool_gm_authentication COMMAND test_tool_gm_authentication "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/gm-login-validation.png")
    set_tests_properties(tool_gm_authentication PROPERTIES TIMEOUT 30 ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
    add_executable(test_tool_gm_command_replies test_gm_command_replies.cpp ${_gm_auth_test_sources})
    cqyh_target_defaults(test_tool_gm_command_replies STRICT)
    foreach(_property INCLUDE_DIRECTORIES COMPILE_DEFINITIONS LINK_LIBRARIES)
        get_target_property(_value gmtool ${_property})
        set_property(TARGET test_tool_gm_command_replies PROPERTY ${_property} "${_value}")
    endforeach()
    set_target_properties(test_tool_gm_command_replies PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON
        AUTOUIC_SEARCH_PATHS "${_gm_application_directory}/gmtool")
    cqyh_use_pch(test_tool_gm_command_replies "${_gm_application_directory}/gmtool")
    cqyh_copy_runtime_dlls(test_tool_gm_command_replies)
    add_test(NAME tool_gm_command_replies COMMAND test_tool_gm_command_replies "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/gm-command-results.png")
    set_tests_properties(tool_gm_command_replies PROPERTIES TIMEOUT 30 ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
    add_executable(test_tool_gm_window test_gm_window.cpp ${_gm_auth_test_sources})
    cqyh_target_defaults(test_tool_gm_window STRICT)
    foreach(_property INCLUDE_DIRECTORIES COMPILE_DEFINITIONS LINK_LIBRARIES)
        get_target_property(_value gmtool ${_property})
        set_property(TARGET test_tool_gm_window PROPERTY ${_property} "${_value}")
    endforeach()
    set_target_properties(test_tool_gm_window PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON
        AUTOUIC_SEARCH_PATHS "${_gm_application_directory}/gmtool")
    cqyh_use_pch(test_tool_gm_window "${_gm_application_directory}/gmtool")
    cqyh_copy_runtime_dlls(test_tool_gm_window)
    add_test(NAME tool_gm_window COMMAND test_tool_gm_window "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/gm-window-validation.png")
    set_tests_properties(tool_gm_window PROPERTIES TIMEOUT 30 ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
    add_executable(test_tool_gm_bulk_ui test_gm_bulk_ui.cpp ${_gm_auth_test_sources})
    cqyh_target_defaults(test_tool_gm_bulk_ui STRICT)
    foreach(_property INCLUDE_DIRECTORIES COMPILE_DEFINITIONS LINK_LIBRARIES)
        get_target_property(_value gmtool ${_property})
        set_property(TARGET test_tool_gm_bulk_ui PROPERTY ${_property} "${_value}")
    endforeach()
    set_target_properties(test_tool_gm_bulk_ui PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON
        AUTOUIC_SEARCH_PATHS "${_gm_application_directory}/gmtool")
    cqyh_use_pch(test_tool_gm_bulk_ui "${_gm_application_directory}/gmtool")
    cqyh_copy_runtime_dlls(test_tool_gm_bulk_ui)
    add_test(NAME tool_gm_bulk_ui COMMAND test_tool_gm_bulk_ui "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/gm-bulk-input")
    add_executable(test_tool_gm_role_lifetime test_gm_role_lifetime.cpp ${_gm_auth_test_sources})
    cqyh_target_defaults(test_tool_gm_role_lifetime STRICT)
    foreach(_property INCLUDE_DIRECTORIES COMPILE_DEFINITIONS LINK_LIBRARIES)
        get_target_property(_value gmtool ${_property})
        set_property(TARGET test_tool_gm_role_lifetime PROPERTY ${_property} "${_value}")
    endforeach()
    set_target_properties(test_tool_gm_role_lifetime PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON
        AUTOUIC_SEARCH_PATHS "${_gm_application_directory}/gmtool")
    cqyh_use_pch(test_tool_gm_role_lifetime "${_gm_application_directory}/gmtool")
    cqyh_copy_runtime_dlls(test_tool_gm_role_lifetime)
    add_test(NAME tool_gm_role_lifetime COMMAND test_tool_gm_role_lifetime)
    set_tests_properties(tool_gm_application tool_gm_authentication tool_gm_command_replies tool_gm_window tool_gm_bulk_ui tool_gm_role_lifetime
        PROPERTIES TIMEOUT 30
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QPA_PLATFORM_PLUGIN_PATH=$<TARGET_FILE_DIR:Qt5::QOffscreenIntegrationPlugin>")
endif()
cqyh_target_defaults(test_tool_gm_exchange_stats STRICT)
target_include_directories(test_tool_gm_exchange_stats PRIVATE ../../tools/gmtool ../../inc/baselib ../../inc/gm
    "${CQYH_SERVER_DIR}/Common/define")
target_compile_definitions(test_tool_gm_exchange_stats PRIVATE NOMINMAX)
add_test(NAME tool_gm_exchange_stats COMMAND test_tool_gm_exchange_stats "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/gm exchange statistics")
set_tests_properties(tool_gm_exchange_stats PROPERTIES TIMEOUT 30)
