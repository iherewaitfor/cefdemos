#include "qcefrenderapp.h"
#include "qclientrenderdelegate.h"
void QCefRenderApp::CreateDelegates(DelegateSet& delegates) {
    delegates.insert(new QClientRenderDelegate);
}

QCefRenderApp::QCefRenderApp(){
    CreateDelegates(delegates_);
}

void QCefRenderApp::OnWebKitInitialized() {
    DelegateSet::iterator it = delegates_.begin();
    for (; it != delegates_.end(); ++it)
        (*it)->OnWebKitInitialized(this);
}

void QCefRenderApp::OnBrowserCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDictionaryValue> extra_info) {
    DelegateSet::iterator it = delegates_.begin();
    for (; it != delegates_.end(); ++it)
        (*it)->OnBrowserCreated(this, browser, extra_info);
}

void QCefRenderApp::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) {
    DelegateSet::iterator it = delegates_.begin();
    for (; it != delegates_.end(); ++it)
        (*it)->OnBrowserDestroyed(this, browser);
}

CefRefPtr<CefLoadHandler> QCefRenderApp::GetLoadHandler() {
    CefRefPtr<CefLoadHandler> load_handler;
    DelegateSet::iterator it = delegates_.begin();
    for (; it != delegates_.end() && !load_handler.get(); ++it)
        load_handler = (*it)->GetLoadHandler(this);

    return load_handler;
}

void QCefRenderApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
    DelegateSet::iterator it = delegates_.begin();
    for (; it != delegates_.end(); ++it)
        (*it)->OnContextCreated(this, browser, frame, context);
}

void QCefRenderApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
    DelegateSet::iterator it = delegates_.begin();
    for (; it != delegates_.end(); ++it)
        (*it)->OnContextReleased(this, browser, frame, context);
}

void QCefRenderApp::OnUncaughtException(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context,
    CefRefPtr<CefV8Exception> exception,
    CefRefPtr<CefV8StackTrace> stackTrace) {
    DelegateSet::iterator it = delegates_.begin();
    for (; it != delegates_.end(); ++it) {
        (*it)->OnUncaughtException(this, browser, frame, context, exception,
            stackTrace);
    }
}

void QCefRenderApp::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefDOMNode> node) {
    DelegateSet::iterator it = delegates_.begin();
    for (; it != delegates_.end(); ++it)
        (*it)->OnFocusedNodeChanged(this, browser, frame, node);
}

bool QCefRenderApp::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {
    DCHECK_EQ(source_process, PID_BROWSER);

    bool handled = false;

    DelegateSet::iterator it = delegates_.begin();
    for (; it != delegates_.end() && !handled; ++it) {
        handled = (*it)->OnProcessMessageReceived(this, browser, frame,
            source_process, message);
    }

    return handled;
}
