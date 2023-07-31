#pragma once
#include "QtCefBrowser/qcefbrowserglobal.h"
#include "qcefv8bindroglobal.h"
#include "include/cef_base.h"
#include <QtCore>

#include "QtCefBrowser/qcefbrowserhandlerdelegate.h"

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
	IMPLEMENT_REFCOUNTING(QCefV8BindBrowserDelegate);
};