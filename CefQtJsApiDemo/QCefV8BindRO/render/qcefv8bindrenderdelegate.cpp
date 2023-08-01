#include "qcefv8bindrenderdelegate.h"
#include "../qcefipcprotocol.h"
#include "../qcefipcvalue.h"
#include "qcefv8handler.h"
#include "../qcefv8objecthelper.h"
#include "../qcefv8bindroapp.h"
#include "../qcefv8bindroapp_p.h"

void QCefV8BindRenderDelegate::OnWebKitInitialized(){
    CefRegisterExtension(CefString(L"qcef/promisecreator"), KPromiseCreatorScript, nullptr);
}
void QCefV8BindRenderDelegate::OnContextCreated( CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
    //每个frame对应一个handler
    CefRefPtr<QCefV8Handler> handler = new QCefV8Handler(frame);
    QCefV8ObjectHelper objectHelper;
    objectHelper.bindGlobalFunctions(context->GetGlobal(), handler);

    // to do , get replicas and bind.
    objectHelper.bindV8ObjectsRO(QCefV8BindAppRO::getInstance()->d_func()->getReplicaTreeHelper()->getObjectsMap(), context, handler);

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
