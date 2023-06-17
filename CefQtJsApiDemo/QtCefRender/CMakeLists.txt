project(qcefrender)
# Target executable names.
set(EXAMPLE_TARGET "qcefrender")

aux_source_directory(. EXAMPLE_SRCS)
#list(APPEND DIR_SRCS ./cefsimple.rc)

# Executable target.
add_executable(${EXAMPLE_TARGET} WIN32 ${EXAMPLE_SRCS})
SET_EXAMPLE_EXECUTABLE_TARGET_PROPERTIES(${EXAMPLE_TARGET})
add_dependencies(${EXAMPLE_TARGET} libcef_dll_wrapper)
target_link_libraries(${EXAMPLE_TARGET} shared)


SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})



  add_library(${SHARED_TARGET} STATIC ${SHARED_SRCS})
SET_EXAMPLE_LIBRARY_TARGET_PROPERTIES(${SHARED_TARGET})
add_dependencies(${SHARED_TARGET} libcef_dll_wrapper)
target_link_libraries(${SHARED_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

if(USE_SANDBOX)
  # Logical target used to link the cef_sandbox library.
  ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
  target_link_libraries(${SHARED_TARGET} cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
endif()

# Copy binary and resource files to the target output directory.
COPY_FILES("${SHARED_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${EXAMPLE_TARGET_OUT_DIR}")
COPY_FILES("${SHARED_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${EXAMPLE_TARGET_OUT_DIR}")
