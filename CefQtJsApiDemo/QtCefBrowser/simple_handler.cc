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

#include "qcefbrowserapp.h"
#include "qcefcoreapp_p.h"
#include <QtCore>
#include "include/base/cef_bind.h"


namespace {

// Returns a data: URI with the specified contents.
std::string GetDataURI(const std::string& data, const std::string& mime_type) {
  return "data:" + mime_type + ";base64," +
         CefURIEncode(CefBase64Encode(data.data(), data.size()), false)
             .ToString();
}

}  // namespace

SimpleHandler::SimpleHandler(std::tr1::shared_ptr<QCefBrowserPrivate> qCefBrowserPrivate):
    m_browerPrivate(qCefBrowserPrivate)
    , m_browser(nullptr)
    , m_closing(false)
    , browserId(0)
    ,use_views_(false){

}

SimpleHandler::~SimpleHandler() {
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
bool SimpleHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& target_url,
    const CefString& target_frame_name,
    WindowOpenDisposition target_disposition,
    bool user_gesture,
    const CefPopupFeatures& popupFeatures,
    CefWindowInfo& windowInfo,
    CefRefPtr<CefClient>& client,
    CefBrowserSettings& settings,
    CefRefPtr<CefDictionaryValue>& extra_info,
    bool* no_javascript_access) {
    CEF_REQUIRE_UI_THREAD();
#ifdef DEBUG_WINDOW_OPEN
    if (!m_closing && m_browser && !windowInfo.parent_window) {
        //若父窗口句柄为空，默认设置弹出窗口的父窗口为自主窗口。
        HWND parentHWND = m_browser->GetHost()->GetWindowHandle();
        windowInfo.parent_window = parentHWND;
    }
    return false; 
#else
    return true; //不允许打开弹出窗口。window.open
#endif 
}
void SimpleHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();
  if (m_browser == nullptr) {
      m_browser = browser;
      browserId = browser->GetIdentifier();
      m_browerPrivate->OnAfterCreated(browser);
  }
  else {
      m_popupBrowsers.push_back(browser);
      m_browerPrivate->OnAfterCreatedPoppup(browser);
  }
  std::set<client::BrowserDelegate*> browserDelegates = qCefCoreAppPrivate()->browserDelegates();
  Q_FOREACH(client::BrowserDelegate * browserDelegate, browserDelegates)
  {
      browserDelegate->OnBrowserCreated(browser);
  }
}

bool SimpleHandler::DoClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();
  if(browserId == browser->GetIdentifier()) {
      m_closing = true;
      if (!m_popupBrowsers.isEmpty()) {
          for (QList<CefRefPtr<CefBrowser>>::const_iterator it = m_popupBrowsers.begin();
              it != m_popupBrowsers.end(); it++) {
              (*it)->GetHost()->CloseBrowser(false);
          }
          m_browerPrivate->OnClosing(browser);
          std::set<client::BrowserDelegate*> browserDelegates = qCefCoreAppPrivate()->browserDelegates();
          Q_FOREACH(client::BrowserDelegate * browserDelegate, browserDelegates)
          {
              browserDelegate->OnBrowserClosing(browser);
          }

          return true; //不关闭。先删除弹出页面。close popupbrowsers first.
      }
      else {
          m_browser = nullptr; //去掉引用计数，以便退出。
          return false;//执行关闭。
      }
  }
  else {
      std::set<client::BrowserDelegate*> browserDelegates = qCefCoreAppPrivate()->browserDelegates();
      Q_FOREACH(client::BrowserDelegate * browserDelegate, browserDelegates)
      {
          browserDelegate->OnBrowserClosing(browser);
      }
  }
  return false;
}

void SimpleHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    std::set<client::BrowserDelegate*> browserDelegates = qCefCoreAppPrivate()->browserDelegates();
    Q_FOREACH(client::BrowserDelegate * browserDelegate, browserDelegates)
    {
        browserDelegate->OnBrowserClosed(browser);
    }
    if (browserId == browser->GetIdentifier()) {
        m_browerPrivate->OnBeforeClose(browser);
        m_browser = nullptr; //去掉引用计数，以便退出
    }
    else {
        bool isLastPopupBrowserDelete = false;
        for (int i = 0; i < m_popupBrowsers.size(); i++) {
            if (m_popupBrowsers.at(i)->GetIdentifier() == browser->GetIdentifier()) {
                m_popupBrowsers.removeAt(i);
                if (m_popupBrowsers.empty()) {
                    isLastPopupBrowserDelete = true;
                }
                break;
            }
        }
        m_browerPrivate->OnBeforeClosePoppup(browser);
        if (m_closing && isLastPopupBrowserDelete) {
            if (m_browser) {
                m_browser->GetHost()->CloseBrowser(false);
            }
        }
    }
  
}

void SimpleHandler::OnLoadStart(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    TransitionType transition_type) {
    CEF_REQUIRE_UI_THREAD();
    if (m_browerPrivate) {
        m_browerPrivate->hookChildWindows();
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

bool SimpleHandler::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {
    CEF_REQUIRE_UI_THREAD();
    std::string message_name = message->GetName();
    std::set<client::BrowserDelegate*> browserDelegates = qCefCoreAppPrivate()->browserDelegates();
    Q_FOREACH(client::BrowserDelegate * browserDelegate, browserDelegates)
    {
        if (browserDelegate->OnProcessMessageReceived(browser, frame, source_process, message))
        {
            return true;
        }
    }

    return false;
}
void SimpleHandler::closeMainBrowser() {
    CefPostTask(TID_UI, base::BindOnce(&SimpleHandler::closeBrowser, this));
}
void SimpleHandler::closeBrowser() {
    CEF_REQUIRE_UI_THREAD();
    if (!CefCurrentlyOn(TID_UI)) {
        return;
    }
    if (m_browser && !m_closing) {
        m_browser->GetHost()->CloseBrowser(false);
    }
}
void SimpleHandler::setSize()
{
    CEF_REQUIRE_UI_THREAD();
    if (!CefCurrentlyOn(TID_UI)) {
        CefPostTask(TID_UI, base::BindOnce(&SimpleHandler::setSize, this));
        return;
    }

    RECT webRect = m_browerPrivate->m_hostWindow->clientRect();
    if (m_browser) {
        HWND cefWindow = (HWND)m_browser->GetHost()->GetWindowHandle();
        SetWindowPos(cefWindow, NULL, webRect.left, webRect.top, webRect.right - webRect.left,
            webRect.bottom - webRect.top, SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

void SimpleHandler::showWindow(bool visible)
{
    if (!CefCurrentlyOn(TID_UI)) {
        CefPostTask(TID_UI, base::BindOnce(&SimpleHandler::showWindow, this, visible));
        return;
    }
    CEF_REQUIRE_UI_THREAD();
    if (m_browerPrivate) {
        m_browerPrivate->showWindow(visible);
    }
}

void SimpleHandler::initWindow()
{
    //CEF_REQUIRE_UI_THREAD();
    if (!CefCurrentlyOn(TID_UI)) {
        CefPostTask(TID_UI, base::BindOnce(&SimpleHandler::initWindow, this));
        return;
    }
    if (m_browerPrivate) {
        m_browerPrivate->createHostWindow();
        m_browerPrivate->createBrowser();
    }
}