#include "qcefv8bindrenderdelegate.h"
#include "../qcefipcprotocol.h"
#include "../qcefipcvalue.h"
#include "qcefv8handler.h"
#include "../qcefv8objecthelper.h"
QString getBrwoserFrameId(int64 browserId, int64 frameId) {
    return QString("%1_%2").arg(browserId).arg(frameId);
}
void QCefV8BindRenderDelegate::OnWebKitInitialized(){
    CefRegisterExtension(CefString(L"qcef/promisecreator"), KPromiseCreatorScript, nullptr);
}
void QCefV8BindRenderDelegate::OnContextCreated( CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
    cefv8bind_protcool::CefApiMetaDatasReq ipc_protocol;
    ipc_protocol.render_pid = ::GetCurrentProcessId();
    frame->SendProcessMessage(PID_BROWSER, ipc_protocol.makeIPCMessage());

    CefRefPtr<QCefV8Handler> handler = new QCefV8Handler(frame);
    QCefV8ObjectHelper objectHelper;
    objectHelper.bindGlobalFunctions(context->GetGlobal(), handler);

    m_frameHandlers.insert(frame->GetIdentifier(), handler);
}

void QCefV8BindRenderDelegate::OnContextReleased(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
    int64 frameId = frame->GetIdentifier();
    if (m_frameHandlers.contains(frameId))
    {
        m_frameHandlers[frameId]->clear();
        m_frameHandlers.remove(frameId);
    }
}

bool QCefV8BindRenderDelegate::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {

    if (message->GetName() == cefv8bind_protcool::CefApiMetaDatasResponse::message_name())
    {
        int64 frameId = frame->GetIdentifier();
        if (m_frameHandlers.contains(frameId)) {
            CefRefPtr<QCefV8Handler> handler = m_frameHandlers.value(frameId);
            if (handler) {
                handler->on_cefMetaDatasResponse(browser, frame, message);
            }
        }
        return true;
    }
    else if (message->GetName() == cefv8bind_protcool::InvokeResp::message_name()) 
    {
        int64 frameId = frame->GetIdentifier();
        if (m_frameHandlers.contains(frameId)) {
            CefRefPtr<QCefV8Handler> handler = m_frameHandlers.value(frameId);
            if (handler) {
                handler->onInvokeResponse(message,frame->GetV8Context());
            }
        }
        return true;
    }
    else if (message->GetName() == cefv8bind_protcool::EmitSignalMsg::message_name())
    {
        int64 frameId = frame->GetIdentifier();
        if (m_frameHandlers.contains(frameId)) {
            CefRefPtr<QCefV8Handler> handler = m_frameHandlers.value(frameId);
            if (handler) {
                handler->onEmitSignalMsg(message);
            }
        }
        return true;
    }
    return false;
}