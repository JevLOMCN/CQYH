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

