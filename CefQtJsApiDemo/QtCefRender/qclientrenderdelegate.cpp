#include "qclientrenderdelegate.h"
#include "../QtCefBrowser/qcefipcprotocol.h"
#include "../QtCefBrowser/qcefipcvalue.h"
void QClientRenderDelegate::OnContextCreated(CefRefPtr<QCefRenderApp> app,
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
    cefv8bind_protcool::CefApiMetaDatasReq ipc_protocol;
    ipc_protocol.render_pid = ::GetCurrentProcessId();
    frame->SendProcessMessage(PID_BROWSER, ipc_protocol.makeIPCMessage());
}

bool QClientRenderDelegate::OnProcessMessageReceived(CefRefPtr<QCefRenderApp> app,
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {

    return false;
}