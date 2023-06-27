#ifndef QCEFCOREAPP_H
#define QCEFCOREAPP_H
#pragma once
#include "qcefbrowserglobal.h"
#include <QtCore>
#include "qcefcoreapp_p.h"
#include <QObject>
class QCefBrowser;
class QCefCoreAppPrivate;
class QCEFBROWSER_DECL_EXPORT QCefCoreApp:public QObject {
	Q_OBJECT;
public:
	QCEFBROWSER_DECLARE_PRIVATE(QCefCoreApp);
	QCefCoreApp();
	virtual ~QCefCoreApp();
	static QCefCoreApp* getInstance();
	QPointer<QCefBrowser> createBrowser(const QString url);
	void setApiRoot(QPointer<QObject> qApiRootObject);
	QPointer<QObject> getApitRootObject();
signals:
	void allClosed();
};

#endif

