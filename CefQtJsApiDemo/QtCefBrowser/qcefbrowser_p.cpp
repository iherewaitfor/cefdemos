
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
    , m_browserId(0)
{
    m_Id = ++win_id_generator;

    connect(this, SIGNAL(afterCreated(int)), SLOT(OnAfterCreatedSlot(int)));
    connect(this, SIGNAL(beforeClose(int)), SLOT(OnBeforeCloseSlot(int)));
    connect(this, SIGNAL(closing(int)), SLOT(OnClosingSlot(int)));

    connect(this, SIGNAL(afterCreatedPoppup(int)), SLOT(afterCreatedPoppupSlot(int)));
    connect(this, SIGNAL(beforeClosePoppup(int)), SLOT(beforeClosePoppupSlot(int)));
}

QCefBrowserPrivate::~QCefBrowserPrivate()
{
}
void QCefBrowserPrivate::createBrowser() {
    CefRefPtr<SimpleHandler> clientHandler = new SimpleHandler(shared_from_this());
    m_clientHandler = clientHandler;

    CefWindowInfo windowInfo;
    windowInfo.SetAsPopup(NULL, "test");
    CefBrowserSettings settings;

    CefString url = m_url.toUtf8().constData();
    CefBrowserHost::CreateBrowser(windowInfo, clientHandler, url, settings, nullptr, nullptr);
}

void QCefBrowserPrivate::closeBrowser()
{
    if (m_clientHandler) {
        m_clientHandler->closeMainBrowser();
    }
}


void QCefBrowserPrivate::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    m_browserId = browser->GetIdentifier();
    emit afterCreated(m_browserId);
}

void QCefBrowserPrivate::OnClosing(CefRefPtr<CefBrowser> browser)
{
    InterlockedExchange(&m_closing, 1);
    emit closing(browser->GetIdentifier());
}

void QCefBrowserPrivate::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    emit beforeClose(browser->GetIdentifier());
}

void QCefBrowserPrivate::OnAfterCreatedSlot(int browserId) {

}

void QCefBrowserPrivate::OnClosingSlot(int browserId) {

}

void QCefBrowserPrivate::OnBeforeCloseSlot(int browserId) {
    
    qCefCoreAppPrivate()->removeBrowser(m_Id);
}

void QCefBrowserPrivate::OnAfterCreatedPoppup(CefRefPtr<CefBrowser> browser) {
    emit afterCreatedPoppup(browser->GetIdentifier());
}

void QCefBrowserPrivate::OnBeforeClosePoppup(CefRefPtr<CefBrowser> browser) {
    emit beforeClosePoppup(browser->GetIdentifier());
}

void QCefBrowserPrivate::afterCreatedPoppupSlot(int browserId) {
    qCefCoreAppPrivate()->addPopupBrowser(browserId);
}

void QCefBrowserPrivate::beforeClosePoppupSlot(int browserId) {

    qCefCoreAppPrivate()->removePopupBrowser(browserId);
}