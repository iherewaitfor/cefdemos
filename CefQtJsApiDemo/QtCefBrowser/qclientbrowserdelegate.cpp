#include "qclientbrowserdelegate.h"
QClientBrowserDelegate::QClientBrowserDelegate() {}

void QClientBrowserDelegate::OnContextInitialized(CefRefPtr<QCefBrowserApp> app){
}

void QClientBrowserDelegate::OnBeforeCommandLineProcessing(
    CefRefPtr<QCefBrowserApp> app,
    CefRefPtr<CefCommandLine> command_line) {
}