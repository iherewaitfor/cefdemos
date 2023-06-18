#ifndef QCEFRENDERAPP_H
#define QCEFRENDERAPP_H
#pragma once
#include "include/cef_app.h"
#include "include/cef_browser_process_handler.h"
#include <set>
#include "qcefbrowserglobal.h"

// Client app implementation for the browser process.
class QCEFBROWSER_DECL_EXPORT QCefBrowserApp : public CefApp, public CefBrowserProcessHandler {
public:
    // Interface for browser delegates. All Delegates must be returned via
    // CreateDelegates. Do not perform work in the Delegate
    // constructor. See CefBrowserProcessHandler for documentation.
    class Delegate : public virtual CefBaseRefCounted {
    public:
        virtual void OnBeforeCommandLineProcessing(
            CefRefPtr<QCefBrowserApp> app,
            CefRefPtr<CefCommandLine> command_line) {}

        virtual void OnContextInitialized(CefRefPtr<QCefBrowserApp> app) {}

        virtual void OnBeforeChildProcessLaunch(
            CefRefPtr<QCefBrowserApp> app,
            CefRefPtr<CefCommandLine> command_line) {}
    };

    typedef std::set<CefRefPtr<Delegate>> DelegateSet;

    QCefBrowserApp();

private:

    // Creates all of the Delegate objects. Implemented by cefclient in
    // client_app_delegates_browser.cc
    static void CreateDelegates(DelegateSet& delegates);

    // CefApp methods.
    void OnBeforeCommandLineProcessing(
        const CefString& process_type,
        CefRefPtr<CefCommandLine> command_line) override;
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
        return this;
    }

    // CefBrowserProcessHandler methods.
    void OnContextInitialized() override;
    void OnBeforeChildProcessLaunch(
        CefRefPtr<CefCommandLine> command_line) override;

    // Set of supported Delegates.
    DelegateSet delegates_;

    IMPLEMENT_REFCOUNTING(QCefBrowserApp);
    DISALLOW_COPY_AND_ASSIGN(QCefBrowserApp);
};

#endif

