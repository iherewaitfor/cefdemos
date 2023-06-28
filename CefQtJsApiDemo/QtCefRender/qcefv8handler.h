#pragma once
#include "include/cef_v8.h"
#include "qclientrenderdelegate.h"
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

private:
	//CefV8Handler
	virtual bool Execute(const CefString& name,
		CefRefPtr<CefV8Value> v8Object,
		const CefV8ValueList& arguments,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception);

private:
	IMPLEMENT_REFCOUNTING(QCefV8Handler);

private:
    CefRefPtr<CefFrame> m_frame;
};


