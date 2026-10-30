cmake_minimum_required(VERSION 3.25)
execute_process(COMMAND "${CASH_TEST}" "${WORK_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/fixtures/cash-original-format.txt"
    RESULT_VARIABLE result OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr TIMEOUT 60)
if(NOT "${result}" STREQUAL "0" OR NOT stderr STREQUAL "")
    message(FATAL_ERROR "Cash accounting checks failed: ${result}\n${stdout}\n${stderr}")
endif()
message(STATUS "${stdout}")
# Exercise safe CLI paths only: no arguments intentionally retains the configured
# legacy database endpoint and must never be invoked by an automated test.
execute_process(COMMAND "${CASH_TOOL}" --help RESULT_VARIABLE result
    OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr TIMEOUT 10)
if(NOT "${result}" STREQUAL "0" OR NOT stdout MATCHES "Usage:" OR NOT stderr STREQUAL "")
    message(FATAL_ERROR "Cash accounting help failed: ${result}\n${stdout}\n${stderr}")
endif()
execute_process(COMMAND "${CASH_TOOL}" --invalid RESULT_VARIABLE result
    OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr TIMEOUT 10)
if(NOT "${result}" STREQUAL "1" OR NOT stderr MATCHES "Usage:")
    message(FATAL_ERROR "Cash accounting invalid argument did not fail: ${result}\n${stdout}\n${stderr}")
endif()
