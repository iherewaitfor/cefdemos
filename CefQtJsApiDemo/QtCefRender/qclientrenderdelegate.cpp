#include "qclientrenderdelegate.h"
#include "../QtCefBrowser/qcefipcprotocol.h"
#include "../QtCefBrowser/qcefipcvalue.h"
QString getBrwoserFrameId(int64 browserId, int64 frameId) {
    return QString("%1_%2").arg(browserId).arg(frameId);
}
void QClientRenderDelegate::OnContextCreated(CefRefPtr<QCefRenderApp> app,
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
    cefv8bind_protcool::CefApiMetaDatasReq ipc_protocol;
    ipc_protocol.render_pid = ::GetCurrentProcessId();
    frame->SendProcessMessage(PID_BROWSER, ipc_protocol.makeIPCMessage());

    CefRefPtr<QCefV8Handler> handler = new QCefV8Handler(frame);
    m_frameHandlers.insert(getBrwoserFrameId(browser->GetIdentifier(),frame->GetIdentifier()), handler);
}

void QClientRenderDelegate::OnContextReleased(CefRefPtr<QCefRenderApp> app,
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
    QString bfId = getBrwoserFrameId(browser->GetIdentifier(), frame->GetIdentifier());
    if (m_frameHandlers.contains(bfId))
    {
        m_frameHandlers[bfId]->clear();
        m_frameHandlers.remove(bfId);
    }
}

bool QClientRenderDelegate::OnProcessMessageReceived(CefRefPtr<QCefRenderApp> app,
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {

    if (message->GetName() == cefv8bind_protcool::CefApiMetaDatasResponse::message_name())
    {
        QString bfId = getBrwoserFrameId(browser->GetIdentifier(), frame->GetIdentifier());
        if (m_frameHandlers.contains(bfId)) {
            CefRefPtr<QCefV8Handler> handler = m_frameHandlers.value(bfId);
            if (handler) {
                handler->on_cefMetaDatasResponse(browser, frame, message);
            }
        }
        return true;
    }
    return false;
}