#pragma once
#include "qcefv8bindglobal.h"
#include "qcefv8bindbrowserdelegate.h"

class QCefV8BindApp;
class QCefV8BindBrowserDelegate;
class QCefV8BindAppPrivate
{
	QCEFV8BIND_DECLARE_PUBLIC(QCefV8BindApp);
public:
	QCefV8BindAppPrivate()
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
private:
	void _init();

private:
	QPointer<QObject>	m_v8RootObject;
	CefRefPtr<QCefV8BindBrowserDelegate>	m_browerDelegate;
};
