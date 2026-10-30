# Exercise the actual generator through CMake; no scripting runtime is required.
add_test(NAME generator_outputs
    COMMAND "${CMAKE_COMMAND}"
        "-DGENERATOR=$<TARGET_FILE:gen_msg>"
        "-DSERVER=${CQYH_SERVER_DIR}"
        "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/generator-output"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_generator.cmake")
set_tests_properties(generator_outputs PROPERTIES TIMEOUT 120)
