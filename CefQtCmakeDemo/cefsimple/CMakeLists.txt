# Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

#
# Source files.
#

aux_source_directory(. DIR_SRCS)
list(APPEND DIR_SRCS ./cefsimple.rc)
# message(NOTICE "DIR_SRCS is ${DIR_SRCS}")

#
# Shared configuration.
#
project(cefsimple)
# Target executable names.
set(CEF_TARGET "cefsimple")

ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

# Determine the target output directory.
# you can find the macro in ${CEF_ROOT}/cmake/cef_macros.cmake
SET_CEF_TARGET_OUT_DIR()
#
# Windows configuration.
#

if(OS_WINDOWS)
  # Executable target.
  add_executable(${CEF_TARGET} WIN32 ${DIR_SRCS})
  add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
  
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
  target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})
  # 链接Qt库
  target_link_libraries(${CEF_TARGET} Qt5::Core)

  if(USE_SANDBOX)
    # Logical target used to link the cef_sandbox library.
    ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
    target_link_libraries(${CEF_TARGET} cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
  endif()

  # Add the custom manifest files to the executable.
  ADD_WINDOWS_MANIFEST("${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_TARGET}" "exe")

  # Copy binary and resource files to the target output directory.
  COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
  COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")

  # 复制Qt5依赖库到对应exe文件目录
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${Qt5HOMEDIR}/bin/windeployqt.exe  ${PROJECT_BINARY_DIR}/$<CONFIGURATION>/${PROJECT_NAME}.exe
  )
endif()
