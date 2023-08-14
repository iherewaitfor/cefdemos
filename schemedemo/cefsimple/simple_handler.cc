// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "simple_handler.h"

#include <sstream>
#include <string>

#include "include/base/cef_callback.h"
#include "include/cef_app.h"
#include "include/cef_parser.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#include "include/wrapper/cef_stream_resource_handler.h"

namespace {

SimpleHandler* g_instance = nullptr;

// Returns a data: URI with the specified contents.
std::string GetDataURI(const std::string& data, const std::string& mime_type) {
  return "data:" + mime_type + ";base64," +
         CefURIEncode(CefBase64Encode(data.data(), data.size()), false)
             .ToString();
}

}  // namespace

////////////////////////////////////////////////////////////

const std::string& test_host = "https://myjstest.com/";

//utility method.
std::string getCurentDllPath() {
    std::string strPath;
    CHAR path[MAX_PATH];
    ::GetModuleFileNameA(NULL, path, MAX_PATH);
    strPath = path;

    size_t index = strPath.rfind("\\");
    strPath = strPath.substr(0, index);
    return strPath;
}

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

// Add example Providers to the CefResourceManager.
void SetupResourceManager(CefRefPtr<CefResourceManager> resource_manager) {
    if (!CefCurrentlyOn(TID_IO)) {
        // Execute on the browser IO thread.
        CefPostTask(TID_IO, base::BindOnce(SetupResourceManager, resource_manager));
        return;
    }
    resource_manager->AddProvider(
        new BinaryResourceProvider(test_host), 100, std::string());

    //"D:\\srccode\\cefdemos\\schemedemo\\build\\cefsimple\\Debug"
    std::string dirPath = getCurentDllPath();
    resource_manager->AddDirectoryProvider("https://mytestdir.com/",
        dirPath,
        201,
        "directoryprovider");

    //"D:\\srccode\\cefdemos\\schemedemo\\build\\cefsimple\\Debug/index.zip"
    std::string zipFilePath = dirPath + "/index.zip";
    resource_manager->AddArchiveProvider("https://myzipdir.com/",
        zipFilePath,
        "", //password
        202,
        "archiveprovider");

}
////////////////////////////////////////////////////////////


SimpleHandler::SimpleHandler(bool use_views)
    : use_views_(use_views), is_closing_(false) {
  DCHECK(!g_instance);
  g_instance = this;
  resource_manager_ = new CefResourceManager();
  SetupResourceManager(resource_manager_);
}

SimpleHandler::~SimpleHandler() {
  g_instance = nullptr;
}

// static
SimpleHandler* SimpleHandler::GetInstance() {
  return g_instance;
}

void SimpleHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                  const CefString& title) {
  CEF_REQUIRE_UI_THREAD();

  if (use_views_) {
    // Set the title of the window using the Views framework.
    CefRefPtr<CefBrowserView> browser_view =
        CefBrowserView::GetForBrowser(browser);
    if (browser_view) {
      CefRefPtr<CefWindow> window = browser_view->GetWindow();
      if (window)
        window->SetTitle(title);
    }
  } else if (!IsChromeRuntimeEnabled()) {
    // Set the title of the window using platform APIs.
    PlatformTitleChange(browser, title);
  }
}

void SimpleHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  // Add to the list of existing browsers.
  browser_list_.push_back(browser);
}

bool SimpleHandler::DoClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  // Closing the main window requires special handling. See the DoClose()
  // documentation in the CEF header for a detailed destription of this
  // process.
  if (browser_list_.size() == 1) {
    // Set a flag to indicate that the window close should be allowed.
    is_closing_ = true;
  }

  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent.
  return false;
}

void SimpleHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  // Remove from the list of existing browsers.
  BrowserList::iterator bit = browser_list_.begin();
  for (; bit != browser_list_.end(); ++bit) {
    if ((*bit)->IsSame(browser)) {
      browser_list_.erase(bit);
      break;
    }
  }

  if (browser_list_.empty()) {
    // All browser windows have closed. Quit the application message loop.
    CefQuitMessageLoop();
  }
}

void SimpleHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                ErrorCode errorCode,
                                const CefString& errorText,
                                const CefString& failedUrl) {
  CEF_REQUIRE_UI_THREAD();

  // Allow Chrome to show the error page.
  if (IsChromeRuntimeEnabled())
    return;

  // Don't display an error for downloaded files.
  if (errorCode == ERR_ABORTED)
    return;

  // Display a load error message using a data: URI.
  std::stringstream ss;
  ss << "<html><body bgcolor=\"white\">"
        "<h2>Failed to load URL "
     << std::string(failedUrl) << " with error " << std::string(errorText)
     << " (" << errorCode << ").</h2></body></html>";

  frame->LoadURL(GetDataURI(ss.str(), "text/html"));
}

void SimpleHandler::CloseAllBrowsers(bool force_close) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    CefPostTask(TID_UI, base::BindOnce(&SimpleHandler::CloseAllBrowsers, this,
                                       force_close));
    return;
  }

  if (browser_list_.empty())
    return;

  BrowserList::const_iterator it = browser_list_.begin();
  for (; it != browser_list_.end(); ++it)
    (*it)->GetHost()->CloseBrowser(force_close);
}


CefRefPtr<CefResourceRequestHandler> SimpleHandler::GetResourceRequestHandler(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    bool is_navigation,
    bool is_download,
    const CefString& request_initiator,
    bool& disable_default_handling) {
    CEF_REQUIRE_IO_THREAD();
    return this;
}

cef_return_value_t SimpleHandler::OnBeforeResourceLoad(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefCallback> callback) {
    CEF_REQUIRE_IO_THREAD();

    return resource_manager_->OnBeforeResourceLoad(browser, frame, request,
        callback);
}

CefRefPtr<CefResourceHandler> SimpleHandler::GetResourceHandler(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request) {
    CEF_REQUIRE_IO_THREAD();

    return resource_manager_->GetResourceHandler(browser, frame, request);
}



// static
bool SimpleHandler::IsChromeRuntimeEnabled() {
  static int value = -1;
  if (value == -1) {
    CefRefPtr<CefCommandLine> command_line =
        CefCommandLine::GetGlobalCommandLine();
    value = command_line->HasSwitch("enable-chrome-runtime") ? 1 : 0;
  }
  return value == 1;
}
