- [CefQtJspApiDemo Qt实现JS接口Demo](#cefqtjspapidemo-qt实现js接口demo)
- [How to Run](#how-to-run)
- [程序结构](#程序结构)
- [QtCefBrowser](#qtcefbrowser)
  - [cef应用核心框架](#cef应用核心框架)
    - [QCefClient--cef初始和关闭](#qcefclient--cef初始和关闭)
    - [Browser进程的CefApp之QCefBrowserApp](#browser进程的cefapp之qcefbrowserapp)
    - [Render进程的CefApp之QCefRenderApp](#render进程的cefapp之qcefrenderapp)
    - [Render进程之CefRenderProcessHandler](#render进程之cefrenderprocesshandler)
  - [Browser消息循环集成](#browser消息循环集成)
    - [CefDoMessageLoopWork()方式集成消息循环](#cefdomessageloopwork方式集成消息循环)
    - [multi\_threaded\_message\_loop = true方式集成消息循环（本项目采用）](#multi_threaded_message_loop--true方式集成消息循环本项目采用)
  - [本方案中涉及的进程和线程](#本方案中涉及的进程和线程)
  - [打开浏览器](#打开浏览器)
- [QCefV8Bind](#qcefv8bind)
  - [Cef自带的IPC](#cef自带的ipc)
    - [Browser进程收发消息](#browser进程收发消息)
    - [Render进程收发消息](#render进程收发消息)
  - [构建QObjects树](#构建qobjects树)
  - [将QObject树映射成JS Ojbects树，进行绑定](#将qobject树映射成js-ojbects树进行绑定)
    - [函数调用绑定QCefV8Handler](#函数调用绑定qcefv8handler)
    - [实际调用QObject的方法](#实际调用qobject的方法)
- [QCefV8BindRO](#qcefv8bindro)
  - [关于QObjectRemote](#关于qobjectremote)
  - [构建QObjects树](#构建qobjects树-1)
  - [将QObject转成JS Object 进行绑定](#将qobject转成js-object-进行绑定)
    - [绑定对象。](#绑定对象)
    - [绑定函数](#绑定函数)
    - [QCefV8Handler](#qcefv8handler)
      - [使用JS的Promise对象存储js调用。](#使用js的promise对象存储js调用)
      - [AsyncCefMethodCallback存储js的调用信息](#asynccefmethodcallback存储js的调用信息)
      - [在render线程 发出信号：调用replica的方法](#在render线程-发出信号调用replica的方法)
      - [在主线程：调用replica的方法](#在主线程调用replica的方法)
    - [返回结果给js](#返回结果给js)
    - [绑定信号](#绑定信号)
      - [JS连接对象](#js连接对象)
      - [](#)
- [关于调试设置](#关于调试设置)
  - [browser进程调试](#browser进程调试)
  - [render进程C++调试](#render进程c调试)
  - [javascript调试](#javascript调试)
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

关于Cef最核心的启动和关闭流程，可以参考[https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeDemo](https://github.com/iherewaitfor/cefdemos/tree/main/CefCmakeDemo)
### Browser进程的CefApp之QCefBrowserApp
涉及的回调方法，
- OnBeforeCommandLineProcessing（）
- GetBrowserProcessHandler()

具体的CefApp的回调方法，可以参考[cef_app.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/cef_app.h)。

在处理回调时，使用了代理的方式，代理接口为QCefBrowserApp::Delegate。代理的实现类QClientBrowserDelegate。


本项目中，QClientBrowserDelegate主要处理了命令行的参数处理。
添加了renderer-process-limit=1，限制renderer进程个数为1个，方便调试render进程。
```C++
void QClientBrowserDelegate::OnBeforeCommandLineProcessing(
    CefRefPtr<QCefBrowserApp> app,
    CefRefPtr<CefCommandLine> command_line) {
    if (!command_line->HasSwitch("renderer-process-limit")) {
        command_line->AppendSwitchWithValue("renderer-process-limit", "1");
    }
}
```


### Render进程的CefApp之QCefRenderApp
Render进程的CefApp主要实现的回调方法是
- GetRenderProcessHandler()

### Render进程之CefRenderProcessHandler
主要涉及的回调方法
- OnWebKitInitialized()
- OnContextCreated()
- OnContextReleased()
- OnBrowserCreated()
- OnBrowserDestroyed()

此处也使用代理的方式，cef回调时，都直接使用调用代理。例如：

```C++
void QCefRenderApp::OnWebKitInitialized() {
    std::set<client::RenderDelegate*> renderDelegates = qCefCoreAppPrivate()->renderDelegates();
    Q_FOREACH(client::RenderDelegate * renderDelegate, renderDelegates)
    {
        renderDelegate->OnWebKitInitialized();
    }
}
```


各个功能需要处理这些事件时，注册对应的代理对象过来即可。
代理对象接口为
```C++
class QCEFBROWSER_DECL_EXPORT RenderDelegate
    {
	public:
		virtual void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) {};
		virtual void OnWebKitInitialized() {}
		virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefDictionaryValue> extra_info) {}
		virtual void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) {}
		virtual void OnContextCreated(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context) {}
		virtual void OnContextReleased(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context) {}
		virtual bool OnProcessMessageReceived(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message) {
				return false;
		}
	};
```

可以通过方法QCefCoreApp::regRenderDelegate(client::RenderDelegate*)进行代理类注册。
本项目中，QCefV8Bind和QCefV8BindRO分别有注册代理类，用于在render进程中绑定js对象。
QCefV8BindRO的代理实现类为QCefV8BindRenderDelegate。具体实现看对应模块。

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
## 打开浏览器

使用QCefCoreApp::createBrowser打开一个浏览器页面。里面核心是调用 Cef的CefBrowserHost::CreateBrowser方法。


# QCefV8Bind
本模块，主要是使用cef自带的ipc通道，在browser进程和render进程之间通道。把QObjects树 映射到Js Objects树上，以实现在js上能调用 到QObject对象的方法、监听信号等。

关键步骤
- cef自带IPC
  - browser进程的收发消息
  - render进程的收发消息
- 构建QObjects树
- 将QObjects树转成JS Object，绑定到window
- 信号处理
## Cef自带的IPC
### Browser进程收发消息
Browser进程收消息。在QCefV8BindBrowserDelegate::OnProcessMessageReceived回调方法里收消息，主要要是重载了BrowserDelegate::OnProcessMessageReceived。

本方法回调是在cef的TID_UI线程的，通过Qt的信号，将算是消息逻辑切到主线程处理。
```C++
bool QCefV8BindBrowserDelegate::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
    //本回调运行在cef消息循环线程
    CefString message_name = message->GetName();
    if (message_name == cefv8bind_protcool::CefApiMetaDatasReq::message_name()
        || message_name == cefv8bind_protcool::InvokeReq::message_name())
    {
        //通过Qt信号把数据切到主线程进行处理。也可以使用CefPostTask切换。
        emit processMessageReceived(browser, frame, source_process, message);
        return true;
    }
	return false;
}
```

发送消息，则是通过
```C++
void CefFrame::SendProcessMessage(CefProcessId target_process,
                                  CefRefPtr<CefProcessMessage> message) = 0;
```
传入的目标CefProcessId为PID_BROWSER。
### Render进程收发消息
Render进程收消息是在
```C++
bool QCefV8BindRenderDelegate::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message)
```
由于render进程的消息循环用的是cef自己的，主线程跑的消息循环，回调都跑在主线程。  render进程中，不需要用到Qt的信号与槽。


发送消息，同样是通过
```C++
void CefFrame::SendProcessMessage(CefProcessId target_process,
                                  CefRefPtr<CefProcessMessage> message) = 0;
```

不过传入的目标CefProcessId为PID_RENDERER。
## 构建QObjects树
render进程在onContextCreated回调中请求browser进程把QObject相关信息。
browser进程收到请求后，进行QObject树信息构建。
```C++
        QList<cefv8bind_protcool::CefMetaObject>  cef_metaObjects;
        QCefV8ObjectHelper objectHelper;
        objectHelper.convertQObjectToCefObjects(QCefV8BindApp::getInstance()->d_func()->getRootObject(), nullptr, cef_metaObjects);
        objectHelper.convertQObjectToCefObjects(QCefCoreApp::getInstance()->getApiWindow(browser->GetIdentifier()), nullptr, cef_metaObjects);
        cefv8bind_protcool::CefApiMetaDatasResponse response;
        response.cef_metaObjects = cef_metaObjects;
        frame->SendProcessMessage(PID_RENDERER, response.makeIPCMessage());
```

具体通过
```C++
void QCefV8ObjectHelper::convertQObjectToCefObjects(const QObject *itemObject, const QObject*parentObject, QList<cefv8bind_protcool::CefMetaObject> &cef_metaObjects)
```
方法获得CEfMetObject。如果有多个根结点，则调用多次。支持多棵树构建。
## 将QObject树映射成JS Ojbects树，进行绑定
render进程收到响应后，使用
```C++
CefRefPtr<CefV8Value> QCefV8ObjectHelper::bindV8Objects(const QList<cefv8bind_protcool::CefMetaObject>& cef_metaObjects, CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Handler> v8Handler)
```
方法，将QOjbects映射成JS Objects树进行绑定。

其中根据每个QObject信息，创建每个JSObject的方法为
```C++
CefRefPtr<CefV8Value> QCefV8ObjectHelper::createV8Object(const cefv8bind_protcool::CefMetaObject& cefMetaObject, CefRefPtr<CefV8Handler> v8Handler, CefRefPtr<CefV8Context> context
)
{
	CefRefPtr<CefV8Value> rootV8Object = context->GetGlobal();
	CefRefPtr<CefV8Value> parentV8Object;
	if (!cefMetaObject.objectName.isEmpty())
	{
		parentV8Object = getV8Object(cefMetaObject.parentId, rootV8Object);
		if (parentV8Object == nullptr)
		{
			return nullptr;
		}
	}

	CefRefPtr<CefV8Value> v8Object = CefV8Value::CreateObject(nullptr, nullptr);
	// 把当前QObject接口的信息存储到一个QObject对象上，并把这个QObject对象存放到对应的V8Object的UserData
	QSharedPointer<QObject> obj = QSharedPointer<QObject>(new QObject());
	obj->setProperty(KCefMetaObject, QVariant::fromValue<cefv8bind_protcool::CefMetaObject>(cefMetaObject));
	obj->setProperty(KObjectId, cefMetaObject.objectId);
	obj->setProperty(KBrowserFrameId, context ? context->GetFrame()->GetIdentifier() : 0);
	v8Object->SetUserData(new QCefV8ObjectHolder<QSharedPointer<QObject>>(obj));

	//存当前对象
	QCefV8BindApp::getInstance()->d_func()->getObjectMgr()->insertRender(cefMetaObject.objectId, obj.data());

	foreach(CefMetaMethod metaMethod, cefMetaObject.metaMethods)
	{
		QString methodName = metaMethod.methodName;

		CefRefPtr<CefV8Value>  methodV8Object;
		if (metaMethod.methodType == QMetaMethod::Method)
		{
			methodV8Object = CefV8Value::CreateFunction(methodName.toStdString(), v8Handler);
		}
		else if (metaMethod.methodType == QMetaMethod::Signal)
		{
			methodV8Object = CefV8Value::CreateObject(nullptr, nullptr);
		}
		//存储方法的元数据，方便后续调用时获取。
		QSharedPointer<QObject> objMethod = QSharedPointer<QObject>(new QObject());
		objMethod->setProperty(KCefMetaMethod, QVariant::fromValue<cefv8bind_protcool::CefMetaMethod>(metaMethod));
		objMethod->setProperty(KObjectId, cefMetaObject.objectId);

		methodV8Object->SetUserData(new QCefV8ObjectHolder<QSharedPointer<QObject>>(objMethod));
		v8Object->SetValue(methodName.toStdString(), methodV8Object, V8_PROPERTY_ATTRIBUTE_NONE);
	}

	foreach(CefMetaProperty metaProperty, cefMetaObject.metaProperties)
	{
		QString propertyName = metaProperty.propertyName;
		CefRefPtr<CefValue> cef_value = metaProperty.cef_propertyValue;

		if (cef_value && cef_value->IsValid())
		{
			
			CefRefPtr<CefV8Value> propV8Object = QCefValueConverter::to(cef_value);
			if (propV8Object->IsValid())
			{
				v8Object->SetValue(QCefValueConverter::to(propertyName), propV8Object, V8_PROPERTY_ATTRIBUTE_READONLY);
			}
		}
	}
	if (parentV8Object)
	{
		//把创建的对象挂到parentObject上，如果有parent的话。
		const CefV8Value::PropertyAttribute attributes = static_cast<CefV8Value::PropertyAttribute>(V8_PROPERTY_ATTRIBUTE_READONLY | V8_PROPERTY_ATTRIBUTE_DONTENUM | V8_PROPERTY_ATTRIBUTE_DONTDELETE);
		parentV8Object->SetValue(cefMetaObject.objectName.toStdString(), v8Object, attributes);
	}

	return v8Object;
}
```

依次创建对象，方法、信号、属性等。
### 函数调用绑定QCefV8Handler
创建js函数绑定时，关键是要传入CefV8Handler对象，用于处理函数调用逻辑。这里使用的是QCefV8Handler
```C++
	foreach(CefMetaMethod metaMethod, cefMetaObject.metaMethods)
	{
		QString methodName = metaMethod.methodName;

		CefRefPtr<CefV8Value>  methodV8Object;
		if (metaMethod.methodType == QMetaMethod::Method)
		{
			methodV8Object = CefV8Value::CreateFunction(methodName.toStdString(), v8Handler);
		}
		else if (metaMethod.methodType == QMetaMethod::Signal)
		{
			methodV8Object = CefV8Value::CreateObject(nullptr, nullptr);
		}
		//存储方法的元数据，方便后续调用时获取。
		QSharedPointer<QObject> objMethod = QSharedPointer<QObject>(new QObject());
		objMethod->setProperty(KCefMetaMethod, QVariant::fromValue<cefv8bind_protcool::CefMetaMethod>(metaMethod));
		objMethod->setProperty(KObjectId, cefMetaObject.objectId);

		methodV8Object->SetUserData(new QCefV8ObjectHolder<QSharedPointer<QObject>>(objMethod));
		v8Object->SetValue(methodName.toStdString(), methodV8Object, V8_PROPERTY_ATTRIBUTE_NONE);
	}
```
另外存储好对应的ObjectId和MethodIndex，方便后续执行调用函数时使用。

JS调用后，handler把相关的调用信息，生成callbackid记录下来。将将要调用的函数信息，发ipc到browser进程，进行实际的QObject对象对应的方法调用 。

```C++
			CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();

			quint64  callbackId = 0;
			if (!cefMetaMethord.returnTypeName.isEmpty() && cefMetaMethord.returnTypeName != "void")
			{
				CefRefPtr<CefV8Value> window = context->GetGlobal();
				CefRefPtr<CefV8Value> createPromise = window->GetValue(KPromiseCreatorFunction);
				CefRefPtr<CefV8Value> promiseWrapper = createPromise->ExecuteFunctionWithContext(context, nullptr, CefV8ValueList());

				retval = promiseWrapper->GetValue("p");

				CefRefPtr<CefV8Value> resolve = promiseWrapper->GetValue("resolve");
				CefRefPtr<CefV8Value> reject = promiseWrapper->GetValue("reject");

				QSharedPointer<AsyncCefMethodCallback> asyncCallback = QSharedPointer<AsyncCefMethodCallback>(
					new AsyncCefMethodCallback(context, resolve, reject, context->GetFrame()->GetIdentifier(), name)
					);
				callbackId = m_asynCallbackMgr->saveAsyncMethodCallback(asyncCallback);
			}
			else
			{
				retval = CefV8Value::CreateInt(0);
			}
			cefv8bind_protcool::InvokeReq req;
			req.objctId = metaObj->property(KObjectId).toInt();
			req.methodName = name;
			req.methodIndex = methodIndex;
			req.callBackId = callbackId;
			req.methodArgs = toProcessMessage(arguments);

			sendIPCMessage(context, req);
```
### 实际调用QObject的方法
browser进程收到 InvokeReq消息后，执行对应的方法调用 。
```C++
    cefv8bind_protcool::InvokeReq req;
    if (req.unPack(message->GetArgumentList()))
    {
        QVariant retVar;

        QObject* apiObject = QCefV8BindApp::getInstance()->d_func()->getObjectMgr()->findBrowserObject(req.objctId);
        if (apiObject) {
            QMetaMethod metaMethod = apiObject->metaObject()->method(req.methodIndex);
            MetaInvoker metaInvoker;
            metaInvoker.object = apiObject;
            metaInvoker.metaMethod = metaMethod;
            
            CefRefPtr<CefValue> args = CefValue::Create();
            args->SetList(req.methodArgs);
            metaInvoker.args = QCefValueConverter::convertFromCefListToVariantList(args);
            metaInvoker.run();
            if (metaMethod.returnType() != QMetaType::Void) {
                cefv8bind_protcool::InvokeResp rsp;
                rsp.callBackId = req.callBackId;
                rsp.invokeResult = metaInvoker.ok;
                rsp.objctId = req.objctId;
                rsp.methodIndex = req.methodIndex;
                rsp.methodName = req.methodName;
                rsp.returnValue = QCefValueConverter::to(metaInvoker.result);
                frame->SendProcessMessage(PID_RENDERER, rsp.makeIPCMessage());
            }
        }
    }

```
调用QObject的方法使用辅助调用类MetaInvoker。最终调用 的是QmetaMethod::invoke。
```C++
bool MetaInvoker::run() {
	QList<QGenericArgument> tempArgs;
	for (int i = 0; i < args.size(); i++) {
        QVariant& arg = args[i];
        QGenericArgument gArg(QMetaType::typeName(arg.userType()),
            const_cast<void*>(arg.constData())
        );
        tempArgs << gArg;
	}
    const int retTypeId = metaMethod.returnType();
    void* retVal = QMetaType::create(retTypeId);
    QGenericReturnArgument returnArg( QMetaType::typeName(retTypeId), retVal);
    ok = metaMethod.invoke(
        object,
        Qt::DirectConnection,
        returnArg,
        tempArgs.value(0),
        tempArgs.value(1),
        tempArgs.value(2),
        tempArgs.value(3),
        tempArgs.value(4),
        tempArgs.value(5),
        tempArgs.value(6),
        tempArgs.value(7),
        tempArgs.value(8),
        tempArgs.value(9)
    );
    result = QVariant(retTypeId, retVal);
    return ok;
}
```
执行获得结果后，把执行结果通过 ipc消息InvokeResp发回到render进程。
# QCefV8BindRO
该模块的功能是使用QRemoteObjects技术，将QObject对象绑定成Javascript接口。让JS方便地调用各个QObject对象。QObject接口不限于browser进程。

关键步骤：

- QRemoteObjects
- 构建QObjects树
- 将QObject转成JS Object 进行绑定
- 调用QObject接口
- QOjbect信号
## 关于QObjectRemote
关于Qt Remote Objects,可以查看Qt官司文档[[Qt Remote Objects](https://qthub.com/static/doc/qt5/qtremoteobjects/qtremoteobjects-index.html#qt-remote-objects)](https://qthub.com/static/doc/qt5/qtremoteobjects/qtremoteobjects-index.html#qt-remote-objects)

Qt Remot Objects的示例，可以参考本git的[QtROsimpleswitch](https://github.com/iherewaitfor/QtDemos/tree/main/QtROsimpleswitch)。

以下两个项目，分别展示了QRemoteObject的提供服务 和客户端使用服务。
- [registryconnected_objects_server](https://github.com/iherewaitfor/QtDemos/tree/main/QtROsimpleswitch/registryconnected_objects_server)
- [registryconnected_objcets_client](https://github.com/iherewaitfor/QtDemos/tree/main/QtROsimpleswitch/registryconnected_objcets_client)

通过QRemoteObject中有几个概念
- 服务器侧 
  - QRemoteObjectRegistryHost
    - 提供注册点，用于注册和查询服务
  - QRemoteObjectHost
    - 提供ipc通信
    - 用于让QObject对象变成服务对象
  - SourceObject
    - QObject对象(继承了QObject)，提供的服务。可以让用户远程调用的对象。
    - Qt元数据系统能处理的属性、Q_INVOKABLE方法、信号、槽等。
- 客户端侧
  - QRemoteObjectNode
    - 提供了ipc通信能力
    - 用于查询有哪些服务对象
  - QRemoteObjectDynamicReplica
    - SourceObject的代理类。
    - 调用replica就像调用源QObject一样，不过是异步调用。
## 构建QObjects树
接口是通过QObject对象形式提供，可以把多个QObject组件成树形结构。接口可以有多颗树。

本项目实例中NumberLogic接口结点，SubNumberLogic为孩子结点。该 树型结构直接使用Qt的QObject树结构进行组织。
其中主要设置了两个事件
- 根结点的parent为空，非根结点需要设置parent
  - 如NumberLogic的根结点为空,SubNumberLogic的parent为NumberLogic
- 把接口的孩子结点，设置为父结点的属性
```c++
SubNumberLogic::SubNumberLogic(QObject*parent)
    : QObject(parent),m_counter(0){
    if (parent) {

        parent->setProperty("SubNumberLogic", QVariant::fromValue((QObject*)this));
    }
    setObjectName("SubNumberLogic");
}
```

可以通过QCefV8BindAppRO::addV8RootObject()方法，把接口实现的根结点添加到树上（browser进程提供的接口）。
通过RemoteObjectTreeHelper::getObjects()提供了接口树数据给render进程。其中RemoteObjectTreeHelper是一个Source Object.render那边可以直接查询得到该服务。

browser进程使用RemoteObjectStruct结构描述一个QObject对象结点。
```C++
typedef struct RemoteObjectStruct
{
    QString objName;
    QString parentName;
    QString v8Name;
}
```
通过以下方法getObjectsList得到所有结点的信息列表。
```C++
QList<RemoteObjectStruct> RemoteObjectTreeHelper::getObjectsList() {
	m_objectsList.clear();
	foreach(QObject * rootObject, m_rootObjectList) {
		convertQObjectToRemoteStructList(rootObject, nullptr, m_objectsList);
	}
	return m_objectsList;
}
```
最后通过getObjects()，提供给render进程获取，以便在render进程进行接口绑定。
```C++
QVariantList RemoteObjectTreeHelper::getObjects() {
	QVariantList list;
	foreach(RemoteObjectStruct objStruct, m_objectsList) {
		QVariantList temObj;
		temObj.append(objStruct.objName);
		temObj.append(objStruct.parentName);
		temObj.append(objStruct.v8Name);
		list.push_back(temObj);
	}
    return list;
}
```
## 将QObject转成JS Object 进行绑定
browser进程上的RemoteObjectTreeHelper已将QObject树信息构建好，并通过QtRemoteObject技术，把RemoteObjectTreeHelper作为Source Object,名字为"QCefRemoteObjectTreeHelper".提供了getObjects方法获取。

render进程的DynamicClientTreeHelper类，包括一个RemoteObjectTreeHelper的replica。因此可以像访问本地对象一样访问RemoteObjectTreeHelper。DynamicClientTreeHelper获取QObject树信息，并将期绑定到js的window对象上。

DynamicClientTreeHelper的replica在初始化好之后，立即调用getObjects方法获取树信息，然后获取所有SourceObject的replica
并使用Map存起来。key分别为名字和id。
```C++
    QMap<QString, QSharedPointer<DynamicClient>> m_dynamicClientsMap;
    QMap<int, QSharedPointer<DynamicClient>> m_dynamicClientsIdMap;
```

因为每个replica包括了QMetaObject（QRemoteObject技术中，source的元信息会自动同步到replica中），因此就可以用这些信息生成绑定所需的信息了。

```C++
void DynamicClientTreeHelper::getMetaObjects(QList<cefv8bind_protcool::CefMetaObject>& cef_metaObjects) {
    QCefV8ObjectHelper objectHelper;
    objectHelper.convertDynamicClientToCefObjects(cef_metaObjects);
}
```
通过DynamicClientTreeHelper::getMetaObjects就获取得到绑定所需的信息。
```C++
	struct  CefMetaMethod
	{
		QMetaMethod::MethodType methodType;
		int methodIndex;
		QString methodName;
		QStringList parameterTypes;
		QString	returnTypeName;
	};
	struct  CefMetaProperty
	{
		int propertyIndex;
		QString propertyName;
		QString propertyTypeName;
		QVariant propertyValue;
		CefRefPtr<CefValue>	cef_propertyValue;
	};
	struct CefMetaObject
	{
		QString					objectName;
		QString					className;
		QString					v8Name;
		int						objectId;
		int						parentId;
		QList<CefMetaMethod>	metaMethods;
		QList<CefMetaProperty>  metaProperties;
	};
```

使用QCefV8ObjectHelper::bindV8Objects将信息绑定到window上。
其中对每个对象执行createV8Object，进行具体的绑定

```C++
CefRefPtr<CefV8Value> QCefV8ObjectHelper::createV8Object(const cefv8bind_protcool::CefMetaObject& cefMetaObject, CefRefPtr<CefV8Handler> v8Handler, CefRefPtr<CefV8Context> context
)
{
	CefRefPtr<CefV8Value> rootV8Object = context->GetGlobal();
	CefRefPtr<CefV8Value> parentV8Object;
	if (!cefMetaObject.objectName.isEmpty())
	{
		parentV8Object = getV8Object(cefMetaObject.parentId, rootV8Object);
		if (parentV8Object == nullptr)
		{
			return nullptr;
		}
	}

	CefRefPtr<CefV8Value> v8Object = CefV8Value::CreateObject(nullptr, nullptr);
	// 把当前QObject接口的信息存储到一个QObject对象上，并把这个QObject对象存放到对应的V8Object的UserData
	QSharedPointer<QObject> obj = QSharedPointer<QObject>(new QObject());
	obj->setProperty(KCefMetaObject, QVariant::fromValue<cefv8bind_protcool::CefMetaObject>(cefMetaObject));
	obj->setProperty(KObjectId, cefMetaObject.objectId);
	obj->setProperty(KBrowserFrameId, context ? context->GetFrame()->GetIdentifier() : 0);
	v8Object->SetUserData(new QCefV8ObjectHolder<QSharedPointer<QObject>>(obj));

	//存当前对象
	QCefV8BindAppRO::getInstance()->d_func()->getObjectMgr()->insertRender(cefMetaObject.objectId, obj.data());

	foreach(CefMetaMethod metaMethod, cefMetaObject.metaMethods)
	{
		QString methodName = metaMethod.methodName;

		CefRefPtr<CefV8Value>  methodV8Object;
		if (metaMethod.methodType == QMetaMethod::Method)
		{
			methodV8Object = CefV8Value::CreateFunction(methodName.toStdString(), v8Handler);
		}
		else if (metaMethod.methodType == QMetaMethod::Signal)
		{
			methodV8Object = CefV8Value::CreateObject(nullptr, nullptr);
		}
		//存储方法的元数据，方便后续调用时获取。
		QSharedPointer<QObject> objMethod = QSharedPointer<QObject>(new QObject());
		objMethod->setProperty(KCefMetaMethod, QVariant::fromValue<cefv8bind_protcool::CefMetaMethod>(metaMethod));
		objMethod->setProperty(KObjectId, cefMetaObject.objectId);

		methodV8Object->SetUserData(new QCefV8ObjectHolder<QSharedPointer<QObject>>(objMethod));
		v8Object->SetValue(methodName.toStdString(), methodV8Object, V8_PROPERTY_ATTRIBUTE_NONE);
	}

	foreach(CefMetaProperty metaProperty, cefMetaObject.metaProperties)
	{
		QString propertyName = metaProperty.propertyName;
		CefRefPtr<CefValue> cef_value = metaProperty.cef_propertyValue;

		if (cef_value && cef_value->IsValid())
		{
			
			CefRefPtr<CefV8Value> propV8Object = QCefValueConverter::to(cef_value);
			if (propV8Object->IsValid())
			{
				v8Object->SetValue(QCefValueConverter::to(propertyName), propV8Object, V8_PROPERTY_ATTRIBUTE_READONLY);
			}
		}
	}
	if (parentV8Object)
	{
		//把创建的对象挂到parentObject上，如果有parent的话。
		const CefV8Value::PropertyAttribute attributes = static_cast<CefV8Value::PropertyAttribute>(V8_PROPERTY_ATTRIBUTE_READONLY | V8_PROPERTY_ATTRIBUTE_DONTENUM | V8_PROPERTY_ATTRIBUTE_DONTDELETE);
		parentV8Object->SetValue(cefMetaObject.v8Name.toStdString(), v8Object, attributes);
	}

	return v8Object;
}
```
### 绑定对象。
使用CefV8Value::CreateObject创建对象。

特别的，使用CefV8Value::SetUserData方法存在QObject相关信息，如OjbectId，FrameId等。
```C++
	CefRefPtr<CefV8Value> v8Object = CefV8Value::CreateObject(nullptr, nullptr);
	// 把当前QObject接口的信息存储到一个QObject对象上，并把这个QObject对象存放到对应的V8Object的UserData
	QSharedPointer<QObject> obj = QSharedPointer<QObject>(new QObject());
	obj->setProperty(KCefMetaObject, QVariant::fromValue<cefv8bind_protcool::CefMetaObject>(cefMetaObject));
	obj->setProperty(KObjectId, cefMetaObject.objectId);
	obj->setProperty(KBrowserFrameId, context ? context->GetFrame()->GetIdentifier() : 0);
	v8Object->SetUserData(new QCefV8ObjectHolder<QSharedPointer<QObject>>(obj));
```
### 绑定函数
绑定函数、使用CreateFunction创建创建函数。并注意把对应的QObject信息存储起来。到时回调时需要用到。
```C++
CefV8Value::CreateFunction(methodName.toStdString(), v8Handler);

//存储方法的元数据，方便后续调用时获取。
QSharedPointer<QObject> objMethod = QSharedPointer<QObject>(new QObject());
objMethod->setProperty(KCefMetaMethod, QVariant::fromValue<cefv8bind_protcool::CefMetaMethod>(metaMethod));
objMethod->setProperty(KObjectId, cefMetaObject.objectId);

methodV8Object->SetUserData(new QCefV8ObjectHolder<QSharedPointer<QObject>>(objMethod));
v8Object->SetValue(methodName.toStdString(), methodV8Object, V8_PROPERTY_ATTRIBUTE_NONE);
```
创建函数时传入QCefV8Handler对象，用于处理js的函数调用 。
### QCefV8Handler

#### 使用JS的Promise对象存储js调用。
在OnWebKitInitialized时就注册好扩展。该扩展用于创建JS的Promise，用于返回js调用异步结果。
```C++
void QCefV8BindRenderDelegate::OnWebKitInitialized(){
    CefRegisterExtension(CefString(L"qcef/promisecreator"), KPromiseCreatorScript, nullptr);
}
```
```C++
const char KPromiseCreatorFunction[] = "qCreatePromise";
const char KPromiseCreatorScript[] = ""
"function qCreatePromise() {"
"   var result = {};"
"   var promise = new Promise(function(resolve, reject) {"
"       result.resolve = resolve; result.reject = reject;"
"   });"
"   result.p = promise;"
"   return result;"
"}";
```
#### AsyncCefMethodCallback存储js的调用信息

```C++
class AsyncCefMethodCallback
{
public:
    AsyncCefMethodCallback(CefRefPtr<CefV8Context> context, const CefRefPtr<CefV8Value>& res, const CefRefPtr<CefV8Value>& rej, int64_t frameId, const CefString& sName);
    ~AsyncCefMethodCallback();

    void success(const CefRefPtr<CefV8Value>& result);
    void fail(const CefString& exception);

    int64_t frameId() const { return _frameId; };
    const CefString& name() const { return _sName; }
    uint64_t callTime() const { return _callTime; }
    void setTimeOutTime(uint64_t t) { m_timeOutTime = t; }
    uint64_t getTimeOutTime() {return m_timeOutTime;}

private:
    CefRefPtr<CefV8Context> _context;
    CefRefPtr<CefV8Value> _resolve;
    CefRefPtr<CefV8Value> _reject;
    int64_t _frameId;
    CefString _sName;
    uint64_t _callTime;
    uint64_t m_timeOutTime; //3s
};
```
#### 在render线程 发出信号：调用replica的方法
在回调函数
```C++
bool QCefV8Handler::Execute(const CefString& name, CefRefPtr<CefV8Value> v8Object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception)

```
中，通过v8Object->GetUserData()获取到之前绑定时存储的QObject及该方法的信息。（即该方法的相关元信息）

```C++
CefRefPtr<CefBaseRefCounted> userData = v8Object->GetUserData();
			cefv8bind_protcool::PendingcallReq req;
			req.objctId = metaObj->property(KObjectId).toInt();
			req.methodName = name;
			req.methodIndex = methodIndex;
			req.callBackId = callbackId;
			req.methodArgs = toProcessMessage(arguments);
			req.frameId = m_frame->GetIdentifier();
			//emit signal
			QCefV8BindAppRO::getInstance()->d_func()->callReplicaMethod(req);
```

取得元信息后，就可以通过这些信息，明确需要调用哪个replica，及其只个方法了。
主要通过 objectId、methodIndox确定。

由于调用完replica的方法后，还需要给回怎么js，所以还需要存在 对应的callBackId、frameId。

由于该调用是在Render的TID_RENDERER线程，此处使用了Qt的信号 callReplicaMethod，将对replica的方法调用 放到Qt的消息循环线程（主线程）执行。

#### 在主线程：调用replica的方法
通过QMetaMethod的invoke进行方法调用 。传入对应的QObject及参数。
```C++
void QCefV8BindAppROPrivate::callReplicaMethod_slot(cefv8bind_protcool::PendingcallReq req) {

	if (!m_pDynamicClientTreeHelper->getObjectsIdMap().contains(req.objctId)) {
		return;
	}
	QSharedPointer<DynamicClient> pDynamicClient = m_pDynamicClientTreeHelper->getObjectsIdMap().value(req.objctId);
	if (pDynamicClient.isNull()) {
		return;;
	}
	QObject* apiObject = pDynamicClient->getReplica().data();
	if (!apiObject) {
		return;
	}
	QMetaMethod metaMethod = apiObject->metaObject()->method(req.methodIndex);
	MetaInvoker metaInvoker;
	metaInvoker.object = apiObject;
	metaInvoker.metaMethod = metaMethod;
	metaInvoker.callbackId = req.callBackId;
	metaInvoker.frameId = req.frameId;

	CefRefPtr<CefValue> args = CefValue::Create();
	args->SetList(req.methodArgs);
	metaInvoker.args = QCefValueConverter::convertFromCefListToVariantList(args);

	metaInvoker.run();
}
```

```C++
bool MetaInvoker::run() {
	QList<QGenericArgument> tempArgs;
	for (int i = 0; i < args.size(); i++) {
        QVariant& arg = args[i];
        QGenericArgument gArg(QMetaType::typeName(arg.userType()),
            const_cast<void*>(arg.constData())
        );
        tempArgs << gArg;
	}
    if (metaMethod.returnType() != QMetaType::Void) {
        QRemoteObjectPendingCall call;
        ok = metaMethod.invoke(
            object,
            Qt::AutoConnection,
            Q_RETURN_ARG(QRemoteObjectPendingCall, call),
            tempArgs.value(0),
            tempArgs.value(1),
            tempArgs.value(2),
            tempArgs.value(3),
            tempArgs.value(4),
            tempArgs.value(5),
            tempArgs.value(6),
            tempArgs.value(7),
            tempArgs.value(8),
            tempArgs.value(9)
        );
        QRemoteObjectPendingCallWatcher* callwathcer = new QRemoteObjectPendingCallWatcher(call);
        callwathcer->setProperty("callbackId", callbackId);
        callwathcer->setProperty("frameId", frameId);
        QObject::connect(callwathcer, SIGNAL(finished(QRemoteObjectPendingCallWatcher*)), 
            QCefV8BindAppRO::getInstance()->d_func(), SLOT(pendingCallResult(QRemoteObjectPendingCallWatcher*)));
        //to do . can save pointer callwathcer, and free at some time later (ex:120 secondes),
        // if the finished signal not emited.(the source remote object perhaps has crashed.)
    }
    else {
     ok = metaMethod.invoke(
            object,
            Qt::AutoConnection,
            tempArgs.value(0),
            tempArgs.value(1),
            tempArgs.value(2),
            tempArgs.value(3),
            tempArgs.value(4),
            tempArgs.value(5),
            tempArgs.value(6),
            tempArgs.value(7),
            tempArgs.value(8),
            tempArgs.value(9)
        );
    }
    return ok;
}
```

其中注意返回类型使用QRemoteObjectPendingCall，使用QRemoteObjectPendingCallWatcher监听sourceObject的调用返回。
```C++
 QRemoteObjectPendingCallWatcher* callwathcer = new QRemoteObjectPendingCallWatcher(call);
        callwathcer->setProperty("callbackId", callbackId);
        callwathcer->setProperty("frameId", frameId);
        QObject::connect(callwathcer, SIGNAL(finished(QRemoteObjectPendingCallWatcher*)), 
            QCefV8BindAppRO::getInstance()->d_func(), SLOT(pendingCallResult(QRemoteObjectPendingCallWatcher*)));
```

关于QRemoteObjectPendingCall可以查看[关于qobjectremote](https://github.com/iherewaitfor/cefdemos/tree/main/CefQtJsApiDemo#%E5%85%B3%E4%BA%8Eqobjectremote)，对应的示例，有展示如果使用QRemoteObjectPendingCall。

### 返回结果给js
```C++
void QCefV8BindAppROPrivate::pendingCallResult(QRemoteObjectPendingCallWatcher* call) {
	qint64 callbackId = call->property("callbackId").toLongLong();
	qint64 frameId = call->property("frameId").toLongLong();
	cefv8bind_protcool::PendingcallResp resp;
	resp.callBackId = callbackId;
	resp.frameId = frameId;
	resp.invokeResult = (call->error() == QRemoteObjectPendingCall::NoError);
	resp.returnValue = QCefValueConverter::to(call->returnValue());
	m_renderDelegate->onPendingcallResp(resp, frameId);
	call->deleteLater(); //释放对象。
}
```
通过 CefPostTask(TID_RENDERER, base::BindOnce(&QCefV8BindRenderDelegate::onPendingcallResp, this, rsp, frameId))将返回逻辑切到TID_RENDERER线程。
```C++
void QCefV8BindRenderDelegate::onPendingcallResp(cefv8bind_protcool::PendingcallResp rsp, int64 frameId) {
    if (!CefCurrentlyOn(TID_RENDERER)) {
        CefPostTask(TID_RENDERER, base::BindOnce(&QCefV8BindRenderDelegate::onPendingcallResp, this, rsp, frameId));
        return;
    }
    if (!m_frameHandlers.contains(frameId)) {
        return;
    }
    CefRefPtr<QCefV8Handler> v8Handler = m_frameHandlers.value(frameId);
    CefRefPtr<CefV8Context> context = v8Handler->getFrame()->GetV8Context();
    v8Handler->onPendingcallResp(rsp, context);
}
```


具体执行js的结果返回。通过callBackId找到调用的上下文。
```C++
void QCefV8Handler::onPendingcallResp(cefv8bind_protcool::PendingcallResp rsp, CefRefPtr<CefV8Context> context)
{
	if (m_frame == NULL)
	{
		return;
	}
	QSharedPointer<AsyncCefMethodCallback> callback = m_asynCallbackMgr->takeAsyncMethodCallback(rsp.callBackId);
	if (!callback)
	{
		return;
	}
	if (!rsp.invokeResult)
	{
		callback->fail("fail exception");
		return;
	}
	if (context->GetFrame()->GetIdentifier() == callback->frameId()
		&& context->Enter()
		)
	{
		CefRefPtr<CefV8Value> retV8Value = QCefValueConverter::to(rsp.returnValue);
		context->Exit();
		callback->success(retV8Value);
	}
}
```
### 绑定信号
信号的绑定，以一个js Object的方式绑定到js对象上。并在js对象上存在 对应的KobejctId及方法相关信息。使用CefV8Value::SetUserData存储到对应的js 对象上。
```C++
	foreach(CefMetaMethod metaMethod, cefMetaObject.metaMethods)
	{
		QString methodName = metaMethod.methodName;

		CefRefPtr<CefV8Value>  methodV8Object;
		if (metaMethod.methodType == QMetaMethod::Method)
		{
			methodV8Object = CefV8Value::CreateFunction(methodName.toStdString(), v8Handler);
		}
		else if (metaMethod.methodType == QMetaMethod::Signal)
		{
			methodV8Object = CefV8Value::CreateObject(nullptr, nullptr);
		}

		QSharedPointer<QObject> obj = QSharedPointer<QObject>(new QObject());
		obj->setProperty(KCefMetaMethod, QVariant::fromValue<cefv8bind_protcool::CefMetaMethod>(metaMethod));
		obj->setProperty(KObjectId, cefMetaObject.objectId);

		methodV8Object->SetUserData(new QCefV8ObjectHolder<QSharedPointer<QObject>>(obj));
		v8Object->SetValue(methodName.toStdString(), methodV8Object, V8_PROPERTY_ATTRIBUTE_NONE);
	}
```

#### JS连接对象
在OnContextCreated的时机，绑定好公共的Connect和disconnectSignal方法。

```C++
void QCefV8ObjectHelper::bindGlobalFunctions(CefRefPtr<CefV8Value> window, CefRefPtr<CefV8Handler> v8Handler)
{
	QStringList globalFuncs;
	globalFuncs.append(ConnectSignal);
	globalFuncs.append(DisConnectSignal);

	foreach(QString functionName, globalFuncs)
	{
		CefRefPtr<CefV8Value> v8Value = CefV8Value::CreateFunction(QCefValueConverter::to(functionName), v8Handler);
		const CefV8Value::PropertyAttribute attributes = static_cast<CefV8Value::PropertyAttribute>(V8_PROPERTY_ATTRIBUTE_READONLY | V8_PROPERTY_ATTRIBUTE_DONTENUM | V8_PROPERTY_ATTRIBUTE_DONTDELETE);
		window->SetValue(QCefValueConverter::to(functionName), v8Value, attributes);
	}
}
```

在v8handler中，发是连接信号函数时，使用QCefV8SignalManager::connectSignal处理连接逻辑。

主要是记录连接信号的ObjectId及methodIndex.
以下时机需要这些信息

- 去连接replica的信号
- 分发信号到js

处理原生连接的逻辑:

发出连接信号，切回主线程处理。
```C++
cefv8bind_protcool::ConnectReplicaSignal connectMsg;
		connectMsg.methodIndex = methodIndex;
		connectMsg.objctId = objectId;
		QCefV8BindAppRO::getInstance()->d_func()->connectReplicaSignal(connectMsg);
```

具体执行连接逻辑。使用AutoSignalsEmitter连接replica的信号 。AutoSignalsEmitter在收到replica信号时，会将其转发到js。
```C++
void QCefV8BindAppROPrivate::connectReplicaSignal_slot(cefv8bind_protcool::ConnectReplicaSignal req) {

	if (!m_pDynamicClientTreeHelper->getObjectsIdMap().contains(req.objctId)) {
		return;
	}
	QSharedPointer<DynamicClient> pDynamicClient = m_pDynamicClientTreeHelper->getObjectsIdMap().value(req.objctId);
	if (pDynamicClient.isNull()) {
		return;;
	}
	QObject* apiObject = pDynamicClient->getReplica().data();
	if (!apiObject) {
		return;
	}
	QMetaMethod metaMethod = apiObject->metaObject()->method(req.methodIndex);
	if (metaMethod.methodType() != QMetaMethod::Signal) {
		return;
	}
	//to do: same the autoEmitter, and check if the autoEmitter exist. if exist do not new.
	//to do: connect the signal of the object
	QString key = QString("%1_%2").arg(req.objctId).arg(req.methodIndex);
	if (m_signalMap.contains(key)) {
		return;
	}
	AutoSignalsEmitter* autoEmitter = new AutoSignalsEmitter(metaMethod,req.objctId, apiObject);
	QObject::connect(apiObject, QString("2").append(metaMethod.methodSignature()).toStdString().c_str(),
		autoEmitter, SLOT(proxySlot()), Qt::DirectConnection);
	m_signalMap.insert(key, autoEmitter);

}
```

转发逻辑：使用CefPostTask切换线程，运行所有v8Handler->dispatchReplicaSignaToJs(rsp, context);给每个页面分发。
```C++
void AutoSignalsEmitter::proxySignalEmit(void** _a)
{
    QVariantList args;
    int i = 0;
    foreach(QByteArray typeName, m_signalMetaMethod.parameterTypes())
    {
        int type = QMetaType::type(typeName.constData());

        // preincrement: start with 1
        QVariant arg(type, _a[++i]);
        // (_a[0] is return value)
        args << arg;
    }
    if (QCefV8BindAppRO::getInstance()->d_func()->getRenderDelegate()) {
        QObject* obj = sender();
        QString objName = obj->objectName();
        if (!QCefV8BindAppRO::getInstance()->d_func()->getReplicaTreeHelper()->getObjectsMap().contains(objName)) {
            return;
        }
        QSharedPointer<DynamicClient> dClient = QCefV8BindAppRO::getInstance()->d_func()->getReplicaTreeHelper()->getObjectsMap().value(objName);
        
        cefv8bind_protcool::DispatchReplicaSignaToJs msg;
        msg.objectId = dClient->property(KObjectId).toInt();
        
        //QString signature = m_signalMetaMethod.methodSignature();
        //QString name = m_signalMetaMethod.name();
        //signature;
        //name;
        msg.methodName = QCefValueConverter::to(QString(m_signalMetaMethod.name()));
        msg.methodIndex = m_signalMetaMethod.methodIndex();
        msg.methodArgs = QCefValueConverter::to(args)->GetList();
        QCefV8BindAppRO::getInstance()->d_func()->getRenderDelegate()->dispatchReplicaSignaToJs(msg);
    }
}
```

最终执行js监听信号的回调函数，返回信号给js。
```C++
void QCefV8SignalManager::dispatchReplicaSignaToJs(const cefv8bind_protcool::DispatchReplicaSignaToJs& msg, CefRefPtr<CefV8Context> context) {
	QString key = _getKey(msg.objectId, msg.methodIndex);
	if (m_slots.contains(key))
	{
		QList<CefRefPtr<CefV8Value>> cef_slots = m_slots[key];

		Q_FOREACH(CefRefPtr<CefV8Value> callBack, cef_slots)
		{
			V8ContextCaller caller(context);
			CefV8ValueList arguments_out = _convertToV8Args(msg.methodArgs);

			// Execute the callback.
			CefRefPtr<CefV8Value> retval = callBack->ExecuteFunctionWithContext(context, nullptr, arguments_out);
			if (retval.get())
			{
				if (retval->IsBool())
					retval->GetBoolValue();
			}
		}
	}
}
```
####
# 关于调试设置
## browser进程调试
## render进程C++调试
## javascript调试
# 参考

[https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md)