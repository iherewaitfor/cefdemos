#ifndef QCEFV8BINDRENDERDELEGATE_H
#define QCEFV8BINDRENDERDELEGATE_H
#include "qcefv8handler.h"
#include <QtCore>
#include "../../QtCefBrowser/render/qcefrenderhandlerdelegate.h"
#include "../qcefipcvalue.h"
//for CefPostTask
#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "include/base/cef_callback.h"

#include <list>
class QCefV8Handler;
class QCefV8BindRenderDelegate : public virtual CefBaseRefCounted,public client::RenderDelegate {
public:
    QCefV8BindRenderDelegate() {}
	virtual ~QCefV8BindRenderDelegate(){}

	virtual void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) {};

	virtual void OnWebKitInitialized();

	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDictionaryValue> extra_info) {}

	virtual void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) {}


	virtual void OnContextCreated(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context);

	virtual void OnContextReleased(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context);
	
public:
	void tobindV8Objects(QList<cefv8bind_protcool::CefMetaObject> cefMetaObjects, int64 frameId);
	void onPendingcallResp(cefv8bind_protcool::PendingcallResp rsp, int64 frameId);
	void dispatchReplicaSignaToJs(cefv8bind_protcool::DispatchReplicaSignaToJs rsp);
private:

    QMap<int64, CefRefPtr<QCefV8Handler>> m_frameHandlers;

    DISALLOW_COPY_AND_ASSIGN(QCefV8BindRenderDelegate);
    IMPLEMENT_REFCOUNTING(QCefV8BindRenderDelegate);
};
#endif