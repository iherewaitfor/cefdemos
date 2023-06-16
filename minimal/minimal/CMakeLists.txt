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

#
# Shared configuration.
#

# Target executable names.
set(EXAMPLE_TARGET "minimal")
if(OS_MAC)
  set(EXAMPLE_HELPER_TARGET "minimal_Helper")
  set(EXAMPLE_HELPER_OUTPUT_NAME "minimal Helper")
endif()


#
# Windows configuration.
#

if(OS_WINDOWS)
    # Add resources to the sources variable for convenience.
  set(EXAMPLE_SRCS
    ${EXAMPLE_SRCS}
    ${EXAMPLE_RESOURCES_SRCS}
    )

  # Executable target.
  add_executable(${EXAMPLE_TARGET} WIN32 ${EXAMPLE_SRCS})
  SET_EXAMPLE_EXECUTABLE_TARGET_PROPERTIES(${EXAMPLE_TARGET})
  add_dependencies(${EXAMPLE_TARGET} shared)
  target_link_libraries(${EXAMPLE_TARGET} shared)
endif()
