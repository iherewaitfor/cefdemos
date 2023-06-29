#include "qcefrenderapp.h"
#include "../qcefcoreapp_p.h"
#include "../qcefcoreapp.h"
#include "qcefrenderhandlerdelegate.h"

QCefRenderApp::QCefRenderApp(){
    
}

void QCefRenderApp::OnWebKitInitialized() {
    std::set<client::RenderDelegate*> renderDelegates = qCefCoreAppPrivate()->renderDelegates();
    Q_FOREACH(client::RenderDelegate * renderDelegate, renderDelegates)
    {
        renderDelegate->OnWebKitInitialized();
    }
}

void QCefRenderApp::OnBrowserCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDictionaryValue> extra_info) {
    std::set<client::RenderDelegate*> renderDelegates = qCefCoreAppPrivate()->renderDelegates();
    Q_FOREACH(client::RenderDelegate * renderDelegate, renderDelegates)
    {
        renderDelegate->OnBrowserCreated(browser, extra_info);
    }
}

void QCefRenderApp::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) {
    
}

void QCefRenderApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
    std::set<client::RenderDelegate*> renderDelegates = qCefCoreAppPrivate()->renderDelegates();
    Q_FOREACH(client::RenderDelegate * renderDelegate, renderDelegates)
    {
        renderDelegate->OnContextCreated(browser, frame, context);
    }
}

void QCefRenderApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
}

void QCefRenderApp::OnUncaughtException(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context,
    CefRefPtr<CefV8Exception> exception,
    CefRefPtr<CefV8StackTrace> stackTrace) {
}

void QCefRenderApp::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefDOMNode> node) {
}

bool QCefRenderApp::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {
    DCHECK_EQ(source_process, PID_BROWSER);
    std::set<client::RenderDelegate*> renderDelegates = qCefCoreAppPrivate()->renderDelegates();
    Q_FOREACH(client::RenderDelegate * renderDelegate, renderDelegates)
    {
        if (renderDelegate->OnProcessMessageReceived(browser, frame,
            source_process, message)) {
            return true;
        }
    }
    return false;
}
