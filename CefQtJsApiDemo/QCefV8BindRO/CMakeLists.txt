project(QCefV8BindRO)
# Target names.
set(CEF_TARGET ${PROJECT_NAME})
ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

set(CMAKE_AUTOMOC ON)

#项目使用UNICODE版本。win中的各种API将使用UNICODE版本。
add_definitions("-DUNICODE" "-D_UNICODE")

add_definitions("-DQCEFV8BINDRO_IMPL")

include_directories(../include)
aux_source_directory(. EXAMPLE_SRCS)
#list(APPEND DIR_SRCS ./cefsimple.rc)
aux_source_directory(./render SRCS_RENDER)
file (GLOB HEADERS
		"*.h")
file (GLOB HEADERS_RENDER 
		"render/*.h")
# 需要执行moc的头文件列表
set(MOC_HEADERS ${HEADERS} ${HEADERS_RENDER})
# 可以排除某些头文件不执行moc
# list(REMOVE_ITEM MOC_HEADERS ${PROJECT_SOURCE_DIR}/autosignalsemitter.h)

set(RENDERLIST
 ${HEADERS_RENDER}
 ${SRCS_RENDER}
 )
source_group("render" FILES ${RENDERLIST}) 

# Executable target.
add_library(${CEF_TARGET} SHARED ${EXAMPLE_SRCS} ${HEADERS} ${RENDERLIST})
SET_EXAMPLE_LIBRARY_TARGET_PROPERTIES(${CEF_TARGET})
add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
add_dependencies(${CEF_TARGET} qcefbrowser)


target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})
# 链接Qt库
target_link_libraries(${CEF_TARGET} Qt5::Core Qt5::RemoteObjects)
target_link_libraries(${CEF_TARGET} qcefbrowser)

if(USE_SANDBOX)
  # Logical target used to link the cef_sandbox library.
  ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
  target_link_libraries(${CEF_TARGET} cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
endif()

# Copy binary and resource files to the target output directory.
COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${EXAMPLE_TARGET_OUT_DIR}")
COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${EXAMPLE_TARGET_OUT_DIR}")
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${Qt5HOMEDIR}/bin/windeployqt.exe  ${EXAMPLE_TARGET_OUT_DIR}/${PROJECT_NAME}.dll
  )
