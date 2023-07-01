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
QSharedPointer<QCefBrowser> QCefCoreApp::createBrowser(const QString url) {
    if (d_ptr->m_bClosing) {
        return QSharedPointer<QCefBrowser>();
    }
    QSharedPointer<QCefBrowser> browser = QSharedPointer<QCefBrowser>(new QCefBrowser(url));
    d_ptr->addBrowser(browser->getId(), browser);
    return browser;
}
void QCefCoreApp::setApiRoot(QPointer<QObject> qApiRootObject) {
    d_ptr->setApiRoot(qApiRootObject);
}
QPointer<QObject> QCefCoreApp::getApitRootObject() {
    return d_ptr->getApitRootObject();
}
void QCefCoreApp::regBrowserDelegate(client::BrowserDelegate* delegate) {
    d_ptr->m_browserDelegates.insert(delegate);
}
void QCefCoreApp::unregBrowserDelegate(client::BrowserDelegate* delegate) {
    d_ptr->m_browserDelegates.erase(delegate);
}
void QCefCoreApp::regRenderDelegate(client::RenderDelegate* delegate) {
    d_ptr->m_renderDelegates.insert(delegate);
}
void QCefCoreApp::unregRenderDelegate(client::RenderDelegate* delegate) {
    d_ptr->m_renderDelegates.erase(delegate);
}
