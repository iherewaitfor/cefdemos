
#include "qcefbrowser_p.h"
#include "qcefcoreapp.h"
#include "qcefcoreapp_p.h"

#include "include/cef_parser.h"
#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <QtCore>

#include "qcefipcprotocol.h"
#include "qcefv8objecthelper.h"

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

    connect(this, SIGNAL(afterCreatedPoppup(CefRefPtr<CefBrowser>)), SLOT(afterCreatedPoppupSlot(CefRefPtr<CefBrowser>)));
    connect(this, SIGNAL(beforeClosePoppup(CefRefPtr<CefBrowser>)), SLOT(beforeClosePoppupSlot(CefRefPtr<CefBrowser>)));
    connect(this, SIGNAL(cefMetaReq(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>)),
        SLOT(onCefMetaReqSlot(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>)));

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
    m_browser = browser;
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
    m_browser = nullptr;
    qCefCoreAppPrivate()->removeBrowser(q_ptr);
}
void QCefBrowserPrivate::OnClosingSlot(CefRefPtr<CefBrowser> browser) {
    InterlockedExchange(&m_closing, 1);
}


void QCefBrowserPrivate::OnAfterCreatedPoppup(CefRefPtr<CefBrowser> browser) {
    emit afterCreatedPoppup(browser);
}
void QCefBrowserPrivate::OnBeforeClosePoppup(CefRefPtr<CefBrowser> browser) {
    emit beforeClosePoppup(browser);
}
void QCefBrowserPrivate::afterCreatedPoppupSlot(CefRefPtr<CefBrowser> browser) {
    qCefCoreAppPrivate()->addPopupBrowser(browser);
}
void QCefBrowserPrivate::beforeClosePoppupSlot(CefRefPtr<CefBrowser> browser) {
    qCefCoreAppPrivate()->removePopupBrowser(browser);
}

void QCefBrowserPrivate::onCefMetaReq(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) {
    emit cefMetaReq(browser, frame);
}

void QCefBrowserPrivate::onCefMetaReqSlot(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) {
    QList<cefv8bind_protcool::CefMetaObject>  cef_metaObjects;
    QCefV8ObjectHelper objectHelper;
    objectHelper.convertQObjectToCefObjects(qCefCoreAppPrivate()->getApitRootObject(), nullptr, cef_metaObjects);
    cefv8bind_protcool::CefApiMetaDatasResponse response;
    response.cef_metaObjects = cef_metaObjects;
    frame->SendProcessMessage(PID_RENDERER, response.makeIPCMessage());
}