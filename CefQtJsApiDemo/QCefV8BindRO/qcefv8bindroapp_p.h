﻿#pragma once
#include "qcefv8bindroglobal.h"
#include "qcefv8bindbrowserdelegate.h"
#include "render/qcefv8bindrenderdelegate.h"
#include "qcefobjectmgr.h"
#include <QtRemoteObjects>
#include "remoteobjecttreehelper.h"
#include "render/dynamicclienttreehelper.h"
#include "render/qcefv8bindrenderdelegate.h"
#include "qcefipcvalue.h"
#include "metainvoker.h"
#include <QtCore>
#include "autosignalsemitter.h"
class QCefObjectMgr;
class QCefV8BindAppRO;
class QCefV8BindBrowserDelegate;
class DynamicClientTreeHelper;
class QCefV8BindAppROPrivate :public QObject
{
	Q_OBJECT
	QCEFV8BINDRO_DECLARE_PUBLIC(QCefV8BindAppRO);
public:
	QCefV8BindAppROPrivate():m_objectMgr(new QCefObjectMgr(nullptr))
	{
		_init();
	}

	virtual ~QCefV8BindAppROPrivate()
	{
	}

	QObject* getRootObject() const
	{
		return m_v8RootObject;
	}
	QPointer<QCefObjectMgr> getObjectMgr()
	{
		return m_objectMgr;
	}
	CefRefPtr<QCefV8BindBrowserDelegate> getBrowserDelegate() {
		return m_browerDelegate;
	}
	CefRefPtr<QCefV8BindRenderDelegate> getRenderDelegate() {
		return m_renderDelegate;
	}
	DynamicClientTreeHelper * getReplicaTreeHelper(){
		return m_pDynamicClientTreeHelper.data();
	}
public Q_SLOTS:
	void callReplicaMethod_slot(cefv8bind_protcool::PendingcallReq req);
	void pendingCallResult(QRemoteObjectPendingCallWatcher* call);
	void connectReplicaSignal_slot(cefv8bind_protcool::ConnectReplicaSignal);
Q_SIGNALS:
	void callReplicaMethod(cefv8bind_protcool::PendingcallReq req);
	void connectReplicaSignal(cefv8bind_protcool::ConnectReplicaSignal);
private:
	void _init();

private:
	QPointer<QObject>	m_v8RootObject;
	CefRefPtr<QCefV8BindBrowserDelegate>	m_browerDelegate;
	CefRefPtr<QCefV8BindRenderDelegate>	m_renderDelegate;
	QPointer<QCefObjectMgr>	m_objectMgr;
	QMap<int, QPointer<QObject>>	m_windowObjects;

	//source in browser process
	QScopedPointer<QRemoteObjectRegistryHost> m_pQRemoteObjectRegistryHost;
	QScopedPointer<QRemoteObjectHost> m_pQRemoteObjectHost;
	RemoteObjectTreeHelper m_remoteObjectTreeHelper;

	//replica in render process
	QScopedPointer<QRemoteObjectNode> m_pQRemoteObjectNode;
	QScopedPointer<DynamicClientTreeHelper> m_pDynamicClientTreeHelper;

	QMap <QString, AutoSignalsEmitter*> m_signalMap;

};
