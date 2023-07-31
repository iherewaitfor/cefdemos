#include "qcefobjectmgr.h"
#include "qcefv8bindapp_p.h"
#include "qcefv8bindapp.h"
#include "qcefv8objecthelper.h"
#include "qcefipcprotocol.h"


QCefObjectMgr::QCefObjectMgr(QObject* parent)
: QObject(parent)
{}

QCefObjectMgr::~QCefObjectMgr()
{}

void QCefObjectMgr::insertRender(quint32 objectId, QObject* obj)
{
	if (!m_renderQObject.contains(objectId))
	{
		QObject::connect(obj, SIGNAL(destroyed(QObject *)), this, SLOT(on_render_destroyed(QObject *)), Qt::DirectConnection);
		
		m_renderQObject.insert(objectId, obj);
	}	
}

void QCefObjectMgr::insertSharePtrObj(quint32 objectId, QSharedPointer<QObject> obj)
{
	if (!m_sharePtrQObject.contains(objectId))
	{
		QObject::connect(obj.data(), SIGNAL(destroyed(QObject *)), this, SLOT(on_shareObj_destroyed(QObject *)), Qt::DirectConnection);
		m_sharePtrQObject.insert(objectId, obj);
	}		
}

void QCefObjectMgr::removeSharePtrObj(quint32 objectId)
{
	m_sharePtrQObject.remove(objectId);
}

QObject* QCefObjectMgr::findBrowserObject(quint32 objectId)
{
	return m_browserQObjects[objectId];
}

QObject* QCefObjectMgr::findRenderObject(quint32 objectId)
{
	return m_renderQObject[objectId];
}

QSharedPointer<QObject> QCefObjectMgr::shareObj(quint32 objectId)
{
	return m_sharePtrQObject[objectId];
}

void QCefObjectMgr::removeBrowerObj(quint32 objectId)
{
	m_sharePtrQObject.remove(objectId);
}

void QCefObjectMgr::insertBrowser(quint32 objectId, QObject* obj)
{
	if (!m_browserQObjects.contains(objectId))
	{
		QObject::connect(obj, SIGNAL(destroyed(QObject *)), this, SLOT(on_browser_destroyed(QObject *)), Qt::DirectConnection);
		m_browserQObjects.insert(objectId, obj);
	}
}

void QCefObjectMgr::on_browser_destroyed(QObject *obj)
{
	if (obj != NULL)
	{
		quint32 uid = obj->property(KObjectId).toUInt();
		m_browserQObjects.remove(uid);
	}
}

void QCefObjectMgr::on_render_destroyed(QObject *obj)
{
	if (obj != NULL)
	{
		quint32 uid = obj->property(KObjectId).toUInt();
		quint64 frameId = obj->property(KBrowserFrameId).toULongLong();
		m_renderQObject.remove(uid);

		emit renderObjDestroyed(uid, frameId);
	}
}

void QCefObjectMgr::on_shareObj_destroyed(QObject *obj)
{
	if (obj != NULL)
	{
		quint32 uid = obj->property(KObjectId).toUInt();
		m_renderQObject.remove(uid);
	}
}

