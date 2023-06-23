
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
    , QObject(nullptr)
    , m_parent(NULL)
    , m_url(url)
    , m_closing(0)
{
    m_uniqueWindowId = ++win_id_generator;

    connect(this, SIGNAL(afterCreated(CefRefPtr<CefBrowser>)), SLOT(OnAfterCreatedSlot(CefRefPtr<CefBrowser>)));
    connect(this, SIGNAL(beforeClose()), SLOT(OnBeforeCloseSlot()));
    connect(this, SIGNAL(closing(CefRefPtr<CefBrowser>)), SLOT(OnClosingSlot(CefRefPtr<CefBrowser>)));
}

QCefBrowserPrivate::~QCefBrowserPrivate()
{
}
void QCefBrowserPrivate::createBrowser() {
    CefRefPtr<SimpleHandler> clientHandler = new SimpleHandler(shared_from_this());

    CefWindowInfo windowInfo;
    windowInfo.SetAsPopup(NULL, "test");
    CefBrowserSettings settings;

    CefString url = m_url.toUtf8().constData();
    CefBrowserHost::CreateBrowser(windowInfo, clientHandler, url, settings, nullptr, nullptr);
}

void QCefBrowserPrivate::closeBrowser()
{
    if (!m_browser || m_closing)
    {
        return;
    }
    m_browser->GetHost()->CloseBrowser(true);
}


void QCefBrowserPrivate::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    emit afterCreated(browser);
}

void QCefBrowserPrivate::OnBeforeClose()
{
    emit beforeClose();
}
void QCefBrowserPrivate::OnClosing(CefRefPtr<CefBrowser> browser)
{
    emit closing(browser);
}

void QCefBrowserPrivate::OnAfterCreatedSlot(CefRefPtr<CefBrowser> browser) {

}
void QCefBrowserPrivate::OnBeforeCloseSlot() {
    qCefCoreAppPrivate()->removeBrowser(q_ptr);
}
void QCefBrowserPrivate::OnClosingSlot(CefRefPtr<CefBrowser> browser) {
    if (browser->IsSame(m_browser))
    {
        InterlockedExchange(&m_closing, 1);
    }
}

