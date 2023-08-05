- [CefQtCmakeDemo](#cefqtcmakedemo)
- [How to Run](#how-to-run)
  - [Cef配置](#cef配置)
  - [Qt配置](#qt配置)
  - [编译运行](#编译运行)
- [Qt配置详解](#qt配置详解)
  - [CMAKE\_AUTOMOC](#cmake_automoc)
  - [设置Qt的cmake目录](#设置qt的cmake目录)


# CefQtCmakeDemo

# How to Run

## Cef配置
Cef部分的配置，请参考
[CefCmakeDemo#how-to-run](https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeDemo#how-to-run).

## Qt配置

本项目使用的cef为32位。

修改QtHOMEDIR为你自己的Qt目录。如项目使用的位置为D:/Qt/Qt5.14.2/5.14.2/msvc2017。
```
#选择64位还是32位的Qt
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(Qt5HOMEDIR D:/Qt/Qt5.14.2/5.14.2/msvc2017_64)
else()
	set(Qt5HOMEDIR D:/Qt/Qt5.14.2/5.14.2/msvc2017)
endif()
```
## 编译运行
```
cmake .. -G "Visual Studio 16 2019" -A Win32
```
详细请参考
[CefCmakeDemo#how-to-run](https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeDemo#how-to-run).

# Qt配置详解

Qt部分的核心配置为
```
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
# set(Qt5_DIR ${Qt5HOMEDIR}/lib/cmake/Qt5)
# find_package(Qt5 COMPONENTS Core)

#只引用Qt5Core写法二
#提供Qt5Core库的.cmake文件位置
set(Qt5Core_DIR ${Qt5HOMEDIR}/lib/cmake/Qt5Core)
find_package(Qt5Core)
```

## CMAKE_AUTOMOC
```
set(CMAKE_AUTOMOC ON)
```

打开该开关关，Qt会自动扫描哪些文件需要执行moc。
## 设置Qt的cmake目录
如上配置所示，Qt的模块引用支持两种写法。其中第一种写法方便后续增加其他模块，增加一个组件，只需要再添加一个组件名即可。
```
set(Qt5_DIR ${Qt5HOMEDIR}/lib/cmake/Qt5)
find_package(Qt5 COMPONENTS Core)
```
${Qt5HOMEDIR}/lib/cmake/Qt5 文件夹为Qt5的cmake目录。

find_package(Qt5 COMPONENTS Core) 是引用 Core模块。如果需要添加Widgets模块，只需要改为
```
find_package(Qt5 COMPONENTS Core Widgets)
```
