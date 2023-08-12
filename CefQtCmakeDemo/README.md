- [CefQtCmakeDemo](#cefqtcmakedemo)
- [How to Run](#how-to-run)
  - [Cef配置](#cef配置)
  - [Qt配置](#qt配置)
  - [编译运行](#编译运行)
- [Qt配置详解](#qt配置详解)
  - [CMAKE\_AUTOMOC](#cmake_automoc)
  - [设置Qt的cmake目录](#设置qt的cmake目录)
- [Qt集成Cef消息循环](#qt集成cef消息循环)
  - [CefDoMessageLoopWork()方式集成消息循环](#cefdomessageloopwork方式集成消息循环)
  - [multi\_threaded\_message\_loop = true方式集成消息循环](#multi_threaded_message_loop--true方式集成消息循环)
- [参考](#参考)


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
在源码目标上新建build文件夹，然后在build目录下运行以下命令。
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

# Qt集成Cef消息循环
Cef和Qt都有各自己的消息消息。如何进行集成。

可以参考Cef官方文档关于消息循环集成的部分
[https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md#markdown-header-message-loop-integration](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md#markdown-header-message-loop-integration)，以了解消息循环集成概念。

集成示例代码可以参考[CefCmakeCefClient/cefclient/cefclient_win.cc](https://github.com/iherewaitfor/cefdemos/blob/main/CefCmakeCefClient/cefclient/cefclient_win.cc)

核心两种
1. CefDoMessageLoopWork()
2. CefSettings.multi_threaded_message_loop = true

   
本项目中使用的是第2种。CefSettings.multi_threaded_message_loop = true
## CefDoMessageLoopWork()方式集成消息循环
Qt中使用CefDoMessageLoopWork方式集成Cef消息循环。

Qt启动自己的消息消息。然后使用定时器，不断地调用 CefDoMessageLoopWork()，以处理Cef的消息。

优点：
该种方式，Cef的Browser的UI线程和主线程为同一线程。Qt和Cef的回调不需要使用线程同步技术。写业务上比较方便。

缺点: 要控制好调用CefDoMessageLoopWork()的调用频率，频率太高了cpu消息比较大，频率太低了，cef的消息处理可能不及时。

## multi_threaded_message_loop = true方式集成消息循环
CefSettings.multi_threaded_message_loop = true 集成的方式中，在主线启动Qt的消息循环。
Cef会单独自己启动另外一个线程执行其消息循环。此时不需要调用CefDoMessageLoopWork() 或 CefRunMessageLoop()。需要在主线程调用 CefInitialize初始始；退出时，调用 CefShutdown()。

项目中的Qt消息循环，使用QCoreApplication调用exec启用。

```
    QCoreApplication applicaiton(argc, argv)
    //...
     CefInitialize(main_args, settings, app.get(), sandbox_info);
    //...
    applicaiton.exec();
    //...
      // Shut down CEF.
     CefShutdown();
```
可以选择程序的退出时机(比如浏览器窗口都关闭了，或者是宿主应用自己定义退出时机)。
```C++
    //主应用退出
    QCoreApplication::exit();
```
主应用退出前，需要先调用 CefShutdown()关闭Cef让其干净退出。


# 参考
[https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeDemo#how-to-run](https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeDemo#how-to-run)

