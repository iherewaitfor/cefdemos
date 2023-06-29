#ifndef QCEFV8BINDRENDERDELEGATE_H
#define QCEFV8BINDRENDERDELEGATE_H
#include "qcefv8handler.h"
#include <QtCore>
#include "../../QtCefBrowser/render/qcefrenderhandlerdelegate.h"
class QCefV8Handler;
class QCefV8BindRenderDelegate : public virtual CefBaseRefCounted,public client::RenderDelegate {
public:
    QCefV8BindRenderDelegate() {}
	virtual ~QCefV8BindRenderDelegate(){}

	virtual void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) {};

	virtual void OnWebKitInitialized() {}

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

	virtual bool OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message);

private:

    QMap<QString, CefRefPtr<QCefV8Handler>> m_frameHandlers;

    DISALLOW_COPY_AND_ASSIGN(QCefV8BindRenderDelegate);
    IMPLEMENT_REFCOUNTING(QCefV8BindRenderDelegate);
};
#endif