#include "qcefcoreapp.h"
#include "qcefcoreapp_p.h"
#include "qcefbrowser.h"
QCefCoreApp* g_instance = NULL;
QCefCoreApp::QCefCoreApp() {
    d_ptr = new QCefCoreAppPrivate(this);
    d_ptr->AddRef();
    g_instance = this;

}
QCefCoreApp::~QCefCoreApp() {
    d_ptr->Release();
    g_instance = NULL;
}

QCefCoreApp* QCefCoreApp::getInstance()
{
    static QCefCoreApp app;
    return g_instance;
}
QSharedPointer<QCefBrowser> QCefCoreApp::createBrowser(const QString url) {
    QSharedPointer<QCefBrowser> browser = QSharedPointer<QCefBrowser>(new QCefBrowser(url));
    d_ptr->addPopupBrowser(browser);

    return browser;

}