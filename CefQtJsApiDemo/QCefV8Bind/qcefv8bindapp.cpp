#include "qcefv8bindapp.h"
#include "include/cef_command_line.h"

#include "../QtCefBrowser/qcefcoreapp.h"
#include "qcefv8objecthelper.h"
#include "qcefv8bindapp_p.h"

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

void QCefV8BindApp::setV8RootObject(QObject* o)
{
	o->setProperty(KV8ObjectName, o->objectName());
	d_func()->m_v8RootObject = o;
}
QCefV8BindApp* QCefV8BindApp::getInstance()
{
	static QCefV8BindApp s_v8BindApp;
	return &s_v8BindApp;
}
