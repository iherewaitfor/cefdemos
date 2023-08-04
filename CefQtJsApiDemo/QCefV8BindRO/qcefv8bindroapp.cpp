#include "qcefv8bindroapp.h"
#include "include/cef_command_line.h"

#include "../QtCefBrowser/qcefcoreapp.h"
#include "qcefv8objecthelper.h"
#include "qcefv8bindroapp_p.h"
#include "qcefv8bindbrowserdelegate.h"

void QCefV8BindAppROPrivate::_init()
{
	CefRefPtr<CefCommandLine>  commandLine = CefCommandLine::CreateCommandLine();
	commandLine->InitFromString(::GetCommandLine());

	if (!commandLine->HasSwitch("type"))
	{
		m_browerDelegate = new QCefV8BindBrowserDelegate();
		QCefCoreApp::getInstance()->regBrowserDelegate(m_browerDelegate.get());
		// to do init QRemoteObjectRegistryHost and QRemoteObjectHost
		m_pQRemoteObjectRegistryHost.reset(new QRemoteObjectRegistryHost(QUrl(QStringLiteral("local:QCefRegistry")))); // create node that hosts registy
		m_pQRemoteObjectHost.reset(new QRemoteObjectHost(QUrl(QStringLiteral("local:QCefReplica")), QUrl(QStringLiteral("local:QCefRegistry")))); // create node that will host source and connect to registry
		
		QObject::connect(QCefCoreApp::getInstance(), SIGNAL(cefApiWindowCreated(int)), this,
			SLOT(cefApiWindowCreated_slot(int)));

	}
	else
	{
		const std::string& process_type = commandLine->GetSwitchValue("type");
		if (process_type == "renderer")
		{
			qRegisterMetaType<cefv8bind_protcool::PendingcallReq>("cefv8bind_protcool::PendingcallReq");
			qRegisterMetaType<cefv8bind_protcool::ConnectReplicaSignal>("cefv8bind_protcool::ConnectReplicaSignal");
			// to do: create and register the delegate of render process
			m_renderDelegate = new QCefV8BindRenderDelegate();
			QCefCoreApp::getInstance()->regRenderDelegate(m_renderDelegate.get());
			m_pQRemoteObjectNode.reset(new QRemoteObjectNode(QUrl(QStringLiteral("local:QCefRegistry"))));

			QSharedPointer<QRemoteObjectDynamicReplica> pRep; // shared pointer to hold replica

			pRep.reset(m_pQRemoteObjectNode->acquireDynamic("QCefRemoteObjectTreeHelper")); // acquire replica of source from host node
			pRep->setObjectName("QCefRemoteObjectTreeHelper");
			m_pDynamicClientTreeHelper.reset(new DynamicClientTreeHelper(pRep));

			QObject::connect(m_pDynamicClientTreeHelper.data(), SIGNAL(contextCreated(int, qint64)), m_pDynamicClientTreeHelper.data(), 
				SLOT(contextCreated_slot(int, qint64)));
			QObject::connect(this, SIGNAL(callReplicaMethod(cefv8bind_protcool::PendingcallReq)), this,
				SLOT(callReplicaMethod_slot(cefv8bind_protcool::PendingcallReq)));
			QObject::connect(this, SIGNAL(connectReplicaSignal(cefv8bind_protcool::ConnectReplicaSignal)), this,
				SLOT(connectReplicaSignal_slot(cefv8bind_protcool::ConnectReplicaSignal)));
		}
	}
}
//////////////////////////////////////////////////////////////////////////
QCefV8BindAppRO::QCefV8BindAppRO()
: d_ptr( new QCefV8BindAppROPrivate())
{
}

QCefV8BindAppRO::~QCefV8BindAppRO(void)
{
	if (d_ptr)
	{
		delete d_ptr;
		d_ptr = NULL;
	}
}

void QCefV8BindAppRO::setV8RootObject(QPointer<QObject> o)
{
	o->setProperty(KV8ObjectName, o->objectName());
	d_func()->m_v8RootObject = o;
	d_func()->m_remoteObjectTreeHelper.setRootObject(o);
	d_func()->m_remoteObjectTreeHelper.enableObjectsRemoting(d_func()->m_pQRemoteObjectHost.data());
}
QCefV8BindAppRO* QCefV8BindAppRO::getInstance()
{
	static QCefV8BindAppRO s_v8BindApp;
	return &s_v8BindApp;
}

void QCefV8BindAppRO::enableRemoting(QObject* apiRemotObject) {
	d_func()->m_pQRemoteObjectHost->enableRemoting(apiRemotObject);
}
