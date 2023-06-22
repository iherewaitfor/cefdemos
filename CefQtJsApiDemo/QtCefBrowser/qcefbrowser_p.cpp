
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
    CefRefPtr<SimpleHandler> clientHandler = new SimpleHandler();

    CefWindowInfo windowInfo;
    windowInfo.SetAsPopup(NULL, "test");
    CefBrowserSettings settings;

    CefString url = m_url.toUtf8().constData();
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
    qCefCoreAppPrivate()->addBrowser(QPointer<QCefBrowser>(q_ptr));
}

void QCefBrowserPrivate::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{

}
void QCefBrowserPrivate::OnClosing(CefRefPtr<CefBrowser> browser)
{
    if (browser->IsSame(m_browser))
    {
        m_isClosing = true;
    }
}


