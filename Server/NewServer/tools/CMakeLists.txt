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

    # Keep tool-owned binaries and symbols together while sharing the build's libraries.
    set(_tool_output_dir "${CMAKE_BINARY_DIR}/NewServer/tools/${tgt}/$<CONFIG>")
    set_target_properties(${tgt} PROPERTIES
        OUTPUT_NAME ${A_OUTPUT}
        RUNTIME_OUTPUT_DIRECTORY "${_tool_output_dir}"
        ARCHIVE_OUTPUT_DIRECTORY "${_tool_output_dir}"
        PDB_OUTPUT_DIRECTORY "${_tool_output_dir}"
        FOLDER "Tools")
    cqyh_copy_runtime_dlls(${tgt})
    if(A_EXCLUDE_FROM_ALL)
        set_target_properties(${tgt} PROPERTIES EXCLUDE_FROM_ALL ON EXCLUDE_FROM_DEFAULT_BUILD ON)
    endif()
endfunction()

# --- console tools on top of the server libraries -------------------------------------------------------------

cqyh_add_tool(guardserver OUTPUT guardserver64 STRICT PCH_DIR ${_tools_dir}/guardserver
    SOURCES
        guardserver/cmd_mgr.cpp
        guardserver/gd_message_handler.cpp
        guardserver/gd_session_mgr.cpp
        guardserver/guardserver.cpp
        guardserver/guard_files.cpp
        guardserver/guard_deploy.cpp
        guardserver/main.cpp
        guardserver/process_mgr.cpp
        guardserver/stdafx.cpp
    INCLUDES ${_tools_dir}/guardserver
    DEFINITIONS WIN32_LEAN_AND_MEAN NOMINMAX
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

cqyh_add_tool(certserver OUTPUT certserver64 GUI STRICT PCH_DIR ${_tools_dir}/certserver
    SOURCES
        certserver/cert_math.cpp
        certserver/cert_message_handler.cpp
        certserver/certserver.cpp
        certserver/stdafx.cpp
    INCLUDES ${_tools_dir}/certserver
    DEFINITIONS NOMINMAX
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

cqyh_add_tool(account_logout OUTPUT account_logout64 STRICT
    SOURCES
        account_logout/account_logout/account_logout/account_logout.cpp
        account_logout/account_logout/account_logout/main.cpp
    INCLUDES ${_tools_dir}/account_logout/account_logout/account_logout
    DEFINITIONS NOMINMAX)

cqyh_add_tool(gmtool_label_version OUTPUT gmtool_label_version64 STRICT
    SOURCES gmtool_label_version/gmtool_label_version/main.cpp
    DEFINITIONS _LABEL _CRT_SECURE_NO_WARNINGS)

# --- MFC tools ---------------------------------------------------------------------------------------------------

if(CQYH_BUILD_MFC_TOOLS AND MSVC)
    cqyh_add_tool(encrypt OUTPUT encrypt64 GUI MFC STRICT PCH_DIR ${_tools_dir}/gmtool-encrypt/Encrypt/encrypt
        SOURCES
            gmtool-encrypt/Encrypt/encrypt/base.cpp
            gmtool-encrypt/Encrypt/encrypt/file_transform.cpp
            gmtool-encrypt/Encrypt/encrypt/encrypt.cpp
            gmtool-encrypt/Encrypt/encrypt/encryptDlg.cpp
            gmtool-encrypt/Encrypt/encrypt/stdafx.cpp
            gmtool-encrypt/Encrypt/encrypt/encrypt.rc
        INCLUDES ${_tools_dir}/gmtool-encrypt/Encrypt/encrypt
        DEFINITIONS _UNICODE UNICODE
        LIBS cqyh::zlib)

    cqyh_add_tool(PosLog OUTPUT PosLog64 GUI MFC STRICT PCH_DIR ${_tools_dir}/PosLog
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
    if(NOT Qt5_FOUND)
        message(FATAL_ERROR
            "Qt5 Core, Gui and Widgets development packages are required for GMTool and MaintainTool.\n"
            "Install a Qt5 SDK matching your compiler and architecture (MSVC x64 on Windows).\n"
            "Set CMAKE_PREFIX_PATH to the SDK root, for example:\n"
            "  cmake -S Server -B Server/build -DCMAKE_PREFIX_PATH=\"C:/Qt/5.15.2/msvc2019_64\"\n"
            "Or set Qt5_DIR to the SDK's lib/cmake/Qt5 directory.\n"
            "Use your local SDK path; runtime DLLs alone do not include development headers.\n"
            "To build without the Qt tools, configure with -DCQYH_BUILD_QT_TOOLS=OFF.")
    endif()
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    set(_qt_libs Qt5::Core Qt5::Gui Qt5::Widgets)

    cqyh_add_tool(gmtool OUTPUT gmtool64 GUI STRICT PCH_DIR ${_tools_dir}/gmtool
        SOURCES
            gmtool-encrypt/Encrypt/encrypt/base.cpp
            gmtool/cmd_manager.cpp gmtool/data_mgr.cpp gmtool/gm_data_packets.cpp gmtool/gm_role_data.cpp
            gmtool/delete_item.cpp gmtool/detail.cpp gmtool/exchange_limit.cpp gmtool/executing.cpp
            gmtool/forbidden_chat.cpp gmtool/gm_item_info.cpp gmtool/gm_log.cpp gmtool/gm_manager.cpp gmtool/gm_application.cpp gmtool/gm_runtime.cpp gmtool/gm_runtime_native.cpp gmtool/gm_configuration.cpp
            gmtool/gmtool.cpp gmtool/group_select.cpp gmtool/item_tree_widget_item.cpp gmtool/kick_user.cpp
            gmtool/ld_widget.cpp gmtool/loader.cpp gmtool/logic.cpp gmtool/mail.cpp gmtool/main.cpp
            gmtool/message_handler.cpp gmtool/gm_receive.cpp gmtool/gm_send.cpp gmtool/gm_name_queries.cpp gmtool/gm_reports.cpp gmtool/gm_role_files.cpp gmtool/gm_bulk_queries.cpp gmtool/gm_role_replies.cpp gmtool/gm_exchange_stats.cpp gmtool/gm_exchange_replies.cpp gmtool/gm_log_data.cpp gmtool/gm_log_view.cpp gmtool/gm_log_receive.cpp gmtool/gm_session_events.cpp gmtool/move_position.cpp gmtool/my_tree_widget_item.cpp
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

    cqyh_add_tool(maintaintool OUTPUT maintaintool64 GUI STRICT PCH_DIR ${_tools_dir}/maintaintool/src
        SOURCES
            maintaintool/src/main.cpp
            maintaintool/src/maintaintool.cpp
            maintaintool/src/maintain_files.cpp
            maintaintool/src/maintain_stats.cpp
            maintaintool/src/message_handler.cpp
            maintaintool/src/session_mgr.cpp
            maintaintool/src/session_ui.cpp
            maintaintool/src/stdafx.cpp
            maintaintool/src/transmit_file_mgr.cpp
        INCLUDES ${_tools_dir}/maintaintool/src
        LIBS ${_qt_libs} baselib cqyh::net cqyh::lua cqyh::zlib fusion cqyh::syslibs)

    # Qt loads platform plugins dynamically, so TARGET_RUNTIME_DLLS cannot find them.
    if(WIN32 AND Qt5_FOUND)
        foreach(_qt_tool IN ITEMS gmtool maintaintool)
            add_custom_command(TARGET ${_qt_tool} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${_qt_tool}>/platforms"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:Qt5::QWindowsIntegrationPlugin>"
                    "$<TARGET_FILE_DIR:${_qt_tool}>/platforms/"
                VERBATIM)
        endforeach()
    endif()
endif()
