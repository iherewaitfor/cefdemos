# Copyright (c) 2017 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

#
# Source files.
#

# Main executable sources.
set(EXAMPLE_SRCS
  app_browser_minimal.cc
  client_minimal.cc
  client_minimal.h
  main_minimal.cc
  app_other_minimal.cc
  app_renderer_minimal.cc
  )
  
# Shared Windows resources (version information, icons).
set(SHARED_RESOURCES_SRCS
../shared/resources/win/big.ico
../shared/resources/win/resource.h
../shared/resources/win/shared.rc
../shared/resources/win/small.ico
)
source_group("shared/resources/win" FILES ${SHARED_RESOURCES_SRCS})

#
# Shared configuration.
#
# Target executable names.
set(EXAMPLE_TARGET "minimal")

#
# Windows configuration.
#

if(OS_WINDOWS)
    # Add resources to the sources variable for convenience.
  set(EXAMPLE_SRCS
    ${EXAMPLE_SRCS}
    ${SHARED_RESOURCES_SRCS}
    )
  # Executable target.
  add_executable(${EXAMPLE_TARGET} WIN32 ${EXAMPLE_SRCS})
  SET_EXAMPLE_EXECUTABLE_TARGET_PROPERTIES(${EXAMPLE_TARGET})
  add_dependencies(${EXAMPLE_TARGET} shared)
  target_link_libraries(${EXAMPLE_TARGET} shared)
endif()
