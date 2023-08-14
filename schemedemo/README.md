- [schemedemo](#schemedemo)
- [通用资源管理](#通用资源管理)
  - [CefClient::GetRequestHandler()](#cefclientgetrequesthandler)
  - [new 一个CefResourceManager](#new-一个cefresourcemanager)
    - [AddDirectoryProvider](#adddirectoryprovider)
    - [AddArchiveProvider](#addarchiveprovider)
  - [资源提供者BinaryResourceProvider](#资源提供者binaryresourceprovider)
    - [重写OnRequest()处理资源加载逻辑](#重写onrequest处理资源加载逻辑)
      - [命中的处理逻辑CefStreamResourceHandler](#命中的处理逻辑cefstreamresourcehandler)
      - [CefResourceManager::Request](#cefresourcemanagerrequest)
- [scheme handler](#scheme-handler)
  - [OnRegisterCustomSchemes](#onregistercustomschemes)
  - [CefRegisterSchemeHandlerFactory](#cefregisterschemehandlerfactory)
    - [注册时机OnContextInitialized](#注册时机oncontextinitialized)
  - [CefSchemeHandlerFactory](#cefschemehandlerfactory)
    - [CefResourceHandler](#cefresourcehandler)
      - [Open](#open)
      - [ReadResponse](#readresponse)
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
需要实现自己的RequestHandler. 主要重写GetResourceRequestHandler()。
而实现CefResourceRequestHandler，主要是重写GetResourceHandler()和重写OnBeforeResourceLoad()方法。
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

- AddProvider
  - 提供自定义的Provier提供资源。
  - 本项目有实现示例。
- AddDirectoryProvider
  - 把某个url和本地目录对应，访问前缀为该对应的url时，直接从本地目录读取。
  - 本项目有实现示例。
- AddArchiveProvider
  - 映射url和压缩文件。
  - 本项目有实现示例。
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

    //"D:\\srccode\\cefdemos\\schemedemo\\build\\cefsimple\\Debug/index.zip"
    std::string zipFilePath = dirPath + "/index.zip";
    resource_manager->AddArchiveProvider("https://myzipdir.com/",
        zipFilePath,
        "", //password
        201,
        "directoryprovider");
}
```
通过 CefPostTask，将逻辑切到TID_IO线程执行。

官方文档有说明：Provider对象可以放在任意线程new。但方法回调，和对象销毁，都会是在TID_IO线程。
本项目中，全部放在了TID_IO线程执行。

### AddDirectoryProvider
```C++
  void AddDirectoryProvider(const std::string& url_path,
                            const std::string& directory_path,
                            int order,
                            const std::string& identifier);
```
将"https://mytestdir.com/",和本dll所有的目录进行映射。当访问前缀为"https://mytestdir.com/"的url时，直接从本地目录进行读取。


### AddArchiveProvider
将url_path 与zipFilePath进行映射。
```C++
void CefResourceManager::AddArchiveProvider(const std::string& url_path,
                                            const std::string& archive_path,
                                            const std::string& password,
                                            int order,
                                            const std::string& identifier)
```

```C++
    //"D:\\srccode\\cefdemos\\schemedemo\\build\\cefsimple\\Debug/index.zip"
    std::string zipFilePath = dirPath + "/index.zip";
    resource_manager->AddArchiveProvider("https://myzipdir.com/",
        zipFilePath,
        "", //password
        201,
        "directoryprovider");
```

比如访问https://myzipdir.com/映射到index.zip文件，
访问https://myzipdir.com/index.html，会从index.zip压缩包中获取index.html.

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

相关方法和接口类
- CefApp::OnRegisterCustomSchemes
  - 每个进程的CefApp都需要实现
    - browser
    - render
    - other(gpu、utility等)
- CefApp::OnContextInitialized()
  - browser进程
- CefRegisterSchemeHandlerFactory()
- CefSchemeHandlerFactory（需实现）
  - Create
    - 返回CefResourceHandler
    - 每个请求都创建一个CefResourceHandler对象处理该请求
- CefResourceHandler（需实现）
  - Open()
  - GetResponseHeaders()
  - ReadResponse()
  - 处理理单个请求。

## OnRegisterCustomSchemes
要添加对应的scheme，cef每个进程的CefApp都需要实现回调函数
- CefApp::OnRegisterCustomSchemes
如
- browser进程
- render进程
- 其他进程（gpu、utility等）

在实现该方法时，调用
```C++
CefSchemeRegistrar::AddCustomScheme
```
相关常量:
```C++
namespace scheme_handler {

const char kScheme[] = "client";
const char kDomain[] = "tests";
const char kFileName[] = "scheme_test.html";

const int kSchemeRegistrationOptions =
    CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_SECURE |
    CEF_SCHEME_OPTION_CORS_ENABLED | CEF_SCHEME_OPTION_FETCH_ENABLED;

}  // namespace scheme_handler
```
如browser进程的CefApp
```C++
void SimpleApp::OnRegisterCustomSchemes(
    CefRawPtr<CefSchemeRegistrar> registrar) {
	registrar->AddCustomScheme(scheme_handler::kScheme, scheme_handler::kSchemeRegistrationOptions);
}
```

render进程的CefApp

```C++
  void SimpleAppRender::OnRegisterCustomSchemes(
      CefRawPtr<CefSchemeRegistrar> registrar) override {
      // Register the custom scheme as standard and secure.
      // Must be the same implementation in all processes.
      registrar->AddCustomScheme(scheme_handler::kScheme, scheme_handler::kSchemeRegistrationOptions);
  }
```

其中进程的Cefapp
```C++
  void SubprocessApp::OnRegisterCustomSchemes(
      CefRawPtr<CefSchemeRegistrar> registrar) override {
    // Register the custom scheme as standard and secure.
    // Must be the same implementation in all processes.
    registrar->AddCustomScheme(kScheme, kSchemeRegistrationOptions);
  }
```
## CefRegisterSchemeHandlerFactory

### 注册时机OnContextInitialized
在browser进程的CefApp的OnContextInitialized()回调函数中，就调用 CefRegisterSchemeHandlerFactory()

```C++
void SimpleApp::OnContextInitialized() {
  CEF_REQUIRE_UI_THREAD();
  scheme_handler::RegisterSchemeHandlerFactory();
  //...
}
```
实际调用CefRegisterSchemeHandlerFactory

```C++
void RegisterSchemeHandlerFactory() {
  CefRegisterSchemeHandlerFactory(kScheme, kDomain,
                                  new ClientSchemeHandlerFactory());
}
```
## CefSchemeHandlerFactory

主要是实现以下方法。用来提供一个CefResourceHandler。处理对应的scheme的资源请求。
```C++
  virtual CefRefPtr<CefResourceHandler> Create(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      const CefString& scheme_name,
      CefRefPtr<CefRequest> request) = 0;
```

### CefResourceHandler
实现该接口类，用于实际处理该scheme下的资源请求。每个请求，都会由上面的CefSchemeHandlerFactory类创建一个CefResourceHandler对象，用来处理该请求。
- CefResourceHandler（需实现）
  - Open()
    - 为了兼容：当返回false且handle_request=false,则ProcessRequest会被调用。
    - - 该方法运行在ThreadPoolForegroupWorker线程
  - ProcessRequest()已过时
    - 注意官方标注：该方法已过时，请实现Open方法代替。
    - 该方法运行在TID_IO线程
  - GetResponseHeaders()
  - ReadResponse()
    - - 该方法运行在TID_IO线程


#### Open
有对应的scheme的请求时，会调用到该Open函数。比如请求

“client://tests/scheme_test.html”

```C++
  bool Open(CefRefPtr<CefRequest> request,
      bool& handle_request,
      CefRefPtr<CefCallback> callback) {
      handle_request = false;

      std::string url = request->GetURL();
      if (strstr(url.c_str(), kFileName) != nullptr) {
          // Load the response html.
          std::wstring relativePath = L"scheme_test.html";
          if (!readFromFile(relativePath, data_)) {
              return false;
          }
          handle_request = true;
          mime_type_ = "text/html";
      }
      else if (strstr(url.c_str(), "logo.png") != nullptr) {
          // Load the response image.
          std::wstring relativePath = L"logo.png";
          if (!readFromFile(relativePath, data_)) {
              return false;
          }
          handle_request = true;
          mime_type_ = "image/png";
      }

      if (handle_request) {
          // Indicate that the headers are available.
          callback->Continue();
          return true;
      }
      return false;
  }
```

而这里的
```C++
      std::string url = request->GetURL();
```
中的url的值为

“client://tests/scheme_test.html”

这样在这个Open方法里，进行该scheme的资源返回。

本项目中，简单地展示了，如果命中以下文件时
- scheme_test.html
- logo.png

直接从磁盘里读对应文件到缓存。然后示意cef已经找到资源了，请继续执行。
```C++
      if (handle_request) {
          // Indicate that the headers are available.
          callback->Continue();
          return true;
      }
```
#### ReadResponse
当Cef需要读取对应的资源时调用

```C++
  bool ReadResponse(void* data_out,
                    int bytes_to_read,
                    int& bytes_read,
                    CefRefPtr<CefCallback> callback) override {
    CEF_REQUIRE_IO_THREAD();

    bool has_data = false;
    bytes_read = 0;

    if (offset_ < data_.length()) {
      // Copy the next block of data into the buffer.
      int transfer_size =
          std::min(bytes_to_read, static_cast<int>(data_.length() - offset_));
      memcpy(data_out, data_.c_str() + offset_, transfer_size);
      offset_ += transfer_size;

      bytes_read = transfer_size;
      has_data = true;
    }

    return has_data;
  }
```


# 参考
[https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-generic-resource-manager](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-generic-resource-manager)

[https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-scheme-handler](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-scheme-handler)