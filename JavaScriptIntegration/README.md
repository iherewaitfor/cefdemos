- [JavaScriptIntegration](#javascriptintegration)
- [How to Run](#how-to-run)
- [程序结构](#程序结构)
- [绑定时机](#绑定时机)
  - [OnWebKitInitialized](#onwebkitinitialized)
  - [CefRenderProcessHandler::OnContextCreated](#cefrenderprocesshandleroncontextcreated)
- [注册JS扩展](#注册js扩展)
- [Basic JS Types](#basic-js-types)
- [JS Arrays](#js-arrays)
- [JS Objects](#js-objects)
- [JS Functions](#js-functions)
- [Javascript测试代码](#javascript测试代码)
- [使用chrome进行调试](#使用chrome进行调试)
- [参考](#参考)


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
# 注册JS扩展
在OnWebKitInitialized回调中，使用CefRegisterExtension进行注册即可。
```C++
void SimpleAppRender::OnWebKitInitialized() {
    // Define the extension contents.
    std::string extensionCode =
        "var test;"
        "if (!test)"
        "  test = {};"
        "(function() {"
        "  test.myval = 'My Value!';"
        "})();";

    // Register the extension.
    CefRegisterExtension("v8/test", extensionCode, nullptr);
}
```
# Basic JS Types
可以参考
[CefV8Value](https://bitbucket.org/chromiumembedded/cef/src/master/include/cef_v8.h)

```C++
  static CefRefPtr<CefV8Value> CreateUndefined();
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

绑定基础类型，经String为例，在OnContextCreated回调方法里添加。

```c++
void SimpleAppRender::OnContextCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
    // Retrieve the context's window object.
    CefRefPtr<CefV8Value> window = context->GetGlobal();

    // Create a new V8 string value. See the "Basic JS Types" section below.
    CefRefPtr<CefV8Value> str = CefV8Value::CreateString("My Value in window!");

    // Add the string to the window object as "window.myval". See the "JS Objects" section below.
    window->SetValue("myval", str, V8_PROPERTY_ATTRIBUTE_NONE);
}
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
```C++
    CefRefPtr<CefV8Accessor> accessor = new MyV8Accessor();
    CefRefPtr<CefV8Value> obj = CefV8Value::CreateObject(accessor, nullptr);
    
    //In order for a value to be passed to the accessor 
    //it must be set using the SetValue() method variant that accepts AccessControl and PropertyAttribute arguments.
    obj->SetValue("myval", V8_ACCESS_CONTROL_DEFAULT,
        V8_PROPERTY_ATTRIBUTE_NONE);
    //给object添加属性
    obj->SetValue("myval", CefV8Value::CreateString("My Value in myObject!"), V8_PROPERTY_ATTRIBUTE_NONE);
    CefRefPtr<CefV8Handler> handler = new MyCefV8Handler();
    // Create the "myfunc" function.
    CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("myfunc", handler);
    // Add the "myfunc" function to the "window" object.
    obj->SetValue("myfunc", func, V8_PROPERTY_ATTRIBUTE_NONE);
    window->SetValue("myObjcet", obj, V8_PROPERTY_ATTRIBUTE_NONE);
```

JS Object使用CefV8Value::CreateObject进行创建。可以使用SetValue给JS Object添加属性值 或者函数。如上代码所示。

# JS Functions
```C++
      CefRefPtr<CefV8ArrayBufferReleaseCallback> release_callback);
  static CefRefPtr<CefV8Value> CreateFunction(const CefString& name,
                                              CefRefPtr<CefV8Handler> handler);
```
JS使用 CreateFunction创建，其中CefV8Handler的回调，用来实现函数的具体执行。
如示例中，的myfunc函数实现了返回一个String的功能。
```C++
class MyCefV8Handler : public CefV8Handler {
public:
    virtual bool Execute(const CefString& name,
        CefRefPtr<CefV8Value> object,
        const CefV8ValueList& arguments,
        CefRefPtr<CefV8Value>& retval,
        CefString& exception) {
        if (name == "myfunc") {
            // Return my string value.
            retval = CefV8Value::CreateString("My Value!");
            return true;
        }
        // Function does not exist.
        return false;
    }
    IMPLEMENT_REFCOUNTING(MyCefV8Handler);
};
```
# Javascript测试代码
绑定对象后，可以在页面上调用。
```Javascript
test.myval; //扩展中注册的test对象的myval
window.myval; // 绑定到window对象的myval
myObjcet.myval;//绑定到myObject的myval，myObject绑定到window对象
myObjcet.myfunc();//绑定以myObject的myfunc函数
```
# 使用chrome进行调试
设置调试接口
```C++
  // Specify CEF global settings here.
  CefSettings settings;
  //设置调试端口
  //调试方法：在chrome中打开http://localhost:8765
  settings.remote_debugging_port = 8765;
```
程序启动后，直接 在chrome中打开 http://localhost:8765。选择对应的页面，即可进入调试命令窗口了。

# 参考
1. [https://bitbucket.org/chromiumembedded/cef/wiki/JavaScriptIntegration.md](https://bitbucket.org/chromiumembedded/cef/wiki/JavaScriptIntegration.md)
2. [https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeDemo#how-to-run](https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeDemo#how-to-run)
3. [https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md#markdown-header-application-structure](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md#markdown-header-application-structure)
