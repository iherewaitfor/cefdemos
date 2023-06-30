#include "qclientbrowserdelegate.h"
QClientBrowserDelegate::QClientBrowserDelegate() {}

void QClientBrowserDelegate::OnContextInitialized(CefRefPtr<QCefBrowserApp> app){
}

void QClientBrowserDelegate::OnBeforeCommandLineProcessing(
    CefRefPtr<QCefBrowserApp> app,
    CefRefPtr<CefCommandLine> command_line) {
    if (!command_line->HasSwitch("renderer-process-limit")) {
        command_line->AppendSwitchWithValue("renderer-process-limit", "1");
    }
}