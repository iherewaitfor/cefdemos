#ifndef QCLIENTRENDERDELEGATE_H
#define QCLIENTRENDERDELEGATE_H
#include "qcefrenderapp.h"
#include "qcefv8handler.h"
#include <QtCore>
class QCefV8Handler;
class QClientRenderDelegate : public QCefRenderApp::Delegate {
public:
    QClientRenderDelegate() {}

    void OnWebKitInitialized(CefRefPtr<QCefRenderApp> app) override {
    }

    void OnContextCreated(CefRefPtr<QCefRenderApp> app,
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context) override;

    void OnContextReleased(CefRefPtr<QCefRenderApp> app,
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context) override;

    void OnFocusedNodeChanged(CefRefPtr<QCefRenderApp> app,
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefDOMNode> node) override {

    }

    bool OnProcessMessageReceived(CefRefPtr<QCefRenderApp> app,
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) override;

private:

    QMap<QString, CefRefPtr<QCefV8Handler>> m_frameHandlers;

    DISALLOW_COPY_AND_ASSIGN(QClientRenderDelegate);
    IMPLEMENT_REFCOUNTING(QClientRenderDelegate);
};
#endif