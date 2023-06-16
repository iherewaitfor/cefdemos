# Copyright (c) 2017 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

#
# Source files.
#

# Main executable sources.
set(MESSAGE_ROUTER_SRCS
  app_browser_impl.cc
  client_impl.cc
  client_impl.h
  app_renderer_impl.cc
  resource_util_win_impl.cc
  )
source_group("message_router" FILES ${MESSAGE_ROUTER_SRCS})

# Main executable resources.
set(EXAMPLE_RESOURCES_SRCS
  # Resources that embed "message_router.html" in the executable.
  resources/message_router.html
  resources/win/resource.h
  resources/win/resource.rc
  )
source_group("message_router/resources" FILES ${EXAMPLE_RESOURCES_SRCS})

set(MINIMAL_SRCS
    ../minimal/app_other_minimal.cc
	../minimal/main_minimal.cc
    )
source_group("minimal" FILES ${MINIMAL_SRCS})

# Shared Windows resources (version information, icons).
set(SHARED_RESOURCES_SRCS
../shared/resources/win/big.ico
../shared/resources/win/resource.h
../shared/resources/win/shared.rc
../shared/resources/win/small.ico
)
source_group("shared/resources/win" FILES ${SHARED_RESOURCES_SRCS})


# Target executable names.
set(EXAMPLE_TARGET "message_router")
#
# Windows configuration.
#

if(OS_WINDOWS)
    # Add resources to the sources variable for convenience.
  set(EXAMPLE_SRCS
    ${MESSAGE_ROUTER_SRCS}
    ${EXAMPLE_RESOURCES_SRCS}
    ${MINIMAL_SRCS}
    ${SHARED_RESOURCES_SRCS}
    )

  # Executable target.
  add_executable(${EXAMPLE_TARGET} WIN32 ${EXAMPLE_SRCS})
  SET_EXAMPLE_EXECUTABLE_TARGET_PROPERTIES(${EXAMPLE_TARGET})
  add_dependencies(${EXAMPLE_TARGET} shared)
  target_link_libraries(${EXAMPLE_TARGET} shared)
endif()
