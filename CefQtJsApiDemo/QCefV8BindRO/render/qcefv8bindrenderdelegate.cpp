#include "qcefv8bindrenderdelegate.h"
#include "../qcefipcprotocol.h"
#include "../qcefipcvalue.h"
#include "qcefv8handler.h"
#include "../qcefv8objecthelper.h"
#include "../qcefv8bindroapp.h"
#include "../qcefv8bindroapp_p.h"
#include "../qcefv8bindutility.h"

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

    CefRefPtr<CefV8Context> frameContext = frame->GetV8Context();
    bool isSame = frameContext->IsSame(context);
    isSame;

    // to do , get replicas and bind.
    //objectHelper.bindV8ObjectsRO(QCefV8BindAppRO::getInstance()->d_func()->getReplicaTreeHelper()->getObjectsMap(), context, handler);

    m_frameHandlers.insert(frame->GetIdentifier(), handler);
    //emit signal to bind.
    QCefV8BindAppRO::getInstance()->d_func()->getReplicaTreeHelper()->contextCreated(browser->GetIdentifier(), frame->GetIdentifier());
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

void QCefV8BindRenderDelegate::tobindV8Objects(QList<cefv8bind_protcool::CefMetaObject> cefMetaObjects, int64 frameId) {
    if (!CefCurrentlyOn(TID_RENDERER)) {
        CefPostTask(TID_RENDERER, base::BindOnce(&QCefV8BindRenderDelegate::tobindV8Objects, this, cefMetaObjects, frameId));
        return;
    }

    CEF_REQUIRE_RENDERER_THREAD();
    if (!m_frameHandlers.contains(frameId)) {
        return;
    }
    CefRefPtr<QCefV8Handler> v8Handler = m_frameHandlers.value(frameId);
    QCefV8ObjectHelper objectHelper;

    CefRefPtr<CefV8Context> context = v8Handler->getFrame()->GetV8Context();
    V8ContextCaller auto_caller(context);
    objectHelper.bindV8Objects(cefMetaObjects, context, v8Handler);
}

void QCefV8BindRenderDelegate::onPendingcallResp(cefv8bind_protcool::PendingcallResp rsp, int64 frameId) {
    if (!CefCurrentlyOn(TID_RENDERER)) {
        CefPostTask(TID_RENDERER, base::BindOnce(&QCefV8BindRenderDelegate::onPendingcallResp, this, rsp, frameId));
        return;
    }
    if (!m_frameHandlers.contains(frameId)) {
        return;
    }
    CefRefPtr<QCefV8Handler> v8Handler = m_frameHandlers.value(frameId);
    CefRefPtr<CefV8Context> context = v8Handler->getFrame()->GetV8Context();
    v8Handler->onPendingcallResp(rsp, context);
}
