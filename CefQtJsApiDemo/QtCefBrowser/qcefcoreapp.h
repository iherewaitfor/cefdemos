#ifndef QCEFCOREAPP_H
#define QCEFCOREAPP_H
#pragma once
#include "qcefbrowserglobal.h"
#include <QtCore>
#include "qcefcoreapp_p.h"
#include <QObject>
#include "qcefbrowserhandlerdelegate.h"
#include "render/qcefrenderhandlerdelegate.h"
#include "browser/browser_struct.h"
#include "window.h"
class QCefBrowser;
class QCefCoreAppPrivate;
class Window;
class QCEFBROWSER_DECL_EXPORT QCefCoreApp:public QObject {
	Q_OBJECT;
public:
	QCEFBROWSER_DECLARE_PRIVATE(QCefCoreApp);
	QCefCoreApp();
	virtual ~QCefCoreApp();
	static QCefCoreApp* getInstance();
	QSharedPointer<QCefBrowser> createBrowser(const BrowserWindowOptions& option);
	void setApiRoot(QPointer<QObject> qApiRootObject);
	QPointer<QObject> getApitRootObject();
	void regBrowserDelegate(client::BrowserDelegate*);
	void unregBrowserDelegate(client::BrowserDelegate*);
	void regRenderDelegate(client::RenderDelegate*);
	void unregRenderDelegate(client::RenderDelegate*);
	CefApi::Window* getApiWindow(int cefBrowserId);

signals:
	void browserCreated(int cefBrowserId);
	void browserClosed(int cefBrowserId);
	void allClosed();
};

#endif

