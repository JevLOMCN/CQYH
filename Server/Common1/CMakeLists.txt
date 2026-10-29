cmake_minimum_required(VERSION 3.25)
project(CQYHCommon1 LANGUAGES C CXX)

# Common1 is an alternative protocol, built and tested independently of Common.
get_filename_component(CQYH_SERVER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
list(APPEND CMAKE_MODULE_PATH "${CQYH_SERVER_DIR}/cmake")
include(CQYH)
include(ThirdParty)
include(Sol2)
if(MSVC)
    enable_language(ASM_MASM)
endif()

add_subdirectory(fusion)
add_subdirectory("${CQYH_SERVER_DIR}/NewServer/src/baselib" baselib)
# The shared implementation must see this variant's definitions in this build.
get_target_property(common1_baselib_includes baselib INCLUDE_DIRECTORIES)
string(REPLACE "/Common/" "/Common1/" common1_baselib_includes "${common1_baselib_includes}")
set_property(TARGET baselib PROPERTY INCLUDE_DIRECTORIES "${common1_baselib_includes}")

enable_testing()
add_executable(test_common1_lua "${CQYH_SERVER_DIR}/NewServer/test/lua_bindings/test_lua_fusion_bindings.cpp")
cqyh_target_defaults(test_common1_lua)
target_include_directories(test_common1_lua PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/define" "${CMAKE_CURRENT_SOURCE_DIR}/fusion/inc"
    "${CMAKE_CURRENT_SOURCE_DIR}/fusion/src")
target_compile_definitions(test_common1_lua PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_link_libraries(test_common1_lua PRIVATE fusion baselib cqyh::sol2 cqyh::syslibs)
cqyh_copy_runtime_dlls(test_common1_lua)
add_test(NAME common1_lua COMMAND test_common1_lua)
set_tests_properties(common1_lua PROPERTIES TIMEOUT 30)

if(MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_executable(test_common1_protocol_layouts
        "${CQYH_SERVER_DIR}/NewServer/test/protocol_layouts/test_common1_protocol_layouts.cpp"
        "${CQYH_SERVER_DIR}/NewServer/test/protocol_layouts/test_common1_mg2c_layouts.cpp"
        "${CQYH_SERVER_DIR}/NewServer/test/protocol_layouts/common1_windows_x64_layouts.inc")
    cqyh_target_defaults(test_common1_protocol_layouts)
    target_include_directories(test_common1_protocol_layouts PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/message" "${CMAKE_CURRENT_SOURCE_DIR}/define"
        "${CMAKE_CURRENT_SOURCE_DIR}/fusion/inc" "${CQYH_SERVER_DIR}/NewServer/inc/baselib")
    target_compile_definitions(test_common1_protocol_layouts PRIVATE NOMINMAX)
    add_test(NAME common1_protocol_layouts COMMAND test_common1_protocol_layouts)
    set_tests_properties(common1_protocol_layouts PROPERTIES TIMEOUT 15)
endif()
