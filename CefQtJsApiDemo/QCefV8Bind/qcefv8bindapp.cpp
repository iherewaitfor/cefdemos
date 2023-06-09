﻿#include "qcefv8bindapp.h"
#include "include/cef_command_line.h"

#include "../QtCefBrowser/qcefcoreapp.h"
#include "qcefv8objecthelper.h"
#include "qcefv8bindapp_p.h"
#include "qcefv8bindbrowserdelegate.h"

void QCefV8BindAppPrivate::_init()
{
	CefRefPtr<CefCommandLine>  commandLine = CefCommandLine::CreateCommandLine();
	commandLine->InitFromString(::GetCommandLine());

	if (!commandLine->HasSwitch("type"))
	{
		m_browerDelegate = new QCefV8BindBrowserDelegate();
		QCefCoreApp::getInstance()->regBrowserDelegate(m_browerDelegate.get());
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
QCefV8BindApp::QCefV8BindApp()
: d_ptr( new QCefV8BindAppPrivate())
{
}

QCefV8BindApp::~QCefV8BindApp(void)
{
	if (d_ptr)
	{
		delete d_ptr;
		d_ptr = NULL;
	}
}

void QCefV8BindApp::setV8RootObject(QPointer<QObject> o)
{
	o->setProperty(KV8ObjectName, o->objectName());
	d_func()->m_v8RootObject = o;
}
QCefV8BindApp* QCefV8BindApp::getInstance()
{
	static QCefV8BindApp s_v8BindApp;
	return &s_v8BindApp;
}

void QCefV8BindApp::addWindowObject(int cefBrowserId, QObject* o)
{
	d_func()->m_windowObjects.insert(cefBrowserId, o);
}

void QCefV8BindApp::removeWindowObject(int cefBrowserId)
{
	d_func()->m_windowObjects.remove(cefBrowserId);
}

QObject* QCefV8BindApp::getWindowObject(int cefBrowserId)
{
	if (d_func()->m_windowObjects.contains(cefBrowserId))
	{
		return d_func()->m_windowObjects[cefBrowserId];
	}
	return NULL;
}

