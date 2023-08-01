project(qcefrender)
# Target executable names.
set(CEF_TARGET "qcefrender")
ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

# 设置子系统为窗口程序
#set ( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS" ) 

set(CMAKE_AUTOMOC ON)
#项目使用UNICODE版本。win中的各种API将使用UNICODE版本。
add_definitions("-DUNICODE" "-D_UNICODE")

aux_source_directory(. EXAMPLE_SRCS)
#list(APPEND DIR_SRCS ./cefsimple.rc)


# 需要执行moc的头文件列表
set(MOC_HEADERS ${HEADERS})
# 可以排除某些头文件不执行moc
# list(REMOVE_ITEM MOC_HEADERS ${PROJECT_SOURCE_DIR}/autosignalsemitter.h)


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

# Executable target.
add_executable(${CEF_TARGET} WIN32 ${EXAMPLE_SRCS} ${HEADERS})
SET_EXAMPLE_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
add_dependencies(${CEF_TARGET} libcef_dll_wrapper)

target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})
# 链接Qt库
target_link_libraries(${CEF_TARGET} Qt5::Core)
target_link_libraries(${CEF_TARGET} Qt5::Widgets)
target_link_libraries(${CEF_TARGET} qcefbrowser)
#target_link_libraries(${CEF_TARGET} qcefv8bind)
target_link_libraries(${CEF_TARGET} QCefV8BindRO)


if(USE_SANDBOX)
  # Logical target used to link the cef_sandbox library.
  ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
  target_link_libraries(${CEF_TARGET} cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
endif()
  # Add the custom manifest files to the executable.
  ADD_WINDOWS_MANIFEST2("${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_TARGET}" "exe")

# Copy binary and resource files to the target output directory.
COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${EXAMPLE_TARGET_OUT_DIR}")
COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${EXAMPLE_TARGET_OUT_DIR}")
