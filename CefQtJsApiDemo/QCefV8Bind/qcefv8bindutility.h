#pragma once
#include "include/cef_v8.h"

template<typename T>
class QCefV8ObjectHolder : public CefBaseRefCounted
{
public:
	QCefV8ObjectHolder(T wrapObject)
		: m_wrapObject(wrapObject)
	{
	}

	virtual ~QCefV8ObjectHolder()
	{
	}

	T wrapObject() const
	{
		return m_wrapObject;
	}

private:
	T m_wrapObject;
	IMPLEMENT_REFCOUNTING(QCefV8ObjectHolder);
};



class V8ContextCaller
{
public:
	V8ContextCaller(CefRefPtr<CefV8Context> context);
	virtual ~V8ContextCaller();

private:
	CefRefPtr<CefV8Context> m_context;
};