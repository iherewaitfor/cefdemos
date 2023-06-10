# Copyright (c) 2016 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

# See the included README.md file for usage instructions.

# For VS2019 and Xcode 12+ support.
cmake_minimum_required(VERSION 3.19)

# Only generate Debug and Release configuration types.
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# Project name.
project(cef)

# Use folders in the resulting project files.
set_property(GLOBAL PROPERTY OS_FOLDERS ON)

if(DEFINED ENV{CEF_ROOT})
	# 如果环境变量有CEF_ROOT，就设置环境变量值。
	set(CEF_ROOT $ENV{CEF_ROOT})
endif()
if(CEF_ROOT AND IS_DIRECTORY "${CEF_ROOT}")
	message(NOTICE "CEF_ROOT is ${CEF_ROOT}")
else()
	#没有设置CEF_ROOT，就手动在这里设置cef的目录
	set(CEF_ROOT "D:/develop/cef_binary_97.1.9+ga00bca5+chromium-97.0.4692.99_windows32")
	message(NOTICE "CEF_ROOT is empty. set to ${CEF_ROOT}")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

#
# Load the CEF configuration.
#

# Execute FindCEF.cmake which must exist in CMAKE_MODULE_PATH.
find_package(CEF REQUIRED)
#
# Target configuration.
#

# Include the libcef_dll_wrapper target (executes libcef_dll/CMakeLists.txt).
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

# Include CEF's test application targets (executes <target>/CMakeLists.txt).
# add_subdirectory(${CEF_ROOT}/tests/cefclient)
# add_subdirectory(${CEF_ROOT}/tests/cefsimple)
# add_subdirectory(${CEF_ROOT}/tests/gtest)
# add_subdirectory(${CEF_ROOT}/tests/ceftests)

# Allow includes relative to the current source directory.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Configure building of the examples.
option(WITH_EXAMPLES "Enable or disable building of examples." ON)
if(WITH_EXAMPLES)
  # Include example application targets.
  add_subdirectory(examples)
endif()

# TODO: Include other application targets here.

# Display configuration settings.
PRINT_CEF_CONFIG()

if(NOT WITH_EXAMPLES)
  message(STATUS "")
  message(STATUS "NOTE: Targets from the examples folder are not included.")
  message(STATUS "Add -DWITH_EXAMPLES=On to the CMake command-line to include them.")
  message(STATUS "")
endif()
