# Compile the production server utilities directly for regression coverage.
foreach(variant IN ITEMS server)
    set(source_dir "${CQYH_SERVER_DIR}/NewServer/src/baselib")

    set(test_target test_baselib_utilities_${variant})
    add_executable(${test_target}
        test_baselib_utilities.cpp
        "${source_dir}/dir.cpp"
        "${source_dir}/string_ex.cpp")
    cqyh_target_defaults(${test_target})
    target_include_directories(${test_target} PRIVATE
        "${source_dir}"
        "${CQYH_SERVER_DIR}/NewServer/inc"
        "${CQYH_SERVER_DIR}/NewServer/inc/baselib")
    target_compile_definitions(${test_target} PRIVATE NOMINMAX)
    target_link_libraries(${test_target} PRIVATE baselib fusion cqyh::syslibs)
    cqyh_copy_runtime_dlls(${test_target})
    if(MSVC)
        target_compile_options(${test_target} PRIVATE /permissive-)
    endif()

    add_test(NAME baselib_utilities_${variant} COMMAND ${test_target})
    set_tests_properties(baselib_utilities_${variant} PROPERTIES TIMEOUT 30)

    set(resource_target test_baselib_resources_${variant})
    add_executable(${resource_target}
        test_baselib_resources.cpp
        "${source_dir}/ifile.cpp"
        "${source_dir}/tabfile.cpp"
        "${source_dir}/lua_script.cpp")
    cqyh_target_defaults(${resource_target})
    target_include_directories(${resource_target} PRIVATE
        "${source_dir}"
        "${CQYH_SERVER_DIR}/NewServer/inc"
        "${CQYH_SERVER_DIR}/NewServer/inc/baselib")
    target_compile_definitions(${resource_target} PRIVATE NOMINMAX)
    target_link_libraries(${resource_target} PRIVATE
        baselib fusion cqyh::sol2 cqyh::zlib cqyh::syslibs)
    if(MSVC)
        target_compile_options(${resource_target} PRIVATE /permissive-)
    endif()
    cqyh_copy_runtime_dlls(${resource_target})
    add_test(NAME baselib_resources_${variant} COMMAND ${resource_target})
    set_tests_properties(baselib_resources_${variant} PROPERTIES TIMEOUT 60)

    set(timing_target test_baselib_timing_${variant})
    add_executable(${timing_target} test_baselib_timing.cpp
        "${source_dir}/simple_timer.cpp" "${source_dir}/time_ex.cpp")
    cqyh_target_defaults(${timing_target})
    target_include_directories(${timing_target} PRIVATE
        "${source_dir}"
        "${CQYH_SERVER_DIR}/NewServer/inc"
        "${CQYH_SERVER_DIR}/NewServer/inc/baselib"
        "${CQYH_SERVER_DIR}/Common/define")
    target_compile_definitions(${timing_target} PRIVATE NOMINMAX)
    add_test(NAME baselib_timing_${variant} COMMAND ${timing_target})
    set_tests_properties(baselib_timing_${variant} PROPERTIES TIMEOUT 30)

endforeach()

add_executable(test_baselib_numbers test_baselib_numbers.cpp)
cqyh_target_defaults(test_baselib_numbers)
target_include_directories(test_baselib_numbers PRIVATE "${CQYH_SERVER_DIR}/NewServer/inc/baselib")
add_test(NAME baselib_numbers COMMAND test_baselib_numbers)
set_tests_properties(baselib_numbers PROPERTIES TIMEOUT 15)

if(WIN32)
    add_executable(test_utf8_owners test_utf8_owners.cpp ../../src/baselib/utf8.cpp)
    cqyh_target_defaults(test_utf8_owners)
    target_include_directories(test_utf8_owners PRIVATE
        "${CQYH_SERVER_DIR}/NewServer/src/baselib" "${CQYH_SERVER_DIR}/NewServer/inc"
        "${CQYH_SERVER_DIR}/NewServer/inc/baselib" "${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_definitions(test_utf8_owners PRIVATE NOMINMAX)
    if(MSVC)
        target_compile_options(test_utf8_owners PRIVATE "/FI${CMAKE_CURRENT_SOURCE_DIR}/fake_codepages.h")
    else()
        target_compile_options(test_utf8_owners PRIVATE -include "${CMAKE_CURRENT_SOURCE_DIR}/fake_codepages.h")
    endif()
    add_test(NAME utf8_owners COMMAND test_utf8_owners)
    set_tests_properties(utf8_owners PROPERTIES TIMEOUT 30)
endif()
