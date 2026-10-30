# Command line / GUI tools - translated from the per-tool .vcxproj/.vcproj files (Release|x64 settings).
# They show up in the Visual Studio solution under the "Tools" folder.
# The source lists are explicit on purpose; add new files here.
#
# Not built here: baselib/ and net/ (stale copies of the libraries in ../src), "gmtool - 副本" (a copy of gmtool),
# autobuild/ (python scripts).

option(CQYH_BUILD_QT_TOOLS "Build the Qt GUI tools (gmtool, maintaintool); needs Qt5 (set CMAKE_PREFIX_PATH)" ON)
option(CQYH_BUILD_MFC_TOOLS "Build the MFC GUI tools (encrypt, PosLog); needs the VS MFC component" ON)

set(_tools_dir "${CMAKE_CURRENT_SOURCE_DIR}")

# cqyh_add_tool(<name> OUTPUT <exe name> [GUI] [MFC] [STRICT] [EXCLUDE_FROM_ALL] [PCH_DIR <dir>]
#               SOURCES ... [INCLUDES ...] [DEFINITIONS ...] [LIBS ...])
function(cqyh_add_tool tgt)
    cmake_parse_arguments(A "GUI;MFC;STRICT;EXCLUDE_FROM_ALL" "OUTPUT;PCH_DIR" "SOURCES;INCLUDES;DEFINITIONS;LIBS" ${ARGN})

    if(A_GUI)
        add_executable(${tgt} WIN32 ${A_SOURCES})
    else()
        add_executable(${tgt} ${A_SOURCES})
    endif()

    if(A_MFC)
        # Match the dependencies' /MD[d] runtime. Persist this per target;
        # a function-local CMAKE_MFC_FLAG does not survive project generation.
        set_target_properties(${tgt} PROPERTIES VS_GLOBAL_UseOfMfc Dynamic)
        target_compile_definitions(${tgt} PRIVATE _AFXDLL)
    endif()

    if(A_STRICT)
        cqyh_target_defaults(${tgt} EXECUTABLE STRICT)
    else()
        cqyh_target_defaults(${tgt} EXECUTABLE)
    endif()
    if(A_MFC)
        # Both MFC tools use Unicode. Do not inherit the server's _MBCS switch,
        # which makes the generated VS project select the wrong GUI startup.
        get_target_property(_mfc_definitions ${tgt} COMPILE_DEFINITIONS)
        list(REMOVE_ITEM _mfc_definitions _MBCS)
        set_target_properties(${tgt} PROPERTIES COMPILE_DEFINITIONS "${_mfc_definitions}")
    endif()
    if(A_PCH_DIR)
        cqyh_use_pch(${tgt} ${A_PCH_DIR})
    endif()

    target_include_directories(${tgt} PRIVATE
        ${A_INCLUDES}
        ${CQYH_SERVER_DIR}/Common/define
        ${CQYH_SERVER_DIR}/Common/message
        ${CQYH_SERVER_DIR}/Common/fusion/inc
        ${CQYH_SERVER_DIR}/NewServer/inc
        ${CQYH_SERVER_DIR}/NewServer/inc/baselib
        ${CQYH_SERVER_DIR}/NewServer/inc/net
        ${CQYH_SERVER_DIR}/NewServer/3rd/lua/include
        ${CQYH_SERVER_DIR}/NewServer/3rd/zlib
    )
    target_compile_definitions(${tgt} PRIVATE _CRT_SECURE_NO_DEPRECATE ${A_DEFINITIONS})
    if(A_LIBS)
        target_link_libraries(${tgt} PRIVATE ${A_LIBS})
    endif()

    cqyh_server_output(${tgt} ${A_OUTPUT})
    set_target_properties(${tgt} PROPERTIES FOLDER "Tools")
    if(A_EXCLUDE_FROM_ALL)
        set_target_properties(${tgt} PROPERTIES EXCLUDE_FROM_ALL ON EXCLUDE_FROM_DEFAULT_BUILD ON)
    endif()
endfunction()

# --- console tools on top of the server libraries -------------------------------------------------------------

cqyh_add_tool(guardserver OUTPUT guardserver64 PCH_DIR ${_tools_dir}/guardserver
    SOURCES
        guardserver/cmd_mgr.cpp
        guardserver/gd_message_handler.cpp
        guardserver/gd_session_mgr.cpp
        guardserver/guardserver.cpp
        guardserver/main.cpp
        guardserver/process_mgr.cpp
        guardserver/stdafx.cpp
    INCLUDES ${_tools_dir}/guardserver
    DEFINITIONS WIN32_LEAN_AND_MEAN
    LIBS baselib cqyh::net cqyh::lua cqyh::zlib fusion cqyh::syslibs shlwapi)

