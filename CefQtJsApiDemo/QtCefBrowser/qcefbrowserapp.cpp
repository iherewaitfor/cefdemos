#include "qcefbrowserapp.h"
#include "qclientbrowserdelegate.h"
QCefBrowserApp::QCefBrowserApp() {
    CreateDelegates(delegates_);
}
void QCefBrowserApp::CreateDelegates(DelegateSet& delegates) {
    delegates.insert(new QClientBrowserDelegate());
}

void QCefBrowserApp::OnBeforeCommandLineProcessing(
    const CefString& process_type,
    CefRefPtr<CefCommandLine> command_line) {
    // Pass additional command-line flags to the browser process.
    if (process_type.empty()) {
        DelegateSet::iterator it = delegates_.begin();
        for (; it != delegates_.end(); ++it)
            (*it)->OnBeforeCommandLineProcessing(this, command_line);
    }
}

void QCefBrowserApp::OnContextInitialized() {
    DelegateSet::iterator it = delegates_.begin();
    for (; it != delegates_.end(); ++it)
        (*it)->OnContextInitialized(this);
}

void QCefBrowserApp::OnBeforeChildProcessLaunch(
    CefRefPtr<CefCommandLine> command_line) {
    DelegateSet::iterator it = delegates_.begin();
    for (; it != delegates_.end(); ++it)
        (*it)->OnBeforeChildProcessLaunch(this, command_line);
}
