# JavaScriptIntegration
本项目，用于展示如果在cef的render进程添加Javascript接口。
主要添加
1. CefRegisterExtension添加扩展
1. 添加属性值
2. 添加对象
3. 添加函数
# How to Run
编译及运行请参考[https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeDemo#how-to-run](https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeDemo#how-to-run)
# 程序结构

主要的回调类
- CefApp
  - GetBrowserProcessHandler
  - GetRenderProcessHandler
- GetBrowserProcessHandler
- **CefRenderProcessHandler**
  - OnWebKitInitialized()
  - OnContextCreated（）
  - OnContextReleased（）
  - CefV8Accessor回调类
  - CefV8Handler回调类
- CefClient 打开页面时需要
  - GetLifeSpanHandler()
- LifeSpanHandler页面生命周期管理
  - OnBeforePopup
  - OnAfterCreated()
  - DoClose()
  - OnBeforeClose()

本项目主要程序结构与CefSimple一样[https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeDemo](https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeDemo)。
主要是扩展了render进程中Javascript相关逻辑。

Javascript运行在render进程。主要
主要逻辑写在CefApp的回调接口CefRenderProcessHandler。

该回调接口，几个核心方法点:
```C++
class CefRenderProcessHandler : public virtual CefBaseRefCounted {
    virtual void OnWebKitInitialized() {}
    virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) {}
    virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context) {}
}
```

绑定V8对象，主要回调对象
- CefV8Accessor
- CefV8Handler

CefV8Accessor

```C++
class CefV8Accessor : public virtual CefBaseRefCounted {
    public:
        virtual bool Get(const CefString& name,
                        const CefRefPtr<CefV8Value> object,
                        CefRefPtr<CefV8Value>& retval,
                        CefString& exception) = 0;
        virtual bool Set(const CefString& name,
                        const CefRefPtr<CefV8Value> object,
                        const CefRefPtr<CefV8Value> value,
                        CefString& exception) = 0;
}
```

CefV8Handler

```C++
class CefV8Handler : public virtual CefBaseRefCounted {
    public:
        virtual bool Execute(const CefString& name,
                            CefRefPtr<CefV8Value> object,
                            const CefV8ValueList& arguments,
                            CefRefPtr<CefV8Value>& retval,
                            CefString& exception) = 0;
};
```

# 绑定时机
## OnWebKitInitialized
在WebKit初始化后就会有。不需要等页面打开。在这里可以使用CefRegisterExtension添加javascript扩展。
这里无CefV8Context。
## CefRenderProcessHandler::OnContextCreated
```C++
  virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context)
```
在每个frame打开时，都会回调。有CefV8Context，可以进行JS对象绑定。
# Basic JS Types
可以参考
[CefV8Value](https://bitbucket.org/chromiumembedded/cef/src/master/include/cef_v8.h)

- CreateUndefined
```C++
  static CefRefPtr<CefV8Value> CreateNull();
  static CefRefPtr<CefV8Value> CreateBool(bool value);
  static CefRefPtr<CefV8Value> CreateInt(int32 value);
  static CefRefPtr<CefV8Value> CreateUInt(uint32 value);
  static CefRefPtr<CefV8Value> CreateDouble(double value);
  static CefRefPtr<CefV8Value> CreateDate(const CefTime& date);
  static CefRefPtr<CefV8Value> CreateString(const CefString& value);
  static CefRefPtr<CefV8Value> CreateArrayBuffer(
      void* buffer,
      size_t length,

```
# JS Arrays
```
  static CefRefPtr<CefV8Value> CreateArray(int length);
```
# JS Objects
```C++
  static CefRefPtr<CefV8Value> CreateObject(
      CefRefPtr<CefV8Accessor> accessor,
      CefRefPtr<CefV8Interceptor> interceptor);
```
# JS Functions
```C++
      CefRefPtr<CefV8ArrayBufferReleaseCallback> release_callback);
  static CefRefPtr<CefV8Value> CreateFunction(const CefString& name,
                                              CefRefPtr<CefV8Handler> handler);
```

# 参考
1. [https://bitbucket.org/chromiumembedded/cef/wiki/JavaScriptIntegration.md](https://bitbucket.org/chromiumembedded/cef/wiki/JavaScriptIntegration.md)
2. [https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeDemo#how-to-run](https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeDemo#how-to-run)
3. [https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md#markdown-header-application-structure](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md#markdown-header-application-structure)
