
#include "qcefbrowser_p.h"
#include "qcefcoreapp.h"
#include "qcefcoreapp_p.h"

#include "include/cef_parser.h"
#include <include/cef_app.h>
#include <include/cef_browser.h>

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

void QCefBrowserPrivate::createBrowser()
{
    CefRefPtr<SimpleHandler> clientHandler = new SimpleHandler(false);
    
    CefWindowInfo windowInfo;
    windowInfo.SetAsPopup(NULL, "test");
    CefBrowserSettings settings;
    CefBrowserHost::CreateBrowser(windowInfo, clientHandler, m_url.toStdWString(), settings, nullptr, nullptr);
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


