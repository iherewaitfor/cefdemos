# cefsubprocessdemo
本项目在Cefsimple项目基础上扩展，主要展示如何使用独立的exe运行  renderer进程及其他进程。

# How to Run
详细请参考
[CefCmakeDemo#how-to-run](https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeDemo#how-to-run).
# 添加cefrender.exe模块的设置

cmake配置。

主要是
在解决方案的cmake中添加

```

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
```
在cefrender.exe模块中添加设置

```
 # Add the custom manifest files to the executable.
  ADD_WINDOWS_MANIFEST2("${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_TARGET}" "exe")
```

以上配置好，libcef在启动进程时，需要读取对应的manifest里的内容，进行系统兼容的判断。

# 代码设置
主要是设置CefSettings中browser_subprocess_path，提供对应子进程的exe文件。示例中的cefrender.exe文件放在了主程序文件的同一级目录下。
```C++
    CefSettings settings;
    // 获取子进程路径
    WCHAR subProcessPath[MAX_PATH] = { 0 };
    GetModuleFileNameW(hInstance, subProcessPath, MAX_PATH);
    *(wcsrchr(subProcessPath, L'\\') + 1) = L'\0';
    LPCWSTR SUB_PROCESS_NAME = L"cefrender.exe";
    wcsncat_s(subProcessPath, MAX_PATH, SUB_PROCESS_NAME, wcslen(SUB_PROCESS_NAME));
    // 设置子进程路径
    cef_string_from_wide(subProcessPath, MAX_PATH, &settings.browser_subprocess_path);
```
