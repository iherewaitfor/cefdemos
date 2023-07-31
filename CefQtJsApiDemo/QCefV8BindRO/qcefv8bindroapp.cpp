﻿#include "qcefv8bindroapp.h"
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
	}
	else
	{
		const std::string& process_type = commandLine->GetSwitchValue("type");
		if (process_type == "renderer")
		{
			// to do: create and register the delegate of render process
			m_renderDelegate = new QCefV8BindRenderDelegate();
			QCefCoreApp::getInstance()->regRenderDelegate(m_renderDelegate.get());
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
}
QCefV8BindAppRO* QCefV8BindAppRO::getInstance()
{
	static QCefV8BindAppRO s_v8BindApp;
	return &s_v8BindApp;
}

