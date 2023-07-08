project(qcefbrowser)
# Target names.
set(CEF_TARGET "qcefbrowser")
ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

set(CMAKE_AUTOMOC ON)

#项目使用UNICODE版本。win中的各种API将使用UNICODE版本。
add_definitions("-DUNICODE" "-D_UNICODE")

add_definitions("-DQCEFBROWSER_IMPL")


aux_source_directory(. EXAMPLE_SRCS)
#list(APPEND DIR_SRCS ./cefsimple.rc)
aux_source_directory(./render SRCS_RENDER)
aux_source_directory(./browser SRCS_BROWSER)
include_directories(./render)
file (GLOB HEADERS
		"*.h")
file (GLOB HEADERS_RENDER 
		"render/*.h")
file (GLOB HEADERS_BROWSER 
		"browser/*.h")
		
message(NOTICE "HEADERS_RENDER: ${HEADERS_RENDER}" )
set(RENDERLIST
 ${HEADERS_RENDER}
 ${SRCS_RENDER}
 )
source_group("render" FILES ${RENDERLIST}) 
set(BROWSERLIST
 ${HEADERS_BROWSER}
 ${SRCS_BROWSER}
 )
source_group("browser" FILES ${BROWSERLIST}) 
		
# 需要执行moc的头文件列表
set(MOC_HEADERS ${HEADERS} ${HEADERS_RENDER} ${HEADERS_BROWSER})
# 可以排除某些头文件不执行moc
# list(REMOVE_ITEM MOC_HEADERS ${PROJECT_SOURCE_DIR}/autosignalsemitter.h)

# Executable target.
add_library(${CEF_TARGET} SHARED ${EXAMPLE_SRCS} ${HEADERS} ${SRCS_RENDER} ${HEADERS_RENDER} ${BROWSERLIST})
SET_EXAMPLE_LIBRARY_TARGET_PROPERTIES(${CEF_TARGET})
add_dependencies(${CEF_TARGET} libcef_dll_wrapper)

target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})
# 链接Qt库
target_link_libraries(${CEF_TARGET} Qt5::Core)

if(USE_SANDBOX)
  # Logical target used to link the cef_sandbox library.
  ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
  target_link_libraries(${CEF_TARGET} cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
endif()

# Copy binary and resource files to the target output directory.
COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${EXAMPLE_TARGET_OUT_DIR}")
COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${EXAMPLE_TARGET_OUT_DIR}")
