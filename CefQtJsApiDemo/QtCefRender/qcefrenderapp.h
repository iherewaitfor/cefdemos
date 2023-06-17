#pragma once

class QCefRenderApp : public CefApp
{
public:
    QCefRenderApp();
    // CefApp methods:
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()
        OVERRIDE{ return renderProcessHandler_; }

private:
    CefRefPtr<QCefRenderProcessHandler> renderProcessHandler_;
    IMPLEMENT_REFCOUNTING(QCefRenderApp);
};  

