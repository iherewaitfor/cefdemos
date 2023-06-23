#include "qcefcoreapp.h"
#include "qcefcoreapp_p.h"
#include "qcefbrowser.h"
QCefCoreApp* g_instance = NULL;
QCefCoreApp::QCefCoreApp(): QObject(nullptr){
    d_ptr = new QCefCoreAppPrivate(this);
    d_ptr->AddRef();
    g_instance = this;

    connect(d_ptr, SIGNAL(allClosed()), this, SIGNAL(allClosed()));

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
QPointer<QCefBrowser> QCefCoreApp::createBrowser(const QString url) {
    QPointer<QCefBrowser> browser = QPointer<QCefBrowser>(new QCefBrowser(url));
    d_ptr->addBrowser(browser);
    return browser;
}
