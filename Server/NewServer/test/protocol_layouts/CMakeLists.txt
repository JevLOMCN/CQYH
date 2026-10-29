# The frozen contracts were measured with MSVC on x64, using the former build settings.
if(MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_executable(test_protocol_layouts test_protocol_layouts.cpp windows_x64_layouts.inc)
    cqyh_target_defaults(test_protocol_layouts)
    target_compile_definitions(test_protocol_layouts PRIVATE NOMINMAX)
    target_include_directories(test_protocol_layouts PRIVATE
        "${CQYH_SERVER_DIR}/Common/message"
        "${CQYH_SERVER_DIR}/Common/define"
        "${CQYH_SERVER_DIR}/Common/fusion/inc"
        "${CQYH_SERVER_DIR}/NewServer/inc"
        "${CQYH_SERVER_DIR}/NewServer/inc/baselib"
        "${CQYH_SERVER_DIR}/NewServer/inc/net")
    add_test(NAME protocol_layouts COMMAND test_protocol_layouts)
    set_tests_properties(protocol_layouts PROPERTIES TIMEOUT 15)
endif()
