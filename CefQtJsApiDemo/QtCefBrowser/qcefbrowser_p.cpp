
#include "qcefbrowser_p.h"
#include "qcefcoreapp.h"
#include "qcefcoreapp_p.h"

#include "include/cef_parser.h"
#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <QtCore>

static int win_id_generator = 0;
QCefBrowserPrivate::QCefBrowserPrivate(QCefBrowser* q, QString url)
    : q_ptr(q)
    , m_parent(NULL)
    , m_url(url)
{
    m_uniqueWindowId = ++win_id_generator;

}

QCefBrowserPrivate::~QCefBrowserPrivate()
{
}
void QCefBrowserPrivate::createBrowser() {
    QMetaObject::invokeMethod(this, "createBroserInVoke", Qt::QueuedConnection);
}
#include "include/wrapper/cef_helpers.h"
void QCefBrowserPrivate::createBroserInVoke()
{
    CefRefPtr<SimpleHandler> clientHandler = new SimpleHandler(false);
    
    CefWindowInfo windowInfo;
    windowInfo.SetAsPopup(NULL, "test");
    CefBrowserSettings settings;
    std::string str2 = "https://www.baidu.com";
    CefString url = str2;
    CefBrowserHost::CreateBrowser(windowInfo, clientHandler, url, settings, nullptr, nullptr);
}

void QCefBrowserPrivate::closeBroser()
{
    if (!m_browser || m_isClosing)
    {
        return;
    }
    m_browser->GetHost()->CloseBrowser(true);
}


void QCefBrowserPrivate::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    m_browser = browser;
    //if (!m_url.isEmpty())
    //{
    //    browser->GetMainFrame()->LoadURL(m_url.toStdWString());
    //}
}

void QCefBrowserPrivate::OnClosing(CefRefPtr<CefBrowser> browser)
{
    if (browser->IsSame(m_browser))
    {
        m_isClosing = true;
    }
}


