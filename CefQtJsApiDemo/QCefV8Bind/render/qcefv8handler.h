#pragma once
#include "include/cef_v8.h"
#include "qcefv8bindrenderdelegate.h"
#include "../qcefipcprotocol.h"
#include "asyncmethodcallback.h"
class QClientRenderDelegate;
class QCefV8Handler : public CefV8Handler
{
public:
	friend QClientRenderDelegate;

	QCefV8Handler(CefRefPtr<CefFrame> frame);
	virtual ~QCefV8Handler();
	void clear();

	CefRefPtr<CefFrame>	getFrame();
	

public:
    void on_cefMetaDatasResponse(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefProcessMessage> message
    );

	void onInvokeResponse(CefRefPtr<CefProcessMessage> message, CefRefPtr<CefV8Context> context);

private:
	//CefV8Handler
	virtual bool Execute(const CefString& name,
		CefRefPtr<CefV8Value> v8Object,
		const CefV8ValueList& arguments,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception);
private:
	CefRefPtr<CefListValue> toProcessMessage(const CefV8ValueList& source);
	void sendIPCMessage(CefRefPtr<CefV8Context> context,
		cefv8bind_protcool::PackageBase& ipc_protocol);

private:
    CefRefPtr<CefFrame> m_frame;
	QPointer<AsyncCefMethodCallbackMgr> m_asynCallbackMgr;

	IMPLEMENT_REFCOUNTING(QCefV8Handler);
};


