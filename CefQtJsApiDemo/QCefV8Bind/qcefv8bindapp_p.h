#pragma once
#include "qcefv8bindglobal.h"
#include "qcefv8bindbrowserdelegate.h"
#include "render/qcefv8bindrenderdelegate.h"
#include "qcefobjectmgr.h"

class QCefObjectMgr;
class QCefV8BindApp;
class QCefV8BindBrowserDelegate;
class QCefV8BindAppPrivate
{
	QCEFV8BIND_DECLARE_PUBLIC(QCefV8BindApp);
public:
	QCefV8BindAppPrivate():m_objectMgr(new QCefObjectMgr(nullptr))
	{
		_init();
	}

	virtual ~QCefV8BindAppPrivate()
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
};
