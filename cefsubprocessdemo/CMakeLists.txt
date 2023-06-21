
# For VS2019 and Xcode 12+ support.
cmake_minimum_required(VERSION 3.19)

# Only generate Debug and Release configuration types.
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# Project name.
# TODO: Change this line to match your project name when you copy this file.
project(cef)

# Use folders in the resulting project files.
#set_property(GLOBAL PROPERTY OS_FOLDERS ON)

add_definitions("-DUNICODE" "-D_UNICODE")
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


set(EXAMPLE_TARGET_OUT_DIR "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")


# Set properties common to all example targets.
macro(SET_EXAMPLE_PROPERTIES target)
  # Output all binaries to the configuration-specific build directory.
  set_target_properties(${target} PROPERTIES
                        ARCHIVE_OUTPUT_DIRECTORY "${EXAMPLE_TARGET_OUT_DIR}"
                        RUNTIME_OUTPUT_DIRECTORY "${EXAMPLE_TARGET_OUT_DIR}"
                        LIBRARY_OUTPUT_DIRECTORY "${EXAMPLE_TARGET_OUT_DIR}")
endmacro()

# Set properties on an example library target.
macro(SET_EXAMPLE_LIBRARY_TARGET_PROPERTIES target)
  SET_LIBRARY_TARGET_PROPERTIES(${target})
  SET_EXAMPLE_PROPERTIES(${target})
endmacro()

# Set properties on an example executable target.
macro(SET_EXAMPLE_EXECUTABLE_TARGET_PROPERTIES target)
  # 该cmake宏位于cef_macros.cmake
  SET_EXECUTABLE_TARGET_PROPERTIES(${target})
  SET_EXAMPLE_PROPERTIES(${target})
  # Add the custom Windows manifest files to the executable.
    
endmacro()

# Add custom manifest files to an executable target.
macro(ADD_WINDOWS_MANIFEST2 manifest_path target extension)
  add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND "mt.exe" -nologo
            -manifest \"${manifest_path}/${target}.${extension}.manifest\" \"${manifest_path}/compatibility.manifest\"
            -outputresource:"${EXAMPLE_TARGET_OUT_DIR}/${target}.${extension}"\;\#1
    COMMENT "Adding manifest..."
    )
endmacro()

# Include the libcef_dll_wrapper target.
# Comes from the libcef_dll/CMakeLists.txt file in the binary distribution
# directory.
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
add_subdirectory(cefsimple)
add_subdirectory(cefrender)

# Display configuration settings.
PRINT_CEF_CONFIG()
