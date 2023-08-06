- [CefQtJspApiDemo Qt实现JS接口Demo](#cefqtjspapidemo-qt实现js接口demo)
- [How to Run](#how-to-run)
- [程序结构](#程序结构)
- [QtCefBrowser](#qtcefbrowser)
  - [cef应用核心框架](#cef应用核心框架)
    - [QCefClient--cef初始和关闭](#qcefclient--cef初始和关闭)
  - [消息循环集成](#消息循环集成)
  - [本方案中涉及的进程和线程](#本方案中涉及的进程和线程)
- [QCefV8Bind](#qcefv8bind)
- [QCefV8BindRO](#qcefv8bindro)

# CefQtJspApiDemo Qt实现JS接口Demo
本项目将Cef和Qt集成。使用Qt实现JS接口。

利用Qt的metaObject系统，让接口以QObject方式提供。js的接口调用，都是正常的函数调用。支持Qt的信号系统。js可以监听信号。

接口实现，只需要写正常的QObject，绑定模块会把这些对象映射成js接口。

项目展示了两种实现方式
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
# QtCefBrowser
该模块主要负责描述cef应用的核心流程：
- 初始化、关闭
- 关闭回调接口的注册和实现。
- 浏览器打开、关闭
## cef应用核心框架
### QCefClient--cef初始和关闭
Cef主流程的初始化和关闭。
核心调用为
- browser进程的 CefInitialize、CefShutdown
- render及其他进程的CefExecuteProcess

其中
```C++
    CefSettings cefSettings;
    cefSettings.multi_threaded_message_loop = true;
```
## 消息循环集成


## 本方案中涉及的进程和线程
由于本项目中，集成线程时，进行了以下两个设置
1.  browser进程中cef初始化时，设置了cefSettings.multi_threaded_message_loop = true;
2.  render进程在单独线程中运行CefExecuteProcess。
   
所以进程的主线程和cef的消息循环线程不是同一线程。涉及的线程如下：

- Browser进程
  - 主线程、Qt线程。Qt消息循环所有线程
  - CEF的TID_UI线程。CEF消息循环线程。
- Render进程
  - 主线线程：Qt线程。Qt消息循环所有线程。
  - CEF的TID_RENDERER线程。render消息循环线程。
# QCefV8Bind
# QCefV8BindRO