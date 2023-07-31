#pragma once

#include <QtCore>

class QCefObjectMgr : public QObject
{
	Q_OBJECT;

public:
	QCefObjectMgr(QObject* parent);
	virtual ~QCefObjectMgr();

	void insertBrowser(quint32 objectId, QObject* obj);
	void insertRender(quint32 objectId, QObject* obj);

	void insertSharePtrObj(quint32 objectId, QSharedPointer<QObject> obj);
	void removeSharePtrObj(quint32 objectId);

	QObject* findBrowserObject(quint32 objectId);
	QObject* findRenderObject(quint32 objectId);
	QSharedPointer<QObject> shareObj(quint32 objectId);

	void removeBrowerObj(quint32 objectId);

signals:
	void renderObjDestroyed(int objectId, quint64 frameId);

private slots:
	void on_browser_destroyed(QObject *);
	void on_render_destroyed(QObject *);
	void on_shareObj_destroyed(QObject *);
	
private:
	QMap<quint32, QPointer<QObject>>	m_browserQObjects;
	QMap<quint32, QPointer<QObject>>	m_renderQObject;
	QMap<quint32, QSharedPointer<QObject>>	m_sharePtrQObject;
};