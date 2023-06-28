#pragma once
#include "qcefv8bindglobal.h"

class QCefV8BindApp;

class QCefV8BindAppPrivate
{
	QCEFV8BIND_DECLARE_PUBLIC(QCefV8BindApp);
public:
	QCefV8BindAppPrivate()
	{
	}

	virtual ~QCefV8BindAppPrivate()
	{
	}

	QObject* getRootObject() const
	{
		return m_v8RootObject;
	}

private:
	QPointer<QObject>	m_v8RootObject;
};
