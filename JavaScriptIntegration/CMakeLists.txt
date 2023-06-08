
# For VS2019 and Xcode 12+ support.
cmake_minimum_required(VERSION 3.19)

# Only generate Debug and Release configuration types.
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# Project name.
# TODO: Change this line to match your project name when you copy this file.
project(cef)

# Use folders in the resulting project files.
#set_property(GLOBAL PROPERTY OS_FOLDERS ON)


#
# CEF_ROOT setup.
# This variable must be set to locate the binary distribution.
# set(CEF_ROOT "D:/develop/cef_binary_97.1.9+ga00bca5+chromium-97.0.4692.99_windows32")

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
# Define CEF-based targets.
#

# Include the libcef_dll_wrapper target.
# Comes from the libcef_dll/CMakeLists.txt file in the binary distribution
# directory.
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
add_subdirectory(cefsimple)

# Display configuration settings.
PRINT_CEF_CONFIG()
