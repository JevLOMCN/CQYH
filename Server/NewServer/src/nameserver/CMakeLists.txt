# nameserver - translated from nameserver.vcxproj (Release|x64 / Debug|x64 settings).
# The source list is explicit on purpose; add new files here.

add_executable(nameserver
    nameserver.cpp
    ns_message_handler.cpp
    stdafx.cpp
)

cqyh_target_defaults(nameserver EXECUTABLE)
cqyh_use_pch(nameserver ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(nameserver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CQYH_SERVER_DIR}/Common/define
    ${CQYH_SERVER_DIR}/Common/message
    ${CQYH_SERVER_DIR}/Common/fusion/inc
    ${CQYH_SERVER_DIR}/NewServer/inc
    ${CQYH_SERVER_DIR}/NewServer/inc/baselib
    ${CQYH_SERVER_DIR}/NewServer/inc/net
    ${CQYH_SERVER_DIR}/NewServer/inc/gameworld
    ${CQYH_SERVER_DIR}/NewServer/3rd/lua/include
    ${CQYH_SERVER_DIR}/NewServer/3rd/gpk/Include
    ${CQYH_SERVER_DIR}/NewServer/3rd/zlib
)
target_compile_definitions(nameserver PRIVATE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
    _CRT_SECURE_NO_DEPRECATE
)
# The MySQL server these servers run against is 5.7, which only offers mysql_native_password.
# The 26.x client in cqyh::mysql no longer ships that plugin, so link the 5.1.65 client like dbserver does.
set(NS_MYSQL51_DIR ${CQYH_SERVER_DIR}/NewServer/3rd/mysql-5.1.65)
add_library(nameserver_mysql51 SHARED IMPORTED)
set_target_properties(nameserver_mysql51 PROPERTIES
    IMPORTED_IMPLIB_DEBUG     ${NS_MYSQL51_DIR}/lib/win64/libmysql64_d.lib
    IMPORTED_LOCATION_DEBUG   ${NS_MYSQL51_DIR}/lib/win64/libmysql64_d.dll
    IMPORTED_IMPLIB_RELEASE   ${NS_MYSQL51_DIR}/lib/win64/libmysql64.lib
    IMPORTED_LOCATION_RELEASE ${NS_MYSQL51_DIR}/lib/win64/libmysql64.dll
    INTERFACE_INCLUDE_DIRECTORIES ${NS_MYSQL51_DIR}/include)
target_link_libraries(nameserver PRIVATE
    baselib
    cqyh::lua
    cqyh::sol2
    cqyh::net
    cqyh::zlib
    cqyh::gpk
    fusion
    nameserver_mysql51
    cqyh::syslibs
)
cqyh_server_output(nameserver nameserver64)
add_custom_command(TARGET nameserver POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:nameserver_mysql51> $<TARGET_FILE_DIR:nameserver>)
