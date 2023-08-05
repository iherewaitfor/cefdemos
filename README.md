- [cefdemos](#cefdemos)
- [CefCmakeDemo 使用CMake配置CefSimple](#cefcmakedemo-使用cmake配置cefsimple)
- [CefCmakeCefClient 使用CMake配置CefClient项目](#cefcmakecefclient-使用cmake配置cefclient项目)
- [CefExamples官方功能示例的本地配置](#cefexamples官方功能示例的本地配置)
- [JavaScriptIntegration--JavaScript集成](#javascriptintegration--javascript集成)
- [schemedemo--自定义scheme，本地资源加载 展示](#schemedemo--自定义scheme本地资源加载-展示)
- [cefsubprocessdemo--渲染进程使用不同的exe](#cefsubprocessdemo--渲染进程使用不同的exe)
- [CefQtCmakeDemo--Cef+Qt集成配置](#cefqtcmakedemo--cefqt集成配置)
- [CefQtJsApiDemo--使用Qt集成JavaScript接口](#cefqtjsapidemo--使用qt集成javascript接口)

# cefdemos
本仓库[https://github.com/iherewaitfor/cefdemos]包含不同的cef展示项目。从cmake配置到各个cef基础功能展示。

每个目录一个项目。每个项目都展示一个以上基础的功能。 其中CefQtJsApiDemo集成功能较多。
具体请查看各个项目的文档。

# CefCmakeDemo 使用CMake配置CefSimple
该项目展示了如果使用Cmake配置自己CefSimple项目。
详解请查看[CefCmakeDemo/README.md](https://github.com/iherewaitfor/cefdemos/blob/main/CefCmakeDemo/README.md)
# CefCmakeCefClient 使用CMake配置CefClient项目
该项目展示了如果使用Cmake配置CefClient项目。
详解请查看[https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeCefClient](https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeCefClient)

# CefExamples官方功能示例的本地配置
本项目为cef官方Examples项目的本地化配置。方便调试。
具体配置修改，请查看[CefExamples](https://github.com/iherewaitfor/cefdemos/tree/main/CefExamples)

# JavaScriptIntegration--JavaScript集成
本项目展示了如何给window对象添加javascript对象及函数。
具体请查看[https://github.com/iherewaitfor/cefdemos/tree/main/JavaScriptIntegration](https://github.com/iherewaitfor/cefdemos/tree/main/JavaScriptIntegration)

# schemedemo--自定义scheme，本地资源加载 展示
本项目是在JavascriptIntegration项目基础上，添加
- 本地资源加载：https://myjstest.com/index.html， 加本本地的index.html
- 自定义scheme资源加载：client://tests/scheme_test.html、client://tests/logo.png 分别加载本地的scheme-test.html、logo.png

具体 请查看[schemedemo](https://github.com/iherewaitfor/cefdemos/tree/main/schemedemo)

# cefsubprocessdemo--渲染进程使用不同的exe
本项目展示了，使用独立的exe作为渲染进程启动程序。
具体请查看[https://github.com/iherewaitfor/cefdemos/tree/main/cefsubprocessdemo](https://github.com/iherewaitfor/cefdemos/tree/main/cefsubprocessdemo)

# CefQtCmakeDemo--Cef+Qt集成配置
本项目使用Cmake集成了Cef和Qt。

具体请查看[CefQtCmakeDemo](https://github.com/iherewaitfor/cefdemos/tree/main/CefQtCmakeDemo)

# CefQtJsApiDemo--使用Qt集成JavaScript接口
本项目集成了Cef和Qt。使用Qt实现JavaScript接口。

展示了两种集成方式
- 使用libcef自带的的ipc通道，在browser和render进程之间通过。
- 使用QtRemoteObjects模块，实现render进程中的Javascript调用其他进程的接口。
    - 接口实现不限于Browser进程
    - 可在用户自己定义的进程，只需要提供对应的QtRemoteObjects的source Object即可。

具体请查看[[CefQtJsApiDemo](https://github.com/iherewaitfor/cefdemos/tree/main/CefQtJsApiDemo)](https://github.com/iherewaitfor/cefdemos/tree/main/CefQtJsApiDemo)