# schemedemo 
本顶目 主要展示了两个功能
- 通用资源管理
  - 可以针对url，优先本地提供资源，不需要走网络请求资源。
  - 如："https://myjstest.com/index.html"
- scheme handler
  - 自定义scheme，提供资源
  - 如："client://tests/scheme_test.html"

## 

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
  - GetResourceHandler()
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


# scheme handler


# 参考
[https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-generic-resource-manager](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-generic-resource-manager)

[https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-scheme-handler](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-scheme-handler)