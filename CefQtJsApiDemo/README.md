- [CefQtJspApiDemo Qt实现JS接口Demo](#cefqtjspapidemo-qt实现js接口demo)
- [How to Run](#how-to-run)
- [程序结构](#程序结构)
- [QCefV8Bind](#qcefv8bind)
- [QCefV8BindRO](#qcefv8bindro)

# CefQtJspApiDemo Qt实现JS接口Demo
本项目将Cef和Qt集成。使用Qt实现JS接口。

利用Qt的metaObject系统，让接口以QObject方式提供。js的接口调用，都是正常的函数调用。支持Qt的信号系统。js可以监听信号。

接口实现，只需要写正常的QObject，绑定模块会把这些对象映射成js接口。

其中有两种实现方式
- QCefV8Bind:使用cef自带的ipc
  - 接口实现放在browser进程
  - 使用cef自带的ipc，在browser和render进程通信。
- QCefV8BindRO: 使用QtRemoteObject
  - 接口实现不限于browser进程。
  - render通过QRemoteObject可以直接与实现进程通信。
# How to Run
可以参考[CefQtCmakeDemo/README.md#how-to-run](https://github.com/iherewaitfor/cefdemos/blob/main/CefQtCmakeDemo/README.md#how-to-run)。

其中DemoAPP为启动程序。可以设置其为启动项。

# 程序结构
- DemoApp
  - 程序入口
- QCefV8Bind
  - 使用cef自带ipc进行通信的js绑定模块
- QCefV8BindRO
  - 使用QReMoteObject进行通信的js绑定模块
- QtCefBrowser
  - Cef的核心应用模块，搭建了程序的基础框架。
- QtCefRender
  - cef子进程的启动程序。
# QCefV8Bind
# QCefV8BindRO