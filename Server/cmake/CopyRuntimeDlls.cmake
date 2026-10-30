# TARGET_RUNTIME_DLLS may be empty for standalone tools and static-only targets.
foreach(runtime_dll IN LISTS RUNTIME_DLLS)
    execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${runtime_dll}" "${DESTINATION}" COMMAND_ERROR_IS_FATAL ANY)
endforeach()
