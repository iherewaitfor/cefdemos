- [schemedemo](#schemedemo)
- [通用资源管理](#通用资源管理)
  - [CefClient::GetRequestHandler()](#cefclientgetrequesthandler)
  - [new 一个CefResourceManager](#new-一个cefresourcemanager)
  - [资源提供者BinaryResourceProvider](#资源提供者binaryresourceprovider)
    - [重写OnRequest()处理资源加载逻辑](#重写onrequest处理资源加载逻辑)
      - [命中的处理逻辑CefStreamResourceHandler](#命中的处理逻辑cefstreamresourcehandler)
      - [CefResourceManager::Request](#cefresourcemanagerrequest)
- [scheme handler](#scheme-handler)
- [参考](#参考)


# schemedemo 
本顶目 主要展示了两个功能
- 通用资源管理
  - 可以针对url，优先本地提供资源，不需要走网络请求资源。
  - 如："https://myjstest.com/index.html"
- scheme handler
  - 自定义scheme，提供资源
  - 如："client://tests/scheme_test.html"


# 通用资源管理

会过滤所有url，无论是什么协议。
比如
- 标准的https的 "https://myjstest.com/index.html"
- 用户自定义的   "abcdef://myjstest.com/index.html"

用户可以根据自己的逻辑需要命中哪些url，然后自己提供对应url的资源。而不需要走网络。

## CefClient::GetRequestHandler()
需要实现自己的RequestHandler.
- CefClient::GetRequestHandler()
- CefRequestHandler::GetResourceRequestHandler()
- CefResourceRequestHandler
  - OnBeforeResourceLoad()
    - 调用CefResourceManager::OnBeforeResourceLoad
  - GetResourceHandler()
    - 调用CefResourceManager::GetResourceHandler
## new 一个CefResourceManager
Browser进程里，
在SimpleHandler(CefClient)中new一个CefResourceManager。
通过AddProvider方法添加新的Provider。提供资源内容。
```C++
// Add example Providers to the CefResourceManager.
void SetupResourceManager(CefRefPtr<CefResourceManager> resource_manager) {
    if (!CefCurrentlyOn(TID_IO)) {
        // Execute on the browser IO thread.
        CefPostTask(TID_IO, base::BindOnce(SetupResourceManager, resource_manager));
        return;
    }
    resource_manager->AddProvider(
        new BinaryResourceProvider(test_host), 100, std::string());
}
```
通过 CefPostTask，将逻辑切到TID_IO线程执行。

官方文档有说明：Provider对象可以放在任意线程new。但方法回调，和对象销毁，都会是在TID_IO线程。
本项目中，全部放在了TID_IO线程执行。


## 资源提供者BinaryResourceProvider

- CefResourceManager::Provider
  - 新建自定义类BinaryResourceProvider继承Provider类
  - 核心方法OnRequest
    - CefResourceManager::Request
      - request->Continue(handler)
      - CefStreamReader
      - CefStreamReader::CreateForFile
    - 返回true表示已处理完，已拿到资源。
    - 返回false，交回经cef处理

新扩展一个类

```C++
////////////////////////////////////////////////////////////

const std::string& test_host = "https://myjstest.com/";


// Provider implementation for loading BINARY resources from the current
// executable.
class BinaryResourceProvider : public CefResourceManager::Provider {
public:
    explicit BinaryResourceProvider(const std::string& root_url)
        : root_url_(root_url) {
        DCHECK(!root_url.empty());
    }

    bool OnRequest(scoped_refptr<CefResourceManager::Request> request) override {
        CEF_REQUIRE_IO_THREAD();

        const std::string& url = request->url();
        if (url.find(root_url_) != 0L) {
            // Not handled by this provider.
            return false;
        }

        CefRefPtr<CefResourceHandler> handler;

        const std::string& relative_path = url.substr(root_url_.length());

#ifndef UNICODE
        std::string strPath;
#else
        std::wstring strPath;
#endif // !UNICODE
        TCHAR path[MAX_PATH];
        ::GetModuleFileName(NULL, path, MAX_PATH);
        strPath = path;

        size_t index = strPath.rfind(TEXT("\\"));
        strPath = strPath.substr(0, index);
        strPath = (TCHAR*)strPath.append(TEXT("\\index.html")).c_str();
        if (relative_path == "index.html") {
            //"D:\\srccode\\cefdemos\\JavaScriptIntegration\\build\\cefsimple\\Debug\\index.html"
            CefRefPtr<CefStreamReader> stream = CefStreamReader::CreateForFile(strPath);
            if (stream.get()) {
                handler = new CefStreamResourceHandler(
                    request->mime_type_resolver().Run(url), stream);
            }
        }

        request->Continue(handler);
        return true;
    }

private:
    std::string root_url_;

    DISALLOW_COPY_AND_ASSIGN(BinaryResourceProvider);
};
```

### 重写OnRequest()处理资源加载逻辑

返回值
    - 返回true： 表示已处理。已取得资源。
    - 返回fasle: 表示未处理。接下来，交回给cef处理。


过滤url，根据自定义的逻辑，是否命中，自己提供资源。
- 若命中
  - 提供资源，返回true
- 未命中
  - 直接返回false

#### 命中的处理逻辑CefStreamResourceHandler

若命中，则自己提供对应的资源。提供的方式有多种：
StreamReader的方式
- CefStreamReader::CreateForFile(const CefString& fileName)
  - 传入文件路径即可。
- CefStreamReader::CreateForData(void* data, size_t size)
  - 传数据data的内存地址和大小 
- CefStreamReader::CreateForHandler(CefRefPtr<CefReadHandler> handler)
  - 传入自定义的CefReadHandler
  - 自定义如何读取内容。

本项目，展示，使用了CefStreamReader::CreateForFile，提供文件的本地的路径。
```C++
        if (relative_path == "index.html") {
            //"D:\\srccode\\cefdemos\\JavaScriptIntegration\\build\\cefsimple\\Debug\\index.html"
            CefRefPtr<CefStreamReader> stream = CefStreamReader::CreateForFile(strPath);
            if (stream.get()) {
                handler = new CefStreamResourceHandler(
                    request->mime_type_resolver().Run(url), stream);
            }
        }
        request->Continue(handler);
```

合到CefStreamReader实例后，再去创建CefStreamResourceHandler对象。
#### CefResourceManager::Request
上面拿到CefStreamResourceHandler对象后，给到CefResourceManager::Request对象使用.

CefResourceManager::Request对象调用Continue方法，进行资源的获取。
```C++
    void Continue(CefRefPtr<CefResourceHandler> handler);
```
至此，cef就可以用用户定义的资源获取handler，获得资源了。

# scheme handler


# 参考
[https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-generic-resource-manager](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-generic-resource-manager)

[https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-scheme-handler](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-scheme-handler)