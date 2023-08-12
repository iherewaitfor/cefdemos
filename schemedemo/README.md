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




# scheme handler


# 参考
[https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-generic-resource-manager](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-generic-resource-manager)

[https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-scheme-handler](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-scheme-handler)