# Integration test of the asio transport (see test_asio_net.cpp). Run with: ctest -C <config> --output-on-failure

add_executable(test_asio_net test_asio_net.cpp)

# Keep the transport test independent of the legacy stdafx.h include order.
cqyh_target_defaults(test_asio_net EXECUTABLE)
target_compile_definitions(test_asio_net PRIVATE NOMINMAX)

target_include_directories(test_asio_net PRIVATE
    ${CQYH_SERVER_DIR}/NewServer/inc
    ${CQYH_SERVER_DIR}/NewServer/inc/baselib
    ${CQYH_SERVER_DIR}/NewServer/inc/net
    ${CQYH_SERVER_DIR}/Common/define
    ${CQYH_SERVER_DIR}/Common/message
)

target_link_libraries(test_asio_net PRIVATE
    cqyh::net
    baselib
    cqyh::lua
    cqyh::sol2
    cqyh::zlib
    fusion
    cqyh::syslibs
    asio::asio
)

add_test(NAME asio_net COMMAND test_asio_net)
set_tests_properties(asio_net PROPERTIES TIMEOUT 300)
