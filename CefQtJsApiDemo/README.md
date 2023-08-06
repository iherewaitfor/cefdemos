- [CefQtJspApiDemo Qt实现JS接口Demo](#cefqtjspapidemo-qt实现js接口demo)
- [How to Run](#how-to-run)
- [程序结构](#程序结构)
- [QtCefBrowser](#qtcefbrowser)
  - [cef应用核心框架](#cef应用核心框架)
    - [QCefClient--cef初始和关闭](#qcefclient--cef初始和关闭)
    - [Browser进程的CefApp之QCefBrowserApp](#browser进程的cefapp之qcefbrowserapp)
    - [Render进程的CefApp之QCefRenderApp](#render进程的cefapp之qcefrenderapp)
  - [Browser消息循环集成](#browser消息循环集成)
    - [CefDoMessageLoopWork()方式集成消息循环](#cefdomessageloopwork方式集成消息循环)
    - [multi\_threaded\_message\_loop = true方式集成消息循环（本项目采用）](#multi_threaded_message_loop--true方式集成消息循环本项目采用)
  - [本方案中涉及的进程和线程](#本方案中涉及的进程和线程)
- [QCefV8Bind](#qcefv8bind)
- [QCefV8BindRO](#qcefv8bindro)
- [参考](#参考)

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
初始Browser进程，CefInitialize需要Browser的CefAPP。
初始render进程，CefInitialize需要Render的CefAPP。

- Browser进程的Cefapp
  - GetBrowserProcessHandler()
    - OnContextInitialized()
    - OnBeforeChildProcessLaunch()
  - OnBeforeCommandLineProcessing()
- Render进程的Cefapp
  - GetRenderProcessHandler()
    - OnWebKitInitialized()
    - OnContextCreated()
    - OnContextReleased()
    - OnProcessMessageReceived()
### Browser进程的CefApp之QCefBrowserApp

### Render进程的CefApp之QCefRenderApp

关于Cef最核心的启动和关闭流程，可以参考[https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeDemo](https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeDemo)
## Browser消息循环集成
消息循环主要有两种集成方式：
Cef和Qt都有各自己的消息消息。如何进行集成。

可以参考Cef官方文档的消息循环集成部分
[https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md#markdown-header-message-loop-integration](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md#markdown-header-message-loop-integration)，以了解消息循环集成概念。

cef官方集成示例代码可以参考[CefCmakeCefClient/cefclient/cefclient_win.cc](https://github.com/iherewaitfor/cefdemos/blob/main/CefCmakeCefClient/cefclient/cefclient_win.cc)

本仓库有个简单的Qt集成示例项目也可以参考[CefQtCmakeDemo](https://github.com/iherewaitfor/cefdemos/tree/main/CefQtCmakeDemo)

主要有两种集成方式
1. CefDoMessageLoopWork()，在应用消息循环中调用 。
2. CefSettings.multi_threaded_message_loop = true。cef单独开线程跑消息循环。

另外还有和一种 就是直接以cef的消息循环作为主消息循环，因为一般主应用程序已有自己的消息循环，在嵌入cef时不太适合用cef的消息循环作为主消息循环。在cef的示例程序中可以使用这种方式。

   
本项目中使用的是第2种。CefSettings.multi_threaded_message_loop = true
### CefDoMessageLoopWork()方式集成消息循环
若使用CefDoMessageLoopWork方式集成Cef消息循环。可以这样操作
- 在程序主线程启动Qt消息循环。
- 然后使用定时器，不断地Qt主消息循环中调用 CefDoMessageLoopWork()，以处理Cef的消息。

优点：
该种方式，Cef的Browser的UI线程和主线程为同一线程。Qt和Cef的回调不需要使用线程同步技术。写业务上比较方便。

缺点: 要控制好调用CefDoMessageLoopWork()的调用频率，频率太高了cpu消息比较大，频率太低了，cef的消息处理可能不及时。

### multi_threaded_message_loop = true方式集成消息循环（本项目采用）
CefSettings.multi_threaded_message_loop = true 集成的方式中，在主线启动Qt的消息循环。
Cef会单独自己启动另外一个线程执行其消息循环。此时不需要调用CefDoMessageLoopWork() 或 CefRunMessageLoop()。需要在主线程调用 CefInitialize初始始；退出时，在主线程调用 CefShutdown()。

关键代码[qcefclient.cpp](https://github.com/iherewaitfor/cefdemos/blob/main/CefQtJsApiDemo/QtCefBrowser/qcefclient.cpp)
```C++
    CefSettings cefSettings;
    cefSettings.multi_threaded_message_loop = true;
```

此种方式，需要注意线程间同步。一个是主线程，一个是TID_UI线程。主线程跑Qt消息循环，TID_UI跑cef消息循环。
注意两个线程间同步。

CEF的回调都跑在TID_UI线程。Cef的UI相关操作都需要切到TID_UI线程进行。

可以使用CefPostTask把逻辑切到TID_UI线程运行。
```c++
void SimpleHandler::initWindow()
{
    if (!CefCurrentlyOn(TID_UI)) {
        CefPostTask(TID_UI, base::BindOnce(&SimpleHandler::initWindow, this));
        return;
    }
    if (m_browerPrivate) {
        m_browerPrivate->createHostWindow();
        m_browerPrivate->createBrowser();
    }
}
```

关于CefPostTask，可以参考Cef官方文档[Posting Tasks](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md#markdown-header-posting-tasks)

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

关于进程和线程的概念，可以参考以下cef官方文档

[https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md#markdown-header-processes](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md#markdown-header-processes)

[https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md#markdown-header-threads](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md#markdown-header-threads)
# QCefV8Bind
# QCefV8BindRO

# 参考

[https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md)