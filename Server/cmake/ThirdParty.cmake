# Third-party libraries that live in NewServer/3rd.
#
# Windows : lib/win64 import/static libs (+ the DLLs that go with the import libs)
# Linux   : lib/linux64 libraries where the repo has them.
# Lua and zlib are built from their bundled sources on all platforms.
# A missing Linux library only fails when something links against it.

set(_3rd "${CQYH_SERVER_DIR}/NewServer/3rd")
include(Lua)
include(Zlib)

# Debug/Release selection for imported libs; RelWithDebInfo/MinSizeRel use the Release one.
function(_cqyh_imported name kind)
    cmake_parse_arguments(A "" "RELEASE;DEBUG;DLL_RELEASE;DLL_DEBUG" "" ${ARGN})
    add_library(cqyh::${name} ${kind} IMPORTED GLOBAL)
    set_target_properties(cqyh::${name} PROPERTIES
        IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE
        MAP_IMPORTED_CONFIG_MINSIZEREL RELEASE)
    if(kind STREQUAL "SHARED" AND WIN32)
        set_target_properties(cqyh::${name} PROPERTIES
            IMPORTED_IMPLIB_RELEASE   "${A_RELEASE}"
            IMPORTED_IMPLIB_DEBUG     "${A_DEBUG}"
            IMPORTED_LOCATION_RELEASE "${A_DLL_RELEASE}"
            IMPORTED_LOCATION_DEBUG   "${A_DLL_DEBUG}")
    else()
        set_target_properties(cqyh::${name} PROPERTIES
            IMPORTED_LOCATION_RELEASE "${A_RELEASE}"
            IMPORTED_LOCATION_DEBUG   "${A_DEBUG}")
    endif()
endfunction()

if(WIN32)

    if(CQYH_USE_LEGACY_MYSQL51)
        # The 9-series client below cannot log in to MySQL 5.7 servers (mysql_native_password was removed),
        # so this switch links the bundled 5.1 client instead.
        _cqyh_imported(mysql SHARED
            RELEASE "${_3rd}/mysql-5.1.65/lib/win64/libmysql64.lib"   DLL_RELEASE "${_3rd}/mysql-5.1.65/lib/win64/libmysql64.dll"
            DEBUG   "${_3rd}/mysql-5.1.65/lib/win64/libmysql64_d.lib" DLL_DEBUG   "${_3rd}/mysql-5.1.65/lib/win64/libmysql64_d.dll")
        set(CQYH_MYSQL_RUNTIME_DLLS "")
    else()
        # Oracle ships one C API client for both Debug and Release applications.
        _cqyh_imported(mysql SHARED
            RELEASE "${_3rd}/mysql-26.10.0/lib/win64/libmysql.lib" DLL_RELEASE "${_3rd}/mysql-26.10.0/lib/win64/libmysql.dll"
            DEBUG   "${_3rd}/mysql-26.10.0/lib/win64/libmysql.lib" DLL_DEBUG   "${_3rd}/mysql-26.10.0/lib/win64/libmysql.dll")
        set(CQYH_MYSQL_RUNTIME_DLLS
            "${_3rd}/mysql-26.10.0/lib/win64/libssl-3-x64.dll"
            "${_3rd}/mysql-26.10.0/lib/win64/libcrypto-3-x64.dll")
    endif()

    # GPK: only a Debug redistributable DLL is shipped; used for both configurations.
    _cqyh_imported(gpk SHARED
        RELEASE "${_3rd}/gpk/Lib/GPKitSvr64.lib" DLL_RELEASE "${_3rd}/gpk/Redist/Debug/Server/64-bit/GPKitSvr64.dll"
        DEBUG   "${_3rd}/gpk/Lib/GPKitSvr64.lib" DLL_DEBUG   "${_3rd}/gpk/Redist/Debug/Server/64-bit/GPKitSvr64.dll")

    # Import libs whose DLLs are not in the repo: link only (the DLLs are expected next to the executable).
    _cqyh_imported(sdobase UNKNOWN
        RELEASE "${_3rd}/sdobase/lib/SdoBaseDLL.lib"
        DEBUG   "${_3rd}/sdobase/lib/SdoBaseDLL.lib")
    _cqyh_imported(fcm UNKNOWN
        RELEASE "${_3rd}/fcm/lib/fcmsdk_X64.lib"
        DEBUG   "${_3rd}/fcm/lib/fcmsdk_X64.lib")

    # Windows system libraries every server needs.
    add_library(cqyh_syslibs INTERFACE)
    target_link_libraries(cqyh_syslibs INTERFACE ws2_32 iphlpapi winmm dbghelp psapi)
    add_library(cqyh::syslibs ALIAS cqyh_syslibs)
else()
    add_library(cqyh_syslibs INTERFACE)
    target_link_libraries(cqyh_syslibs INTERFACE pthread dl rt)
    add_library(cqyh::syslibs ALIAS cqyh_syslibs)

    _cqyh_imported(mysql SHARED
        RELEASE "${_3rd}/mysql-26.10.0/lib/linux64/libmysqlclient.so.24"
        DEBUG   "${_3rd}/mysql-26.10.0/lib/linux64/libmysqlclient.so.24")
    _cqyh_imported(gpk STATIC
        RELEASE "${_3rd}/gpk/Redist/Debug/Server/64-Centos. 6.x/libGPKitSvr.a"
        DEBUG   "${_3rd}/gpk/Redist/Debug/Server/64-Centos. 6.x/libGPKitSvr.a")

    # sdobase / fcm are Windows-only SDKs (the Linux build of manageserver/loginserver never used them)
    add_library(cqyh::sdobase INTERFACE IMPORTED GLOBAL)
    add_library(cqyh::fcm INTERFACE IMPORTED GLOBAL)
endif()

if(CQYH_USE_LEGACY_MYSQL51 AND WIN32)
    set_target_properties(cqyh::mysql PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_3rd}/mysql-5.1.65/include")
else()
    set_target_properties(cqyh::mysql PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_3rd}/mysql-26.10.0/include")
endif()
