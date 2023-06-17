
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


# Qt
# 设置moc rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#选择64位还是32位的Qt
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(Qt5HOMEDIR D:/Qt/Qt5.14.2/5.14.2/msvc2017_64)
else()
	set(Qt5HOMEDIR D:/Qt/Qt5.14.2/5.14.2/msvc2017)
endif()
#只引用Qt5Core写法一
# 提供Qt5位置定位.cmake文件
set(Qt5_DIR ${Qt5HOMEDIR}/lib/cmake/Qt5)
find_package(Qt5 COMPONENTS Core Widgets)

#只引用Qt5Core写法二
#提供Qt5Core库的.cmake文件位置
# set(Qt5Core_DIR ${Qt5HOMEDIR}/lib/cmake/Qt5Core)
# find_package(Qt5Core)


set(EXAMPLE_TARGET_OUT_DIR "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")




#
# Define CEF-based targets.
#
# Include the libcef_dll_wrapper target.
# Comes from the libcef_dll/CMakeLists.txt file in the binary distribution
# directory.
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
add_subdirectory(DemoApp)
add_subdirectory(QtCefBrowser)
add_subdirectory(QtCefRender)

# Display configuration settings.
PRINT_CEF_CONFIG()