cqyh_add_tool(exportdb OUTPUT exportdb64 STRICT PCH_DIR ${_tools_dir}/exportdb
    SOURCES
        exportdb/exportdb.cpp
        exportdb/export_sql.cpp
        exportdb/role_records.cpp
        exportdb/role_gameworld.cpp
        exportdb/guild.cpp
        exportdb/stdafx.cpp
    INCLUDES
        ${_tools_dir}/exportdb
        ${CQYH_SERVER_DIR}/NewServer/inc/gameworld
        ${CQYH_SERVER_DIR}/NewServer/3rd/gpk/Include
        ${CQYH_SERVER_DIR}/NewServer/src/manageserver
    DEFINITIONS NOMINMAX
    LIBS gameworld baselib cqyh::net cqyh::lua cqyh::sol2 cqyh::zlib cqyh::gpk cqyh::mysql fusion cqyh::syslibs)

cqyh_add_tool(analysis_log OUTPUT analysis_log64 STRICT PCH_DIR ${_tools_dir}/analysis_log
    SOURCES
        analysis_log/analysis_log.cpp
        analysis_log/analysis_records.cpp
        analysis_log/iocp_mgr.cpp
        analysis_log/stdafx.cpp
    INCLUDES ${_tools_dir}/analysis_log)

cqyh_add_tool(common_log OUTPUT common_log64 STRICT PCH_DIR ${_tools_dir}/common_log
    SOURCES
        common_log/anticheat_log.cpp
        common_log/common_log.cpp
        common_log/main.cpp
        common_log/stdafx.cpp
    INCLUDES ${_tools_dir}/common_log
    DEFINITIONS _CRT_SECURE_NO_WARNINGS)

cqyh_add_tool(RoleLogCheck OUTPUT RoleLogCheck64 STRICT PCH_DIR ${_tools_dir}/RoleLogCheck/RoleLogCheck
    SOURCES
        RoleLogCheck/RoleLogCheck/main.cpp
        RoleLogCheck/RoleLogCheck/rolelogcheck.cpp
        RoleLogCheck/RoleLogCheck/stdafx.cpp
    INCLUDES ${_tools_dir}/RoleLogCheck/RoleLogCheck
    DEFINITIONS NOMINMAX
    LIBS baselib cqyh::lua cqyh::zlib fusion cqyh::syslibs)

cqyh_add_tool(cash_accounting OUTPUT cash_accounting64 STRICT PCH_DIR ${_tools_dir}/cash_accounting/cash_accounting
    SOURCES
        cash_accounting/cash_accounting/cash_accounting.cpp
        cash_accounting/cash_accounting/cash_accounting_stmt.list.cpp
        cash_accounting/cash_accounting/main.cpp
        cash_accounting/cash_accounting/stdafx.cpp
    INCLUDES ${_tools_dir}/cash_accounting/cash_accounting
    DEFINITIONS NOMINMAX
    LIBS baselib cqyh::mysql cqyh::syslibs)

cqyh_add_tool(certserver OUTPUT certserver64 PCH_DIR ${_tools_dir}/certserver
    SOURCES
        certserver/cert_message_handler.cpp
        certserver/certserver.cpp
        certserver/stdafx.cpp
    INCLUDES ${_tools_dir}/certserver
    LIBS baselib cqyh::net cqyh::zlib cqyh::syslibs)

cqyh_add_tool(pak OUTPUT pak64 STRICT PCH_DIR ${_tools_dir}/pak
    SOURCES
        pak/pak.cpp
        pak/pak_file.cpp
        pak/stdafx.cpp
    INCLUDES ${_tools_dir}/pak
    DEFINITIONS NOMINMAX
    LIBS baselib cqyh::zlib cqyh::syslibs)

cqyh_add_tool(update_string_map OUTPUT update_string_map64 STRICT PCH_DIR ${_tools_dir}/update_string_map
    SOURCES
        update_string_map/stdafx.cpp
        update_string_map/update_string_map.cpp
    INCLUDES ${_tools_dir}/update_string_map
    DEFINITIONS NOMINMAX WIN32_LEAN_AND_MEAN)

cqyh_add_tool(account_logout OUTPUT account_logout64 PCH_DIR ${_tools_dir}/account_logout/account_logout/account_logout
    SOURCES
        account_logout/account_logout/account_logout/account_logout.cpp
        account_logout/account_logout/account_logout/main.cpp
    INCLUDES ${_tools_dir}/account_logout/account_logout/account_logout
    LIBS baselib cqyh::syslibs)

cqyh_add_tool(gmtool_label_version OUTPUT gmtool_label_version64 STRICT
    SOURCES gmtool_label_version/gmtool_label_version/main.cpp
    DEFINITIONS _LABEL _CRT_SECURE_NO_WARNINGS)

# --- MFC tools ---------------------------------------------------------------------------------------------------

