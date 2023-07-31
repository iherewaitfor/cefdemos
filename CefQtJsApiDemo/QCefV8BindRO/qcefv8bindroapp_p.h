﻿#pragma once
#include "qcefv8bindroglobal.h"
#include "qcefv8bindbrowserdelegate.h"
#include "render/qcefv8bindrenderdelegate.h"
#include "qcefobjectmgr.h"
#include <QtRemoteObjects>
#include "remoteobjecttreehelper.h"
#include "render/dynamicclienttreehelper.h"
class QCefObjectMgr;
class QCefV8BindAppRO;
class QCefV8BindBrowserDelegate;
class QCefV8BindAppROPrivate
{
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

private:
	void _init();

private:
	QPointer<QObject>	m_v8RootObject;
	CefRefPtr<QCefV8BindBrowserDelegate>	m_browerDelegate;
	CefRefPtr<QCefV8BindRenderDelegate>	m_renderDelegate;
	QPointer<QCefObjectMgr>	m_objectMgr;
	QMap<int, QPointer<QObject>>	m_windowObjects;

	//source
	QScopedPointer<QRemoteObjectRegistryHost> m_pQRemoteObjectRegistryHost;
	QScopedPointer<QRemoteObjectHost> m_pQRemoteObjectHost;
	RemoteObjectTreeHelper m_remoteObjectTreeHelper;

	//replica
	QScopedPointer<QRemoteObjectNode> m_pQRemoteObjectNode;
	QScopedPointer<DynamicClientTreeHelper> m_pDynamicClientTreeHelper;

};
