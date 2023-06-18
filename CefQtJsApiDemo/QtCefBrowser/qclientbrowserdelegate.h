#ifndef QCLIENTBROWSERDELEGATE_H
#define QCLIENTBROWSERDELEGATE_H
#include "qcefbrowserapp.h"

class QClientBrowserDelegate : public QCefBrowserApp::Delegate {
public:
    QClientBrowserDelegate();

    void OnContextInitialized(CefRefPtr<QCefBrowserApp> app) override;

    void OnBeforeCommandLineProcessing(
        CefRefPtr<QCefBrowserApp> app,
        CefRefPtr<CefCommandLine> command_line) override;

private:
    DISALLOW_COPY_AND_ASSIGN(QClientBrowserDelegate);
    IMPLEMENT_REFCOUNTING(QClientBrowserDelegate);
};
#endif