if(CQYH_BUILD_MFC_TOOLS AND MSVC)
    cqyh_add_tool(encrypt OUTPUT encrypt64 GUI MFC PCH_DIR ${_tools_dir}/gmtool-encrypt/Encrypt/encrypt
        SOURCES
            gmtool-encrypt/Encrypt/encrypt/base.cpp
            gmtool-encrypt/Encrypt/encrypt/encrypt.cpp
            gmtool-encrypt/Encrypt/encrypt/encryptDlg.cpp
            gmtool-encrypt/Encrypt/encrypt/stdafx.cpp
            gmtool-encrypt/Encrypt/encrypt/encrypt.rc
        INCLUDES ${_tools_dir}/gmtool-encrypt/Encrypt/encrypt
        DEFINITIONS _UNICODE UNICODE
        LIBS cqyh::zlib)

    cqyh_add_tool(PosLog OUTPUT PosLog64 GUI MFC PCH_DIR ${_tools_dir}/PosLog
        SOURCES
            PosLog/bmp_file.cpp
            PosLog/common_log.cpp
            PosLog/PosLog.cpp
            PosLog/PosLogDlg.cpp
            PosLog/stdafx.cpp
            PosLog/PosLog.rc
        INCLUDES ${_tools_dir}/PosLog
        DEFINITIONS _UNICODE UNICODE _CRT_SECURE_NO_WARNINGS)
endif()

# --- Qt tools ----------------------------------------------------------------------------------------------------

if(CQYH_BUILD_QT_TOOLS)
    find_package(Qt5 COMPONENTS Core Gui Widgets QUIET)
    if(Qt5_FOUND)
        set(CMAKE_AUTOMOC ON)
        set(CMAKE_AUTOUIC ON)
        set(CMAKE_AUTORCC ON)
        set(_qt_libs Qt5::Core Qt5::Gui Qt5::Widgets)
        set(_qt_exclude)
    else()
        # Keep the projects visible in the solution so the sources can be browsed, but do not build them.
        message(STATUS "Qt5 not found - gmtool and maintaintool are listed but excluded from the build "
                       "(set CMAKE_PREFIX_PATH to your Qt5 install to build them)")
        set(_qt_libs)
        set(_qt_exclude EXCLUDE_FROM_ALL)
    endif()

    cqyh_add_tool(gmtool OUTPUT gmtool64 GUI ${_qt_exclude} PCH_DIR ${_tools_dir}/gmtool
        SOURCES
            gmtool-encrypt/Encrypt/encrypt/base.cpp
            gmtool/cmd_data.cpp gmtool/cmd_manager.cpp gmtool/cmd_manager_message.cpp gmtool/data_mgr.cpp
            gmtool/delete_item.cpp gmtool/detail.cpp gmtool/exchange_limit.cpp gmtool/executing.cpp
            gmtool/forbidden_chat.cpp gmtool/gm_item_info.cpp gmtool/gm_log.cpp gmtool/gm_manager.cpp
            gmtool/gmtool.cpp gmtool/group_select.cpp gmtool/item_tree_widget_item.cpp gmtool/kick_user.cpp
            gmtool/ld_widget.cpp gmtool/loader.cpp gmtool/logic.cpp gmtool/mail.cpp gmtool/main.cpp
            gmtool/message_handler.cpp gmtool/move_position.cpp gmtool/my_tree_widget_item.cpp
            gmtool/role_gm_cmd.cpp gmtool/role_info.cpp gmtool/server_list.cpp gmtool/server_mgr.cpp
            gmtool/session_mgr.cpp gmtool/stdafx.cpp gmtool/svr_select.cpp gmtool/svr_tree_widget_item.cpp
            gmtool/ui_exchange_history.cpp gmtool/view_friend.cpp gmtool/view_guild.cpp
            gmtool/gmtool.qrc gmtool/gmtool.rc
        INCLUDES
            ${_tools_dir}/gmtool
            ${CQYH_SERVER_DIR}/NewServer/inc/gm
            ${CQYH_SERVER_DIR}/NewServer/3rd/gpk/Include
        DEFINITIONS GMTOOL QT_DLL
        LIBS ${_qt_libs} baselib cqyh::net cqyh::lua cqyh::zlib fusion cqyh::syslibs)

    cqyh_add_tool(maintaintool OUTPUT maintaintool64 GUI ${_qt_exclude} PCH_DIR ${_tools_dir}/maintaintool/src
        SOURCES
            maintaintool/src/main.cpp
            maintaintool/src/maintaintool.cpp
            maintaintool/src/message_handler.cpp
            maintaintool/src/session_mgr.cpp
            maintaintool/src/stdafx.cpp
            maintaintool/src/transmit_file_mgr.cpp
        INCLUDES ${_tools_dir}/maintaintool/src
        LIBS ${_qt_libs} baselib cqyh::net cqyh::lua cqyh::zlib fusion cqyh::syslibs)
endif()
