# Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

#
# Source files.
#
add_definitions("-DUNICODE" "-D_UNICODE")
# cefsimple sources.
set(CEFSIMPLE_SRCS
  #simple_app.cc
  #simple_app.h
  #simple_handler.cc
  #simple_handler.h
  #simple_handler_win.cc
  cefsimple_win.cc
  )
# set(CEFSIMPLE_SRCS_WINDOWS
  # cefsimple.rc
  # resource.h
  # simple_handler_win.cc
  # cefsimple_win.cc
  # )
# APPEND_PLATFORM_SOURCES(CEFSIMPLE_SRCS)
# source_group(cefsimple FILES ${CEFSIMPLE_SRCS})

set(CEFSIMPLE_SRCS
  ${CEFSIMPLE_SRCS}
  )
  
# APPEND_PLATFORM_SOURCES(CEFSIMPLE_HELPER_SRCS)
# source_group(cefsimple FILES ${CEFSIMPLE_HELPER_SRCS})

# cefsimple resources.


#
# Shared configuration.
#

# Target executable names.
set(CEF_TARGET "cefrender")

ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

# Determine the target output directory.
# you can find the macro in ${CEF_ROOT}/cmake/cef_macros.cmake
SET_CEF_TARGET_OUT_DIR()
#
# Windows configuration.
#

if(OS_WINDOWS)
  # Executable target.
  add_executable(${CEF_TARGET} WIN32 ${CEFSIMPLE_SRCS})
  SET_EXAMPLE_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
  add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
  
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
  target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

  if(USE_SANDBOX)
    # Logical target used to link the cef_sandbox library.
    ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
    target_link_libraries(${CEF_TARGET} cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
  endif()

  # Add the custom manifest files to the executable.
  ADD_WINDOWS_MANIFEST2("${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_TARGET}" "exe")

  # Copy binary and resource files to the target output directory.
  COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${EXAMPLE_TARGET_OUT_DIR}")
  COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${EXAMPLE_TARGET_OUT_DIR}")
endif()
