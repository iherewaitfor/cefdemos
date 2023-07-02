#pragma once
#include "../QtCefBrowser/qcefbrowserglobal.h"
#include "qcefv8bindglobal.h"
#include "include/cef_base.h"
#include <QtCore>

#include "../QtCefBrowser/qcefbrowserhandlerdelegate.h"

class QCefV8BindBrowserDelegate : public QObject,  public virtual CefBaseRefCounted
	, public client::BrowserDelegate
{
	Q_OBJECT
public:
	QCefV8BindBrowserDelegate();
	~QCefV8BindBrowserDelegate();
	//client::BrowserDelegate Methods
	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser);
	virtual void OnBrowserClosing(CefRefPtr<CefBrowser> browser);
	virtual void OnBrowserClosed(CefRefPtr<CefBrowser> browser);
	virtual bool OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message);
signals:
	void processMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message);
private slots:
	void OnProcessMessageReceivedSlot(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message);
private:
	base::Lock lock;
	QSet<int>	m_subscribeBrowsers;


	IMPLEMENT_REFCOUNTING(QCefV8BindBrowserDelegate);
};