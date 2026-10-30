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

add_executable(test_tool_gm_file_transform test_gm_file_transform.cpp
    ../../tools/gmtool-encrypt/Encrypt/encrypt/base.cpp)
cqyh_target_defaults(test_tool_gm_file_transform STRICT)
target_include_directories(test_tool_gm_file_transform PRIVATE ../../tools/gmtool-encrypt/Encrypt/encrypt)
add_test(NAME tool_gm_file_transform COMMAND test_tool_gm_file_transform)
set_tests_properties(tool_gm_file_transform PROPERTIES TIMEOUT 30)

if(CQYH_BUILD_QT_TOOLS)
    find_package(Qt5 COMPONENTS Core Gui Widgets QUIET)
    if(Qt5_FOUND)
    endif()
endif()

