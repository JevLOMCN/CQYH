cmake_minimum_required(VERSION 3.25)

project(CQYHServer LANGUAGES C CXX)

if(MSVC)
    enable_language(ASM_MASM)   # baselib/code.asm
endif()

# Root of the server tree (this directory); the sub-projects refer to everything relative to it.
set(CQYH_SERVER_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)   # Visual Studio solution folders; CMAKE_FOLDER is the default for new targets

include(CQYH)

set(CMAKE_FOLDER "Dependencies/ThirdParty")
include(ThirdParty)
include(Sol2)
include(Asio)

# Static libraries
set(CMAKE_FOLDER "Dependencies/Libraries")
add_subdirectory(Common/fusion)
add_subdirectory(NewServer/src/baselib)
add_subdirectory(NewServer/src/net)
add_subdirectory(NewServer/src/gameworld)

# Servers
set(CMAKE_FOLDER "Servers")
foreach(server
        gateserver loginserver balanceserver dbserver datalogserver
        gameserver manageserver gmserver mgateserver mobileserver gen_msg)
    add_subdirectory(NewServer/src/${server})
endforeach()

# Tools (they set their own "Tools" folder)
option(CQYH_BUILD_TOOLS "Build the command line / GUI tools in NewServer/tools" ON)
if(CQYH_BUILD_TOOLS)
    add_subdirectory(NewServer/tools)
endif()

# Tests
set(CMAKE_FOLDER "Tests")
option(CQYH_BUILD_TESTS "Build the test programs (ctest)" ON)
if(CQYH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(NewServer/test/baselib)
    add_subdirectory(NewServer/test/zlib)
    add_subdirectory(NewServer/test/asio_net)
    add_subdirectory(NewServer/test/lua_bindings)
    if(WIN32)
        add_subdirectory(NewServer/test/mysql)
    endif()
endif()

# nameserver was not part of the original universe.sln, so it remains opt-in.
option(CQYH_BUILD_NAMESERVER "Build nameserver (not in the original universe.sln)" OFF)
if(CQYH_BUILD_NAMESERVER)
    add_subdirectory(NewServer/src/nameserver)
endif()
