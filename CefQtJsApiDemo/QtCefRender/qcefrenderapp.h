#ifndef QCEFRENDERAPP_H
#define QCEFRENDERAPP_H
#include "include/cef_app.h"
#include <set>

class QCefRenderApp : public CefApp,public CefRenderProcessHandler
{
public:
    // Interface for renderer delegates. All Delegates must be returned via
     // CreateDelegates. Do not perform work in the Delegate
     // constructor. See CefRenderProcessHandler for documentation.
    class Delegate : public virtual CefBaseRefCounted {
    public:
        virtual void OnWebKitInitialized(CefRefPtr<QCefRenderApp> app) {}

        virtual void OnBrowserCreated(CefRefPtr<QCefRenderApp> app,
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefDictionaryValue> extra_info) {}

        virtual void OnBrowserDestroyed(CefRefPtr<QCefRenderApp> app,
            CefRefPtr<CefBrowser> browser) {}

        virtual CefRefPtr<CefLoadHandler> GetLoadHandler(
            CefRefPtr<QCefRenderApp> app) {
            return nullptr;
        }

        virtual void OnContextCreated(CefRefPtr<QCefRenderApp> app,
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefRefPtr<CefV8Context> context) {}

        virtual void OnContextReleased(CefRefPtr<QCefRenderApp> app,
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefRefPtr<CefV8Context> context) {}

        virtual void OnUncaughtException(CefRefPtr<QCefRenderApp> app,
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefRefPtr<CefV8Context> context,
            CefRefPtr<CefV8Exception> exception,
            CefRefPtr<CefV8StackTrace> stackTrace) {}

        virtual void OnFocusedNodeChanged(CefRefPtr<QCefRenderApp> app,
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefRefPtr<CefDOMNode> node) {}

        // Called when a process message is received. Return true if the message was
        // handled and should not be passed on to other handlers. Delegates
        // should check for unique message names to avoid interfering with each
        // other.
        virtual bool OnProcessMessageReceived(
            CefRefPtr<QCefRenderApp> app,
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefProcessId source_process,
            CefRefPtr<CefProcessMessage> message) {
            return false;
        }
    };


    typedef std::set<CefRefPtr<Delegate>> DelegateSet;

    QCefRenderApp();

private:
    // Creates all of the Delegate objects. Implemented by cefclient in
    // client_app_delegates_renderer.cc
    static void CreateDelegates(DelegateSet& delegates);

    // CefApp methods.
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
        return this;
    }

    // CefRenderProcessHandler methods.
    void OnWebKitInitialized() override;
    void OnBrowserCreated(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefDictionaryValue> extra_info) override;
    void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) override;
    CefRefPtr<CefLoadHandler> GetLoadHandler() override;
    void OnContextCreated(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context) override;
    void OnContextReleased(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context) override;
    void OnUncaughtException(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context,
        CefRefPtr<CefV8Exception> exception,
        CefRefPtr<CefV8StackTrace> stackTrace) override;
    void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefDOMNode> node) override;
    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) override;

private:
    // Set of supported Delegates.
    DelegateSet delegates_;

    IMPLEMENT_REFCOUNTING(QCefRenderApp);
    DISALLOW_COPY_AND_ASSIGN(QCefRenderApp);
    
};  
#endif

